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
import xeno.map.MC_UTK04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1040
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_UTK04_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    MAPUnit screen;
    MAPUnit line;
    MAPUnit obj_01;
    boolean SCREEN_FLAG = false;
    boolean FIN_FLAG = false;
    boolean ENEMY5_CHECK = false;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy10;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc_d1;
    Enepc npc_d2;
    Enepc npc_d3;
    Uwamono col;
    int talkFlag1 = 0;
    int talkFlag2 = 0;
    int deadFlag1 = 0;
    int deadFlag2 = 0;
    int TrapTalk1 = 0;
    int TrapTalk2 = 0;
    Uwamono doorA;
    Uwamono doorB_1;
    Uwamono doorB_2;
    Uwamono door_2;
    Uwamono door_3;
    Uwamono K1;
    Uwamono K2;
    Uwamono K3;
    Uwamono item2;
    Uwamono item3;
    Uwamono item5;
    Uwamono item7;
    Uwamono item9;
    Uwamono item11;
    Effect Obj600_A;
    Effect Obj600_B;
    Effect Obj601_A;
    Effect Obj601_B;
    Effect Obj602_A;
    Effect Obj602_B;
    Effect Obj603;
    Effect Obj604;
    Effect Obj605;
    Effect Obj606;
    Effect Obj607;
    Effect Obj608;
    Effect Obj609;
    Effect Obj610;
    Effect Obj611;
    Effect Obj612;
    Effect Obj613;
    Effect Obj614;
    Effect fade;
    Effect E_Move;
    Effect E_Bang1;
    Effect E_Bang2;
    Effect Jr_Gun_Effect;
    Unit Jr_Gun;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Uwamono teiten10;
    Uwamono teiten11;
    Uwamono teiten12;
    Uwamono teiten13;
    Uwamono teiten14;
    Uwamono teiten15;
    Uwamono teiten16;
    Uwamono teiten17;
    Light light = new Light(0);
    boolean EnterCheck = false;
    int selected = 0;
    int[] RedScreenParam;
    int[] DefaultScreenParam;
    int NO3_CARDKEY;
    int NO4;
    int SHUTTER;
    int TRAP1;
    int TRAP1_FIRST;
    int TRAP2;
    int TRAP2_FIRST;
    int HEISHI;
    int KOW1;
    int HEISHI_LIFE1;
    int HEISHI_LIFE2;
    int HEISHI_LIFE3;
    int RADER;
    int FIRST_PLAY;
    int NO3_HINTO1;
    int NO3_HINTO2;
    int page;
    String[] CLOSE;
    String[] NO4_CLOSE;
    String[] Q1;
    String[] Q2;
    String[] HINT;
    String[] NPC1_TALK1;
    String[] NPC1_TALK2;
    String[] NPC1_TALK3;
    String[] NPC1_TALK4;
    String[] NPC1_TALK5;
    String[] NPC2_TALK1;
    String[] NPC2_TALK2;
    String[] NPC2_TALK3;
    String[] NPC2_TALK4;
    String[] NPC2_TALK5;
    String[] NPC2_TALK6;
    String[] NPC2_TALK7;
    String[] NPC2_TALK8_A;
    String[] NPC2_TALK8_B;
    String[] NPC3_TALK1;
    String[] DEAD1_TALK1;
    String[] DEAD1_TALK2;
    String[] DEAD2_TALK1;
    String[] DEAD2_TALK2;

    ST1040() {
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x100000;
        nArray[3] = 0x600000FF;
        this.RedScreenParam = nArray;
        int[] nArray2 = new int[8];
        nArray2[2] = 0x100000;
        nArray2[3] = -2130771968;
        this.DefaultScreenParam = nArray2;
        this.NO3_CARDKEY = Runtime.checkItem(10, 3);
        this.NO4 = Runtime.getFlags(6001, 1);
        this.SHUTTER = Runtime.getFlags(6002, 1);
        this.TRAP1 = Runtime.getFlags(6003, 1);
        this.TRAP1_FIRST = Runtime.getFlags(6030, 1);
        this.TRAP2 = Runtime.getFlags(6004, 1);
        this.TRAP2_FIRST = Runtime.getFlags(6031, 1);
        this.HEISHI = Runtime.getFlags(6005, 1);
        this.KOW1 = Runtime.getFlags(6006, 1);
        this.HEISHI_LIFE1 = Runtime.getFlags(6007, 1);
        this.HEISHI_LIFE2 = Runtime.getFlags(6008, 1);
        this.HEISHI_LIFE3 = Runtime.getFlags(6009, 1);
        this.RADER = Runtime.getFlags(6025, 1);
        this.FIRST_PLAY = Runtime.getFlags(6035, 1);
        this.NO3_HINTO1 = Runtime.getFlags(6038, 1);
        this.NO3_HINTO2 = Runtime.getFlags(6049, 1);
        this.CLOSE = new String[]{"/[label(Jr.)]", "It's locked electronically.", "/[waitkey(64)]/[close()]"};
        this.NO4_CLOSE = new String[]{"/[label(Jr.)]", "It's inscribed with a large number 4. Don't think I can open it with my bare hands.", "/[waitkey(64)]/[close()]"};
        this.Q1 = new String[]{"There is a switch. Press it?", "/[waitkey(64)]/[close()]"};
        this.Q2 = new String[]{"'Door No. 4 switch'\n", " Operate it?", "/[waitkey(64)]/[close()]"};
        this.HINT = new String[]{"/[label(Jr.)]", "It's locked electronically. I can see a switch flashing in the back.", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK1 = new String[]{"Little Master, your goal is the ship's bridge. The layout of this ship is rather unique, so its location is currently unknown. The other units have been dispatched to get control of the enemy forces. We'll take care of things here, so please go after the rest.", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK2 = new String[]{"According to the recon data from the lead squadron, some areas are booby-trapped. Please be careful.", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK3 = new String[]{"Little Master, please head to the bridge at once. Leave this area to us.", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK4 = new String[]{"That idiot! He said he'd...pull out second, but he ended up being the first to run! He got scared just because we got some casualties! He's so incompetent!", "/[waitkey(1)]/[clear()]", "Little Master, if you find him, please tell him to come right back immediately.", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK5 = new String[]{"Little Master, if you find that idiot, please tell him to return to his post.", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK1 = new String[]{"The enemy's defensive line seems to be located down this corridor and to the right. It's possible that mulitple enemies have set up an ambush.", "/[waitkey(1)]/[clear()]", "Please watch your six.", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK2 = new String[]{"I believe you'll be able to get past easily if you avoid unnecessary battles. As they say, \"A wise man never courts danger.\"", "/[waitkey(1)]/[clear()]", "Please do not recklessly fire your gun or poke around suspicious locations.", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK3 = new String[]{"Talking to yourself will not change the situation. Let us hurry on!", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(1)]/[clear()]", "May I ask you one thing? Little Master, do you ever get scared before a fight?", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK4 = new String[]{"That's surprising. But I'd guess you conquered your fears of battle long ago, so you probably don't feel it anymore, Little Master.", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK5 = new String[]{"You're definitely not like me, Little Master. I run the moment I get scared. Just earlier, I charged ahead of you and confronted the enemy in front of Door No. 3, but then...", "/[waitkey(1)]/[clear()]", "Come to think of it, they seemed rather panicked. They were saying they had to close it quickly, or the bridge...or something like that. Door No. 3 is definitely worth checking.", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK6 = new String[]{"But please don't overdo it. If it gets dangerous...pulling out first is difficult, so I'll run away second.", "/[waitkey(1)]/[clear()]", "Just kidding. Really, I'm just kidding!\n", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK7 = new String[]{"Oh, Little Master! Did you fall back here because it got too dangerous for you too? I think you made a wise decision.", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK8_A = new String[]{"What? Me? Well, I felt that I was in danger too, so I came back here for the time being.", "/[waitkey(1)]/[clear()]", "I-it's true!", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK8_B = new String[]{"What?", "/[waitkey(1)]/[clear()]", "My patrol partner is looking for me? Oh...he's so hopeless without me.", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(1)]/[clear()]", "I-I'll wait here for a bit longer, then head back.", "/[waitkey(64)]/[close()]"};
        this.NPC3_TALK1 = new String[]{"Please be careful, Little Master.", "/[waitkey(64)]/[close()]"};
        this.DEAD1_TALK1 = new String[]{"...", "/[waitkey(64)]/[close()]"};
        this.DEAD1_TALK2 = new String[]{"...", "/[waitkey(64)]/[close()]"};
        this.DEAD2_TALK1 = new String[]{"...", "/[waitkey(64)]/[close()]"};
        this.DEAD2_TALK2 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    }

    void DefaultLight() {
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
    }

    int DefaultMenu(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    void DefaultTalk(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
    }

    void EOB(int n) {
        System.println("EOB**********");
        switch (n) {
            case 3: {
                System.println("enemyNo case 3");
                Runtime.setFlags(6007, 1, 1);
                this.HEISHI_LIFE1 = Runtime.getFlags(6007, 1);
                break;
            }
            case 4: {
                System.println("enemyNo case 4");
                Runtime.setFlags(6008, 1, 1);
                this.HEISHI_LIFE2 = Runtime.getFlags(6008, 1);
                break;
            }
            case 5: {
                System.println("enemyNo case 5");
                this.ENEMY5_CHECK = true;
                Runtime.setFlags(6009, 1, 1);
                this.HEISHI_LIFE3 = Runtime.getFlags(6009, 1);
                break;
            }
            case 10: {
                System.println("EOB case 10 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                this.enemy1.setVisible(false);
                this.enemy2.setVisible(false);
                this.cam0.setMode(1);
                this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
                this.cam0.setCFHokan(2, 0.03f, 0.03f);
                this.enemy1.setTranslate(19.0f, 0.0f, -4.0f);
                this.enemy1.setRotate(0.0f, 270.0f, 0.0f);
                this.enemy2.setTranslate(20.0f, 0.0f, -4.25f);
                this.enemy2.setRotate(0.0f, 270.0f, 0.0f);
                this.Jr_Gun.setVisible(false);
                this.Jr_Gun_Effect.disp(false);
                break;
            }
        }
    }

    void EV2_Camera01_S() {
        System.println("発火始動カメラ（壊れ物２）");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.892149f, 6.565871f, -11.881127f);
        this.camEV.setRotate(-53.36829f, 16.05867f, 0.0f);
        this.camEV.setFov(42.5f);
        this.camEV.change();
    }

    void EV2_Camera02() {
        System.println("爆発カメラ（壊れ物２）");
        float[] fArray = new float[]{1.0f, 19.353931f, 4.037933f, -12.488494f, 2.0f, 19.33893f, 4.035933f, -12.473494f, 3.0f, 19.353931f, 4.037933f, -12.488494f, 4.0f, 19.36893f, 4.0399327f, -12.503494f, 5.0f, 19.353931f, 4.037933f, -12.488494f, 6.0f, 19.33893f, 4.035933f, -12.473494f, 7.0f, 19.353931f, 4.037933f, -12.488494f, 8.0f, 19.36893f, 4.0399327f, -12.503494f, 9.0f, 19.353931f, 4.037933f, -12.488494f, 10.0f, 19.33893f, 4.035933f, -12.473494f, 11.0f, 19.353931f, 4.037933f, -12.488494f, 12.0f, 19.36893f, 4.0399327f, -12.503494f, 13.0f, 19.353931f, 4.037933f, -12.488494f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -21.263065f;
        fArray2[2] = 55.15857f;
        fArray2[4] = 13.0f;
        fArray2[5] = -21.263065f;
        fArray2[6] = 55.15857f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 13);
        this.camEV.rotateSPL(fArray3, 1, 2, 13);
        this.camEV.setFov(31.939917f);
        this.camEV.change();
    }

    void EV_Camera00() {
        System.println("イベント（回転）操作 カメラ");
        float[] fArray = new float[]{1.0f, 19.0f, 2.1859553f, 10.151785f, 40.0f, 18.2f, 2.1859553f, 10.0f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -11.709911f;
        fArray2[2] = 405.7579f;
        fArray2[4] = 40.0f;
        fArray2[5] = -11.709911f;
        fArray2[6] = 350.02167f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 0, 40);
        this.camEV.rotateSPL(fArray3, 1, 2, 40);
        this.camEV.setFov(34.64001f);
        this.camEV.change();
    }

    void EV_Camera01_S() {
        System.println("発火始動カメラ１（壊れ物１）");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(15.299084f, 7.1418433f, -9.109473f);
        this.camEV.setRotate(-35.741875f, 70.31783f, 0.0f);
        this.camEV.setFov(42.5f);
        this.camEV.change();
    }

    void EV_Camera02() {
        System.println("爆発カメラ（壊れ物１）");
        float[] fArray = new float[]{1.0f, 18.826666f, 4.517926f, -2.3418963f, 2.0f, 18.841665f, 4.5159264f, -2.3568962f, 3.0f, 18.826666f, 4.517926f, -2.3418963f, 4.0f, 18.811655f, 4.519926f, -2.3268962f, 5.0f, 18.826666f, 4.517926f, -2.3418963f, 6.0f, 18.841665f, 4.5159264f, -2.3568962f, 7.0f, 18.826666f, 4.517926f, -2.3418963f, 8.0f, 18.811665f, 4.519926f, -2.3268962f, 9.0f, 18.826666f, 4.517926f, -2.3418963f, 10.0f, 18.841665f, 4.5159264f, -2.3568962f, 11.0f, 18.826666f, 4.517926f, -2.3418963f, 12.0f, 18.811665f, 4.519926f, -2.3268962f, 13.0f, 18.826666f, 4.517926f, -2.3418963f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -16.794998f;
        fArray2[2] = 60.599083f;
        fArray2[4] = 13.0f;
        fArray2[5] = -16.794998f;
        fArray2[6] = 60.599083f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 13);
        this.camEV.rotateSPL(fArray3, 1, 2, 13);
        this.camEV.setFov(22.98003f);
        this.camEV.change();
    }

    void EV_Camera1() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(20.551956f, 6.581907f, 11.144051f);
        this.camEV.setRotate(-22.204039f, 0.0f, 0.0f);
        this.camEV.setFov(34.23999f);
        this.camEV.change();
    }

    void EV_Camera10() {
        System.println("BANG!　カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(19.57745f, 2.293966f, -4.3824234f);
        this.camEV.setRotate(-7.630357f, 74.75907f, 0.0f);
        this.camEV.setFov(22.020006f);
        this.camEV.change();
    }

    void EV_Camera11() {
        System.println("イベント（回転）初期位置");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(19.0f, 2.1859553f, 10.151785f);
        this.camEV.setRotate(-11.709911f, 405.7579f, 0.0f);
        this.camEV.setFov(34.64001f);
        this.camEV.change();
    }

    void EV_Camera2() {
        System.println("NO4扉前　カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(18.907171f, 8.085587f, -3.3220673f);
        this.camEV.setRotate(-49.56094f, -40.57747f, 0.0f);
        this.camEV.setFov(38.02001f);
        this.camEV.change();
    }

    void EV_Camera3() {
        System.println("NO4コンソール前　カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(16.681269f, 2.4418747f, 8.869694f);
        this.camEV.setRotate(-26.549812f, 398.13422f, 0.0f);
        this.camEV.setFov(49.67969f);
        this.camEV.change();
    }

    void EV_Camera4() {
        System.println("トラップ1 警報カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.0416355f, 2.4219677f, -6.0749803f);
        this.camEV.setRotate(-8.929254f, 274.11502f, 0.0f);
        this.camEV.setFov(28.419994f);
        this.camEV.change();
    }

    void EV_Camera5() {
        System.println("トラップ2 警報カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(18.784216f, 2.741995f, -22.667286f);
        this.camEV.setRotate(-11.762804f, 99.499214f, 0.0f);
        this.camEV.setFov(26.500008f);
        this.camEV.change();
    }

    void EV_Camera8() {
        System.println("柱の陰に隠れた兵士　カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(23.351412f, 1.8081758f, 15.285099f);
        this.camEV.setRotate(-17.443424f, 432.63046f, 0.0f);
        this.camEV.setFov(44.739414f);
        this.camEV.change();
    }

    void EV_Camera9() {
        System.println("シャッター扉前　カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(20.13164f, 1.6219348f, -12.296457f);
        this.camEV.setRotate(-3.6826243f, 357.9955f, 0.0f);
        this.camEV.setFov(22.65953f);
        this.camEV.change();
    }

    void Final_init(int n) {
        System.println("Final_init !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        switch (n) {
            case 1: {
                this.enemy1.kickEnepc(4, 1);
                break;
            }
            case 2: {
                this.enemy2.kickEnepc(4, 1);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        switch (n) {
            case 3: {
                if (n2 != 10) break;
                this.enemy3.setES_mGiveUp(5.0f);
                this.enemy3.setES_mGiveUp(0.075f, 0.11f);
                this.enemy3.setES_cRange(160.0f, 5.0f, 4.0f);
                this.enemy3.setRotate(0.0f, 0.0f, 0.0f);
                System.println("俊足　→　ノーマル警備員に変更");
                break;
            }
            case 4: {
                if (n2 != 11) break;
                this.enemy4.setES_mGiveUp(6.0f);
                this.enemy4.setES_mGiveUp(0.075f, 0.13f);
                this.enemy4.setES_cRange(160.0f, 6.0f, 4.0f);
                this.enemy4.setRotate(0.0f, 0.0f, 0.0f);
                System.println("超俊足　→　俊足警備員に変更");
                break;
            }
            case 5: {
                if (n2 == 6) {
                    this.enemy5.setES_mGiveUp(2.0f, 5.0f);
                    this.enemy5.setES_mGiveUp(0.075f, 0.1175f);
                    this.enemy5.setES_cRange(160.0f, 5.0f, 4.0f);
                    System.println("俊足(弱）警備員変更");
                    break;
                }
                if (n2 != 7) break;
                this.enemy5.setES_mGiveUp(2.0f, 5.0f);
                this.enemy5.setES_mGiveUp(0.075f, 0.125f);
                this.enemy5.setES_cRange(160.0f, 5.0f, 4.0f);
                System.println("俊足警備員に変更");
                break;
            }
            case 100: {
                if (n2 == 0 && this.TRAP1 == 0) {
                    Sound.effectPlay(196745);
                    this.player.getTranslate();
                    Runtime.setPlayerControl(false);
                    if (this.TRAP1_FIRST == 0) {
                        this.screen.start(1, "Screen");
                        this.enemy1.setTranslate(19.0f, 0.0f, -1.0f);
                        this.enemy1.setRotate(0.0f, 180.0f, 0.0f);
                        this.enemy2.setTranslate(20.0f, 0.0f, -2.0f);
                        this.enemy2.setRotate(0.0f, 180.0f, 0.0f);
                        System.sleep(2);
                        Runtime.enable(65536);
                        this.player.setTranslate(this.player.px - 0.35f, this.player.py, this.player.pz);
                        this.player.setRotate(0.0f, 90.0f, 0.0f);
                        this.Jr_Gun_Effect.setTranslate(this.player.px - 0.1f, this.player.py + 1.5f, this.player.pz + 0.5f);
                        this.EV_Camera4();
                        this.enemy1.setVisible(true);
                        this.enemy2.setVisible(true);
                        this.enemy1.kickEnepc(1, 3);
                        this.enemy1.moveEnepc(15, 19.0f, -6.0f, 23);
                        this.enemy2.kickEnepc(1, 3);
                        this.enemy2.moveEnepc(15, 20.0f, -7.0f, 21);
                        System.sleep(24);
                        this.player.mtn(10, 0, 129, 50, 1, 2.05f, true);
                        if (this.player.pz < -8.0f) {
                            this.enemy1.moveEnepc(17, 265.0f, 1.0f, 6);
                            this.enemy2.moveEnepc(17, 270.0f, 1.0f, 4);
                        } else if (this.player.pz < -6.0f && this.player.pz >= -8.0f) {
                            this.enemy1.moveEnepc(17, 270.0f, 1.0f, 6);
                            this.enemy2.moveEnepc(17, 270.0f, 1.0f, 4);
                        } else {
                            this.enemy1.moveEnepc(17, 275.0f, 1.0f, 6);
                            this.enemy2.moveEnepc(17, 280.0f, 1.0f, 4);
                        }
                        System.sleep(8);
                        this.enemy1.kickEnepc(0, 0);
                        this.enemy2.kickEnepc(0, 0);
                        System.sleep(1);
                        this.enemy1.kickEnepc(2, 5, 0, 32, 1, 100);
                        this.enemy2.kickEnepc(1, 9);
                        System.sleep(25);
                        this.enemy1.kickEnepc(1, 8);
                        this.enemy2.kickEnepc(1, 7);
                        System.sleep(25);
                        this.enemy2.kickEnepc(0, 11);
                        System.sleep(20);
                        this.EV_Camera10();
                        int n3 = 0;
                        while (n3 < 20) {
                            if (n3 == 1) {
                                this.player.mtn(15, 1, 1.0f, true);
                                this.Jr_Gun.setVisible(true);
                                this.enemy1.kickEnepc(1, 3);
                                this.enemy1.moveEnepc(15, this.player.px + 1.0f, this.player.pz + 0.75f, 27);
                                this.enemy2.kickEnepc(1, 3);
                                this.enemy2.moveEnepc(15, this.player.px + 1.0f, this.player.pz, 27);
                            } else if (n3 == 14) {
                                Sound.effectPlay(196746);
                            } else if (n3 == 16) {
                                this.Jr_Gun_Effect.disp(true);
                            }
                            System.sleep(1);
                            ++n3;
                        }
                        Runtime.setFlags(6030, 1, 1);
                        this.TRAP1_FIRST = Runtime.getFlags(6030, 1);
                    } else {
                        this.screen.start(1, "Screen");
                        this.enemy1.setTranslate(19.0f, 0.0f, -6.0f);
                        this.enemy1.setRotate(0.0f, 270.0f, 0.0f);
                        this.enemy2.setTranslate(20.0f, 0.0f, -7.0f);
                        this.enemy2.setRotate(0.0f, 270.0f, 0.0f);
                        System.sleep(1);
                        this.enemy1.setVisible(true);
                        this.enemy2.setVisible(true);
                        this.player.setTranslate(this.player.px - 0.2f, this.player.py, this.player.pz);
                        this.Jr_Gun_Effect.setTranslate(this.player.px - 0.5f, 2.0f, this.player.pz + 1.0f);
                        this.cam0.setCFAngle(2, -28.0f, 300.0f, 0.0f, 7.0f, 42.5f);
                        this.cam0.setCFHokan(2, 0.015f, 0.015f);
                        System.sleep(2);
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 90.0f, 0.0f);
                        int n4 = 0;
                        while (n4 < 35) {
                            if (n4 == 0) {
                                this.enemy1.kickEnepc(1, 3);
                                this.enemy1.moveEnepc(15, this.player.px + 1.0f, this.player.pz, 38);
                                this.enemy2.kickEnepc(1, 3);
                                this.enemy2.moveEnepc(15, this.player.px + 1.0f, this.player.pz, 42);
                            } else if (n4 == 13) {
                                this.Jr_Gun.setVisible(true);
                                this.player.mtn(15, 1, 1.0f, true);
                            } else if (n4 == 21) {
                                Sound.effectPlay(196746);
                            } else if (n4 == 23) {
                                this.Jr_Gun_Effect.setRotate(0.0f, 0.0f, 150.0f);
                                this.Jr_Gun_Effect.disp(true);
                            }
                            System.sleep(1);
                            ++n4;
                        }
                    }
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    this.FIN_FLAG = true;
                    Sound.effectStop(196745);
                    Runtime.setDefocus(0, 1, this.DefaultScreenParam);
                    this.enemy10.kickEnepc(14, 10, 0);
                    break;
                }
                if (n2 == 1 && this.TRAP2 == 0) {
                    Sound.effectPlay(196745);
                    this.player.getTranslate();
                    Runtime.setPlayerControl(false);
                    this.screen.start(1, "Screen");
                    if (this.TRAP2_FIRST == 0) {
                        this.enemy1.setTranslate(2.25f, 0.0f, -14.0f);
                        this.enemy1.setRotate(0.0f, 180.0f, 0.0f);
                        this.enemy2.setTranslate(2.25f, 0.0f, -13.0f);
                        this.enemy2.setRotate(0.0f, 180.0f, 0.0f);
                        this.enemy1.setVisible(true);
                        this.enemy2.setVisible(true);
                        System.sleep(2);
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 90.0f, 0.0f);
                        this.player.setTranslate(this.player.px - 0.2f, this.player.py, this.player.pz);
                        this.EV_Camera5();
                        int n5 = 0;
                        while (n5 < 32) {
                            if (n5 == 0) {
                                this.enemy1.kickEnepc(1, 3);
                                this.enemy1.moveEnepc(15, 2.25f, -21.0f, 28);
                                this.enemy2.kickEnepc(1, 3);
                                this.enemy2.moveEnepc(15, 2.0f, -20.0f, 30);
                            } else if (n5 == 10) {
                                this.player.mtn(11, 0, 20, 20, 1, 1.0f, true);
                            }
                            System.sleep(1);
                            ++n5;
                        }
                        if (this.player.pz < -22.0f) {
                            System.println("AAAAA --- PATTERN");
                            this.enemy1.moveEnepc(17, 90.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 95.0f, -1.0f, 4);
                        } else if (this.player.pz < -20.7f && this.player.pz >= -22.0f) {
                            System.println("BBBBB --- PATTERN");
                            this.enemy1.moveEnepc(17, 85.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 90.0f, -1.0f, 4);
                        } else if (this.player.pz < -20.25f && this.player.pz >= -20.7f) {
                            System.println("CCCCC --- PATTERN");
                            this.enemy1.moveEnepc(17, 75.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 80.0f, -1.0f, 4);
                        } else {
                            System.println("DDDDD --- PATTERN");
                            this.enemy1.moveEnepc(17, 65.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 60.0f, -1.0f, 4);
                        }
                        System.sleep(7);
                        this.enemy1.kickEnepc(0, 0);
                        this.enemy2.kickEnepc(0, 0);
                        System.sleep(1);
                        this.enemy1.kickEnepc(0, 7);
                        this.enemy2.kickEnepc(0, 11);
                        System.sleep(30);
                        int n6 = 0;
                        while (n6 < 15) {
                            if (n6 == 0) {
                                this.enemy1.kickEnepc(1, 3);
                                this.enemy1.moveEnepc(15, this.player.px - 1.0f, this.player.pz, 15);
                                this.enemy2.kickEnepc(1, 3);
                                this.enemy2.moveEnepc(15, this.player.px - 1.0f, this.player.pz, 20);
                            } else if (n6 == 5) {
                                this.player.mtn(7, 1, 1.0f, true);
                            }
                            System.sleep(1);
                            ++n6;
                        }
                        Runtime.setFlags(6031, 1, 1);
                        this.TRAP2_FIRST = Runtime.getFlags(6031, 1);
                    } else {
                        this.enemy1.setTranslate(2.5f, 0.0f, -17.0f);
                        this.enemy1.setRotate(0.0f, 180.0f, 0.0f);
                        this.enemy2.setTranslate(2.0f, 0.0f, -15.5f);
                        this.enemy2.setRotate(0.0f, 180.0f, 0.0f);
                        this.enemy1.setVisible(true);
                        this.enemy2.setVisible(true);
                        this.cam0.setCFAngle(2, -28.0f, 400.0f, 0.0f, 8.0f, 42.5f);
                        this.cam0.setCFHokan(2, 0.015f, 0.015f);
                        System.sleep(2);
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 90.0f, 0.0f);
                        this.player.setTranslate(this.player.px - 0.2f, this.player.py, this.player.pz);
                        this.enemy1.kickEnepc(1, 3);
                        this.enemy1.moveEnepc(15, 2.5f, -21.0f, 15);
                        this.enemy2.kickEnepc(1, 3);
                        this.enemy2.moveEnepc(15, 2.0f, -19.5f, 17);
                        this.player.mtn(11, 0, 20, 20, 1, 1.0f, true);
                        System.sleep(18);
                        this.enemy1.kickEnepc(0, 0);
                        this.enemy2.kickEnepc(0, 0);
                        System.sleep(1);
                        if (this.player.pz < -22.0f) {
                            System.println("AAAAA --- PATTERN");
                            this.enemy1.moveEnepc(17, 90.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 95.0f, -1.0f, 4);
                        } else if (this.player.pz < -20.7f && this.player.pz >= -22.0f) {
                            System.println("BBBBB --- PATTERN");
                            this.enemy1.moveEnepc(17, 85.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 90.0f, -1.0f, 4);
                        } else if (this.player.pz < -20.25f && this.player.pz >= -20.7f) {
                            System.println("CCCCC --- PATTERN");
                            this.enemy1.moveEnepc(17, 75.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 80.0f, -1.0f, 4);
                        } else {
                            System.println("DDDDD --- PATTERN");
                            this.enemy1.moveEnepc(17, 65.0f, -1.0f, 6);
                            this.enemy2.moveEnepc(17, 60.0f, -1.0f, 4);
                        }
                        System.sleep(7);
                        this.enemy1.kickEnepc(0, 0);
                        this.enemy2.kickEnepc(0, 0);
                        System.sleep(1);
                        int n7 = 0;
                        while (n7 < 25) {
                            if (n7 == 0) {
                                this.enemy1.kickEnepc(1, 3);
                                this.enemy1.moveEnepc(15, this.player.px - 1.0f, this.player.pz, 25);
                                this.enemy2.kickEnepc(1, 3);
                                this.enemy2.moveEnepc(15, this.player.px - 1.0f, this.player.pz, 30);
                            } else if (n7 == 23) {
                                this.player.rotY(20, -90.0f, true);
                            }
                            System.sleep(1);
                            ++n7;
                        }
                    }
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    this.FIN_FLAG = true;
                    Sound.effectStop(196745);
                    Runtime.setDefocus(0, 1, this.DefaultScreenParam);
                    this.enemy10.kickEnepc(14, 10, 0);
                    break;
                }
                if (n2 == 2 && this.SHUTTER == 0 && !this.EnterCheck) {
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    Runtime.enable(65536);
                    if (this.DefaultMenu(this.Q1) == 0) {
                        System.println("シャッターを開けます");
                        if (this.player.px >= 20.55f) {
                            System.println("パターンA");
                            this.player.setRotateY(200.0f);
                        } else if (this.player.px >= 20.4f && this.player.px < 20.55f) {
                            System.println("パターンB");
                            this.player.setRotateY(190.0f);
                        } else if (this.player.px >= 20.205f && this.player.px < 20.4f) {
                            System.println("パターンC");
                            this.player.setRotateY(180.0f);
                        } else if (this.player.px >= 20.09f && this.player.px < 20.205f) {
                            System.println("パターンD");
                            this.player.setRotateY(170.0f);
                        } else if (this.player.px < 20.09f) {
                            System.println("パターンE");
                            this.player.setRotateY(160.0f);
                        }
                        this.player.mtn(16, 1, 1.0f, true);
                        int n8 = 0;
                        while (n8 < 50) {
                            if (n8 == 30) {
                                this.Obj600_A.disp(false);
                                this.Obj600_B.disp(true);
                                Sound.effectPlay(196741);
                            }
                            System.sleep(1);
                            ++n8;
                        }
                        this.EV_Camera1();
                        this.doorB_1.DoorOpen();
                        Sound.effectPlay(196744);
                        System.sleep(10);
                        this.doorB_2.DoorOpen();
                        System.sleep(70);
                        this.cam0.setMode(0);
                        Runtime.setFlags(6002, 1, 1);
                        this.SHUTTER = Runtime.getFlags(6002, 1);
                    }
                    Runtime.disable(65536);
                    this.EnterCheck = false;
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 3 && this.NO4 == 0 && !this.EnterCheck) {
                    Runtime.setPlayerControl(false);
                    this.player.getTranslate();
                    this.EnterCheck = true;
                    System.sleep(2);
                    Runtime.enable(65536);
                    this.EV_Camera3();
                    this.light.setColor(0, 0.4f, 0.4f, 0.4f);
                    this.light.setColor(1, 0.35f, 0.35f, 0.35f);
                    this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.light.setColor(2, 0.3f, 0.3f, 0.9f);
                    this.light.setDirection2(2, 0.0f, 1.5f, -1.0f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.light.setColor(3, 0.5f, 0.3f, 0.3f);
                    this.light.setDirection2(3, 0.0f, -0.25f, -1.0f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.win = Window.create();
                    this.win.setSize(3, 30);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Q2, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    if (this.menu.getSelected() == 0) {
                        System.println("NO4を開けます");
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        if (this.player.px >= 15.283f) {
                            System.println("パターンA");
                            this.player.setRotateY(200.0f);
                        } else if (this.player.px >= 15.075f && this.player.px < 15.283f) {
                            System.println("パターンB");
                            this.player.setRotateY(185.0f);
                        } else if (this.player.px >= 15.016f && this.player.px < 15.075f) {
                            System.println("パターンC");
                            this.player.setRotateY(180.0f);
                        } else if (this.player.px >= 14.81f && this.player.px < 15.016f) {
                            System.println("パターンD");
                            this.player.setRotateY(165.0f);
                        } else if (this.player.px < 14.81f) {
                            System.println("パターンE");
                            this.player.setRotateY(160.0f);
                        }
                        this.player.mtn(25, 1, 1.0f, true);
                        int n9 = 0;
                        while (n9 <= 60) {
                            if (n9 == 30) {
                                this.Obj601_A.disp(false);
                                this.Obj601_B.disp(true);
                                this.Obj602_A.disp(false);
                                this.Obj602_B.disp(true);
                                Sound.effectPlay(196741);
                            } else if (n9 == 50) {
                                this.EV_Camera11();
                            } else if (n9 == 60) {
                                this.EV_Camera00();
                            }
                            System.sleep(1);
                            ++n9;
                        }
                        System.sleep(40);
                        this.doorA.DoorOpen();
                        Sound.effectPlay(196744);
                        System.sleep(45);
                        this.EV_Camera2();
                        System.sleep(80);
                        Runtime.setFlags(6001, 1, 1);
                        this.NO4 = Runtime.getFlags(6001, 1);
                    }
                    this.cam0.setMode(0);
                    this.DefaultLight();
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 4 && this.NO4 == 0 && !this.EnterCheck) {
                    if (this.HEISHI_LIFE3 == 0) {
                        this.enemy5.kickEnepc(4, 2);
                    }
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    this.EV_Camera2();
                    this.DefaultTalk(this.NO4_CLOSE);
                    this.cam0.setMode(0);
                    this.EnterCheck = false;
                    Runtime.setPlayerControl(true);
                    if (this.HEISHI_LIFE3 != 0) break;
                    this.enemy5.kickEnepc(4, 0);
                    break;
                }
                if (n2 == 5 && this.HEISHI == 0) {
                    System.println("柱の陰に隠れた兵士イベント");
                    this.EV_Camera8();
                    this.enemy3.kickEnepc(4, 1);
                    this.enemy4.kickEnepc(4, 1);
                    this.enemy3.kickEnepc(0, 7);
                    this.enemy4.kickEnepc(0, 7);
                    System.sleep(30);
                    this.enemy3.kickEnepc(1, 3);
                    this.enemy3.moveEnepc(15, 19.5f, 15.0f, 10);
                    this.enemy4.kickEnepc(1, 3);
                    this.enemy4.moveEnepc(15, 20.5f, 15.0f, 10);
                    System.sleep(10);
                    this.enemy3.kickEnepc(7, 2);
                    System.println("俊足警備員初期化");
                    this.enemy4.kickEnepc(7, 3);
                    System.println("超俊足警備員初期化");
                    this.enemy3.kickEnepc(4, 0);
                    this.enemy4.kickEnepc(4, 0);
                    this.enemy3.dispRadar(true);
                    this.enemy4.dispRadar(true);
                    this.cam0.setMode(0);
                    Runtime.setFlags(6005, 1, 1);
                    this.HEISHI = Runtime.getFlags(6005, 1);
                    break;
                }
                if (n2 == 7 && this.ENEMY5_CHECK) break;
                if (n2 == 9 && this.SHUTTER == 0 && !this.EnterCheck) {
                    this.player.getTranslate();
                    if (this.player.pz < -17.0f) {
                        System.println("Test");
                        return;
                    }
                    System.println("奥に見えるスイッチ");
                    if (this.HEISHI_LIFE3 == 0 && this.KOW1 == 1) {
                        this.enemy5.setVisible(false);
                        this.enemy5.kickEnepc(4, 2);
                    }
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.EV_Camera9();
                    this.DefaultTalk(this.HINT);
                    System.sleep(5);
                    this.cam0.setMode(0);
                    this.EnterCheck = false;
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    if (this.HEISHI_LIFE3 != 0 || this.KOW1 != 1) break;
                    this.enemy5.setVisible(true);
                    this.enemy5.kickEnepc(4, 0);
                    break;
                }
                if ((n2 == 8 || n2 == 12) && this.SHUTTER == 0) {
                    if (this.EnterCheck) break;
                    if (this.HEISHI_LIFE3 == 0) {
                        this.enemy5.kickEnepc(4, 2);
                    }
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    this.DefaultTalk(this.CLOSE);
                    System.sleep(10);
                    Runtime.setPlayerControl(true);
                    this.EnterCheck = false;
                    if (this.HEISHI_LIFE3 != 0) break;
                    this.enemy5.kickEnepc(4, 0);
                    break;
                }
                if (n2 == 13) {
                    if (this.RADER != 0) break;
                    System.println("serial_13 レーダー・ON！");
                    Runtime.enable(524288);
                    Runtime.setFlags(6025, 1, 1);
                    this.RADER = Runtime.getFlags(6025, 1);
                    break;
                }
                if (n2 == 14) {
                    this.ManualClip(true);
                    break;
                }
                if (n2 == 15) {
                    this.ManualClip(false);
                    break;
                }
                return;
            }
        }
    }

    void ManualClip(boolean bl) {
        this.Obj605.disp(bl);
        this.Obj606.disp(bl);
        this.Obj607.disp(bl);
        this.Obj608.disp(bl);
        this.Obj609.disp(bl);
        this.Obj610.disp(bl);
        this.Obj611.disp(bl);
        this.Obj612.disp(bl);
        this.Obj614.disp(bl);
    }

    public void Talk_dead1(Enepc enepc, Window window) {
        System.println("DEAD_TALK1");
        if (this.deadFlag1 < 2) {
            window.print(this.DEAD1_TALK1, 0);
            ST1040.waitPage(window, 64);
            ++this.deadFlag1;
        } else {
            window.print(this.DEAD1_TALK2, 0);
            ST1040.waitPage(window, 64);
        }
    }

    public void Talk_dead2(Enepc enepc, Window window) {
        System.println("DEAD_TALK2");
        if (this.NO3_CARDKEY == 1) {
            window.print(this.DEAD2_TALK2, 0);
            ST1040.waitPage(window, 64);
        } else if (this.deadFlag2 < 10) {
            window.print(this.DEAD2_TALK1, 0);
            ST1040.waitPage(window, 64);
            ++this.deadFlag2;
        } else {
            window.print(this.DEAD2_TALK2, 0);
            ST1040.waitPage(window, 64);
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        System.println("NPC1_TALK1");
        if (this.NO3_CARDKEY == 0) {
            switch (this.talkFlag1) {
                case 0: {
                    window.print(this.NPC1_TALK1, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag1;
                    break;
                }
                case 1: {
                    window.print(this.NPC1_TALK2, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag1;
                    break;
                }
                default: {
                    window.print(this.NPC1_TALK3, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag1;
                    break;
                }
            }
        } else {
            switch (this.talkFlag1) {
                case 0: {
                    window.print(this.NPC1_TALK4, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag1;
                    break;
                }
                default: {
                    window.print(this.NPC1_TALK5, 0);
                    ST1040.waitPage(window, 64);
                    break;
                }
            }
        }
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        System.println("NPC2_TALK1");
        if (this.NO3_CARDKEY == 0) {
            switch (this.talkFlag2) {
                case 0: {
                    window.print(this.NPC2_TALK1, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag2;
                    break;
                }
                case 1: {
                    window.print(this.NPC2_TALK2, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag2;
                    break;
                }
                case 2: {
                    window.print(this.NPC2_TALK3, 0);
                    ST1040.waitPage(window, 64);
                    this.menu = Menu.create();
                    this.menu.setLocation(10, 205);
                    this.menu.addItem("Of course\nNo way");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    if (this.selected == 0) {
                        this.DefaultTalk(this.NPC2_TALK4);
                    } else {
                        this.DefaultTalk(this.NPC2_TALK5);
                        Runtime.setFlags(6038, 1, 1);
                        this.NO3_HINTO1 = Runtime.getFlags(6038, 1);
                        Runtime.setFlags(6049, 1, 1);
                        this.NO3_HINTO2 = Runtime.getFlags(6049, 1);
                    }
                    ++this.talkFlag2;
                    break;
                }
                default: {
                    window.print(this.NPC2_TALK6, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag2;
                    break;
                }
            }
        } else {
            switch (this.talkFlag2) {
                case 0: {
                    window.print(this.NPC2_TALK7, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag2;
                    break;
                }
                default: {
                    if (this.talkFlag1 == 0) {
                        window.print(this.NPC2_TALK8_A, 0);
                        ST1040.waitPage(window, 64);
                        ++this.talkFlag2;
                        break;
                    }
                    window.print(this.NPC2_TALK8_B, 0);
                    ST1040.waitPage(window, 64);
                    ++this.talkFlag2;
                    break;
                }
            }
        }
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        System.println("NPC3_TALK1");
        window.print(this.NPC3_TALK1, 0);
        ST1040.waitPage(window, 64);
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("TRAP1 破壊イベント！");
                Runtime.disable(524288);
                this.E_Move.setTranslate(4.0f, 0.35f, -15.6f);
                Runtime.setPlayerControl(false);
                this.EV_Camera01_S();
                System.sleep(5);
                Runtime.enable(65536);
                this.player.mtn(1, 1, 1.0f, true);
                this.E_Move.disp(true);
                int n2 = 0;
                Sound.effectPlay(196742);
                float f = 4.0f;
                while (f <= 5.6f) {
                    if (n2 == 15) {
                        this.player.rotY(55, 65.5f, true);
                    }
                    this.E_Move.setTranslate(f, 0.2f, -15.6f);
                    System.sleep(1);
                    ++n2;
                    f += 0.05f;
                }
                f = -15.6f;
                while (f <= -12.0f) {
                    this.E_Move.setTranslate(5.6f, 0.2f, f);
                    System.sleep(1);
                    f += 0.2f;
                }
                int n3 = 0;
                float f2 = 5.6f;
                while (f2 <= 10.0f) {
                    this.E_Move.setTranslate(f2, 0.2f, -12.0f);
                    System.sleep(1);
                    ++n3;
                    f2 += 0.2f;
                }
                System.sleep(40);
                this.E_Move.disp(false);
                this.E_Bang1.disp(true);
                Sound.effectPlay(196743);
                this.EV_Camera02();
                this.Obj603.disp(true);
                System.sleep(10);
                Stage.setVisible(135, false);
                Stage.setVisible(134, false);
                Stage.setVisible(132, false);
                Stage.setVisible(133, false);
                System.sleep(90);
                this.cam0.setMode(0);
                this.E_Bang1.disp(false);
                Sound.effectStop(196742);
                Runtime.enable(524288);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                this.E_Bang1.disp(false);
                Runtime.setFlags(6003, 1, 1);
                this.TRAP1 = Runtime.getFlags(6003, 1);
                this.teiten6 = new Uwamono(28690, 3.958f, 0.0f, -15.606f, 0.0f);
                this.teiten6.SetBgm(196615);
                break;
            }
            case 2: {
                System.println("TRAP2 破壊イベント！");
                if (this.HEISHI_LIFE3 == 0) {
                    this.enemy5.kickEnepc(4, 2);
                    this.enemy5.setVisible(false);
                }
                Runtime.disable(524288);
                this.E_Move.setTranslate(12.8f, 0.35f, -15.05f);
                Runtime.setPlayerControl(false);
                this.EV2_Camera01_S();
                System.sleep(5);
                Runtime.enable(65536);
                this.player.mtn(1, 1, 1.0f, true);
                Sound.effectPlay(196742);
                this.E_Move.disp(true);
                float f = 12.8f;
                while (f >= 10.0f) {
                    this.E_Move.setTranslate(f, 0.35f, -15.05f);
                    System.sleep(1);
                    f -= 0.15f;
                }
                f = -15.05f;
                while (f >= -16.5f) {
                    this.E_Move.setTranslate(10.0f, 0.35f, f);
                    System.sleep(1);
                    f -= 0.15f;
                }
                System.sleep(35);
                this.E_Move.disp(false);
                this.E_Bang2.disp(true);
                Sound.effectPlay(196743);
                Sound.effectStop(196742);
                this.player.rotY(20, 230.0f, true);
                this.EV2_Camera02();
                this.Obj604.disp(true);
                System.sleep(30);
                Stage.setVisible(136, false);
                Stage.setVisible(137, false);
                Stage.setVisible(138, false);
                Stage.setVisible(139, false);
                System.sleep(75);
                if (this.HEISHI_LIFE3 == 0) {
                    this.enemy5.kickEnepc(4, 0);
                    this.enemy5.setVisible(true);
                }
                this.cam0.setMode(0);
                this.E_Bang2.disp(false);
                Runtime.enable(524288);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                this.E_Bang2.disp(false);
                Runtime.setFlags(6004, 1, 1);
                this.TRAP2 = Runtime.getFlags(6004, 1);
                this.teiten7 = new Uwamono(28690, 15.0f, 0.0f, -16.5f, 0.0f);
                this.teiten7.SetBgm(196615);
                break;
            }
            case 3: {
                Runtime.setFlags(6006, 1, 1);
                this.KOW1 = Runtime.getFlags(6006, 1);
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1780, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(1050, 1);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.obj_01 = new MAPUnit();
        this.obj_01.mapUnit(26);
        this.obj_01.start(4, null);
        this.obj_01.getTranslate();
        this.obj_01.setTranslate(this.obj_01.px, this.obj_01.py, this.obj_01.pz + 2.5f);
        this.npc1 = new NPC_NORMAL(1025, 11, 0, 5, 5, -13.0f, 0.0f, 2.75f, 180.0f);
        this.npc1.talkto("Talk_npc1");
        this.npc1.enableDTKFlag(4);
        this.npc1.enableDTKFlag(2);
        this.npc1.setMotion(0, 10);
        if (this.NO3_CARDKEY == 0) {
            this.npc2 = new NPC_NORMAL(1026, 12, 0, 5, 5, -12.0f, 0.0f, 2.75f, 180.0f);
            this.npc2.talkto("Talk_npc2");
            this.npc2.disableDTKFlag(8);
            this.npc2.enableDTKFlag(4);
            this.npc2.enableDTKFlag(2);
        } else {
            this.npc2 = new NPC_NORMAL(1026, 12, 0, 5, 10, -14.8f, 0.0f, -41.65f, 90.0f);
            this.npc2.talkto("Talk_npc2");
            this.npc2.enableDTKFlag(4);
            this.npc2.disableDTKFlag(8);
            this.npc2.disableDTKFlag(131072);
            this.npc2.disableDTKFlag(1);
            this.npc2.disableDTKFlag(2);
            this.npc2.setMotion(0, 5);
        }
        this.npc3 = new NPC_NORMAL(1025, 13, 0, 5, 5, -12.5f, 0.0f, -40.5f, 300.0f);
        this.npc3.talkto("Talk_npc3");
        this.npc3.enableDTKFlag(4);
        this.npc3.enableDTKFlag(2);
        this.npc3.setMotion(0, 10);
        this.npc_d1 = new NPC_NORMAL(778, 14, 0, 5, 9, -11.5f, 0.0f, -28.75f, 165.0f);
        this.npc_d1.talkto("Talk_dead1");
        this.npc_d1.disableDTKFlag(131072);
        this.npc_d1.disableDTKFlag(1);
        this.npc_d1.disableDTKFlag(2);
        this.npc_d1.dispRadar(false);
        this.npc_d1.setMotion(0, 4);
        this.npc_d2 = new NPC_NORMAL(778, 15, 0, 5, 9, -11.25f, 0.0f, -19.25f, 270.0f);
        this.npc_d2.talkto("Talk_dead2");
        this.npc_d2.setMotion(0, 5);
        this.npc_d2.disableDTKFlag(1);
        this.npc_d2.disableDTKFlag(131072);
        this.npc_d2.dispRadar(false);
        this.npc_d2.disableDTKFlag(2);
        this.col = new Uwamono(28672, -11.25f, 0.0f, -19.25f, 270.0f);
        this.col.SetSize(0.5f, 1.0f, 1.25f);
        if (this.NO3_CARDKEY == 0) {
            this.npc3.enableDTKFlag(4);
        }
        if (this.RADER == 0) {
            Runtime.disable(524288);
        }
        this.teiten1 = new Uwamono(28690, -14.69f, 0.0f, -40.677f, 0.0f);
        this.teiten1.SetBgm(196612);
        this.teiten2 = new Uwamono(28690, -17.75f, 0.0f, -33.5f, 0.0f);
        this.teiten2.SetBgm(196612);
        this.teiten3 = new Uwamono(28690, -11.4f, 0.0f, -29.5f, 0.0f);
        this.teiten3.SetBgm(196613);
        this.teiten4 = new Uwamono(28690, -14.5f, 0.0f, -16.5f, 0.0f);
        this.teiten4.SetBgm(196614);
        this.teiten5 = new Uwamono(28690, -14.0f, 0.0f, 2.5f, 0.0f);
        this.teiten5.SetBgm(196615);
        this.teiten8 = new Uwamono(28690, 10.0f, 0.0f, -7.0f, 0.0f);
        this.teiten8.SetBgm(196616);
        this.teiten9 = new Uwamono(28690, 10.0f, 0.0f, -21.0f, 0.0f);
        this.teiten9.SetBgm(196616);
        this.teiten10 = new Uwamono(28690, 5.5f, 0.0f, -11.5f, 0.0f);
        this.teiten10.SetBgm(196617);
        this.teiten11 = new Uwamono(28690, 8.0f, 0.0f, -10.5f, 0.0f);
        this.teiten11.SetBgm(196617);
        this.teiten12 = new Uwamono(28690, 6.5f, 0.0f, -17.0f, 0.0f);
        this.teiten12.SetBgm(196617);
        this.teiten13 = new Uwamono(28690, 10.0f, 0.0f, -14.0f, 0.0f);
        this.teiten13.SetBgm(196617);
        this.teiten14 = new Uwamono(28690, 20.35f, 0.0f, -24.784f, 0.0f);
        this.teiten14.SetBgm(196618);
        this.teiten15 = new Uwamono(28690, -14.5f, 0.0f, 0.0f, 90.0f);
        this.teiten15.SetBgm(196617);
        this.teiten15.SetBgmType('\u0001');
        if (this.NO4 == 0) {
            this.Jr_Gun_Effect = new Effect(1702, 0.0f, 0.0f, 0.0f, 0.0f);
            this.Jr_Gun_Effect.setRotate(0.0f, 90.0f, 0.0f);
            this.Jr_Gun_Effect.disp(false);
            this.Jr_Gun = new Unit();
            this.Jr_Gun.init(24644, 0.0f, 0.0f, 0.0f, 0.0f);
            this.Jr_Gun.start(4, null);
            this.Jr_Gun.setParent(this.player, 72);
            this.Jr_Gun.setVisible(false);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.325f, 0.325f, 0.325f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.325f, 0.325f, 0.325f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.325f, 0.325f, 0.325f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 1, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 2, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightCol(1, 3, 0.225f, 0.225f, 0.225f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 1, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 2, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 3, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(3, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightCol(4, 1, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightCol(4, 2, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightCol(4, 3, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 345.0f, 0.0f, 10.0f, 41.5f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFLockX(3, 20.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 6.0f, 42.5f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFLockX(4, 20.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.cam0.setCFLockX(6, 20.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(7, 0.015f, 0.015f);
        this.cam0.setCFLockX(7, 20.0f);
        this.cam0.setCFAngle(8, -28.0f, 15.0f, 0.0f, 6.0f, 42.5f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFPedestal(9, -15.763621f, 12.274669f, -33.92609f, 52.58906f, -71.2932f, -15.561236f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(10, 0.02f, 0.02f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(11, 0.02f, 0.02f);
        this.cam0.setCFLockX(11, 6.0f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 10.0f, 50.0f);
        this.cam0.setCFHokan(12, 0.02f, 0.02f);
        this.cam0.setCFPedestal(13, 13.016f, 9.767789f, 15.914001f, 40.559406f, -87.52626f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(13, 100.0f, 100.0f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 10.0f, 50.0f);
        this.cam0.setCFHokan(14, 100.0f, 100.0f);
        this.cam0.setCFAngle(15, -28.0f, -7.0f, 0.0f, 10.0f, 50.0f);
        this.cam0.setCFHokan(15, 0.02f, 0.02f);
        this.cam0.setCFAngle(16, -28.0f, 0.0f, 0.0f, 10.0f, 42.5f);
        this.cam0.setCFHokan(16, 100.0f, 100.0f);
        this.cam0.setCFLockX(16, 6.0f);
        this.cam0.setCFAngle(17, -28.0f, 0.0f, 0.0f, 7.0f, 42.5f);
        this.cam0.setCFHokan(17, 0.02f, 0.02f);
        this.cam0.setFog(1, 8.5f, 15.0f, 0.0f, 0.15f, 75, 75, 75, 255);
        this.cam0.setFog(9, 3.0f, 5.0f, 0.0f, 0.1f, 75, 75, 75, 255);
        this.doorA = new Uwamono(38, 42, '\u0001');
        new Uwamono(37, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0002');
        this.doorA.SetDoorRange(2.0f);
        this.doorA.SetDoorSpd(70);
        this.doorB_1 = new Uwamono(186, 40, '\u0001');
        this.doorB_1.SetDoorType('\u0002');
        this.doorB_1.SetDoorRange(2.0f);
        this.doorB_1.SetDoorSpd(55);
        this.doorB_2 = new Uwamono(187, 40, '\u0001');
        this.doorB_2.SetDoorType('\u0002');
        this.doorB_2.SetDoorRange(2.0f);
        this.doorB_2.SetDoorSpd(55);
        this.door_2 = new Uwamono(18, 40, '\u0001');
        this.door_2.SetDoorType('\u0004');
        this.door_3 = new Uwamono(19, 40, '\u0001');
        this.door_3.SetDoorType('\u0004');
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 126);
        this.item3 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 127);
        this.item5 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 128);
        this.item7 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 129);
        this.item9 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 130);
        this.item11 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 131);
        new Uwamono(188, 30, this.item9);
        new Uwamono(189, 30, this.item2);
        new Uwamono(190, 30, this.item3);
        new Uwamono(191, 30);
        new Uwamono(192, 30, this.item5);
        new Uwamono(193, 30);
        new Uwamono(90, 20, this.item7);
        new Uwamono(15, 1, this.item11);
        this.Obj603 = new Effect(1432, 3);
        this.Obj603.disp(false);
        this.Obj603.setClip(true);
        this.Obj604 = new Effect(1432, 4);
        this.Obj604.disp(false);
        this.Obj604.setClip(true);
        this.Obj605 = new Effect(1403, 5);
        this.Obj605.disp(true);
        this.Obj605.setClip(true);
        this.Obj606 = new Effect(1403, 6);
        this.Obj606.disp(true);
        this.Obj606.setClip(true);
        this.Obj607 = new Effect(1402, 7);
        this.Obj607.setScale(1.5f, 1.5f, 1.5f);
        this.Obj607.disp(true);
        this.Obj607.setClip(true);
        this.Obj608 = new Effect(1402, 8);
        this.Obj608.setScale(1.5f, 1.5f, 1.5f);
        this.Obj608.disp(true);
        this.Obj608.setClip(true);
        this.Obj609 = new Effect(1432, 9);
        this.Obj608.setScale(1.5f, 1.5f, 1.5f);
        this.Obj609.disp(true);
        this.Obj609.setClip(true);
        this.Obj610 = new Effect(1402, 10);
        this.Obj610.setScale(0.75f, 2.0f, 1.75f);
        this.Obj610.disp(true);
        this.Obj610.setClip(true);
        this.Obj611 = new Effect(1528, 11);
        this.Obj611.disp(true);
        this.Obj611.setClip(true);
        this.Obj612 = new Effect(1722, 12);
        this.Obj612.setScale(3.5f, 2.0f, 3.5f);
        this.Obj612.disp(true);
        this.Obj612.setClip(true);
        this.Obj613 = new Effect(1722, 13);
        this.Obj613.setScale(3.0f, 2.0f, 3.0f);
        this.Obj613.disp(true);
        this.Obj613.setClip(true);
        this.Obj614 = new Effect(1722, 14);
        this.Obj614.setScale(3.0f, 2.0f, 3.0f);
        this.Obj614.disp(true);
        this.Obj614.setClip(true);
        if (this.NO4 == 0) {
            System.println("NO4扉閉　初期化");
            this.Obj601_A = new Effect(1431, 1);
            this.Obj601_A.disp(true);
            this.Obj601_A.setClip(true);
            this.Obj602_A = new Effect(1434, 2);
            this.Obj602_A.disp(true);
            this.Obj602_A.setClip(true);
            this.Obj601_B = new Effect(1430, 1);
            this.Obj601_B.disp(false);
            this.Obj601_B.setClip(true);
            this.Obj602_B = new Effect(1433, 2);
            this.Obj602_B.disp(false);
            this.Obj602_B.setClip(true);
        } else {
            this.doorA.DoorOpen();
            this.Obj601_B = new Effect(1430, 1);
            this.Obj601_B.disp(true);
            this.Obj601_B.setClip(true);
            this.Obj602_B = new Effect(1433, 2);
            this.Obj602_B.disp(true);
            this.Obj602_B.setClip(true);
        }
        if (this.SHUTTER == 0) {
            System.println("シャッター閉　初期化");
            this.Obj600_A = new Effect(1431, 0);
            this.Obj600_A.disp(true);
            this.Obj600_A.setClip(true);
            this.Obj600_B = new Effect(1430, 0);
            this.Obj600_B.disp(false);
            this.Obj600_B.setClip(true);
        } else {
            this.Obj600_B = new Effect(1430, 0);
            this.Obj600_B.disp(true);
            this.Obj600_B.setClip(true);
            this.doorB_1.DoorOpen();
            this.doorB_2.DoorOpen();
            Stage.setVisible(135, false);
            Stage.setVisible(134, false);
            Stage.setVisible(132, false);
            Stage.setVisible(133, false);
            Stage.setVisible(136, false);
            Stage.setVisible(137, false);
            Stage.setVisible(138, false);
            Stage.setVisible(139, false);
            Stage.setVisible(194, false);
            Stage.setVisible(195, false);
        }
        this.screen = new Mapunits();
        this.screen.mapUnit(182);
        if (this.TRAP1 == 1) {
            Stage.setVisible(135, false);
            Stage.setVisible(134, false);
            Stage.setVisible(132, false);
            Stage.setVisible(133, false);
            Stage.setVisible(194, false);
            this.Obj603.disp(true);
            this.teiten6 = new Uwamono(28690, 3.958f, 0.0f, -15.606f, 0.0f);
            this.teiten6.SetBgm(196615);
        } else {
            this.K1 = new Uwamono(194, 9);
            this.K1.SetCallNo(1);
        }
        if (this.TRAP2 == 1) {
            Stage.setVisible(136, false);
            Stage.setVisible(137, false);
            Stage.setVisible(138, false);
            Stage.setVisible(139, false);
            Stage.setVisible(195, false);
            this.Obj604.disp(true);
            this.teiten7 = new Uwamono(28690, 15.0f, 0.0f, -16.5f, 0.0f);
            this.teiten7.SetBgm(196615);
        } else {
            this.K2 = new Uwamono(195, 9);
            this.K2.SetCallNo(2);
        }
        if (this.TRAP1 == 0 || this.TRAP2 == 0 || this.NO4 == 0) {
            System.println("トラップ発火エフェク初期化");
            this.E_Move = new Effect(1432, 4.0f, 0.2f, -15.6f, 0.0f);
            this.E_Move.disp(false);
            this.E_Bang1 = new Effect(1508, 10.0f, 0.2f, -12.0f, 0.0f);
            this.E_Bang1.disp(false);
            this.E_Bang2 = new Effect(1508, 10.0f, 0.35f, -16.5f, 0.0f);
            this.E_Bang2.disp(false);
            System.println("トラップ兵士初期化");
            this.enemy1 = new Enepc();
            this.enemy1.init(17153, 6, 0.0f, 0.0f, 0.0f, 0.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(5, 5, 5, 5);
            this.enemy1.setParams(0, 0, 1, 6);
            this.enemy1.setTranslate(19.0f, 0.0f, -1.0f);
            this.enemy1.setRotate(0.0f, 180.0f, 0.0f);
            this.enemy1.setBatEvent(10);
            this.enemy1.setVisible(false);
            this.enemy2 = new Enepc();
            this.enemy2.init(17153, 6, 0.0f, 0.0f, 0.0f, 0.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(5, 5, 5, 5);
            this.enemy2.setParams(0, 0, 2, 6);
            this.enemy2.setTranslate(20.0f, 0.0f, -2.0f);
            this.enemy2.setRotate(0.0f, 180.0f, 0.0f);
            this.enemy2.setBatEvent(10);
            this.enemy2.setVisible(false);
            System.println("戦闘呼び出し用ENEMY");
            this.enemy10 = new Enepc();
            this.enemy10.init(17153, 6, 0.0f, 0.0f, 7.0f, 0.0f);
            this.enemy10.id = 10;
            this.enemy10.setGroup(5, 5, 5, 5);
            this.enemy10.setParams(0, 0, 10, 6);
            this.enemy10.setBatEvent(10);
            this.enemy10.setInvalidID(1);
            this.enemy10.setTogetherWith(-2, -1, -1, -1);
            this.enemy10.setVisible(false);
        }
        if (this.HEISHI_LIFE3 == 0) {
            System.println("潜伏する兵士初期化");
            this.enemy5 = new Enepc();
            this.enemy5.init(17153, 3, 22.728f, 0.0f, -13.0f, 270.0f);
            this.enemy5.id = 5;
            this.enemy5.setBatEvent(2);
            this.enemy5.enableDTKFlag(262144);
            float[] fArray2 = new float[20];
            fArray2[0] = 18.0f;
            fArray2[2] = -1.0f;
            fArray2[3] = -1.0f;
            fArray2[4] = 19.0f;
            fArray2[6] = -6.5f;
            fArray2[8] = 20.0f;
            fArray2[10] = -13.1f;
            fArray2[11] = 1.0f;
            fArray2[12] = 2.0f;
            fArray2[14] = -6.5f;
            fArray2[15] = 1.0f;
            fArray2[16] = 20.0f;
            fArray2[18] = -14.85f;
            fArray2[19] = 2.0f;
            fArray = fArray2;
            this.enemy5.setGroup(1, 1, 1, 1);
            this.enemy5.setParams(1, 1, 5, 3, fArray);
        }
        if (this.KOW1 == 0 && this.HEISHI_LIFE3 == 0) {
            System.println("潜伏する兵士 Aパターン");
            this.K3 = new Uwamono(36, 20, this.enemy5);
            this.K3.SetCallNo(3);
        } else if (this.KOW1 == 1 && this.HEISHI_LIFE3 == 0) {
            System.println("潜伏する兵士 Bパターン");
            new Uwamono(36, 20);
            this.enemy5.setTranslate(18.0f, 0.0f, -1.0f);
            this.enemy5.setRotate(0.0f, 180.0f, 0.0f);
        } else {
            new Uwamono(36, 20);
        }
        float[] fArray3 = new float[68];
        fArray3[0] = 17.85f;
        fArray3[2] = 17.0f;
        fArray3[3] = -1.0f;
        fArray3[4] = 17.85f;
        fArray3[6] = 20.5f;
        fArray3[8] = 19.25f;
        fArray3[10] = 17.0f;
        fArray3[12] = 19.25f;
        fArray3[14] = 17.0f;
        fArray3[15] = 2.0f;
        fArray3[16] = 19.25f;
        fArray3[18] = 15.0f;
        fArray3[19] = 3.0f;
        fArray3[20] = 19.25f;
        fArray3[22] = 13.0f;
        fArray3[23] = 4.0f;
        fArray3[24] = 19.25f;
        fArray3[26] = 11.0f;
        fArray3[27] = 5.0f;
        fArray3[28] = 14.375f;
        fArray3[30] = 20.5f;
        fArray3[31] = 1.0f;
        fArray3[32] = 12.0f;
        fArray3[34] = 21.0f;
        fArray3[35] = 7.0f;
        fArray3[36] = 11.0f;
        fArray3[38] = 20.5f;
        fArray3[39] = 8.0f;
        fArray3[40] = 10.0f;
        fArray3[42] = 21.0f;
        fArray3[43] = 9.0f;
        fArray3[44] = 9.35f;
        fArray3[46] = 20.5f;
        fArray3[47] = 10.0f;
        fArray3[48] = 7.75f;
        fArray3[50] = 20.5f;
        fArray3[51] = 11.0f;
        fArray3[52] = 7.75f;
        fArray3[54] = 17.25f;
        fArray3[55] = 12.0f;
        fArray3[56] = 7.75f;
        fArray3[58] = 15.0f;
        fArray3[59] = 13.0f;
        fArray3[60] = 7.75f;
        fArray3[62] = 13.0f;
        fArray3[63] = 14.0f;
        fArray3[64] = 7.75f;
        fArray3[66] = 11.0f;
        fArray3[67] = 15.0f;
        fArray = fArray3;
        float[] fArray4 = new float[54];
        fArray4[0] = 18.0f;
        fArray4[2] = 17.0f;
        fArray4[3] = 20.0f;
        fArray4[5] = 17.0f;
        fArray4[6] = 20.0f;
        fArray4[8] = 21.0f;
        fArray4[9] = 13.0f;
        fArray4[11] = 21.0f;
        fArray4[12] = 6.0f;
        fArray4[14] = 21.0f;
        fArray4[15] = 6.0f;
        fArray4[17] = 16.5f;
        fArray4[18] = 6.0f;
        fArray4[20] = 11.0f;
        fArray4[21] = 6.0f;
        fArray4[23] = 16.5f;
        fArray4[24] = 6.0f;
        fArray4[26] = 21.0f;
        fArray4[27] = 13.0f;
        fArray4[29] = 21.0f;
        fArray4[30] = 20.0f;
        fArray4[32] = 21.0f;
        fArray4[33] = 20.0f;
        fArray4[35] = 16.0f;
        fArray4[36] = 20.0f;
        fArray4[38] = 11.0f;
        fArray4[39] = 20.0f;
        fArray4[41] = 6.0f;
        fArray4[42] = 20.0f;
        fArray4[44] = 4.0f;
        fArray4[45] = 20.0f;
        fArray4[47] = 9.0f;
        fArray4[48] = 20.0f;
        fArray4[50] = 17.0f;
        fArray4[51] = 18.0f;
        fArray4[53] = 17.0f;
        float[] fArray5 = fArray4;
        float[] fArray6 = new float[120];
        fArray6[0] = 22.15f;
        fArray6[2] = 17.0f;
        fArray6[3] = -1.0f;
        fArray6[4] = 22.15f;
        fArray6[6] = 19.0f;
        fArray6[8] = 20.75f;
        fArray6[10] = 17.0f;
        fArray6[12] = 20.75f;
        fArray6[14] = 17.0f;
        fArray6[15] = 2.0f;
        fArray6[16] = 20.75f;
        fArray6[18] = 15.0f;
        fArray6[19] = 3.0f;
        fArray6[20] = 20.75f;
        fArray6[22] = 13.0f;
        fArray6[23] = 4.0f;
        fArray6[24] = 20.75f;
        fArray6[26] = 11.0f;
        fArray6[27] = 5.0f;
        fArray6[28] = 22.15f;
        fArray6[30] = 21.25f;
        fArray6[31] = 1.0f;
        fArray6[32] = 22.15f;
        fArray6[34] = 22.5f;
        fArray6[35] = 7.0f;
        fArray6[36] = 20.0f;
        fArray6[38] = 22.5f;
        fArray6[39] = 8.0f;
        fArray6[40] = 18.0f;
        fArray6[42] = 22.5f;
        fArray6[43] = 9.0f;
        fArray6[44] = 16.0f;
        fArray6[46] = 22.5f;
        fArray6[47] = 10.0f;
        fArray6[48] = 14.0f;
        fArray6[50] = 22.5f;
        fArray6[51] = 11.0f;
        fArray6[52] = 13.0f;
        fArray6[54] = 22.5f;
        fArray6[55] = 12.0f;
        fArray6[56] = 12.0f;
        fArray6[58] = 20.0f;
        fArray6[59] = 13.0f;
        fArray6[60] = 11.0f;
        fArray6[62] = 19.5f;
        fArray6[63] = 14.0f;
        fArray6[64] = 10.0f;
        fArray6[66] = 20.0f;
        fArray6[67] = 15.0f;
        fArray6[68] = 9.75f;
        fArray6[70] = 22.5f;
        fArray6[71] = 16.0f;
        fArray6[72] = 9.75f;
        fArray6[74] = 22.5f;
        fArray6[75] = 17.0f;
        fArray6[76] = 7.2f;
        fArray6[78] = 22.5f;
        fArray6[79] = 18.0f;
        fArray6[80] = 5.75f;
        fArray6[82] = 22.5f;
        fArray6[83] = 19.0f;
        fArray6[84] = 4.25f;
        fArray6[86] = 22.5f;
        fArray6[87] = 20.0f;
        fArray6[88] = 4.25f;
        fArray6[90] = 20.25f;
        fArray6[91] = 21.0f;
        fArray6[92] = 4.25f;
        fArray6[94] = 18.0f;
        fArray6[95] = 22.0f;
        fArray6[96] = 4.25f;
        fArray6[98] = 15.75f;
        fArray6[99] = 23.0f;
        fArray6[100] = 4.25f;
        fArray6[102] = 13.5f;
        fArray6[103] = 24.0f;
        fArray6[104] = 4.25f;
        fArray6[106] = 11.0f;
        fArray6[107] = 25.0f;
        fArray6[108] = 6.0f;
        fArray6[110] = 8.5f;
        fArray6[111] = 26.0f;
        fArray6[112] = 6.875f;
        fArray6[114] = 9.0f;
        fArray6[115] = 27.0f;
        fArray6[116] = 7.75f;
        fArray6[118] = 9.5f;
        fArray6[119] = 28.0f;
        float[] fArray7 = fArray6;
        if (this.HEISHI_LIFE1 == 0) {
            System.println("enemy 3 定義");
            this.enemy3 = new Enepc();
            this.enemy3.init(17153, 6, 17.5f, 0.0f, 15.0f, 90.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(0, 0, 1, 1);
            this.enemy3.enableDTKFlag(262144);
            this.enemy3.setBatEvent(2);
        }
        if (this.HEISHI_LIFE2 == 0) {
            System.println("enemy 4 定義");
            this.enemy4 = new Enepc();
            this.enemy4.init(17153, 6, 22.5f, 0.0f, 15.0f, 270.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(0, 0, 1, 1);
            this.enemy4.enableDTKFlag(262144);
            this.enemy4.setBatEvent(2);
        }
        if (this.HEISHI == 0) {
            System.println("柱の陰に隠れた兵士初期化");
            this.enemy3.setParams(2, 0, 3, 6, fArray);
            this.enemy3.dispRadar(false);
            this.enemy4.setParams(2, 0, 4, 6, fArray7);
            this.enemy4.dispRadar(false);
        } else if (this.HEISHI == 1) {
            if (this.HEISHI_LIFE1 == 0 && this.HEISHI_LIFE2 == 0) {
                System.println("柱の陰終了後 Aパターン");
                this.enemy3.setTranslate(18.0f, 0.0f, 17.0f);
                this.enemy3.setParams(2, 4, 3, 3, fArray);
                this.enemy3.setParams(fArray5);
                this.enemy4.setTranslate(22.0f, 0.0f, 17.0f);
                this.enemy4.setParams(2, 3, 4, 3, fArray7);
            } else if (this.HEISHI_LIFE1 == 0 && this.HEISHI_LIFE2 == 1) {
                System.println("柱の陰終了後 Bパターン");
                this.enemy3.setTranslate(18.0f, 0.0f, 17.0f);
                this.enemy3.setParams(2, 4, 3, 3, fArray);
                this.enemy3.setRotateY(180.0f);
                this.enemy3.setParams(fArray5);
            } else if (this.HEISHI_LIFE1 == 1 && this.HEISHI_LIFE2 == 0) {
                System.println("柱の陰終了後 Cパターン");
                this.enemy4.setTranslate(22.0f, 0.0f, 17.0f);
                this.enemy4.setParams(2, 3, 4, 3, fArray7);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Screen() {
            ST1040.this.SCREEN_FLAG = true;
            ST1040.this.FIN_FLAG = false;
            System.println("画面の点滅開始");
            int n = 0;
            while (ST1040.this.SCREEN_FLAG) {
                if (ST1040.this.FIN_FLAG) {
                    Runtime.setDefocus(0, 1, ST1040.this.DefaultScreenParam);
                    ST1040.this.SCREEN_FLAG = false;
                    break;
                }
                if (n % 20 == 0) {
                    n = 0;
                    Runtime.setDefocus(0, 1, ST1040.this.DefaultScreenParam);
                } else if (n % 10 == 0) {
                    Runtime.setDefocus(0, 1, ST1040.this.RedScreenParam);
                }
                ++n;
                System.sleep(1);
            }
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

        void init() {
        }

        public void talk(Window window) {
        }
    }
}

