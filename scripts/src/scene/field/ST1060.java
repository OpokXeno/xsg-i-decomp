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
import xeno.map.MC_UTK06_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1060
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK06_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    Uwamono doorA;
    Uwamono door_0;
    Uwamono door_1;
    Uwamono door_2;
    Uwamono door_3;
    Uwamono K1;
    Uwamono IA;
    Uwamono item1;
    Uwamono item3;
    Uwamono item5;
    Uwamono item6;
    Enepc AGWS1;
    Enepc AGWS2;
    Enepc AGWS3;
    Enepc AGWS4;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    boolean Final_Check = true;
    boolean HEISHI_CODE = false;
    boolean EnterCheck = false;
    Effect Obj600_A;
    Effect Obj600_B;
    Effect Obj601_A;
    Effect Obj601_B;
    Effect Obj602;
    Effect Obj603;
    Effect Obj604;
    Effect Obj605;
    Effect Obj606;
    Effect Obj607;
    Effect Obj608;
    Effect Obj609;
    Effect Obj610;
    Effect Obj611;
    Effect fade;
    Effect Bikkuri;
    Effect Bang;
    Unit monitor;
    int talkFlag = 0;
    Light light = new Light(0);
    Uwamono teiten1;
    int NO3_CARDKEY = Runtime.checkItem(10, 3);
    int NO5 = Runtime.getFlags(6011, 1);
    int NO5_CONSOLE = Runtime.getFlags(6022, 1);
    int HEISHI_AGWS = Runtime.getFlags(6023, 1);
    int HEISHI_LIFE1 = Runtime.getFlags(6024, 1);
    int HEISHI_LIFE2 = Runtime.getFlags(6051, 1);
    int HEISHI_LIFE3 = Runtime.getFlags(6052, 1);
    String[] Q_NO5_CONSOLE = new String[]{"Operate Door No. 5 switch?", "/[waitkey(64)]/[close()]"};
    String[] A_NO5 = new String[]{"/[label(Jr.)]", "Door No. 5. Damn, it's locked.\n", "/[waitkey(64)]/[close()]"};
    String[] WARNING_1 = new String[]{"/[label(Jr.)]", "It says,", "/[waitkey(1)]/[clear()]", "'Emergency!\n", "Emergency!\n", "Prepare for close-quarter combat.'", "/[waitkey(64)]/[close()]"};
    String[] WARNING_2 = new String[]{"/[label(Jr.)]", "It says", "/[waitkey(1)]/[clear()]", "'Emergency!\n", "Emergency!\n", "Prepare for close-quarter combat.'", "/[waitkey(1)]/[clear()]", "Well, no need to read any further.", "/[waitkey(64)]/[close()]"};
    String[] WARNING_3 = new String[]{"/[label(Jr.)]", "They've sounded the emergency alarm.", "/[waitkey(64)]/[close()]"};

    ST1060() {
    }

    void DefaultLight() {
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
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

    void DefaultTalk(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
    }

    void EOB(int n) {
        System.println("EOB *********************");
        switch (n) {
            case 1: {
                System.println("HEISHI1_LIFE = END");
                Runtime.setFlags(6024, 1, 1);
                this.HEISHI_LIFE1 = Runtime.getFlags(6024, 1);
                break;
            }
            case 2: {
                System.println("HEISHI2_LIFE = END");
                Runtime.setFlags(6051, 1, 1);
                this.HEISHI_LIFE2 = Runtime.getFlags(6051, 1);
                break;
            }
            case 3: {
                System.println("HEISHI3_LIFE = END");
                Runtime.setFlags(6052, 1, 1);
                this.HEISHI_LIFE3 = Runtime.getFlags(6052, 1);
                break;
            }
        }
    }

    void EV_Camera() {
        System.println("NO5 コンソール前");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.882831f, 3.0238116f, -28.571447f);
        this.camEV.setRotate(-28.69892f, -83.316505f, 0.0f);
        this.camEV.setFov(25.918947f);
        this.camEV.change();
    }

    void EV_Camera01() {
        System.println("爆発カメラ");
        float[] fArray = new float[]{1.0f, 13.619641f, 1.4613715f, 6.592691f, 2.0f, 13.669641f, 1.5113716f, 6.592691f, 3.0f, 13.619641f, 1.4613715f, 6.592691f, 4.0f, 13.719641f, 1.4113716f, 6.592691f, 5.0f, 13.619641f, 1.4613715f, 6.592691f, 6.0f, 13.669641f, 1.5113716f, 6.592691f, 7.0f, 13.619641f, 1.4613715f, 6.592691f, 8.0f, 13.719641f, 1.4113716f, 6.592691f, 9.0f, 13.619641f, 1.4613715f, 6.592691f, 10.0f, 13.669641f, 1.5113716f, 6.592691f, 11.0f, 13.619641f, 1.4613715f, 6.592691f, 12.0f, 13.719641f, 1.4113716f, 6.592691f, 13.0f, 13.619641f, 1.4613715f, 6.592691f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -10.955497f;
        fArray2[2] = 382.27686f;
        fArray2[4] = 13.0f;
        fArray2[5] = -10.955497f;
        fArray2[6] = 382.27686f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 13);
        this.camEV.rotateSPL(fArray3, 1, 2, 13);
        this.camEV.setFov(28.279972f);
        this.camEV.change();
    }

    void EV_Camera01_F() {
        System.println("イベントカメラフォロー");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.644905f, -0.1592295f, -3.9143114f);
        this.camEV.setRotate(-11.703484f, 428.60132f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera2() {
        System.println("NO5 Openカメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.243235f, 2.4478216f, -24.16162f);
        this.camEV.setRotate(-9.31967f, -26.878132f, 0.0f);
        this.camEV.setFov(29.43886f);
        this.camEV.change();
    }

    void EV_Camera3() {
        System.println("NO5 扉前");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.1604624f, 2.2959938f, -26.167828f);
        this.camEV.setRotate(-7.920214f, 50.39975f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
        System.println(" FINAL_INIT !!!!!");
        switch (n) {
            case 11: {
                System.println("id = 11");
                this.AGWS1.kickEnepc(4, 1);
                this.AGWS1.kickEnepc(10, 150, 0);
                break;
            }
            case 12: {
                System.println("id = 12");
                this.AGWS2.kickEnepc(4, 1);
                this.AGWS2.kickEnepc(10, 150, 0);
                break;
            }
            case 13: {
                System.println("id = 13");
                this.AGWS3.kickEnepc(4, 1);
                this.AGWS3.kickEnepc(10, 150, 0);
                break;
            }
            case 14: {
                System.println("id = 14");
                this.AGWS4.kickEnepc(4, 1);
                this.AGWS4.kickEnepc(10, 150, 0);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        switch (n) {
            case 100: {
                if (n2 == 0) {
                    Stage.setVisible(9, true);
                    break;
                }
                if (n2 == 1) {
                    Stage.setVisible(9, false);
                    break;
                }
                if (n2 == 2 && !this.EnterCheck && this.NO5 == 0) {
                    if (this.NO3_CARDKEY == 1) {
                        this.enemy4.kickEnepc(4, 2);
                    }
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    System.sleep(2);
                    Runtime.enable(65536);
                    this.EV_Camera3();
                    this.player.mtn(11, 1, 1.0f, true);
                    this.DefaultTalk(this.A_NO5);
                    this.cam0.setMode(0);
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    if (this.NO3_CARDKEY != 1) break;
                    this.enemy4.kickEnepc(4, 0);
                    break;
                }
                if (n2 == 3 && this.NO5_CONSOLE == 0 && !this.EnterCheck) {
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Stage.setVisible(21, false);
                    this.EV_Camera();
                    this.light.setColor(0, 0.4f, 0.4f, 0.4f);
                    this.light.setColor(1, 0.35f, 0.35f, 0.35f);
                    this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.light.setColor(2, 0.3f, 0.3f, 0.9f);
                    this.light.setDirection2(2, 0.0f, 1.5f, -1.0f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.light.setColor(3, 0.5f, 0.3f, 0.3f);
                    this.light.setDirection2(3, 0.0f, -0.25f, -1.0f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    Runtime.enable(65536);
                    if (this.DefaultMenu(this.Q_NO5_CONSOLE) == 0) {
                        this.player.getTranslate();
                        if (this.player.px >= -4.735f) {
                            System.println("Pattern A");
                            this.player.setRotateY(210.0f);
                        } else if (this.player.px >= -4.9f && this.player.px < -4.735f) {
                            System.println("パターンB");
                            this.player.setRotateY(195.0f);
                        } else if (this.player.px >= -5.1f && this.player.px < -4.9f) {
                            System.println("パターンC");
                            this.player.setRotateY(180.0f);
                        } else if (this.player.px >= -5.2f && this.player.px < -5.1f) {
                            System.println("パターンD");
                            this.player.setRotateY(170.0f);
                        } else if (this.player.px < -5.2f) {
                            System.println("パターンE");
                            this.player.setRotateY(160.0f);
                        }
                        this.player.mtn(25, 1, 1.0f, true);
                        int n3 = 0;
                        while (n3 < 50) {
                            if (n3 == 30) {
                                this.Obj600_A.disp(false);
                                this.Obj600_B.disp(true);
                                this.Obj601_A.disp(false);
                                this.Obj601_B.disp(true);
                                Sound.effectPlay(196741);
                            }
                            System.sleep(1);
                            ++n3;
                        }
                        this.EV_Camera2();
                        this.doorA.DoorOpen();
                        Sound.effectPlay(196744);
                        System.sleep(90);
                        Runtime.setFlags(6011, 1, 1);
                        this.NO5 = Runtime.getFlags(6011, 1);
                        Runtime.setFlags(6022, 1, 1);
                        this.NO5_CONSOLE = Runtime.getFlags(6022, 1);
                    }
                    Stage.setVisible(21, true);
                    this.cam0.setMode(0);
                    this.DefaultLight();
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 6) {
                    if (this.NO3_CARDKEY == 1) {
                        Runtime.enable(262144);
                    }
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    System.sleep(2);
                    Runtime.enable(65536);
                    this.player.mtn(11, 1, 1.0f, true);
                    if (this.talkFlag < 3) {
                        this.DefaultTalk(this.WARNING_1);
                        ++this.talkFlag;
                    } else if (this.talkFlag < 5) {
                        this.DefaultTalk(this.WARNING_2);
                        ++this.talkFlag;
                    } else {
                        this.DefaultTalk(this.WARNING_3);
                    }
                    this.cam0.setMode(0);
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    if (this.NO3_CARDKEY != 1) break;
                    Runtime.disable(262144);
                    break;
                }
                return;
            }
        }
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(6023, 1, 1);
                this.HEISHI_AGWS = Runtime.getFlags(6023, 1);
                Runtime.disable(524288);
                this.enemy1.setVisible(true);
                this.enemy1.kickEnepc(0, 1);
                this.enemy1.moveEnepc(15, 0.0f, -14.5f, 55);
                if (this.NO3_CARDKEY == 1) {
                    this.enemy4.kickEnepc(4, 1);
                    this.enemy4.setVisible(false);
                }
                int n2 = 0;
                while (n2 <= 140) {
                    if (n2 == 0) {
                        this.EV_Camera01();
                        this.Bang.disp(true);
                        Sound.effectPlay(196743);
                    } else if (n2 == 20) {
                        this.Bikkuri.disp(true);
                        this.enemy1.kickEnepc(4, 0);
                        this.enemy1.kickEnepc(4, 2);
                    } else if (n2 == 40) {
                        this.Bikkuri.disp(false);
                        this.enemy1.kickEnepc(4, 0);
                        this.enemy1.kickEnepc(4, 1);
                        this.enemy1.kickEnepc(0, 3);
                        this.enemy1.moveEnepc(15, 0.0f, -14.5f, 20);
                    } else if (n2 == 50) {
                        this.enemy1.kickEnepc(9, 100);
                    } else if (n2 == 70) {
                        this.enemy1.kickEnepc(9, -1);
                        this.enemy1.kickEnepc(4, 0);
                        this.enemy1.kickEnepc(4, 2);
                    } else if (n2 == 75) {
                        this.enemy1.kickEnepc(4, 0);
                        this.enemy1.kickEnepc(4, 1);
                        this.enemy1.moveEnepc(17, 270.0f, -1.0f, 5);
                    } else if (n2 == 81) {
                        this.enemy1.kickEnepc(0, 3);
                        this.enemy1.moveEnepc(15, -15.0f, -16.0f, 60);
                    } else if (n2 == 130) {
                        if (this.NO3_CARDKEY == 1) {
                            this.enemy4.kickEnepc(4, 0);
                            this.enemy4.setVisible(true);
                        }
                        this.cam0.setMode(0);
                    } else if (n2 == 140) {
                        this.enemy1.moveEnepc(17, 90.0f, -1.0f, 15);
                    }
                    System.sleep(1);
                    ++n2;
                }
                this.enemy1.setInvalidID(0);
                this.enemy1.setRotate(0.0f, 90.0f, 0.0f);
                this.enemy1.setTranslate(-13.9f, -3.0f, -12.5f);
                this.enemy1.kickEnepc(4, 0);
                Runtime.enable(524288);
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
                Runtime.jumpCF(1050, 3);
                break;
            }
            case 1: {
                Runtime.setFlags(6033, 1, 1);
                Runtime.jumpCF(1050, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(1020, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(1010, 1);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.235f, 0.235f, 0.235f);
        Runtime.setIdLightCol(1, 1, 0.235f, 0.235f, 0.235f);
        Runtime.setIdLightCol(1, 2, 0.235f, 0.235f, 0.235f);
        Runtime.setIdLightCol(1, 3, 0.235f, 0.235f, 0.235f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightCol(3, 1, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightCol(3, 2, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightCol(3, 3, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.425f, 0.4f, 0.415f);
        Runtime.setIdLightCol(4, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.5f, -1.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(6, 0, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightCol(6, 1, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightCol(6, 2, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightCol(6, 3, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightVec(6, 1, 0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(6, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(6, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(7, 0, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightCol(7, 1, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightCol(7, 2, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightCol(7, 3, 0.475f, 0.475f, 0.475f);
        Runtime.setIdLightVec(7, 1, -0.075f, 1.0f, 0.0f);
        Runtime.setIdLightVec(7, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(7, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(8, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(8, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(8, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(8, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(8, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(8, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(8, 3, 0.0f, -1.0f, -3.0f);
        this.teiten1 = new Uwamono(28690, 7.0f, -3.0f, -15.0f);
        this.teiten1.SetBgm(196621);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFPedestal(1, -6.1802845f, 2.3198824f, -21.419668f, 42.55889f, -16.639921f, -16.458939f, 0.0f, 2.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.03f, 0.03f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 137);
        this.item3 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 138);
        this.item5 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 139);
        this.item6 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 140);
        new Uwamono(116, 32, this.item1);
        new Uwamono(115, 32);
        new Uwamono(117, 32, this.item3);
        new Uwamono(118, 32, this.item5);
        new Uwamono(28, 23, this.item6);
        Stage.setVisible(63, false);
        this.monitor = new Unit();
        this.monitor.init(24613, -8.0f, 2.0365f, -18.2226f, 0.0f);
        this.monitor.setArgs(0, 0.0f, 0.0f, 5.15f, 2.223f);
        this.monitor.setArgs(1, 20023, 0, 5150, 2223);
        this.monitor.setArgs(2, 55, 0, 15, -1);
        this.monitor.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor.signal(1);
        this.AGWS1 = new NPC_NORMAL(17409, 11, 0, 0, 8, -25.5f, -7.0f, -9.4f, 0.0f);
        this.AGWS1.dispRadar(false);
        this.AGWS2 = new NPC_NORMAL(17409, 12, 0, 0, 8, -17.5f, -7.0f, -9.4f, 0.0f);
        this.AGWS2.dispRadar(false);
        this.AGWS3 = new NPC_NORMAL(17411, 13, 0, 0, 8, -1.5f, -7.0f, -9.4f, 0.0f);
        this.AGWS3.dispRadar(false);
        this.AGWS4 = new NPC_NORMAL(17411, 14, 0, 0, 8, 6.5f, -7.0f, -9.4f, 0.0f);
        this.AGWS4.dispRadar(false);
        this.Obj600_B = new Effect(1430, 0);
        this.Obj600_B.disp(false);
        this.Obj601_B = new Effect(1433, 1);
        this.Obj601_B.disp(false);
        this.Obj602 = new Effect(1438, 2);
        this.Obj602.disp(true);
        this.Obj603 = new Effect(1438, 3);
        this.Obj603.disp(true);
        this.Obj604 = new Effect(1438, 4);
        this.Obj604.disp(true);
        this.Obj605 = new Effect(1438, 5);
        this.Obj605.disp(true);
        this.Obj606 = new Effect(1438, 6);
        this.Obj606.disp(true);
        this.Obj607 = new Effect(1439, 7);
        this.Obj607.disp(true);
        this.Obj608 = new Effect(1439, 8);
        this.Obj608.disp(true);
        this.Obj609 = new Effect(1439, 9);
        this.Obj609.disp(true);
        this.Obj610 = new Effect(1439, 10);
        this.Obj610.disp(true);
        this.Obj611 = new Effect(1439, 11);
        this.Obj611.disp(true);
        this.IA = new Uwamono(28677, 5.5f, -3.0f, -14.25f, 180.0f, 142);
        this.IA.SetSymbol(28683);
        this.doorA = new Uwamono(22, 42, '\u0001');
        new Uwamono(23, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0002');
        this.doorA.SetDoorRange(2.0f);
        this.doorA.SetDoorSpd(60);
        if (this.NO5 == 1) {
            this.doorA.SetDoorSpd(0);
            this.doorA.DoorOpen();
            this.Obj600_B.disp(true);
            this.Obj601_B.disp(true);
        } else {
            System.println("コンソールスイッチ & モニターを点滅");
            this.Obj600_A = new Effect(1431, 0);
            this.Obj600_A.disp(true);
            this.Obj601_A = new Effect(1434, 1);
            this.Obj601_A.disp(true);
        }
        this.door_0 = new Uwamono(25, 40, '\u0001');
        this.door_0.SetDoorType('\u0004');
        this.door_1 = new Uwamono(20, 40, '\u0001');
        this.door_1.SetDoorType('\u0004');
        this.door_2 = new Uwamono(49, 40, '\u0001');
        this.door_2.SetDoorType('\u0004');
        this.door_3 = new Uwamono(65, 40, '\u0001');
        this.door_3.SetDoorType('\u0004');
        float[] fArray2 = new float[40];
        fArray2[0] = -27.0f;
        fArray2[1] = -3.0f;
        fArray2[2] = -12.5f;
        fArray2[3] = -1.0f;
        fArray2[4] = -22.0f;
        fArray2[5] = -3.0f;
        fArray2[6] = -12.5f;
        fArray2[8] = -18.0f;
        fArray2[9] = -3.0f;
        fArray2[10] = -12.5f;
        fArray2[11] = 1.0f;
        fArray2[12] = -14.0f;
        fArray2[13] = -3.0f;
        fArray2[14] = -12.5f;
        fArray2[15] = 2.0f;
        fArray2[16] = -10.0f;
        fArray2[17] = -3.0f;
        fArray2[18] = -12.5f;
        fArray2[19] = 3.0f;
        fArray2[20] = -6.0f;
        fArray2[21] = -3.0f;
        fArray2[22] = -12.5f;
        fArray2[23] = 4.0f;
        fArray2[24] = -2.0f;
        fArray2[25] = -3.0f;
        fArray2[26] = -12.5f;
        fArray2[27] = 5.0f;
        fArray2[28] = 2.0f;
        fArray2[29] = -3.0f;
        fArray2[30] = -12.5f;
        fArray2[31] = 6.0f;
        fArray2[32] = 6.5f;
        fArray2[33] = -3.0f;
        fArray2[34] = -12.5f;
        fArray2[35] = 7.0f;
        fArray2[36] = 10.0f;
        fArray2[37] = -3.0f;
        fArray2[38] = -12.5f;
        fArray2[39] = 8.0f;
        float[] fArray3 = fArray2;
        float[] fArray4 = new float[]{-27.0f, -3.0f, -12.5f, -27.0f, -3.0f, -12.5f, -24.0f, -3.0f, -12.5f, -22.0f, -3.0f, -12.5f, -18.0f, -3.0f, -12.5f, -14.0f, -3.0f, -12.5f, -10.0f, -3.0f, -12.5f, -6.0f, -3.0f, -12.5f, -2.0f, -3.0f, -12.5f, 2.0f, -3.0f, -12.5f, 6.0f, -3.0f, -12.5f, 10.0f, -3.0f, -12.5f, 6.0f, -3.0f, -12.5f, 2.0f, -3.0f, -12.5f, -2.0f, -3.0f, -12.5f, -6.0f, -3.0f, -12.5f, -10.0f, -3.0f, -12.5f, -14.0f, -3.0f, -12.5f, -18.0f, -3.0f, -12.5f, -22.0f, -3.0f, -12.5f, -27.0f, -3.0f, -12.5f};
        float[] fArray5 = new float[36];
        fArray5[0] = -14.0f;
        fArray5[1] = -3.0f;
        fArray5[2] = -12.5f;
        fArray5[3] = -1.0f;
        fArray5[4] = -22.0f;
        fArray5[5] = -3.0f;
        fArray5[6] = -12.5f;
        fArray5[8] = -27.0f;
        fArray5[9] = -3.0f;
        fArray5[10] = -12.5f;
        fArray5[11] = 1.0f;
        fArray5[12] = -10.0f;
        fArray5[13] = -3.0f;
        fArray5[14] = -12.5f;
        fArray5[16] = -6.0f;
        fArray5[17] = -3.0f;
        fArray5[18] = -12.5f;
        fArray5[19] = 3.0f;
        fArray5[20] = -2.0f;
        fArray5[21] = -3.0f;
        fArray5[22] = -12.5f;
        fArray5[23] = 4.0f;
        fArray5[24] = 2.0f;
        fArray5[25] = -3.0f;
        fArray5[26] = -12.5f;
        fArray5[27] = 5.0f;
        fArray5[28] = 6.5f;
        fArray5[29] = -3.0f;
        fArray5[30] = -12.5f;
        fArray5[31] = 6.0f;
        fArray5[32] = 10.0f;
        fArray5[33] = -3.0f;
        fArray5[34] = -12.5f;
        fArray5[35] = 7.0f;
        float[] fArray6 = fArray5;
        float[] fArray7 = new float[]{-14.0f, -3.0f, -12.5f, -14.0f, -3.0f, -12.5f, -10.0f, -3.0f, -12.5f, -6.0f, -3.0f, -12.5f, -2.0f, -3.0f, -12.5f, 2.0f, -3.0f, -12.5f, 5.0f, -3.0f, -12.5f, 2.0f, -3.0f, -12.5f, -2.0f, -3.0f, -12.5f, -6.0f, -3.0f, -12.5f, -10.0f, -3.0f, -12.5f, -12.0f, -3.0f, -12.5f, -13.9999f, -3.0f, -12.5f, -15.0f, -3.0f, -12.5f, -17.0f, -3.0f, -12.5f, -21.0f, -3.0f, -12.5f, -25.0f, -3.0f, -12.5f, -21.0f, -3.0f, -12.5f, -17.0f, -3.0f, -12.5f, -14.0f, -3.0f, -12.5f};
        if (this.HEISHI_AGWS == 0 && this.HEISHI_LIFE1 == 0) {
            System.println("兵士１(AGWS)　初期化");
            this.enemy1 = new Enepc();
            this.enemy1.init(17153, 3, 0.0f, 0.0f, -21.0f, 0.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(1, 1, 2, 2);
            this.enemy1.setBatEvent(2);
            this.enemy1.setParams(1, 3, 1, 3, fArray6);
            this.enemy1.setParams(fArray7);
            this.enemy1.setVisible(false);
            this.enemy1.setInvalidID(1);
            this.enemy1.kickEnepc(4, 1);
            this.Bang = new Effect(1508, 10.0f, -2.5f, -5.5f, 0.0f);
            this.Bang.disp(false);
            this.Bikkuri = new Effect(1625, 0.625f, 3.55f, -18.0f, 0.0f);
            this.Bikkuri.setScale(1.25f, 1.25f, 1.25f);
            this.Bikkuri.disp(false);
            this.K1 = new Uwamono(119, 32);
            this.K1.SetCallNo(1);
        } else if (this.HEISHI_AGWS == 1 && this.HEISHI_LIFE1 == 0) {
            System.println("兵士１(索敵)　初期化");
            this.enemy1 = new Enepc();
            this.enemy1.init(17153, 3, -27.0f, -3.0f, -12.5f, 90.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(1, 1, 2, 2);
            this.enemy1.setBatEvent(2);
            this.enemy1.setParams(1, 3, 1, 3, fArray3);
            this.enemy1.setParams(fArray4);
            new Uwamono(119, 32);
        } else if (this.HEISHI_AGWS == 1 && this.HEISHI_LIFE1 == 1) {
            new Uwamono(119, 32);
        }
        if (this.HEISHI_LIFE2 == 0) {
            System.println("部屋前通路にいる兵士（左）　初期化");
            this.enemy2 = new Enepc();
            this.enemy2.init(17153, 3, -11.0f, 0.0f, 6.0f, 0.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(1, 1, 2, 2);
            this.enemy2.setBatEvent(2);
            float[] fArray8 = new float[236];
            fArray8[0] = -11.0f;
            fArray8[2] = 6.0f;
            fArray8[3] = -1.0f;
            fArray8[4] = -10.0f;
            fArray8[6] = 6.0f;
            fArray8[8] = -10.0f;
            fArray8[10] = 5.0f;
            fArray8[11] = 1.0f;
            fArray8[12] = -9.5f;
            fArray8[14] = 4.5f;
            fArray8[15] = 2.0f;
            fArray8[16] = -11.0f;
            fArray8[18] = 7.0f;
            fArray8[20] = -10.0f;
            fArray8[22] = 8.0f;
            fArray8[23] = 4.0f;
            fArray8[24] = -11.0f;
            fArray8[26] = 9.0f;
            fArray8[27] = 4.0f;
            fArray8[28] = -10.0f;
            fArray8[30] = 9.5f;
            fArray8[31] = 6.0f;
            fArray8[32] = -11.0f;
            fArray8[34] = 11.0f;
            fArray8[35] = 6.0f;
            fArray8[36] = -11.0f;
            fArray8[38] = 13.0f;
            fArray8[39] = 8.0f;
            fArray8[40] = -11.0f;
            fArray8[42] = 15.0f;
            fArray8[43] = 9.0f;
            fArray8[44] = -10.0f;
            fArray8[46] = 14.0f;
            fArray8[47] = 9.0f;
            fArray8[48] = -9.0f;
            fArray8[50] = 14.0f;
            fArray8[51] = 11.0f;
            fArray8[52] = -9.0f;
            fArray8[54] = 15.0f;
            fArray8[55] = 12.0f;
            fArray8[56] = -9.0f;
            fArray8[58] = 16.0f;
            fArray8[59] = 13.0f;
            fArray8[60] = -9.0f;
            fArray8[62] = 17.0f;
            fArray8[63] = 14.0f;
            fArray8[64] = -11.0f;
            fArray8[66] = 17.0f;
            fArray8[67] = 10.0f;
            fArray8[68] = -11.0f;
            fArray8[70] = 18.0f;
            fArray8[71] = 16.0f;
            fArray8[72] = -11.0f;
            fArray8[74] = 20.0f;
            fArray8[75] = 17.0f;
            fArray8[76] = -11.0f;
            fArray8[78] = 22.0f;
            fArray8[79] = 18.0f;
            fArray8[80] = -11.0f;
            fArray8[82] = 24.0f;
            fArray8[83] = 19.0f;
            fArray8[84] = -11.0f;
            fArray8[86] = 25.0f;
            fArray8[87] = 20.0f;
            fArray8[88] = -11.0f;
            fArray8[90] = 26.0f;
            fArray8[91] = 21.0f;
            fArray8[92] = -9.0f;
            fArray8[94] = 22.0f;
            fArray8[95] = 19.0f;
            fArray8[96] = -9.0f;
            fArray8[98] = 24.0f;
            fArray8[99] = 20.0f;
            fArray8[100] = -9.0f;
            fArray8[102] = 26.0f;
            fArray8[103] = 22.0f;
            fArray8[104] = -7.0f;
            fArray8[106] = 26.0f;
            fArray8[107] = 25.0f;
            fArray8[108] = -7.0f;
            fArray8[110] = 24.0f;
            fArray8[111] = 24.0f;
            fArray8[112] = -7.0f;
            fArray8[114] = 22.0f;
            fArray8[115] = 23.0f;
            fArray8[116] = -7.5f;
            fArray8[118] = 21.0f;
            fArray8[119] = 23.0f;
            fArray8[120] = -7.0f;
            fArray8[122] = 20.0f;
            fArray8[123] = 29.0f;
            fArray8[124] = -7.0f;
            fArray8[126] = 18.0f;
            fArray8[127] = 15.0f;
            fArray8[128] = -7.0f;
            fArray8[130] = 17.0f;
            fArray8[131] = 14.0f;
            fArray8[132] = -8.0f;
            fArray8[134] = 14.0f;
            fArray8[135] = 12.0f;
            fArray8[136] = -7.0f;
            fArray8[138] = 15.0f;
            fArray8[139] = 33.0f;
            fArray8[140] = -8.0f;
            fArray8[142] = 9.5f;
            fArray8[143] = 7.0f;
            fArray8[144] = -7.0f;
            fArray8[146] = 9.0f;
            fArray8[147] = 35.0f;
            fArray8[148] = -8.0f;
            fArray8[150] = 8.0f;
            fArray8[151] = 7.0f;
            fArray8[152] = -7.25f;
            fArray8[154] = 10.0f;
            fArray8[155] = 35.0f;
            fArray8[156] = -7.5f;
            fArray8[158] = 8.0f;
            fArray8[159] = 37.0f;
            fArray8[160] = -7.0f;
            fArray8[162] = 7.0f;
            fArray8[163] = 37.0f;
            fArray8[164] = -7.0f;
            fArray8[166] = 6.5f;
            fArray8[167] = 40.0f;
            fArray8[168] = -8.0f;
            fArray8[170] = 6.0f;
            fArray8[171] = 37.0f;
            fArray8[172] = -9.0f;
            fArray8[174] = 4.5f;
            fArray8[175] = 2.0f;
            fArray8[176] = -8.0f;
            fArray8[178] = 5.0f;
            fArray8[179] = 42.0f;
            fArray8[180] = -9.0f;
            fArray8[182] = 5.0f;
            fArray8[183] = 1.0f;
            fArray8[184] = -8.5f;
            fArray8[186] = 5.0f;
            fArray8[187] = 42.0f;
            fArray8[188] = -9.5f;
            fArray8[190] = 5.0f;
            fArray8[191] = 2.0f;
            fArray8[192] = -8.5f;
            fArray8[194] = 4.5f;
            fArray8[195] = 45.0f;
            fArray8[196] = -9.5f;
            fArray8[198] = 4.15f;
            fArray8[199] = 3.0f;
            fArray8[200] = -9.0f;
            fArray8[202] = 4.15f;
            fArray8[203] = 3.0f;
            fArray8[204] = -8.5f;
            fArray8[206] = 4.15f;
            fArray8[207] = 50.0f;
            fArray8[208] = -10.75f;
            fArray8[210] = 10.0f;
            fArray8[211] = 6.0f;
            fArray8[212] = -10.75f;
            fArray8[214] = 11.5f;
            fArray8[215] = 8.0f;
            fArray8[216] = -7.0f;
            fArray8[218] = 11.0f;
            fArray8[219] = 38.0f;
            fArray8[220] = -7.25f;
            fArray8[222] = 11.5f;
            fArray8[223] = 54.0f;
            fArray8[224] = -7.0f;
            fArray8[226] = 13.0f;
            fArray8[227] = 33.0f;
            fArray8[228] = -8.0f;
            fArray8[230] = 12.5f;
            fArray8[231] = 55.0f;
            fArray8[232] = -7.75f;
            fArray8[234] = 13.25f;
            fArray8[235] = 33.0f;
            fArray = fArray8;
            this.enemy2.setParams(2, 2, 2, 3, fArray);
        }
        if (this.HEISHI_LIFE3 == 0) {
            System.println("部屋前通路にいる兵士（右）　初期化");
            this.enemy3 = new Enepc();
            this.enemy3.init(17153, 3, -7.0f, 0.0f, 6.0f, 0.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(1, 1, 2, 2);
            this.enemy3.setBatEvent(2);
            float[] fArray9 = new float[256];
            fArray9[0] = -9.0f;
            fArray9[2] = 6.5f;
            fArray9[3] = -1.0f;
            fArray9[4] = -7.0f;
            fArray9[6] = 6.0f;
            fArray9[8] = -10.5f;
            fArray9[10] = 8.0f;
            fArray9[12] = -7.5f;
            fArray9[14] = 10.0f;
            fArray9[15] = 1.0f;
            fArray9[16] = -10.5f;
            fArray9[18] = 10.0f;
            fArray9[19] = 2.0f;
            fArray9[20] = -7.5f;
            fArray9[22] = 13.0f;
            fArray9[23] = 3.0f;
            fArray9[24] = -8.0f;
            fArray9[26] = 6.0f;
            fArray9[28] = -9.0f;
            fArray9[30] = 5.0f;
            fArray9[32] = -8.0f;
            fArray9[34] = 5.0f;
            fArray9[35] = 6.0f;
            fArray9[36] = -8.5f;
            fArray9[38] = 5.0f;
            fArray9[39] = 6.0f;
            fArray9[40] = -8.25f;
            fArray9[42] = 4.5f;
            fArray9[43] = 8.0f;
            fArray9[44] = -10.5f;
            fArray9[46] = 13.0f;
            fArray9[47] = 4.0f;
            fArray9[48] = -10.5f;
            fArray9[50] = 15.0f;
            fArray9[51] = 11.0f;
            fArray9[52] = -7.5f;
            fArray9[54] = 15.0f;
            fArray9[55] = 5.0f;
            fArray9[56] = -10.5f;
            fArray9[58] = 17.5f;
            fArray9[59] = 12.0f;
            fArray9[60] = -7.5f;
            fArray9[62] = 17.5f;
            fArray9[63] = 13.0f;
            fArray9[64] = -10.5f;
            fArray9[66] = 19.5f;
            fArray9[67] = 14.0f;
            fArray9[68] = -7.5f;
            fArray9[70] = 19.5f;
            fArray9[71] = 15.0f;
            fArray9[72] = -10.5f;
            fArray9[74] = 21.5f;
            fArray9[75] = 16.0f;
            fArray9[76] = -7.5f;
            fArray9[78] = 21.5f;
            fArray9[79] = 17.0f;
            fArray9[80] = -10.0f;
            fArray9[82] = 23.0f;
            fArray9[83] = 18.0f;
            fArray9[84] = -8.0f;
            fArray9[86] = 23.0f;
            fArray9[87] = 19.0f;
            fArray9[88] = -10.0f;
            fArray9[90] = 25.0f;
            fArray9[91] = 20.0f;
            fArray9[92] = -8.0f;
            fArray9[94] = 25.0f;
            fArray9[95] = 21.0f;
            fArray9[96] = -10.0f;
            fArray9[98] = 7.0f;
            fArray9[99] = 6.0f;
            fArray9[100] = -10.0f;
            fArray9[102] = 8.25f;
            fArray9[103] = 24.0f;
            fArray9[104] = -10.0f;
            fArray9[106] = 9.5f;
            fArray9[107] = 25.0f;
            fArray9[108] = -10.0f;
            fArray9[110] = 10.0f;
            fArray9[111] = 26.0f;
            fArray9[112] = -10.0f;
            fArray9[114] = 11.0f;
            fArray9[115] = 27.0f;
            fArray9[116] = -10.0f;
            fArray9[118] = 12.0f;
            fArray9[119] = 28.0f;
            fArray9[120] = -10.0f;
            fArray9[122] = 13.0f;
            fArray9[123] = 29.0f;
            fArray9[124] = -10.0f;
            fArray9[126] = 13.5f;
            fArray9[127] = 30.0f;
            fArray9[128] = -10.0f;
            fArray9[130] = 14.75f;
            fArray9[131] = 31.0f;
            fArray9[132] = -10.0f;
            fArray9[134] = 16.25f;
            fArray9[135] = 32.0f;
            fArray9[136] = -10.0f;
            fArray9[138] = 17.5f;
            fArray9[139] = 33.0f;
            fArray9[140] = -10.0f;
            fArray9[142] = 18.0f;
            fArray9[143] = 34.0f;
            fArray9[144] = -10.0f;
            fArray9[146] = 19.0f;
            fArray9[147] = 35.0f;
            fArray9[148] = -10.0f;
            fArray9[150] = 20.0f;
            fArray9[151] = 36.0f;
            fArray9[152] = -10.0f;
            fArray9[154] = 21.0f;
            fArray9[155] = 37.0f;
            fArray9[156] = -9.5f;
            fArray9[158] = 22.0f;
            fArray9[159] = 38.0f;
            fArray9[160] = -8.5f;
            fArray9[162] = 22.0f;
            fArray9[163] = 39.0f;
            fArray9[164] = -8.0f;
            fArray9[166] = 21.0f;
            fArray9[167] = 40.0f;
            fArray9[168] = -8.0f;
            fArray9[170] = 20.0f;
            fArray9[171] = 41.0f;
            fArray9[172] = -8.0f;
            fArray9[174] = 19.0f;
            fArray9[175] = 42.0f;
            fArray9[176] = -8.0f;
            fArray9[178] = 18.0f;
            fArray9[179] = 43.0f;
            fArray9[180] = -8.0f;
            fArray9[182] = 17.5f;
            fArray9[183] = 44.0f;
            fArray9[184] = -8.0f;
            fArray9[186] = 16.25f;
            fArray9[187] = 45.0f;
            fArray9[188] = -8.0f;
            fArray9[190] = 14.75f;
            fArray9[191] = 46.0f;
            fArray9[192] = -8.0f;
            fArray9[194] = 13.5f;
            fArray9[195] = 47.0f;
            fArray9[196] = -8.0f;
            fArray9[198] = 13.0f;
            fArray9[199] = 48.0f;
            fArray9[200] = -8.0f;
            fArray9[202] = 12.0f;
            fArray9[203] = 49.0f;
            fArray9[204] = -8.0f;
            fArray9[206] = 11.0f;
            fArray9[207] = 50.0f;
            fArray9[208] = -8.0f;
            fArray9[210] = 10.0f;
            fArray9[211] = 51.0f;
            fArray9[212] = -8.0f;
            fArray9[214] = 9.5f;
            fArray9[215] = 52.0f;
            fArray9[216] = -8.0f;
            fArray9[218] = 8.25f;
            fArray9[219] = 53.0f;
            fArray9[220] = -8.0f;
            fArray9[222] = 7.0f;
            fArray9[223] = 54.0f;
            fArray9[224] = -8.5f;
            fArray9[226] = 17.5f;
            fArray9[227] = 44.0f;
            fArray9[228] = -8.5f;
            fArray9[230] = 16.25f;
            fArray9[231] = 56.0f;
            fArray9[232] = -8.5f;
            fArray9[234] = 14.75f;
            fArray9[235] = 57.0f;
            fArray9[236] = -8.5f;
            fArray9[238] = 13.5f;
            fArray9[239] = 58.0f;
            fArray9[240] = -9.5f;
            fArray9[242] = 13.5f;
            fArray9[243] = 30.0f;
            fArray9[244] = -9.5f;
            fArray9[246] = 14.75f;
            fArray9[247] = 60.0f;
            fArray9[248] = -9.5f;
            fArray9[250] = 16.25f;
            fArray9[251] = 61.0f;
            fArray9[252] = -9.5f;
            fArray9[254] = 17.5f;
            fArray9[255] = 62.0f;
            fArray = fArray9;
            this.enemy3.setParams(2, 5, 3, 3, fArray);
        }
        if (this.NO3_CARDKEY == 1) {
            System.println("カニメカ　初期化");
            this.enemy4 = new Enepc();
            this.enemy4.init(16643, 5, -13.0f, 0.0f, -16.0f, 270.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(3, 3, 3, 4);
            this.enemy4.setBatEvent(2);
            float[] fArray10 = new float[44];
            fArray10[0] = -13.0f;
            fArray10[2] = -16.0f;
            fArray10[3] = -1.0f;
            fArray10[4] = -9.0f;
            fArray10[6] = -16.0f;
            fArray10[8] = -9.0f;
            fArray10[10] = -7.0f;
            fArray10[11] = 1.0f;
            fArray10[12] = -9.0f;
            fArray10[14] = -3.0f;
            fArray10[15] = 2.0f;
            fArray10[16] = -9.0f;
            fArray10[19] = 3.0f;
            fArray10[22] = -16.0f;
            fArray10[23] = 1.0f;
            fArray10[26] = -20.0f;
            fArray10[27] = 5.0f;
            fArray10[30] = -27.0f;
            fArray10[31] = 6.0f;
            fArray10[34] = -29.0f;
            fArray10[35] = 7.0f;
            fArray10[36] = 2.0f;
            fArray10[38] = -27.0f;
            fArray10[39] = 7.0f;
            fArray10[40] = 3.0f;
            fArray10[42] = -16.0f;
            fArray10[43] = 5.0f;
            fArray = fArray10;
            float[] fArray11 = new float[60];
            fArray11[0] = -13.0f;
            fArray11[2] = -16.0f;
            fArray11[3] = -9.0f;
            fArray11[5] = -16.0f;
            fArray11[6] = -9.0f;
            fArray11[8] = -10.0f;
            fArray11[9] = -9.0f;
            fArray11[11] = -4.0f;
            fArray11[12] = -9.0f;
            fArray11[14] = 2.0f;
            fArray11[15] = -9.0f;
            fArray11[17] = -2.0f;
            fArray11[18] = -9.0f;
            fArray11[20] = -5.0f;
            fArray11[21] = -9.0f;
            fArray11[23] = -10.0f;
            fArray11[24] = -9.0f;
            fArray11[26] = -15.9f;
            fArray11[27] = -5.0f;
            fArray11[29] = -16.0f;
            fArray11[32] = -16.0f;
            fArray11[35] = -21.5f;
            fArray11[38] = -27.0f;
            fArray11[41] = -21.5f;
            fArray11[44] = -16.0f;
            fArray11[45] = 3.5f;
            fArray11[47] = -16.0f;
            fArray11[48] = -1.0f;
            fArray11[50] = -16.0f;
            fArray11[51] = -6.0f;
            fArray11[53] = -16.0f;
            fArray11[54] = -9.0f;
            fArray11[56] = -16.0f;
            fArray11[57] = -13.0f;
            fArray11[59] = -16.0f;
            float[] fArray12 = fArray11;
            this.enemy4.setParams(3, 6, 4, 5, fArray);
            this.enemy4.setParams(fArray12);
        }
        System.println("初期化終了");
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
            this.setShadow(4, 16);
        }

        void init() {
        }
    }
}

