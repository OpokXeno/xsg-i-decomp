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
import xeno.map.MC_UTK05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1050
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
        MC_UTK05_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int selected = 0;
    int Blink_Count = 0;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy7;
    Enepc enemy8;
    Enepc enemy10;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc_d1;
    Enepc npc_d2;
    Enepc npc_d3;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono door_0;
    Uwamono door_1;
    Uwamono door_2;
    Uwamono door_3;
    Uwamono door_4;
    Uwamono door_5;
    Uwamono K1;
    Uwamono K2;
    Uwamono col_1;
    Uwamono col_2;
    Uwamono col_3;
    Uwamono col_4;
    Uwamono item1;
    Uwamono item3;
    Uwamono item5;
    Uwamono item7;
    Uwamono item9;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten_s1;
    Uwamono teiten_s2;
    Uwamono teiten_s3;
    Uwamono teiten_s4;
    Unit READER;
    Effect Obj600;
    Effect Obj601;
    Effect Obj602_A;
    Effect Obj602_B;
    Effect Obj603_A;
    Effect Obj603_B;
    Effect Obj604_A;
    Effect Obj604_B;
    Effect Obj605_A;
    Effect Obj605_B;
    Effect Obj605_C;
    Effect Obj606;
    Effect Obj607;
    Effect E_Move1;
    Effect E_Move2;
    Effect E_Bang1;
    Effect E_Bang2;
    Effect fade;
    Effect Jr_Gun_Effect;
    Unit Jr_Gun;
    Effect Blood_A;
    Effect Blood_B;
    Effect Blood_C;
    Effect Blood_D;
    Effect Blood_E;
    Effect Blood_F;
    MAPUnit screen;
    MAPUnit switch1;
    MAPUnit destroy1;
    MAPUnit switch2;
    MAPUnit destroy2;
    MAPUnit sound;
    MAPUnit sound_blink;
    MAPUnit Oto;
    boolean SCREEN_FLAG = false;
    boolean FIN_FLAG = false;
    boolean BLINK_FLAG1 = false;
    boolean BLINK_FLAG2 = false;
    boolean EnterCheck = false;
    boolean BATTLE_MODE = false;
    boolean SOUND_FLAG = false;
    boolean SOUND_BLINK_FLAG = false;
    boolean SOUND_NUMBER = false;
    Light light = new Light(0);
    int TalkFlag1A = 0;
    int TalkFlag1B = 0;
    int TalkFlag2 = 0;
    int TalkFlag3 = 0;
    int TalkFlag4 = 0;
    int TalkFlag5 = 0;
    int[] RedScreenParam;
    int[] DefaultScreenParam;
    int[] Tenmetu;
    int[] timing;
    int NO4;
    int NO3;
    int NO5;
    int KOW1;
    int KOW2;
    int CONSOLE;
    int SWITCH;
    int HEISHI;
    int ROOM4;
    int KIZETU;
    int DEAD_TALK;
    int TRAP_FIRST;
    int FROM1060;
    int NO3_HINTO1;
    int NO3_HINTO2;
    int NO3_HINTO3;
    int NO3_CARDKEY;
    int ROOM4_CARDKEY;
    int HEISHI_LIFE1;
    int HEISHI_LIFE2;
    int page;
    String[] CardReader1;
    String[] CardReader2;
    String[] CardReader3;
    String[] CardReader4;
    String[] Console1;
    String[] NO3_TALK;
    String[] NO5_TALK;
    String[] NO3_TALK_A;
    String[] NO3_TALK_B;
    String[] Q_NO3_CARDKEY;
    String[] Q_ROOM4_CARDKEY;
    String[] NPC1_TALK1;
    String[] NPC1_TALK2;
    String[] NPC1_TALK3;
    String[] NPC2_TALK1;
    String[] NPC2_TALK2;
    String[] NPC2_TALK3;
    String[] NPC3_TALK1;
    String[] NPC3_TALK2;
    String[] NPC3_TALK3;
    String[] NPC3_TALK4;
    String[] NPC3_TALK5;
    String[] npc_d_TALK1;
    String[] npc_d_TALK2;
    String[] npc_d_TALK3;
    String[] npc_d_TALK4;
    String[] npc_d_TALK5;
    String[] npc_d_TALK6;
    String[] npc_d_TALK7;
    String[] Q_CARDKEY2;

    ST1050() {
        int[] nArray = new int[8];
        nArray[1] = 1;
        nArray[2] = 0x100000;
        nArray[3] = 0x600000FF;
        this.RedScreenParam = nArray;
        int[] nArray2 = new int[8];
        nArray2[2] = 0x100000;
        nArray2[3] = 0x40404040;
        this.DefaultScreenParam = nArray2;
        this.Tenmetu = new int[]{10, 18, 24, 29, 34, 37, 39, 40};
        this.timing = new int[]{15, 10, 8, 6, 5, 4, 3, 2, 1};
        this.NO4 = Runtime.getFlags(6001, 1);
        this.NO3 = Runtime.getFlags(6010, 1);
        this.NO5 = Runtime.getFlags(6011, 1);
        this.KOW1 = Runtime.getFlags(6012, 1);
        this.KOW2 = Runtime.getFlags(6013, 1);
        this.CONSOLE = Runtime.getFlags(6014, 1);
        this.SWITCH = Runtime.getFlags(6015, 1);
        this.HEISHI = Runtime.getFlags(6016, 1);
        this.ROOM4 = Runtime.getFlags(6017, 1);
        this.KIZETU = Runtime.getFlags(6020, 1);
        this.DEAD_TALK = Runtime.getFlags(6027, 1);
        this.TRAP_FIRST = Runtime.getFlags(6032, 1);
        this.FROM1060 = Runtime.getFlags(6033, 1);
        this.NO3_HINTO1 = Runtime.getFlags(6038, 1);
        this.NO3_HINTO2 = Runtime.getFlags(6049, 1);
        this.NO3_HINTO3 = Runtime.getFlags(6050, 1);
        this.NO3_CARDKEY = Runtime.checkItem(10, 3);
        this.ROOM4_CARDKEY = Runtime.checkItem(10, 4);
        this.HEISHI_LIFE1 = Runtime.getFlags(6018, 1);
        this.HEISHI_LIFE2 = Runtime.getFlags(6019, 1);
        this.CardReader1 = new String[]{"...\n", "...\n", "You inserted /[color(0x329bbe)]Card No. 3/[color(0x808080)].", "/[waitkey(64)]/[close()]"};
        this.CardReader2 = new String[]{"There's a key card slot.", "/[waitkey(64)]/[close()]"};
        this.CardReader3 = new String[]{"It looks as though the door will not open without the key card.", "/[waitkey(64)]/[close()]"};
        this.CardReader4 = new String[]{"I have to slide the /[color(0x329bbe)]U-TIC Card/[color(0x808080)] through the card reader...", "/[waitkey(64)]/[close()]"};
        this.Console1 = new String[]{"It looks as though the door cannot be controlled unless you boot up the console.", "/[waitkey(64)]/[close()]"};
        this.NO3_TALK = new String[]{"/[label(Jr.)]", "Doesn't look like it'll open.", "/[waitkey(64)]/[close()]"};
        this.NO5_TALK = new String[]{"/[label(Jr.)]", "No. 5...huh? I wonder if there is a switch somewhere.", "/[waitkey(64)]/[close()]"};
        this.NO3_TALK_A = new String[]{"/[label(Jr.)]", "This is door No. 3. Considering the layout of the ship, the bridge should be past this door...", "/[waitkey(64)]/[close()]"};
        this.NO3_TALK_B = new String[]{"/[label(Jr.)]", "This is door No. 3. Just as I heard earlier, it looks like the bridge is beyond this door...but there's no way to open it.", "/[waitkey(1)]/[clear()]", "I guess I'll try another way first.", "/[waitkey(64)]/[close()]"};
        this.Q_NO3_CARDKEY = new String[]{"There's a key card slot. It looks like /[color(0x329bbe)]Card No. 3/[color(0x808080)]\nmight work on it.", "/[waitkey(1)]/[clear()]", "Use /[color(0x329bbe)]Card No. 3/[color(0x808080)]?", "/[waitkey(64)]/[close()]"};
        this.Q_ROOM4_CARDKEY = new String[]{"There's a key card slot. It looks like the /[color(0x329bbe)]U-TIC Card/[color(0x808080)]\nmight work on it.", "/[waitkey(1)]/[clear()]", "Use the /[color(0x329bbe)]U-TIC Card/[color(0x808080)]?", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK1 = new String[]{"Oh, Little Master, you aren't hurt, are you? We were attacked when the door closed suddenly. My carelessness resulted in our men being wounded.\nI'm sorry.", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK2 = new String[]{"But we were saved when Door No. 4 opened. I think we got away without heavy casualties.", "/[waitkey(64)]/[close()]"};
        this.NPC1_TALK3 = new String[]{"Little Master, it is believed some of our forces have gone further up ahead.", "/[waitkey(1)]/[clear()]", "Please tell them to fall back if the situation gets worse.", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK1 = new String[]{"Door No. 3 was open, but it started to close suddenly, so we scrambled to get inside. That's when they ambushed us. Most of the shots missed me, but he took\nquite a number of hits because he was in the front.", "/[waitkey(1)]/[clear()]", "Thankfully, I think he will be okay, since none of his vitals were hit...", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK2 = new String[]{"When Door No. 3 closed, Doors No. 4 and No. 5 began to close too. Some of our allies rushed back out, but I wonder if they made it out okay?", "/[waitkey(64)]/[close()]"};
        this.NPC2_TALK3 = new String[]{"We're fine. Leave this place to us. Please hurry on ahead, Little Master.", "/[waitkey(64)]/[close()]"};
        this.NPC3_TALK1 = new String[]{"Ha ha ha ha ha. I'm sorry, Little Master. I had...let my guard down a little.\n", "/[waitkey(1)]/[clear()]", "Well, I'll be fine after I...rest a little. Please hurry on ahead.", "/[waitkey(64)]/[close()]"};
        this.NPC3_TALK2 = new String[]{"Pitiful, isn't it? Those guys would never have gotten me if I hadn't leapt out there so carelessly...owww.", "/[waitkey(64)]/[close()]"};
        this.NPC3_TALK3 = new String[]{"I don't feel...very well.", "/[waitkey(64)]/[close()]"};
        this.NPC3_TALK4 = new String[]{"It looks like he passed out.", "/[waitkey(64)]/[close()]"};
        this.NPC3_TALK5 = new String[]{"He is unconscious...", "/[waitkey(64)]/[close()]"};
        this.npc_d_TALK1 = new String[]{"/[label(Jr.)]", "So this is the location I heard about. Attacked in such a confined area where all the corridors meet.", "/[waitkey(64)]/[close()]"};
        this.npc_d_TALK2 = new String[]{"/[label(Jr.)]", "...They might have been a bit overconfident.", "/[waitkey(64)]/[close()]"};
        this.npc_d_TALK3 = new String[]{"/[label(Jr.)]", "It looks like the aftermath of a surprise attack.", "/[waitkey(64)]/[close()]"};
        this.npc_d_TALK4 = new String[]{"/[label(Jr.)]", "I hope our troops haven't taken any losses.", "/[waitkey(64)]/[close()]"};
        this.npc_d_TALK5 = new String[]{"/[label(Jr.)]", "It looks like there was a battle here. U-TIC Organization soldiers must have been laying in wait.", "/[waitkey(64)]/[close()]"};
        this.npc_d_TALK6 = new String[]{"/[label(Jr.)]", "The wounded soldiers in the back probably got hit over here.", "/[waitkey(64)]/[close()]"};
        this.npc_d_TALK7 = new String[]{"...", "/[waitkey(64)]/[close()]"};
        this.Q_CARDKEY2 = new String[]{"Operate Door No. 3 switch?", "/[waitkey(64)]/[close()]"};
    }

    float Abs(float f, float f2) {
        float f3 = f - f2;
        if (f3 >= 0.0f) {
            return f3;
        }
        return -f3;
    }

    void BlueLight() {
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
    }

    void Dead_Event() {
        this.Oto = new Mapunits();
        this.Oto.mapUnit(6);
        this.Oto.start(1, "Teiten_Move1");
        Runtime.enable(65536);
        Runtime.disable(524288);
        this.EV_Camera01_F();
        int n = 0;
        while (n <= 435) {
            if (n == 10) {
                this.EV_Camera01();
                this.player.rotY(80, 0.0f, true);
            }
            if (n == 120 && this.NO3_HINTO2 == 1) {
                this.DefaultTalk(this.npc_d_TALK1);
            } else if (n == 120 && this.NO3_HINTO3 == 1) {
                this.DefaultTalk(this.npc_d_TALK5);
            } else if (n == 120) {
                this.DefaultTalk(this.npc_d_TALK3);
            }
            if (n == 170) {
                this.player.mtn(14, 1, 1.0f, true);
            }
            if (n == 370 && this.NO3_HINTO2 == 1) {
                ST1050.waitPage(this.win, 64);
                this.DefaultTalk(this.npc_d_TALK2);
            } else if (n == 370 && this.NO3_HINTO3 == 1) {
                ST1050.waitPage(this.win, 64);
                this.DefaultTalk(this.npc_d_TALK6);
            } else if (n == 370) {
                ST1050.waitPage(this.win, 64);
                this.DefaultTalk(this.npc_d_TALK4);
            }
            System.sleep(1);
            ++n;
        }
        ST1050.waitPage(this.win, 64);
        Runtime.setFlags(6027, 1, 1);
        this.DEAD_TALK = Runtime.getFlags(6027, 1);
        this.cam0.setMode(0);
        this.Oto.start(1, "Teiten_Move2");
        Runtime.enable(524288);
        Runtime.disable(65536);
    }

    public void Dead_Talk(Enepc enepc) {
        System.println("Dead_Talk");
        Runtime.setPlayerControl(false);
        if (this.DEAD_TALK == 0 && this.NO3_CARDKEY == 0) {
            this.Dead_Event();
        } else {
            this.DefaultTalk(this.npc_d_TALK7);
            ST1050.waitPage(this.win, 64);
        }
        Runtime.setPlayerControl(true);
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
        System.println("EOB !!!!!!!!!!!!!!!!!!!!!");
        switch (n) {
            case 1: {
                System.println("enemy1 消滅");
                Runtime.setFlags(6018, 1, 1);
                this.HEISHI_LIFE1 = Runtime.getFlags(6018, 1);
                break;
            }
            case 2: {
                System.println("enemy2 消滅");
                Runtime.setFlags(6019, 1, 1);
                this.HEISHI_LIFE2 = Runtime.getFlags(6019, 1);
                break;
            }
            case 10: {
                System.println("enemy10 !!!!!!!!!!!!!!!!!!!!!");
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                this.enemy3.setVisible(false);
                this.enemy4.setVisible(false);
                this.Jr_Gun.setVisible(false);
                this.Jr_Gun_Effect.disp(false);
                this.Enemy_Start();
                this.enemy3.setTranslate(-1.0f, 0.0f, -6.0f);
                this.enemy3.setRotate(0.0f, 0.0f, 0.0f);
                this.enemy4.setTranslate(-2.0f, 0.0f, -5.0f);
                this.enemy4.setRotate(0.0f, 0.0f, 0.0f);
                if (this.KOW1 == 0) {
                    this.switch1.start(1, "Blink_1");
                    this.SOUND_FLAG = false;
                    this.SOUND_BLINK_FLAG = false;
                }
                if (this.KOW2 != 0) break;
                this.switch2.start(1, "Blink_2");
                this.SOUND_FLAG = false;
                this.SOUND_BLINK_FLAG = false;
                break;
            }
        }
    }

    void EOB_Always(int n) {
        System.println("EOB_Always !!!!!!!!!!!!!!!!!!!!!");
        if (this.KOW1 == 1) {
            this.BLINK_FLAG1 = false;
            this.Obj600.disp(false);
        }
        if (this.KOW2 == 1) {
            this.BLINK_FLAG2 = false;
            this.Obj601.disp(false);
        }
    }

    void EV_Camera() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.835972f, 2.4921246f, -5.983624f);
        this.camEV.setRotate(-12.726374f, 16.187437f, 0.0f);
        this.camEV.setFov(44.76137f);
        this.camEV.change();
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
        this.camEV.setRotate(15.662678f, 20.319937f, 0.0f);
        this.camEV.transSPL(fArray, 1, 1, 120);
        this.camEV.rotateSPL(fArray3, 1, 0, 120);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        System.println("イベント（回転）操作 カメラ");
        float[] fArray = new float[]{1.0f, -1.885102f, 2.7299542f, 10.86827f, 450.0f, -2.5209932f, 2.7299542f, -0.7232599f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -8.54682f;
        fArray2[2] = 3.1399436f;
        fArray2[4] = 450.0f;
        fArray2[5] = -8.54682f;
        fArray2[6] = 3.1399436f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 450);
        this.camEV.rotateSPL(fArray3, 1, 2, 450);
        this.camEV.setFov(27.5f);
        this.camEV.change();
    }

    void EV_Camera01_F() {
        System.println("イベント（回転）操作 カメラ フォロー");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.885102f, 2.7299542f, 10.86827f);
        this.camEV.setRotate(-8.54682f, 3.1399436f, 0.0f);
        this.camEV.setFov(27.5f);
        this.camEV.change();
    }

    void EV_Camera10() {
        System.println("コンソール　押す前カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-10.174998f, 2.4368122f, -11.156108f);
        this.camEV.setRotate(-14.715947f, 312.49435f, 0.0f);
        this.camEV.setFov(40.708572f);
        this.camEV.change();
    }

    void EV_Camera11() {
        System.println("トラップコード(左）B");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(24.192522f, 3.2039192f, -1.7521007f);
        this.camEV.setRotate(-13.706746f, 78.83675f, 0.0f);
        this.camEV.setFov(27.519808f);
        this.camEV.change();
    }

    void EV_Camera12() {
        System.println("トラップコード(右）B");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.08186f, 7.711888f, 7.822129f);
        this.camEV.setRotate(-23.631447f, -54.659615f, 0.0f);
        this.camEV.setFov(19.20001f);
        this.camEV.change();
    }

    void EV_Camera2() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(27.483055f, 5.6319485f, 8.330599f);
        this.camEV.setRotate(-54.92433f, 302.92032f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera3() {
        System.println("NO3カードキー差込口カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-12.157209f, 2.584224f, -11.997588f);
        this.camEV.setRotate(-23.479279f, -58.174755f, 0.0f);
        this.camEV.setFov(44.360363f);
        this.camEV.change();
    }

    void EV_Camera4() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-12.229435f, 2.107851f, -12.264319f);
        this.camEV.setRotate(-11.507347f, 300.60345f, 0.0f);
        this.camEV.setFov(30.559258f);
        this.camEV.change();
    }

    void EV_Camera5() {
        System.println("NO3コンソール前カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-11.100876f, 3.620716f, -11.653109f);
        this.camEV.setRotate(-29.487503f, 305.3761f, 0.0f);
        this.camEV.setFov(27.038849f);
        this.camEV.change();
    }

    void EV_Camera6() {
        System.println("NO3扉前カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.2562631f, 5.7058225f, -11.746665f);
        this.camEV.setRotate(-41.826283f, 52.338604f, 0.0f);
        this.camEV.setFov(35.199493f);
        this.camEV.change();
    }

    void EV_Camera7() {
        System.println("トラップコード(左）A");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.732958f, 4.735944f, -1.9614477f);
        this.camEV.setRotate(-42.763855f, 324.45523f, 0.0f);
        this.camEV.setFov(29.119978f);
        this.camEV.change();
    }

    void EV_Camera8() {
        System.println("トラップコード（右）A");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(25.248053f, 4.8958697f, -2.3457315f);
        this.camEV.setRotate(-49.291065f, 320.07452f, 0.0f);
        this.camEV.setFov(30.399746f);
        this.camEV.change();
    }

    void EV_Camera9() {
        System.println("挟み撃ちする兵士　カメラ");
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(20.48009f, 1.8859617f, 13.649689f);
        this.camEV.setRotate(-17.388416f, 446.88956f, 0.0f);
        this.camEV.setFov(32.31983f);
        this.camEV.change();
    }

    void Enemy_Start() {
        System.println("ENEMY_START!!!");
        if (this.NO3_CARDKEY == 1) {
            this.enemy7.setVisible(true);
            this.enemy8.setVisible(true);
            this.enemy7.kickEnepc(4, 0);
            this.enemy8.kickEnepc(4, 0);
        } else {
            if (this.HEISHI_LIFE1 == 0) {
                this.enemy1.setVisible(true);
                this.enemy1.kickEnepc(4, 0);
            }
            if (this.HEISHI_LIFE2 == 0) {
                this.enemy2.setVisible(true);
                this.enemy2.kickEnepc(4, 0);
            }
        }
    }

    void Enemy_Stop() {
        System.println("ENEMY_STOP!!!");
        if (this.NO3_CARDKEY == 1) {
            this.enemy7.setVisible(false);
            this.enemy8.setVisible(false);
            this.enemy7.kickEnepc(4, 2);
            this.enemy8.kickEnepc(4, 2);
        } else {
            if (this.HEISHI_LIFE1 == 0) {
                this.enemy1.setVisible(false);
                this.enemy1.kickEnepc(4, 2);
            }
            if (this.HEISHI_LIFE2 == 0) {
                this.enemy2.setVisible(false);
                this.enemy2.kickEnepc(4, 2);
            }
        }
    }

    void Final_init(int n) {
        System.println("FINAL INIT !!!!!!!!!!!!!!!!!!!!!!!!!");
        switch (n) {
            case 3: {
                System.println("case 3");
                this.enemy3.kickEnepc(4, 1);
                break;
            }
            case 4: {
                System.println("case 4");
                this.enemy4.kickEnepc(4, 1);
                break;
            }
            case 11: {
                System.println("case 11");
                this.npc1.kickEnepc(9, 13);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        switch (n) {
            case 100: {
                if (n2 == 0 && this.NO3 != 1 && !this.EnterCheck) {
                    System.println("NO3 扉前");
                    if (this.DEAD_TALK == 0 && this.NO3_CARDKEY == 0) {
                        this.Dead_Event();
                        break;
                    }
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.EV_Camera6();
                    if (this.NO3_HINTO1 == 0) {
                        this.DefaultTalk(this.NO3_TALK_A);
                    } else {
                        this.DefaultTalk(this.NO3_TALK_B);
                    }
                    this.cam0.setMode(0);
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 1 && this.NO3_CARDKEY != 1) {
                    if (this.EnterCheck) break;
                    System.println("NO3 カードリーダー前");
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.EV_Camera3();
                    this.RedLight2();
                    this.DefaultTalk(this.CardReader2);
                    this.EnterCheck = false;
                    this.cam0.setMode(0);
                    this.DefaultLight();
                    Runtime.setPlayerControl(true);
                    Runtime.disable(65536);
                    break;
                }
                if (n2 == 1 && this.NO3_CARDKEY == 1) {
                    if (this.EnterCheck || this.CONSOLE == 1) break;
                    System.println("NO3 コンソール前");
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    this.EV_Camera3();
                    Runtime.disable(524288);
                    this.RedLight();
                    Runtime.enable(65536);
                    if (this.DefaultMenu(this.Q_NO3_CARDKEY) == 0) {
                        this.player.getTranslate();
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        if (this.player.px >= -9.65f) {
                            System.println("パターンA");
                            this.player.setRotateY(200.0f);
                        } else if (this.player.px >= -9.73f && this.player.px < -9.65f) {
                            System.println("パターンB");
                            this.player.setRotateY(195.0f);
                        } else if (this.player.px >= -9.9f && this.player.px < -9.73f) {
                            System.println("パターンC");
                            this.player.setRotateY(180.0f);
                        } else if (this.player.px >= -9.95f && this.player.px < -9.9f) {
                            System.println("パターンD");
                            this.player.setRotateY(175.0f);
                        } else if (this.player.px < -9.95f) {
                            System.println("パターンE");
                            this.player.setRotateY(165.0f);
                        }
                        this.player.mtn(25, 1, 1.0f, true);
                        int n3 = 0;
                        while (n3 < 50) {
                            if (n3 == 30) {
                                this.Obj603_A.disp(false);
                                this.Obj603_B.disp(true);
                                Sound.effectPlay(196741);
                            }
                            System.sleep(1);
                            ++n3;
                        }
                        this.EV_Camera4();
                        System.sleep(30);
                        Sound.effectPlay(196741);
                        Stage.setVisible(22, true);
                        System.sleep(2);
                        Sound.effectPlay(196741);
                        this.Obj605_A.disp(false);
                        this.Obj605_B.disp(true);
                        this.BlueLight();
                        System.sleep(48);
                        Runtime.setFlags(6014, 1, 1);
                        this.CONSOLE = Runtime.getFlags(6014, 1);
                    }
                    this.cam0.setMode(0);
                    Runtime.enable(524288);
                    this.DefaultLight();
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 2 && this.CONSOLE != 1 && !this.EnterCheck) {
                    System.println("コンソールを立ち上げないと…");
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.EV_Camera10();
                    this.RedLight();
                    this.DefaultTalk(this.Console1);
                    this.cam0.setMode(0);
                    this.DefaultLight();
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 2 && this.CONSOLE == 1) {
                    if (this.EnterCheck || this.NO3 == 1) break;
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    this.EV_Camera5();
                    Runtime.disable(524288);
                    this.BlueLight();
                    Runtime.enable(65536);
                    if (this.DefaultMenu(this.Q_CARDKEY2) == 0) {
                        this.player.getTranslate();
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        if (this.player.px >= -7.8f) {
                            System.println("パターンA");
                            this.player.setRotateY(215.0f);
                        } else if (this.player.px >= -7.955f && this.player.px < -7.8f) {
                            System.println("パターンB");
                            this.player.setRotateY(205.0f);
                        } else if (this.player.px >= -8.0f && this.player.px < -7.955f) {
                            System.println("パターンC");
                            this.player.setRotateY(195.0f);
                        } else if (this.player.px >= -8.15f && this.player.px < -8.0f) {
                            System.println("パターンD");
                            this.player.setRotateY(190.0f);
                        } else if (this.player.px < -8.15f) {
                            System.println("パターンE");
                            this.player.setRotateY(180.0f);
                        }
                        this.player.mtn(25, 1, 1.0f, true);
                        int n4 = 0;
                        while (n4 < 60) {
                            if (n4 == 30) {
                                this.Obj604_A.disp(false);
                                this.Obj604_B.disp(true);
                                Sound.effectPlay(196741);
                            } else if (n4 == 35) {
                                this.Obj605_B.disp(false);
                                this.Obj605_C.disp(true);
                            }
                            System.sleep(1);
                            ++n4;
                        }
                        this.EV_Camera();
                        this.DefaultLight();
                        this.doorB.DoorOpen();
                        Sound.effectPlay(196744);
                        int n5 = 0;
                        while (n5 <= 100) {
                            if (n5 == 49) {
                                this.npc_d1.kickEnepc(4, 1);
                                this.npc_d1.setTranslate(-3.75f, 0.0f, -15.2f);
                                this.npc_d1.kickEnepc(0, 4);
                            }
                            System.sleep(1);
                            ++n5;
                        }
                        Runtime.setFlags(6010, 1, 1);
                        this.NO3 = Runtime.getFlags(6010, 1);
                        this.cam0.setMode(0);
                        this.npc_d1.setInvalidID(0);
                        this.npc_d1.kickEnepc(4, 0);
                        this.npc_d1.setMotion(0, 4);
                    }
                    this.cam0.setMode(0);
                    this.DefaultLight();
                    Runtime.enable(524288);
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 3 && this.SWITCH != 1 && !this.EnterCheck) {
                    this.Enemy_Stop();
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.EV_Camera2();
                    if (this.ROOM4_CARDKEY == 0) {
                        this.DefaultTalk(this.CardReader2);
                    } else if (this.DefaultMenu(this.Q_ROOM4_CARDKEY) == 0) {
                        this.player.getTranslate();
                        int n6 = 0;
                        if (this.Abs(this.player.pz, 5.85f) < 0.05f) {
                            System.println("pattern A");
                            int n7 = 0;
                            while (n7 < 70) {
                                if (n7 == 0) {
                                    this.player.rotY(5, 90.0f, true);
                                } else if (n7 == 7) {
                                    this.player.mtn(16, 1, 1.0f, true);
                                } else if (n7 == 45) {
                                    this.Obj602_A.disp(false);
                                    this.Obj602_B.disp(true);
                                    Sound.effectPlay(196741);
                                }
                                System.sleep(1);
                                ++n7;
                            }
                            this.door_4.DoorOpen();
                            Sound.effectPlay(196744);
                            System.sleep(90);
                            Runtime.setFlags(6015, 1, 1);
                            this.SWITCH = Runtime.getFlags(6015, 1);
                            this.Enemy_Start();
                            this.cam0.setMode(0);
                            Runtime.disable(65536);
                            this.EnterCheck = false;
                            Runtime.setPlayerControl(true);
                            this.door_4.SetDoorType('\u0004');
                            return;
                        }
                        if (this.Abs(this.player.pz, 5.85f) < 0.2f) {
                            System.println("pattern B");
                            n6 = 5;
                        } else if (this.Abs(this.player.pz, 5.85f) < 0.4f) {
                            System.println("pattern C");
                            n6 = 8;
                        } else if (this.Abs(this.player.pz, 5.85f) < 0.6f) {
                            System.println("pattern D");
                            n6 = 12;
                        } else if (this.Abs(this.player.pz, 5.85f) < 0.8f) {
                            System.println("pattern E");
                            n6 = 16;
                        } else if (this.Abs(this.player.pz, 5.85f) < 1.0f) {
                            System.println("pattern F");
                            n6 = 20;
                        } else if (this.Abs(this.player.pz, 5.85f) < 1.5f) {
                            System.println("pattern G");
                            n6 = 25;
                        }
                        int n8 = 0;
                        while (n8 < 70 + n6) {
                            if (n8 == 0) {
                                this.player.move(n6, 29.15f, 5.85f, true);
                                this.player.mtn(2, 0, n6, n6, 8, 1.0f, true);
                            } else if (n8 == 5 + n6) {
                                this.player.rotY(5, 90.0f, true);
                            } else if (n8 == 12 + n6) {
                                this.player.mtn(16, 1, 1.0f, true);
                            } else if (n8 == 45 + n6) {
                                this.Obj602_A.disp(false);
                                this.Obj602_B.disp(true);
                                Sound.effectPlay(196741);
                            }
                            System.sleep(1);
                            ++n8;
                        }
                        this.door_4.DoorOpen();
                        Sound.effectPlay(196744);
                        System.sleep(90);
                        Runtime.setFlags(6015, 1, 1);
                        this.SWITCH = Runtime.getFlags(6015, 1);
                        this.door_4.SetDoorType('\u0004');
                    }
                    this.Enemy_Start();
                    this.cam0.setMode(0);
                    Runtime.disable(65536);
                    this.EnterCheck = false;
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 4 && this.SWITCH != 1 && !this.EnterCheck) {
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.Enemy_Stop();
                    this.EV_Camera2();
                    if (this.ROOM4_CARDKEY == 0) {
                        this.DefaultTalk(this.CardReader3);
                    } else {
                        this.DefaultTalk(this.CardReader4);
                    }
                    this.Enemy_Start();
                    this.cam0.setMode(0);
                    this.EnterCheck = false;
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 5 && this.HEISHI == 0) {
                    if (this.NO3_CARDKEY == 1) {
                        return;
                    }
                    System.println("陰に隠れた兵士イベント");
                    this.EV_Camera9();
                    this.enemy1.kickEnepc(4, 1);
                    this.enemy2.kickEnepc(4, 1);
                    this.enemy1.kickEnepc(0, 7);
                    this.enemy2.kickEnepc(0, 5);
                    System.sleep(25);
                    this.enemy1.setRotate(0.0f, 90.0f, 0.0f);
                    this.enemy2.setRotate(0.0f, 270.0f, 0.0f);
                    this.enemy1.kickEnepc(1, 3);
                    this.enemy1.moveEnepc(15, 14.25f, 13.5f, 10);
                    this.enemy2.kickEnepc(1, 3);
                    this.enemy2.moveEnepc(15, 16.75f, 13.5f, 10);
                    System.sleep(10);
                    this.enemy1.setRotate(0.0f, 0.0f, 0.0f);
                    this.enemy2.setRotate(0.0f, 0.0f, 0.0f);
                    this.enemy1.kickEnepc(4, 0);
                    this.enemy2.kickEnepc(4, 0);
                    this.enemy1.kickEnepc(7, 3);
                    System.println("超足警備員初期化");
                    this.enemy2.kickEnepc(7, 2);
                    System.println("俊足警備員初期化");
                    this.enemy1.dispRadar(true);
                    this.enemy2.dispRadar(true);
                    this.cam0.setMode(0);
                    Runtime.setFlags(6016, 1, 1);
                    this.HEISHI = Runtime.getFlags(6016, 1);
                    break;
                }
                if (n2 == 6 && this.KOW1 != 1) {
                    System.println("トラップコード１");
                    this.Enemy_Stop();
                    this.SOUND_NUMBER = false;
                    this.player.getTranslate();
                    Runtime.setPlayerControl(false);
                    if (this.TRAP_FIRST == 0) {
                        System.println("トラップコード1A");
                        this.EV_Camera7();
                        this.enemy3.setTranslate(14.0f, 0.0f, -3.0f);
                        this.enemy3.setRotate(0.0f, 135.0f, 0.0f);
                        this.enemy4.setTranslate(18.0f, 0.0f, -3.0f);
                        this.enemy4.setRotate(0.0f, 225.0f, 0.0f);
                        System.sleep(10);
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.sound_blink.start(1, "Sound_Blink");
                        System.sleep(30);
                        this.player.mtn(6, 1, 1.0f, true);
                        System.sleep(80);
                        this.enemy3.setVisible(true);
                        this.enemy4.setVisible(true);
                        this.enemy3.kickEnepc(1, 3);
                        this.enemy3.moveEnepc(15, this.player.px - 0.5f, this.player.pz + 0.5f, 13);
                        this.enemy4.kickEnepc(1, 3);
                        this.enemy4.moveEnepc(15, this.player.px + 0.5f, this.player.pz + 0.5f, 13);
                        System.sleep(12);
                        Runtime.setFlags(6032, 1, 1);
                        this.TRAP_FIRST = Runtime.getFlags(6032, 1);
                    } else {
                        System.println("トラップコード1B");
                        boolean bl = false;
                        boolean bl2 = false;
                        boolean bl3 = true;
                        this.player.getTranslate();
                        this.Jr_Gun_Effect.setTranslate(this.player.px - 0.5f, 1.75f, this.player.pz - 0.725f);
                        this.EV_Camera7();
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.enemy3.setTranslate(-1.0f, 0.0f, -5.0f);
                        this.enemy3.setRotate(0.0f, 0.0f, 0.0f);
                        this.enemy3.setVisible(true);
                        this.enemy4.setTranslate(-2.0f, 0.0f, -4.0f);
                        this.enemy4.setRotate(0.0f, 0.0f, 0.0f);
                        this.enemy4.setVisible(true);
                        int n9 = 0;
                        while (n9 <= 113) {
                            if (n9 == 0) {
                                this.sound_blink.start(1, "Sound_Blink");
                            } else if (n9 == 55) {
                                this.player.mtn(6, 1, 1.0f, true);
                            } else if (n9 == 58) {
                                this.EV_Camera11();
                                this.enemy3.kickEnepc(1, 3);
                                this.enemy3.moveEnepc(15, -1.0f, -1.0f, 8);
                                this.enemy4.kickEnepc(1, 3);
                                this.enemy4.moveEnepc(15, -2.0f, -2.5f, 9);
                            } else if (n9 == 68) {
                                this.enemy3.kickEnepc(0, 0);
                                this.enemy4.kickEnepc(0, 0);
                            } else if (n9 == 71) {
                                this.enemy3.moveEnepc(17, 95.0f, 1.0f, 8);
                                this.enemy4.moveEnepc(17, 100.0f, 1.0f, 6);
                            } else if (n9 == 80) {
                                this.enemy3.kickEnepc(1, 3);
                                this.enemy3.moveEnepc(15, this.player.px, this.player.pz, 40);
                                this.enemy4.kickEnepc(1, 3);
                                this.enemy4.moveEnepc(15, this.player.px, this.player.pz, 43);
                            } else if (n9 == 85) {
                                this.player.rotY(7, 280.0f, true);
                            } else if (n9 == 95) {
                                this.Jr_Gun.setVisible(true);
                                this.player.mtn(15, 1, 1.0f, true);
                            } else if (n9 == 103) {
                                Sound.effectPlay(196746);
                            } else if (n9 == 107) {
                                this.Jr_Gun_Effect.disp(true);
                            }
                            System.sleep(1);
                            ++n9;
                        }
                    }
                    this.EnterCheck = false;
                    this.FIN_FLAG = true;
                    Sound.effectStop(196745);
                    Runtime.setDefocus(0, 1, this.DefaultScreenParam);
                    this.enemy10.kickEnepc(14, 10, 0);
                    break;
                }
                if (n2 == 6 && this.KOW1 == 1) {
                    this.door_2.SetDoorType('\u0004');
                    break;
                }
                if (n2 == 7 && this.KOW2 != 1) {
                    System.println("トラップコード2");
                    this.Enemy_Stop();
                    this.SOUND_NUMBER = true;
                    this.player.getTranslate();
                    Runtime.setPlayerControl(false);
                    if (this.TRAP_FIRST == 0) {
                        System.println("トラップコード2A");
                        System.sleep(2);
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.EV_Camera8();
                        this.enemy3.setTranslate(25.0f, 0.0f, -3.0f);
                        this.enemy3.setRotate(0.0f, 135.0f, 0.0f);
                        this.enemy4.setTranslate(29.0f, 0.0f, -3.0f);
                        this.enemy4.setRotate(0.0f, 225.0f, 0.0f);
                        System.sleep(10);
                        Runtime.enable(65536);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.sound_blink.start(1, "Sound_Blink");
                        System.sleep(30);
                        this.player.mtn(6, 1, 1.0f, true);
                        System.sleep(80);
                        this.enemy3.setVisible(true);
                        this.enemy4.setVisible(true);
                        this.enemy3.kickEnepc(4, 1);
                        this.enemy4.kickEnepc(4, 1);
                        this.enemy3.kickEnepc(1, 3);
                        this.enemy3.kickEnepc(1, 3);
                        this.enemy3.moveEnepc(15, this.player.px, this.player.pz, 18);
                        this.enemy4.kickEnepc(1, 3);
                        this.enemy4.moveEnepc(15, this.player.px, this.player.pz, 17);
                        System.sleep(12);
                        Runtime.setFlags(6033, 1, 1);
                        this.TRAP_FIRST = Runtime.getFlags(6033, 1);
                    } else {
                        System.println("トラップコード2B");
                        this.enemy3.setTranslate(18.0f, 0.0f, -2.0f);
                        this.enemy3.setRotate(0.0f, 95.0f, 0.0f);
                        this.enemy3.setVisible(true);
                        this.enemy4.setTranslate(28.0f, 0.0f, 4.0f);
                        this.enemy4.setRotate(0.0f, 190.0f, 0.0f);
                        this.enemy4.setVisible(true);
                        System.sleep(2);
                        Runtime.enable(65536);
                        boolean bl = false;
                        boolean bl4 = false;
                        boolean bl5 = true;
                        int n10 = 0;
                        while (n10 <= 120) {
                            if (n10 == 0) {
                                this.EV_Camera8();
                                this.sound_blink.start(1, "Sound_Blink");
                            } else if (n10 == 15) {
                                this.player.mtn(6, 1, 1.0f, true);
                            } else if (n10 == 90) {
                                this.EV_Camera12();
                                this.player.rotY(10, 0.0f, true);
                            } else if (n10 == 92) {
                                this.enemy3.kickEnepc(1, 3);
                                this.enemy3.moveEnepc(15, this.player.px, this.player.pz, 35);
                                this.enemy4.kickEnepc(1, 3);
                                this.enemy4.moveEnepc(15, this.player.px, this.player.pz, 37);
                            } else if (n10 == 102) {
                                this.player.mtn(27, 1, 1.0f, true);
                            }
                            System.sleep(1);
                            ++n10;
                        }
                    }
                    this.EnterCheck = false;
                    this.FIN_FLAG = true;
                    Sound.effectStop(196745);
                    Runtime.setDefocus(0, 1, this.DefaultScreenParam);
                    this.enemy10.kickEnepc(14, 10, 0);
                    break;
                }
                if (n2 == 7 && this.KOW2 == 1) {
                    this.door_3.SetDoorType('\u0004');
                    break;
                }
                if (n2 == 8 && !this.EnterCheck && this.NO5 == 0) {
                    System.println("NO5 扉前");
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    this.DefaultTalk(this.NO5_TALK);
                    this.EnterCheck = false;
                    Runtime.setPlayerControl(true);
                    break;
                }
                if (n2 == 9) {
                    if (this.NO3_CARDKEY == 0) {
                        this.ManualClip_A(true);
                        break;
                    }
                    this.ManualClip_B(true);
                    break;
                }
                if (n2 == 10) {
                    if (this.NO3_CARDKEY == 0) {
                        this.ManualClip_A(false);
                        break;
                    }
                    this.ManualClip_B(true);
                    break;
                }
                return;
            }
        }
    }

    void ManualClip_A(boolean bl) {
        this.npc1.setVisible(bl);
        this.npc2.setVisible(bl);
        this.npc3.setVisible(bl);
        this.npc_d1.setVisible(bl);
        this.npc_d2.setVisible(bl);
        this.npc_d3.setVisible(bl);
        this.Blood_A.disp(bl);
        this.Blood_B.disp(bl);
        this.Blood_C.disp(bl);
        this.Blood_D.disp(bl);
    }

    void ManualClip_B(boolean bl) {
        this.npc_d1.setVisible(bl);
        this.npc_d2.setVisible(bl);
        this.npc_d3.setVisible(bl);
        this.Blood_A.disp(bl);
        this.Blood_B.disp(bl);
        this.Blood_C.disp(bl);
        this.Blood_D.disp(bl);
    }

    public void Npc_Talk1A(Enepc enepc, Window window) {
        System.println("Npc_Talk1");
        this.npc1.kickEnepc(9, 100);
        Runtime.setPlayerControl(false);
        if (this.TalkFlag1A == 0) {
            window.print(this.NPC1_TALK1, 0);
            ST1050.waitPage(window, 64);
            ++this.TalkFlag1A;
            Runtime.setFlags(6050, 1, 1);
            this.NO3_HINTO3 = Runtime.getFlags(6050, 1);
        } else if (this.TalkFlag1A == 1) {
            window.print(this.NPC1_TALK2, 0);
            ST1050.waitPage(window, 64);
            ++this.TalkFlag1A;
        } else {
            window.print(this.NPC1_TALK3, 0);
            ST1050.waitPage(window, 64);
        }
        this.npc1.kickEnepc(9, 13);
        Runtime.setPlayerControl(true);
    }

    public void Npc_Talk2(Enepc enepc, Window window) {
        System.println("Npc_Talk2");
        Runtime.setPlayerControl(false);
        if (this.TalkFlag2 == 0) {
            window.print(this.NPC2_TALK1, 0);
            ST1050.waitPage(window, 64);
            ++this.TalkFlag2;
            Runtime.setFlags(6049, 1, 1);
            this.NO3_HINTO2 = Runtime.getFlags(6049, 1);
        } else if (this.TalkFlag2 == 1) {
            window.print(this.NPC2_TALK2, 0);
            ST1050.waitPage(window, 64);
            ++this.TalkFlag2;
        } else {
            window.print(this.NPC2_TALK3, 0);
            ST1050.waitPage(window, 64);
        }
        Runtime.setPlayerControl(true);
    }

    public void Npc_Talk3(Enepc enepc, Window window) {
        System.println("Npc_Talk3");
        Runtime.setPlayerControl(false);
        if (this.KIZETU == 1) {
            window.print(this.NPC3_TALK5, 0);
            ST1050.waitPage(window, 64);
        } else if (this.TalkFlag3 == 0) {
            this.npc3.kickEnepc(9, 100);
            window.print(this.NPC3_TALK1, 0);
            ST1050.waitPage(window, 64);
            ++this.TalkFlag3;
            Runtime.setFlags(6050, 1, 1);
            this.NO3_HINTO3 = Runtime.getFlags(6050, 1);
        } else if (this.TalkFlag3 == 1) {
            this.npc3.kickEnepc(9, 100);
            window.print(this.NPC3_TALK2, 0);
            ST1050.waitPage(window, 64);
            ++this.TalkFlag3;
        } else if (this.TalkFlag3 <= 3) {
            this.npc3.kickEnepc(9, 100);
            window.print(this.NPC3_TALK3, 0);
            ST1050.waitPage(window, 64);
            ++this.TalkFlag3;
        } else {
            Runtime.setFlags(6020, 1, 1);
            this.KIZETU = Runtime.getFlags(6020, 1);
            window.print(this.NPC3_TALK4, 0);
            ST1050.waitPage(window, 64);
        }
        this.npc3.kickEnepc(9, -1);
        Runtime.setPlayerControl(true);
    }

    void RedLight() {
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.9f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.5f, -1.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -0.25f, -1.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
    }

    void RedLight2() {
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.9f, 0.3f, 0.3f);
        this.light.setDirection2(2, 1.5f, 1.5f, -1.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.3f, 0.5f, 0.3f);
        this.light.setDirection2(3, 0.0f, -0.25f, -1.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                this.teiten1.setTranslate(0.0f, -1000.0f, 0.0f);
                Runtime.setFlags(6012, 1, 1);
                this.KOW1 = Runtime.getFlags(6012, 1);
                System.println("電気系ショート1（エフェクト）");
                this.Obj606.disp(true);
                this.destroy1.start(1, "Bang_1");
                break;
            }
            case 2: {
                this.teiten2.setTranslate(0.0f, -1000.0f, 10.0f);
                Runtime.setFlags(6013, 1, 1);
                this.KOW2 = Runtime.getFlags(6013, 1);
                System.println("電気系ショート2（エフェクト）");
                this.Obj607.disp(true);
                this.destroy2.start(1, "Bang_2");
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setFlags(6033, 1, 0);
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1040, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(1060, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(1060, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(1080, 1);
                break;
            }
            case 4: {
                Runtime.jumpCF(1090, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(1110, 1);
                break;
            }
            case 6: {
                Runtime.jumpCF(1030, 1);
                break;
            }
            case 7: {
                Runtime.jumpCF(1100, 1);
                break;
            }
        }
    }

    void init() {
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
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.375f, 0.375f, 0.375f);
        this.light.setColor(1, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.375f, 0.375f, 0.375f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightCol(1, 1, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightCol(1, 2, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightCol(1, 3, 0.215f, 0.215f, 0.215f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 3, 0.25f, 0.25f, 0.25f);
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
        Runtime.setIdLightCol(4, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(4, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(4, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(4, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(5, 3, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(6, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(6, 1, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(6, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(6, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(6, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(6, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(6, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 6.25f, 40.0f);
        this.cam0.setCFHokan(2, 0.025f, 0.025f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.8f, 40.0f);
        this.cam0.setCFHokan(3, 0.03f, 0.03f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 5.25f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 5.25f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 5.25f, 40.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.015f, 0.015f);
        this.cam0.setCFPedestal(10, -9.5491705f, 6.9488134f, -10.363721f, 58.078888f, -74.148026f, 352.7398f, 0.0f, 2.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.02f, 0.02f);
        this.cam0.setCFPedestal(12, 18.632877f, 9.503831f, 19.64083f, 51.51986f, -39.944786f, 375.10858f, 0.0f, 2.0f);
        this.cam0.setCFHokan(12, 100.0f, 100.0f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 5.8f, 40.0f);
        this.cam0.setCFHokan(13, 0.02f, 0.02f);
        System.println("doorA の初期化");
        this.doorA = new Uwamono(13, 42, '\u0001');
        new Uwamono(12, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0002');
        this.doorA.SetDoorRange(2.0f);
        this.doorA.SetDoorSpd(0);
        this.doorA.DoorOpen();
        if (this.NO4 == 1) {
            this.doorA.DoorOpen();
        }
        System.println("doorB の初期化");
        this.doorB = new Uwamono(49, 42, '\u0001');
        new Uwamono(48, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0002');
        this.doorB.SetDoorRange(2.0f);
        if (this.NO3 != 1) {
            this.doorB.SetDoorSpd(85);
        } else {
            this.doorB.DoorOpen();
        }
        System.println("doorC の初期化");
        this.doorC = new Uwamono(56, 42, '\u0001');
        new Uwamono(55, 42, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        this.doorC.SetDoorSpd(39);
        System.println("doorD の初期化");
        this.doorD = new Uwamono(53, 42, '\u0001');
        new Uwamono(54, 42, '\u0001', this.doorD);
        this.doorD.SetDoorType('\u0002');
        this.doorD.SetDoorRange(2.0f);
        if (this.NO5 == 1) {
            this.doorD.DoorOpen();
        }
        this.door_0 = new Uwamono(71, 40, '\u0001');
        this.door_0.SetDoorType('\u0004');
        this.door_1 = new Uwamono(72, 40, '\u0001');
        this.door_1.SetDoorType('\u0004');
        this.door_2 = new Uwamono(70, 40, '\u0001');
        this.door_3 = new Uwamono(69, 40, '\u0001');
        this.door_4 = new Uwamono(50, 40, '\u0001');
        this.Obj602_B = new Effect(1542, 2);
        this.Obj602_B.setScale(0.196f, 0.115f, 1.0f);
        if (this.SWITCH == 0) {
            this.door_4.SetDoorType('\u0002');
            System.println("部屋４のカードリーダー（赤点灯）初期化");
            this.Obj602_A = new Effect(1539, 2);
            this.Obj602_A.setScale(0.196f, 0.115f, 1.0f);
            this.Obj602_A.disp(true);
            this.Obj602_B.disp(false);
        } else {
            this.Obj602_B.disp(true);
            this.door_4.SetDoorType('\u0004');
        }
        this.door_5 = new Uwamono(67, 40, '\u0001');
        this.door_5.SetDoorType('\u0004');
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 132);
        this.item3 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 133);
        this.item5 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 134);
        this.item7 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 135);
        this.item9 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 136);
        new Uwamono(127, 23, this.item1);
        new Uwamono(128, 23);
        new Uwamono(130, 45, this.item3);
        new Uwamono(131, 45);
        new Uwamono(132, 45, this.item5);
        new Uwamono(31, 20, this.item7);
        new Uwamono(129, 23, this.item9);
        if (this.NO3_CARDKEY == 0) {
            System.println("NPC1");
            this.npc1 = new NPC_NORMAL(1025, 11, 0, 8, 11, -0.775f, 0.0f, 1.95f, 172.5f);
            this.npc1.talkto("Npc_Talk1A");
            System.println("FINAL INIT !!!!!!!!!!!!!!!!!!!!!!!!!");
            System.println("NPC2");
            this.npc2 = new NPC_NORMAL(1026, 12, 0, 8, 13, -0.325f, 0.0f, 0.45f, 0.0f);
            this.npc2.talkto("Npc_Talk2");
            this.npc2.enableDTKFlag(4);
            this.npc2.disableDTKFlag(8);
            this.npc2.disableDTKFlag(131072);
            this.npc2.disableDTKFlag(1);
            this.npc2.disableDTKFlag(2);
            this.npc2.setMotion(0, 5);
            System.println("NPC3");
            this.npc3 = new NPC_NORMAL(1027, 13, 0, 7, 12, -0.325f, 0.0f, 0.95f, 270.0f);
            this.npc3.talkto("Npc_Talk3");
            this.npc3.disableDTKFlag(8);
            this.npc3.disableDTKFlag(131072);
            this.npc3.disableDTKFlag(1);
            this.npc3.disableDTKFlag(2);
            this.npc3.setMotion(0, 6);
            this.col_4 = new Uwamono(28672, -0.325f, 0.0f, 0.95f, 270.0f);
            this.col_4.SetSize(0.325f, 1.0f, 1.5f);
        }
        this.Blood_D = new Effect(1404, -0.775f, 0.0f, 0.95f, 0.0f);
        this.npc_d1 = new NPC_NORMAL(778, 14, 0, 7, 12, -3.75f, 0.0f, -14.75f, 0.0f);
        this.npc_d1.talkto("Dead_Talk");
        this.npc_d1.disableDTKFlag(131072);
        this.npc_d1.disableDTKFlag(1);
        this.npc_d1.disableDTKFlag(2);
        this.npc_d1.dispRadar(false);
        this.col_1 = new Uwamono(28672, -3.75f, 0.0f, -14.75f, 0.0f);
        this.col_1.SetSize(0.5f, 1.0f, 1.5f);
        if (this.NO3 == 0) {
            this.npc_d1.setMotion(0, 5);
        } else {
            this.npc_d1.setTranslate(-3.75f, 0.0f, -15.2f);
            this.npc_d1.setMotion(0, 4);
            this.col_1.setTranslate(-3.75f, 0.0f, -15.2f);
            this.col_1.SetSize(0.5f, 1.0f, 1.5f);
        }
        this.Blood_A = new Effect(1404, -3.75f, 0.0f, -14.75f, 0.0f);
        this.npc_d2 = new NPC_NORMAL(778, 15, 0, 7, 12, -1.5f, 0.0f, -12.5f, -20.0f);
        this.npc_d2.talkto("Dead_Talk");
        this.npc_d2.disableDTKFlag(131072);
        this.npc_d2.disableDTKFlag(1);
        this.npc_d2.disableDTKFlag(2);
        this.npc_d2.setMotion(0, 7);
        this.npc_d2.dispRadar(false);
        this.col_2 = new Uwamono(28672, -1.4f, 0.0f, -12.5f, -10.0f);
        this.col_2.SetSize(0.7f, 1.0f, 1.5f);
        this.Blood_C = new Effect(1527, -1.5f, 0.0f, -12.5f, -20.0f);
        this.npc_d3 = new NPC_NORMAL(778, 16, 0, 7, 12, -4.55f, 0.0f, -11.0f, 25.0f);
        this.npc_d3.talkto("Dead_Talk");
        this.npc_d3.disableDTKFlag(131072);
        this.npc_d3.disableDTKFlag(1);
        this.npc_d3.disableDTKFlag(2);
        this.npc_d3.setMotion(0, 2);
        this.npc_d3.dispRadar(false);
        this.col_3 = new Uwamono(28672, -4.55f, 0.0f, -11.0f, 25.0f);
        this.col_3.SetSize(0.65f, 1.0f, 1.5f);
        this.Blood_B = new Effect(1528, -4.55f, 0.0f, -11.0f, 0.0f);
        if (this.NO3_CARDKEY == 1 && this.FROM1060 == 1) {
            System.println("手動でのクリッピング処理 B");
            this.ManualClip_B(false);
        } else if (this.FROM1060 == 1) {
            System.println("手動でのクリッピング処理 A");
            this.ManualClip_A(false);
        }
        this.teiten3 = new Uwamono(28690, 22.048f, -1000.0f, -4.652f, 0.0f);
        this.teiten3.SetBgm(196620);
        this.teiten4 = new Uwamono(28690, 29.459f, -1000.0f, 2.093f, 0.0f);
        this.teiten4.SetBgm(196620);
        System.println("エフェクト１(緑）を初期化");
        this.Obj600 = new Effect(1548, 0);
        this.Obj600.setScale(0.196f, 0.115f, 1.0f);
        System.println("エフェクト２(緑）を初期化");
        this.Obj601 = new Effect(1548, 1);
        this.Obj601.setScale(0.196f, 0.115f, 1.0f);
        System.println("電気系ショート1 初期化");
        this.Obj606 = new Effect(1432, 6);
        this.Obj606.setScale(0.35f, 0.35f, 0.35f);
        System.println("電気系ショート2 初期化");
        this.Obj607 = new Effect(1432, 7);
        this.Obj607.setScale(0.35f, 0.35f, 0.35f);
        if (this.KOW1 == 1) {
            System.println("壊れ物１は壊れています");
            System.println("エフェクト１（緑）不点灯");
            this.Obj600.disp(false);
            System.println("電気系ショート1点灯）");
            this.Obj606.disp(true);
            System.println("環境音・スパーク 小（左）");
            this.teiten3.setTranslate(22.048f, 0.0f, -4.652f);
            this.door_2.SetDoorType('\u0004');
            Stage.setVisible(125, false);
        } else {
            this.door_2.SetDoorType('\u0002');
            this.door_2.SetDoorSpd(20);
            System.println("壊れ物１を初期化");
            this.K1 = new Uwamono(125, 30);
            this.K1.SetCallNo(1);
            System.println("エフェクト１（緑）点灯");
            this.Obj600.disp(true);
            System.println("電気系ショート1不点灯");
            this.Obj606.disp(false);
            System.println("破壊エフェクト(爆発１）の初期化");
            this.switch1 = new Mapunits();
            this.switch1.mapUnit(6);
            this.switch1.start(1, "Blink_1");
            this.destroy1 = new Mapunits();
            this.destroy1.mapUnit(6);
            this.E_Move1 = new Effect(1432, 6);
            this.E_Move1.setScale(0.8f, 0.8f, 0.8f);
            this.E_Move1.disp(false);
            this.E_Bang1 = new Effect(1508, 0);
            this.E_Bang1.disp(false);
            System.println("環境音・ドアロック仕掛け（左）");
            this.teiten1 = new Uwamono(28690, 23.968f, 0.0f, -4.706f, 0.0f);
            this.teiten1.SetBgm(196619);
        }
        if (this.KOW2 == 1) {
            System.println("壊れ物２は壊れています");
            System.println("エフェクト2（緑）不点灯");
            this.Obj601.disp(false);
            System.println("電気系ショート2 点灯");
            this.Obj607.disp(true);
            System.println("環境音・スパーク 小（右）");
            this.teiten4.setTranslate(29.459f, 0.0f, 2.093f);
            this.door_3.SetDoorType('\u0004');
            Stage.setVisible(126, false);
        } else {
            this.door_3.SetDoorType('\u0002');
            this.door_3.SetDoorSpd(20);
            System.println("壊れ物２を初期化");
            this.K2 = new Uwamono(126, 20);
            this.K2.SetCallNo(2);
            System.println("エフェクト2（緑）点灯");
            this.Obj601.disp(true);
            System.println("電気系ショート2 不点灯");
            this.Obj607.disp(false);
            System.println("破壊エフェクト(爆発２）の初期化");
            this.switch2 = new Mapunits();
            this.switch2.mapUnit(6);
            this.switch2.start(1, "Blink_2");
            this.destroy2 = new Mapunits();
            this.destroy2.mapUnit(6);
            this.E_Move2 = new Effect(1432, 7);
            this.E_Move2.setScale(0.8f, 0.8f, 0.8f);
            this.E_Move2.disp(false);
            this.E_Bang2 = new Effect(1508, 1);
            this.E_Bang2.disp(false);
            System.println("環境音・ドアロック仕掛け（右）");
            this.teiten2 = new Uwamono(28690, 29.705f, 0.0f, 2.99f, 0.0f);
            this.teiten2.SetBgm(196619);
        }
        if (this.KOW1 == 0 || this.KOW2 == 0) {
            System.println("Jr_Gunの初期化");
            this.Jr_Gun_Effect = new Effect(1702, 0.0f, 0.0f, 0.0f, 0.0f);
            this.Jr_Gun_Effect.setRotate(0.0f, 270.0f, 0.0f);
            this.Jr_Gun_Effect.disp(false);
            this.Jr_Gun = new Unit();
            this.Jr_Gun.init(24644, 0.0f, 0.0f, 0.0f, 0.0f);
            this.Jr_Gun.start(4, null);
            this.Jr_Gun.setParent(this.player, 72);
            this.Jr_Gun.setVisible(false);
            System.println("トラップコード 兵士の初期化");
            this.screen = new Mapunits();
            this.screen.mapUnit(6);
            this.sound = new Mapunits();
            this.sound.mapUnit(6);
            this.sound_blink = new Mapunits();
            this.sound_blink.mapUnit(6);
            this.enemy3 = new Enepc();
            this.enemy3.init(17153, 3, 14.0f, 0.0f, -3.0f, 135.0f);
            this.enemy3.id = 3;
            this.enemy3.setGroup(5, 5, 5, 5);
            this.enemy3.setParams(0, 0, 3, 3);
            this.enemy3.setBatEvent(10);
            this.enemy3.setVisible(false);
            this.enemy3.setInvalidID(1);
            this.enemy3.kickEnepc(4, 1);
            this.enemy4 = new Enepc();
            this.enemy4.init(17153, 3, 18.0f, 0.0f, -3.0f, 225.0f);
            this.enemy4.id = 4;
            this.enemy4.setGroup(5, 5, 5, 5);
            this.enemy4.setParams(0, 0, 4, 3);
            this.enemy4.setBatEvent(10);
            this.enemy4.setVisible(false);
            this.enemy4.setInvalidID(1);
            this.enemy4.kickEnepc(4, 1);
            this.enemy10 = new Enepc();
            this.enemy10.init(17153, 3, 18.0f, 0.0f, -3.0f, 225.0f);
            this.enemy10.id = 10;
            this.enemy10.setGroup(5, 5, 5, 5);
            this.enemy10.setParams(0, 0, 10, 3);
            this.enemy10.setInvalidID(1);
            this.enemy10.setTogetherWith(-2, -1, -1, -1);
            this.enemy10.setBatEvent(10);
            this.enemy10.setVisible(false);
            this.enemy10.setInvalidID(1);
        }
        this.Obj603_B = new Effect(1435, 3);
        this.Obj603_B.disp(false);
        this.Obj604_B = new Effect(1430, 4);
        this.Obj604_B.disp(false);
        this.Obj605_C = new Effect(1433, 5);
        this.Obj605_C.disp(false);
        if (this.CONSOLE == 0 && this.NO3 == 0) {
            System.println("NO3コンソール起動前の初期化");
            Stage.setVisible(22, false);
            this.Obj603_A = new Effect(1436, 3);
            this.Obj603_A.disp(true);
            this.Obj604_A = new Effect(1431, 4);
            this.Obj604_A.disp(true);
            this.Obj605_A = new Effect(1437, 5);
            this.Obj605_A.disp(true);
            this.Obj605_B = new Effect(1434, 5);
            this.Obj605_B.disp(false);
            this.Obj605_C = new Effect(1433, 5);
            this.Obj605_C.disp(false);
        } else if (this.CONSOLE == 1 && this.NO3 == 0) {
            System.println("NO3コンソール起動後の初期化");
            this.Obj603_B.disp(true);
            this.Obj604_A = new Effect(1431, 4);
            this.Obj604_A.disp(true);
            this.Obj605_B = new Effect(1434, 5);
            this.Obj605_B.disp(true);
        } else if (this.NO3 == 1) {
            System.println("NO3コンソール起動後の初期化");
            this.Obj603_B.disp(true);
            this.Obj604_B.disp(true);
            this.Obj605_C.disp(true);
        }
        float[] fArray = new float[252];
        fArray[0] = 14.5f;
        fArray[2] = 15.0f;
        fArray[3] = -1.0f;
        fArray[4] = 14.5f;
        fArray[6] = 14.0f;
        fArray[8] = 14.5f;
        fArray[10] = 13.0f;
        fArray[11] = 1.0f;
        fArray[12] = 14.5f;
        fArray[14] = 16.0f;
        fArray[16] = 14.5f;
        fArray[18] = 17.0f;
        fArray[19] = 3.0f;
        fArray[20] = 12.5f;
        fArray[22] = 17.5f;
        fArray[23] = 4.0f;
        fArray[24] = 10.5f;
        fArray[26] = 17.5f;
        fArray[27] = 5.0f;
        fArray[28] = 8.5f;
        fArray[30] = 17.5f;
        fArray[31] = 6.0f;
        fArray[32] = 6.5f;
        fArray[34] = 17.5f;
        fArray[35] = 7.0f;
        fArray[36] = 4.5f;
        fArray[38] = 17.5f;
        fArray[39] = 8.0f;
        fArray[40] = 3.0f;
        fArray[42] = 17.5f;
        fArray[43] = 9.0f;
        fArray[44] = 2.5f;
        fArray[46] = 17.0f;
        fArray[47] = 10.0f;
        fArray[48] = 8.0f;
        fArray[50] = 15.0f;
        fArray[51] = 7.0f;
        fArray[52] = 6.0f;
        fArray[54] = 15.0f;
        fArray[55] = 8.0f;
        fArray[56] = 4.0f;
        fArray[58] = 15.0f;
        fArray[59] = 9.0f;
        fArray[60] = 2.5f;
        fArray[62] = 15.0f;
        fArray[63] = 14.0f;
        fArray[64] = 16.0f;
        fArray[66] = 15.0f;
        fArray[68] = 16.0f;
        fArray[70] = 14.0f;
        fArray[71] = 1.0f;
        fArray[72] = 16.0f;
        fArray[74] = 13.0f;
        fArray[75] = 2.0f;
        fArray[76] = 16.0f;
        fArray[78] = 16.0f;
        fArray[79] = 16.0f;
        fArray[80] = 16.0f;
        fArray[82] = 17.0f;
        fArray[83] = 19.0f;
        fArray[84] = 17.5f;
        fArray[86] = 17.5f;
        fArray[87] = 20.0f;
        fArray[88] = 19.5f;
        fArray[90] = 17.5f;
        fArray[91] = 21.0f;
        fArray[92] = 21.5f;
        fArray[94] = 17.5f;
        fArray[95] = 22.0f;
        fArray[96] = 23.5f;
        fArray[98] = 17.5f;
        fArray[99] = 23.0f;
        fArray[100] = 25.5f;
        fArray[102] = 17.5f;
        fArray[103] = 24.0f;
        fArray[104] = 27.5f;
        fArray[106] = 17.5f;
        fArray[107] = 25.0f;
        fArray[108] = 22.0f;
        fArray[110] = 15.0f;
        fArray[111] = 23.0f;
        fArray[112] = 24.0f;
        fArray[114] = 15.0f;
        fArray[115] = 24.0f;
        fArray[116] = 26.0f;
        fArray[118] = 15.0f;
        fArray[119] = 25.0f;
        fArray[120] = 28.0f;
        fArray[122] = 15.0f;
        fArray[123] = 26.0f;
        fArray[124] = 26.0f;
        fArray[126] = 13.0f;
        fArray[127] = 29.0f;
        fArray[128] = 26.0f;
        fArray[130] = 11.0f;
        fArray[131] = 31.0f;
        fArray[132] = 26.0f;
        fArray[134] = 9.0f;
        fArray[135] = 32.0f;
        fArray[136] = 26.0f;
        fArray[138] = 7.0f;
        fArray[139] = 33.0f;
        fArray[140] = 26.0f;
        fArray[142] = 5.0f;
        fArray[143] = 34.0f;
        fArray[144] = 26.0f;
        fArray[146] = 3.0f;
        fArray[147] = 35.0f;
        fArray[148] = 26.0f;
        fArray[150] = 1.0f;
        fArray[151] = 36.0f;
        fArray[152] = 26.0f;
        fArray[154] = -1.0f;
        fArray[155] = 37.0f;
        fArray[156] = 26.0f;
        fArray[158] = -3.0f;
        fArray[159] = 38.0f;
        fArray[160] = 28.0f;
        fArray[162] = 12.0f;
        fArray[163] = 31.0f;
        fArray[164] = 28.0f;
        fArray[166] = 10.0f;
        fArray[167] = 32.0f;
        fArray[168] = 28.0f;
        fArray[170] = 8.0f;
        fArray[171] = 33.0f;
        fArray[172] = 28.0f;
        fArray[174] = 6.0f;
        fArray[175] = 33.0f;
        fArray[176] = 28.0f;
        fArray[178] = 4.0f;
        fArray[179] = 34.0f;
        fArray[180] = 28.0f;
        fArray[182] = 2.0f;
        fArray[183] = 35.0f;
        fArray[184] = 28.0f;
        fArray[187] = 36.0f;
        fArray[188] = 28.0f;
        fArray[190] = -2.0f;
        fArray[191] = 37.0f;
        fArray[192] = 24.0f;
        fArray[194] = -1.0f;
        fArray[195] = 38.0f;
        fArray[196] = 20.5f;
        fArray[198] = -1.0f;
        fArray[199] = 48.0f;
        fArray[200] = 17.0f;
        fArray[202] = -1.0f;
        fArray[203] = 49.0f;
        fArray[204] = 13.5f;
        fArray[206] = -1.0f;
        fArray[207] = 50.0f;
        fArray[208] = 10.0f;
        fArray[210] = -1.0f;
        fArray[211] = 51.0f;
        fArray[212] = 6.5f;
        fArray[214] = -1.0f;
        fArray[215] = 52.0f;
        fArray[216] = 3.0f;
        fArray[218] = -1.0f;
        fArray[219] = 53.0f;
        fArray[220] = 24.0f;
        fArray[222] = -3.0f;
        fArray[223] = 39.0f;
        fArray[224] = 22.5f;
        fArray[226] = -3.0f;
        fArray[227] = 48.0f;
        fArray[228] = 19.0f;
        fArray[230] = -3.0f;
        fArray[231] = 49.0f;
        fArray[232] = 15.5f;
        fArray[234] = -3.0f;
        fArray[235] = 50.0f;
        fArray[236] = 12.0f;
        fArray[238] = -3.0f;
        fArray[239] = 51.0f;
        fArray[240] = 8.5f;
        fArray[242] = -3.0f;
        fArray[243] = 52.0f;
        fArray[244] = 5.0f;
        fArray[246] = -3.0f;
        fArray[247] = 53.0f;
        fArray[248] = 3.0f;
        fArray[250] = -3.0f;
        fArray[251] = 61.0f;
        float[] fArray2 = fArray;
        float[] fArray3 = new float[252];
        fArray3[0] = 16.5f;
        fArray3[2] = 15.0f;
        fArray3[3] = -1.0f;
        fArray3[4] = 16.5f;
        fArray3[6] = 16.0f;
        fArray3[8] = 16.5f;
        fArray3[10] = 14.0f;
        fArray3[12] = 16.5f;
        fArray3[14] = 13.0f;
        fArray3[15] = 2.0f;
        fArray3[16] = 14.5f;
        fArray3[18] = 16.0f;
        fArray3[19] = 1.0f;
        fArray3[20] = 14.5f;
        fArray3[22] = 15.0f;
        fArray3[24] = 14.5f;
        fArray3[26] = 14.0f;
        fArray3[27] = 2.0f;
        fArray3[28] = 14.5f;
        fArray3[30] = 13.0f;
        fArray3[31] = 3.0f;
        fArray3[32] = 12.5f;
        fArray3[34] = 16.0f;
        fArray3[35] = 4.0f;
        fArray3[36] = 10.5f;
        fArray3[38] = 16.0f;
        fArray3[39] = 8.0f;
        fArray3[40] = 8.5f;
        fArray3[42] = 16.0f;
        fArray3[43] = 9.0f;
        fArray3[44] = 6.5f;
        fArray3[46] = 16.0f;
        fArray3[47] = 10.0f;
        fArray3[48] = 4.5f;
        fArray3[50] = 16.0f;
        fArray3[51] = 11.0f;
        fArray3[52] = 2.5f;
        fArray3[54] = 16.0f;
        fArray3[55] = 12.0f;
        fArray3[56] = 14.5f;
        fArray3[58] = 17.0f;
        fArray3[59] = 4.0f;
        fArray3[60] = 12.5f;
        fArray3[62] = 17.0f;
        fArray3[63] = 14.0f;
        fArray3[64] = 10.5f;
        fArray3[66] = 17.0f;
        fArray3[67] = 15.0f;
        fArray3[68] = 8.5f;
        fArray3[70] = 17.0f;
        fArray3[71] = 16.0f;
        fArray3[72] = 6.5f;
        fArray3[74] = 17.0f;
        fArray3[75] = 17.0f;
        fArray3[76] = 4.5f;
        fArray3[78] = 17.0f;
        fArray3[79] = 18.0f;
        fArray3[80] = 2.5f;
        fArray3[82] = 17.0f;
        fArray3[83] = 19.0f;
        fArray3[84] = 18.5f;
        fArray3[86] = 16.0f;
        fArray3[87] = 1.0f;
        fArray3[88] = 20.5f;
        fArray3[90] = 16.0f;
        fArray3[91] = 21.0f;
        fArray3[92] = 22.5f;
        fArray3[94] = 16.0f;
        fArray3[95] = 22.0f;
        fArray3[96] = 24.5f;
        fArray3[98] = 16.0f;
        fArray3[99] = 23.0f;
        fArray3[100] = 25.5f;
        fArray3[102] = 16.0f;
        fArray3[103] = 24.0f;
        fArray3[104] = 27.5f;
        fArray3[106] = 16.0f;
        fArray3[107] = 25.0f;
        fArray3[108] = 16.5f;
        fArray3[110] = 17.0f;
        fArray3[111] = 1.0f;
        fArray3[112] = 18.5f;
        fArray3[114] = 17.0f;
        fArray3[115] = 27.0f;
        fArray3[116] = 20.5f;
        fArray3[118] = 17.0f;
        fArray3[119] = 28.0f;
        fArray3[120] = 22.5f;
        fArray3[122] = 17.0f;
        fArray3[123] = 29.0f;
        fArray3[124] = 24.5f;
        fArray3[126] = 17.0f;
        fArray3[127] = 30.0f;
        fArray3[128] = 26.5f;
        fArray3[130] = 17.0f;
        fArray3[131] = 31.0f;
        fArray3[132] = 27.5f;
        fArray3[134] = 17.0f;
        fArray3[135] = 32.0f;
        fArray3[136] = 27.5f;
        fArray3[138] = 14.0f;
        fArray3[139] = 26.0f;
        fArray3[140] = 27.5f;
        fArray3[142] = 11.5f;
        fArray3[143] = 34.0f;
        fArray3[144] = 27.5f;
        fArray3[146] = 9.0f;
        fArray3[147] = 35.0f;
        fArray3[148] = 27.5f;
        fArray3[150] = 7.5f;
        fArray3[151] = 36.0f;
        fArray3[152] = 27.5f;
        fArray3[154] = 5.0f;
        fArray3[155] = 37.0f;
        fArray3[156] = 27.5f;
        fArray3[158] = 2.5f;
        fArray3[159] = 38.0f;
        fArray3[160] = 27.5f;
        fArray3[163] = 39.0f;
        fArray3[164] = 27.5f;
        fArray3[166] = -1.0f;
        fArray3[167] = 40.0f;
        fArray3[168] = 27.5f;
        fArray3[170] = -2.75f;
        fArray3[171] = 41.0f;
        fArray3[172] = 25.5f;
        fArray3[174] = 14.0f;
        fArray3[175] = 25.0f;
        fArray3[176] = 25.5f;
        fArray3[178] = 11.5f;
        fArray3[179] = 43.0f;
        fArray3[180] = 25.5f;
        fArray3[182] = 9.0f;
        fArray3[183] = 44.0f;
        fArray3[184] = 25.5f;
        fArray3[186] = 7.5f;
        fArray3[187] = 45.0f;
        fArray3[188] = 25.5f;
        fArray3[190] = 5.0f;
        fArray3[191] = 46.0f;
        fArray3[192] = 25.5f;
        fArray3[194] = 2.5f;
        fArray3[195] = 47.0f;
        fArray3[196] = 25.5f;
        fArray3[199] = 48.0f;
        fArray3[200] = 25.5f;
        fArray3[202] = -1.0f;
        fArray3[203] = 49.0f;
        fArray3[204] = 25.5f;
        fArray3[206] = -2.75f;
        fArray3[207] = 42.0f;
        fArray3[208] = 22.5f;
        fArray3[210] = -2.75f;
        fArray3[211] = 51.0f;
        fArray3[212] = 19.0f;
        fArray3[214] = -2.75f;
        fArray3[215] = 52.0f;
        fArray3[216] = 15.5f;
        fArray3[218] = -2.75f;
        fArray3[219] = 53.0f;
        fArray3[220] = 12.0f;
        fArray3[222] = -2.75f;
        fArray3[223] = 54.0f;
        fArray3[224] = 8.5f;
        fArray3[226] = -2.75f;
        fArray3[227] = 55.0f;
        fArray3[228] = 22.5f;
        fArray3[230] = -1.25f;
        fArray3[231] = 50.0f;
        fArray3[232] = 19.0f;
        fArray3[234] = -1.25f;
        fArray3[235] = 57.0f;
        fArray3[236] = 14.5f;
        fArray3[238] = -1.25f;
        fArray3[239] = 58.0f;
        fArray3[240] = 12.5f;
        fArray3[242] = -1.25f;
        fArray3[243] = 59.0f;
        fArray3[244] = 8.5f;
        fArray3[246] = -1.25f;
        fArray3[247] = 60.0f;
        fArray3[248] = 5.0f;
        fArray3[250] = -2.0f;
        fArray3[251] = 61.0f;
        float[] fArray4 = fArray3;
        float[] fArray5 = new float[252];
        fArray5[0] = 14.5f;
        fArray5[2] = 13.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = 14.5f;
        fArray5[6] = 14.0f;
        fArray5[8] = 14.5f;
        fArray5[10] = 15.0f;
        fArray5[11] = 1.0f;
        fArray5[12] = 14.5f;
        fArray5[14] = 16.0f;
        fArray5[15] = 2.0f;
        fArray5[16] = 14.5f;
        fArray5[18] = 17.0f;
        fArray5[19] = 3.0f;
        fArray5[20] = 12.5f;
        fArray5[22] = 17.5f;
        fArray5[23] = 4.0f;
        fArray5[24] = 10.5f;
        fArray5[26] = 17.5f;
        fArray5[27] = 5.0f;
        fArray5[28] = 8.5f;
        fArray5[30] = 17.5f;
        fArray5[31] = 6.0f;
        fArray5[32] = 6.5f;
        fArray5[34] = 17.5f;
        fArray5[35] = 7.0f;
        fArray5[36] = 4.5f;
        fArray5[38] = 17.5f;
        fArray5[39] = 8.0f;
        fArray5[40] = 3.0f;
        fArray5[42] = 17.5f;
        fArray5[43] = 9.0f;
        fArray5[44] = 2.5f;
        fArray5[46] = 17.0f;
        fArray5[47] = 10.0f;
        fArray5[48] = 8.0f;
        fArray5[50] = 15.0f;
        fArray5[51] = 7.0f;
        fArray5[52] = 6.0f;
        fArray5[54] = 15.0f;
        fArray5[55] = 8.0f;
        fArray5[56] = 4.0f;
        fArray5[58] = 15.0f;
        fArray5[59] = 9.0f;
        fArray5[60] = 2.5f;
        fArray5[62] = 15.0f;
        fArray5[63] = 14.0f;
        fArray5[64] = 16.0f;
        fArray5[66] = 13.0f;
        fArray5[68] = 16.0f;
        fArray5[70] = 14.0f;
        fArray5[71] = 16.0f;
        fArray5[72] = 16.0f;
        fArray5[74] = 15.0f;
        fArray5[75] = 17.0f;
        fArray5[76] = 16.0f;
        fArray5[78] = 16.0f;
        fArray5[79] = 18.0f;
        fArray5[80] = 16.0f;
        fArray5[82] = 17.0f;
        fArray5[83] = 19.0f;
        fArray5[84] = 17.5f;
        fArray5[86] = 17.5f;
        fArray5[87] = 20.0f;
        fArray5[88] = 19.5f;
        fArray5[90] = 17.5f;
        fArray5[91] = 21.0f;
        fArray5[92] = 21.5f;
        fArray5[94] = 17.5f;
        fArray5[95] = 22.0f;
        fArray5[96] = 23.5f;
        fArray5[98] = 17.5f;
        fArray5[99] = 23.0f;
        fArray5[100] = 25.5f;
        fArray5[102] = 17.5f;
        fArray5[103] = 24.0f;
        fArray5[104] = 27.5f;
        fArray5[106] = 17.5f;
        fArray5[107] = 25.0f;
        fArray5[108] = 22.0f;
        fArray5[110] = 15.0f;
        fArray5[111] = 23.0f;
        fArray5[112] = 24.0f;
        fArray5[114] = 15.0f;
        fArray5[115] = 24.0f;
        fArray5[116] = 26.0f;
        fArray5[118] = 15.0f;
        fArray5[119] = 25.0f;
        fArray5[120] = 28.0f;
        fArray5[122] = 15.0f;
        fArray5[123] = 26.0f;
        fArray5[124] = 26.0f;
        fArray5[126] = 13.0f;
        fArray5[127] = 29.0f;
        fArray5[128] = 26.0f;
        fArray5[130] = 11.0f;
        fArray5[131] = 31.0f;
        fArray5[132] = 26.0f;
        fArray5[134] = 9.0f;
        fArray5[135] = 32.0f;
        fArray5[136] = 26.0f;
        fArray5[138] = 7.0f;
        fArray5[139] = 33.0f;
        fArray5[140] = 26.0f;
        fArray5[142] = 5.0f;
        fArray5[143] = 34.0f;
        fArray5[144] = 26.0f;
        fArray5[146] = 3.0f;
        fArray5[147] = 35.0f;
        fArray5[148] = 26.0f;
        fArray5[150] = 1.0f;
        fArray5[151] = 36.0f;
        fArray5[152] = 26.0f;
        fArray5[154] = -1.0f;
        fArray5[155] = 37.0f;
        fArray5[156] = 26.0f;
        fArray5[158] = -3.0f;
        fArray5[159] = 38.0f;
        fArray5[160] = 28.0f;
        fArray5[162] = 12.0f;
        fArray5[163] = 31.0f;
        fArray5[164] = 28.0f;
        fArray5[166] = 10.0f;
        fArray5[167] = 32.0f;
        fArray5[168] = 28.0f;
        fArray5[170] = 8.0f;
        fArray5[171] = 33.0f;
        fArray5[172] = 28.0f;
        fArray5[174] = 6.0f;
        fArray5[175] = 33.0f;
        fArray5[176] = 28.0f;
        fArray5[178] = 4.0f;
        fArray5[179] = 34.0f;
        fArray5[180] = 28.0f;
        fArray5[182] = 2.0f;
        fArray5[183] = 35.0f;
        fArray5[184] = 28.0f;
        fArray5[187] = 36.0f;
        fArray5[188] = 28.0f;
        fArray5[190] = -2.0f;
        fArray5[191] = 37.0f;
        fArray5[192] = 24.0f;
        fArray5[194] = -1.0f;
        fArray5[195] = 38.0f;
        fArray5[196] = 20.5f;
        fArray5[198] = -1.0f;
        fArray5[199] = 48.0f;
        fArray5[200] = 17.0f;
        fArray5[202] = -1.0f;
        fArray5[203] = 49.0f;
        fArray5[204] = 13.5f;
        fArray5[206] = -1.0f;
        fArray5[207] = 50.0f;
        fArray5[208] = 10.0f;
        fArray5[210] = -1.0f;
        fArray5[211] = 51.0f;
        fArray5[212] = 6.5f;
        fArray5[214] = -1.0f;
        fArray5[215] = 52.0f;
        fArray5[216] = 3.0f;
        fArray5[218] = -1.0f;
        fArray5[219] = 53.0f;
        fArray5[220] = 24.0f;
        fArray5[222] = -3.0f;
        fArray5[223] = 39.0f;
        fArray5[224] = 22.5f;
        fArray5[226] = -3.0f;
        fArray5[227] = 48.0f;
        fArray5[228] = 19.0f;
        fArray5[230] = -3.0f;
        fArray5[231] = 49.0f;
        fArray5[232] = 15.5f;
        fArray5[234] = -3.0f;
        fArray5[235] = 50.0f;
        fArray5[236] = 12.0f;
        fArray5[238] = -3.0f;
        fArray5[239] = 51.0f;
        fArray5[240] = 8.5f;
        fArray5[242] = -3.0f;
        fArray5[243] = 52.0f;
        fArray5[244] = 5.0f;
        fArray5[246] = -3.0f;
        fArray5[247] = 53.0f;
        fArray5[248] = 3.0f;
        fArray5[250] = -3.0f;
        fArray5[251] = 61.0f;
        float[] fArray6 = fArray5;
        float[] fArray7 = new float[252];
        fArray7[0] = 14.5f;
        fArray7[2] = 17.0f;
        fArray7[3] = -1.0f;
        fArray7[4] = 14.5f;
        fArray7[6] = 16.0f;
        fArray7[8] = 14.5f;
        fArray7[10] = 15.0f;
        fArray7[11] = 1.0f;
        fArray7[12] = 14.5f;
        fArray7[14] = 14.0f;
        fArray7[15] = 2.0f;
        fArray7[16] = 14.5f;
        fArray7[18] = 13.0f;
        fArray7[19] = 3.0f;
        fArray7[20] = 16.5f;
        fArray7[22] = 17.0f;
        fArray7[24] = 16.5f;
        fArray7[26] = 16.0f;
        fArray7[27] = 5.0f;
        fArray7[28] = 16.5f;
        fArray7[30] = 15.0f;
        fArray7[31] = 6.0f;
        fArray7[32] = 16.5f;
        fArray7[34] = 14.0f;
        fArray7[35] = 7.0f;
        fArray7[36] = 16.5f;
        fArray7[38] = 13.0f;
        fArray7[39] = 8.0f;
        fArray7[40] = 12.5f;
        fArray7[42] = 17.0f;
        fArray7[44] = 10.5f;
        fArray7[46] = 17.0f;
        fArray7[47] = 10.0f;
        fArray7[48] = 8.5f;
        fArray7[50] = 17.0f;
        fArray7[51] = 11.0f;
        fArray7[52] = 6.5f;
        fArray7[54] = 17.0f;
        fArray7[55] = 12.0f;
        fArray7[56] = 4.5f;
        fArray7[58] = 17.0f;
        fArray7[59] = 13.0f;
        fArray7[60] = 2.5f;
        fArray7[62] = 17.0f;
        fArray7[63] = 14.0f;
        fArray7[64] = 12.5f;
        fArray7[66] = 16.0f;
        fArray7[67] = 1.0f;
        fArray7[68] = 10.5f;
        fArray7[70] = 16.0f;
        fArray7[71] = 16.0f;
        fArray7[72] = 8.5f;
        fArray7[74] = 16.0f;
        fArray7[75] = 17.0f;
        fArray7[76] = 6.5f;
        fArray7[78] = 16.0f;
        fArray7[79] = 18.0f;
        fArray7[80] = 4.5f;
        fArray7[82] = 16.0f;
        fArray7[83] = 19.0f;
        fArray7[84] = 2.5f;
        fArray7[86] = 16.0f;
        fArray7[87] = 20.0f;
        fArray7[88] = 18.5f;
        fArray7[90] = 17.0f;
        fArray7[91] = 5.0f;
        fArray7[92] = 20.5f;
        fArray7[94] = 17.0f;
        fArray7[95] = 22.0f;
        fArray7[96] = 22.5f;
        fArray7[98] = 17.0f;
        fArray7[99] = 23.0f;
        fArray7[100] = 24.5f;
        fArray7[102] = 17.0f;
        fArray7[103] = 24.0f;
        fArray7[104] = 26.0f;
        fArray7[106] = 17.0f;
        fArray7[107] = 25.0f;
        fArray7[108] = 27.5f;
        fArray7[110] = 17.0f;
        fArray7[111] = 26.0f;
        fArray7[112] = 18.5f;
        fArray7[114] = 16.0f;
        fArray7[115] = 6.0f;
        fArray7[116] = 20.5f;
        fArray7[118] = 16.0f;
        fArray7[119] = 28.0f;
        fArray7[120] = 22.5f;
        fArray7[122] = 16.0f;
        fArray7[123] = 29.0f;
        fArray7[124] = 24.5f;
        fArray7[126] = 16.0f;
        fArray7[127] = 30.0f;
        fArray7[128] = 26.0f;
        fArray7[130] = 16.0f;
        fArray7[131] = 31.0f;
        fArray7[132] = 27.5f;
        fArray7[134] = 16.0f;
        fArray7[135] = 32.0f;
        fArray7[136] = 27.5f;
        fArray7[138] = 14.0f;
        fArray7[139] = 33.0f;
        fArray7[140] = 27.5f;
        fArray7[142] = 11.5f;
        fArray7[143] = 34.0f;
        fArray7[144] = 27.5f;
        fArray7[146] = 9.0f;
        fArray7[147] = 35.0f;
        fArray7[148] = 27.5f;
        fArray7[150] = 7.5f;
        fArray7[151] = 36.0f;
        fArray7[152] = 27.5f;
        fArray7[154] = 5.0f;
        fArray7[155] = 37.0f;
        fArray7[156] = 27.5f;
        fArray7[158] = 2.5f;
        fArray7[159] = 38.0f;
        fArray7[160] = 27.5f;
        fArray7[163] = 39.0f;
        fArray7[164] = 27.5f;
        fArray7[166] = -1.0f;
        fArray7[167] = 40.0f;
        fArray7[168] = 27.5f;
        fArray7[170] = -2.75f;
        fArray7[171] = 41.0f;
        fArray7[172] = 26.0f;
        fArray7[174] = 14.0f;
        fArray7[175] = 26.0f;
        fArray7[176] = 26.0f;
        fArray7[178] = 11.5f;
        fArray7[179] = 43.0f;
        fArray7[180] = 26.0f;
        fArray7[182] = 9.0f;
        fArray7[183] = 44.0f;
        fArray7[184] = 26.0f;
        fArray7[186] = 7.5f;
        fArray7[187] = 45.0f;
        fArray7[188] = 26.0f;
        fArray7[190] = 5.0f;
        fArray7[191] = 46.0f;
        fArray7[192] = 26.0f;
        fArray7[194] = 2.5f;
        fArray7[195] = 47.0f;
        fArray7[196] = 26.0f;
        fArray7[199] = 48.0f;
        fArray7[200] = 26.0f;
        fArray7[202] = -1.0f;
        fArray7[203] = 49.0f;
        fArray7[204] = 26.0f;
        fArray7[206] = -2.75f;
        fArray7[207] = 42.0f;
        fArray7[208] = 22.5f;
        fArray7[210] = -2.75f;
        fArray7[211] = 51.0f;
        fArray7[212] = 19.0f;
        fArray7[214] = -2.75f;
        fArray7[215] = 52.0f;
        fArray7[216] = 14.5f;
        fArray7[218] = -2.75f;
        fArray7[219] = 53.0f;
        fArray7[220] = 12.0f;
        fArray7[222] = -2.75f;
        fArray7[223] = 54.0f;
        fArray7[224] = 8.5f;
        fArray7[226] = -2.75f;
        fArray7[227] = 55.0f;
        fArray7[228] = 22.5f;
        fArray7[230] = -1.25f;
        fArray7[231] = 50.0f;
        fArray7[232] = 19.0f;
        fArray7[234] = -1.25f;
        fArray7[235] = 57.0f;
        fArray7[236] = 14.5f;
        fArray7[238] = -1.25f;
        fArray7[239] = 58.0f;
        fArray7[240] = 12.5f;
        fArray7[242] = -1.25f;
        fArray7[243] = 59.0f;
        fArray7[244] = 8.5f;
        fArray7[246] = -1.25f;
        fArray7[247] = 60.0f;
        fArray7[248] = 5.0f;
        fArray7[250] = -2.0f;
        fArray7[251] = 61.0f;
        float[] fArray8 = fArray7;
        float[] fArray9 = new float[90];
        fArray9[0] = 14.5f;
        fArray9[2] = 15.0f;
        fArray9[3] = 14.5f;
        fArray9[5] = 17.0f;
        fArray9[6] = 16.5f;
        fArray9[8] = 17.0f;
        fArray9[9] = 21.0f;
        fArray9[11] = 17.0f;
        fArray9[12] = 27.0f;
        fArray9[14] = 17.0f;
        fArray9[15] = 27.0f;
        fArray9[17] = 12.0f;
        fArray9[18] = 27.0f;
        fArray9[20] = 6.0f;
        fArray9[21] = 27.0f;
        fArray9[23] = 1.0f;
        fArray9[24] = 27.0f;
        fArray9[26] = -2.0f;
        fArray9[27] = 21.0f;
        fArray9[29] = -2.0f;
        fArray9[30] = 16.0f;
        fArray9[32] = -2.0f;
        fArray9[33] = 10.0f;
        fArray9[35] = -2.0f;
        fArray9[36] = 5.0f;
        fArray9[38] = -2.0f;
        fArray9[39] = 10.0f;
        fArray9[41] = -2.0f;
        fArray9[42] = 16.0f;
        fArray9[44] = -2.0f;
        fArray9[45] = 21.0f;
        fArray9[47] = -2.0f;
        fArray9[48] = 27.0f;
        fArray9[50] = -2.0f;
        fArray9[51] = 27.0f;
        fArray9[53] = 1.0f;
        fArray9[54] = 27.0f;
        fArray9[56] = 6.0f;
        fArray9[57] = 27.0f;
        fArray9[59] = 12.0f;
        fArray9[60] = 27.0f;
        fArray9[62] = 17.0f;
        fArray9[63] = 21.0f;
        fArray9[65] = 17.0f;
        fArray9[66] = 14.0f;
        fArray9[68] = 17.0f;
        fArray9[69] = 9.5f;
        fArray9[71] = 17.0f;
        fArray9[72] = 5.0f;
        fArray9[74] = 17.0f;
        fArray9[75] = 2.5f;
        fArray9[77] = 17.0f;
        fArray9[78] = 5.0f;
        fArray9[80] = 17.0f;
        fArray9[81] = 9.5f;
        fArray9[83] = 17.0f;
        fArray9[84] = 14.5f;
        fArray9[86] = 17.0f;
        fArray9[87] = 14.5f;
        fArray9[89] = 15.0f;
        float[] fArray10 = fArray9;
        float[] fArray11 = new float[21];
        fArray11[0] = 14.5f;
        fArray11[2] = 13.0f;
        fArray11[3] = 13.5f;
        fArray11[5] = 13.0f;
        fArray11[6] = 15.0f;
        fArray11[8] = 13.0f;
        fArray11[9] = 16.0f;
        fArray11[11] = 13.0f;
        fArray11[12] = 18.5f;
        fArray11[14] = 13.0f;
        fArray11[15] = 16.0f;
        fArray11[17] = 13.0f;
        fArray11[18] = 14.5f;
        fArray11[20] = 13.0f;
        float[] fArray12 = fArray11;
        float[] fArray13 = new float[81];
        fArray13[0] = 14.5f;
        fArray13[2] = 17.0f;
        fArray13[3] = 12.0f;
        fArray13[5] = 17.0f;
        fArray13[6] = 9.0f;
        fArray13[8] = 17.0f;
        fArray13[9] = 12.5f;
        fArray13[11] = 17.0f;
        fArray13[12] = 15.5f;
        fArray13[14] = 17.0f;
        fArray13[15] = 18.5f;
        fArray13[17] = 17.0f;
        fArray13[18] = 21.0f;
        fArray13[20] = 17.0f;
        fArray13[21] = 27.0f;
        fArray13[23] = 17.0f;
        fArray13[24] = 27.0f;
        fArray13[26] = 12.0f;
        fArray13[27] = 27.0f;
        fArray13[29] = 6.0f;
        fArray13[30] = 27.0f;
        fArray13[32] = 1.0f;
        fArray13[33] = 27.0f;
        fArray13[35] = -2.0f;
        fArray13[36] = 21.0f;
        fArray13[38] = -2.0f;
        fArray13[39] = 16.0f;
        fArray13[41] = -2.0f;
        fArray13[42] = 10.0f;
        fArray13[44] = -2.0f;
        fArray13[45] = 5.0f;
        fArray13[47] = -2.0f;
        fArray13[48] = 10.0f;
        fArray13[50] = -2.0f;
        fArray13[51] = 16.0f;
        fArray13[53] = -2.0f;
        fArray13[54] = 21.0f;
        fArray13[56] = -2.0f;
        fArray13[57] = 27.0f;
        fArray13[59] = -2.0f;
        fArray13[60] = 27.0f;
        fArray13[62] = 1.0f;
        fArray13[63] = 27.0f;
        fArray13[65] = 6.0f;
        fArray13[66] = 27.0f;
        fArray13[68] = 12.0f;
        fArray13[69] = 27.0f;
        fArray13[71] = 17.0f;
        fArray13[72] = 21.0f;
        fArray13[74] = 17.0f;
        fArray13[75] = 18.0f;
        fArray13[77] = 17.0f;
        fArray13[78] = 14.5f;
        fArray13[80] = 17.0f;
        float[] fArray14 = fArray13;
        if (this.NO3_CARDKEY == 0) {
            if (this.HEISHI_LIFE1 == 0) {
                this.enemy1 = new Enepc();
                this.enemy1.init(17153, 6, 12.5f, 0.0f, 13.5f, 90.0f);
                this.enemy1.id = 1;
                this.enemy1.setBatEvent(2);
                this.enemy1.setGroup(0, 0, 1, 1);
            }
            if (this.HEISHI_LIFE2 == 0) {
                this.enemy2 = new Enepc();
                this.enemy2.init(17153, 6, 18.5f, 0.0f, 13.5f, 0.0f);
                this.enemy2.id = 2;
                this.enemy2.setBatEvent(2);
                this.enemy2.setGroup(0, 0, 1, 1);
            }
            if (this.HEISHI == 0) {
                System.println("陰に隠れた兵士 初期化");
                this.enemy1.setParams(1, 0, 1, 6, fArray2);
                this.enemy1.dispRadar(false);
                this.enemy2.setParams(1, 0, 2, 6, fArray4);
                this.enemy2.dispRadar(false);
            } else if (this.HEISHI == 1) {
                if (this.HEISHI_LIFE1 == 0 && this.HEISHI_LIFE2 == 0) {
                    System.println("柱の陰終了後 Aパターン");
                    this.enemy1.setParams(1, 9, 1, 3, fArray2);
                    this.enemy1.setParams(fArray10);
                    this.enemy1.setTranslate(14.5f, 0.0f, 17.0f);
                    this.enemy2.setParams(1, 1, 2, 3, fArray4);
                    this.enemy2.setTranslate(16.5f, 0.0f, 15.0f);
                } else if (this.HEISHI_LIFE1 == 0 && this.HEISHI_LIFE2 == 1) {
                    System.println("柱の陰終了後 Bパターン");
                    this.enemy1.setParams(1, 9, 1, 3, fArray2);
                    this.enemy1.setParams(fArray10);
                    this.enemy1.setTranslate(14.5f, 0.0f, 17.0f);
                } else if (this.HEISHI_LIFE1 == 1 && this.HEISHI_LIFE2 == 0) {
                    System.println("柱の陰終了後 Cパターン");
                    this.enemy2.setParams(1, 1, 2, 3, fArray4);
                    this.enemy2.setTranslate(16.5f, 0.0f, 15.0f);
                }
            }
        } else {
            System.println("カニメカ 初期化");
            this.enemy7 = new Enepc();
            this.enemy7.init(16643, 5, 14.5f, 0.0f, 13.0f, 270.0f);
            this.enemy7.id = 7;
            this.enemy7.setGroup(3, 3, 4, 4);
            this.enemy7.setBatEvent(2);
            this.enemy7.setParams(1, 5, 7, 5, fArray6);
            this.enemy7.setParams(fArray12);
            this.enemy8 = new Enepc();
            this.enemy8.init(16643, 5, 14.5f, 0.0f, 17.0f, 90.0f);
            this.enemy8.id = 8;
            this.enemy8.setGroup(3, 3, 4, 4);
            this.enemy8.setBatEvent(2);
            this.enemy8.setParams(1, 5, 8, 5, fArray8);
            this.enemy8.setParams(fArray14);
        }
        System.println("初期化終了");
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

        void Bang_1() {
            System.println("TRAP1破壊イベント");
            ST1050.this.E_Move1.setTranslate(23.0f, 0.0f, -4.652f);
            ST1050.this.E_Move1.disp(true);
            Sound.effectPlay(196742);
            float f = 23.0f;
            while (f >= 17.1f) {
                ST1050.this.E_Move1.setTranslate(f, 0.0f, -4.652f);
                System.sleep(1);
                f -= 0.3f;
            }
            float f2 = 0.0f;
            while (f2 <= 1.379f) {
                ST1050.this.E_Move1.setTranslate(17.091f, f2, -4.8f);
                System.sleep(1);
                f2 += 0.3f;
            }
            ST1050.this.BLINK_FLAG1 = false;
            ST1050.this.E_Move1.disp(false);
            ST1050.this.E_Bang1.disp(true);
            Sound.effectStop(196742);
            Sound.effectPlay(196743);
            ST1050.this.Obj600.disp(false);
            ST1050.this.teiten3.setTranslate(22.048f, 0.0f, -4.652f);
        }

        void Bang_2() {
            System.println("TRAP1破壊イベント");
            ST1050.this.E_Move2.setTranslate(29.4f, 0.25f, 2.1f);
            ST1050.this.E_Move2.disp(true);
            Sound.effectPlay(196742);
            float f = 2.1f;
            while (f >= -4.8f) {
                ST1050.this.E_Move2.setTranslate(29.459f, 0.0f, f);
                System.sleep(1);
                f -= 0.3f;
            }
            float f2 = 29.459f;
            while (f2 > 28.136f) {
                ST1050.this.E_Move2.setTranslate(f2, 0.0f, -4.7f);
                System.sleep(1);
                f2 -= 0.3f;
            }
            float f3 = 0.0f;
            while (f3 <= 1.38f) {
                ST1050.this.E_Move2.setTranslate(28.136f, f3, -4.3f);
                System.sleep(1);
                f3 += 0.15f;
            }
            ST1050.this.BLINK_FLAG2 = false;
            ST1050.this.E_Move2.disp(false);
            ST1050.this.E_Bang2.disp(true);
            Sound.effectStop(196742);
            Sound.effectPlay(196743);
            ST1050.this.Obj601.disp(false);
            ST1050.this.teiten4.setTranslate(29.459f, 0.0f, 2.093f);
        }

        void Blink_1() {
            ST1050.this.BLINK_FLAG1 = true;
            System.println("エフェクト1の点滅開始");
            while (ST1050.this.BLINK_FLAG1) {
                ST1050.this.Obj600.disp(true);
                System.sleep(10);
                ST1050.this.Obj600.disp(false);
                System.sleep(10);
            }
        }

        void Blink_2() {
            ST1050.this.BLINK_FLAG2 = true;
            System.println("エフェクト2の点滅開始");
            while (ST1050.this.BLINK_FLAG2) {
                ST1050.this.Obj601.disp(true);
                System.sleep(10);
                ST1050.this.Obj601.disp(false);
                System.sleep(10);
            }
        }

        void Screen() {
            ST1050.this.SCREEN_FLAG = true;
            ST1050.this.FIN_FLAG = false;
            System.println("画面の点滅開始");
            int n = 0;
            while (ST1050.this.SCREEN_FLAG) {
                if (ST1050.this.FIN_FLAG) {
                    Runtime.setDefocus(0, 1, ST1050.this.DefaultScreenParam);
                    ST1050.this.SCREEN_FLAG = false;
                    break;
                }
                if (n % 20 == 0) {
                    n = 0;
                    Runtime.setDefocus(0, 1, ST1050.this.DefaultScreenParam);
                } else if (n % 10 == 0) {
                    Runtime.setDefocus(0, 1, ST1050.this.RedScreenParam);
                }
                ++n;
                System.sleep(1);
            }
        }

        void Sound_Blink() {
            System.println("SOUND_BLINK");
            ST1050.this.BLINK_FLAG1 = false;
            ST1050.this.BLINK_FLAG2 = false;
            if (!ST1050.this.SOUND_NUMBER) {
                int n = 0;
                while (n < 8) {
                    ST1050.this.Obj600.disp(false);
                    System.sleep(ST1050.this.timing[n]);
                    ST1050.this.Obj600.disp(true);
                    Sound.effectPlay(196741);
                    System.sleep(ST1050.this.timing[n + 1]);
                    if (n == 5) {
                        ST1050.this.screen.start(1, "Screen");
                        Sound.effectPlay(196745);
                    }
                    ++n;
                }
            } else if (ST1050.this.SOUND_NUMBER) {
                int n = 0;
                while (n < 8) {
                    ST1050.this.Obj601.disp(false);
                    System.sleep(ST1050.this.timing[n]);
                    ST1050.this.Obj601.disp(true);
                    Sound.effectPlay(196741);
                    System.sleep(ST1050.this.timing[n + 1]);
                    if (n == 5) {
                        ST1050.this.screen.start(1, "Screen");
                        Sound.effectPlay(196745);
                    }
                    ++n;
                }
            }
            ST1050.this.sound.start(1, "Sound_Loop");
        }

        void Sound_Loop() {
            System.println("SOUND_LOOP");
            ST1050.this.SOUND_FLAG = true;
            boolean bl = false;
            while (ST1050.this.SOUND_FLAG) {
                Sound.effectPlay(196741);
                if (!ST1050.this.SOUND_NUMBER) {
                    ST1050.this.Obj600.disp(bl);
                } else {
                    ST1050.this.Obj601.disp(bl);
                }
                bl ^= true;
                System.sleep(1);
            }
        }

        void Teiten_Move1() {
            ST1050.this.player.getTranslate();
            ST1050.this.teiten_s1 = new Uwamono(28690, 0.0f, 0.0f, 0.0f, 0.0f);
            ST1050.this.teiten_s1.SetBgm(196618);
            ST1050.this.teiten_s2 = new Uwamono(28690, 0.0f, 0.0f, 0.0f, 0.0f);
            ST1050.this.teiten_s2.SetBgm(196619);
            ST1050.this.teiten_s3 = new Uwamono(28690, 0.0f, 0.0f, 0.0f, 0.0f);
            ST1050.this.teiten_s3.SetBgm(196621);
            float f = 6.5f;
            while (f >= 4.0f) {
                ST1050.this.teiten_s1.setTranslate(ST1050.this.player.px, ST1050.this.player.py, ST1050.this.player.pz + f);
                ST1050.this.teiten_s2.setTranslate(ST1050.this.player.px, ST1050.this.player.py, ST1050.this.player.pz + f);
                ST1050.this.teiten_s3.setTranslate(ST1050.this.player.px, ST1050.this.player.py, ST1050.this.player.pz + f);
                System.sleep(1);
                f -= 0.015f;
            }
        }

        void Teiten_Move2() {
            ST1050.this.player.getTranslate();
            float f = -4.0f;
            while (f >= -100.0f) {
                ST1050.this.teiten_s1.setTranslate(ST1050.this.player.px, ST1050.this.player.py + f, ST1050.this.player.pz);
                ST1050.this.teiten_s2.setTranslate(ST1050.this.player.px, ST1050.this.player.py + f, ST1050.this.player.pz);
                ST1050.this.teiten_s3.setTranslate(ST1050.this.player.px, ST1050.this.player.py + f, ST1050.this.player.pz);
                System.sleep(1);
                f -= 0.015f;
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

