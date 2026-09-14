import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS06B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0700
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS06B_PRJ {
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
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int button2_flg = 0;
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
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono item06;
    Uwamono item07;
    Uwamono item08;
    Uwamono item09;
    Uwamono item10;
    Unit PodA;
    Unit PodB;
    Unit trap1;
    Unit trap2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    Effect EF08;
    Effect EF09;
    Effect EF10;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    int page;
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 14.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 14.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 14, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST0700() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3214, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        System.sleep(1);
                        Runtime.enable(262144);
                        System.sleep(1);
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3214, 1, 1);
                        Runtime.setPlayerControl(true);
                        System.sleep(1);
                        Runtime.disable(262144);
                        this.button2_flg = 0;
                        break;
                    }
                    if (Runtime.getFlags(3234, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        System.sleep(1);
                        Runtime.enable(262144);
                        System.sleep(1);
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.button2_flg = 0;
                        System.sleep(1);
                        Runtime.disable(262144);
                        break;
                    }
                    if (Runtime.getFlags(3294, 1) != 0) break;
                    if (this.button2_flg == 1) {
                        return;
                    }
                    this.button2_flg = 1;
                    System.sleep(1);
                    Runtime.enable(262144);
                    System.sleep(1);
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(56);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SUB_03, 0);
                    System.waitFor(this.win);
                    this.doorB.SetDoorType('\u0004');
                    Runtime.setFlags(3294, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.button2_flg = 0;
                    System.sleep(1);
                    Runtime.disable(262144);
                    break;
                }
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66226, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(66226, 3);
                break;
            }
            case 2: {
                Runtime.setFlags(3053, 1, 1);
                Runtime.jumpCF(650, 1);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(3092, 1) == 0) {
            Runtime.setOutFriend(1);
            Runtime.setOutFriend(2);
            Runtime.setOutFriend(3);
            Runtime.resetOutFriend(4);
            Runtime.resetOutFriend(6);
            Runtime.setFlags(3092, 1, 1);
        }
        Stage.setVisible(-1, true);
        this.PodA = new Obj();
        this.PodA.init(20497, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PodA.setTranslate(-0.91f, -0.21f, -15.77f);
        this.PodA.setRotate(18.0f, -39.0f, -41.0f);
        this.PodA.setScale(0.8f, 0.8f, 0.8f);
        this.PodB = new Obj();
        this.PodB.init(20591, 0.0f, 0.0f, 0.0f, 0.0f);
        this.PodB.setTranslate(-0.91f, -0.21f, -15.77f);
        this.PodB.setRotate(18.0f, -39.0f, -41.0f);
        this.PodB.setScale(0.8f, 0.8f, 0.8f);
        this.PodB.setVisible(1, false);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(8, false);
        Stage.setVisible(391, false);
        Stage.setVisible(218, false);
        Stage.setVisible(0, false);
        Stage.setVisible(392, false);
        Stage.setVisible(2, false);
        Stage.setVisible(219, false);
        Stage.setVisible(6, false);
        Stage.setVisible(7, false);
        Stage.setVisible(9, false);
        Stage.setVisible(10, false);
        Stage.setVisible(1, false);
        Stage.setVisible(3, false);
        Stage.setVisible(4, false);
        Stage.setVisible(5, false);
        Stage.setVisible(431, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 0.0f, 0.0f, -10.0f, 0.0f);
        this.teiten1.SetBgm(196639);
        this.teiten2 = new Uwamono(28690, 3.8f, 0.0f, -16.2f, 0.0f);
        this.teiten2.SetBgm(196638);
        Sound.effectStop(196639);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setFog(0, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(1, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(2, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(3, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(4, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(5, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(6, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(7, 8.0f, 20.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, 0.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFLockX(2, 7.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -10.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 7.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 7.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 20.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.enemy1 = new Enepc();
        this.enemy1.init(16641, 3, -2.5f, 0.0f, -1.0f, 135.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 0, 0);
        float[] fArray = new float[28];
        fArray[0] = -2.5f;
        fArray[2] = -1.0f;
        fArray[3] = 1.0f;
        fArray[4] = -1.356f;
        fArray[6] = -1.456f;
        fArray[7] = 2.0f;
        fArray[8] = 0.232f;
        fArray[10] = -2.117f;
        fArray[11] = 3.0f;
        fArray[12] = 1.125f;
        fArray[14] = -3.209f;
        fArray[15] = 4.0f;
        fArray[16] = 1.787f;
        fArray[18] = -4.996f;
        fArray[19] = 5.0f;
        fArray[20] = 2.547f;
        fArray[22] = -7.576f;
        fArray[23] = 6.0f;
        fArray[24] = 2.79f;
        fArray[26] = -10.396f;
        fArray[27] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 0, 1, 3, fArray2);
        this.enemy2 = new Enepc();
        this.enemy2.init(16641, 3, 2.5f, 0.0f, 3.0f, 225.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(0, 0, 0, 0);
        float[] fArray3 = new float[32];
        fArray3[0] = 2.5f;
        fArray3[2] = 3.0f;
        fArray3[3] = 1.0f;
        fArray3[4] = 1.247f;
        fArray3[6] = 3.0f;
        fArray3[7] = 2.0f;
        fArray3[8] = -0.705f;
        fArray3[10] = 3.298f;
        fArray3[11] = 3.0f;
        fArray3[12] = -1.731f;
        fArray3[14] = 4.687f;
        fArray3[15] = 4.0f;
        fArray3[16] = -2.161f;
        fArray3[18] = 6.308f;
        fArray3[19] = 5.0f;
        fArray3[20] = -2.393f;
        fArray3[22] = 7.632f;
        fArray3[23] = 6.0f;
        fArray3[24] = -2.591f;
        fArray3[26] = 8.955f;
        fArray3[27] = 7.0f;
        fArray3[28] = -2.426f;
        fArray3[30] = 10.411f;
        fArray3[31] = -1.0f;
        float[] fArray4 = fArray3;
        this.enemy2.setParams(1, 0, 2, 3, fArray4);
        this.enemy3 = new Enepc();
        this.enemy3.init(16641, 3, -3.0f, 0.0f, 13.0f, 135.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        float[] fArray5 = new float[40];
        fArray5[0] = -3.0f;
        fArray5[2] = 13.0f;
        fArray5[3] = 1.0f;
        fArray5[4] = -2.801f;
        fArray5[6] = 15.084f;
        fArray5[7] = 2.0f;
        fArray5[8] = -2.14f;
        fArray5[10] = 17.069f;
        fArray5[11] = 3.0f;
        fArray5[12] = -2.438f;
        fArray5[14] = 19.683f;
        fArray5[15] = 4.0f;
        fArray5[16] = -2.305f;
        fArray5[18] = 22.131f;
        fArray5[19] = 5.0f;
        fArray5[20] = -2.371f;
        fArray5[22] = 25.175f;
        fArray5[23] = 6.0f;
        fArray5[24] = -0.386f;
        fArray5[26] = 26.664f;
        fArray5[27] = 7.0f;
        fArray5[28] = -1.566f;
        fArray5[30] = 26.697f;
        fArray5[31] = 8.0f;
        fArray5[32] = -1.896f;
        fArray5[34] = 24.679f;
        fArray5[35] = 9.0f;
        fArray5[36] = 0.242f;
        fArray5[38] = 24.348f;
        fArray5[39] = -1.0f;
        float[] fArray6 = fArray5;
        this.enemy3.setParams(1, 0, 3, 3, fArray6);
        this.enemy4 = new Enepc();
        this.enemy4.init(16641, 3, 3.0f, 0.0f, 22.0f, 225.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(0, 0, 1, 1);
        float[] fArray7 = new float[32];
        fArray7[0] = 3.0f;
        fArray7[2] = 22.0f;
        fArray7[3] = 1.0f;
        fArray7[4] = 2.14f;
        fArray7[6] = 19.85f;
        fArray7[7] = 2.0f;
        fArray7[8] = 1.511f;
        fArray7[10] = 16.74f;
        fArray7[11] = 3.0f;
        fArray7[12] = 1.114f;
        fArray7[14] = 14.258f;
        fArray7[15] = 4.0f;
        fArray7[16] = 0.585f;
        fArray7[18] = 12.042f;
        fArray7[19] = 5.0f;
        fArray7[20] = 0.651f;
        fArray7[22] = 9.924f;
        fArray7[23] = 6.0f;
        fArray7[24] = 1.577f;
        fArray7[26] = 7.708f;
        fArray7[27] = 7.0f;
        fArray7[28] = 2.504f;
        fArray7[30] = 5.822f;
        fArray7[31] = -1.0f;
        float[] fArray8 = fArray7;
        this.enemy4.setParams(1, 0, 4, 3, fArray8);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 106);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 107);
        this.item03 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 108);
        this.item04 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 109);
        this.item05 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 110);
        this.item06 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 111);
        this.item07 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 112);
        this.item08 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 113);
        this.item09 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 114);
        this.item10 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 115);
        new Uwamono(419, 4, this.item05);
        new Uwamono(420, 4, this.item09);
        new Uwamono(421, 4, this.item10);
        new Uwamono(422, 0, this.item06);
        this.itembox = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 270.0f, 419);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        new Uwamono(423, 0, this.itembox);
        new Uwamono(424, 0, this.item07);
        new Uwamono(425, 0, this.item08);
        new Uwamono(435, 23);
        new Uwamono(436, 23, this.item03);
        new Uwamono(437, 23, this.item04);
        new Uwamono(438, 23, this.item02);
        new Uwamono(439, 23, this.item01);
        if (Runtime.getFlags(3214, 1) == 0) {
            new Uwamono(440, 23);
        } else {
            Stage.setVisible(440, false);
        }
        this.trap1 = new Uwamono(28673, -3.582f, 0.0f, 8.984f, 0.0f);
        this.trap2 = new Uwamono(28673, 3.794f, 0.0f, 13.19f, 0.0f);
        this.doorA = new Uwamono(432, 40, '\u0001');
        new Uwamono(433, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(3294, 1) == 0) {
            this.doorB = new Uwamono(434, 40, '\u0004');
            this.doorB.SetDoorType('\u0002');
        } else {
            this.doorB = new Uwamono(434, 40, '\u0004');
            this.doorB.SetDoorType('\u0004');
        }
        this.eve00 = new Effect(1443, 0);
        this.eve00.disp(true);
        this.eve01 = new Effect(1443, 1);
        this.eve01.disp(true);
        this.eve02 = new Effect(1443, 2);
        this.eve02.disp(true);
        this.EF01 = new Effect(1521, 0.0f, 0.0f, -11.5f, 0.0f);
        this.EF01.disp(true);
        this.EF02 = new Effect(1402, 3.83f, 0.0f, -16.2f, 0.0f);
        this.EF02.disp(true);
        this.EF03 = new Effect(1515, -2.0f, 0.0f, -10.0f, 0.0f);
        this.EF03.disp(true);
        this.EF04 = new Effect(1401, 0.219f, 0.911f, -9.166f, 0.0f);
        this.EF04.disp(true);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3230, 1, 1);
                break;
            }
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
}

