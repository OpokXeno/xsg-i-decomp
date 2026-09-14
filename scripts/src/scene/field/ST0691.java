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
import xeno.map.MC_ELS05B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0691
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS05B_PRJ {
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
    Enepc kosmos;
    Enepc chaos;
    Enepc momo;
    Enepc ziggy;
    Enepc shion;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int kosmostalked = 0;
    int chaostalked = 0;
    int momotalked = 0;
    int ziggytalked = 0;
    int shiontalked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean kosmosflg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono doorH;
    Uwamono doorI;
    Uwamono trap1;
    Uwamono trap2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int button_flg = 0;
    Unit Y1;
    Unit Y2;
    Unit X1;
    Unit X2;
    Unit kidou;
    Effect EF01;
    Effect eve00;
    Effect eve01;
    Effect eve02;
    Effect eve03;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;
    String[] Loc0_1 = new String[]{"Operating XX floor. Workers within the movement range, please retreat to a safe area immediately.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_01 = new String[]{"/[label(Ziggy)]", "Are you in charge of this ship?", "/[waitkey(64)]/[close()]"};
    String[] SHI_01 = new String[]{"/[label(Shion)]", "Oh, no. I'm just a passenger.", "/[waitkey(1)]/[clear()]", "More importantly, are you two the only ones on that ship?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_02 = new String[]{"/[label(Ziggy)]", "Yes, that's right. I am a Federation government cyborg, Ziggurat...", "/[waitkey(64)]/[close()]"};
    String[] MOM_01 = new String[]{"/[label(MOMO)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_03 = new String[]{"/[label(Ziggy)]", "Call me Ziggy...", "/[waitkey(64)]/[close()]"};
    String[] MOM_02 = new String[]{"/[label(MOMO)]", "*Smile*", "/[waitkey(64)]/[close()]"};
    String[] ZIG_04_01 = new String[]{"/[label(Ziggy)]", "She is MOMO...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_04_02 = new String[]{"/[label(Ziggy)]", "She's a Realian. She is with the Federation government just like me.", "/[waitkey(64)]/[close()]"};
    String[] MOM_03 = new String[]{"/[label(MOMO)]", "Hello, I'm MOMO. Thank you for saving us.", "/[waitkey(64)]/[close()]"};
    String[] SHI_02 = new String[]{"/[label(Shion)]", "I see. I'd like to say, \"Thank goodness you're safe,\"", "/[waitkey(1)]/[clear()]", "but enemy units have invaded the ship. It's way too early to be saying that.", "/[waitkey(1)]/[clear()]", "Who are these people after you?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_05_01 = new String[]{"/[label(Ziggy)]", "The enemy units are Auto-Techs that the U-TIC Organization uses.", "/[waitkey(1)]/[clear()]", "Basically, they're unmanned fighter craft.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_05_02 = new String[]{"/[label(Ziggy)]", "We escaped from their base, and they are the last of the pursuit. Unfortunately, the mother ship has a transfer system.", "/[waitkey(1)]/[clear()]", "The combat units will continue to arrive unless we destroy the mother ship.", "/[waitkey(64)]/[close()]"};
    String[] SHI_03 = new String[]{"/[label(Shion)]", "So unless we do something, the enemy will overrun the ship.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_06 = new String[]{"/[label(Ziggy)]", "I'm sorry for getting you involved in this.", "/[waitkey(64)]/[close()]"};
    String[] SHI_04 = new String[]{"/[label(Shion)]", "What? Oh, no, don't let it bother you. It isn't your fault, Ziggy.", "/[waitkey(1)]/[clear()]", "More importantly, we have to do something about that mother ship!", "/[waitkey(64)]/[close()]"};
    String[] ZIG_07 = new String[]{"/[label(Ziggy)]", "Agreed. We can't do much, but we will help too. It's my responsibility to take care of this.", "/[waitkey(64)]/[close()]"};
    String[] SHI_05 = new String[]{"/[label(Shion)]", "We? As in MOMO too?!", "/[waitkey(64)]/[close()]"};
    String[] ZIG_08 = new String[]{"/[label(Ziggy)]", "Yes, she might not look it, but she's actually more useful than me.", "/[waitkey(64)]/[close()]"};
    String[] SHI_06 = new String[]{"/[label(Shion)]", "Really?", "/[waitkey(64)]/[close()]"};
    String[] MOM_04 = new String[]{"/[label(MOMO)]", "Well, I don't know about that, but I'll do my best!", "/[waitkey(64)]/[close()]"};
    String[] SHI_07 = new String[]{"/[label(Shion)]", "Okay, let's hurry!", "/[waitkey(64)]/[close()]"};

    ST0691() {
    }

    void EV_Camera00() {
        float[] fArray = new float[]{1.0f, 0.737f, 3.055f, -10.733f, 120.0f, 0.737f, 1.935f, -10.733f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -19.355f;
        fArray2[2] = -70.119f;
        fArray2[4] = 120.0f;
        fArray2[5] = -19.355f;
        fArray2[6] = -70.119f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 1, 120);
        this.camEV.rotateSPL(fArray3, 1, 0, 120);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.881f, 1.807f, -12.919f);
        this.camEV.setRotate(-14.683f, 134.737f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.111f, 1.871f, -12.378f);
        this.camEV.setRotate(-11.383f, -108.358f, 0.0f);
        this.camEV.setFov(26.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.826f, 1.743f, -10.393f);
        this.camEV.setRotate(-13.903f, 35.379f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.066f, 1.903f, -12.137f);
        this.camEV.setRotate(-13.803f, 107.218f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        float[] fArray = new float[]{1.0f, 1.987f, 0.399f, -11.448f, 120.0f, 1.987f, 1.263f, -11.448f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -4.743f;
        fArray2[2] = 113.559f;
        fArray2[4] = 120.0f;
        fArray2[5] = -4.743f;
        fArray2[6] = 113.559f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 1, 120);
        this.camEV.rotateSPL(fArray3, 1, 0, 120);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.581f, 1.711f, -12.204f);
        this.camEV.setRotate(-10.283f, 110.059f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera07() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.944f, 1.711f, -11.625f);
        this.camEV.setRotate(-7.703f, 70.759f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera08() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.278f, 1.711f, -9.496f);
        this.camEV.setRotate(-9.677f, -30.259f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera09() {
        float[] fArray = new float[]{1.0f, 5.179f, 2.607f, -10.533f, 120.0f, 5.179f, 1.647f, -10.533f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -9.423f;
        fArray2[2] = 68.319f;
        fArray2[4] = 120.0f;
        fArray2[5] = -9.423f;
        fArray2[6] = 68.319f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 120);
        this.camEV.rotateSPL(fArray3, 1, 3, 120);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera10() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.936f, 1.615f, -12.653f);
        this.camEV.setRotate(-4.703f, 131.178f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera11() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.627f, 1.871f, -10.962f);
        this.camEV.setRotate(-20.043f, -37.539f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera12() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.224f, 2.799f, -9.501f);
        this.camEV.setRotate(-32.623f, 39.219f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera13() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.096f, 1.295f, -11.115f);
        this.camEV.setRotate(0.756f, -52.579f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera14() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.821f, 1.903f, -11.597f);
        this.camEV.setRotate(-11.783f, 95.999f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera15() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.965f, 1.871f, -13.014f);
        this.camEV.setRotate(-22.023f, -145.638f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera16() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.48f, 1.711f, -12.678f);
        this.camEV.setRotate(-9.623f, 121.056f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera17() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.691f, 1.263f, -10.461f);
        this.camEV.setRotate(-0.047f, 300.811f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera18() {
        float[] fArray = new float[]{1.0f, 2.336f, 1.391f, -12.63f, 120.0f, 2.481f, 1.391f, -11.686f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -8.337f;
        fArray2[2] = -171.238f;
        fArray2[4] = 120.0f;
        fArray2[5] = -8.337f;
        fArray2[6] = -171.238f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 120);
        this.camEV.rotateSPL(fArray3, 1, 3, 120);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera19() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.295f, 1.679f, -10.808f);
        this.camEV.setRotate(-11.317f, -39.199f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(66236, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(66216, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(66236, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(66256, 1);
                break;
            }
            case 4: {
                Runtime.jumpCF(66197, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(66197, 2);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(3093, 1) == 0) {
            Runtime.setTakeAgws(8193);
            Runtime.setTakeAgws(8450);
            Runtime.resetOutFriend(1);
            Runtime.resetOutFriend(2);
            Runtime.resetOutFriend(3);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 1);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 2);
            Runtime.setPartyData(65546, 3);
            Runtime.setPartyData(16777260, 1);
            Runtime.setFlags(3093, 1, 1);
        }
        Runtime.disable(524288);
        this.EF01 = new Effect(1417, 0);
        this.EF01.disp(true);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(50, false);
        Stage.setVisible(51, false);
        Stage.setVisible(53, false);
        Stage.setVisible(52, false);
        Stage.setVisible(55, false);
        Stage.setVisible(54, false);
        Stage.setVisible(56, false);
        Stage.setVisible(57, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, -9.4f, 0.0f, 0.0f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten1.SetBgmType('\u0001');
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(1, 0.35f, 1.0f, 0.35f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, -2.0f, 1.0f, 4.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 2.0f, 1.0f, -4.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setFog(1, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(2, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(3, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(4, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(5, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(6, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(7, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(8, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(9, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(10, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(11, 8.0f, 25.0f, 0.2f, 0.8f, 50, 50, 50, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFLockX(1, 9.0f);
        this.cam0.setCFAngle(2, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 9.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, 0.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestal(8, 9.0f, 6.0f, -15.5f, 40.0f, -90.0f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFAngle(9, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.03f, 0.03f);
        this.cam0.setCFAngle(11, 0.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.03f, 0.03f);
        this.cam0.setCFLockX(11, 9.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.kosmos = new NPC_NORMAL(2, 12, 0, 4, 9, 9.281927f, 0.0f, -13.242063f, -65.0f);
        this.kosmos.talkto("Talk_kosmos");
        this.kosmos.enableDTKFlag(4);
        this.kosmos.enableDTKFlag(262144);
        this.kosmos.setMotion(0, 27);
        this.kosmos.dispRadar(false);
        this.chaos = new NPC_NORMAL(3, 13, 0, 5, 7, 9.51838f, 0.0f, -10.665459f, -117.0f);
        this.chaos.talkto("Talk_kosmos");
        this.chaos.enableDTKFlag(4);
        this.chaos.enableDTKFlag(262144);
        this.chaos.setMotion(0, 10);
        this.chaos.dispRadar(false);
        this.momo = new NPC_NORMAL(4, 14, 0, 6, 11, 1.0253974f, 0.0f, -11.013028f, 115.0f);
        this.momo.talkto("Talk_kosmos");
        this.momo.enableDTKFlag(262144);
        this.momo.dispRadar(false);
        this.ziggy = new NPC_NORMAL(6, 15, 0, 7, 5, 1.9867167f, 0.0f, -11.919607f, 90.0f);
        this.ziggy.talkto("Talk_kosmos");
        this.ziggy.enableDTKFlag(262144);
        this.ziggy.dispRadar(false);
        this.shion = new NPC_NORMAL(1, 16, 0, 3, 3, 7.5967145f, 0.0f, -12.255739f, 270.0f);
        this.shion.talkto("Talk_kosmos");
        this.shion.enableDTKFlag(262144);
        this.shion.dispRadar(false);
        new Uwamono(28, 4);
        new Uwamono(29, 0);
        new Uwamono(30, 0);
        new Uwamono(31, 0);
        new Uwamono(32, 4);
        new Uwamono(4, 24);
        new Uwamono(39, 24);
        this.trap1 = new Uwamono(28674, -7.5f, 0.0f, 0.5f, 0.0f);
        this.trap2 = new Uwamono(28674, 0.5f, 0.0f, 22.5f, 0.0f);
        this.kidou = new Mapunits();
        this.kidou.mapUnit(11);
        this.kidou.start(4, null);
        this.kidou.start(1, "Evt");
        this.doorA = new Uwamono(45, 40, '\u0001');
        new Uwamono(60, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorA.SetDoorRange(0.75f);
        this.doorB = new Uwamono(48, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(58, 40, '\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorC.SetSize(0.25f, 2.25f, 1.5f);
        this.doorD = new Uwamono(46, 40, '\u0001');
        this.doorD.SetDoorType('\u0004');
        this.doorD.SetDoorRange(1.4f);
        this.doorE = new Uwamono(44, 40, '\u0001');
        this.doorE.SetDoorType('\u0004');
        if (Runtime.getFlags(3020, 1) == 0) {
            this.player.setID(2);
        } else if (Runtime.getFlags(3020, 1) == 0) {
            this.player.setID(1);
        }
        this.player.setTranslate(0.0f, 0.0f, 0.07f);
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

        void Evt() {
            ST0691.this.cam0.setMode(-1);
            ST0691.this.EV_Camera00();
            Runtime.setPlayerControl(false);
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(9, 15);
            ST0691.this.shion.kickEnepc(1, 1);
            ST0691.this.shion.moveEnepc(15, 3.1506724f, -11.94681f, 60);
            System.sleep(60);
            ST0691.this.shion.kickEnepc(4, 0);
            System.sleep(30);
            ST0691.this.EV_Camera01();
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_01, 0);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 0);
            ST0691.this.EV_Camera02();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(0, 8);
            System.sleep(40);
            ST0691.this.shion.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.SHI_01, 0);
            System.waitFor(ST0691.this.win);
            ST0691.this.shion.kickEnepc(4, 0);
            System.sleep(60);
            ST0691.this.EV_Camera03();
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(2, 7, 0, 19, 1, 75);
            System.sleep(60);
            ST0691.this.ziggy.kickEnepc(0, 9);
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.momo.kickEnepc(9, 15);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_02, 0);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 0);
            ST0691.this.EV_Camera04();
            ST0691.this.momo.kickEnepc(4, 1);
            System.sleep(30);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.MOM_01, 0);
            System.waitFor(ST0691.this.win);
            ST0691.this.momo.kickEnepc(4, 0);
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(9, 14);
            System.sleep(60);
            ST0691.this.ziggy.kickEnepc(2, 7, 0, 19, 1, 75);
            System.sleep(60);
            ST0691.this.momo.kickEnepc(0, 7);
            System.sleep(40);
            ST0691.this.ziggy.kickEnepc(9, 16);
            ST0691.this.ziggy.kickEnepc(0, 9);
            System.sleep(40);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_03, 0);
            System.sleep(90);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 0);
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.momo.kickEnepc(9, 15);
            System.sleep(30);
            ST0691.this.momo.kickEnepc(0, 7);
            System.sleep(30);
            ST0691.this.momo.kickEnepc(4, 0);
            ST0691.this.EV_Camera05();
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.momo.kickEnepc(1, 27);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_04_01, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(0, 9);
            ST0691.this.EV_Camera07();
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_04_02, 0);
            System.sleep(180);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(9, 16);
            ST0691.this.ziggy.kickEnepc(4, 0);
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.momo.moveEnepc(17, 90.0f, 20.0f, 20);
            System.sleep(20);
            ST0691.this.EV_Camera06();
            ST0691.this.momo.kickEnepc(9, 16);
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.momo.kickEnepc(1, 1);
            ST0691.this.momo.moveEnepc(15, 2.6591406f, -10.877264f, 30);
            System.sleep(30);
            ST0691.this.momo.moveEnepc(17, 180.0f, 20.0f, 20);
            System.sleep(20);
            ST0691.this.momo.kickEnepc(0, 7);
            System.sleep(20);
            ST0691.this.momo.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.MOM_03, 0);
            System.sleep(180);
            System.waitFor(ST0691.this.win);
            ST0691.this.momo.kickEnepc(4, 0);
            ST0691.this.shion.kickEnepc(9, 14);
            ST0691.this.EV_Camera08();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.SHI_02, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.shion.kickEnepc(9, 15);
            ST0691.this.shion.kickEnepc(4, 0);
            ST0691.this.EV_Camera09();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(4, 0);
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(2, 7, 0, 19, 1, 75);
            System.sleep(60);
            ST0691.this.ziggy.kickEnepc(1, 9, 16, 145, 1, 75);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_05_01, 0);
            System.sleep(60);
            System.waitFor(ST0691.this.win);
            ST0691.this.EV_Camera10();
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_05_02, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 0);
            System.sleep(60);
            ST0691.this.EV_Camera11();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(0, 27);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.SHI_03, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.shion.kickEnepc(4, 0);
            ST0691.this.EV_Camera12();
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(2, 8, 0, 22, 1, 75);
            System.sleep(60);
            ST0691.this.ziggy.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_06, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 0);
            ST0691.this.EV_Camera13();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(0, 8);
            System.sleep(50);
            ST0691.this.shion.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.SHI_04, 0);
            System.sleep(90);
            System.waitFor(ST0691.this.win);
            ST0691.this.shion.kickEnepc(4, 0);
            ST0691.this.EV_Camera14();
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(2, 7, 0, 19, 1, 75);
            System.sleep(60);
            ST0691.this.ziggy.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_07, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 0);
            System.sleep(60);
            ST0691.this.EV_Camera15();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(9, 14);
            System.sleep(60);
            ST0691.this.shion.kickEnepc(9, 15);
            ST0691.this.shion.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.SHI_05, 0);
            System.sleep(60);
            System.waitFor(ST0691.this.win);
            ST0691.this.shion.kickEnepc(4, 0);
            ST0691.this.EV_Camera16();
            ST0691.this.ziggy.kickEnepc(4, 1);
            ST0691.this.ziggy.kickEnepc(2, 7, 0, 19, 1, 75);
            System.sleep(60);
            ST0691.this.ziggy.kickEnepc(0, 9);
            ST0691.this.ziggy.kickEnepc(9, 14);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.ZIG_08, 0);
            System.waitFor(ST0691.this.win);
            ST0691.this.ziggy.kickEnepc(4, 0);
            ST0691.this.EV_Camera17();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(9, 14);
            ST0691.this.shion.kickEnepc(0, 10);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.SHI_06, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.shion.kickEnepc(4, 0);
            ST0691.this.EV_Camera18();
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.momo.kickEnepc(0, 8);
            System.sleep(60);
            ST0691.this.momo.kickEnepc(4, 1);
            ST0691.this.momo.kickEnepc(0, 27);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.MOM_04, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.momo.kickEnepc(4, 0);
            ST0691.this.EV_Camera19();
            ST0691.this.shion.kickEnepc(4, 1);
            ST0691.this.shion.kickEnepc(0, 7);
            System.sleep(40);
            ST0691.this.shion.kickEnepc(0, 9);
            ST0691.this.win = Window.create();
            ST0691.this.win.setSize(4, 45);
            ST0691.this.win.setLocation(15, 305);
            ST0691.this.win.print(ST0691.this.SHI_07, 0);
            System.sleep(120);
            System.waitFor(ST0691.this.win);
            ST0691.this.shion.kickEnepc(4, 0);
            System.sleep(30);
            ST0691.this.fade.call(0);
            System.sleep(30);
            Runtime.setFlags(7014, 1, 1);
            Runtime.setPlayerControl(true);
            Runtime.setPartyData(0x1010000, 5);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 2);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 3);
            Runtime.setPartyData(65546, 3);
            Runtime.jumpCF(66226, 7);
            ST0691.this.cam0.setMode(0);
        }
    }
}

