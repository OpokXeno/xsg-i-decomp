import xeno.Camera;
import xeno.Chr;
import xeno.Enepc;
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

class ST0380
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK10B_PRJ {
    public Player player;
    Camera cam1;
    Menu menu;
    Window win;
    int entrance;
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
    Enepc enemy6;
    Enepc enemy7;
    Unit[] unit;
    int count = 0;
    int selected = 0;
    int mapno;
    int enemy;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono itembox;
    Chr dummy1;
    Chr dummy2;
    int dummy1talked = 0;
    int dummy2talked = 0;
    int pass1 = 0;
    int page;
    String[] Dummy_1 = new String[]{"There's a switch. Pressing it?\n", "/[waitkey(64)]/[close()]"};
    String[] Dummy_2 = new String[]{"Opening airlock.", "/[waitkey(64)]/[close()]"};
    String[] Dummy_2_1 = new String[]{"Canceling opening of airlock.", "/[waitkey(64)]/[close()]"};
    String[] Dummy_3 = new String[]{"Locking airlock.", "/[waitkey(64)]/[close()]"};
    String[] Dummy_3_1 = new String[]{"Canceling locking of airlock.", "/[waitkey(64)]/[close()]"};
    String[] npc_1_0 = new String[]{"Damn it!! I can't believe I forgot something in a place like that!!", "/[waitkey(64)]/[close()]"};
    String[] npc_1_1 = new String[]{"If only I could stop that monster, I could get that thing I forgot.", "/[waitkey(64)]/[close()]"};
    String[] npc_2_0 = new String[]{"A key is required to ", "/[waitkey(64)]/[close()]"};
    String[] npc_2_1 = new String[]{"get into the subroute. ", "/[waitkey(64)]/[close()]"};
    String[] npc_4_0 = new String[]{"I want to move on ahead, but ", "/[waitkey(64)]/[close()]"};
    String[] npc_4_1 = new String[]{"the car is in the way. ", "/[waitkey(64)]/[close()]"};
    String[] Test_1 = new String[]{"Location test\n", "/[waitkey(64)]/[close()]"};

    ST0380() {
    }

    void Final_init(int n) {
        switch (n) {
            case 2: {
                this.enemy2.kickEnepc(10, 50, 0);
            }
            case 3: {
                this.enemy3.kickEnepc(10, 50, 0);
            }
            case 4: {
                this.enemy4.kickEnepc(10, 40, 0);
            }
            case 5: {
                this.enemy5.kickEnepc(10, 40, 0);
            }
            case 6: {
                this.enemy6.kickEnepc(10, 40, 0);
            }
            case 7: {
                this.enemy7.kickEnepc(10, 90, 0);
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Dummy_1, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.Dummy_2, 0);
                            System.waitFor(this.win);
                            this.enemy1.kickEnepc(1, 1);
                            this.enemy1.moveEnepc(17, 15.0f, 0.0f, 30);
                            this.enemy1.moveEnepc(15, 0.0f, -27.0f, 30);
                            this.enemy1.kickEnepc(7, 4);
                            this.enemy1.kickEnepc(10, 70, 60);
                            this.enemy1.kickEnepc(8, 13);
                            Runtime.setPlayerControl(true);
                            this.dummy1talked = 1;
                            return;
                        }
                        case 1: {
                            this.win = Window.create();
                            this.win.setSize(4, 45);
                            this.win.setLocation(15, 305);
                            this.win.print(this.Dummy_2_1, 0);
                            System.waitFor(this.win);
                            Runtime.setPlayerControl(true);
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
        if (n2 != 1) return;
        switch (n) {
            case 100: {
                if (this.pass1 != 0) return;
                this.enemy4.moveEnepc(17, 15.0f, 0.0f, 30);
                this.enemy4.moveEnepc(15, -12.43f, 4.06f, 30);
                this.enemy4.kickEnepc(7, 80);
                this.enemy5.moveEnepc(17, 15.0f, 0.0f, 30);
                this.enemy5.moveEnepc(15, -15.6f, 4.03f, 30);
                this.enemy5.kickEnepc(7, 80);
                this.enemy6.moveEnepc(17, 15.0f, 0.0f, 30);
                this.enemy6.moveEnepc(15, -15.11f, 5.67f, 30);
                this.enemy6.kickEnepc(7, 80);
                this.pass1 = 1;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            case 0: {
                Runtime.jumpCF(390, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(410, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(410, 3);
                break;
            }
            case 3: {
                Runtime.jumpCF(350, 1);
                break;
            }
        }
    }

    void init() {
        int n = 0;
        while (n < 501) {
            Stage.setVisible(n, true);
        }
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 15.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 335.0f, 0.0f, 12.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestal(8, -34.8197f, 11.7357f, -13.3628f, 42.35f, -43.4036f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(9, 0.03f, 0.03f);
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, 0.0f, 0.0f, -27.0f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(4, 4, 4, 4);
        float[] fArray = new float[4];
        fArray[2] = -27.0f;
        fArray[3] = 1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(0, 78, 1, -1, fArray2);
        float[] fArray3 = new float[9];
        fArray3[0] = -3.0f;
        fArray3[2] = -27.0f;
        fArray3[5] = -27.0f;
        fArray3[6] = 3.0f;
        fArray3[8] = -27.0f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy2 = new Enepc();
        this.enemy2.init(16385, -34.12f, 0.0f, -23.49f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(4, 4, 4, 4);
        float[] fArray5 = new float[4];
        fArray5[0] = -34.12f;
        fArray5[2] = -23.49f;
        fArray5[3] = 1.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(0, 5, 2, -1, fArray6);
        float[] fArray7 = new float[9];
        fArray7[0] = -35.62f;
        fArray7[2] = -23.56f;
        fArray7[3] = -34.12f;
        fArray7[5] = -23.49f;
        fArray7[6] = -35.08f;
        fArray7[8] = -22.03f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy3 = new Enepc();
        this.enemy3.init(16385, -33.89f, 0.0f, -20.27f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(4, 4, 4, 4);
        float[] fArray9 = new float[4];
        fArray9[0] = -33.89f;
        fArray9[2] = -20.27f;
        fArray9[3] = 1.0f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(0, 5, 3, -1, fArray10);
        float[] fArray11 = new float[9];
        fArray11[0] = -31.6f;
        fArray11[2] = -21.09f;
        fArray11[3] = -33.89f;
        fArray11[5] = -20.27f;
        fArray11[6] = -35.13f;
        fArray11[8] = -21.99f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy4 = new Enepc();
        this.enemy4.init(16385, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy4.id = 4;
        this.enemy4.setParams(0, 4, 4, -1);
        this.enemy4.setLocation(5, 0);
        this.enemy5 = new Enepc();
        this.enemy5.init(16385, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy5.id = 5;
        this.enemy5.setParams(0, 4, 5, -1);
        this.enemy5.setLocation(5, 1);
        this.enemy6 = new Enepc();
        this.enemy6.init(16385, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy6.id = 6;
        this.enemy6.setParams(0, 4, 6, -1);
        this.enemy6.setLocation(5, 2);
        this.enemy7 = new Enepc();
        this.enemy7.init(16385, 9.5f, 2.0f, 22.0f, -90.0f);
        this.enemy7.id = 7;
        this.enemy7.setParams(0, 5, 7, -1);
        Stage.setVisible(4, false);
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(5, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.npc1 = new People();
        this.npc1.init(526, -32.0f, 0.0f, -30.0f, 0.0f);
        this.npc1.id = 11;
        this.npc1.setParams(0, 4, 11, 4);
        this.npc2 = new People();
        this.npc2.init(526, -4.66f, 2.0f, 20.27f, 0.0f);
        this.npc2.id = 12;
        this.npc2.setParams(0, 4, 12, 4);
        this.npc3 = new People();
        this.npc3.init(526, 0.0f, 0.0f, -29.0f, 0.0f);
        this.npc3.id = 13;
        this.npc3.setParams(0, 4, 13, 4);
        this.npc3.setVisible(false);
        this.npc4 = new People();
        this.npc4.init(526, -14.0f, 0.0f, -13.0f, 0.0f);
        this.npc4.id = 14;
        this.npc4.setParams(0, 4, 14, 4);
        this.doorA = new Uwamono(44, 42, '\u0001');
        new Uwamono(45, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(48, 42, '\u0001');
        new Uwamono(49, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(52, 42, '\u0001');
        new Uwamono(53, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(56, 42, '\u0001');
        new Uwamono(57, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(148, 42, '\u0001');
        new Uwamono(147, 42, '\u0001', this.doorE);
        new Uwamono(89, 0);
        new Uwamono(90, 0);
        new Uwamono(91, 0);
        new Uwamono(97, 0);
        this.trap1 = new Uwamono(28673, -36.0f, 0.0f, -27.2f, 0.0f);
        this.trap2 = new Uwamono(28673, -31.7f, 0.0f, -23.8f, 0.0f);
        this.player.setTranslate(9.0187f, 0.0f, -24.973f);
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

    class People
            extends Enepc {
        People() {
        }

        void init() {
        }

        public void talk() {
            switch (this.id) {
                case 11: {
                    if (ST0380.this.npc1talked == 0) {
                        Runtime.setPlayerControl(false);
                        ST0380.this.win = Window.create();
                        ST0380.this.win.setSize(4, 45);
                        ST0380.this.win.setLocation(15, 305);
                        ST0380.this.win.print(ST0380.this.npc_1_0, 0);
                        ST0380.this.npc1talked = 1;
                        System.waitFor(ST0380.this.win);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (ST0380.this.npc1talked != 1) break;
                    Runtime.setPlayerControl(false);
                    ST0380.this.win = Window.create();
                    ST0380.this.win.setSize(4, 45);
                    ST0380.this.win.setLocation(15, 305);
                    ST0380.this.win.print(ST0380.this.npc_1_1, 0);
                    System.waitFor(ST0380.this.win);
                    ST0380.this.npc1talked = 0;
                    Runtime.setPlayerControl(true);
                    break;
                }
                case 12: {
                    if (ST0380.this.npc2talked == 0) {
                        Runtime.setPlayerControl(false);
                        ST0380.this.win = Window.create();
                        ST0380.this.win.setSize(4, 45);
                        ST0380.this.win.setLocation(15, 305);
                        ST0380.this.win.print(ST0380.this.npc_2_0, 0);
                        ST0380.this.npc2talked = 1;
                        System.waitFor(ST0380.this.win);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (ST0380.this.npc2talked != 1) break;
                    Runtime.setPlayerControl(false);
                    ST0380.this.win = Window.create();
                    ST0380.this.win.setSize(4, 45);
                    ST0380.this.win.setLocation(15, 305);
                    ST0380.this.win.print(ST0380.this.npc_2_1, 0);
                    System.waitFor(ST0380.this.win);
                    ST0380.this.npc2talked = 0;
                    Runtime.setPlayerControl(true);
                    break;
                }
                case 14: {
                    if (ST0380.this.npc4talked == 0) {
                        Runtime.setPlayerControl(false);
                        ST0380.this.win = Window.create();
                        ST0380.this.win.setSize(4, 45);
                        ST0380.this.win.setLocation(15, 305);
                        ST0380.this.win.print(ST0380.this.npc_4_0, 0);
                        ST0380.this.npc4talked = 1;
                        System.waitFor(ST0380.this.win);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    if (ST0380.this.npc4talked != 1) break;
                    Runtime.setPlayerControl(false);
                    ST0380.this.win = Window.create();
                    ST0380.this.win.setSize(4, 45);
                    ST0380.this.win.setLocation(15, 305);
                    ST0380.this.win.print(ST0380.this.npc_4_1, 0);
                    System.waitFor(ST0380.this.win);
                    ST0380.this.npc4talked = 0;
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
        }
    }
}

