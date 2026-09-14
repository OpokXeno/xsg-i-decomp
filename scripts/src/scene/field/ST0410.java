import xeno.Camera;
import xeno.Chr;
import xeno.Enepc;
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

class ST0410
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK12B_PRJ {
    public Player player;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
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
    int passed1 = 0;
    int passed2 = 0;
    int passed3 = 0;
    int passed4 = 0;
    int page;
    String[] Npc_1_0 = new String[]{"この先はグノーシスだらけで\n", "進めないぜ！！\n", "\n", "\n", "…もっとも\n", "ＡＧＷＳでもありゃ別だけどな！\n", "/[waitkey(64)]/[close()]"};
    String[] Npc_1_1 = new String[]{"相棒の野郎！！\n", "武器も持たねぇで格納庫にいっちまった\n", "\n", "\n", "俺はここに隠れて\n", "グノーシスが通り過ぎるのを待つぜ…\n", "/[waitkey(64)]/[close()]"};
    String[] Npc_2_0 = new String[]{"相棒の制止を振り切って\n", "ＵＭＮ通信室を飛び出したんだ…\n", "\n", "\n", "こんなにバケモノだらけとは\n", "思ってもみなかったゼ！！\n", "/[waitkey(64)]/[close()]"};
    String[] Npc_2_1 = new String[]{"まったく相棒の言うとおりだ…\n", "丸腰であんなバケモノに適うわけがねぇ\n", "/[waitkey(64)]/[close()]"};
    String[] Npc_2_2 = new String[]{"…へんじがない\n", "ただのしかばねのようだ…\n", "/[waitkey(64)]/[close()]"};
    String[] Test_0_1 = new String[]{"ここでカメラ切り替えます。（仮）\n", "/[waitkey(64)]/[close()]"};
    String[] Npc_5_0 = new String[]{"グノーシスの習性について聞きたいか？\n", "/[waitkey(64)]/[close()]"};
    String[] Npc_5_1 = new String[]{"ヤツらの中には\n", "動くものに興味を示すやつがいるんだ\n", "武器を持ってないんなら\n", "自分以外の何かに気を引かせればいい\n", "例えば…\n", "テレビや映写機なんかな…\n", "/[waitkey(64)]/[close()]"};
    String[] Npc_5_2 = new String[]{"そうか…\n", "幸運を祈るぜ！！\n", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_0 = new String[]{"スイッチがある。\n", "押してみますか？\n", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_1 = new String[]{"艦内モニターを点灯します。\n", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_2 = new String[]{"艦内モニターの点灯を中止します。\n", "/[waitkey(64)]/[close()]"};

    ST0410() {
    }

    void Final_init(int n) {
        switch (n) {
            case 1: {
                this.enemy1.kickEnepc(10, 70, 0);
            }
            case 2: {
                this.enemy2.kickEnepc(10, 70, 0);
            }
            case 4: {
                this.enemy4.kickEnepc(10, 70, 0);
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
            return;
        }
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    if (this.passed4 != 1) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.enemy4.kickEnepc(8, 14);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Test_0_1, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    this.passed4 = 1;
                    return;
                }
            }
            return;
        }
        if (n2 == 4) {
            switch (n) {
                case 100: {
                    if (this.npc1talked == 0) {
                        Runtime.setPlayerControl(false);
                        this.Kakuheki_Close();
                        System.sleep(10);
                        Runtime.setPlayerControl(true);
                        this.npc1talked = 1;
                        return;
                    }
                    if (this.npc1talked != 1) return;
                    Runtime.setPlayerControl(false);
                    this.Kakuheki_Open();
                    System.sleep(10);
                    Runtime.setPlayerControl(true);
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
                        this.Kakuheki_Close();
                        System.sleep(10);
                        Runtime.setPlayerControl(true);
                        this.npc1talked = 1;
                        return;
                    }
                    if (this.npc1talked != 1) return;
                    Runtime.setPlayerControl(false);
                    this.Kakuheki_Open();
                    System.sleep(10);
                    Runtime.setPlayerControl(true);
                    this.npc1talked = 0;
                    return;
                }
            }
            return;
        }
        if (n2 != 6) return;
        switch (n) {
            case 100: {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Loc_6_0, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("はい\nいいえ");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Loc_6_1, 0);
                        System.waitFor(this.win);
                        this.enemy3.moveEnepc(13, 20.5f, 1.3f, 60);
                        this.enemy3.kickEnepc(7, 4);
                        this.enemy3.kickEnepc(1, 8);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    case 1: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Loc_6_2, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                }
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            case 0: {
                Runtime.jumpCF(380, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(380, 3);
                break;
            }
            case 3: {
                Runtime.jumpCF(320, 1);
                break;
            }
        }
    }

    void init() {
        int n = 0;
        while (n < 500) {
            Stage.setVisible(n, true);
            ++n;
        }
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        Stage.setVisible(23, false);
        Stage.setVisible(24, false);
        Stage.setVisible(25, false);
        Stage.setVisible(26, false);
        Stage.setVisible(27, false);
        Stage.setVisible(28, false);
        Stage.setVisible(58, false);
        Stage.setVisible(59, false);
        Stage.setVisible(64, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 12345, 1);
        Runtime.setDefocusQuick(1, 1, 23456, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.005f, 0.005f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.05f, 0.05f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.05f, 0.05f);
        this.cam0.setCFLockX(3, 21.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.05f, 0.05f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.05f, 0.05f);
        this.cam0.setCFLockX(5, 3.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.05f, 0.05f);
        this.cam0.setCFPedestal(7, 10.724f, 7.143f, -21.846f, 62.91f, -69.846f, -9.719f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFPedestal(8, 24.136f, 10.694f, 4.2844f, 58.52f, -69.413f, 9.159f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFAngle(9, -28.0f, 325.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFPedestal(11, 9.149f, 2.5f, -20.874f, 40.0f, -11.187f, 345.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 100.0f, 100.0f);
        this.cam0.setCFAngle(12, -28.0f, 325.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.05f, 0.05f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, -18.0f, 0.0f, 20.0f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setParams(1, 0, 1, -1);
        this.enemy2 = new Enepc();
        this.enemy2.init(16385, -10.0f, 0.0f, 20.0f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setParams(2, 78, 2, -1);
        this.enemy3 = new Enepc();
        this.enemy3.init(16385, 21.0f, 0.0f, -3.0f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setParams(3, 78, 3, -1);
        this.enemy4 = new Enepc();
        this.enemy4.init(16385, -11.0f, 2.0f, -27.0f, 90.0f);
        this.enemy4.id = 4;
        this.enemy4.setParams(4, 78, 4, -1);
        this.npc1 = new People();
        this.npc1.init(526, 11.13f, 0.0f, -24.75f, 0.0f);
        this.npc1.id = 11;
        this.npc1.setParams(0, 0, 11, 4);
        this.npc2 = new People();
        this.npc2.init(526, 1.5f, 0.0f, -16.14f, 90.0f);
        this.npc2.id = 12;
        this.npc2.setParams(0, 4, 12, 4);
        this.npc3 = new People();
        this.npc3.init(526, 0.0f, 0.0f, 0.0f, 0.0f);
        this.npc3.id = 13;
        this.npc3.setParams(0, 4, 13, 4);
        this.npc3.setLocation(4, 0);
        this.npc3.setVisible(false);
        this.npc4 = new People();
        this.npc4.init(526, 17.0f, 0.0f, -17.0f, 270.0f);
        this.npc4.id = 14;
        this.npc4.setParams(0, 4, 14, 4);
        this.npc4.setVisible(false);
        this.npc5 = new People();
        this.npc5.init(519, 22.61f, 0.0f, 20.21f, -45.0f);
        this.npc5.id = 15;
        this.npc5.setParams(0, 4, 15, 7);
        new Uwamono(0, 40, '\u0001');
        this.doorA = new Uwamono(13, 42, '\u0001');
        new Uwamono(14, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(17, 42, '\u0001');
        new Uwamono(18, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(19, 42, '\u0001');
        new Uwamono(20, 42, '\u0002', this.doorC);
        this.doorD = new Uwamono(140, 42, '\u0001');
        new Uwamono(139, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(186, 40, '\u0001');
        new Uwamono(185, 40, '\u0001', this.doorE);
        this.doorD.SetDoorType('\u0002');
        this.doorD.SetDoorRange(3.0f);
        this.doorD.DoorOpen();
        new Uwamono(84, 84);
        new Uwamono(88, 84);
        new Uwamono(92, 84);
        new Uwamono(96, 84);
        new Uwamono(150, 31);
        new Uwamono(151, 21);
        new Uwamono(187, 21);
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
            this.init(1, 3);
            this.setPlayer();
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

        public void talk() {
            block0:
            switch (this.id) {
                case 11: {
                    if (ST0410.this.npc1talked == 0) {
                        Runtime.setPlayerControl(false);
                        ST0410.this.npc1.kickEnepc(4, 1);
                        ST0410.this.npc1.kickEnepc(9, 100);
                        ST0410.this.npc1.kickEnepc(0, 10);
                        ST0410.this.win = Window.create();
                        ST0410.this.win.setSize(4, 45);
                        ST0410.this.win.setLocation(15, 305);
                        ST0410.this.win.print(ST0410.this.Npc_1_0, 0);
                        System.waitFor(ST0410.this.win);
                        ST0410.this.npc1talked = 1;
                        ST0410.this.npc1.kickEnepc(9, -1);
                        ST0410.this.npc1.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (ST0410.this.npc1talked != 1) break;
                    Runtime.setPlayerControl(false);
                    ST0410.this.npc1.kickEnepc(4, 1);
                    ST0410.this.npc1.kickEnepc(9, 100);
                    ST0410.this.npc1.kickEnepc(0, 10);
                    ST0410.this.win = Window.create();
                    ST0410.this.win.setSize(4, 45);
                    ST0410.this.win.setLocation(15, 305);
                    ST0410.this.win.print(ST0410.this.Npc_1_1, 0);
                    System.waitFor(ST0410.this.win);
                    ST0410.this.npc1talked = 0;
                    ST0410.this.npc1.kickEnepc(9, -1);
                    ST0410.this.npc1.kickEnepc(4, 0);
                    Runtime.setPlayerControl(true);
                    break;
                }
                case 12: {
                    if (ST0410.this.npc2talked == 0) {
                        Runtime.setPlayerControl(false);
                        ST0410.this.npc2.kickEnepc(4, 1);
                        ST0410.this.npc2.kickEnepc(9, 100);
                        ST0410.this.npc2.kickEnepc(0, 10);
                        ST0410.this.win = Window.create();
                        ST0410.this.win.setSize(4, 45);
                        ST0410.this.win.setLocation(15, 305);
                        ST0410.this.win.print(ST0410.this.Npc_2_0, 0);
                        ST0410.this.npc2talked = 1;
                        System.waitFor(ST0410.this.win);
                        ST0410.this.npc2.kickEnepc(9, -1);
                        ST0410.this.npc2.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (ST0410.this.npc2talked == 1) {
                        Runtime.setPlayerControl(false);
                        ST0410.this.npc2.kickEnepc(4, 1);
                        ST0410.this.npc2.kickEnepc(9, 100);
                        ST0410.this.npc2.kickEnepc(0, 10);
                        ST0410.this.win = Window.create();
                        ST0410.this.win.setSize(4, 45);
                        ST0410.this.win.setLocation(15, 305);
                        ST0410.this.win.print(ST0410.this.Npc_2_1, 0);
                        ST0410.this.npc2talked = 2;
                        System.waitFor(ST0410.this.win);
                        ST0410.this.npc2.kickEnepc(9, -1);
                        ST0410.this.npc2.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (ST0410.this.npc2talked != 2) break;
                    Runtime.setPlayerControl(false);
                    ST0410.this.npc2.kickEnepc(4, 2);
                    ST0410.this.win = Window.create();
                    ST0410.this.win.setSize(4, 45);
                    ST0410.this.win.setLocation(15, 305);
                    ST0410.this.win.print(ST0410.this.Npc_2_2, 0);
                    System.waitFor(ST0410.this.win);
                    Runtime.setPlayerControl(true);
                    break;
                }
                case 15: {
                    Runtime.setPlayerControl(false);
                    ST0410.this.npc5.kickEnepc(4, 1);
                    ST0410.this.npc5.kickEnepc(9, 100);
                    ST0410.this.npc5.kickEnepc(0, 10);
                    ST0410.this.win = Window.create();
                    ST0410.this.win.setSize(4, 45);
                    ST0410.this.win.setLocation(15, 305);
                    ST0410.this.win.print(ST0410.this.Npc_5_0, 0);
                    System.waitFor(ST0410.this.win);
                    ST0410.this.menu = Menu.create();
                    ST0410.this.menu.addItem("知りたい\n別に聞きたくない");
                    System.waitFor(ST0410.this.menu);
                    ST0410.this.selected = ST0410.this.menu.getSelected();
                    switch (ST0410.this.selected) {
                        case 0: {
                            ST0410.this.win = Window.create();
                            ST0410.this.win.setSize(4, 45);
                            ST0410.this.win.setLocation(15, 305);
                            ST0410.this.win.print(ST0410.this.Npc_5_1, 0);
                            System.waitFor(ST0410.this.win);
                            ST0410.this.npc5.kickEnepc(9, -1);
                            ST0410.this.npc5.kickEnepc(4, 0);
                            Runtime.setPlayerControl(true);
                            break block0;
                        }
                        case 1: {
                            ST0410.this.win = Window.create();
                            ST0410.this.win.setSize(4, 45);
                            ST0410.this.win.setLocation(15, 305);
                            ST0410.this.win.print(ST0410.this.Npc_5_2, 0);
                            System.waitFor(ST0410.this.win);
                            ST0410.this.npc5.kickEnepc(9, -1);
                            ST0410.this.npc5.kickEnepc(4, 0);
                            Runtime.setPlayerControl(true);
                            break block0;
                        }
                    }
                }
            }
        }
    }

    class Mapunits
            extends Unit {
        Mapunits() {
        }
    }
}

