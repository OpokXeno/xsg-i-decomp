import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_UTK04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1049
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK04_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Uwamono doorA;
    Uwamono doorB_1;
    Uwamono doorB_2;
    Uwamono door_2;
    Uwamono door_3;
    MAPUnit obj_01;
    Uwamono item2;
    Uwamono item3;
    Uwamono item5;
    Uwamono item7;
    Uwamono item9;
    Uwamono item11;
    Effect fade;
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
    Light light = new Light(0);
    boolean EnterCheck = false;
    int selected = 0;
    int page;
    int b_flg;
    String[] EVS = new String[]{"Exit the EVS (Environmental Simulator)?", "/[waitkey(64)]/[close()]"};

    ST1049() {
    }

    void EOB(int n) {
        System.println("EOB**********");
    }

    void Final_init(int n) {
        System.println("Final_init !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
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
                Runtime.jumpCF(1059, 1);
                break;
            }
        }
    }

    void evsExit() {
        System.println("evsExitをコールしました");
        if (this.b_flg == 1) {
            return;
        }
        this.b_flg = 1;
        Runtime.enable(262144);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.EVS, 0);
        ST1049.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
                Runtime.evsExit();
                return;
            }
        }
        Runtime.setPlayerControl(true);
        Runtime.disable(262144);
        this.b_flg = 0;
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
        this.player.setID(2);
        this.obj_01 = new MAPUnit();
        this.obj_01.mapUnit(26);
        this.obj_01.start(4, null);
        this.obj_01.getTranslate();
        this.obj_01.setTranslate(this.obj_01.px, this.obj_01.py, this.obj_01.pz + 2.5f);
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
        Stage.setVisible(135, false);
        Stage.setVisible(134, false);
        Stage.setVisible(132, false);
        Stage.setVisible(133, false);
        Stage.setVisible(136, false);
        Stage.setVisible(137, false);
        Stage.setVisible(138, false);
        Stage.setVisible(139, false);
        this.doorA = new Uwamono(38, 42, '\u0001');
        new Uwamono(37, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0002');
        this.doorA.SetDoorRange(2.0f);
        this.doorA.SetDoorSpd(0);
        this.doorA.DoorOpen();
        this.doorB_1 = new Uwamono(186, 40, '\u0001');
        this.doorB_1.SetDoorType('\u0002');
        this.doorB_1.SetDoorRange(2.0f);
        this.doorB_1.SetDoorSpd(0);
        this.doorB_1.DoorOpen();
        this.doorB_2 = new Uwamono(187, 40, '\u0001');
        this.doorB_2.SetDoorType('\u0002');
        this.doorB_2.SetDoorRange(2.0f);
        this.doorB_2.SetDoorSpd(0);
        this.doorB_2.DoorOpen();
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
        new Uwamono(194, 9);
        new Uwamono(195, 9);
        new Uwamono(36, 20);
        new Uwamono(28734, -13.0f, 0.0f, -40.0f);
        System.println("ENEMY初期化");
        System.println("元・潜伏していた兵士初期化");
        this.enemy5 = new Enepc();
        this.enemy5.init(17153, 3, 18.0f, 0.0f, -1.0f, 180.0f);
        this.enemy5.id = 5;
        this.enemy5.enableDTKFlag(262144);
        float[] fArray = new float[16];
        fArray[0] = 18.0f;
        fArray[2] = -1.0f;
        fArray[3] = -1.0f;
        fArray[4] = 19.0f;
        fArray[6] = -6.5f;
        fArray[8] = 20.0f;
        fArray[10] = -14.0f;
        fArray[11] = 1.0f;
        fArray[12] = 2.0f;
        fArray[14] = -6.5f;
        fArray[15] = 1.0f;
        float[] fArray2 = fArray;
        this.enemy5.setGroup(1, 1, 1, 1);
        this.enemy5.setParams(1, 1, 5, 3, fArray2);
        System.println("元・陰に隠れていた兵士初期化");
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
        float[] fArray4 = fArray3;
        float[] fArray5 = new float[54];
        fArray5[0] = 18.0f;
        fArray5[2] = 17.0f;
        fArray5[3] = 20.0f;
        fArray5[5] = 17.0f;
        fArray5[6] = 20.0f;
        fArray5[8] = 21.0f;
        fArray5[9] = 13.0f;
        fArray5[11] = 21.0f;
        fArray5[12] = 6.0f;
        fArray5[14] = 21.0f;
        fArray5[15] = 6.0f;
        fArray5[17] = 16.5f;
        fArray5[18] = 6.0f;
        fArray5[20] = 11.0f;
        fArray5[21] = 6.0f;
        fArray5[23] = 16.5f;
        fArray5[24] = 6.0f;
        fArray5[26] = 21.0f;
        fArray5[27] = 13.0f;
        fArray5[29] = 21.0f;
        fArray5[30] = 20.0f;
        fArray5[32] = 21.0f;
        fArray5[33] = 20.0f;
        fArray5[35] = 16.0f;
        fArray5[36] = 20.0f;
        fArray5[38] = 11.0f;
        fArray5[39] = 20.0f;
        fArray5[41] = 6.0f;
        fArray5[42] = 20.0f;
        fArray5[44] = 4.0f;
        fArray5[45] = 20.0f;
        fArray5[47] = 9.0f;
        fArray5[48] = 20.0f;
        fArray5[50] = 17.0f;
        fArray5[51] = 18.0f;
        fArray5[53] = 17.0f;
        float[] fArray6 = fArray5;
        float[] fArray7 = new float[120];
        fArray7[0] = 22.15f;
        fArray7[2] = 17.0f;
        fArray7[3] = -1.0f;
        fArray7[4] = 22.15f;
        fArray7[6] = 19.0f;
        fArray7[8] = 20.75f;
        fArray7[10] = 17.0f;
        fArray7[12] = 20.75f;
        fArray7[14] = 17.0f;
        fArray7[15] = 2.0f;
        fArray7[16] = 20.75f;
        fArray7[18] = 15.0f;
        fArray7[19] = 3.0f;
        fArray7[20] = 20.75f;
        fArray7[22] = 13.0f;
        fArray7[23] = 4.0f;
        fArray7[24] = 20.75f;
        fArray7[26] = 11.0f;
        fArray7[27] = 5.0f;
        fArray7[28] = 22.15f;
        fArray7[30] = 21.25f;
        fArray7[31] = 1.0f;
        fArray7[32] = 22.15f;
        fArray7[34] = 22.5f;
        fArray7[35] = 7.0f;
        fArray7[36] = 20.0f;
        fArray7[38] = 22.5f;
        fArray7[39] = 8.0f;
        fArray7[40] = 18.0f;
        fArray7[42] = 22.5f;
        fArray7[43] = 9.0f;
        fArray7[44] = 16.0f;
        fArray7[46] = 22.5f;
        fArray7[47] = 10.0f;
        fArray7[48] = 14.0f;
        fArray7[50] = 22.5f;
        fArray7[51] = 11.0f;
        fArray7[52] = 13.0f;
        fArray7[54] = 22.5f;
        fArray7[55] = 12.0f;
        fArray7[56] = 12.0f;
        fArray7[58] = 20.0f;
        fArray7[59] = 13.0f;
        fArray7[60] = 11.0f;
        fArray7[62] = 19.5f;
        fArray7[63] = 14.0f;
        fArray7[64] = 10.0f;
        fArray7[66] = 20.0f;
        fArray7[67] = 15.0f;
        fArray7[68] = 9.75f;
        fArray7[70] = 22.5f;
        fArray7[71] = 16.0f;
        fArray7[72] = 9.75f;
        fArray7[74] = 22.5f;
        fArray7[75] = 17.0f;
        fArray7[76] = 7.2f;
        fArray7[78] = 22.5f;
        fArray7[79] = 18.0f;
        fArray7[80] = 5.75f;
        fArray7[82] = 22.5f;
        fArray7[83] = 19.0f;
        fArray7[84] = 4.25f;
        fArray7[86] = 22.5f;
        fArray7[87] = 20.0f;
        fArray7[88] = 4.25f;
        fArray7[90] = 20.25f;
        fArray7[91] = 21.0f;
        fArray7[92] = 4.25f;
        fArray7[94] = 18.0f;
        fArray7[95] = 22.0f;
        fArray7[96] = 4.25f;
        fArray7[98] = 15.75f;
        fArray7[99] = 23.0f;
        fArray7[100] = 4.25f;
        fArray7[102] = 13.5f;
        fArray7[103] = 24.0f;
        fArray7[104] = 4.25f;
        fArray7[106] = 11.0f;
        fArray7[107] = 25.0f;
        fArray7[108] = 6.0f;
        fArray7[110] = 8.5f;
        fArray7[111] = 26.0f;
        fArray7[112] = 6.875f;
        fArray7[114] = 9.0f;
        fArray7[115] = 27.0f;
        fArray7[116] = 7.75f;
        fArray7[118] = 9.5f;
        fArray7[119] = 28.0f;
        float[] fArray8 = fArray7;
        System.println("enemy 3 定義");
        this.enemy3 = new Enepc();
        this.enemy3.init(17153, 3, 18.0f, 0.0f, 17.0f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(0, 0, 1, 1);
        this.enemy3.enableDTKFlag(262144);
        this.enemy3.setParams(2, 4, 3, 3, fArray4);
        this.enemy3.setParams(fArray6);
        System.println("enemy 4 定義");
        this.enemy4 = new Enepc();
        this.enemy4.init(17153, 3, 22.0f, 0.0f, 17.0f, 0.0f);
        this.enemy4.id = 4;
        this.enemy4.setGroup(0, 0, 1, 1);
        this.enemy4.enableDTKFlag(262144);
        this.enemy4.setParams(2, 3, 4, 3, fArray8);
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

