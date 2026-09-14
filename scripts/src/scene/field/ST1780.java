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
import xeno.map.MC_DYU08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1780
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU08_PRJ {
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
    Unit Doa;
    Unit Kidou;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1btalked = 0;
    int npc2talked = 0;
    int npc2btalked = 0;
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
    Uwamono saveA;
    Effect fade;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] ENG1_00 = new String[]{"Little Master! All teams have been deployed! We'll secure their route of retreat!", "/[waitkey(64)]/[close()]"};
    String[] ENG1_01 = new String[]{"I will defend the path of retreat with my life!", "/[waitkey(1)]/[clear()]", "Give 'em hell, Sir!", "/[waitkey(64)]/[close()]"};
    String[] ENG1_02 = new String[]{"This room is located at the tip of the battering ram.", "/[waitkey(1)]/[clear()]", "After ramming the enemy ship, we then charge in through that hatch.", "/[waitkey(64)]/[close()]"};
    String[] ENG1_03 = new String[]{"Structurally, the hatch leads directly outside.", "/[waitkey(1)]/[clear()]", "It's dangerous, so please stay away from the hatch.", "/[waitkey(64)]/[close()]"};
    String[] ENG1_00_3 = new String[]{"Little Master? Here to discuss another reckless tuning of your A.G.W.S.?", "/[waitkey(1)]/[clear()]", "Not possible, hmm...not possible. No way! We're the ones that get in trouble for tuning them too recklessly!!", "/[waitkey(64)]/[close()]"};
    String[] ENG1_01_3 = new String[]{"Just between you and me, I tuned it according to your last request!", "/[waitkey(1)]/[clear()]", "Just don't tell Mary and Shelley!", "/[waitkey(64)]/[close()]"};
    String[] ENG1_02_3 = new String[]{"Little Master's piloting really makes me nervous!", "/[waitkey(64)]/[close()]"};
    String[] ENG1_03_3 = new String[]{"Little Master pushes the A.G.W.S.' performance to the limit!", "/[waitkey(1)]/[clear()]", "Of course, it makes our work as maintenance crew more worthwhile!!", "/[waitkey(64)]/[close()]"};
    String[] ENG2_00 = new String[]{"The enemy forces seem to be fairly tough.", "/[waitkey(1)]/[clear()]", "Multiple A.G.W.S. have also been dispatched. Please be careful.", "/[waitkey(64)]/[close()]"};
    String[] ENG2_01 = new String[]{"Mary! Please take care of Little Master!", "/[waitkey(64)]/[close()]"};
    String[] ENG2_02 = new String[]{"What do you think of the Durandal's equipment? We're as good as any Federation battleship!", "/[waitkey(64)]/[close()]"};
    String[] ENG2_03 = new String[]{"Well, in the beginning, the Kukai Foundation was established under the leadership of the Second Miltian government as a special armed organization.", "/[waitkey(1)]/[clear()]", "So it isn't at all strange for it to have all this equipment.", "/[waitkey(64)]/[close()]"};
    String[] ENG2_00_3 = new String[]{"Little Master, please don't worry! The A.G.W.S. on this ship are undefeatable!", "/[waitkey(1)]/[clear()]", "After all, I'm the one that maintains them!", "/[waitkey(64)]/[close()]"};
    String[] ENG2_01_3 = new String[]{"Oh, Little Master? How's Mary? Well, um, that is...I was just wondering how she's been doing lately.", "/[waitkey(64)]/[close()]"};
    String[] ENG2_02_3 = new String[]{"Oh, did you see Little Master's A.G.W.S.?", "/[waitkey(1)]/[clear()]", "Of all the A.G.W.S. I've worked on, this one has gotten the most amount of attention!", "/[waitkey(64)]/[close()]"};
    String[] ENG2_03_3 = new String[]{"But Little Master's rough piloting makes me worry...", "/[waitkey(1)]/[clear()]", "What? What am I worried about? Uh, well, about Little Master, of course!!", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Go back to the U-TIC ship?", "/[waitkey(64)]/[close()]"};
    String[] JR_00 = new String[]{"Our main objective is to hack into the mainframe of the U-TIC ship.", "/[waitkey(64)]/[close()]"};
    String[] S01_00 = new String[]{"Damn, those Federation bastards did whatever they pleased! They don't know what they're doing and they messed everything up!", "/[waitkey(1)]/[clear()]", "They don't understand how they're supposed to treat things.", "/[waitkey(1)]/[clear()]", "I have to adjust everything all over again, damn it!", "/[waitkey(64)]/[close()]"};
    String[] S02_00 = new String[]{"Huh? Allen? He hasn't come by here.", "/[waitkey(1)]/[clear()]", "Maybe he went down to the Foundation?", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1780() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            if (Runtime.getFlags(6036, 1) == 0) {
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(60, -3.5f, 0.0f, true);
                System.sleep(65);
                this.player.mtn(28, 1, 1.0f, true);
                System.sleep(20);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.JR_00, 0);
                System.waitFor(this.win);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
            }
        } else if (n2 == 1) {
            switch (n) {
                default:
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getFlags(6036, 1) == 0) {
            if (Runtime.getLeader() == 5) {
                if (this.npc1btalked == 0) {
                    window.print(this.ENG1_00, 0);
                    System.waitFor(window);
                    this.npc1btalked = 1;
                } else {
                    window.print(this.ENG1_01, 0);
                    System.waitFor(window);
                    this.npc1btalked = 0;
                }
            } else if (this.npc1talked == 0) {
                window.print(this.ENG1_02, 0);
                System.waitFor(window);
                this.npc1talked = 1;
            } else {
                window.print(this.ENG1_03, 0);
                System.waitFor(window);
                this.npc1talked = 0;
            }
        } else if (Runtime.getLeader() == 5) {
            if (this.npc1btalked == 0) {
                window.print(this.ENG1_00_3, 0);
                System.waitFor(window);
                this.npc1btalked = 1;
            } else {
                window.print(this.ENG1_01_3, 0);
                System.waitFor(window);
                this.npc1btalked = 0;
            }
        } else if (this.npc1talked == 0) {
            window.print(this.ENG1_02_3, 0);
            System.waitFor(window);
            this.npc1talked = 1;
        } else {
            window.print(this.ENG1_03_3, 0);
            System.waitFor(window);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        window.print(this.S01_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getFlags(6036, 1) == 0) {
            if (Runtime.getLeader() == 5) {
                if (this.npc2btalked == 0) {
                    window.print(this.ENG2_00, 0);
                    System.waitFor(window);
                    this.npc2btalked = 1;
                } else {
                    window.print(this.ENG2_00, 0);
                    System.waitFor(window);
                    this.npc2btalked = 0;
                }
            } else if (this.npc2talked == 0) {
                window.print(this.ENG2_02, 0);
                System.waitFor(window);
                this.npc2talked = 1;
            } else {
                window.print(this.ENG2_03, 0);
                System.waitFor(window);
                this.npc2talked = 0;
            }
        } else if (Runtime.getLeader() == 5) {
            if (this.npc2btalked == 0) {
                window.print(this.ENG2_00_3, 0);
                System.waitFor(window);
                this.npc2btalked = 1;
            } else {
                window.print(this.ENG2_01_3, 0);
                System.waitFor(window);
                this.npc2btalked = 0;
            }
        } else if (this.npc2talked == 0) {
            window.print(this.ENG2_02_3, 0);
            System.waitFor(window);
            this.npc2talked = 1;
        } else {
            window.print(this.ENG2_03_3, 0);
            System.waitFor(window);
            this.npc2talked = 0;
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        window.print(this.S02_00, 0);
        System.waitFor(window);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1860, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1040, 1);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(6036, 1) == 0) {
            this.teiten1 = new Uwamono(28690, 4.0f, 1.0f, -8.0f, 0.0f);
            this.teiten1.SetBgm(196619);
        }
        this.teiten2 = new Uwamono(28690, -7.0f, 1.0f, -5.5f, 0.0f);
        this.teiten2.SetBgm(196620);
        this.teiten3 = new Uwamono(28690, -7.0f, 1.0f, -4.0f, 0.0f);
        this.teiten3.SetBgm(196620);
        this.teiten4 = new Uwamono(28690, -7.0f, 1.0f, 4.0f, 0.0f);
        this.teiten4.SetBgm(196620);
        this.teiten5 = new Uwamono(28690, -7.0f, 1.0f, 5.5f, 0.0f);
        this.teiten5.SetBgm(196620);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(6036, 1) != 0) {
            this.Doa = new Mapunits();
            this.Doa.mapUnit(42);
            this.Doa.start(4, null);
            this.Doa.setTranslate(this.Doa.px, this.Doa.py, this.Doa.pz - 4.0f);
        } else if (Runtime.getFlags(6036, 1) == 0 && Runtime.getFlags(6035, 1) == 0) {
            System.println("パーティーチェック");
            Runtime.setPartyData(16777260, 5);
            Runtime.setPartyData(0x1010000, 7);
            Runtime.setPartyData(65538, 2);
            Runtime.setFriend(288);
            Runtime.setPartyData(0x1010004, 20);
            Runtime.setPartyData(65542, 7);
            Runtime.setFriend(289);
            Runtime.setPartyData(0x1010008, 19);
            Runtime.setPartyData(65546, 9);
            Runtime.setFlags(6035, 1, 1);
        }
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
        Stage.setVisible(25, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.saveA = new Uwamono(28678, -1.0f, 0.0f, 0.5f);
        this.npc1 = new NPC_NORMAL(527, 11, 0, 2, 3, -5.451f, 0.0f, 3.098f, 0.0f);
        this.npc1.disableDTKFlag(131072);
        this.npc1.enableDTKFlag(12);
        this.npc1.setMotion(0, 2);
        this.npc2 = new NPC_NORMAL(527, 12, 0, 2, 3, -4.056f, 0.0f, -5.425f, 0.0f);
        this.npc2.disableDTKFlag(131072);
        this.npc2.enableDTKFlag(12);
        if (Runtime.getFlags(346, 1) == 0) {
            this.npc1.talkto("TalkNPC1");
            this.npc2.talkto("TalkNPC2");
        } else {
            this.npc1.talkto("TalkNPC1a");
            this.npc2.talkto("TalkNPC2a");
        }
        if (Runtime.getFlags(303, 1) == 1 && Runtime.getFlags(304, 1) == 0 && Runtime.getFlags(3165, 1) == 0) {
            this.Kidou = new Mapunits();
            this.Kidou.mapUnit(38);
            this.Kidou.start(4, null);
            Runtime.setFlags(3165, 1, 1);
            this.Kidou.start(1, "Mail");
        }
        this.doorA = new Uwamono(36, 42, '\u0001');
        new Uwamono(37, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(6036, 1) == 0) {
            this.player.setID(2);
        } else if (Runtime.getFlags(6036, 1) == 1) {
            this.player.setID(1);
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
            this.setShadow(1, 80);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Mail() {
            Runtime.setPlayerControl(false);
            ST1780.this.win = Window.create();
            ST1780.this.win.setSize(4, 45);
            ST1780.this.win.setLocation(15, 305);
            ST1780.this.win.print(ST1780.this.msgMAIL1, 0);
            Runtime.mailArriveSet(49);
            Runtime.setFlags(3165, 1, 1);
            System.waitFor(ST1780.this.win);
            ST1780.this.menu = Menu.create();
            ST1780.this.menu.addItem("Read email\nDon't read email");
            System.waitFor(ST1780.this.menu);
            System.sleep(10);
            ST1780.this.selected = ST1780.this.menu.getSelected();
            switch (ST1780.this.selected) {
                case 0: {
                    Runtime.setPlayerControl(true);
                    Runtime.mailExec(1);
                    break;
                }
                default: {
                    Runtime.setPlayerControl(true);
                }
            }
        }
    }
}

