import xeno.Camera;
import xeno.Chr;
import xeno.Enepc;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK07B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0350
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK07B_PRJ {
    public Player player;
    Camera cam1;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Unit[] unit;
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
    Uwamono doorA;
    Uwamono doorB;
    int page;
    String[] msg57ADD19E = new String[]{"/[label(Sailor (E))]", "Looks like just a corpse...", "/[waitkey(1)]/[clear()]", "There's a key. Pick it up?", "/[waitkey(64)]/[close()]"};
    String[] msg57ADD19F = new String[]{"/[label(Sailor (E))]", "He's dead...", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D45 = new String[]{"It is locked.", "/[wait(256)]", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D46 = new String[]{"The door opened.", "/[waitkey(64)]/[close()]"};
    String[] msg57653BC8 = new String[]{"/[label(Sailor (F))]", "Aaaah! Stay away! Go away!!", "/[waitkey(64)]/[close()]"};
    String[] msg57653BC9 = new String[]{"/[label(Sailor (F))]", "I don't want to die yet! As the old adage goes, \"Slow and steady wins the race,\" right?!", "/[waitkey(64)]/[close()]"};

    ST0350() {
    }

    void Final_init(int n) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            case 0: {
                Runtime.jumpCF(380, 4);
                break;
            }
            case 3: {
                Runtime.jumpCF(400, 1);
                break;
            }
        }
    }

    void init() {
        int n = 0;
        while (n < 501) {
            Stage.setVisible(n, true);
            ++n;
        }
        int n2 = Runtime.getEntrance();
        if (n2 >= 0) {
            Runtime.setRegister(0, n2);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n2);
        }
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(4, false);
        Stage.setVisible(5, false);
        Stage.setVisible(6, false);
        Stage.setVisible(7, false);
        Stage.setVisible(8, false);
        Stage.setVisible(9, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.1f, 0.1f);
        this.cam0.setCFLockX(1, -9.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFPedestal(5, -18.872f, 4.022f, 16.727f, 65.0f, -44.62f, -21.666f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 10.0f, 10.0f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.npc5 = new People();
        this.npc5.init(526, -10.2f, 0.0f, 26.2f, -45.0f);
        this.npc5.id = 5;
        this.npc5.setParams(0, 4, 5, 4);
        this.npc6 = new People();
        this.npc6.init(526, -11.9f, 0.0f, 28.2f, 45.0f);
        this.npc6.id = 6;
        this.npc6.setParams(0, 4, 6, 4);
        this.npc6.kickEnepc(4, 1);
        this.npc6.setVisible(false);
        this.npc7 = new People();
        this.npc7.init(526, -11.9f, 0.0f, 36.4f, 45.0f);
        this.npc7.id = 7;
        this.npc7.setParams(0, 4, 7, 4);
        this.npc7.kickEnepc(4, 1);
        this.npc7.setVisible(false);
        new Uwamono(38, 40, '\u0001');
        new Uwamono(40, 40, '\u0001');
        new Uwamono(35, 40, '\u0001');
        new Uwamono(36, 40, '\u0001');
        new Uwamono(37, 40, '\u0001');
        new Uwamono(39, 40, '\u0001');
        new Uwamono(41, 40, '\u0001');
        new Uwamono(42, 40, '\u0001');
        new Uwamono(85, 40, '\u0001');
        new Uwamono(86, 40, '\u0001');
        new Uwamono(87, 40, '\u0001');
        new Uwamono(88, 40, '\u0001');
        new Uwamono(89, 40, '\u0001');
        new Uwamono(90, 40, '\u0001');
        new Uwamono(91, 40, '\u0001');
        new Uwamono(92, 40, '\u0001');
        new Uwamono(93, 40, '\u0001');
        new Uwamono(94, 40, '\u0001');
        new Uwamono(95, 40, '\u0001');
        new Uwamono(96, 40, '\u0001');
        this.doorA = new Uwamono(81, 42, '\u0001');
        new Uwamono(82, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(83, 42, '\u0001');
        new Uwamono(84, 42, '\u0001', this.doorB);
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
                case 5: {
                    switch (ST0350.this.npc5talked) {
                        case 0: {
                            ST0350.this.npc1.kickEnepc(4, 2);
                            ST0350.this.win = Window.create();
                            ST0350.this.win.setSize(4, 45);
                            ST0350.this.win.setLocation(15, 305);
                            ST0350.this.win.print(ST0350.this.msg57ADD19E, 0);
                            ST0350.waitPage(ST0350.this.win, 64);
                            System.waitFor(ST0350.this.win);
                            ST0350.this.menu = Menu.create();
                            ST0350.this.menu.addItem("Take it\nLeave it");
                            System.waitFor(ST0350.this.menu);
                            ST0350.this.selected = ST0350.this.menu.getSelected();
                            switch (ST0350.this.selected) {
                                case 0: {
                                    ST0350.this.npc5talked += 10;
                                    ST0350.this.npc5.kickEnepc(4, 0);
                                    return;
                                }
                                case 1: {
                                    ST0350.this.npc5.kickEnepc(4, 0);
                                    return;
                                }
                            }
                            return;
                        }
                        case 10: {
                            ST0350.this.npc5.kickEnepc(4, 2);
                            ST0350.this.win = Window.create();
                            ST0350.this.win.setSize(4, 45);
                            ST0350.this.win.setLocation(15, 305);
                            ST0350.this.win.print(ST0350.this.msg57ADD19F, 0);
                            ST0350.waitPage(ST0350.this.win, 64);
                            return;
                        }
                    }
                }
                case 6: {
                    switch (ST0350.this.npc5talked) {
                        case 0: {
                            ST0350.this.win = Window.create();
                            ST0350.this.win.setSize(4, 45);
                            ST0350.this.win.setLocation(15, 305);
                            ST0350.this.win.print(ST0350.this.msg57AD5D45, 0);
                            ST0350.waitPage(ST0350.this.win, 64);
                            return;
                        }
                        case 10: {
                            ST0350.this.win = Window.create();
                            ST0350.this.win.setSize(4, 45);
                            ST0350.this.win.setLocation(15, 305);
                            ST0350.this.win.print(ST0350.this.msg57AD5D46, 0);
                            ST0350.waitPage(ST0350.this.win, 64);
                            return;
                        }
                    }
                }
                case 7: {
                    switch (ST0350.this.npc7talked) {
                        case 0: {
                            ST0350.this.win = Window.create();
                            ST0350.this.win.setSize(4, 45);
                            ST0350.this.win.setLocation(15, 305);
                            ST0350.this.win.print(ST0350.this.msg57653BC8, 0);
                            ST0350.waitPage(ST0350.this.win, 64);
                            ++ST0350.this.npc7talked;
                            return;
                        }
                        case 1: {
                            ST0350.this.win = Window.create();
                            ST0350.this.win.setSize(4, 45);
                            ST0350.this.win.setLocation(15, 305);
                            ST0350.this.win.print(ST0350.this.msg57653BC9, 0);
                            ST0350.waitPage(ST0350.this.win, 64);
                            --ST0350.this.npc7talked;
                            return;
                        }
                    }
                }
            }
        }
    }
}

