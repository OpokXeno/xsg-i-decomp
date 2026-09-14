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
import xeno.map.MC_KAS17_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2510
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS17_PRJ {
    static final int MTN_TEST1 = 257;
    static final int MTN_TEST2 = 258;
    static final int MTN_TEST3 = 259;
    static final int MTN_TEST4 = 260;
    static final int MTN_TEST5 = 261;
    static final int MTN_TEST6 = 262;
    static final int MTN_TEST7 = 263;
    static final int MTN_TEST8 = 264;
    static final int MTN_TEST9 = 265;
    static final int MTN_TEST10 = 266;
    Player player;
    Camera cam1;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    MAPUnit hasi;
    Unit osake;
    Unit dummy;
    Unit dummy2;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
    Effect tabako;
    Effect taki;
    Effect fire;
    Effect sibuki;
    Effect mokumoku;
    Effect mokumoku2;
    Effect fade;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    Uwamono C1;
    Uwamono C2;
    Uwamono doorA;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono winch;
    boolean winch_move = false;
    boolean game_start = false;
    int syuukai = 0;
    boolean u_return = false;
    boolean player_ichi = true;
    int lo = 0;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    int page;
    String[] oku = new String[]{"Try putting a /[color(0x329bbe)]S. Carrot Juice/[color(0x808080)] there?", "/[waitkey(64)]/[close()]"};
    String[] kagi = new String[]{"Someone seems to be home, but there's no response.", "/[waitkey(64)]/[close()]"};
    String[] okenai = new String[]{"A weird rabbit seems to have been drinking something here.\n", "/[waitkey(64)]/[close()]"};
    String[] npc1_1 = new String[]{"/[label(Bunnie]", "I win!!! I'm not letting anyone into this room!", "/[waitkey(64)]/[close()]"};
    String[] make = new String[]{"That weird rabbit almost seems as though it has eyes on the back of its head.\n", "/[waitkey(64)]/[close()]"};
    String[] kachi1 = new String[]{"/[label(Bunnie)]", "I lost.", "/[waitkey(64)]/[close()]"};
    String[] kachi2 = new String[]{"/[label(Bunnie)]", "I'm Bunnie. You're the first people to come into this room.\n", "/[waitkey(1)]/[clear()]", "Forests these days are even more dangerous, so be careful!\n", "/[waitkey(64)]/[close()]"};
    String[] kachi3 = new String[]{"/[label(Bunnie)]", "Forests these days are even more dangerous, so be careful!\n", "/[waitkey(64)]/[close()]"};

    ST2510() {
    }

    void EOB(int n) {
        if (n == 0) {
            System.println("EOB:0");
        }
        if (n == 3) {
            Runtime.setFlags(8080, 1, 1);
        }
    }

    void EOB_Always(int n) {
        System.println("EOB_Always");
        Runtime.progressEffect(60);
    }

    void Ene_Restart() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        if (Runtime.getFlags(8080, 1) == 0) {
            this.enemy3.kickEnepc(4, 0);
        }
    }

    void Ene_Stop() {
        this.enemy1.kickEnepc(4, 2);
        this.enemy2.kickEnepc(4, 2);
        if (Runtime.getFlags(8080, 1) == 0) {
            this.enemy3.kickEnepc(4, 2);
        }
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (this.lo == 1) {
            return;
        }
        if (n == 100) {
            switch (n2) {
                case 0: {
                    if (!this.game_start || Runtime.getFlags(8059, 1) != 0) return;
                    Runtime.setFlags(8059, 1, 1);
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("lo:0");
                    Runtime.disable(524288);
                    this.npc1.kickEnepc(4, 2);
                    this.npc1.setVisible(false);
                    this.npc2.kickEnepc(4, 0);
                    this.npc2.kickEnepc(4, 1);
                    this.C1.setTranslate(15.8f, -10.5f, 19.7f);
                    this.C2.setTranslate(1.8f, -10.5f, 14.2f);
                    this.game_start = false;
                    this.syuukai = 0;
                    Runtime.enable(524288);
                    this.fade.call(0);
                    System.sleep(20);
                    this.npc2.setVisible(true);
                    this.dummy2.setVisible(true);
                    this.npc2.setInvalidID(1);
                    this.npc2.setTranslate(8.3f, 0.0f, -9.0f);
                    this.npc2.setRotate(0.0f, 0.0f, 0.0f);
                    this.npc2.kickEnepc(7, 3);
                    this.npc2.setMotion(0, 34);
                    System.sleep(10);
                    this.player.setTranslate(8.3f, 0.0f, -7.0f);
                    this.cam0.setMode(-1);
                    this.camEV = Camera.create(1);
                    this.camEV.setRotate(-9.3f, 44.1f, 0.0f);
                    this.camEV.setTranslate(10.5f, 1.4f, -5.6f);
                    this.camEV.setFov(42.5f);
                    this.camEV.change();
                    System.sleep(30);
                    this.nwin(this.kachi1);
                    this.npc2.moveEnepc(17, 170.0f, 0.0f, 30);
                    System.sleep(60);
                    this.nwin(this.kachi2);
                    this.lo = 0;
                    this.Ene_Restart();
                    Runtime.enable(524288);
                    this.npc2.disableDTKFlag(4);
                    this.npc2.disableDTKFlag(2);
                    this.npc2.disableDTKFlag(1);
                    this.npc2.enableDTKFlag(65536);
                    this.npc2.talkto("TalkNPC1");
                    Runtime.setPlayerControl(true);
                    this.doorA.SetDoorType('\u0004');
                    this.fade.call(0);
                    System.sleep(20);
                    this.cam0.setMode(0);
                    System.sleep(10);
                    return;
                }
                case 1: {
                    Runtime.setRegister(1, (float) Runtime.checkItem(10, 62));
                    System.println("ジュースの数: /[#1]");
                    if (this.game_start || Runtime.getFlags(8059, 1) != 0) return;
                    if (Runtime.checkItem(10, 62) >= 1) {
                        Runtime.setPlayerControl(false);
                        this.lo = 1;
                        System.println("lo:1");
                        this.nwin(this.oku);
                        this.yesno();
                        switch (this.selected) {
                            case 0: {
                                Runtime.removeItem(10, 62);
                                Runtime.disable(524288);
                                Stage.setVisible(18, true);
                                System.sleep(30);
                                this.npc1.setInvalidID(1);
                                this.Ene_Stop();
                                this.fade.call(0);
                                System.sleep(30);
                                this.game_start = true;
                                this.cam0.setMode(-1);
                                this.camEV = Camera.create(1);
                                this.camEV.setRotate(-25.1f, -8.5f, 0.0f);
                                this.camEV.setTranslate(7.4f, 8.0f, 30.8f);
                                this.camEV.setFov(40.0f);
                                this.camEV.change();
                                this.player.setTranslate(14.8f, 0.0f, 19.5f);
                                Runtime.enable(65536);
                                this.player.rotY(10, 225.0f, true);
                                this.C1.setTranslate(15.8f, -0.5f, 19.7f);
                                this.C2.setTranslate(1.8f, -0.5f, 14.2f);
                                this.dummy.setVisible(false);
                                this.doorA.DoorOpen();
                                this.npc1.kickEnepc(4, 1);
                                this.npc1.kickEnepc(1, 1);
                                this.npc1.moveEnepc(15, 8.5f, 12.0f, 150);
                                System.sleep(150);
                                this.npc1.kickEnepc(1, 62);
                                System.sleep(139);
                                Stage.setVisible(18, false);
                                this.dummy.setVisible(true);
                                System.sleep(10);
                                this.npc1.kickEnepc(1, 1);
                                this.npc1.moveEnepc(15, 7.9f, 9.6f, 60);
                                System.sleep(90);
                                this.npc1.kickEnepc(7, 2);
                                this.npc1.kickEnepc(4, 0);
                                this.npc1.setInvalidID(0);
                                Runtime.disable(65536);
                                this.lo = 0;
                                Runtime.setPlayerControl(true);
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        this.lo = 0;
                        return;
                    }
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    this.nwin(this.okenai);
                    this.lo = 0;
                    Runtime.setPlayerControl(true);
                    return;
                }
                case 2: {
                    if (Runtime.getFlags(8062, 1) != 0) return;
                    Runtime.setFlags(8062, 1, 1);
                    Runtime.setPlayerControl(false);
                    Runtime.disable(524288);
                    this.Ene_Stop();
                    this.lo = 1;
                    System.println("イベント起動");
                    this.npc2.kickEnepc(4, 0);
                    Runtime.enable(65536);
                    this.player.rotY(10, 90.0f, true);
                    System.sleep(10);
                    this.npc2.setVisible(true);
                    this.cam0.setMode(-1);
                    this.camEV = Camera.create(1);
                    this.camEV.setRotate(-18.5f, 0.0f, 0.0f);
                    this.camEV.setTranslate(6.4f, 7.0f, 26.2f);
                    this.camEV.setFov(40.0f);
                    this.camEV.change();
                    System.sleep(25);
                    Sound.effectPlay(393218);
                    System.sleep(30);
                    Sound.effectPlay(393218);
                    System.sleep(30);
                    Sound.effectPlay(393218);
                    System.sleep(35);
                    this.dummy2.setVisible(true);
                    Stage.setVisible(18, false);
                    System.sleep(20);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(393218);
                    System.sleep(10);
                    Sound.effectPlay(196710);
                    this.doorA.DoorClose();
                    this.npc2.kickEnepc(4, 2);
                    this.mokumoku2.disp(true);
                    System.sleep(30);
                    this.mokumoku2.disp(false);
                    this.mokumoku2.clearEffect();
                    System.sleep(20);
                    this.npc1.setVisible(true);
                    this.npc2.setVisible(false);
                    this.dummy2.setVisible(false);
                    this.cam0.setMode(0);
                    this.lo = 0;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    Runtime.enable(524288);
                    this.Ene_Restart();
                    return;
                }
                case 4: {
                    if (Runtime.getFlags(8059, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("lo:0");
                    this.nwin(this.kagi);
                    this.lo = 0;
                    Runtime.setPlayerControl(true);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n != 11) return;
        System.println("うーくん");
        switch (n2) {
            case 0: {
                if (!this.game_start || Runtime.getFlags(8059, 1) != 0) return;
                this.game_start = false;
                Runtime.setPlayerControl(false);
                this.npc1.setTranslate(8.4f, 3.0f, -9.0f);
                this.npc1.kickEnepc(4, 2);
                this.npc1.setVisible(false);
                this.doorA.DoorClose();
                this.mokumoku2.disp(true);
                System.sleep(30);
                this.mokumoku2.disp(false);
                this.mokumoku2.clearEffect();
                this.nwin(this.npc1_1);
                System.sleep(20);
                if (Runtime.getFlags(8096, 1) == 0) {
                    Runtime.setFlags(8096, 1, 1);
                    System.println("負け:1");
                } else if (Runtime.getFlags(8097, 1) == 0) {
                    Runtime.setFlags(8097, 1, 1);
                    System.println("負け:2");
                } else if (Runtime.getFlags(8098, 1) == 0) {
                    Runtime.setFlags(8098, 1, 1);
                    System.println("負け:3");
                } else if (Runtime.getFlags(8099, 1) == 0) {
                    this.nwin(this.make);
                    Runtime.setFlags(8099, 1, 1);
                    System.println("負け:4");
                }
                System.sleep(30);
                this.npc1.kickEnepc(4, 0);
                this.C1.setTranslate(15.8f, -10.5f, 19.7f);
                this.C2.setTranslate(1.8f, -10.5f, 14.2f);
                this.syuukai = 0;
                Runtime.enable(524288);
                this.npc1.setTranslate(8.3f, 3.0f, -9.0f);
                this.npc1.setRotate(0.0f, 0.0f, 0.0f);
                this.npc1.kickEnepc(7, 3);
                this.npc1.setVisible(true);
                this.fade.call(0);
                System.sleep(20);
                this.player.getTranslate();
                this.cam0.setMode(0);
                System.sleep(10);
                this.Ene_Restart();
                this.mokumoku2.disp(false);
                this.mokumoku2.clearEffect();
                Runtime.setPlayerControl(true);
                return;
            }
            case 3: {
                System.println("周回");
                if (!this.game_start) return;
                ++this.syuukai;
                if (this.syuukai != 5) return;
                this.npc1.kickEnepc(4, 1);
                this.npc1.moveEnepc(17, 180.0f, 0.0f, 10);
                this.npc1.kickEnepc(1, 1);
                this.npc1.moveEnepc(15, 8.3f, -9.0f, 120);
                this.u_return = true;
                return;
            }
            default: {
                return;
            }
        }
    }

    void TalkNPC1(Enepc enepc) {
        this.nwin(this.kachi3);
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("broken:1");
                if (Runtime.getFlags(8060, 1) != 0) break;
                this.Ene_Stop();
                this.winch_move = true;
                Runtime.setFlags(8060, 1, 1);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("MC_KAS16・２");
                Runtime.jumpCF(2509, 2);
                break;
            }
            case 1: {
                System.println("MC_KAS16・３");
                Runtime.jumpCF(2509, 3);
                break;
            }
            case 2: {
                if (Runtime.getFlags(335, 1) == 0) {
                    Runtime.setFlags(335, 1, 1);
                    System.println("イベント3023");
                    Runtime.jumpEvent(3230);
                    break;
                }
                System.println("教会外観・１");
                Runtime.jumpCF(2419, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        this.hasi = new Mapunits();
        this.hasi.mapUnit(57);
        this.hasi.start(4, null);
        this.teiten1 = new Uwamono(28690, 7.0f, 0.0f, -10.7f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, -13.0f, 0.0f, -4.5f);
        this.teiten2.SetBgm(196613);
        this.light01 = new Effect(1626, -9.4f, 0.0f, -8.8f, 0.0f);
        this.light01.setRotate(0.0f, 90.0f, 0.0f);
        this.light02 = new Effect(1626, -16.4f, 0.0f, -4.1f, 0.0f);
        this.light02.setRotate(0.0f, 90.0f, 0.0f);
        this.light03 = new Effect(1626, 22.6f, 0.0f, -21.0f, 0.0f);
        this.light03.setRotate(0.0f, 90.0f, 0.0f);
        this.light04 = new Effect(1626, 2.7f, 0.0f, -25.5f, 0.0f);
        this.light04.setRotate(0.0f, 90.0f, 0.0f);
        this.taki = new Effect(1450, -13.3f, -2.4f, -5.4f, 0.0f);
        this.taki.setScale(1.5f, 2.3f, 1.5f);
        this.fire = new Effect(1402, 7.0f, 0.0f, -10.7f, 0.0f);
        this.fire.setScale(0.3f, 0.3f, 0.3f);
        this.sibuki = new Effect(1496, 4.8f, -7.9f, -6.5f, 0.0f);
        this.mokumoku = new Effect(1514, 7.0f, 4.2f, -10.7f, 0.0f);
        this.light05 = new Effect(1405, 9.5f, 1.8f, -10.0f, 0.0f);
        this.light05.setScale(0.5f, 0.5f, 0.5f);
        Runtime.progressEffect(60);
        this.light06 = new Effect(1660, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light06.disp(false);
        this.light06.setForceLoop(true);
        this.light07 = new Effect(1660, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light07.disp(false);
        this.light07.setForceLoop(true);
        this.light08 = new Effect(1660, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light08.disp(false);
        this.light08.setForceLoop(true);
        this.light09 = new Effect(1660, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light09.disp(false);
        this.light09.setForceLoop(true);
        this.tabako = new Effect(1533, 10.6f, 1.0f, -10.0f, 0.0f);
        this.mokumoku2 = new Effect(1612, 8.5f, 0.0f, -3.7f, 0.0f);
        this.mokumoku2.disp(false);
        this.mokumoku2.setForceLoop(true);
        this.mokumoku2.setScale(0.5f, 0.5f, 0.5f);
        Stage.setVisible(54, false);
        Stage.setVisible(55, false);
        Stage.setVisible(56, false);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        if (n == 3) {
            this.player_ichi = false;
            this.mokumoku.disp(false);
        }
        this.player.setID(1);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 5000, 1);
        Runtime.setDefocusQuick(1, 1, 4000, 1);
        Runtime.setDefocusQuick(2, 1, 3000, 1);
        Runtime.setDefocusQuick(3, 1, 2000, 1);
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
        Runtime.setIdLightCol(2, 0, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 1, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 2, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 3, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.45f, 0.45f, 0.375f);
        Runtime.setIdLightCol(4, 1, 0.45f, 0.45f, 0.375f);
        Runtime.setIdLightCol(4, 2, 0.45f, 0.45f, 0.375f);
        Runtime.setIdLightCol(4, 3, 0.45f, 0.45f, 0.375f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(5, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(5, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(5, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 15.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(4, 0.015f, 0.015f);
        this.cam0.setCFAngle(5, -28.0f, 6.5f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 11.0f, 42.5f);
        this.cam0.setCFHokan(6, 0.015f, 0.015f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(7, 0.015f, 0.015f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(8, 0.015f, 0.015f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(9, 0.015f, 0.015f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(10, 0.015f, 0.015f);
        this.cam0.setFog(1, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(2, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(3, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(4, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(5, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(6, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(7, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(8, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(9, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        this.cam0.setFog(10, 8.0f, 12.0f, 0.0f, 0.5f, 50, 50, 50, 255);
        float[] fArray = new float[164];
        fArray[0] = 8.4f;
        fArray[1] = 3.0f;
        fArray[2] = -5.0f;
        fArray[3] = -1.0f;
        fArray[4] = 8.4f;
        fArray[5] = 3.0f;
        fArray[6] = -3.5f;
        fArray[8] = 8.4f;
        fArray[9] = 3.0f;
        fArray[10] = -2.0f;
        fArray[11] = 1.0f;
        fArray[12] = 8.4f;
        fArray[13] = 3.0f;
        fArray[14] = -0.5f;
        fArray[15] = 2.0f;
        fArray[16] = 8.4f;
        fArray[17] = 3.0f;
        fArray[18] = 1.0f;
        fArray[19] = 3.0f;
        fArray[20] = 8.4f;
        fArray[21] = 3.0f;
        fArray[22] = 2.5f;
        fArray[23] = 4.0f;
        fArray[24] = 8.4f;
        fArray[25] = 3.0f;
        fArray[26] = 4.0f;
        fArray[27] = 5.0f;
        fArray[28] = 8.4f;
        fArray[29] = 3.0f;
        fArray[30] = 5.5f;
        fArray[31] = 6.0f;
        fArray[32] = 8.4f;
        fArray[33] = 3.0f;
        fArray[34] = 7.0f;
        fArray[35] = 7.0f;
        fArray[36] = 8.4f;
        fArray[37] = 3.0f;
        fArray[38] = 8.5f;
        fArray[39] = 8.0f;
        fArray[40] = 8.4f;
        fArray[41] = 3.0f;
        fArray[42] = 10.0f;
        fArray[43] = 9.0f;
        fArray[44] = 8.4f;
        fArray[45] = 3.0f;
        fArray[46] = 11.5f;
        fArray[47] = 10.0f;
        fArray[48] = 8.4f;
        fArray[49] = 3.0f;
        fArray[50] = 13.0f;
        fArray[51] = 11.0f;
        fArray[52] = 8.4f;
        fArray[53] = 3.0f;
        fArray[54] = 14.5f;
        fArray[55] = 12.0f;
        fArray[56] = 8.4f;
        fArray[57] = 3.0f;
        fArray[58] = 16.0f;
        fArray[59] = 13.0f;
        fArray[60] = 8.4f;
        fArray[61] = 3.0f;
        fArray[62] = -2.0f;
        fArray[63] = 1.0f;
        fArray[64] = 8.7f;
        fArray[65] = 3.0f;
        fArray[66] = -0.5f;
        fArray[67] = 15.0f;
        fArray[68] = 9.0f;
        fArray[69] = 3.0f;
        fArray[70] = 1.0f;
        fArray[71] = 16.0f;
        fArray[72] = 9.3f;
        fArray[73] = 3.0f;
        fArray[74] = 2.5f;
        fArray[75] = 17.0f;
        fArray[76] = 9.6f;
        fArray[77] = 3.0f;
        fArray[78] = 4.0f;
        fArray[79] = 18.0f;
        fArray[80] = 9.9f;
        fArray[81] = 3.0f;
        fArray[82] = 5.5f;
        fArray[83] = 19.0f;
        fArray[84] = 10.2f;
        fArray[85] = 3.0f;
        fArray[86] = 7.0f;
        fArray[87] = 20.0f;
        fArray[88] = 10.5f;
        fArray[89] = 3.0f;
        fArray[90] = 8.5f;
        fArray[91] = 21.0f;
        fArray[92] = 10.8f;
        fArray[93] = 3.0f;
        fArray[94] = 10.0f;
        fArray[95] = 22.0f;
        fArray[96] = 11.1f;
        fArray[97] = 3.0f;
        fArray[98] = 11.5f;
        fArray[99] = 23.0f;
        fArray[100] = 11.4f;
        fArray[101] = 3.0f;
        fArray[102] = 13.0f;
        fArray[103] = 24.0f;
        fArray[104] = 11.7f;
        fArray[105] = 3.0f;
        fArray[106] = 14.5f;
        fArray[107] = 25.0f;
        fArray[108] = 12.0f;
        fArray[109] = 3.0f;
        fArray[110] = 16.0f;
        fArray[111] = 26.0f;
        fArray[112] = 8.4f;
        fArray[113] = 3.0f;
        fArray[114] = -2.0f;
        fArray[115] = 1.0f;
        fArray[116] = 8.1f;
        fArray[117] = 3.0f;
        fArray[118] = -0.5f;
        fArray[119] = 28.0f;
        fArray[120] = 7.8f;
        fArray[121] = 3.0f;
        fArray[122] = 1.0f;
        fArray[123] = 29.0f;
        fArray[124] = 7.5f;
        fArray[125] = 3.0f;
        fArray[126] = 2.5f;
        fArray[127] = 30.0f;
        fArray[128] = 7.2f;
        fArray[129] = 3.0f;
        fArray[130] = 4.0f;
        fArray[131] = 31.0f;
        fArray[132] = 6.9f;
        fArray[133] = 3.0f;
        fArray[134] = 5.5f;
        fArray[135] = 32.0f;
        fArray[136] = 6.6f;
        fArray[137] = 3.0f;
        fArray[138] = 7.0f;
        fArray[139] = 33.0f;
        fArray[140] = 6.3f;
        fArray[141] = 3.0f;
        fArray[142] = 8.5f;
        fArray[143] = 34.0f;
        fArray[144] = 6.0f;
        fArray[145] = 3.0f;
        fArray[146] = 10.0f;
        fArray[147] = 35.0f;
        fArray[148] = 5.7f;
        fArray[149] = 3.0f;
        fArray[150] = 11.5f;
        fArray[151] = 36.0f;
        fArray[152] = 5.4f;
        fArray[153] = 3.0f;
        fArray[154] = 13.0f;
        fArray[155] = 37.0f;
        fArray[156] = 5.1f;
        fArray[157] = 3.0f;
        fArray[158] = 14.5f;
        fArray[159] = 38.0f;
        fArray[160] = 4.8f;
        fArray[161] = 3.0f;
        fArray[162] = 16.0f;
        fArray[163] = 39.0f;
        float[] fArray2 = fArray;
        this.npc1 = new NpcEnemy(8705, 11, 0, 3, 3, 8.3f, 3.0f, -9.0f, 0.0f, fArray2);
        float[] fArray3 = new float[]{6.7f, 3.0f, 10.2f, 7.7f, 3.0f, 9.9f, 8.8f, 3.0f, 9.6f, 9.8f, 3.0f, 9.9f, 10.5f, 3.0f, 10.5f, 11.7f, 3.0f, 11.2f, 11.9f, 3.0f, 12.5f, 11.4f, 3.0f, 14.2f, 10.9f, 3.0f, 15.4f, 9.9f, 3.0f, 16.0f, 8.5f, 3.0f, 16.8f, 7.0f, 3.0f, 15.8f, 6.0f, 3.0f, 15.3f, 5.7f, 3.0f, 14.0f, 5.5f, 3.0f, 13.0f, 6.0f, 3.0f, 12.0f};
        this.npc1.setParams(fArray3);
        this.npc1.enableDTKFlag(262144);
        this.npc1.disableDTKFlag(131072);
        this.npc1.setVisible(3, false);
        this.npc1.dispRadar(false);
        this.npc1.disableDTKFlag(4);
        this.npc1.disableDTKFlag(2);
        this.npc1.disableDTKFlag(1);
        this.npc1.setShadow(0, 16);
        this.dummy = new Obj();
        this.dummy.init(24663, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy.setParent(this.npc1, 60);
        this.dummy.setTranslate(0.21f, -0.21f, 0.01f);
        this.dummy.setRotate(120.0f, 0.0f, 0.0f);
        this.dummy.setScale(1.0f, 1.0f, 1.0f);
        this.dummy.setVisible(false);
        this.npc2 = new NpcEnemy(8705, 12, 0, 3, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.npc2.setVisible(3, false);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 61);
        this.npc2.dispRadar(false);
        this.npc2.kickEnepc(4, 2);
        this.npc2.setVisible(false);
        this.npc2.setShadow(0, 16);
        this.dummy2 = new Obj();
        this.dummy2.init(24663, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dummy2.setParent(this.npc2, 60);
        this.dummy2.setTranslate(0.21f, -0.21f, 0.01f);
        this.dummy2.setRotate(120.0f, 0.0f, 0.0f);
        this.dummy2.setScale(1.0f, 1.0f, 1.0f);
        this.dummy2.setVisible(false);
        float[] fArray4 = new float[12];
        fArray4[0] = -0.81f;
        fArray4[1] = 2.5f;
        fArray4[2] = 1.01f;
        fArray4[3] = -1.0f;
        fArray4[4] = -1.81f;
        fArray4[5] = 2.5f;
        fArray4[6] = 1.01f;
        fArray4[8] = 1.81f;
        fArray4[9] = 2.5f;
        fArray4[10] = 1.01f;
        float[] fArray5 = fArray4;
        this.enemy1 = new NpcEnemy(16388, 1, 1, 24, 5, -0.81f, 2.5f, 1.01f, 0.0f, fArray5);
        this.enemy1.setGroup(0, 0, 1, 1);
        float[] fArray6 = new float[12];
        fArray6[0] = -2.3f;
        fArray6[1] = 2.5f;
        fArray6[2] = 16.2f;
        fArray6[3] = -1.0f;
        fArray6[4] = -2.3f;
        fArray6[5] = 2.5f;
        fArray6[6] = 15.2f;
        fArray6[8] = -2.3f;
        fArray6[9] = 2.5f;
        fArray6[10] = 17.2f;
        float[] fArray7 = fArray6;
        this.enemy2 = new NpcEnemy(16390, 2, 2, 19, 7, -2.3f, 2.5f, 16.2f, 225.0f, fArray7);
        this.enemy2.setGroup(2, 2, 2, 2);
        if (Runtime.getFlags(8080, 1) == 0) {
            float[] fArray8 = new float[12];
            fArray8[0] = -8.4f;
            fArray8[1] = 0.1f;
            fArray8[2] = 2.8f;
            fArray8[3] = -1.0f;
            fArray8[4] = -9.4f;
            fArray8[5] = 0.1f;
            fArray8[6] = 2.8f;
            fArray8[8] = -7.4f;
            fArray8[9] = 0.1f;
            fArray8[10] = 2.8f;
            float[] fArray9 = fArray8;
            this.enemy3 = new NpcEnemy(16388, 3, 3, 24, 5, -8.4f, 0.1f, 2.8f, 90.0f, fArray9);
            this.enemy3.setGroup(0, 0, 1, 1);
        }
        this.item1 = new Uwamono(28677, 7.1f, 0.0f, -9.0f, 180.0f, 412);
        this.item1.SetCallNo(1);
        this.item1.SetSymbol(28686);
        this.C1 = new Uwamono(28672, 15.8f, -10.5f, 19.7f, -30.0f);
        this.C1.SetSize(0.2f, 3.0f, 8.0f);
        this.C2 = new Uwamono(28672, 1.8f, -10.5f, 14.2f, 15.0f);
        this.C2.SetSize(0.2f, 3.0f, 15.0f);
        this.item4 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 296);
        if (Runtime.getFlags(8060, 1) == 1) {
            this.hasi.setRotate(-90.0f, 0.0f, 0.0f);
            Stage.setVisible(58, false);
            this.player.setID(2);
        } else {
            this.winch = new Uwamono(58, 1);
            this.winch.SetCallNo(1);
        }
        new Uwamono(17, 56, this.item4);
        if (Runtime.getFlags(8062, 1) == 1) {
            Stage.setVisible(18, false);
        }
        this.doorA = new Uwamono(7, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorA.SetDoorSpd(5);
        if (Runtime.getFlags(8059, 1) == 1) {
            this.npc1.enableDTKFlag(65536);
            this.npc1.talkto("TalkNPC1");
            this.doorA.SetDoorType('\u0004');
            this.npc1.setInvalidID(1);
            this.npc1.setMotion(0, 34);
            this.npc1.setTranslate(11.8f, 0.6f, -9.5f);
            this.npc1.setRotate(0.0f, 135.0f, 0.0f);
        } else {
            Stage.setVisible(25, false);
            Stage.setVisible(24, false);
            Stage.setVisible(27, false);
            Stage.setVisible(26, false);
            this.tabako.disp(false);
        }
        if (Runtime.getFlags(8062, 1) == 0) {
            this.dummy2.setVisible(false);
            this.npc1.setVisible(false);
            this.doorA.DoorOpen();
        }
        this.hasi.start(1, "idle");
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                System.println("ITEM:1");
                Runtime.setFlags(3223, 1, 1);
                break;
            }
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2510.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2510.waitPage(this.win, 64);
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

    class Obj
            extends Unit {
        Obj() {
        }
    }

    class NpcEnemy
            extends Enepc {
        NpcEnemy(int n, int n2, int n3, int n4, int n5) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            boolean bl = false;
            while (true) {
                if (ST2510.this.winch_move) {
                    if (!bl) {
                        if (f4 >= -90.0f) {
                            f4 = -(f * f) / 20.0f;
                            ST2510.this.hasi.setRotate(f4, 0.0f, 0.0f);
                        }
                        if (f4 <= -90.0f) {
                            f = -10.0f;
                            bl = true;
                        }
                    } else {
                        f4 = -81.0f - f * f / 10.0f;
                        ST2510.this.hasi.setRotate(f4, 0.0f, 0.0f);
                        if (f4 <= -90.0f) {
                            ST2510.this.hasi.setRotate(-90.0f, 0.0f, 0.0f);
                            ST2510.this.player.setID(2);
                            ST2510.this.Ene_Restart();
                            ST2510.this.winch_move = false;
                        }
                    }
                    if (f == 40.0f) {
                        Sound.effectPlay(196741);
                    }
                    f += 1.0f;
                }
                ST2510.this.player.getTranslate();
                if (ST2510.this.player.pz > -18.0f) {
                    if (!ST2510.this.player_ichi) {
                        ST2510.this.mokumoku.disp(true);
                    }
                    ST2510.this.player_ichi = true;
                } else {
                    if (ST2510.this.player_ichi) {
                        ST2510.this.mokumoku.disp(false);
                    }
                    ST2510.this.player_ichi = false;
                }
                if (ST2510.this.game_start) {
                    if (f3 % 20.0f == 0.0f) {
                        ST2510.this.light06.disp(false);
                        ST2510.this.light06.clearEffect();
                        ST2510.this.npc1.getTranslate();
                        ST2510.this.light06.setTranslate(ST2510.this.npc1.px, ST2510.this.npc1.py - 4.0f, ST2510.this.npc1.pz);
                        ST2510.this.light06.disp(true);
                    }
                    if (f3 % 20.0f - 5.0f == 0.0f) {
                        ST2510.this.light07.disp(false);
                        ST2510.this.light07.clearEffect();
                        ST2510.this.npc1.getTranslate();
                        ST2510.this.light07.setTranslate(ST2510.this.npc1.px, ST2510.this.npc1.py - 4.0f, ST2510.this.npc1.pz);
                        ST2510.this.light07.disp(true);
                    }
                    if (f3 % 20.0f - 10.0f == 0.0f) {
                        ST2510.this.light08.disp(false);
                        ST2510.this.light08.clearEffect();
                        ST2510.this.npc1.getTranslate();
                        ST2510.this.light08.setTranslate(ST2510.this.npc1.px, ST2510.this.npc1.py - 4.0f, ST2510.this.npc1.pz);
                        ST2510.this.light08.disp(true);
                    }
                    if (f3 % 20.0f - 15.0f == 0.0f) {
                        ST2510.this.light09.disp(false);
                        ST2510.this.light09.clearEffect();
                        ST2510.this.npc1.getTranslate();
                        ST2510.this.light09.setTranslate(ST2510.this.npc1.px, ST2510.this.npc1.py - 4.0f, ST2510.this.npc1.pz);
                        ST2510.this.light09.disp(true);
                    }
                    f3 += 1.0f;
                    Runtime.setMenuLock();
                } else {
                    ST2510.this.light06.disp(false);
                    ST2510.this.light07.disp(false);
                    ST2510.this.light08.disp(false);
                    ST2510.this.light09.disp(false);
                }
                if (ST2510.this.u_return) {
                    f2 += 1.0f;
                }
                System.sleep(1);
            }
        }
    }
}

