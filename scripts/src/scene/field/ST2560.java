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
import xeno.map.MC_KAS31_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2560
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KAS31_PRJ {
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
    MAPUnit mapunit;
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
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono co01;
    int lo = 0;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    int page;
    String[] sub_01 = new String[]{"Discovered Segment Address No. 5.", "/[waitkey(64)]/[close()]"};
    String[] sub_02 = new String[]{"It is marked as Segment Address No. 5.", "/[waitkey(64)]/[close()]"};
    String[] sub_03 = new String[]{"Segment Address No.5, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] open = new String[]{"There's a button. Press it?", "/[waitkey(64)]/[close()]"};
    String[] locked = new String[]{"It's locked.", "/[waitkey(64)]/[close()]"};
    String[] annai = new String[]{"'◇Eternal Peace for the Children◇\n", "                       Miltia Park'", "/[waitkey(64)]/[close()]"};
    String[] momo = new String[]{"/[label(MOMO)]", "Huh?", "/[waitkey(64)]/[close()]"};
    String[] jr = new String[]{"/[label(Jr.)]", "Oh?", "/[waitkey(64)]/[close()]"};
    String[] getda = new String[]{"/[color(0x329bbe)]PM Card A/[color(0x808080)] obtained.", "/[waitkey(64)]/[close()]"};

    ST2560() {
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
                Runtime.enable(262144);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("サブルート扉見つけた");
                if (Runtime.getFlags(3205, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setFlags(3205, 1, 1);
                    this.nwin(this.sub_01);
                } else if (Runtime.getFlags(3225, 1) == 0) {
                    this.nwin(this.sub_02);
                } else if (Runtime.getFlags(3285, 1) == 0) {
                    Sound.effectPlay(56);
                    this.nwin(this.sub_03);
                    this.doorC.SetDoorType('\u0004');
                    Runtime.setFlags(3285, 1, 1);
                }
                Runtime.disable(262144);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 1: {
                if (Runtime.getFlags(8076, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("左");
                this.nwin(this.open);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        Runtime.setFlags(8076, 1, 1);
                        this.cam0.setMode(-1);
                        this.cam1 = Camera.create(1);
                        this.cam1.setRotate(-38.3f, 393.7f, 0.0f);
                        this.cam1.setTranslate(9.3f, 6.6f, -16.2f);
                        this.cam1.setFov(40.0f);
                        this.cam1.change();
                        Runtime.enable(65536);
                        this.player.rotY(10, 180.0f, true);
                        System.sleep(10);
                        Sound.effectPlay(196709);
                        this.doorA.DoorOpen();
                        System.sleep(40);
                        Runtime.disable(65536);
                        this.light02.disp(false);
                        this.cam0.setMode(0);
                        Runtime.setPlayerControl(true);
                        this.lo = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 2: {
                if (Runtime.getFlags(8075, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("右");
                this.nwin(this.open);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        Runtime.setFlags(8075, 1, 1);
                        this.cam0.setMode(-1);
                        this.cam1 = Camera.create(1);
                        this.cam1.setRotate(-25.3f, 318.0f, 0.0f);
                        this.cam1.setTranslate(7.1f, 4.8f, -17.0f);
                        this.cam1.setFov(40.0f);
                        this.cam1.change();
                        Runtime.enable(65536);
                        this.player.rotY(10, 180.0f, true);
                        System.sleep(10);
                        Sound.effectPlay(196709);
                        this.doorB.DoorOpen();
                        System.sleep(40);
                        Runtime.disable(65536);
                        this.light01.disp(false);
                        this.cam0.setMode(0);
                        Runtime.setPlayerControl(true);
                        this.lo = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 3: {
                if (Runtime.getFlags(8076, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("左扉");
                this.nwin(this.locked);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 4: {
                if (Runtime.getFlags(8075, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("右扉");
                this.nwin(this.locked);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 5: {
                if (Runtime.getFlags(8093, 1) != 0) break;
                System.println("穴");
                Runtime.setPlayerControl(false);
                this.lo = 1;
                if (Runtime.getLeader() == 4) {
                    this.co01.setTranslate(-1.0f, -3.0f, 10.0f);
                    this.nwin(this.momo);
                    this.get();
                    this.co01.setTranslate(-1.0f, 0.0f, 10.0f);
                } else if (Runtime.getLeader() == 5) {
                    this.co01.setTranslate(-1.0f, -3.0f, 10.0f);
                    this.nwin(this.jr);
                    this.get();
                    this.co01.setTranslate(-1.0f, 0.0f, 10.0f);
                } else {
                    this.co01.setTranslate(-1.0f, 0.0f, 10.0f);
                }
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
            case 6: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.nwin(this.annai);
                this.lo = 0;
                Runtime.setPlayerControl(true);
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
                System.println("瓦礫１【MC_KAS12】・１");
                Runtime.jumpCF(2460, 1);
                break;
            }
            case 1: {
                System.println("地下水道【MC_KAS14】・１");
                Runtime.jumpCF(2480, 1);
                break;
            }
            case 2: {
                System.println("サブルート【MC_KAS34】・１");
                Runtime.jumpCF(2570, 1);
                break;
            }
        }
    }

    void get() {
        Runtime.setFlags(8093, 1, 1);
        Runtime.enable(65536);
        this.player.move(30, -1.5f, 10.0f, true);
        this.player.mtn(2, 1, 1.0f, true);
        System.sleep(33);
        Runtime.addItem(10, 36);
        Sound.effectPlay(6);
        this.nwin(this.getda);
        System.sleep(3);
        this.player.move(30, 0.0f, 10.0f, true);
        this.player.mtn(2, 1, 1.0f, true);
        System.sleep(33);
        Runtime.disable(65536);
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        this.teiten1 = new Uwamono(28690, 8.5f, 0.0f, -30.0f);
        this.teiten1.SetBgm(196619);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 1, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 2, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 3, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1056, 17.1f, 0.9f, -24.5f, 0.0f);
        this.light01.setRotate(-20.0f, 0.0f, 0.0f);
        this.light01.disp(true);
        this.light02 = new Effect(1056, 3.5f, 0.9f, -23.5f, 0.0f);
        this.light02.setRotate(-20.0f, 0.0f, 0.0f);
        this.light02.disp(true);
        float f = 2.7f;
        this.light03 = new Effect(1631, 12.0f, f, -24.2f, 0.0f);
        this.light03.disp(true);
        this.light04 = new Effect(1631, 5.0f, f, -24.2f, 0.0f);
        this.light04.disp(true);
        this.light05 = new Effect(1631, 6.1f, f, -18.0f, 0.0f);
        this.light05.disp(true);
        this.light06 = new Effect(1631, 10.9f, f, -18.0f, 0.0f);
        this.light06.disp(true);
        this.light07 = new Effect(1631, 10.8f, f, -14.0f, 0.0f);
        this.light07.disp(true);
        this.light08 = new Effect(1631, 6.1f, f, -14.0f, 0.0f);
        this.light08.disp(true);
        this.light09 = new Effect(1631, 2.7f, f, -7.5f, 0.0f);
        this.light09.disp(true);
        this.light10 = new Effect(1631, -3.7f, f, -7.5f, 0.0f);
        this.light10.disp(true);
        this.light11 = new Effect(1631, -7.5f, f, -3.6f, 0.0f);
        this.light11.disp(true);
        this.light12 = new Effect(1631, 7.5f, f, -2.7f, 0.0f);
        this.light12.disp(true);
        this.light13 = new Effect(1631, 7.5f, f, 2.7f, 0.0f);
        this.light13.disp(true);
        this.light14 = new Effect(1631, -7.5f, f, 2.7f, 0.0f);
        this.light14.disp(true);
        this.light15 = new Effect(1631, -2.8f, f, 7.5f, 0.0f);
        this.light15.disp(true);
        this.light16 = new Effect(1631, 2.8f, f, 7.5f, 0.0f);
        this.light16.disp(true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        if (Runtime.getFlags(8070, 1) == 0) {
            Runtime.setFlags(8070, 1, 1);
            Runtime.resetOutFriend(1);
            Runtime.resetOutFriend(3);
            Runtime.setOutFriend(6);
            Runtime.setOutFriend(4);
            Runtime.setOutFriend(5);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 1);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
        }
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, -15.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        float[] fArray = new float[12];
        fArray[0] = 1.8f;
        fArray[1] = -3.0f;
        fArray[2] = -15.3f;
        fArray[3] = -1.0f;
        fArray[4] = 2.3f;
        fArray[5] = -3.0f;
        fArray[6] = -15.3f;
        fArray[8] = 1.3f;
        fArray[9] = -3.0f;
        fArray[10] = -15.3f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16390, 1, 1, 16, 3, 1.8f, -3.0f, -15.3f, 0.0f, fArray2);
        this.enemy1.setGroup(0, 0, 0, 0);
        this.co01 = new Uwamono(28672, -1.0f, 0.0f, 10.0f, 0.0f);
        this.co01.SetSize(0.2f, 1.0f, 1.0f);
        this.item1 = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 180.0f, 315);
        this.item1.SetSymbol(28685);
        this.item3 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 312);
        this.item4 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 313);
        new Uwamono(4, 34, this.item1);
        new Uwamono(5, 10, this.item3);
        new Uwamono(6, 2, this.item4);
        if (Runtime.getFlags(3205, 1) == 0) {
            new Uwamono(3, 31);
        } else {
            Stage.setVisible(3, false);
        }
        this.doorA = new Uwamono(0, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorB = new Uwamono(1, 40, '\u0001');
        this.doorB.SetDoorType('\u0002');
        this.doorC = new Uwamono(2, 40, '\u0004');
        this.doorC.SetDoorType('\u0002');
        if (Runtime.getFlags(3285, 1) == 1) {
            this.doorC.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(8076, 1) == 1) {
            this.doorA.SetDoorType('\u0004');
            this.light02.disp(false);
        }
        if (Runtime.getFlags(8075, 1) == 1) {
            this.doorB.SetDoorType('\u0004');
            this.light01.disp(false);
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2560.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2560.waitPage(this.win, 64);
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
    }
}

