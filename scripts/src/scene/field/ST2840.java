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
import xeno.map.MC_UTA14_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST2840
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA14_PRJ {
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
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light09;
    Effect light10;
    Effect light11;
    Effect light12;
    Effect light13;
    Effect light14;
    Effect light15;
    Effect light16;
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
    Uwamono box01;
    Uwamono box02;
    Uwamono box03;
    Uwamono box04;
    Uwamono box05;
    Uwamono box06;
    Uwamono box07;
    Uwamono box08;
    Uwamono box09;
    Uwamono box10;
    Uwamono box11;
    Uwamono box12;
    Uwamono box13;
    Uwamono box14;
    Uwamono box15;
    Uwamono box16;
    Uwamono box17;
    Uwamono box18;
    MAPUnit tobira;
    boolean tobira_move = false;
    int green = 0;
    int red = 0;
    int blue = 0;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int lo = 0;
    Effect fade;
    int page;
    String[] annai = new String[]{"It says, \"Song of Nephilim System.\"", "/[waitkey(1)]/[clear()]", "It appears that the room next to it has a device having something to do with sound.", "/[waitkey(64)]/[close()]"};

    ST2840() {
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
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("lo:0");
                this.nwin(this.annai);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void broken(int n) {
        int n2 = 10;
        switch (n) {
            case 1: {
                System.println("1");
                System.println("青");
                System.sleep(n2);
                this.light01.setTranslate(this.light01.px, this.light01.py - 1.0f, this.light01.pz);
                this.hantei(3);
                break;
            }
            case 2: {
                System.println("2");
                System.println("青");
                System.sleep(n2);
                this.light01.setTranslate(this.light01.px, this.light01.py - 1.0f, this.light01.pz);
                this.hantei(3);
                break;
            }
            case 3: {
                System.println("3");
                System.println("青");
                System.sleep(n2);
                this.light01.setTranslate(this.light01.px, this.light01.py - 1.0f, this.light01.pz);
                this.hantei(3);
                break;
            }
            case 4: {
                System.println("4");
                System.println("青");
                System.sleep(n2);
                this.light01.setTranslate(this.light01.px, this.light01.py - 1.0f, this.light01.pz);
                this.hantei(3);
                break;
            }
            case 5: {
                System.println("5");
                System.println("青");
                System.sleep(n2);
                this.light01.setTranslate(this.light01.px, this.light01.py - 1.0f, this.light01.pz);
                this.hantei(3);
                break;
            }
            case 6: {
                System.println("6");
                System.println("赤");
                System.sleep(n2);
                this.light02.setTranslate(this.light02.px, this.light02.py - 1.0f, this.light02.pz);
                this.hantei(2);
                break;
            }
            case 7: {
                System.println("7");
                System.println("赤");
                System.sleep(n2);
                this.light02.setTranslate(this.light02.px, this.light02.py - 1.0f, this.light02.pz);
                this.hantei(2);
                break;
            }
            case 8: {
                System.println("8");
                System.println("赤");
                System.sleep(n2);
                this.light02.setTranslate(this.light02.px, this.light02.py - 1.0f, this.light02.pz);
                this.hantei(2);
                break;
            }
            case 9: {
                System.println("9");
                System.println("赤");
                System.sleep(n2);
                this.light02.setTranslate(this.light02.px, this.light02.py - 1.0f, this.light02.pz);
                this.hantei(2);
                break;
            }
            case 10: {
                System.println("10");
                System.println("赤");
                System.sleep(n2);
                this.light02.setTranslate(this.light02.px, this.light02.py - 1.0f, this.light02.pz);
                this.hantei(2);
                break;
            }
            case 11: {
                System.println("11");
                System.println("緑");
                System.sleep(n2);
                this.light03.setTranslate(this.light03.px, this.light03.py - 1.0f, this.light03.pz);
                this.hantei(1);
                break;
            }
            case 12: {
                System.println("12");
                System.println("緑");
                System.sleep(n2);
                this.light03.setTranslate(this.light03.px, this.light03.py - 1.0f, this.light03.pz);
                this.hantei(1);
                break;
            }
            case 13: {
                System.println("13");
                System.println("緑");
                System.sleep(n2);
                this.light03.setTranslate(this.light03.px, this.light03.py - 1.0f, this.light03.pz);
                this.hantei(1);
                break;
            }
            case 14: {
                System.println("14");
                System.println("緑");
                System.sleep(n2);
                this.light03.setTranslate(this.light03.px, this.light03.py - 1.0f, this.light03.pz);
                this.hantei(1);
                break;
            }
            case 15: {
                System.println("15");
                System.println("緑");
                System.sleep(n2);
                this.light03.setTranslate(this.light03.px, this.light03.py - 1.0f, this.light03.pz);
                this.hantei(1);
                break;
            }
            case 16: {
                System.println("16");
                System.println("青");
                System.sleep(n2);
                this.light01.setTranslate(this.light01.px, this.light01.py - 1.0f, this.light01.pz);
                this.hantei(3);
                break;
            }
            case 17: {
                System.println("17");
                System.println("赤");
                System.sleep(n2);
                this.light02.setTranslate(this.light02.px, this.light02.py - 1.0f, this.light02.pz);
                this.hantei(2);
                break;
            }
            case 18: {
                System.println("18");
                System.println("緑");
                System.sleep(n2);
                this.light03.setTranslate(this.light03.px, this.light03.py - 1.0f, this.light03.pz);
                this.hantei(1);
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
                Runtime.jumpCF(68366, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(2870, 10);
                break;
            }
        }
    }

    void hantei(int n) {
        if (n == 1) {
            ++this.green;
        }
        if (n == 2) {
            ++this.red;
        }
        if (n == 3) {
            ++this.blue;
        }
        if (this.green == 1 && this.red == 4 && this.blue == 3 && Runtime.getFlags(8054, 1) == 0) {
            System.println("成功");
            this.tobira_move = true;
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
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
        Runtime.setIdLightCol(1, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(3, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 2, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightCol(3, 3, 0.6f, 0.6f, 0.6f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        this.tobira = new Mapunits();
        this.tobira.mapUnit(32);
        this.tobira.start(4, null);
        this.light01 = new Effect(1589, -4.0f, 0.0f, -2.0f, 0.0f);
        this.light01.disp(true);
        this.light01.setScale(0.15f, 0.1f, 0.15f);
        this.light02 = new Effect(1590, 0.0f, 0.0f, -9.0f, 0.0f);
        this.light02.disp(true);
        this.light02.setScale(0.15f, 0.1f, 0.15f);
        this.light03 = new Effect(1591, 4.0f, 0.0f, -2.0f, 0.0f);
        this.light03.disp(true);
        this.light03.setScale(0.15f, 0.1f, 0.15f);
        this.light04 = new Effect(1713, -3.7f, 3.0f, 6.8f, 0.0f);
        this.light04.setScale(0.5f, 1.0f, 0.5f);
        this.light05 = new Effect(1713, 3.7f, 3.0f, 6.8f, 0.0f);
        this.light05.setScale(0.5f, 1.0f, 0.5f);
        this.light04.disp(false);
        this.light05.disp(false);
        this.light06 = new Effect(1592, 0.0f, 0.0f, -4.3f, 0.0f);
        this.light07 = new Effect(1593, 0.0f, 0.0f, -4.3f, 0.0f);
        this.light06.disp(false);
        this.light07.disp(false);
        this.light08 = new Effect(1595, -18.9f, 0.0f, -2.3f, 0.0f);
        this.light09 = new Effect(1596, -18.0f, 0.0f, -3.9f, 0.0f);
        this.light10 = new Effect(1597, -17.1f, 0.0f, -2.3f, 0.0f);
        this.light08.disp(false);
        this.light09.disp(false);
        this.light10.disp(false);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 14.0f, 45.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFLockX(1, 0.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 0.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 14.0f, 45.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 0.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, -18.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.box06 = new Uwamono(20, 92);
        this.box06.SetCallNo(6);
        this.box06.SetGravity(true);
        this.box07 = new Uwamono(21, 92);
        this.box07.SetCallNo(7);
        this.box07.SetGravity(true);
        this.box08 = new Uwamono(22, 92);
        this.box08.SetCallNo(8);
        this.box08.SetGravity(true);
        this.box09 = new Uwamono(23, 92);
        this.box09.SetCallNo(9);
        this.box09.SetGravity(true);
        this.box10 = new Uwamono(24, 92);
        this.box10.SetCallNo(10);
        this.box10.SetGravity(true);
        this.box17 = new Uwamono(25, 92);
        this.box17.SetCallNo(17);
        this.box17.SetGravity(true);
        this.box06.SetSe0(196751);
        this.box07.SetSe0(196752);
        this.box08.SetSe0(196750);
        this.box09.SetSe0(196754);
        this.box10.SetSe0(196753);
        this.box17.SetSe0(196755);
        this.box01 = new Uwamono(50, 92);
        this.box01.SetCallNo(1);
        this.box01.SetGravity(true);
        this.box02 = new Uwamono(51, 92);
        this.box02.SetCallNo(2);
        this.box02.SetGravity(true);
        this.box03 = new Uwamono(52, 92);
        this.box03.SetCallNo(3);
        this.box03.SetGravity(true);
        this.box04 = new Uwamono(53, 92);
        this.box04.SetCallNo(4);
        this.box04.SetGravity(true);
        this.box05 = new Uwamono(54, 92);
        this.box05.SetCallNo(5);
        this.box05.SetGravity(true);
        this.box16 = new Uwamono(55, 92);
        this.box16.SetCallNo(16);
        this.box16.SetGravity(true);
        this.box01.SetSe0(196760);
        this.box02.SetSe0(196761);
        this.box03.SetSe0(196762);
        this.box04.SetSe0(196759);
        this.box05.SetSe0(196763);
        this.box16.SetSe0(196764);
        this.box11 = new Uwamono(44, 92);
        this.box11.SetCallNo(11);
        this.box11.SetGravity(true);
        this.box12 = new Uwamono(45, 92);
        this.box12.SetCallNo(12);
        this.box12.SetGravity(true);
        this.box13 = new Uwamono(46, 92);
        this.box13.SetCallNo(13);
        this.box13.SetGravity(true);
        this.box14 = new Uwamono(47, 92);
        this.box14.SetCallNo(14);
        this.box14.SetGravity(true);
        this.box15 = new Uwamono(48, 92);
        this.box15.SetCallNo(15);
        this.box15.SetGravity(true);
        this.box18 = new Uwamono(49, 92);
        this.box18.SetCallNo(18);
        this.box18.SetGravity(true);
        this.box11.SetSe0(196768);
        this.box12.SetSe0(196769);
        this.box13.SetSe0(196770);
        this.box14.SetSe0(196771);
        this.box15.SetSe0(196772);
        this.box18.SetSe0(196767);
        this.tobira.start(1, "idle");
        this.player.setID(1);
        if (Runtime.getFlags(8054, 1) == 1) {
            this.player.setID(2);
            this.tobira.getTranslate();
            this.tobira.setTranslate(this.tobira.px, this.tobira.py - 2.2f, this.tobira.pz);
            this.light04.disp(true);
            this.light05.disp(true);
            this.light06.disp(true);
            this.light07.disp(true);
            Runtime.progressEffect(200);
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2840.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2840.waitPage(this.win, 64);
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
            float f3 = 210.0f;
            ST2840.this.tobira.getTranslate();
            float f4 = 0.0f;
            boolean bl = true;
            while (true) {
                ST2840.this.player.getTranslate();
                if (ST2840.this.player.px > -6.3f) {
                    if (!bl) {
                        ST2840.this.light01.disp(true);
                        ST2840.this.light02.disp(true);
                        ST2840.this.light03.disp(true);
                        ST2840.this.light08.disp(false);
                        ST2840.this.light09.disp(false);
                        ST2840.this.light10.disp(false);
                    }
                    bl = true;
                } else {
                    if (bl) {
                        ST2840.this.light01.disp(false);
                        ST2840.this.light02.disp(false);
                        ST2840.this.light03.disp(false);
                        ST2840.this.light08.disp(true);
                        ST2840.this.light09.disp(true);
                        ST2840.this.light10.disp(true);
                    }
                    bl = false;
                }
                if (ST2840.this.tobira_move) {
                    Runtime.setPlayerControl(false);
                    if (f2 == 0.0f) {
                        Runtime.setPlayerControl(false);
                        Runtime.disable(524288);
                        ST2840.this.cam0.setMode(-1);
                        ST2840.this.camEV = Camera.create(1);
                        ST2840.this.camEV.setRotate(-27.3f, 0.0f, 0.0f);
                        ST2840.this.camEV.setTranslate(0.0f, 7.9f, 8.5f);
                        ST2840.this.camEV.setFov(40.0f);
                        ST2840.this.camEV.change();
                        Sound.effectPlay(196775);
                        ST2840.this.light06.disp(true);
                    }
                    if (f2 == 40.0f) {
                        ST2840.this.light07.disp(true);
                    }
                    if (f2 == 100.0f) {
                        float[] fArray = new float[8];
                        fArray[2] = 7.9f;
                        fArray[3] = 8.5f;
                        fArray[4] = f3 - 100.0f;
                        fArray[6] = 7.8f;
                        fArray[7] = 15.0f;
                        float[] fArray2 = fArray;
                        ST2840.this.camEV.setRotate(-27.3f, 0.0f, 0.0f);
                        ST2840.this.camEV.transSPL(fArray2, 0, 1, (int) f3 - 100);
                    }
                    if (f2 == 180.0f) {
                        ST2840.this.light04.disp(true);
                        ST2840.this.light05.disp(true);
                    }
                    if (f2 >= f3 && f2 < f3 + 180.0f) {
                        ST2840.this.tobira.setTranslate(ST2840.this.tobira.px, Math.sin((f2 - f3) * 3.14f / 180.0f) * 1.1f - 1.1f, ST2840.this.tobira.pz);
                    }
                    if (f2 == f3 + 180.0f) {
                        ST2840.this.player.setID(2);
                        ST2840.this.tobira_move = false;
                        ST2840.this.cam0.setMode(0);
                        Runtime.enable(524288);
                        Runtime.setPlayerControl(true);
                        Runtime.setFlags(8054, 1, 1);
                    }
                    f2 += 1.0f;
                }
                f4 = Math.cos((f * 24.0f - 90.0f) * 3.14f / 180.0f) + 1.3f;
                if (f >= 0.0f && f < 15.0f) {
                    ST2840.this.light10.setScale(1.0f, f4, 1.0f);
                    ST2840.this.light08.setScale(1.0f, 0.3f, 1.0f);
                    ST2840.this.light09.setScale(1.0f, 0.3f, 1.0f);
                }
                if (f >= 15.0f && f < 30.0f) {
                    ST2840.this.light09.setScale(1.0f, f4, 1.0f);
                    ST2840.this.light08.setScale(1.0f, 0.3f, 1.0f);
                    ST2840.this.light10.setScale(1.0f, 0.3f, 1.0f);
                }
                if (f >= 30.0f && f < 45.0f) {
                    ST2840.this.light08.setScale(1.0f, f4, 1.0f);
                    ST2840.this.light10.setScale(1.0f, 0.3f, 1.0f);
                    ST2840.this.light09.setScale(1.0f, 0.3f, 1.0f);
                }
                if (ST2840.this.player.px <= -6.3f) {
                    if (f == 0.0f) {
                        Sound.effectPlay(196767);
                    }
                    if (f == 15.0f) {
                        Sound.effectPlay(196750);
                    }
                    if (f == 30.0f) {
                        Sound.effectPlay(196759);
                    }
                }
                if ((f += 0.25f) == 45.0f) {
                    f = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

