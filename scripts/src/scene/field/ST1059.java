import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_UTK05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1059
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK05_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int selected = 0;
    int Blink_Count = 0;
    Enepc enemy7;
    Enepc enemy8;
    boolean EnterCheck = false;
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
    Uwamono item1;
    Uwamono item3;
    Uwamono item5;
    Uwamono item7;
    Uwamono item9;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono K1;
    Uwamono K2;
    Effect Obj602_A;
    Effect Obj602_B;
    Effect fade;
    Light light = new Light(0);
    int ROOM4_CARDKEY = Runtime.checkItem(10, 4);
    int SWITCH = Runtime.getFlags(6015, 1);
    int page;
    String[] CardReader2 = new String[]{"There's a key card slot.\n", "/[waitkey(64)]/[close()]"};
    String[] CardReader3 = new String[]{"It looks as though the door will not open without the key card.\n", "/[waitkey(64)]/[close()]"};
    String[] CardReader4 = new String[]{"Unless you slide a /[color(0x329bbe)]U-TIC Card/[color(0x808080)] through the card reader...", "/[waitkey(64)]/[close()]"};
    String[] Q_ROOM4_CARDKEY = new String[]{"There's a key card slot. It looks like the /[color(0x329bbe)]U-TIC Card/[color(0x808080)]\nmight work on it.", "/[waitkey(1)]/[clear()]", "Use the /[color(0x329bbe)]U-TIC Card/[color(0x808080)]?\n"};

    ST1059() {
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

    void EOB_Always(int n) {
        System.println("EOB_Always !!!!!!!!!!!!!!!!!!!!!");
    }

    void EV_Camera2() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(27.483055f, 5.6319485f, 8.330599f);
        this.camEV.setRotate(-54.92433f, 302.92032f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Enemy_Start() {
        System.println("ENEMY_START!!!");
        this.enemy7.setVisible(true);
        this.enemy8.setVisible(true);
        this.enemy7.kickEnepc(4, 0);
        this.enemy8.kickEnepc(4, 0);
    }

    void Enemy_Stop() {
        System.println("ENEMY_STOP!!!");
        this.enemy7.setVisible(false);
        this.enemy8.setVisible(false);
        this.enemy7.kickEnepc(4, 2);
        this.enemy8.kickEnepc(4, 2);
    }

    void Final_init(int n) {
        System.println("FINAL INIT !!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    public void KickEvent(int n, int n2) {
        switch (n) {
            case 100: {
                if (n2 == 3 && this.SWITCH != 1 && !this.EnterCheck) {
                    this.Enemy_Stop();
                    this.EnterCheck = true;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.EV_Camera2();
                    if (this.ROOM4_CARDKEY == 0) {
                        this.DefaultTalk(this.CardReader2);
                    } else if (this.DefaultMenu(this.Q_ROOM4_CARDKEY) == 0) {
                        System.sleep(20);
                        this.Obj602_A.disp(false);
                        this.Obj602_B.disp(true);
                        Sound.effectPlay(196741);
                        System.sleep(30);
                        this.door_4.DoorOpen();
                        Sound.effectPlay(196744);
                        System.sleep(70);
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
                if (n2 != 4 || this.SWITCH == 1 || this.EnterCheck) break;
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
        }
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                this.teiten1.setTranslate(0.0f, -1000.0f, 0.0f);
                break;
            }
            case 2: {
                this.teiten2.setTranslate(0.0f, -1000.0f, 0.0f);
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
                Runtime.jumpCF(1049, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(1069, 2);
                break;
            }
            case 2: {
                Runtime.jumpCF(1069, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(1089, 1);
                break;
            }
            case 4: {
                Runtime.jumpCF(1099, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(1119, 1);
                break;
            }
            case 6: {
                Runtime.jumpCF(1039, 1);
                break;
            }
            case 7: {
                Runtime.jumpCF(1109, 1);
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
        this.teiten1 = new Uwamono(28690, 23.968f, 0.0f, -4.706f, 0.0f);
        this.teiten1.SetBgm(196619);
        this.teiten2 = new Uwamono(28690, 29.705f, 0.0f, 2.99f, 0.0f);
        this.teiten2.SetBgm(196619);
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
        System.println("doorB の初期化");
        this.doorB = new Uwamono(49, 42, '\u0001');
        new Uwamono(48, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0002');
        this.doorB.SetDoorRange(2.0f);
        this.doorB.DoorOpen();
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
        this.doorD.SetDoorSpd(0);
        this.doorD.DoorOpen();
        this.door_0 = new Uwamono(71, 40, '\u0001');
        this.door_0.SetDoorType('\u0004');
        this.door_1 = new Uwamono(72, 40, '\u0001');
        this.door_1.SetDoorType('\u0004');
        this.door_2 = new Uwamono(70, 40, '\u0001');
        this.door_2.SetDoorType('\u0004');
        this.door_3 = new Uwamono(69, 40, '\u0001');
        this.door_3.SetDoorType('\u0004');
        this.door_5 = new Uwamono(67, 40, '\u0001');
        this.door_5.SetDoorType('\u0004');
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
        this.K1 = new Uwamono(125, 30);
        this.K1.SetCallNo(1);
        this.K2 = new Uwamono(126, 20);
        this.K2.SetCallNo(2);
        float[] fArray = new float[252];
        fArray[0] = 14.5f;
        fArray[2] = 13.0f;
        fArray[3] = -1.0f;
        fArray[4] = 14.5f;
        fArray[6] = 14.0f;
        fArray[8] = 14.5f;
        fArray[10] = 15.0f;
        fArray[11] = 1.0f;
        fArray[12] = 14.5f;
        fArray[14] = 16.0f;
        fArray[15] = 2.0f;
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
        fArray[66] = 13.0f;
        fArray[68] = 16.0f;
        fArray[70] = 14.0f;
        fArray[71] = 16.0f;
        fArray[72] = 16.0f;
        fArray[74] = 15.0f;
        fArray[75] = 17.0f;
        fArray[76] = 16.0f;
        fArray[78] = 16.0f;
        fArray[79] = 18.0f;
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
        fArray3[0] = 14.5f;
        fArray3[2] = 17.0f;
        fArray3[3] = -1.0f;
        fArray3[4] = 14.5f;
        fArray3[6] = 16.0f;
        fArray3[8] = 14.5f;
        fArray3[10] = 15.0f;
        fArray3[11] = 1.0f;
        fArray3[12] = 14.5f;
        fArray3[14] = 14.0f;
        fArray3[15] = 2.0f;
        fArray3[16] = 14.5f;
        fArray3[18] = 13.0f;
        fArray3[19] = 3.0f;
        fArray3[20] = 16.5f;
        fArray3[22] = 17.0f;
        fArray3[24] = 16.5f;
        fArray3[26] = 16.0f;
        fArray3[27] = 5.0f;
        fArray3[28] = 16.5f;
        fArray3[30] = 15.0f;
        fArray3[31] = 6.0f;
        fArray3[32] = 16.5f;
        fArray3[34] = 14.0f;
        fArray3[35] = 7.0f;
        fArray3[36] = 16.5f;
        fArray3[38] = 13.0f;
        fArray3[39] = 8.0f;
        fArray3[40] = 12.5f;
        fArray3[42] = 17.0f;
        fArray3[44] = 10.5f;
        fArray3[46] = 17.0f;
        fArray3[47] = 10.0f;
        fArray3[48] = 8.5f;
        fArray3[50] = 17.0f;
        fArray3[51] = 11.0f;
        fArray3[52] = 6.5f;
        fArray3[54] = 17.0f;
        fArray3[55] = 12.0f;
        fArray3[56] = 4.5f;
        fArray3[58] = 17.0f;
        fArray3[59] = 13.0f;
        fArray3[60] = 2.5f;
        fArray3[62] = 17.0f;
        fArray3[63] = 14.0f;
        fArray3[64] = 12.5f;
        fArray3[66] = 16.0f;
        fArray3[67] = 1.0f;
        fArray3[68] = 10.5f;
        fArray3[70] = 16.0f;
        fArray3[71] = 16.0f;
        fArray3[72] = 8.5f;
        fArray3[74] = 16.0f;
        fArray3[75] = 17.0f;
        fArray3[76] = 6.5f;
        fArray3[78] = 16.0f;
        fArray3[79] = 18.0f;
        fArray3[80] = 4.5f;
        fArray3[82] = 16.0f;
        fArray3[83] = 19.0f;
        fArray3[84] = 2.5f;
        fArray3[86] = 16.0f;
        fArray3[87] = 20.0f;
        fArray3[88] = 18.5f;
        fArray3[90] = 17.0f;
        fArray3[91] = 5.0f;
        fArray3[92] = 20.5f;
        fArray3[94] = 17.0f;
        fArray3[95] = 22.0f;
        fArray3[96] = 22.5f;
        fArray3[98] = 17.0f;
        fArray3[99] = 23.0f;
        fArray3[100] = 24.5f;
        fArray3[102] = 17.0f;
        fArray3[103] = 24.0f;
        fArray3[104] = 26.0f;
        fArray3[106] = 17.0f;
        fArray3[107] = 25.0f;
        fArray3[108] = 27.5f;
        fArray3[110] = 17.0f;
        fArray3[111] = 26.0f;
        fArray3[112] = 18.5f;
        fArray3[114] = 16.0f;
        fArray3[115] = 6.0f;
        fArray3[116] = 20.5f;
        fArray3[118] = 16.0f;
        fArray3[119] = 28.0f;
        fArray3[120] = 22.5f;
        fArray3[122] = 16.0f;
        fArray3[123] = 29.0f;
        fArray3[124] = 24.5f;
        fArray3[126] = 16.0f;
        fArray3[127] = 30.0f;
        fArray3[128] = 26.0f;
        fArray3[130] = 16.0f;
        fArray3[131] = 31.0f;
        fArray3[132] = 27.5f;
        fArray3[134] = 16.0f;
        fArray3[135] = 32.0f;
        fArray3[136] = 27.5f;
        fArray3[138] = 14.0f;
        fArray3[139] = 33.0f;
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
        fArray3[172] = 26.0f;
        fArray3[174] = 14.0f;
        fArray3[175] = 26.0f;
        fArray3[176] = 26.0f;
        fArray3[178] = 11.5f;
        fArray3[179] = 43.0f;
        fArray3[180] = 26.0f;
        fArray3[182] = 9.0f;
        fArray3[183] = 44.0f;
        fArray3[184] = 26.0f;
        fArray3[186] = 7.5f;
        fArray3[187] = 45.0f;
        fArray3[188] = 26.0f;
        fArray3[190] = 5.0f;
        fArray3[191] = 46.0f;
        fArray3[192] = 26.0f;
        fArray3[194] = 2.5f;
        fArray3[195] = 47.0f;
        fArray3[196] = 26.0f;
        fArray3[199] = 48.0f;
        fArray3[200] = 26.0f;
        fArray3[202] = -1.0f;
        fArray3[203] = 49.0f;
        fArray3[204] = 26.0f;
        fArray3[206] = -2.75f;
        fArray3[207] = 42.0f;
        fArray3[208] = 22.5f;
        fArray3[210] = -2.75f;
        fArray3[211] = 51.0f;
        fArray3[212] = 19.0f;
        fArray3[214] = -2.75f;
        fArray3[215] = 52.0f;
        fArray3[216] = 14.5f;
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
        float[] fArray5 = new float[21];
        fArray5[0] = 14.5f;
        fArray5[2] = 13.0f;
        fArray5[3] = 13.5f;
        fArray5[5] = 13.0f;
        fArray5[6] = 15.0f;
        fArray5[8] = 13.0f;
        fArray5[9] = 16.0f;
        fArray5[11] = 13.0f;
        fArray5[12] = 18.5f;
        fArray5[14] = 13.0f;
        fArray5[15] = 16.0f;
        fArray5[17] = 13.0f;
        fArray5[18] = 14.5f;
        fArray5[20] = 13.0f;
        float[] fArray6 = fArray5;
        float[] fArray7 = new float[81];
        fArray7[0] = 14.5f;
        fArray7[2] = 17.0f;
        fArray7[3] = 12.0f;
        fArray7[5] = 17.0f;
        fArray7[6] = 9.0f;
        fArray7[8] = 17.0f;
        fArray7[9] = 12.5f;
        fArray7[11] = 17.0f;
        fArray7[12] = 15.5f;
        fArray7[14] = 17.0f;
        fArray7[15] = 18.5f;
        fArray7[17] = 17.0f;
        fArray7[18] = 21.0f;
        fArray7[20] = 17.0f;
        fArray7[21] = 27.0f;
        fArray7[23] = 17.0f;
        fArray7[24] = 27.0f;
        fArray7[26] = 12.0f;
        fArray7[27] = 27.0f;
        fArray7[29] = 6.0f;
        fArray7[30] = 27.0f;
        fArray7[32] = 1.0f;
        fArray7[33] = 27.0f;
        fArray7[35] = -2.0f;
        fArray7[36] = 21.0f;
        fArray7[38] = -2.0f;
        fArray7[39] = 16.0f;
        fArray7[41] = -2.0f;
        fArray7[42] = 10.0f;
        fArray7[44] = -2.0f;
        fArray7[45] = 5.0f;
        fArray7[47] = -2.0f;
        fArray7[48] = 10.0f;
        fArray7[50] = -2.0f;
        fArray7[51] = 16.0f;
        fArray7[53] = -2.0f;
        fArray7[54] = 21.0f;
        fArray7[56] = -2.0f;
        fArray7[57] = 27.0f;
        fArray7[59] = -2.0f;
        fArray7[60] = 27.0f;
        fArray7[62] = 1.0f;
        fArray7[63] = 27.0f;
        fArray7[65] = 6.0f;
        fArray7[66] = 27.0f;
        fArray7[68] = 12.0f;
        fArray7[69] = 27.0f;
        fArray7[71] = 17.0f;
        fArray7[72] = 21.0f;
        fArray7[74] = 17.0f;
        fArray7[75] = 18.0f;
        fArray7[77] = 17.0f;
        fArray7[78] = 14.5f;
        fArray7[80] = 17.0f;
        float[] fArray8 = fArray7;
        System.println("カニメカ 初期化");
        this.enemy7 = new Enepc();
        this.enemy7.init(16643, 3, 14.5f, 0.0f, 13.0f, 90.0f);
        this.enemy7.id = 7;
        this.enemy7.setGroup(3, 3, 4, 4);
        this.enemy7.setParams(1, 5, 7, 3, fArray2);
        this.enemy7.setParams(fArray6);
        this.enemy8 = new Enepc();
        this.enemy8.init(16643, 3, 14.5f, 0.0f, 17.0f, 90.0f);
        this.enemy8.id = 8;
        this.enemy8.setGroup(3, 3, 4, 4);
        this.enemy8.setParams(1, 5, 8, 3, fArray4);
        this.enemy8.setParams(fArray8);
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

