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
import xeno.map.MC_GNK10_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3040
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK10_PRJ {
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
    Unit Ele00;
    Unit Ele01;
    Unit Ele02;
    Unit Ele03;
    Unit MJele;
    Unit PUPU;
    Uwamono Stop1;
    Uwamono Stop2;
    Uwamono Stop3;
    Uwamono Stop4;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono item06;
    Unit UTIC1a;
    Unit UTIC1b;
    Unit UTIC1c;
    Unit UTIC1d;
    Unit UTIC2a;
    Unit UTIC2b;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect E235A;
    Effect E235B;
    Effect E235C;
    Effect E235D;
    Effect E236A;
    Effect E236B;
    Effect E236C;
    Effect E236D;
    Effect E237A;
    Effect E237B;
    Effect E237C;
    Effect E237D;
    Effect E238A;
    Effect E238B;
    Effect E238C;
    Effect E238D;
    Effect E239;
    Effect E240A;
    Effect E240AA;
    Effect E240B;
    Effect E240BB;
    Effect fade;
    Light light = new Light(0);
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
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int page;
    String[] Info_00 = new String[]{"Use the elevator?", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};

    ST3040() {
    }

    void EOB(int n) {
        if (n == 5) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3180, 1, 1);
        }
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-12.204f, 1.551f, 11.806f);
        this.camEV.setRotate(-13.626f, 59.438f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.303f, 12.335f, 14.174f);
        this.camEV.setRotate(-59.106f, 50.719f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.229f, 1.903f, -2.916f);
        this.camEV.setRotate(-11.646f, 138.658f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.097f, 1.187f, -2.94f);
        this.camEV.setRotate(3.493f, 100.519f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-3.278f, 1.807f, -17.573f);
        this.camEV.setRotate(-14.766f, -50.881f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.369f, 12.815f, -9.359f);
        this.camEV.setRotate(-58.646f, 26.64f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.044f, 1.647f, -17.223f);
        this.camEV.setRotate(-8.946f, 59.76f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera07() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.279f, 7.343f, -2.589f);
        this.camEV.setRotate(-18.666f, -19.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera08() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.180101f, 14.0f, -12.874101f);
        this.camEV.setRotate(-51.60708f, -20.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3079, 1) == 0) {
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
                                this.cam0.setMode(-1);
                                this.EV_Camera00();
                                Runtime.enable(65536);
                                this.player.mtn(26, 1, 1.0f, true);
                                System.sleep(30);
                                Sound.effectPlay(196741);
                                System.sleep(30);
                                this.EV_Camera01();
                                Sound.effectPlay(196742);
                                this.Ele00.start(1, "AU");
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        return;
                    }
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
                            this.cam0.setMode(-1);
                            this.EV_Camera00();
                            Runtime.setPlayerControl(false);
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(30);
                            this.EV_Camera01();
                            Sound.effectPlay(196742);
                            this.Ele00.start(1, "AD");
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3080, 1) == 0) {
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
                                this.cam0.setMode(-1);
                                this.EV_Camera02();
                                Runtime.enable(65536);
                                this.player.mtn(26, 1, 1.0f, true);
                                System.sleep(30);
                                Sound.effectPlay(196741);
                                System.sleep(30);
                                this.EV_Camera03();
                                Runtime.setPlayerControl(false);
                                Sound.effectPlay(196742);
                                this.Ele00.start(1, "BD");
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        return;
                    }
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
                            this.cam0.setMode(-1);
                            this.EV_Camera02();
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(30);
                            this.EV_Camera03();
                            Runtime.setPlayerControl(false);
                            Sound.effectPlay(196742);
                            this.Ele00.start(1, "BU");
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3081, 1) == 0) {
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
                                this.cam0.setMode(-1);
                                this.EV_Camera04();
                                Runtime.setPlayerControl(false);
                                Runtime.enable(65536);
                                this.player.mtn(26, 1, 1.0f, true);
                                System.sleep(30);
                                Sound.effectPlay(196741);
                                System.sleep(30);
                                this.EV_Camera05();
                                Sound.effectPlay(196742);
                                this.Ele00.start(1, "CU");
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        return;
                    }
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
                            this.cam0.setMode(-1);
                            this.EV_Camera04();
                            Runtime.setPlayerControl(false);
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(30);
                            this.EV_Camera05();
                            Sound.effectPlay(196742);
                            this.Ele00.start(1, "CD");
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3082, 1) == 0) {
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
                                this.cam0.setMode(-1);
                                this.EV_Camera06();
                                Runtime.setPlayerControl(false);
                                Runtime.enable(65536);
                                this.player.mtn(26, 1, 1.0f, true);
                                System.sleep(30);
                                Sound.effectPlay(196741);
                                System.sleep(30);
                                this.EV_Camera07();
                                Sound.effectPlay(196742);
                                this.Ele00.start(1, "DD");
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        return;
                    }
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
                            this.cam0.setMode(-1);
                            this.EV_Camera06();
                            Runtime.setPlayerControl(false);
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(30);
                            this.EV_Camera07();
                            Sound.effectPlay(196742);
                            this.Ele00.start(1, "DU");
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 != 4) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3083, 1) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Info_00, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.doorA.SetDoorType('\u0002');
                        this.doorA.DoorClose();
                        System.sleep(45);
                        Runtime.enable(65536);
                        this.player.mtn(2, 9, 1.0f, true);
                        this.player.move(30, 15.294f, -18.682f, true);
                        System.sleep(35);
                        this.player.rotY(15, 0.0f, true);
                        System.sleep(20);
                        this.cam0.setMode(-1);
                        this.EV_Camera08();
                        this.player.mtn(28, 9, 1.0f, true);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(196748);
                        this.MJele.setArgs(12, 15.0f);
                        System.sleep(180);
                        Runtime.setPlayerControl(true);
                        Runtime.setFlags(3084, 1, 1);
                        this.doorA.SetDoorType('\u0004');
                        if (Runtime.getFlags(3152, 1) == 0) {
                            System.println("152152152152152152152152152152152152152152152152152152152152152");
                            Runtime.setFlags(3152, 1, 1);
                        }
                        this.fade.call(0);
                        System.sleep(30);
                        Runtime.jumpCF(68586, 0);
                        return;
                    }
                }
                Runtime.enable(65536);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(60, 15.294f, -15.414f, true);
                System.sleep(60);
                this.doorA.DoorClose();
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                return;
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
                Runtime.jumpCF(68566, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 11.0f, 5.0f, 3.0f, 0.0f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, -1.47f, 0.0f, -18.9f, 0.0f);
        this.teiten2.SetBgm(196613);
        this.teiten3 = new Uwamono(28690, 9.56f, 0.0f, -18.9f, 0.0f);
        this.teiten3.SetBgm(196613);
        this.teiten4 = new Uwamono(28690, 4.0f, 0.0f, -22.0f, 0.0f);
        this.teiten4.SetBgm(196613);
        this.teiten5 = new Uwamono(28690, -14.54f, 0.0f, -0.9f, 0.0f);
        this.teiten5.SetBgm(196613);
        this.teiten6 = new Uwamono(28690, -14.54f, 0.0f, 10.2f, 0.0f);
        this.teiten6.SetBgm(196613);
        this.teiten7 = new Uwamono(28690, -17.7f, 0.0f, 4.664f, 0.0f);
        this.teiten7.SetBgm(196613);
        this.teiten8 = new Uwamono(28690, 15.25f, 5.0f, -19.0f, 0.0f);
        this.teiten8.SetBgm(196614);
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(3152, 1) == 0) {
            if (Runtime.getFlags(3079, 1) == 0) {
                this.Stop1 = new Uwamono(28672, -9.5f, 0.0f, 10.25f, 0.0f);
                this.Stop1.SetSize(3.0f, 1.0f, 5.0f);
                this.Stop1.getTranslate();
            } else {
                this.Stop1 = new Uwamono(28672, -9.5f, 5.0f, 10.25f, 0.0f);
                this.Stop1.SetSize(3.0f, 1.0f, 5.0f);
                this.Stop1.getTranslate();
            }
            if (Runtime.getFlags(3080, 1) == 0) {
                this.Stop2 = new Uwamono(28672, -9.5f, 5.0f, -0.75f, 0.0f);
                this.Stop2.SetSize(3.0f, 1.0f, 5.0f);
                this.Stop2.getTranslate();
            } else {
                this.Stop2 = new Uwamono(28672, -9.5f, 0.0f, -0.75f, 0.0f);
                this.Stop2.SetSize(3.0f, 1.0f, 5.0f);
                this.Stop2.getTranslate();
            }
            if (Runtime.getFlags(3081, 1) == 0) {
                this.Stop3 = new Uwamono(28672, -1.5f, 0.0f, -13.75f, 0.0f);
                this.Stop3.SetSize(5.0f, 1.0f, 3.0f);
                this.Stop3.getTranslate();
            } else {
                this.Stop3 = new Uwamono(28672, -1.5f, 5.0f, -13.75f, 0.0f);
                this.Stop3.SetSize(5.0f, 1.0f, 3.0f);
                this.Stop3.getTranslate();
            }
            if (Runtime.getFlags(3082, 1) == 0) {
                this.Stop4 = new Uwamono(28672, 9.5f, 5.0f, -13.75f, 0.0f);
                this.Stop4.SetSize(5.0f, 1.0f, 3.0f);
                this.Stop4.getTranslate();
            } else {
                this.Stop4 = new Uwamono(28672, 9.5f, 0.0f, -13.75f, 0.0f);
                this.Stop4.SetSize(5.0f, 1.0f, 3.0f);
                this.Stop4.getTranslate();
            }
        } else {
            this.Stop1 = new Uwamono(28672, -9.5f, 0.0f, 10.25f, 0.0f);
            this.Stop1.SetSize(3.0f, 1.0f, 5.0f);
            this.Stop1.getTranslate();
            this.Stop2 = new Uwamono(28672, -9.5f, 0.0f, -0.75f, 0.0f);
            this.Stop2.SetSize(3.0f, 1.0f, 5.0f);
            this.Stop2.getTranslate();
            this.Stop3 = new Uwamono(28672, -1.5f, 0.0f, -13.75f, 0.0f);
            this.Stop3.SetSize(5.0f, 1.0f, 3.0f);
            this.Stop3.getTranslate();
            this.Stop4 = new Uwamono(28672, 9.5f, 0.0f, -13.75f, 0.0f);
            this.Stop4.SetSize(5.0f, 1.0f, 3.0f);
            this.Stop4.getTranslate();
        }
        if (Runtime.getFlags(3152, 1) == 0) {
            if (Runtime.getFlags(3079, 1) == 0) {
                this.Ele00 = new Mapunits();
                this.Ele00.mapUnit(83);
                this.Ele00.start(4, null);
            } else {
                this.Ele00 = new Mapunits();
                this.Ele00.mapUnit(83);
                this.Ele00.start(4, null);
                this.Ele00.setTranslate(this.Ele00.px, this.Ele00.py - 5.0f, this.Ele00.pz);
            }
            if (Runtime.getFlags(3080, 1) == 0) {
                this.Ele01 = new Mapunits();
                this.Ele01.mapUnit(82);
                this.Ele01.start(4, null);
            } else {
                this.Ele01 = new Mapunits();
                this.Ele01.mapUnit(82);
                this.Ele01.start(4, null);
                this.Ele01.setTranslate(this.Ele01.px, this.Ele01.py + 5.0f, this.Ele01.pz);
            }
            if (Runtime.getFlags(3081, 1) == 0) {
                this.Ele02 = new Mapunits();
                this.Ele02.mapUnit(81);
                this.Ele02.start(4, null);
            } else {
                this.Ele02 = new Mapunits();
                this.Ele02.mapUnit(81);
                this.Ele02.start(4, null);
                this.Ele02.setTranslate(this.Ele02.px, this.Ele02.py - 5.0f, this.Ele02.pz);
            }
            if (Runtime.getFlags(3082, 1) == 0) {
                this.Ele03 = new Mapunits();
                this.Ele03.mapUnit(80);
                this.Ele03.start(4, null);
            } else {
                this.Ele03 = new Mapunits();
                this.Ele03.mapUnit(80);
                this.Ele03.start(4, null);
                this.Ele03.setTranslate(this.Ele03.px, this.Ele03.py + 5.0f, this.Ele03.pz);
            }
        } else {
            this.Ele00 = new Mapunits();
            this.Ele00.mapUnit(83);
            this.Ele00.start(4, null);
            this.Ele01 = new Mapunits();
            this.Ele01.mapUnit(82);
            this.Ele01.start(4, null);
            this.Ele01.setTranslate(this.Ele01.px, this.Ele01.py + 5.0f, this.Ele01.pz);
            this.Ele02 = new Mapunits();
            this.Ele02.mapUnit(81);
            this.Ele02.start(4, null);
            this.Ele03 = new Mapunits();
            this.Ele03.mapUnit(80);
            this.Ele03.start(4, null);
            this.Ele03.setTranslate(this.Ele03.px, this.Ele03.py + 5.0f, this.Ele03.pz);
        }
        if (Runtime.getFlags(3083, 1) == 1) {
            this.MJele = new Mapunits();
            this.MJele.initElevator(92, 0.055555556f, 15.0f);
            this.MJele.setArgs(1, 0, 1);
        } else {
            this.MJele = new Mapunits();
            this.MJele.initElevator(92, 0.055555556f, 5.0f);
            this.MJele.setArgs(1, 0, 1);
            this.MJele.setArgs(12, 5.0f);
        }
        this.E235A = new Effect(1634, -14.9f, 1.03f, 10.165f, 0.0f);
        this.E235A.disp(true);
        this.E235A.setClip(true);
        this.E235A.setRotate(0.0f, 90.0f, 0.0f);
        this.E235A.noAttach(false);
        this.E235B = new Effect(1634, -14.9f, 1.03f, -0.915f, 0.0f);
        this.E235B.disp(true);
        this.E235B.setRotate(0.0f, 90.0f, 0.0f);
        this.E235B.setClip(true);
        this.E235B.noAttach(false);
        this.E235C = new Effect(1634, -1.472f, 1.03f, -19.25f, 0.0f);
        this.E235C.disp(true);
        this.E235C.setClip(true);
        this.E235D = new Effect(1634, 9.555f, 1.03f, -19.25f, 0.0f);
        this.E235D.disp(true);
        this.E235D.setClip(true);
        this.E236A = new Effect(1635, -14.5f, 1.13f, 10.165f, 0.0f);
        this.E236A.disp(true);
        this.E236A.setRotate(0.0f, 90.0f, 0.0f);
        this.E236A.setClip(true);
        this.E236A.noAttach(false);
        this.E236B = new Effect(1635, -14.5f, 1.13f, -0.915f, 0.0f);
        this.E236B.disp(true);
        this.E236B.setRotate(0.0f, 90.0f, 0.0f);
        this.E236B.setClip(true);
        this.E236B.noAttach(false);
        this.E236C = new Effect(1635, -1.472f, 1.13f, -18.815f, 0.0f);
        this.E236C.disp(true);
        this.E236C.setClip(true);
        this.E236C.noAttach(false);
        this.E236D = new Effect(1635, 9.555f, 1.13f, -18.815f, 0.0f);
        this.E236D.disp(true);
        this.E236D.setClip(true);
        this.E236D.noAttach(false);
        this.E237A = new Effect(1636, -16.125f, 2.5f, 10.173f, 0.0f);
        this.E237A.disp(true);
        this.E237A.setClip(true);
        this.E237A.setRotate(0.0f, 90.0f, 0.0f);
        this.E237A.noAttach(false);
        this.E237B = new Effect(1636, -16.125f, 2.5f, -0.846f, 0.0f);
        this.E237B.disp(true);
        this.E237B.setClip(true);
        this.E237B.setRotate(0.0f, 90.0f, 0.0f);
        this.E237B.noAttach(false);
        this.E237C = new Effect(1636, -1.482f, 2.5f, -20.439f, 0.0f);
        this.E237C.disp(true);
        this.E237C.setClip(true);
        this.E237C.noAttach(false);
        this.E237D = new Effect(1636, 9.537f, 2.5f, -20.439f, 0.0f);
        this.E237D.disp(true);
        this.E237D.setClip(true);
        this.E237D.noAttach(false);
        this.E238A = new Effect(1637, -8.172f, 7.846f, 15.53f, 0.0f);
        this.E238A.disp(true);
        this.E238A.setClip(true);
        this.E238B = new Effect(1637, -8.173f, 7.846f, -4.332f, 0.0f);
        this.E238B.disp(true);
        this.E238B.setClip(true);
        this.E238C = new Effect(1637, -8.178f, 7.846f, -9.322f, 0.0f);
        this.E238C.disp(true);
        this.E238C.setClip(true);
        this.E238D = new Effect(1637, -4.975f, 7.846f, -12.616f, 0.0f);
        this.E238D.disp(true);
        this.E238D.setClip(true);
        this.E239 = new Effect(1638, 15.31f, 7.515f, -17.268f, 0.0f);
        this.E239.disp(true);
        this.E239.setClip(true);
        this.E240A = new Effect(1639, -2.472f, 6.567f, 2.76f, 0.0f);
        this.E240A.disp(true);
        this.E240A.setClip(true);
        this.E240AA = new Effect(1639, -2.472f, 6.567f, 2.8f, 0.0f);
        this.E240AA.disp(true);
        this.E240AA.setClip(true);
        this.E240B = new Effect(1639, -2.472f, 6.567f, 6.56f, 0.0f);
        this.E240B.disp(true);
        this.E240B.setClip(true);
        this.E240BB = new Effect(1639, -2.472f, 6.567f, 6.58f, 0.0f);
        this.E240BB.disp(true);
        this.E240BB.setClip(true);
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
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.35f, 1.35f, 1.35f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(2, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(2, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(3, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(3, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(3, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(4, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(4, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(4, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(4, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, -9.5f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, -20.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 15.273f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.01f, 0.01f);
        this.cam0.setCFAngle(15, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(15, 0.01f, 0.01f);
        this.cam0.setCFAngle(16, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(16, 0.01f, 0.01f);
        this.cam0.setCFAngle(17, -28.0f, 0.0f, 0.0f, 2.0f, 40.0f);
        this.cam0.setCFHokan(17, 0.01f, 0.01f);
        this.cam0.setCFLockX(17, 4.0f);
        this.cam0.setCFAngle(18, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(18, 0.01f, 0.01f);
        this.cam0.setCFLockX(18, 4.0f);
        this.cam0.setCFAngle(19, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(19, 0.01f, 0.01f);
        this.cam0.setCFLockX(19, 4.0f);
        this.enemy1 = new Enepc();
        this.enemy1.init(16647, 3, 0.5f, 0.0f, -3.3f, 90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 1, 2, 2);
        float[] fArray = new float[8];
        fArray[0] = 0.5f;
        fArray[2] = -3.3f;
        fArray[3] = -1.0f;
        fArray[4] = 0.5f;
        fArray[6] = -9.8f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 3, 1, 3, fArray2);
        float[] fArray3 = new float[12];
        fArray3[0] = 0.5f;
        fArray3[2] = -3.3f;
        fArray3[3] = 0.5f;
        fArray3[5] = -6.5f;
        fArray3[6] = 0.5f;
        fArray3[8] = -9.8f;
        fArray3[9] = 0.5f;
        fArray3[11] = -6.5f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.kickEnepc(19, 1, 0, 560, 1);
        this.enemy2 = new Enepc();
        this.enemy2.init(16647, 3, 9.0f, 0.0f, -10.0f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 1, 2, 2);
        float[] fArray5 = new float[16];
        fArray5[0] = 9.0f;
        fArray5[2] = -10.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = 9.0f;
        fArray5[6] = -3.0f;
        fArray5[8] = 15.5f;
        fArray5[10] = -3.5f;
        fArray5[11] = 1.0f;
        fArray5[12] = 15.5f;
        fArray5[14] = -10.0f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(1, 3, 2, 3, fArray6);
        float[] fArray7 = new float[12];
        fArray7[0] = 9.0f;
        fArray7[2] = -10.0f;
        fArray7[3] = 9.0f;
        fArray7[5] = -6.0f;
        fArray7[6] = 9.0f;
        fArray7[8] = -3.0f;
        fArray7[9] = 9.0f;
        fArray7[11] = -6.0f;
        float[] fArray8 = fArray7;
        this.enemy2.setParams(fArray8);
        this.enemy3 = new Enepc();
        this.enemy3.init(16647, 3, -1.0f, 0.0f, 11.5f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(1, 1, 2, 2);
        float[] fArray9 = new float[12];
        fArray9[0] = -1.0f;
        fArray9[2] = 11.5f;
        fArray9[3] = -1.0f;
        fArray9[4] = 4.2f;
        fArray9[6] = 11.5f;
        fArray9[8] = 6.6f;
        fArray9[10] = 11.5f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(1, 3, 3, 3, fArray10);
        float[] fArray11 = new float[12];
        fArray11[0] = -1.0f;
        fArray11[2] = 11.5f;
        fArray11[3] = 3.5f;
        fArray11[5] = 11.5f;
        fArray11[6] = 6.6f;
        fArray11[8] = 11.5f;
        fArray11[9] = 3.6f;
        fArray11[11] = 11.5f;
        float[] fArray12 = fArray11;
        this.enemy3.setParams(fArray12);
        this.enemy4 = new Enepc();
        this.enemy4.init(16647, 3, 9.5f, 0.0f, 12.5f, 90.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(1, 1, 2, 2);
        float[] fArray13 = new float[20];
        fArray13[0] = 9.5f;
        fArray13[2] = 12.5f;
        fArray13[3] = -1.0f;
        fArray13[4] = 15.5f;
        fArray13[6] = 12.5f;
        fArray13[8] = 15.5f;
        fArray13[10] = -0.5f;
        fArray13[11] = 1.0f;
        fArray13[12] = 15.5f;
        fArray13[14] = 16.8f;
        fArray13[15] = 1.0f;
        fArray13[16] = 9.5f;
        fArray13[18] = 16.8f;
        fArray13[19] = 3.0f;
        float[] fArray14 = fArray13;
        this.enemy4.setParams(1, 1, 4, 3, fArray14);
        float[] fArray15 = new float[18];
        fArray15[0] = 9.5f;
        fArray15[2] = 12.5f;
        fArray15[3] = 15.5f;
        fArray15[5] = 12.5f;
        fArray15[6] = 15.5f;
        fArray15[8] = 6.5f;
        fArray15[9] = 15.5f;
        fArray15[11] = -0.5f;
        fArray15[12] = 15.5f;
        fArray15[14] = 6.5f;
        fArray15[15] = 15.5f;
        fArray15[17] = 12.5f;
        float[] fArray16 = fArray15;
        this.enemy4.setParams(fArray16);
        if (Runtime.getFlags(3180, 1) == 0) {
            this.enemy5 = new Enepc();
            this.enemy5.init(16403, 17, 8.0f, 5.5f, 5.5f, -90.0f);
            this.enemy5.id = 5;
            this.enemy5.setGroup(4, 4, 4, 4);
            float[] fArray17 = new float[]{8.0f, 5.0f, 5.0f, 1.0f, 3.5f, 5.0f, 5.0f, -1.0f};
            this.enemy5.setParams(1, 4, 5, 17, fArray17);
            float[] fArray18 = new float[]{8.0f, 5.0f, 5.0f, 3.5f, 5.0f, 5.0f};
            this.enemy5.setParams(fArray18);
        }
        this.UTIC1a = new Unit();
        this.UTIC1a.init(20643, 0.5f, 0.0f, 16.2f, 180.0f);
        this.UTIC1b = new Unit();
        this.UTIC1b.init(20643, 3.0f, 0.0f, 16.2f, 180.0f);
        this.UTIC1c = new Unit();
        this.UTIC1c.init(20643, 5.5f, 0.0f, 16.2f, 180.0f);
        this.UTIC1d = new Unit();
        this.UTIC1d.init(20643, -6.5f, 0.0f, 14.5f, 180.0f);
        this.UTIC2a = new Unit();
        this.UTIC2a.init(20644, -5.0f, 0.0f, -6.5f, 90.0f);
        this.UTIC2b = new Unit();
        this.UTIC2b.init(20644, -5.0f, 0.0f, -4.0f, 90.0f);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 371);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 372);
        this.item03 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 373);
        this.item04 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 374);
        this.item05 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 375);
        this.item06 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 376);
        new Uwamono(7, 0, this.item04);
        new Uwamono(8, 4, this.item06);
        new Uwamono(9, 4, this.item05);
        new Uwamono(10, 4);
        new Uwamono(11, 4, this.item02);
        new Uwamono(12, 4);
        new Uwamono(13, 4, this.item03);
        new Uwamono(14, 4);
        new Uwamono(15, 4);
        new Uwamono(17, 0, this.item01);
        if (Runtime.getFlags(3180, 1) == 0) {
            new Uwamono(16, 4, this.enemy5);
        } else {
            new Uwamono(16, 4);
        }
        new Uwamono(28673, 5.0f, 5.0f, 6.7f, 0.0f);
        new Uwamono(28673, 5.9f, 0.0f, -7.3f, 0.0f);
        this.doorA = new Uwamono(97, 40, '\u0001');
        new Uwamono(96, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(3083, 1) == 1) {
            this.PUPU = new Mapunits();
            this.PUPU.mapUnit(39);
            this.PUPU.start(4, null);
            this.PUPU.start(1, "Evt");
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
            this.setElevatorMode(1);
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

        void AD() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele00.getTranslate();
                    ST3040.this.Ele00.setTranslate(ST3040.this.Ele00.px, ST3040.this.Ele00.py + 0.041666668f, ST3040.this.Ele00.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3079, 1, 0);
            ST3040.this.Stop1.setTranslate(ST3040.this.Stop1.px, ST3040.this.Stop1.py - 5.0f, ST3040.this.Stop1.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void AU() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele00.getTranslate();
                    ST3040.this.Ele00.setTranslate(ST3040.this.Ele00.px, ST3040.this.Ele00.py - 0.041666668f, ST3040.this.Ele00.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3079, 1, 1);
            ST3040.this.Stop1.setTranslate(ST3040.this.Stop1.px, ST3040.this.Stop1.py + 5.0f, ST3040.this.Stop1.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void BD() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele01.getTranslate();
                    ST3040.this.Ele01.setTranslate(ST3040.this.Ele01.px, ST3040.this.Ele01.py + 0.041666668f, ST3040.this.Ele01.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3080, 1, 1);
            ST3040.this.Stop2.setTranslate(ST3040.this.Stop2.px, ST3040.this.Stop2.py - 5.0f, ST3040.this.Stop2.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void BU() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele01.getTranslate();
                    ST3040.this.Ele01.setTranslate(ST3040.this.Ele01.px, ST3040.this.Ele01.py - 0.041666668f, ST3040.this.Ele01.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3080, 1, 0);
            ST3040.this.Stop2.setTranslate(ST3040.this.Stop2.px, ST3040.this.Stop2.py + 5.0f, ST3040.this.Stop2.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void CD() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele02.getTranslate();
                    ST3040.this.Ele02.setTranslate(ST3040.this.Ele02.px, ST3040.this.Ele02.py + 0.041666668f, ST3040.this.Ele02.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3081, 1, 0);
            ST3040.this.Stop3.setTranslate(ST3040.this.Stop3.px, ST3040.this.Stop3.py - 5.0f, ST3040.this.Stop3.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void CU() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele02.getTranslate();
                    ST3040.this.Ele02.setTranslate(ST3040.this.Ele02.px, ST3040.this.Ele02.py - 0.041666668f, ST3040.this.Ele02.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3081, 1, 1);
            ST3040.this.Stop3.setTranslate(ST3040.this.Stop3.px, ST3040.this.Stop3.py + 5.0f, ST3040.this.Stop3.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void DD() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele03.getTranslate();
                    ST3040.this.Ele03.setTranslate(ST3040.this.Ele03.px, ST3040.this.Ele03.py + 0.041666668f, ST3040.this.Ele03.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3082, 1, 1);
            ST3040.this.Stop4.setTranslate(ST3040.this.Stop4.px, ST3040.this.Stop4.py - 5.0f, ST3040.this.Stop4.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void DU() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 120) {
                    ST3040.this.Ele03.getTranslate();
                    ST3040.this.Ele03.setTranslate(ST3040.this.Ele03.px, ST3040.this.Ele03.py - 0.041666668f, ST3040.this.Ele03.pz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setFlags(3082, 1, 0);
            ST3040.this.Stop4.setTranslate(ST3040.this.Stop4.px, ST3040.this.Stop4.py + 5.0f, ST3040.this.Stop4.pz);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            ST3040.this.cam0.setMode(0);
        }

        void Evt() {
            ST3040.this.cam0.setMode(-1);
            ST3040.this.EV_Camera08();
            ST3040.this.player.setLocation(1, 2);
            Runtime.setPlayerControl(false);
            ST3040.this.player.setTranslate(15.294f, 10.0f, -18.682f);
            Sound.effectPlay(196749);
            ST3040.this.MJele.setArgs(12, 5.0f);
            System.sleep(180);
            ST3040.this.doorA.SetDoorType('\u0002');
            ST3040.this.doorA.DoorOpen();
            System.sleep(30);
            ST3040.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST3040.this.player.mtn(2, 9, 1.0f, true);
            ST3040.this.player.move(60, 15.294f, -15.414f, true);
            System.sleep(60);
            ST3040.this.doorA.DoorClose();
            ST3040.this.doorA.SetDoorType('\u0004');
            Runtime.setFlags(3083, 1, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }
}

