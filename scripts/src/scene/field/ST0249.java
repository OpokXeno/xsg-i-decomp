import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK24_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0241
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK24_PRJ {
    Player player;
    Camera cam1;
    Camera cam2;
    Camera cam3;
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
    Enepc enemy6;
    Menu menu;
    Window win;
    int count = 0;
    int kcount = 0;
    int doorswitch;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono trap4;
    Uwamono trap5;
    Uwamono item1;
    Uwamono item2;
    Uwamono tansu;
    Uwamono monsbox;
    Uwamono lift;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Effect ef30201a;
    Effect ef30201b;
    Effect ef30202a;
    Effect ef30202b;
    Effect ef30203;
    Effect ef30205a;
    Effect ef30205b;
    Effect fade;
    int key;
    int doorcheck_boss;
    int doorcheck_2F;
    int tansu_check_2F;
    int S03;
    int serihu1;
    Light light = new Light(0);
    int button_flg;
    String moji;
    int page;
    String[] msg_shion_0 = new String[]{"/[label(Shion)]", "Not I can unlock the door!\n", "/[waitkey(64)]/[close()]"};
    String[] msg_shion_1 = new String[]{"/[label(Shion)]", "Ah, here it is...\n", "This should be the room.\n", "/[waitkey(64)]/[close()]"};
    String[] msg_shion_2 = new String[]{"/[label(Shion)]", "Let's see, to open this door...\n", "/[waitkey(64)]/[clear()]"};
    String[] msg_shion_3 = new String[]{"/[label(Shion)]", "Looks like I need a Mission Key to open this door.\n", "/[waitkey(64)]/[close()]"};
    String[] msg_shion_4 = new String[]{"/[label(Shion)]", "It won't open from this side. I think it's locked.", "/[waitkey(64)]/[close()]"};
    String[] Talk02_2 = new String[]{"/[label(Shion)]", "Looks like there's nothing else.\n", "/[waitkey(64)]/[close()]"};
    String[] Talk03_1 = new String[]{"/[label(Shion)]", "Whoa!\n", "/[waitkey(64)]/[clear()]"};
    String[] msg_allen_0 = new String[]{"/[label(Allen)]", "You need to insert the Mission Key into the card slot on the right!", "/[waitkey(64)]/[close()]"};
    String[] Talk03_2 = new String[]{"/[label(Allen)]", "Chief, items aren't the only things that are inside containers!\n", "/[waitkey(64)]/[close()]"};
    String[] sys_0 = new String[]{"There is a door marked Number 10.\n", "/[waitkey(64)]/[close()]"};
    String[] sys_1 = new String[]{"Inserted the Mission Key!\n", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 10.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 10.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 10, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST0241() {
    }

    void Action1() {
        Runtime.setPlayerControl(false);
        switch (this.count) {
            case 0: {
                this.cam0.setMode(-1);
                this.EV_Camera_2();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg_shion_1, 0);
                ST0241.waitPage(this.win, 64);
                this.cam0.setMode(0);
                ++this.count;
                break;
            }
            case 1: {
                this.cam0.setMode(-1);
                this.Player_();
                this.EV_Camera_3_1();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg_shion_2, 0);
                ST0241.waitPage(this.win, 64);
                this.EV_Camera_3_2();
                this.win.print(this.msg_allen_0, 0);
                this.TikaTika();
                ST0241.waitPage(this.win, 64);
                this.cam0.setMode(0);
                Runtime.setFlags(1008, 1, 1);
                break;
            }
        }
        Runtime.setPlayerControl(true);
    }

    void Before_Key() {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg_shion_3, 0);
        ST0241.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
    }

    void Door_2F() {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg_shion_4, 0);
        ST0241.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
    }

    void EOB(int n) {
    }

    void EV_Camera() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.044f, 5.454f, -3.768f);
        this.camEV.setRotate(-40.113f, 306.88f, 0.0f);
        this.camEV.setFov(29.0f);
        this.camEV.change();
    }

    void EV_Camera_2() {
        float[] fArray = new float[]{1.0f, 8.467f, 1.998f, -1.004f, 30.0f, 8.467f, 0.882f, -11.895f};
        this.camEV = Camera.create(1);
        this.camEV.setRotate(0.0f, 0.0f, 0.0f);
        this.camEV.transSPL(fArray, 1, 1, 30);
        this.camEV.setFov(29.999f);
        this.camEV.change();
    }

    void EV_Camera_3_1() {
        this.camEV = Camera.create(1);
        this.camEV.setRotate(0.0f, 0.0f, 0.0f);
        this.camEV.setTranslate(8.467f, 1.998f, -1.004f);
        this.camEV.setFov(29.999f);
        this.camEV.change();
    }

    void EV_Camera_3_2() {
        float[] fArray = new float[]{1.0f, 8.467f, 1.998f, -1.004f, 30.0f, 9.467f, 1.998f, -1.004f};
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 1, 30);
        this.camEV.setFov(29.999f);
        this.camEV.change();
    }

    void EV_Camera_4() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-7.738f, 4.8699f, 9.101f);
        this.camEV.setRotate(-37.5223f, 14.02f, 0.0f);
        this.camEV.setFov(39.0f);
        this.camEV.change();
    }

    void EV_Camera_5() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.67f, 6.3f, -7.7f);
        this.camEV.setRotate(-24.0f, 469.0f, 0.0f);
        this.camEV.setFov(26.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            case 1: {
                this.enemy1.kickEnepc(10, 70, 0);
                break;
            }
            case 2: {
                this.enemy2.kickEnepc(10, 100, 0);
                break;
            }
            case 3: {
                this.enemy3.kickEnepc(10, 85, 0);
                break;
            }
            case 4: {
                this.enemy4.kickEnepc(10, 70, 0);
                break;
            }
        }
    }

    void KeyGet() {
        switch (this.kcount) {
            case 0: {
                if (this.key == 0) {
                    System.sleep(15);
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.setTranslate(0.247f, 4.0f, -7.15f);
                    this.player.setRotate(0.0f, 0.0f, 0.0f);
                    this.cam0.setMode(-1);
                    this.EV_Camera_5();
                    this.player.mtn(26, 9, 1.0f, true);
                    System.sleep(32);
                    this.ef30203.disp(false);
                    Stage.setVisible(15, false);
                    System.sleep(45);
                    this.cam0.setMode(0);
                    Runtime.disable(65536);
                    this.moji = Runtime.getItemName(10, 1);
                    this.win = Window.create();
                    this.win.print("/[color(0x329bbe)]Mission Key/[color(0x808080)] obtained!/[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 1);
                    ST0241.waitPage(this.win, 64);
                    this.win = Window.create();
                    this.win.print(this.msg_shion_0, 0);
                    ST0241.waitPage(this.win, 64);
                    Runtime.setFlags(1001, 1, 1);
                    Runtime.setPlayerControl(true);
                    ++this.kcount;
                } else {
                    this.win = Window.create();
                    this.win.print(this.Talk02_2, 0);
                    ST0241.waitPage(this.win, 64);
                }
                return;
            }
            case 1: {
                this.win = Window.create();
                this.win.print(this.Talk02_2, 0);
                ST0241.waitPage(this.win, 64);
                return;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        Runtime.enable(262144);
        switch (n2) {
            case 2: {
                System.println("KickEvent2F!!!!!!!!!!!!!!!!");
                this.doorcheck_2F = Runtime.getFlags(1003, 1);
                if (this.button_flg == 1) {
                    Runtime.disable(262144);
                    return;
                }
                if (this.doorcheck_2F == 1) {
                    Runtime.disable(262144);
                    System.println("スイッチＯＮのため何もせずに終了");
                    return;
                }
                if (this.S03 == 0 && this.doorcheck_2F == 0) {
                    this.button_flg = 1;
                    System.println("ロックしてあるメッセージ");
                    this.Door_2F();
                    Runtime.disable(262144);
                }
                if (this.S03 == 1 && this.doorcheck_2F == 0) {
                    this.button_flg = 1;
                    Runtime.setPlayerControl(false);
                    System.println("ロックを解除するメッセージ");
                    this.ef30205a.disp(false);
                    this.ef30205b.disp(true);
                    Runtime.setFlags(1003, 1, 1);
                    this.win = Window.create();
                    Sound.effectPlay(58);
                    this.win.print("ロックが解除されました/[waitkey(64)]/[close()]");
                    ST0241.waitPage(this.win, 64);
                    Runtime.disable(262144);
                    this.doorC.SetDoorType('\u0004');
                    Runtime.setPlayerControl(true);
                }
                System.println("処理を終了したメッセージ");
                break;
            }
            case 5: {
                System.println("サブルート");
                this.player.getTranslate();
                if (this.player.py > 3.0f) {
                    Runtime.disable(262144);
                    System.println("高さが違うので抜けます");
                    return;
                }
                if (this.button_flg == 1) {
                    Runtime.disable(262144);
                    return;
                }
                if (Runtime.getFlags(3290, 1) == 1) {
                    Runtime.disable(262144);
                    return;
                }
                if (Runtime.getFlags(3210, 1) == 0) {
                    this.button_flg = 1;
                    this.SubRoot_1st();
                    break;
                }
                if (Runtime.getFlags(3230, 1) == 0) {
                    this.button_flg = 1;
                    this.SubRoot_Not_Yet();
                    Runtime.disable(262144);
                    this.button_flg = 0;
                    return;
                }
                this.button_flg = 1;
                this.SubRoot_Key();
                Runtime.disable(262144);
                this.button_flg = 0;
                return;
            }
        }
        Runtime.disable(262144);
        this.button_flg = 0;
    }

    void NotYet() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print("工事中です/[wait(30)]/[close()]");
    }

    void Open_Mes() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.sys_1, 0);
        ST0241.waitPage(this.win, 64);
    }

    void Player_() {
        Runtime.enable(65536);
        this.player.setTranslate(8.467f, 0.0f, -5.453f);
        this.player.setRotate(0.0f, 180.0f, 0.0f);
        Runtime.disable(65536);
    }

    void Player__() {
        Runtime.enable(65536);
        this.player.mtn(12, 8, 1.0f, true);
        System.sleep(300);
        Runtime.disable(65536);
    }

    void SubRoot_1st() {
        System.println("サブルート発見");
        Runtime.setFlags(3210, 1, 1);
        Runtime.setPlayerControl(false);
        Sound.effectPlay(55);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.SUB_01, 0);
        ST0241.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
    }

    void SubRoot_Key() {
        System.println("サブルート発見済＆鍵あり");
        this.doorE.SetDoorType('\u0004');
        Runtime.setFlags(3290, 1, 1);
        Runtime.setPlayerControl(false);
        Sound.effectPlay(56);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.SUB_03, 0);
        ST0241.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
    }

    void SubRoot_Not_Yet() {
        System.println("サブルート発見済＆鍵なし");
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.SUB_02, 0);
        ST0241.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
    }

    void TikaTika() {
        int n = 0;
        while (n < 8) {
            this.ef30202a.disp(false);
            this.ef30202b.disp(true);
            System.sleep(5);
            this.ef30202a.disp(true);
            this.ef30202b.disp(false);
            System.sleep(5);
            ++n;
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("broken");
                this.enemy1.kickEnepc(7, 6);
                Runtime.setPlayerControl(true);
                break;
            }
            case 2: {
                System.println("フォークリフトを壊しました。次回は生成しません。");
                Runtime.setFlags(1018, 1, 1);
                break;
            }
            case 3: {
                System.println("2Fの壊れ物を壊しました。次回は生成しません。");
                Runtime.setFlags(1004, 1, 1);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(269, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(259, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(259, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(259, 3);
                break;
            }
            case 4: {
                Runtime.jumpCF(239, 1);
                break;
            }
            case 5: {
                Runtime.jumpCF(239, 2);
                break;
            }
            case 6: {
                System.println("サブルートへ");
                System.println("FUJI_25セット");
                Runtime.setFlags(1025, 1, 1);
                Runtime.jumpCF(220, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 8.5f, 0.0f, -4.0f, 0.0f);
        this.teiten1.SetBgm(196616);
        this.teiten2 = new Uwamono(28690, 4.911f, 5.0f, -10.0f, 0.0f);
        this.teiten2.SetBgm(196617);
        this.teiten3 = new Uwamono(28690, 8.748f, 5.0f, -10.0f, 0.0f);
        this.teiten3.SetBgm(196617);
        this.teiten4 = new Uwamono(28690, 13.0f, 5.0f, -9.804f, 0.0f);
        this.teiten4.SetBgm(196617);
        this.teiten5 = new Uwamono(28690, 13.0f, 5.0f, -5.676f, 0.0f);
        this.teiten5.SetBgm(196617);
        this.teiten6 = new Uwamono(28690, 8.5f, 0.0f, 6.0f, 0.0f);
        this.teiten6.SetBgm(196618);
        this.teiten7 = new Uwamono(28690, -5.5f, 0.0f, -0.0f, 0.0f);
        this.teiten7.SetBgm(196618);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.78f, 0.78f, 0.78f);
        this.light.setDirection2(1, -0.023f, 1.0f, 0.008f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(2, -0.015f, 0.81f, 0.586f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 0.024f, -0.0f, -1.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 0, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 1, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 2, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 3, 0.15f, 0.15f, 0.15f);
        this.key = Runtime.getFlags(1001, 1);
        this.doorcheck_boss = Runtime.getFlags(1002, 1);
        this.doorcheck_2F = Runtime.getFlags(1003, 1);
        this.S03 = Runtime.getFlags(3, 1);
        this.tansu_check_2F = Runtime.getFlags(1004, 1);
        this.serihu1 = Runtime.getFlags(1006, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(61, false);
        Stage.setVisible(62, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 12345, 1);
        Runtime.setDefocusQuick(1, 1, 23456, 1);
        this.cam0.setFog(1, 12.0f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setFog(2, 13.0f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setFog(3, 13.0f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setFog(4, 13.0f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setFog(5, 6.5f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setFog(6, 11.0f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setFog(7, 7.0f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setFog(8, 11.5f, 20.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setCFAngle(1, -28.0f, 375.0f, 0.0f, 14.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 340.0f, 0.0f, 20.0f, 30.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFPedestal(5, -1.427f, 8.437f, -1.396f, 50.0f, -34.288f, -4.269f, 0.0f, 2.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(5, 1);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 30.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFLockX(6, -3.0f);
        this.cam0.setCFAngle(7, -28.0f, 25.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 14.0f, 30.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.enemy1 = new Enepc();
        this.enemy1.init(16407, 5, -10.03f, 0.0f, 5.19f, 90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(3, 3, 4, 4);
        float[] fArray = new float[32];
        fArray[0] = -10.03f;
        fArray[2] = 5.19f;
        fArray[3] = -1.0f;
        fArray[4] = -8.0f;
        fArray[6] = 4.813f;
        fArray[8] = -5.865f;
        fArray[10] = 3.652f;
        fArray[11] = 1.0f;
        fArray[12] = -3.88f;
        fArray[14] = 3.652f;
        fArray[15] = 2.0f;
        fArray[16] = -1.988f;
        fArray[18] = 3.652f;
        fArray[19] = 3.0f;
        fArray[20] = -1.558f;
        fArray[22] = 2.659f;
        fArray[23] = 4.0f;
        fArray[24] = -0.771f;
        fArray[26] = 1.741f;
        fArray[27] = 5.0f;
        fArray[28] = 0.015f;
        fArray[30] = 0.936f;
        fArray[31] = 6.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(2, 9, 1, 5, fArray2);
        this.enemy2 = new Enepc();
        this.enemy2.init(16407, 5, 8.468f, 0.0f, -2.261f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(4, 4, 4, 4);
        float[] fArray3 = new float[16];
        fArray3[0] = 8.468f;
        fArray3[2] = -2.261f;
        fArray3[3] = -1.0f;
        fArray3[4] = 11.218f;
        fArray3[6] = 0.352f;
        fArray3[8] = 8.524f;
        fArray3[10] = 3.398f;
        fArray3[11] = 1.0f;
        fArray3[12] = 7.115f;
        fArray3[14] = 0.663f;
        fArray3[15] = 2.0f;
        float[] fArray4 = fArray3;
        this.enemy2.setParams(1, 7, 2, 5, fArray4);
        this.enemy3 = new Enepc();
        this.enemy3.init(16408, 6, 11.757f, 6.0f, -8.5f, 90.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(5, 5, 5, 5);
        float[] fArray5 = new float[84];
        fArray5[0] = 11.757f;
        fArray5[1] = 6.0f;
        fArray5[2] = -8.5f;
        fArray5[3] = -1.0f;
        fArray5[4] = 9.0f;
        fArray5[5] = 6.0f;
        fArray5[6] = -8.5f;
        fArray5[8] = 7.0f;
        fArray5[9] = 6.0f;
        fArray5[10] = -8.5f;
        fArray5[11] = 1.0f;
        fArray5[12] = 4.5f;
        fArray5[13] = 6.0f;
        fArray5[14] = -8.5f;
        fArray5[15] = 2.0f;
        fArray5[16] = 4.5f;
        fArray5[17] = 6.0f;
        fArray5[18] = -6.0f;
        fArray5[19] = 3.0f;
        fArray5[20] = 4.5f;
        fArray5[21] = 6.0f;
        fArray5[22] = -3.5f;
        fArray5[23] = 4.0f;
        fArray5[24] = 2.0f;
        fArray5[25] = 6.0f;
        fArray5[26] = -3.5f;
        fArray5[27] = 5.0f;
        fArray5[29] = 6.0f;
        fArray5[30] = -3.5f;
        fArray5[31] = 6.0f;
        fArray5[32] = -2.5f;
        fArray5[33] = 6.0f;
        fArray5[34] = -3.5f;
        fArray5[35] = 7.0f;
        fArray5[36] = -3.0f;
        fArray5[37] = 6.0f;
        fArray5[38] = -1.5f;
        fArray5[39] = 8.0f;
        fArray5[40] = -6.0f;
        fArray5[41] = 6.0f;
        fArray5[42] = -1.5f;
        fArray5[43] = 9.0f;
        fArray5[44] = -9.0f;
        fArray5[45] = 6.0f;
        fArray5[46] = -1.5f;
        fArray5[47] = 10.0f;
        fArray5[48] = -11.5f;
        fArray5[49] = 6.0f;
        fArray5[50] = -1.5f;
        fArray5[51] = 11.0f;
        fArray5[52] = 11.757f;
        fArray5[53] = 5.0f;
        fArray5[54] = -6.379f;
        fArray5[56] = 11.757f;
        fArray5[57] = 4.0f;
        fArray5[58] = -4.679f;
        fArray5[59] = 13.0f;
        fArray5[60] = 11.757f;
        fArray5[61] = 3.5f;
        fArray5[62] = -2.679f;
        fArray5[63] = 14.0f;
        fArray5[64] = 11.757f;
        fArray5[65] = 3.0f;
        fArray5[66] = -1.0f;
        fArray5[67] = 15.0f;
        fArray5[68] = 11.218f;
        fArray5[70] = 0.352f;
        fArray5[71] = 16.0f;
        fArray5[72] = 8.524f;
        fArray5[74] = 3.398f;
        fArray5[75] = 17.0f;
        fArray5[76] = 7.115f;
        fArray5[78] = 0.663f;
        fArray5[79] = 18.0f;
        fArray5[80] = 8.468f;
        fArray5[82] = -2.261f;
        fArray5[83] = 19.0f;
        float[] fArray6 = fArray5;
        this.enemy3.setParams(1, 8, 3, 6, fArray6);
        float[] fArray7 = new float[75];
        fArray7[0] = 11.0f;
        fArray7[1] = 6.0f;
        fArray7[2] = -8.5f;
        fArray7[3] = 9.0f;
        fArray7[4] = 6.0f;
        fArray7[5] = -8.5f;
        fArray7[6] = 7.0f;
        fArray7[7] = 6.0f;
        fArray7[8] = -8.5f;
        fArray7[9] = 4.5f;
        fArray7[10] = 6.0f;
        fArray7[11] = -8.5f;
        fArray7[12] = 4.5f;
        fArray7[13] = 6.0f;
        fArray7[14] = -6.0f;
        fArray7[15] = 4.5f;
        fArray7[16] = 6.0f;
        fArray7[17] = -3.5f;
        fArray7[18] = 2.0f;
        fArray7[19] = 6.0f;
        fArray7[20] = -3.5f;
        fArray7[22] = 6.0f;
        fArray7[23] = -3.5f;
        fArray7[24] = -2.5f;
        fArray7[25] = 6.0f;
        fArray7[26] = -3.5f;
        fArray7[27] = -3.0f;
        fArray7[28] = 6.0f;
        fArray7[29] = -1.5f;
        fArray7[30] = -6.0f;
        fArray7[31] = 6.0f;
        fArray7[32] = -1.5f;
        fArray7[33] = -9.0f;
        fArray7[34] = 6.0f;
        fArray7[35] = -1.5f;
        fArray7[36] = -11.5f;
        fArray7[37] = 6.0f;
        fArray7[38] = -1.5f;
        fArray7[39] = -9.0f;
        fArray7[40] = 6.0f;
        fArray7[41] = -1.5f;
        fArray7[42] = -6.0f;
        fArray7[43] = 6.0f;
        fArray7[44] = -1.5f;
        fArray7[45] = -3.0f;
        fArray7[46] = 6.0f;
        fArray7[47] = -1.5f;
        fArray7[48] = -2.5f;
        fArray7[49] = 6.0f;
        fArray7[50] = -3.5f;
        fArray7[52] = 6.0f;
        fArray7[53] = -3.5f;
        fArray7[54] = 2.0f;
        fArray7[55] = 6.0f;
        fArray7[56] = -3.5f;
        fArray7[57] = 4.5f;
        fArray7[58] = 6.0f;
        fArray7[59] = -3.5f;
        fArray7[60] = 4.5f;
        fArray7[61] = 6.0f;
        fArray7[62] = -6.0f;
        fArray7[63] = 4.5f;
        fArray7[64] = 6.0f;
        fArray7[65] = -8.5f;
        fArray7[66] = 7.0f;
        fArray7[67] = 6.0f;
        fArray7[68] = -8.5f;
        fArray7[69] = 9.0f;
        fArray7[70] = 6.0f;
        fArray7[71] = -8.5f;
        fArray7[72] = 11.0f;
        fArray7[73] = 6.0f;
        fArray7[74] = -8.5f;
        float[] fArray8 = fArray7;
        this.enemy3.setParams(fArray8);
        this.trap1 = new Uwamono(28674, 0.0f, 0.0f, 0.0f, 0.0f);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 2);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 5);
        new Uwamono(3, 0);
        this.monsbox = new Uwamono(4, 0, this.enemy1);
        if (this.tansu_check_2F == 0) {
            new Uwamono(13, 20);
            this.tansu = new Uwamono(13, 20);
            this.tansu.SetCallNo(3);
        } else {
            Stage.setVisible(13, false);
        }
        if (Runtime.getFlags(1018, 1) == 0) {
            this.lift = new Uwamono(43, 12);
            this.lift.SetCallNo(2);
        } else {
            Stage.setVisible(43, false);
        }
        new Uwamono(41, 0, this.item1);
        new Uwamono(42, 0, this.item2);
        this.doorA = new Uwamono(40, 42, '\u0001');
        new Uwamono(39, 42, '\u0001', this.doorA);
        this.doorA.SetDiffSize(0.0f, -2.0f, 0.0f);
        this.doorB = new Uwamono(19, 40, '\u0001');
        this.doorC = new Uwamono(21, 40, '\u0001');
        this.doorD = new Uwamono(20, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0004');
        if (Runtime.getFlags(3290, 1) == 0) {
            this.doorE = new Uwamono(60, 40, '\u0004');
            this.doorE.SetDoorType('\u0002');
        } else {
            this.doorE = new Uwamono(60, 40, '\u0004');
            this.doorE.SetDoorType('\u0004');
        }
        this.ef30201a = new Effect(1016, 4);
        this.ef30201b = new Effect(1053, 4);
        this.ef30202a = new Effect(1017, 1);
        this.ef30202b = new Effect(1054, 1);
        this.ef30203 = new Effect(1018, 3);
        this.ef30205a = new Effect(1020, 2);
        this.ef30205b = new Effect(1055, 2);
        this.ef30201a.disp(true);
        this.ef30201b.disp(false);
        this.ef30202a.disp(true);
        this.ef30202b.disp(false);
        this.ef30205a.disp(true);
        this.ef30205b.disp(false);
        this.trap1.setLocation(0, 0);
        this.ef30201a.setLocation(6, 3);
        this.ef30201b.setLocation(6, 3);
        this.ef30202a.setLocation(6, 0);
        this.ef30202b.setLocation(6, 0);
        this.ef30203.setLocation(6, 2);
        this.ef30205a.setLocation(6, 1);
        this.ef30205b.setLocation(6, 1);
        if (this.key == 1) {
            Stage.setVisible(15, false);
            this.ef30203.disp(false);
        }
        if (this.doorcheck_boss == 1) {
            this.doorA.DoorOpen(0);
            this.ef30201a.disp(false);
            this.ef30201b.disp(true);
            this.ef30202a.disp(false);
            this.ef30202b.disp(true);
        }
        if (this.doorcheck_2F == 1) {
            this.doorC.SetDoorType('\u0004');
            this.ef30205a.disp(false);
            this.ef30205b.disp(true);
        }
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
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
}

