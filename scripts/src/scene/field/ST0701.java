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
import xeno.map.MC_ELS06B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0701
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
    Camera camEV;
    Enepc npc1;
    Enepc ziggy;
    Enepc momo;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int ziggytalked = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono itembox;
    Unit PodA;
    Unit PodB;
    Unit kidou;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect fade;
    Light light = new Light(0);
    int page;
    String[] ZIG_01 = new String[]{"/[label(Ziggy)]", "We must first get to the bridge in order to explain the situation to the person in charge.", "/[waitkey(1)]/[clear()]", "Can you make it?", "/[waitkey(64)]/[close()]"};
    String[] MOM_01 = new String[]{"/[label(MOMO)]", "Tee hee...don't you remember what I said earlier?", "/[waitkey(1)]/[clear()]", "I'm built a lot tougher than I look.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_02 = new String[]{"/[label(Ziggy)]", "That's right. Then, let's go!", "/[waitkey(64)]/[close()]"};

    ST0701() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 5.083f, 12.034f, 1.172f, 120.0f, -0.304f, 2.439f, -2.41f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -30.894f;
        fArray2[2] = 20.899f;
        fArray2[4] = 120.0f;
        fArray2[5] = -15.678f;
        fArray2[6] = -7.779f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 1, 120);
        this.camEV.rotateSPL(fArray3, 1, 0, 120);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.81f, 1.183f, -6.567f);
        this.camEV.setRotate(2.275f, -92.259f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.881f, 2.047f, -5.857f);
        this.camEV.setRotate(-21.104f, 81.439f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.599f, 2.111f, -3.64f);
        this.camEV.setRotate(-15.183f, -43.418f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
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
                Runtime.jumpCF(650, 1);
                break;
            }
        }
    }

    void init() {
        Runtime.resetTakeAgws(8193);
        Runtime.resetTakeAgws(8450);
        Runtime.setPartyData(0x1010000, 5);
        Runtime.setPartyData(65538, 1);
        Runtime.setPartyData(0x1010004, 6);
        Runtime.setPartyData(65542, 2);
        Runtime.setPartyData(0x1010008, 0);
        Runtime.setPartyData(65546, 0);
        Runtime.setFriend(6);
        Runtime.setFriend(4);
        Runtime.setPartyData(16777260, 6);
        System.println("パーティー情報・leader_ziggy,momo,*,*,*,*");
        Runtime.disable(524288);
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
        Sound.effectPlay(196639, 100, 64);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, -0.3f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 0.5f, 3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -0.5f, -3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
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
        this.ziggy = new NPC_NORMAL(6, 12, 0, 2, 3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.ziggy.setLocation(4, 1);
        this.ziggy.talkto("TalkNPC2");
        this.ziggy.disableDTKFlag(8);
        this.ziggy.disableDTKFlag(131072);
        this.ziggy.disableDTKFlag(1);
        this.ziggy.disableDTKFlag(2);
        this.momo = new NPC_NORMAL(4, 13, 0, 3, 5, 0.0f, 0.0f, 0.0f, 0.0f);
        this.momo.setLocation(4, 0);
        this.momo.talkto("TalkNPC3");
        this.momo.disableDTKFlag(8);
        this.momo.disableDTKFlag(131072);
        this.momo.disableDTKFlag(1);
        this.momo.disableDTKFlag(2);
        new Uwamono(419, 4);
        new Uwamono(420, 4);
        new Uwamono(421, 4);
        new Uwamono(422, 0);
        this.itembox = new Uwamono(28677, 0.0f, 0.0f, 0.0f, 270.0f, 1);
        new Uwamono(423, 0, this.itembox);
        new Uwamono(424, 0);
        new Uwamono(425, 0);
        new Uwamono(435, 23);
        new Uwamono(436, 23);
        new Uwamono(437, 23);
        new Uwamono(438, 23);
        new Uwamono(439, 23);
        new Uwamono(440, 23);
        this.doorA = new Uwamono(432, 40, '\u0001');
        new Uwamono(433, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(434, 40, '\u0004');
        this.doorB.SetDoorType('\u0004');
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
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
        this.kidou = new Mapunits();
        this.kidou.mapUnit(44);
        this.kidou.start(4, null);
        this.kidou.start(1, "Evt");
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Evt() {
            System.sleep(1);
            ST0701.this.ziggy.kickEnepc(9, 13);
            System.println("13131313131313131313131313131313131313131313");
            ST0701.this.momo.kickEnepc(9, 12);
            System.println("12121212121212121212121212121212121212121212");
            Runtime.setPlayerControl(false);
            ST0701.this.cam0.setMode(-1);
            ST0701.this.EV_Camera00();
            System.sleep(150);
            ST0701.this.EV_Camera01();
            ST0701.this.ziggy.kickEnepc(4, 1);
            ST0701.this.ziggy.kickEnepc(1, 29);
            ST0701.this.win = Window.create();
            ST0701.this.win.setSize(4, 45);
            ST0701.this.win.setLocation(15, 305);
            ST0701.this.win.print(ST0701.this.ZIG_01, 0);
            System.sleep(150);
            System.waitFor(ST0701.this.win);
            ST0701.this.EV_Camera02();
            ST0701.this.momo.kickEnepc(4, 1);
            ST0701.this.momo.kickEnepc(1, 27);
            ST0701.this.win = Window.create();
            ST0701.this.win.setSize(4, 45);
            ST0701.this.win.setLocation(15, 305);
            ST0701.this.win.print(ST0701.this.MOM_01, 0);
            System.sleep(120);
            System.waitFor(ST0701.this.win);
            System.sleep(30);
            ST0701.this.EV_Camera03();
            ST0701.this.ziggy.kickEnepc(4, 1);
            ST0701.this.ziggy.kickEnepc(0, 7);
            System.sleep(40);
            ST0701.this.ziggy.kickEnepc(0, 9);
            ST0701.this.win = Window.create();
            ST0701.this.win.setSize(4, 45);
            ST0701.this.win.setLocation(15, 305);
            ST0701.this.win.print(ST0701.this.ZIG_02, 0);
            System.sleep(120);
            System.waitFor(ST0701.this.win);
            ST0701.this.ziggy.kickEnepc(4, 0);
            ST0701.this.fade.call(0);
            System.sleep(30);
            Runtime.setPlayerControl(true);
            Runtime.jumpCF(66236, 4);
            ST0701.this.cam0.setMode(0);
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
}

