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
import xeno.map.MC_PRO04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0840
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO04_PRJ {
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
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Unit car1;
    Unit car2;
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
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono k1;
    Uwamono k2;
    Uwamono k3;
    Uwamono k4;
    Uwamono k5;
    Uwamono k6;
    Uwamono k7;
    Uwamono tA;
    Uwamono IA;
    Uwamono tB;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono bgm;
    MAPUnit crane01;
    MAPUnit box;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect dokan01;
    Effect red01;
    Effect fade;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    boolean crane = false;
    int momo;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] cr = new String[]{"There is a switch to operate the crane. Press it?", "/[waitkey(64)]/[close()]"};
    String[] momo1 = new String[]{"/[label(MOMO)]", "The Master Key should be somewhere in this sector.", "/[waitkey(1)]/[clear()]", "The soldiers always seem to come from the Monitoring Room up ahead. The Master Key is probably in there.", "/[waitkey(64)]/[close()]"};
    String[] kagi = new String[]{"It's locked. You need the Master Key to open it.", "/[waitkey(64)]/[close()]"};
    String[] kagiopen = new String[]{"Used the Master Key.", "/[waitkey(64)]/[close()]"};
    String[] annai = new String[]{"'←Shrine Ruins\n", " →Solitary Confinement/Monitoring Room'", "/[waitkey(64)]/[close()]"};
    String[] sekiban = new String[]{"There is a tombstone. It seems to be a relic from when this base was a shrine.", "/[waitkey(64)]/[close()]"};
    String[] yakkai = new String[]{"/[label(Ziggurat 8)]", "Hmm, that A.G.W.S. looks formidable. I'd like to avoid fighting if possible.", "/[waitkey(1)]/[clear()]", "I wonder if I could distract him?", "/[waitkey(64)]/[close()]"};
    String[] npc4_1 = new String[]{"/[label(Soldier)]", "...", "/[waitkey(64)]/[close()]"};
    String[] momo2 = new String[]{"/[label(MOMO)]", "If we get into a fight, I will support you from the rear row. I don't want to get in your way up in front.", "/[waitkey(1)]/[clear()]", "Plus, if I am directly behind you, I can prevent the enemy from pushing you back to the rear row with certain attacks like \"Back Toss.\"", "/[waitkey(64)]/[close()]"};
    String[] z81 = new String[]{"/[label(Ziggurat 8)]", "Affirmative.", "/[waitkey(64)]/[close()]"};
    String[] z82 = new String[]{"/[label(Ziggurat 8)]", "But if I guard myself against attacks like \"Back Toss,\" I can prevent that from happening. Don't concern yourself with me. Concentrate on yourself.", "/[waitkey(1)]/[clear()]", "We must hurry. Let's go!", "/[waitkey(64)]/[close()]"};

    ST0840() {
    }

    void EOB(int n) {
        if (Runtime.getFlags(107, 1) == 1) {
            Sound.effectPlay(196748);
        }
        System.println("EOB****************************************************");
        if (n == 1) {
            System.sleep(1);
            Runtime.setFlags(8026, 1, 1);
            if (Runtime.getFlags(3224, 1) == 0) {
                Sound.effectPlay(6);
                Runtime.addItemWin(10, 19);
                Runtime.setFlags(3224, 1, 1);
            }
        }
    }

    void EOB_Escape(int n) {
        System.println("EOB_Escape");
        if (Runtime.getFlags(107, 1) == 1) {
            Sound.effectPlay(196748);
        }
        if (n == 1) {
            this.enemy1.kickEnepc(7, 8);
        }
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (this.lo == 1) {
            return;
        }
        switch (n2) {
            case 0: {
                if (!this.crane) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.cr, 0);
                    this.lo = 1;
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.crane01.start(1, "kaiten");
                            this.cam0.setMode(0);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                return;
            }
            case 1: {
                if (Runtime.getFlags(105, 1) != 0) break;
                Runtime.enable(262144);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("イベント2004A:モモと守衛の会話");
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(105, 1, 1);
                Runtime.jumpEvent(2040);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
                break;
            }
            case 3: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                if (this.momo >= 1) {
                    if (Runtime.getFlags(8033, 1) == 0) {
                        Runtime.setFlags(8033, 1, 1);
                        this.light02.disp(false);
                        this.light03.disp(true);
                        this.nwin(this.kagiopen);
                        this.doorB.SetDoorType('\u0004');
                    }
                } else {
                    this.nwin(this.kagi);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 4: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                if (this.momo >= 1) {
                    if (Runtime.getFlags(8032, 1) == 0) {
                        Runtime.setFlags(8032, 1, 1);
                        this.light04.disp(false);
                        this.light05.disp(true);
                        this.nwin(this.kagiopen);
                        this.doorC.SetDoorType('\u0004');
                    }
                } else {
                    this.nwin(this.kagi);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 5: {
                if (Runtime.getFlags(106, 1) != 1 || Runtime.getFlags(107, 1) != 0) break;
                if (this.momo >= 1) {
                    this.lo = 1;
                    if (Runtime.getFlags(8050, 1) == 0) {
                        Runtime.setFlags(8050, 1, 1);
                        Runtime.setPlayerControl(false);
                        this.light06.disp(false);
                        this.light07.disp(true);
                        this.nwin(this.kagiopen);
                        Runtime.setPlayerControl(true);
                    } else {
                        Runtime.setPlayerControl(false);
                        this.doorD.DoorOpen();
                        System.sleep(30);
                        System.println("イベント2004C:モモと合流");
                        Runtime.setFriend(4);
                        Runtime.setPartyData(0x1010000, 5);
                        Runtime.setPartyData(65538, 2);
                        Runtime.setPartyData(0x1010004, 6);
                        Runtime.setPartyData(65542, 6);
                        Runtime.setPartyData(0x1010008, 0);
                        Runtime.setPartyData(65546, 0);
                        Runtime.setPartyData(16777260, 6);
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.setFlags(107, 1, 1);
                        Runtime.jumpEvent(2042);
                        Runtime.setPlayerControl(true);
                    }
                    this.lo = 0;
                    break;
                }
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("早く鍵を取って来い");
                this.nwin(this.momo1);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 6: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("案内");
                this.nwin(this.annai);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 7: {
                this.enemy2.kickEnepc(4, 2);
                this.enemy3.kickEnepc(4, 2);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("石版");
                this.nwin(this.sekiban);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                this.enemy2.kickEnepc(4, 0);
                this.enemy3.kickEnepc(4, 0);
                break;
            }
            case 8: {
                if (Runtime.getFlags(8046, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("厄介な奴がいるぞ");
                this.nwin(this.yakkai);
                Runtime.setFlags(8046, 1, 1);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void TalkNPC1(Enepc enepc, Window window) {
    }

    void TalkNPC4(Enepc enepc) {
        this.nwin(this.npc4_1);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(107, 1) == 1) {
                    if (Runtime.getFlags(110, 1) == 0) {
                        Runtime.jumpCF(831, 2);
                        break;
                    }
                    Runtime.jumpCF(830, 2);
                    break;
                }
                Runtime.jumpCF(830, 2);
                break;
            }
            case 1: {
                if (Runtime.getFlags(107, 1) == 1) {
                    Runtime.jumpCF(851, 1);
                    break;
                }
                Runtime.jumpCF(850, 1);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        float[] fArray2;
        int n;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, -13.76f, 9.0f, -3.32f, 0.0f);
        this.teiten1.SetBgm(196618);
        this.teiten2 = new Uwamono(28690, 3.5f, 5.0f, -9.0f, 0.0f);
        this.teiten2.SetBgm(196619);
        this.teiten3 = new Uwamono(28690, 8.0f, 5.0f, -9.0f, 0.0f);
        this.teiten3.SetBgm(196619);
        this.teiten4 = new Uwamono(28690, 12.0f, 5.0f, -9.0f, 0.0f);
        this.teiten4.SetBgm(196619);
        this.teiten5 = new Uwamono(28690, 5.724f, 0.0f, 0.285f, 0.0f);
        this.teiten5.SetBgm(196620);
        this.teiten6 = new Uwamono(28690, 9.724f, 0.0f, 0.285f, 0.0f);
        this.teiten6.SetBgm(196620);
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(3, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightCol(4, 1, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightCol(4, 2, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightCol(4, 3, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        this.crane01 = new Mapunits();
        this.crane01.mapUnit(159);
        this.crane01.start(4, null);
        this.red01 = new Effect(1551, 0.0f, 0.0f, 0.0f, 0.0f);
        this.red01.disp(false);
        if (Runtime.getFlags(107, 1) == 1) {
            this.red01.disp(true);
            Sound.effectPlay(196748);
        }
        this.dokan01 = new Effect(605, -14.4f, 0.0f, 2.7f, 0.0f);
        this.dokan01.disp(false);
        this.light01 = new Effect(1020, -15.3f, 10.05f, -4.95f, 90.0f);
        this.light01.setRotate(90.0f, -60.0f, 0.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1020, 4.94f, 6.29f, -6.0f, 0.0f);
        this.light02.setRotate(0.0f, 0.0f, 0.0f);
        this.light02.setScale(0.5f, 1.0f, 1.0f);
        this.light02.disp(true);
        this.light03 = new Effect(1055, 4.94f, 6.19f, -6.0f, 0.0f);
        this.light03.setRotate(0.0f, 0.0f, 0.0f);
        this.light03.setScale(0.5f, 1.0f, 1.0f);
        this.light03.disp(false);
        this.light04 = new Effect(1020, 9.37f, 6.29f, -6.0f, 0.0f);
        this.light04.setRotate(0.0f, 0.0f, 0.0f);
        this.light04.setScale(0.5f, 1.0f, 1.0f);
        this.light04.disp(true);
        this.light05 = new Effect(1055, 9.37f, 6.19f, -6.0f, 0.0f);
        this.light05.setRotate(0.0f, 0.0f, 0.0f);
        this.light05.setScale(0.5f, 1.0f, 1.0f);
        this.light05.disp(false);
        this.light06 = new Effect(1020, 13.69f, 6.29f, -6.0f, 0.0f);
        this.light06.setRotate(0.0f, 0.0f, 0.0f);
        this.light06.setScale(0.5f, 1.0f, 1.0f);
        this.light06.disp(true);
        this.light07 = new Effect(1055, 13.69f, 6.19f, -6.0f, 0.0f);
        this.light07.setRotate(0.0f, 0.0f, 0.0f);
        this.light07.setScale(0.5f, 1.0f, 1.0f);
        this.light07.disp(false);
        if (Runtime.getFlags(107, 1) == 1) {
            this.light06.disp(false);
            this.light07.disp(true);
        }
        if ((n = Runtime.getEntrance()) >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(155, false);
        Stage.setVisible(113, false);
        Stage.setVisible(145, false);
        Stage.setVisible(152, false);
        Stage.setVisible(146, false);
        Stage.setVisible(114, false);
        Stage.setVisible(153, false);
        Stage.setVisible(154, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 6.5f, 42.5f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFPedestal(4, -7.8776965f, 19.063416f, 5.0534835f, 47.039658f, -51.3813f, 19.764612f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(5, 0.03f, 0.03f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(7, 0.015f, 0.015f);
        this.cam0.setCFPedestal(8, 3.6961403f, 10.523862f, -7.5947003f, 61.316906f, -77.19061f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFPedestal(9, 7.8589134f, 10.875857f, -7.4535127f, 57.15763f, -76.53005f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFPedestal(10, 12.161203f, 11.227599f, -8.074023f, 56.516815f, -83.207825f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(11, 100.0f, 100.0f);
        this.cam0.setCFAngle(12, -28.0f, 25.0f, 0.0f, 15.0f, 42.5f);
        this.cam0.setCFHokan(12, 0.03f, 0.03f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 9.0f, 42.5f);
        this.cam0.setCFHokan(13, 0.015f, 0.015f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(14, 100.0f, 100.0f);
        this.cam0.setCFAngle(15, -28.0f, 0.0f, 0.0f, 8.0f, 42.5f);
        this.cam0.setCFHokan(15, 100.0f, 100.0f);
        if (Runtime.getFlags(107, 1) == 0) {
            this.npc1 = new NPC_NORMAL(4, 11, 0, 7, 4, 12.6f, 5.0f, -7.2f, 0.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.enableDTKFlag(65536);
            this.npc1.setShadow(0, 0);
        }
        if (Runtime.getFlags(8021, 1) == 1) {
            this.enemy1 = new Enepc();
            this.enemy1.init(17409, 8, -5.4f, 5.0f, -5.2f, 90.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            float[] fArray3 = new float[12];
            fArray3[0] = -5.4f;
            fArray3[1] = 5.0f;
            fArray3[2] = -5.2f;
            fArray3[3] = -1.0f;
            fArray3[4] = -2.2f;
            fArray3[5] = 5.0f;
            fArray3[6] = -5.2f;
            fArray3[8] = -3.3f;
            fArray3[9] = 5.0f;
            fArray3[10] = -5.2f;
            fArray3[11] = 1.0f;
            fArray2 = fArray3;
            this.enemy1.setParams(1, 8, 1, 8, fArray2);
            this.enemy1.setTranslate(-10.2f, 5.0f, -0.1f);
            this.enemy1.setRotate(0.0f, 180.0f, 0.0f);
        } else {
            this.enemy1 = new Enepc();
            this.enemy1.init(17409, 8, -5.4f, 5.0f, -5.2f, 90.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            float[] fArray4 = new float[12];
            fArray4[0] = -5.4f;
            fArray4[1] = 5.0f;
            fArray4[2] = -5.2f;
            fArray4[3] = -1.0f;
            fArray4[4] = -2.2f;
            fArray4[5] = 5.0f;
            fArray4[6] = -5.2f;
            fArray4[8] = -3.3f;
            fArray4[9] = 5.0f;
            fArray4[10] = -5.2f;
            fArray4[11] = 1.0f;
            fArray2 = fArray4;
            this.enemy1.setParams(1, 10, 1, 8, fArray2);
            fArray = new float[]{-5.4f, 5.0f, -5.2f, -7.4f, 5.0f, -5.2f, -4.4f, 5.0f, -5.2f};
            this.enemy1.setParams(fArray);
        }
        this.enemy1.setVisible(15, false);
        this.enemy2 = new Enepc();
        this.enemy2.init(17153, 6, -8.2f, 0.0f, 2.9f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(2, 2, 2, 2);
        float[] fArray5 = new float[60];
        fArray5[0] = -8.2f;
        fArray5[2] = 2.9f;
        fArray5[3] = 1.0f;
        fArray5[4] = -9.25f;
        fArray5[6] = 7.28f;
        fArray5[7] = 2.0f;
        fArray5[8] = -10.99f;
        fArray5[10] = 7.43f;
        fArray5[11] = 3.0f;
        fArray5[12] = -13.27f;
        fArray5[14] = 7.43f;
        fArray5[15] = 4.0f;
        fArray5[16] = -13.27f;
        fArray5[18] = 4.92f;
        fArray5[19] = 5.0f;
        fArray5[20] = -13.27f;
        fArray5[22] = 2.52f;
        fArray5[23] = 6.0f;
        fArray5[24] = -11.2f;
        fArray5[26] = 2.52f;
        fArray5[27] = 7.0f;
        fArray5[28] = -9.2f;
        fArray5[30] = 2.9f;
        fArray5[31] = 8.0f;
        fArray5[32] = -9.2f;
        fArray5[34] = 2.9f;
        fArray5[35] = -1.0f;
        fArray5[36] = -6.6f;
        fArray5[38] = 2.9f;
        fArray5[39] = 8.0f;
        fArray5[40] = -6.6f;
        fArray5[42] = -1.8f;
        fArray5[43] = 9.0f;
        fArray5[44] = 8.0f;
        fArray5[46] = 8.0f;
        fArray5[47] = 1.0f;
        fArray5[48] = 13.7f;
        fArray5[50] = 8.0f;
        fArray5[51] = 11.0f;
        fArray5[52] = 8.0f;
        fArray5[54] = 1.6f;
        fArray5[55] = 9.0f;
        fArray5[56] = 13.7f;
        fArray5[58] = 1.6f;
        fArray5[59] = 13.0f;
        fArray2 = fArray5;
        this.enemy2.setParams(0, 9, 2, 6, fArray2);
        float[] fArray6 = new float[12];
        fArray6[0] = -8.2f;
        fArray6[2] = 2.9f;
        fArray6[3] = -8.25f;
        fArray6[5] = 7.28f;
        fArray6[6] = -12.7f;
        fArray6[8] = 7.43f;
        fArray6[9] = -12.7f;
        fArray6[11] = 2.52f;
        fArray = fArray6;
        this.enemy2.setParams(fArray);
        this.enemy3 = new Enepc();
        this.enemy3.init(16643, 11, 7.5f, 0.0f, 2.2f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(1, 1, 1, 1);
        float[] fArray7 = new float[72];
        fArray7[0] = 8.5f;
        fArray7[2] = 2.2f;
        fArray7[3] = 1.0f;
        fArray7[4] = 6.0f;
        fArray7[6] = 2.2f;
        fArray7[7] = 2.0f;
        fArray7[8] = 4.0f;
        fArray7[10] = 2.2f;
        fArray7[11] = 3.0f;
        fArray7[12] = 2.0f;
        fArray7[14] = 2.2f;
        fArray7[15] = 4.0f;
        fArray7[18] = 2.2f;
        fArray7[19] = 5.0f;
        fArray7[20] = -2.0f;
        fArray7[22] = 2.2f;
        fArray7[23] = 6.0f;
        fArray7[24] = -4.5f;
        fArray7[26] = 2.2f;
        fArray7[27] = 7.0f;
        fArray7[28] = -4.5f;
        fArray7[30] = 5.0f;
        fArray7[31] = 8.0f;
        fArray7[32] = -4.5f;
        fArray7[34] = 8.0f;
        fArray7[35] = 9.0f;
        fArray7[36] = -2.5f;
        fArray7[38] = 8.0f;
        fArray7[39] = 10.0f;
        fArray7[42] = 8.0f;
        fArray7[43] = 11.0f;
        fArray7[44] = 2.5f;
        fArray7[46] = 8.0f;
        fArray7[47] = 12.0f;
        fArray7[48] = 4.0f;
        fArray7[50] = 8.0f;
        fArray7[51] = 13.0f;
        fArray7[52] = 6.5f;
        fArray7[54] = 8.0f;
        fArray7[55] = 14.0f;
        fArray7[56] = 8.5f;
        fArray7[58] = 8.0f;
        fArray7[59] = 15.0f;
        fArray7[60] = 8.5f;
        fArray7[62] = 5.0f;
        fArray7[63] = -1.0f;
        fArray7[64] = -13.5f;
        fArray7[66] = 8.0f;
        fArray7[67] = 8.0f;
        fArray7[68] = 13.2f;
        fArray7[70] = 8.0f;
        fArray7[71] = 14.0f;
        float[] fArray8 = fArray7;
        this.enemy3.setParams(2, 10, 3, 11, fArray8);
        float[] fArray9 = new float[48];
        fArray9[0] = 8.5f;
        fArray9[2] = 2.2f;
        fArray9[3] = 6.0f;
        fArray9[5] = 2.2f;
        fArray9[6] = 4.0f;
        fArray9[8] = 2.2f;
        fArray9[9] = 2.0f;
        fArray9[11] = 2.2f;
        fArray9[14] = 2.2f;
        fArray9[15] = -2.0f;
        fArray9[17] = 2.2f;
        fArray9[18] = -4.5f;
        fArray9[20] = 2.2f;
        fArray9[21] = -4.5f;
        fArray9[23] = 5.0f;
        fArray9[24] = -4.5f;
        fArray9[26] = 8.0f;
        fArray9[27] = -2.5f;
        fArray9[29] = 8.0f;
        fArray9[32] = 8.0f;
        fArray9[33] = 2.5f;
        fArray9[35] = 8.0f;
        fArray9[36] = 4.0f;
        fArray9[38] = 8.0f;
        fArray9[39] = 6.5f;
        fArray9[41] = 8.0f;
        fArray9[42] = 8.5f;
        fArray9[44] = 8.0f;
        fArray9[45] = 8.5f;
        fArray9[47] = 5.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(fArray10);
        this.enemy4 = new Enepc();
        this.enemy4.init(778, 12, 13.8f, 5.0f, -5.8f, 0.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(2, 2, 2, 2);
        this.enemy4.setParams(0, 11, 4, 12);
        this.enemy4.setMotion(0, 6);
        this.enemy4.setInvalidID(1);
        this.enemy4.talkto("TalkNPC4");
        this.enemy4.disableDTKFlag(4);
        this.enemy4.disableDTKFlag(2);
        this.enemy4.disableDTKFlag(1);
        this.enemy4.disableDTKFlag(131072);
        this.enemy4.enableDTKFlag(65536);
        this.enemy4.dispRadar(false);
        this.enemy5 = new Enepc();
        this.enemy5.init(17153, 12, 10.6f, 5.0f, -4.5f, 30.0f);
        this.enemy5.id = 5;
        this.enemy5.setGroup(2, 2, 2, 2);
        this.enemy5.setParams(0, 11, 5, 12);
        this.enemy5.setMotion(0, 7);
        this.enemy5.setInvalidID(1);
        this.enemy5.talkto("TalkNPC4");
        this.enemy5.disableDTKFlag(4);
        this.enemy5.disableDTKFlag(2);
        this.enemy5.disableDTKFlag(1);
        this.enemy5.disableDTKFlag(131072);
        this.enemy5.enableDTKFlag(65536);
        this.enemy5.dispRadar(false);
        new Uwamono(28678, 8.0f, 5.0f, -9.9f);
        this.car1 = new Unit();
        this.car1.init(20553, 11.5f, 0.0f, 4.5f, 0.0f);
        this.item1 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 45);
        this.item2 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 46);
        this.item3 = new Uwamono(28681, -12.6f, 5.0f, -1.0f, 0.0f, 47);
        this.IA = new Uwamono(28677, 0.0f, -900.0f, 0.0f, 180.0f, 54);
        this.IA.SetSymbol(28683);
        new Uwamono(16, 0, this.item2);
        new Uwamono(38, 0, this.item1);
        this.k7 = new Uwamono(17, 0, this.item3);
        this.tA = new Uwamono(28672, -10.2f, 3.5f, -1.9f, 0.0f);
        this.tA.SetHitKind('\u0001');
        this.tA.SetSize(1.5f, 2.0f, 1.5f);
        this.tB = new Uwamono(28672, -15.0f, 3.5f, 0.2f, 0.0f);
        this.tB.SetHitKind('\u0001');
        this.tB.SetSize(1.5f, 2.0f, 1.5f);
        new Uwamono(28673, -7.7f, 0.0f, 2.3f, 0.0f);
        new Uwamono(28673, 15.0f, 0.0f, 6.1f, 0.0f);
        this.doorA = new Uwamono(112, 42, '\u0001');
        new Uwamono(111, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(156, 40, '\u0001');
        this.doorC = new Uwamono(157, 40, '\u0001');
        this.doorD = new Uwamono(158, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.momo = Runtime.checkItem(10, 2);
        if (Runtime.getFlags(8050, 1) == 0) {
            this.doorD.SetDoorType('\u0002');
        } else {
            this.doorD.SetDoorType('\u0004');
            this.light06.disp(false);
            this.light07.disp(true);
        }
        this.enemy2.kickEnepc(19, 1, 0, 420, 1);
        if (Runtime.getFlags(8021, 1) == 1) {
            this.crane01.setRotateY(-85.0f);
            this.light01.disp(false);
            this.crane = true;
            Stage.setVisible(10, false);
            Stage.setVisible(89, false);
            Stage.setVisible(90, false);
            Stage.setVisible(91, false);
            Stage.setVisible(19, false);
            this.IA.setTranslate(-14.4f, 0.0f, 2.7f);
            this.tA.setTranslate(-10.2f, 0.0f, -1.9f);
            this.enemy1.setTranslate(-10.2f, 5.0f, -0.1f);
            this.enemy1.setRotate(0.0f, 180.0f, 0.0f);
        } else {
            this.k1 = new Uwamono(10, 4);
            this.k2 = new Uwamono(89, 4);
            this.k3 = new Uwamono(90, 4);
            this.k4 = new Uwamono(91, 4);
            this.k6 = new Uwamono(19, 4);
            this.k1.SetBroken(false);
            this.k2.SetBroken(false);
            this.k3.SetBroken(false);
            this.k4.SetBroken(false);
            this.k6.SetItem(this.IA);
        }
        if (Runtime.getFlags(8026, 1) == 1) {
            this.enemy1.kickEnepc(4, 2);
            this.enemy1.setVisible(false);
        }
        if (Runtime.getFlags(8033, 1) == 1) {
            this.light02.disp(false);
            this.light03.disp(true);
            this.doorB.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(8032, 1) == 1) {
            this.light04.disp(false);
            this.light05.disp(true);
            this.doorC.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(8112, 1) == 0 && Runtime.getFlags(107, 1) == 1) {
            this.npc2 = new NPC_NORMAL(4, 13, 0, 7, 25, 13.0f, 5.0f, -5.4f, 270.0f);
            this.npc2.disableDTKFlag(65536);
            this.npc3 = new NPC_NORMAL(6, 12, 0, 0, 24, 12.0f, 5.0f, -5.4f, 90.0f);
            this.npc3.disableDTKFlag(65536);
            this.crane01.start(1, "goryu");
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST0840.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST0840.waitPage(this.win, 64);
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }
    }

    class NPC_EVENT
            extends Enepc {
        NPC_EVENT() {
        }

        void init() {
        }

        public void talk() {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void goryu() {
            Runtime.setFlags(8112, 1, 1);
            ST0840.this.npc2.kickEnepc(4, 1);
            ST0840.this.npc3.kickEnepc(4, 1);
            ST0840.this.npc2.look_char(ST0840.this.npc3);
            ST0840.this.npc3.look_char(ST0840.this.npc2);
            Runtime.setPlayerControl(false);
            ST0840.this.player.setVisible(false);
            ST0840.this.cam0.setMode(-1);
            ST0840.this.camEV = Camera.create(1);
            ST0840.this.camEV.setRotate(-23.5f, -58.2f, 0.0f);
            ST0840.this.camEV.setTranslate(9.0f, 7.8f, -3.1f);
            ST0840.this.camEV.setFov(29.3f);
            ST0840.this.camEV.change();
            System.sleep(5);
            ST0840.this.npc2.kickEnepc(0, 9);
            ST0840.this.nwin(ST0840.this.momo2);
            ST0840.this.npc2.kickEnepc(1, 0);
            ST0840.this.npc3.kickEnepc(0, 7);
            ST0840.this.nwin(ST0840.this.z81);
            ST0840.this.npc3.kickEnepc(0, 9);
            ST0840.this.nwin(ST0840.this.z82);
            ST0840.this.npc2.kickEnepc(0, 7);
            ST0840.this.fade.call(0);
            System.sleep(27);
            ST0840.this.npc2.setVisible(false);
            ST0840.this.npc3.setVisible(false);
            ST0840.this.player.setVisible(true);
            ST0840.this.cam0.setMode(0);
            System.sleep(3);
            Runtime.setPlayerControl(true);
        }

        void kaiten() {
            ST0840.this.player.setTranslate(-14.0f, 9.0f, -5.0f);
            ST0840.this.cam0.setMode(-1);
            ST0840.this.camEV = Camera.create(1);
            ST0840.this.camEV.setTranslate(-7.5523086f, 19.063314f, 13.694325f);
            ST0840.this.camEV.setRotate(-39.547924f, 8.236943f, 0.0f);
            ST0840.this.camEV.setFov(42.879883f);
            ST0840.this.camEV.change();
            Runtime.enable(65536);
            ST0840.this.player.rotY(10, -90.0f, true);
            System.sleep(10);
            ST0840.this.player.mtn(25, 1, 1.0f, true);
            System.sleep(20);
            Sound.effectPlay(196741);
            System.sleep(20);
            int n = 0;
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            float f5 = 0.0f;
            float f6 = 0.0f;
            boolean bl = false;
            while (true) {
                if (n >= 0 && n <= 85) {
                    Runtime.setFlags(8021, 1, 1);
                    this.setRotateY(-n);
                }
                if (n == 1) {
                    ST0840.this.crane = true;
                    ST0840.this.lo = 0;
                    Sound.effectPlay(196742);
                }
                if (n == 30) {
                    ST0840.this.tA.setTranslate(-10.2f, 0.0f, -1.9f);
                    ST0840.this.k2.SendSignal();
                }
                if (n == 38) {
                    ST0840.this.enemy1.kickEnepc(4, 1);
                    ST0840.this.enemy1.moveEnepc(17, -90.0f, 0.0f, 10);
                }
                if (n == 40) {
                    ST0840.this.k1.SendSignal();
                }
                if (n == 48) {
                    ST0840.this.enemy1.kickEnepc(1, 1);
                    ST0840.this.enemy1.moveEnepc(15, -10.2f, -5.3f, 30);
                }
                if (n == 60) {
                    ST0840.this.k3.SendSignal();
                    ST0840.this.k4.SendSignal();
                    boolean bl2 = false;
                    ST0840.this.k6.getTranslate();
                    f3 = ST0840.this.k6.px;
                    f4 = ST0840.this.k6.pz;
                    f = (f3 - -14.7f) / 60.0f;
                    f2 = (f4 - 4.0f) / 60.0f;
                }
                if (n >= 60 && n <= 120) {
                    float f7 = (float) n - 60.0f;
                    f5 = (-12.8f * f7 / 30.0f + 9.0f) * f7 / 30.0f + 7.0f;
                    ST0840.this.k6.setTranslate(f3 - f * f7, f5, f4 - f2 * f7);
                    ST0840.this.k6.setRotate(f7 * 2.0f, -f7 * 2.0f, -f7 * 2.0f);
                    if (f5 <= 0.0f) {
                        ST0840.this.k6.SendSignal();
                        ST0840.this.dokan01.disp(true);
                    }
                }
                if (n == 95) {
                    ST0840.this.enemy2.kickEnepc(4, 1);
                    ST0840.this.enemy2.getTranslate();
                }
                if (n >= 95 && n <= 120) {
                    float f8 = (float) n - 95.0f;
                    f6 = (-12.8f * f8 / 30.0f + 7.0f) * f8 / 30.0f + 0.0f;
                    if (f6 <= 0.0f) {
                        ST0840.this.enemy2.setTranslate(ST0840.this.enemy2.px, ST0840.this.enemy2.py, ST0840.this.enemy2.pz);
                        if (!bl) {
                            ST0840.this.enemy2.kickEnepc(4, 0);
                            bl = true;
                        }
                    } else {
                        ST0840.this.enemy2.setTranslate(ST0840.this.enemy2.px, f6, ST0840.this.enemy2.pz);
                    }
                }
                if (n == 78) {
                    ST0840.this.enemy1.moveEnepc(17, 360.0f, 0.0f, 10);
                }
                if (n == 88) {
                    ST0840.this.enemy1.kickEnepc(1, 1);
                    ST0840.this.enemy1.moveEnepc(15, -10.2f, -0.1f, 60);
                }
                if (n == 148) {
                    ST0840.this.light01.disp(false);
                    ST0840.this.enemy1.moveEnepc(17, 240.0f, 0.0f, 20);
                }
                if (n == 168) {
                    ST0840.this.enemy1.moveEnepc(17, 300.0f, 0.0f, 20);
                    float[] fArray = new float[8];
                    fArray[1] = -39.547924f;
                    fArray[2] = 8.236943f;
                    fArray[4] = 60.0f;
                    fArray[5] = -55.547924f;
                    fArray[6] = 8.236943f;
                    float[] fArray2 = fArray;
                    ST0840.this.camEV.rotateSPL(fArray2, 0, 1, 60);
                }
                if (n == 278) {
                    if (Runtime.getFlags(8026, 1) == 1) {
                        System.println("ロボは死んでいる");
                        ST0840.this.enemy1.kickEnepc(4, 2);
                        ST0840.this.enemy1.setVisible(false);
                        break;
                    }
                    ST0840.this.enemy1.kickEnepc(4, 0);
                    ST0840.this.enemy1.kickEnepc(7, 8);
                    break;
                }
                ++n;
                System.sleep(1);
            }
            ST0840.this.cam0.setMode(0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }
}

