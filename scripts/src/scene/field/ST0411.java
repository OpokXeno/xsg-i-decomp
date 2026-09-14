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
import xeno.map.MC_VOK12B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class ST0411
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK12B_PRJ {
    Player player;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc npc9;
    Enepc npc10;
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
    int testXXX = 0;
    int shion01 = 0;
    int passed1 = 0;
    int passed2 = 0;
    int passed3 = 0;
    int passed4 = 0;
    int button_flg = 0;
    int button2_flg = 0;
    int button3_flg = 0;
    int Loc_flg = 0;
    int test = 0;
    int guno4pass = 0;
    int escape = 0;
    int passloc = 0;
    int gunoyoke = 0;
    int ene2look = 0;
    int g4serch = 0;
    int Tuika = 0;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono doorH;
    Uwamono Glass;
    Uwamono lc;
    Uwamono itembox;
    Uwamono trap;
    Uwamono col1;
    Uwamono col2;
    Uwamono obj01;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono Base1;
    Uwamono SAVE;
    Unit monitor1;
    Unit kidou;
    Unit okazu;
    Thread SE01;
    Thread SE02;
    Thread SE03;
    Effect FadeIn01;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    Effect EF08;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int smd = 0;
    int finalflg;
    int page;
    int E4flg;
    String[] Npc_1_0 = new String[]{"My girlfriend went out there unarmed!!", "/[waitkey(1)]/[clear()]", "Meanwhile, all I can do is cower right here. I'm so pathetic...", "/[waitkey(64)]/[close()]"};
    String[] Npc_1_1 = new String[]{"I-I-It's swarming with Gnosis up ahead. It's practically impossible to move from here!!", "/[waitkey(1)]/[clear()]", "At this rate, my girlfriend's going to get eaten alive by those monsters!!", "/[waitkey(64)]/[close()]"};
    String[] Npc_2_0 = new String[]{"I-I never thought...that these kinds of monsters really existed...", "/[waitkey(1)]/[clear()]", "T-they really do e-exist...and there are so m-many of them...", "/[waitkey(64)]/[close()]"};
    String[] Npc_2_1 = new String[]{"I...s-should have listened...to what he said...", "/[waitkey(64)]/[close()]"};
    String[] Npc_2_2 = new String[]{"She's dead.", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_0 = new String[]{"Did you see any Gnosis on your way here?", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_1 = new String[]{"Some of them show interest in moving objects.", "/[waitkey(1)]/[clear()]", "Things like images on monitors.", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_2 = new String[]{"I see.\n", "I wish you luck!!", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_3 = new String[]{"There's nothing to be ashamed of! You should wait here for help to come, too!", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_4 = new String[]{"If you insist on going no matter what, I'll tell you something about them...", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_5 = new String[]{"...About those monsters' behavior.", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_6 = new String[]{"Well, with a little luck, you might manage to escape.", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_7 = new String[]{"Damn!! With so many Gnosis crawling around all over the place, we can't go anywhere!!", "/[waitkey(1)]/[clear()]", "Even if we go on ahead, who knows how many of them will be waiting for us!", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_8 = new String[]{"You know, don't you? Even the slightest amount of contact with the Gnosis means certain death!", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_9 = new String[]{"If we can draw their attention away somehow, maybe we could get past them.", "/[waitkey(64)]/[close()]"};
    String[] Npc_3_10 = new String[]{"Wait a minute!!", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_0 = new String[]{"!!\n", "Oh, you're from Vector.", "/[waitkey(1)]/[clear()]", "By the way, do you know...?", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_1 = new String[]{"Damn!! Why do scientists act like know-it-alls when they actually don't know anything!", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_2 = new String[]{"Hey?! You're panicking aren't you?!", "/[waitkey(1)]/[clear()]", "You should take the time to listen to me BECAUSE it's an emergency!!", "/[waitkey(64)]/[close()]"};
    String[] Npc_4_3 = new String[]{"Listen closely to what people have to say!! Everything will work out no matter what if you obey that rule!", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_0 = new String[]{"There is a switch.\n", "Press it?", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_1 = new String[]{"Activating ship monitors.", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_2 = new String[]{"/[label(Shion)]", "(I better not.)", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_3 = new String[]{"The ship monitor controls are broken.", "/[waitkey(64)]/[close()]"};
    String[] Sion_0 = new String[]{"/[label(Shion)]", "(Looks like I managed to get past them.)", "/[waitkey(64)]/[close()]"};
    String[] Sion_1 = new String[]{"/[label(Shion)]", "(Anyway, I should hurry and find someone.)", "/[waitkey(64)]/[close()]"};
    String[] Sion_2 = new String[]{"/[label(Shion)]", "C-could it be the entire ship has been taken over by Gnosis?!", "/[waitkey(64)]/[close()]"};
    String[] Sion_3 = new String[]{"/[label(Shion)]", "I wonder if Allen and the others are okay. It's impossible for them to try to defeat the Gnosis without KOS-MOS.\n", "Anyway, it's over if I get caught! I have to get to KOS-MOS somehow without coming in contact with the enemy!!", "/[waitkey(64)]/[close()]"};
    String[] Sion_4 = new String[]{"/[label(Shion)]", "Partition switch? It's better than nothing, I suppose.", "/[waitkey(1)]/[clear()]", "Anyway, I have to get away from here quickly before the Gnosis notice me!", "/[waitkey(64)]/[close()]"};
    String[] Sion_5 = new String[]{"/[label(Shion)]", "Until I can activate the Hilbert Effect, it'll be useless to fight them...", "/[waitkey(64)]/[close()]"};
    String[] Sion_5_1 = new String[]{"/[label(Shion)]", "It might be dangerous to get any closer.", "/[waitkey(64)]/[close()]"};
    String[] Sion_6 = new String[]{"/[label(Shion)]", "Shoot?!\n", "They saw me?!", "/[waitkey(64)]/[close()]"};
    String[] Sion_7 = new String[]{"/[label(Shion)]", "Those guys?", "/[waitkey(64)]/[close()]"};
    String[] Sion_8 = new String[]{"/[label(Shion)]", "I have to get past here in order to find everybody.", "/[waitkey(1)]/[clear()]", "But the Gnosis looks too powerful for me to take on...", "/[waitkey(64)]/[close()]"};
    String[] Sion_9 = new String[]{"/[label(Shion)]", "Looks like they're fixated by the monitor. It's definitely just like he said.", "/[waitkey(64)]/[close()]"};
    String[] Sion_10 = new String[]{"/[label(Shion)]", "I might be able to slip by without having to fight.", "/[waitkey(64)]/[close()]"};
    String[] Sion_11 = new String[]{"/[label(Shion)]", "There's no reason to put myself in more danger.", "/[waitkey(64)]/[close()]"};
    String[] Sion_12 = new String[]{"/[label(Shion)]", "!!", "/[waitkey(64)]/[close()]"};
    String[] Sion_13 = new String[]{"/[label(Shion)]", "No response. Was the bridge attacked too?!", "/[waitkey(64)]/[close()]"};
    String[] Sion_14 = new String[]{"/[label(Shion)]", "The bridge is sealed off?! ...This can't be good.", "/[waitkey(64)]/[close()]"};
    String[] Sion_15 = new String[]{"/[label(Shion)]", "Turning back now isn't going to get me anywhere. I better find another way.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 16, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST0411() {
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, 28.327f, 1.189f, 0.813f, 90.0f, 28.077f, 1.189f, -0.104f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -1.889f;
        fArray2[2] = 105.243f;
        fArray2[4] = 90.0f;
        fArray2[5] = -1.899f;
        fArray2[6] = 105.243f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        float[] fArray = new float[]{1.0f, -16.841f, 4.866f, 26.471f, 120.0f, -16.841f, 2.53f, 26.471f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -31.427f;
        fArray2[2] = 18.119f;
        fArray2[4] = 120.0f;
        fArray2[5] = -13.307f;
        fArray2[6] = 18.119f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 0, 120);
        this.camEV.rotateSPL(fArray3, 1, 0, 120);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03a() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-16.841f, 2.53f, 26.471f);
        this.camEV.setRotate(-13.307f, 18.119f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        float[] fArray = new float[]{1.0f, -6.644f, 2.594f, 18.994f, 120.0f, -10.913f, 1.954f, 26.105f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -7.505f;
        fArray2[2] = 86.578f;
        fArray2[4] = 120.0f;
        fArray2[5] = -6.425f;
        fArray2[6] = -3.381f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 0, 120);
        this.camEV.rotateSPL(fArray3, 1, 0, 120);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera04a() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-10.913f, 1.954f, 26.105f);
        this.camEV.setRotate(-6.425f, -3.381f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.263f, 0.994f, 30.789f);
        this.camEV.setRotate(9.992f, 151.079f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        float[] fArray = new float[]{1.0f, -13.593f, 1.666f, 31.001f, 90.0f, -13.593f, 0.962f, 31.001f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = 14.434f;
        fArray2[2] = 159.816f;
        fArray2[4] = 90.0f;
        fArray2[5] = 14.434f;
        fArray2[6] = 159.816f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera07() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.407f, 1.73f, 23.127f);
        this.camEV.setRotate(-7.972f, 128.379f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera08() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-14.713f, 1.282f, 28.116f);
        this.camEV.setRotate(10.64f, 26.999f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera09() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-12.221f, 1.57f, 24.853f);
        this.camEV.setRotate(-17.425f, -58.659f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera10() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.077f, 1.378f, 23.294f);
        this.camEV.setRotate(2.834f, 73.558f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera11() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.586f, 0.835f, 21.0f);
        this.camEV.setRotate(3.342f, 90.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera12() {
        float[] fArray = new float[]{1.0f, 29.306f, 1.475f, 4.491f, 120.0f, 29.698f, 1.475f, 2.485f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -0.139f;
        fArray2[2] = 78.939f;
        fArray2[4] = 120.0f;
        fArray2[5] = -0.139f;
        fArray2[6] = 78.939f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 120);
        this.camEV.rotateSPL(fArray3, 1, 2, 120);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera12_01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(29.698f, 1.475f, 2.485f);
        this.camEV.setRotate(-0.139f, 78.939f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera13() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(23.717f, 1.477f, 20.547f);
        this.camEV.setRotate(-4.405f, 21.7f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera14() {
        float[] fArray = new float[]{1.0f, 0.536f, 4.139f, -15.499f, 270.0f, 0.536f, 4.139f, -15.499f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -18.443f;
        fArray2[2] = -62.517f;
        fArray2[4] = 270.0f;
        fArray2[5] = -18.443f;
        fArray2[6] = -78.936f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 135);
        this.camEV.rotateSPL(fArray3, 1, 2, 135);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera15() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.006f, 1.542f, -20.078f);
        this.camEV.setRotate(-10.706f, 35.36f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera16() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(18.228f, 0.307f, 13.67f);
        this.camEV.setRotate(8.419f, -17.918f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera17() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(26.76f, 2.373f, 2.514f);
        this.camEV.setRotate(-37.549f, 40.503f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera19() {
        float[] fArray = new float[]{1.0f, 0.118f, 4.742f, -16.639f, 150.0f, 0.118f, 2.502f, -16.639f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -14.345f;
        fArray2[2] = -76.018f;
        fArray2[4] = 150.0f;
        fArray2[5] = -14.345f;
        fArray2[6] = -76.018f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 150);
        this.camEV.rotateSPL(fArray3, 1, 2, 150);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera20() {
        float[] fArray = new float[]{1.0f, -12.899f, 1.794f, 31.105f, 90.0f, -12.899f, 1.794f, 31.105f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -9.245f;
        fArray2[2] = 8.974f;
        fArray2[4] = 90.0f;
        fArray2[5] = -9.245f;
        fArray2[6] = 130.054f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera21() {
        float[] fArray = new float[]{1.0f, 13.637f, 3.526f, -27.28f, 205.0f, 13.637f, 2.662f, -27.28f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -11.345f;
        fArray2[2] = 149.177f;
        fArray2[4] = 205.0f;
        fArray2[5] = -11.345f;
        fArray2[6] = 149.177f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 205);
        this.camEV.rotateSPL(fArray3, 1, 2, 205);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera22() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-9.269f, 3.074f, 26.855f);
        this.camEV.setRotate(-14.345f, 140.594f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera23() {
        float[] fArray = new float[]{1.0f, -9.269f, 3.074f, 26.855f, 60.0f, -9.269f, 3.074f, 26.855f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -14.345f;
        fArray2[2] = 140.594f;
        fArray2[4] = 60.0f;
        fArray2[5] = -14.345f;
        fArray2[6] = 7.155f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 60);
        this.camEV.rotateSPL(fArray3, 1, 3, 60);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera24() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(21.458f, 1.177f, 1.173f);
        this.camEV.setRotate(3.318f, 7.819f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera25() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(20.609f, 3.939f, 25.039f);
        this.camEV.setRotate(-23.031f, 61.079f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            case 5: {
                if (this.finalflg == 1) {
                    return;
                }
                this.enemy5.kickEnepc(10, 50, 0);
                System.println("pass3");
                this.finalflg = 1;
                break;
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
            if (this.escape != 0) return;
            switch (n) {
                case 100: {
                    this.enemy2.setTranslate(-1.0f, 0.0f, 21.0f);
                    this.enemy2.kickEnepc(4, 1);
                    this.enemy2.kickEnepc(1, 27);
                    this.cam0.setMode(-1);
                    this.EV_Camera11();
                    Stage.setVisible(23, true);
                    Runtime.setPlayerControl(false);
                    this.enemy2.moveEnepc(17, 90.0f, 20.0f, 15);
                    System.sleep(15);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_12, 0);
                    System.waitFor(this.win);
                    this.enemy2.kickEnepc(1, 3);
                    this.player.getTranslate();
                    this.enemy2.moveEnepc(15, this.player.px - 2.0f, 21.0f, 60);
                    System.sleep(60);
                    Runtime.setPlayerControl(true);
                    this.enemy2.kickEnepc(14, 0);
                    return;
                }
            }
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (this.g4serch != 1) return;
                    System.println("serch_off!!!!!!!!!!!!!!!!!!!!!!");
                    this.enemy4.setES_mGiveUp(0.0f);
                    this.enemy4.setES_cRange(120.0f, 2.0f, 2.0f);
                    this.g4serch = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    if (this.g4serch == 1) {
                        Sound.effectPlay(196756);
                        Runtime.disable(524288);
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.cam0.setMode(-1);
                        this.EV_Camera21();
                        this.player.setTranslate(8.746641f, 0.0f, -21.80871f);
                        this.player.setRotate(0.0f, -30.0f, 0.0f);
                        this.enemy4.kickEnepc(4, 3);
                        this.enemy4.mtn(2, 9, 1.0f, true);
                        this.enemy4.move(60, 6.66f, -17.0f, true);
                        System.sleep(65);
                        this.enemy4.rotY(60, 160.0f, true);
                        System.sleep(65);
                        this.enemy4.mtn(28, 1, 1.0f, true);
                        this.player.mtn(11, 1, 1.0f, true);
                        System.sleep(70);
                        this.EV_Camera19();
                        this.enemy4.mtn(2, 9, 1.0f, true);
                        this.enemy4.move(60, 17.942f, -17.378f, true);
                        System.sleep(65);
                        this.enemy4.move(15, 19.739f, -16.711f, true);
                        System.sleep(20);
                        this.enemy4.move(15, 20.568f, -14.634f, true);
                        System.sleep(20);
                        this.enemy4.move(15, 20.653f, -12.05f, true);
                        System.sleep(20);
                        this.enemy4.rotY(20, 0.0f, true);
                        System.sleep(25);
                        this.enemy4.kickEnepc(4, 0);
                        this.enemy4.kickEnepc(7, 3);
                        this.guno4pass = 1;
                        this.EV_Camera15();
                        this.player.mtn(28, 1, 1.0f, true);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Sion_0, 0);
                        System.waitFor(this.win);
                        this.cam0.setMode(0);
                        this.g4serch = 2;
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        Runtime.enable(524288);
                        return;
                    }
                    if (this.g4serch != 2) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_1, 0);
                    System.waitFor(this.win);
                    System.sleep(30);
                    this.FadeIn01.call(0);
                    System.sleep(30);
                    Runtime.setPlayerControl(true);
                    Runtime.setFlags(3009, 1, 1);
                    if (Runtime.getFlags(40, 1) == 0) {
                        Runtime.setFlags(40, 1, 1);
                        Runtime.jumpEvent(1320);
                        return;
                    }
                    Runtime.jumpCF(65917, 2);
                    return;
                }
            }
            return;
        }
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    if (this.g4serch != 0) return;
                    System.println("serch_on!!!!!!!!!!!!!!!!!!!!!!!");
                    this.enemy4.setES_mGiveUp(15.0f);
                    this.enemy4.setES_cRange(360.0f, 8.0f, 8.0f);
                    this.g4serch = 1;
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
                        Runtime.enable(65536);
                        this.player.setTranslate(-0.6503207f, 0.0f, 19.406563f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.player.mtn(12, 1, 1.0f, true);
                        System.sleep(50);
                        this.Kakuheki_Close();
                        System.sleep(10);
                        this.EF03.disp(false);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.npc1talked = 1;
                        return;
                    }
                    if (this.npc1talked != 1) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.setTranslate(-0.6503207f, 0.0f, 19.406563f);
                    this.player.setRotate(0.0f, 180.0f, 0.0f);
                    this.player.mtn(12, 1, 1.0f, true);
                    System.sleep(50);
                    this.Kakuheki_Open();
                    System.sleep(10);
                    this.EF03.disp(true);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    this.npc1talked = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 5) {
            switch (n) {
                case 100: {
                    if (this.npc1talked != 0) {
                        if (this.npc1talked != 1) return;
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.setTranslate(6.386705f, 0.0f, 19.406563f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.player.mtn(12, 1, 1.0f, true);
                        System.sleep(50);
                        this.Kakuheki_Open();
                        System.sleep(10);
                        this.EF02.disp(true);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.npc1talked = 0;
                        return;
                    }
                    if (Runtime.getFlags(3034, 1) == 0) {
                        Runtime.disable(524288);
                        this.player.setShadow(0, 0);
                        this.enemy2.setTranslate(-1.0f, 0.0f, 21.0f);
                        this.enemy2.kickEnepc(4, 1);
                        this.enemy2.kickEnepc(1, 27);
                        this.cam0.setMode(-1);
                        this.EV_Camera11();
                        Stage.setVisible(23, true);
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.setTranslate(6.386705f, 0.0f, 19.406563f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.player.mtn(12, 1, 1.0f, true);
                        System.sleep(50);
                        this.SE01.stop();
                        this.Kakuheki_Close();
                        System.sleep(10);
                        this.EF03.disp(false);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.player.setShadow(4, 16);
                        this.cam0.setMode(0);
                        Stage.setVisible(23, false);
                        this.enemy2.setTranslate(-11.0f, 0.0f, 19.27f);
                        this.escape = 1;
                        Runtime.setFlags(3034, 1, 1);
                        this.enemy2.kickEnepc(7, 17);
                        Runtime.enable(524288);
                        return;
                    }
                    if (Runtime.getFlags(3034, 1) != 1) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.mtn(11, 1, 1.0f, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_11, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    return;
                }
            }
            return;
        }
        if (n2 == 6) {
            switch (n) {
                case 100: {
                    if (this.Loc_flg == 0) {
                        if (this.button_flg == 1) {
                            return;
                        }
                        this.button_flg = 1;
                        Runtime.disable(524288);
                        Runtime.enable(65536);
                        this.player.setTranslate(26.018f, 0.025999f, 0.907f);
                        this.player.setRotate(0.0f, 270.0f, 0.0f);
                        this.player.mtn(26, 1, 1.0f, true);
                        this.player.setShadow(0, 0);
                        this.cam0.setMode(-1);
                        this.EV_Camera01();
                        System.sleep(45);
                        Sound.effectPlay(196751);
                        System.sleep(45);
                        Sound.effectPlay(196752);
                        this.EF01.disp(false);
                        this.moooon();
                        this.npc8.kickEnepc(10, 80, 30);
                        this.npc9.kickEnepc(10, 80, 30);
                        System.sleep(30);
                        this.enemy3.kickEnepc(4, 1);
                        this.enemy3.kickEnepc(1, 1);
                        this.enemy3.moveEnepc(17, -90.0f, 1.0f, 30);
                        System.sleep(30);
                        this.enemy3.moveEnepc(15, 20.0f, 1.0f, 60);
                        System.sleep(60);
                        this.enemy3.kickEnepc(1, 27);
                        this.enemy3.kickEnepc(7, 17);
                        this.player.setShadow(4, 16);
                        this.cam0.setMode(0);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.Loc_flg = 1;
                        Runtime.enable(524288);
                        this.button_flg = 0;
                        return;
                    }
                    if (this.Loc_flg != 1) return;
                    if (this.button_flg == 1) {
                        return;
                    }
                    Runtime.setPlayerControl(false);
                    this.button_flg = 1;
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Loc_6_3, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.button_flg = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 7) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3216, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        Sound.effectPlay(55);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_01, 0);
                        System.waitFor(this.win);
                        Runtime.setFlags(3216, 1, 1);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    }
                    if (Runtime.getFlags(3236, 1) == 0) {
                        if (this.button2_flg == 1) {
                            return;
                        }
                        this.button2_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.SUB_02, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button2_flg = 0;
                        return;
                    }
                    if (Runtime.getFlags(3296, 1) != 0) return;
                    if (this.button2_flg == 1) {
                        return;
                    }
                    this.button2_flg = 1;
                    this.off();
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(56);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SUB_03, 0);
                    System.waitFor(this.win);
                    this.doorF.SetDoorType('\u0004');
                    Runtime.setFlags(3296, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.on();
                    this.button2_flg = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 8) {
            switch (n) {
                case 100: {
                    if (this.shion01 == 0) {
                        this.player.setLocation(1, 5);
                        Runtime.setPlayerControl(false);
                        this.cam0.setMode(-1);
                        this.EV_Camera08();
                        Stage.setVisible(23, true);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Sion_5, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.shion01 = 1;
                        this.cam0.setMode(0);
                        Stage.setVisible(23, false);
                        return;
                    }
                    if (this.shion01 != 1) return;
                    this.player.setLocation(1, 5);
                    Runtime.setPlayerControl(false);
                    this.cam0.setMode(-1);
                    this.EV_Camera08();
                    Stage.setVisible(23, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_5_1, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.shion01 = 0;
                    this.cam0.setMode(0);
                    Stage.setVisible(23, false);
                    return;
                }
            }
            return;
        }
        if (n2 == 9) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3033, 1) != 0) return;
                    this.player.setLocation(1, 6);
                    Runtime.setPlayerControl(false);
                    this.cam0.setMode(-1);
                    this.EV_Camera09();
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_4, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.cam0.setMode(0);
                    Runtime.setFlags(3033, 1, 1);
                    return;
                }
            }
            return;
        }
        if (n2 == 10) {
            switch (n) {
                case 100: {
                    if (this.ene2look != 0) return;
                    this.SE02.stop();
                    this.SE03.stop();
                    this.enemy2.kickEnepc(4, 1);
                    this.enemy2.setMotion(2, 3);
                    this.enemy2.setMotion(0, 0);
                    this.enemy2.kickEnepc(8, 100);
                    this.enemy2.kickEnepc(4, 0);
                    this.enemy2.kickEnepc(1, 27);
                    this.enemy2.kickEnepc(7, 10);
                    this.ene2look = 1;
                    return;
                }
            }
            return;
        }
        if (n2 == 11) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3037, 1) == 0) {
                        if (Runtime.getFlags(3036, 1) != 0) return;
                        Runtime.setPlayerControl(false);
                        this.enemy3.kickEnepc(4, 1);
                        this.enemy3.kickEnepc(1, 27);
                        this.cam0.setMode(-1);
                        Stage.setVisible(23, true);
                        Stage.setVisible(28, true);
                        this.EV_Camera24();
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Sion_8, 0);
                        System.waitFor(this.win);
                        Stage.setVisible(23, false);
                        Stage.setVisible(28, false);
                        this.cam0.setMode(0);
                        this.enemy3.kickEnepc(4, 0);
                        Runtime.setPlayerControl(true);
                        Runtime.setFlags(3036, 1, 1);
                        return;
                    }
                    if (this.Loc_flg != 0) return;
                    this.enemy3.kickEnepc(4, 3);
                    System.println("*****************************************");
                    System.sleep(1);
                    this.enemy3.mtn(2, 9, 1.0f, true);
                    System.sleep(5);
                    this.enemy3.move(60, 21.0f, -2.0f, true);
                    System.sleep(60);
                    this.enemy3.kickEnepc(4, 0);
                    this.enemy3.kickEnepc(7, 24);
                    Runtime.setFlags(3037, 1, 0);
                    return;
                }
            }
            return;
        }
        if (n2 == 12) {
            switch (n) {
                case 100: {
                    if (this.Loc_flg != 0) return;
                    if (Runtime.getFlags(3037, 1) != 0) return;
                    Runtime.setFlags(3037, 1, 1);
                    this.enemy3.kickEnepc(7, 16);
                    return;
                }
            }
            return;
        }
        if (n2 == 13) {
            switch (n) {
                case 100: {
                    if (this.Loc_flg != 1) return;
                    if (this.gunoyoke == 0) {
                        Runtime.setPlayerControl(false);
                        this.player.getTranslate();
                        this.player.setTranslate(this.player.px + 0.2f, this.player.py, this.player.pz);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Sion_9, 0);
                        System.waitFor(this.win);
                        this.gunoyoke = 1;
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px + 0.2f, this.player.py, this.player.pz);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_10, 0);
                    System.waitFor(this.win);
                    this.gunoyoke = 0;
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 14) {
            switch (n) {
                case 100: {
                    if (this.Loc_flg != 1) return;
                    if (this.gunoyoke == 0) {
                        Runtime.setPlayerControl(false);
                        this.player.getTranslate();
                        this.player.setTranslate(this.player.px + 0.2f, this.player.py, this.player.pz);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Sion_9, 0);
                        System.waitFor(this.win);
                        this.gunoyoke = 1;
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px + 0.2f, this.player.py, this.player.pz);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_10, 0);
                    System.waitFor(this.win);
                    this.gunoyoke = 0;
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 == 15) {
            switch (n) {
                case 100: {
                    if (this.Loc_flg != 0) return;
                    System.println("pipopapopipopapopipopapopipopapopipopapopipopapopipopapopipopapo");
                    this.enemy3.setES_mGiveUp(0.115f, 0.18f);
                    return;
                }
            }
            return;
        }
        if (n2 == 16) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3018, 1) == 0) {
                        if (this.button3_flg == 1) {
                            return;
                        }
                        this.button3_flg = 1;
                        this.off();
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Sion_14, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        this.on();
                        this.button3_flg = 0;
                        return;
                    }
                    if (this.button3_flg == 1) {
                        return;
                    }
                    this.button3_flg = 1;
                    this.off();
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_13, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.on();
                    this.button3_flg = 0;
                    return;
                }
            }
            return;
        }
        if (n2 == 17) {
            switch (n) {
                case 100: {
                    if (this.escape != 0) return;
                    this.cam0.setMode(-1);
                    this.EV_Camera25();
                    Runtime.setPlayerControl(false);
                    this.enemy2.kickEnepc(4, 3);
                    this.enemy2.mtn(28, 1, 1.0f, true);
                    Runtime.enable(65536);
                    this.player.mtn(4, 9, 1.0f, true);
                    this.player.move(60, 13.903f, 13.672f, true);
                    System.sleep(75);
                    this.player.mtn(28, 9, 1.0f, true);
                    this.enemy2.mtn(2, 9, 1.0f, true);
                    this.enemy2.move(120, -3.0f, 21.0f, true);
                    System.sleep(120);
                    this.SE01.stop();
                    this.Kakuheki_Close();
                    System.sleep(50);
                    this.escape = 1;
                    this.enemy2.kickEnepc(4, 0);
                    Runtime.setFlags(3034, 1, 1);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    this.cam0.setMode(0);
                    return;
                }
            }
            return;
        }
        if (n2 == 18) {
            switch (n) {
                case 100: {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_15, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 != 19) return;
        switch (n) {
            case 100: {
                if (this.Tuika == 0) {
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.player.setTranslate(this.player.px + 0.3f, this.player.py, this.player.pz);
                    Runtime.enable(65536);
                    this.player.mtn(28, 1, 1.0f, true);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Sion_5, 0);
                    System.waitFor(this.win);
                    this.Tuika = 1;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    return;
                }
                Runtime.setPlayerControl(false);
                this.player.getTranslate();
                this.player.setTranslate(this.player.px + 0.3f, this.player.py, this.player.pz);
                Runtime.enable(65536);
                this.player.mtn(28, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Sion_5_1, 0);
                System.waitFor(this.win);
                this.Tuika = 0;
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
            }
        }
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (this.npc1talked == 0) {
            window.print(this.Npc_1_0, 0);
            ST0411.waitPage(window, 64);
            this.npc1talked = 1;
        } else if (this.npc1talked == 1) {
            window.print(this.Npc_1_1, 0);
            ST0411.waitPage(window, 64);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC10(Enepc enepc) {
        this.off();
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.Npc_2_2, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
        this.on();
    }

    public void TalkNPC2(Enepc enepc) {
        if (this.npc2talked == 0) {
            this.off();
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Npc_2_0, 0);
            this.npc2talked = 1;
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
            this.on();
        } else if (this.npc2talked == 1) {
            this.off();
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Npc_2_1, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
            this.on();
        } else if (this.npc2talked == 2) {
            this.off();
            this.npc2.disableDTKFlag(1);
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Npc_2_2, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
            this.on();
        }
    }

    public void TalkNPC3(Enepc enepc) {
        if (Runtime.getFlags(3036, 1) == 0) {
            if (this.npc3talked == 0) {
                this.off();
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Npc_3_7, 0);
                System.waitFor(this.win);
                this.npc3talked = 1;
                Runtime.setPlayerControl(true);
                this.on();
            } else {
                this.off();
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Npc_3_8, 0);
                System.waitFor(this.win);
                this.npc3talked = 0;
                Runtime.setPlayerControl(true);
                this.on();
            }
        } else if (Runtime.getFlags(3036, 1) == 1) {
            this.off();
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Npc_3_9, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
            this.on();
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(65917, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(65916, 3);
                break;
            }
            case 3: {
                if (Runtime.getFlags(3007, 1) == 1) {
                    Runtime.jumpCF(65861, 1);
                    break;
                }
                Runtime.jumpCF(65857, 1);
                break;
            }
            case 4: {
                Runtime.setFlags(3046, 1, 1);
                Runtime.jumpCF(65726, 1);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        this.FadeIn01 = new Effect(0);
        this.FadeIn01.args[0] = -268435456;
        this.FadeIn01.args[1] = 30;
        this.FadeIn01.args[2] = 0;
        this.SE01 = Thread.create(this, "sound01");
        this.SE02 = Thread.create(this, "sound02");
        this.SE03 = Thread.create(this, "sound03");
        this.teiten1 = new Uwamono(28690, 24.0f, 0.0f, 22.0f, 0.0f);
        this.teiten1.SetBgm(196634);
        this.teiten2 = new Uwamono(28690, 22.0f, 0.0f, -20.0f, 0.0f);
        this.teiten2.SetBgm(196634);
        this.teiten3 = new Uwamono(28690, 30.0f, 0.0f, 1.0f, 0.0f);
        this.teiten3.SetBgm(196634);
        this.teiten4 = new Uwamono(28690, 25.0f, 0.0f, 1.0f, 0.0f);
        this.teiten4.SetBgm(196635);
        this.teiten5 = new Uwamono(28690, 20.5f, 0.0f, 13.0f, 0.0f);
        this.teiten5.SetBgm(196635);
        this.teiten6 = new Uwamono(28690, 20.5f, 0.0f, -9.0f, 0.0f);
        this.teiten6.SetBgm(196635);
        this.teiten7 = new Uwamono(28690, 18.0f, 0.0f, 1.0f, 0.0f);
        this.teiten7.SetBgm(196635);
        this.teiten8 = new Uwamono(28690, -14.0f, 0.0f, 16.0f, 0.0f);
        this.teiten8.SetBgm(196642);
        this.teiten9 = new Uwamono(28690, 18.0f, 0.0f, 1.0f, 0.0f);
        this.teiten9.SetBgm(196754);
        this.teiten9.SetBgm(-1);
        Sound.effectStop(196754);
        this.EF01 = new Effect(1056, 0);
        this.EF01.disp(true);
        this.EF01.setClip(true);
        this.EF02 = new Effect(1011, 1);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1011, 2);
        this.EF03.disp(true);
        this.EF03.setClip(true);
        this.EF04 = new Effect(1528, -9.75f, 0.0f, 18.5f, 45.0f);
        this.EF04.setScale(2.0f, 2.0f, 2.0f);
        this.EF04.disp(true);
        this.EF04.setClip(true);
        this.EF06 = new Effect(1528, 0.5f, 0.0f, -15.52f, 142.0f);
        this.EF06.disp(true);
        this.EF06.setClip(true);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(23, false);
        Stage.setVisible(24, false);
        Stage.setVisible(25, false);
        Stage.setVisible(26, false);
        Stage.setVisible(27, false);
        Stage.setVisible(28, false);
        Stage.setVisible(31, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 21.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 3.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestal(7, 10.724f, 7.143f, -21.846f, 62.91f, -69.846f, -9.719f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.cam0.setCFPedestal(8, 27.6133f, 5.6059f, 4.9391f, 62.358f, -60.2295f, 0.2437f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFPedestal(9, -6.92025f, 4.6719f, 28.7121f, 40.0f, -20.5988f, -30.9395f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(9, 1);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestal(11, 9.149f, 2.5f, -20.874f, 40.0f, -11.187f, 345.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFPedestal(13, 13.524f, 8.32379f, 15.4844f, 48.0f, -88.00234f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(13, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(13, 1);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.npc1 = new NPC_NORMAL(519, 11, 0, 22, 3, 8.12f, 0.0f, -26.51f, 45.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.touchto("TouchNPC1");
        this.npc1.disableDTKFlag(131082);
        this.npc1.enableDTKFlag(4);
        this.npc1.setInvalidID(1);
        this.npc2 = new NPC_NORMAL(523, 12, 0, 11, 32, 0.5f, 0.0f, -15.52f, 142.0f);
        this.npc2.talkto("TalkNPC2");
        this.npc2.disableDTKFlag(131083);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc3 = new NPC_NORMAL(519, 13, 12, 22, 3, 21.78f, 0.0f, 22.751f, 225.0f);
        this.npc3.talkto("TalkNPC3");
        this.npc3.touchto("TouchNPC3");
        this.npc3.disableDTKFlag(131082);
        this.npc3.enableDTKFlag(4);
        if (Runtime.getFlags(3030, 1) == 0) {
            this.npc6 = new NPC_NORMAL(1, 16, 0, 11, 17, -14.0f, 0.0f, 36.0f, 180.0f);
            this.npc6.talkto("TalkNPC6");
            this.npc6.disableDTKFlag(131072);
            this.npc6.disableDTKFlag(8);
            this.npc6.setInvalidID(1);
            this.npc6.setShadow(0, 0);
        }
        this.npc7 = new NPC_NORMAL(519, 17, 0, 21, 4, -9.75f, 0.0f, 18.5f, 45.0f);
        this.npc7.talkto("TalkNPC7");
        this.npc7.disableDTKFlag(8);
        this.npc7.setMotion(0, 4);
        this.npc8 = new NPC_NORMAL(1592, 18, 0, 6, 13, 19.0f, 1.8f, 0.7f, 35.0f);
        this.npc8.kickEnepc(10, 0, 0);
        this.npc8.setTP(500);
        this.npc8.setInvalidID(1);
        this.npc8.setMotion(0, 10);
        this.npc8.setShadow(0, 0);
        this.npc8.dispRadar(false);
        this.npc9 = new NPC_NORMAL(1595, 19, 0, 6, 13, 19.0f, 1.8f, 1.3f, 110.0f);
        this.npc9.kickEnepc(10, 0, 0);
        this.npc9.setTP(500);
        this.npc9.setInvalidID(1);
        this.npc9.setMotion(0, 9);
        this.npc9.setShadow(0, 0);
        this.npc9.dispRadar(false);
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16385, 18, -18.0f, 0.0f, 23.0f, 90.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            float[] fArray2 = new float[8];
            fArray2[0] = -18.0f;
            fArray2[2] = 23.0f;
            fArray2[3] = 1.0f;
            fArray2[4] = -17.0f;
            fArray2[6] = 23.0f;
            fArray2[7] = -1.0f;
            fArray = fArray2;
            this.enemy1.setParams(0, 0, 1, 18, fArray);
            this.enemy1.setBatEvent(13);
            this.enemy1.setTP(300);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy1 = new Enepc();
            this.enemy1.init(16385, 18, -18.0f, 0.0f, 23.0f, 90.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(3, 3, 3, 6);
            float[] fArray3 = new float[8];
            fArray3[0] = -18.0f;
            fArray3[2] = 23.0f;
            fArray3[3] = 1.0f;
            fArray3[4] = -17.0f;
            fArray3[6] = 23.0f;
            fArray3[7] = -1.0f;
            fArray = fArray3;
            this.enemy1.setParams(0, 0, 1, 18, fArray);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16385, 6, -11.0f, 0.0f, 19.27f, 135.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(0, 0, 0, 0);
            float[] fArray4 = new float[28];
            fArray4[0] = -11.0f;
            fArray4[2] = 19.27f;
            fArray4[3] = 1.0f;
            fArray4[4] = -13.16f;
            fArray4[6] = 20.23f;
            fArray4[7] = 2.0f;
            fArray4[8] = -11.09f;
            fArray4[10] = 18.23f;
            fArray4[11] = 3.0f;
            fArray4[12] = -9.56f;
            fArray4[14] = 20.57f;
            fArray4[15] = 4.0f;
            fArray4[16] = -6.13f;
            fArray4[18] = 20.93f;
            fArray4[19] = 5.0f;
            fArray4[20] = -2.44f;
            fArray4[22] = 21.02f;
            fArray4[23] = 6.0f;
            fArray4[24] = 0.46f;
            fArray4[26] = 21.38f;
            fArray4[27] = -1.0f;
            fArray = fArray4;
            this.enemy2.setParams(0, 13, 2, 6, fArray);
            this.enemy2.enableDTKFlag(262144);
            this.enemy2.setBatEvent(13);
            this.enemy2.setTP(300);
            this.enemy2.kickEnepc(8, 17);
            this.enemy2.setMotion(2, 27);
            this.enemy2.setMotion(0, 27);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16385, 6, -11.0f, 0.0f, 19.27f, -45.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(3, 3, 3, 6);
            float[] fArray5 = new float[28];
            fArray5[0] = -11.0f;
            fArray5[2] = 19.27f;
            fArray5[3] = 1.0f;
            fArray5[4] = -13.16f;
            fArray5[6] = 20.23f;
            fArray5[7] = 2.0f;
            fArray5[8] = -11.09f;
            fArray5[10] = 18.23f;
            fArray5[11] = 3.0f;
            fArray5[12] = -9.56f;
            fArray5[14] = 20.57f;
            fArray5[15] = 4.0f;
            fArray5[16] = -6.13f;
            fArray5[18] = 20.93f;
            fArray5[19] = 5.0f;
            fArray5[20] = -2.44f;
            fArray5[22] = 21.02f;
            fArray5[23] = 6.0f;
            fArray5[24] = 0.46f;
            fArray5[26] = 21.38f;
            fArray5[27] = -1.0f;
            fArray = fArray5;
            this.enemy2.setParams(0, 13, 2, 6, fArray);
            this.enemy2.enableDTKFlag(262144);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy3 = new Enepc();
            this.enemy3.init(20228, 10, 21.0f, 0.0f, -2.0f, 0.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(2, 2, 2, 2);
            float[] fArray6 = new float[8];
            fArray6[0] = 21.0f;
            fArray6[2] = -2.0f;
            fArray6[3] = 1.0f;
            fArray6[4] = 20.0f;
            fArray6[6] = -2.0f;
            fArray6[7] = -1.0f;
            fArray = fArray6;
            this.enemy3.setParams(0, 2, 3, 10, fArray);
            this.enemy3.setBatEvent(13);
            this.enemy3.setTP(300);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy3 = new Enepc();
            this.enemy3.init(20228, 10, 21.0f, 0.0f, -2.0f, 0.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(5, 5, 7, 7);
            float[] fArray7 = new float[8];
            fArray7[0] = 21.0f;
            fArray7[2] = -2.0f;
            fArray7[3] = 1.0f;
            fArray7[4] = 20.0f;
            fArray7[6] = -2.0f;
            fArray7[7] = -1.0f;
            fArray = fArray7;
            this.enemy3.setParams(0, 2, 3, 10, fArray);
        }
        if (Runtime.getFlags(3018, 1) == 0) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16385, 6, 0.131f, 0.0f, -27.053f, 30.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(0, 0, 0, 0);
            float[] fArray8 = new float[44];
            fArray8[0] = 0.131f;
            fArray8[2] = -27.053f;
            fArray8[3] = 1.0f;
            fArray8[4] = 0.5f;
            fArray8[6] = -26.88f;
            fArray8[7] = 2.0f;
            fArray8[8] = 2.94f;
            fArray8[10] = -26.78f;
            fArray8[11] = 3.0f;
            fArray8[12] = 2.94f;
            fArray8[14] = -23.71f;
            fArray8[15] = 4.0f;
            fArray8[16] = 2.94f;
            fArray8[18] = -20.1f;
            fArray8[19] = 5.0f;
            fArray8[20] = 2.98f;
            fArray8[22] = -17.19f;
            fArray8[23] = 6.0f;
            fArray8[24] = 5.09f;
            fArray8[26] = -17.19f;
            fArray8[27] = 7.0f;
            fArray8[28] = 7.21f;
            fArray8[30] = -17.19f;
            fArray8[31] = 8.0f;
            fArray8[32] = 9.72f;
            fArray8[34] = -17.19f;
            fArray8[35] = 9.0f;
            fArray8[36] = 11.83f;
            fArray8[38] = -17.16f;
            fArray8[39] = 10.0f;
            fArray8[40] = 14.01f;
            fArray8[42] = -17.16f;
            fArray8[43] = -1.0f;
            fArray = fArray8;
            this.enemy4.setParams(0, 8, 4, 6, fArray);
            float[] fArray9 = new float[60];
            fArray9[0] = 0.131f;
            fArray9[2] = -27.053f;
            fArray9[3] = 0.5f;
            fArray9[5] = -26.88f;
            fArray9[6] = 2.94f;
            fArray9[8] = -26.78f;
            fArray9[9] = 2.94f;
            fArray9[11] = -23.71f;
            fArray9[12] = 2.94f;
            fArray9[14] = -20.1f;
            fArray9[15] = 2.98f;
            fArray9[17] = -17.19f;
            fArray9[18] = 5.09f;
            fArray9[20] = -17.19f;
            fArray9[21] = 7.21f;
            fArray9[23] = -17.19f;
            fArray9[24] = 9.72f;
            fArray9[26] = -17.19f;
            fArray9[27] = 11.83f;
            fArray9[29] = -17.16f;
            fArray9[30] = 14.01f;
            fArray9[32] = -17.16f;
            fArray9[33] = 11.83f;
            fArray9[35] = -17.16f;
            fArray9[36] = 9.72f;
            fArray9[38] = -17.19f;
            fArray9[39] = 7.21f;
            fArray9[41] = -17.19f;
            fArray9[42] = 5.09f;
            fArray9[44] = -17.19f;
            fArray9[45] = 2.98f;
            fArray9[47] = -17.19f;
            fArray9[48] = 2.94f;
            fArray9[50] = -20.1f;
            fArray9[51] = 2.94f;
            fArray9[53] = -23.71f;
            fArray9[54] = 2.94f;
            fArray9[56] = -26.78f;
            fArray9[57] = 0.5f;
            fArray9[59] = -26.88f;
            float[] fArray10 = fArray9;
            this.enemy4.setParams(fArray10);
            this.enemy4.enableDTKFlag(262144);
            this.enemy4.setBatEvent(13);
            this.enemy4.setTP(300);
        } else if (Runtime.getFlags(3018, 1) == 1) {
            this.enemy4 = new Enepc();
            this.enemy4.init(16385, 6, 0.131f, 0.0f, -27.053f, 30.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(3, 3, 3, 6);
            float[] fArray11 = new float[44];
            fArray11[0] = 0.131f;
            fArray11[2] = -27.053f;
            fArray11[3] = 1.0f;
            fArray11[4] = 0.5f;
            fArray11[6] = -26.88f;
            fArray11[7] = 2.0f;
            fArray11[8] = 2.94f;
            fArray11[10] = -26.78f;
            fArray11[11] = 3.0f;
            fArray11[12] = 2.94f;
            fArray11[14] = -23.71f;
            fArray11[15] = 4.0f;
            fArray11[16] = 2.94f;
            fArray11[18] = -20.1f;
            fArray11[19] = 5.0f;
            fArray11[20] = 2.98f;
            fArray11[22] = -17.19f;
            fArray11[23] = 6.0f;
            fArray11[24] = 5.09f;
            fArray11[26] = -17.19f;
            fArray11[27] = 7.0f;
            fArray11[28] = 7.21f;
            fArray11[30] = -17.19f;
            fArray11[31] = 8.0f;
            fArray11[32] = 9.72f;
            fArray11[34] = -17.19f;
            fArray11[35] = 9.0f;
            fArray11[36] = 11.83f;
            fArray11[38] = -17.16f;
            fArray11[39] = 10.0f;
            fArray11[40] = 14.01f;
            fArray11[42] = -17.16f;
            fArray11[43] = -1.0f;
            fArray = fArray11;
            this.enemy4.setParams(0, 8, 4, 6, fArray);
            float[] fArray12 = new float[60];
            fArray12[0] = 0.131f;
            fArray12[2] = -27.053f;
            fArray12[3] = 0.5f;
            fArray12[5] = -26.88f;
            fArray12[6] = 2.94f;
            fArray12[8] = -26.78f;
            fArray12[9] = 2.94f;
            fArray12[11] = -23.71f;
            fArray12[12] = 2.94f;
            fArray12[14] = -20.1f;
            fArray12[15] = 2.98f;
            fArray12[17] = -17.19f;
            fArray12[18] = 5.09f;
            fArray12[20] = -17.19f;
            fArray12[21] = 7.21f;
            fArray12[23] = -17.19f;
            fArray12[24] = 9.72f;
            fArray12[26] = -17.19f;
            fArray12[27] = 11.83f;
            fArray12[29] = -17.16f;
            fArray12[30] = 14.01f;
            fArray12[32] = -17.16f;
            fArray12[33] = 11.83f;
            fArray12[35] = -17.16f;
            fArray12[36] = 9.72f;
            fArray12[38] = -17.19f;
            fArray12[39] = 7.21f;
            fArray12[41] = -17.19f;
            fArray12[42] = 5.09f;
            fArray12[44] = -17.19f;
            fArray12[45] = 2.98f;
            fArray12[47] = -17.19f;
            fArray12[48] = 2.94f;
            fArray12[50] = -20.1f;
            fArray12[51] = 2.94f;
            fArray12[53] = -23.71f;
            fArray12[54] = 2.94f;
            fArray12[56] = -26.78f;
            fArray12[57] = 0.5f;
            fArray12[59] = -26.88f;
            float[] fArray13 = fArray12;
            this.enemy4.setParams(fArray13);
            this.enemy4.enableDTKFlag(262144);
        }
        if (Runtime.getFlags(3001, 2) == 2) {
            if (Runtime.getFlags(3018, 1) == 0) {
                this.enemy5 = new Enepc();
                this.enemy5.init(16386, 8, 12.48f, 0.0f, -29.144f, 0.0f);
                this.enemy5.id = 5;
                this.enemy5.setGroup(1, 1, 1, 1);
                this.enemy5.setParams(1, 4, 5, 8);
                this.enemy5.setBatEvent(13);
                this.enemy5.setTP(300);
            } else if (Runtime.getFlags(3018, 1) == 1) {
                this.enemy5 = new Enepc();
                this.enemy5.init(16386, 8, 12.48f, 0.0f, -29.144f, 0.0f);
                this.enemy5.id = 5;
                this.enemy5.setGroup(4, 4, 4, 4);
                this.enemy5.setParams(1, 4, 5, 8);
            }
        }
        if (Runtime.getFlags(3030, 1) == 0) {
            this.doorA = new Uwamono(13, 42, '\u0001');
            new Uwamono(14, 42, '\u0001', this.doorA);
            this.doorA.SetDoorType('\u0002');
            this.doorA.DoorClose();
        } else if (Runtime.getFlags(3030, 1) == 1) {
            this.doorA = new Uwamono(13, 42, '\u0001');
            new Uwamono(14, 42, '\u0001', this.doorA);
            this.doorA.SetDoorType('\u0002');
        }
        this.doorB = new Uwamono(17, 42, '\u0001');
        new Uwamono(18, 42, '\u0001', this.doorB);
        if (Runtime.getFlags(3007, 1) == 1) {
            this.doorB.SetDoorType('\u0004');
        } else {
            this.doorB.SetDoorType('\u0002');
        }
        this.doorC = new Uwamono(19, 42, '\u0001');
        new Uwamono(20, 42, '\u0002', this.doorC);
        if (Runtime.getFlags(3009, 1) == 1) {
            this.doorC.SetDoorType('\u0004');
        } else {
            this.doorC.SetDoorType('\u0002');
        }
        this.doorD = new Uwamono(119, 42, '\u0001');
        new Uwamono(118, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0002');
        this.doorD.SetDoorRange(3.0f);
        this.doorD.DoorOpen();
        this.doorD.setVisible(false);
        this.doorE = new Uwamono(165, 40, '\u0001');
        new Uwamono(164, 40, '\u0001', this.doorE);
        this.doorE.SetDoorType('\u0002');
        if (Runtime.getFlags(3296, 1) == 0) {
            this.doorF = new Uwamono(4, 40, '\u0004');
            this.doorF.SetDoorType('\u0002');
        } else {
            this.doorF = new Uwamono(4, 40, '\u0004');
            this.doorF.SetDoorType('\u0004');
        }
        this.doorG = new Uwamono(0, 40, '\u0001');
        this.doorG.SetDoorType('\u0004');
        this.col1 = new Uwamono(28672, 24.0f, 0.0f, 1.0f, 0.0f);
        this.col1.SetSize(0.5f, 1.0f, 3.0f);
        this.Base1 = new Uwamono(28672, 27.5f, -1.0f, 0.0f, 0.0f);
        this.Base1.SetSize(8.0f, 1.0f, 10.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 20);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 21);
        this.item3 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 22);
        new Uwamono(170, 84);
        this.lc = new Uwamono(168, 84, this.item1);
        this.lc.SetSize(0.75f, 1.0f, 0.755f);
        new Uwamono(169, 84);
        new Uwamono(129, 31);
        new Uwamono(130, 21);
        this.Glass = new Uwamono(162, 126);
        this.Glass.SetParticle(1429);
        new Uwamono(166, 21, this.item3);
        this.obj01 = new Uwamono(167, 6, this.item2);
        this.obj01.SetSize(2.5f, 0.5f, 2.5f);
        this.itembox = new Uwamono(28677, 10.0f, 0.0f, -31.0f, 180.0f, 31);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        this.SAVE = new Uwamono(28678, 14.5f, 0.0f, -24.0f);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 18.1f, 2.0f, 1.0f, 90.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 2.38f);
        this.monitor1.setArgs(1, 18001, 0, 256, 128);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor1.setScale(1.2f, 0.81f, 1.0f);
        Stage.setVisible(127, false);
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        if (Runtime.getFlags(3030, 1) == 0) {
            this.player.setTranslate(100.0f, 0.0f, 100.0f);
        }
        if (Runtime.getFlags(3030, 1) == 0) {
            this.kidou = new Mapunits();
            this.kidou.mapUnit(29);
            this.kidou.start(4, null);
            this.kidou.start(1, "Evt");
        }
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3238, 1, 1);
                break;
            }
        }
    }

    void moooon() {
        float f = 0.0f;
        float f2 = 0.0f;
        this.monitor1.signal(1);
        this.teiten9.SetBgm(196754);
        while (f <= 5.0f) {
            this.monitor1.setScale(1.2f, f2 + f * 2.7f * 6.0f / 100.0f, 1.0f);
            f += 1.0f;
            System.sleep(1);
        }
    }

    void off() {
        this.enemy1.kickEnepc(4, 1);
        this.enemy2.kickEnepc(4, 1);
        this.enemy4.kickEnepc(4, 1);
        this.enemy1.kickEnepc(1, 27);
        this.enemy2.kickEnepc(1, 27);
        this.enemy4.kickEnepc(1, 27);
        if (this.Loc_flg == 0) {
            this.enemy3.kickEnepc(4, 1);
            this.enemy3.kickEnepc(1, 27);
        }
    }

    void on() {
        this.enemy1.kickEnepc(4, 0);
        this.enemy2.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
        if (this.Loc_flg == 0) {
            this.enemy3.kickEnepc(4, 0);
        }
    }

    void sound01() {
        boolean bl = false;
        while (true) {
            Sound.effectPlay(196748);
            System.println("＊＊＊＊＊＊音が鳴った！！＊＊＊＊＊＊＊");
            System.sleep(50);
        }
    }

    void sound02() {
        boolean bl = false;
        while (true) {
            Sound.effectPlay(196749);
            System.sleep(66);
            System.println("＊＊＊＊＊＊音２が鳴った！！＊＊＊＊＊＊＊");
        }
    }

    void sound03() {
        boolean bl = false;
        while (true) {
            System.sleep(38);
            Sound.effectPlay(196750);
            System.sleep(28);
            System.println("＊＊＊＊＊＊音３が鳴った！！＊＊＊＊＊＊＊");
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
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST0411.this.cam0.setMode(-1);
            ST0411.this.SE01.start();
            ST0411.this.EV_Camera07();
            ST0411.this.doorA.DoorOpen();
            ST0411.this.npc6.kickEnepc(4, 1);
            ST0411.this.enemy2.kickEnepc(4, 1);
            ST0411.this.npc6.kickEnepc(1, 3);
            ST0411.this.npc6.moveEnepc(15, -14.0f, 32.0f, 37);
            System.sleep(37);
            ST0411.this.npc6.kickEnepc(1, 27);
            System.sleep(30);
            ST0411.this.doorA.DoorClose();
            ST0411.this.doorA.SetDoorType('\u0002');
            ST0411.this.EV_Camera03();
            System.sleep(120);
            ST0411.this.EV_Camera03a();
            ST0411.this.win = Window.create();
            ST0411.this.win.setSize(4, 45);
            ST0411.this.win.setLocation(15, 305);
            ST0411.this.win.print(ST0411.this.Sion_2, 0);
            System.waitFor(ST0411.this.win);
            ST0411.this.enemy2.kickEnepc(4, 0);
            ST0411.this.EV_Camera04();
            ST0411.this.SE02.start();
            ST0411.this.SE03.start();
            System.sleep(120);
            ST0411.this.EV_Camera04a();
            ST0411.this.win = Window.create();
            ST0411.this.win.setSize(4, 45);
            ST0411.this.win.setLocation(15, 305);
            ST0411.this.win.print(ST0411.this.Sion_3, 0);
            System.sleep(90);
            System.waitFor(ST0411.this.win);
            ST0411.this.npc6.setTranslate(-100.0f, -100.0f, -100.0f);
            ST0411.this.player.hairStop(0, 1);
            ST0411.this.player.setLocation(1, 4);
            Runtime.setFlags(3030, 1, 1);
            Runtime.setPlayerControl(true);
            ST0411.this.cam0.setMode(0);
            Runtime.enable(524288);
        }
    }

    class Object
            extends Unit {
        Object() {
        }
    }
}

