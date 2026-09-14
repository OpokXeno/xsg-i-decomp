import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_DYU17_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1870
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU17_PRJ {
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
    Unit Oyasumi;
    Unit Kidou;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int pass = 0;
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
    Uwamono saveA;
    Uwamono itembox;
    Uwamono DonotBreak;
    Effect fade;
    Effect fade1;
    Effect fade2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int page;
    String[] Player_00 = new String[]{"/[label(Shion)]", "I'm really tired now...I think I'll call it a day.", "/[waitkey(64)]/[close()]"};
    String[] Player_01 = new String[]{"/[label(Shion)]", "Pushing ourselves too hard is bad for our health. We should turn in for the night.", "/[waitkey(64)]/[close()]"};
    String[] Player_02 = new String[]{"/[label(Shion)]", "It's a large ship. I think I'll go have some more fun.", "/[waitkey(64)]/[close()]"};
    String[] Player_03 = new String[]{"/[label(Shion)]", "I was a little too cold towards Allen earlier. I better apologize to him in person.", "/[waitkey(1)]/[clear()]", "I wonder where he went?", "/[waitkey(64)]/[close()]"};
    String[] D00_00 = new String[]{"what do you wanT? you look like a little braT.", "/[waitkey(1)]/[clear()]", "if you want to rest, go to your own beD!!", "/[waitkey(64)]/[close()]"};
    String[] D00_01 = new String[]{"heY! well well, what do we have here, a customeR?", "/[waitkey(1)]/[clear()]", "i'lL watch over you, even when i'M sleepinG!", "/[waitkey(64)]/[close()]"};
    String[] D00_02 = new String[]{"you again, squirT? You're the captaiN of this shiP?", "/[waitkey(1)]/[clear()]", "you think i believe rubbish like thaT?", "/[waitkey(64)]/[close()]"};
    String[] D00_03 = new String[]{"hello therE! your friend, the little runt, is a bit of a punK!", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Get some rest?", "/[waitkey(64)]/[close()]"};
    String[] SYS_01 = new String[]{"HP & EP restored!!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1870() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (this.pass == 0) {
                        if (Runtime.getFlags(304, 1) == 1 && Runtime.getFlags(3075, 1) == 0) {
                            Runtime.setPlayerControl(false);
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(60, -6.5f, -0.5f, true);
                            System.sleep(65);
                            this.player.mtn(28, 9, 1.0f, true);
                            System.sleep(5);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.Player_00, 0);
                            System.waitFor(this.win);
                            this.menu = Menu.create();
                            this.menu.addItem("It's best to rest when you're tired\nI'll search a little while longer");
                            System.waitFor(this.menu);
                            this.selected = this.menu.getSelected();
                            switch (this.selected) {
                                case 0: {
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 305);
                                    this.win.print(this.Player_01, 0);
                                    System.waitFor(this.win);
                                    Runtime.setFlags(3075, 1, 1);
                                    this.fade.call(0);
                                    System.sleep(30);
                                    Runtime.charAllRecovery();
                                    if (Runtime.getFlags(305, 1) == 0) {
                                        Runtime.setFlags(305, 1, 1);
                                        Runtime.jumpEvent(3030);
                                    }
                                    Runtime.setPlayerControl(true);
                                    Runtime.disable(65536);
                                    this.pass = 1;
                                    break;
                                }
                                default: {
                                    this.win = Window.create();
                                    this.win.setSize(4, 45);
                                    this.win.setLocation(15, 305);
                                    this.win.print(this.Player_02, 0);
                                    System.waitFor(this.win);
                                    System.sleep(30);
                                    Runtime.setPlayerControl(true);
                                    Runtime.disable(65536);
                                    this.pass = 1;
                                    break;
                                }
                            }
                        }
                    } else {
                        this.pass = 0;
                    }
                    if (Runtime.getFlags(346, 1) != 1 || Runtime.getFlags(3169, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.mailArriveSet(69);
                    Runtime.setFlags(3169, 1, 1);
                    System.waitFor(this.win);
                    System.sleep(15);
                    Runtime.setPlayerControl(true);
                    Runtime.mailExec(1);
                    return;
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    System.println("\\\\\\\\\\\\\\\\\\\\\\");
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SYS_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.disable(524288);
                            this.fade1.call(0);
                            System.sleep(60);
                            Runtime.enable(65536);
                            this.player.rotY(1, 0.0f, true);
                            this.player.setTranslate(-5.5f, 0.6f, 0.0f);
                            Runtime.charAllRecovery();
                            if (Runtime.getFlags(304, 1) == 1 && Runtime.getFlags(305, 1) == 0) {
                                Runtime.setFlags(305, 1, 1);
                                Runtime.setFlags(3075, 1, 1);
                                Runtime.jumpEvent(3030);
                            }
                            this.fade2.call(0);
                            System.sleep(60);
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.SYS_01, 0);
                            System.waitFor(this.win);
                            Runtime.disable(65536);
                            Runtime.setPlayerControl(true);
                            Runtime.enable(524288);
                            return;
                        }
                        default: {
                            Runtime.setPlayerControl(true);
                            return;
                        }
                    }
                }
                default: {
                    return;
                }
            }
        }
        if (n2 == 2 || n2 != 3) return;
        switch (n) {
            default:
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.D00_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.D00_01, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.D00_02, 0);
            System.waitFor(window);
        } else {
            window.print(this.D00_03, 0);
            System.waitFor(window);
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1750, 4);
                break;
            }
        }
    }

    void init() {
        int n;
        if (Runtime.getFlags(346, 1) == 1 && Runtime.getFlags(3105, 1) == 0) {
            Runtime.setFlags(3105, 1, 1);
        }
        Stage.setVisible(-1, true);
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
        if (Runtime.getFlags(346, 1) == 1 && Runtime.getFlags(3136, 1) == 0) {
            this.Kidou = new Mapunits();
            this.Kidou.mapUnit(10);
            this.Kidou.start(4, null);
            Runtime.setOutFriend(4);
            System.println("モモ、抜けます！！");
            Runtime.setOutFriend(6);
            System.println("ジギー、抜けます！！");
            Runtime.setOutFriend(3);
            System.println("ケイオス、抜けます！！");
            Runtime.setOutFriend(5);
            System.println("Ｊｒ．、抜けます！！");
            Runtime.setOutFriend(2);
            System.println("コスモス、抜けます！！");
            System.println("パーティーセットをシオンだけにするぞ！！");
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 2);
            Runtime.setPartyData(0x1010004, 0);
            Runtime.setPartyData(65542, 0);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            System.println("完了！！完了！！パーティーはシオンだけになったはず！？");
            Runtime.setPartyData(16777260, 1);
            this.Kidou.start(1, "Open");
            Runtime.setFlags(3136, 1, 1);
        }
        if ((n = Runtime.getEntrance()) >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(3, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        if (Runtime.getFlags(326, 1) == 0) {
            this.npc1 = new NPC_NORMAL(1612, 11, 0, 0, 3, -7.75f, 0.6f, -4.25f, 0.0f);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(131074);
            this.npc1.enableDTKFlag(8);
            this.npc1.enableDTKFlag(4);
            this.npc1.setInvalidID(1);
        } else {
            this.npc1 = new NPC_NORMAL(1612, 11, 0, 0, 3, 0.0f, 0.0f, -1.5f, 0.0f);
            this.npc1.talkto("TalkNPC1a");
            this.npc1.disableDTKFlag(131074);
            this.npc1.enableDTKFlag(8);
            this.npc1.enableDTKFlag(4);
        }
        if (Runtime.getFlags(326, 1) == 0) {
            this.DonotBreak = new Uwamono(1, 40);
            this.DonotBreak.SetBroken(false);
        } else {
            this.DonotBreak = new Uwamono(1, 40);
        }
        new Uwamono(0, 40);
        this.doorA = new Uwamono(2, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(326, 1) != 0) {
            this.itembox = new Uwamono(28677, -7.81f, 0.6f, -6.019f, 180.0f, 457);
            this.itembox.SetSymbol(28686);
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

    class Obj
            extends Unit {
        Obj() {
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Open() {
            Runtime.setPlayerControl(false);
            Runtime.enable(65536);
            ST1870.this.player.setTranslate(-6.599f, 0.6f, -0.267f);
            ST1870.this.player.rotY(1, 20.0f, true);
            ST1870.this.player.mtn(28, 9, 1.0f, true);
            ST1870.this.win = Window.create();
            ST1870.this.win.setSize(4, 45);
            ST1870.this.win.setLocation(15, 305);
            ST1870.this.win.print(ST1870.this.Player_03, 0);
            Runtime.setFlags(310, 1, 1);
            Runtime.setFlags(3160, 1, 1);
            Runtime.setFlags(3026, 1, 0);
            Runtime.setFlags(3027, 1, 0);
            Runtime.setFlags(3038, 1, 0);
            Runtime.setFlags(3039, 1, 0);
            System.waitFor(ST1870.this.win);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }
}

