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
import xeno.map.MC_GNU11_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1510
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNU11_PRJ {
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
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Unit unit1;
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
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono doorA;
    Uwamono box01;
    Uwamono box02;
    Uwamono box03;
    Uwamono box04;
    Uwamono tA;
    Uwamono tB;
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
    Effect light17;
    Effect light18;
    Effect light19;
    Effect light20;
    Effect hokori01;
    MAPUnit eleh;
    MAPUnit elel;
    MAPUnit elel2;
    MAPUnit elel3;
    MAPUnit kaiten;
    MAPUnit locked;
    MAPUnit open;
    Unit elv;
    int eleh_move = 1;
    int elel_move = 0;
    int elel_ichi = 0;
    int kaiten_move = 0;
    boolean lo2 = false;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Effect fade;
    int page;
    String[] mj = new String[]{"Go up a level?", "/[waitkey(64)]/[close()]"};
    String[] elel_down = new String[]{"Operate elevator?", "/[waitkey(64)]/[close()]"};
    String[] elel_up = new String[]{"Operate elevator?", "/[waitkey(64)]/[close()]"};
    String[] kaitenban = new String[]{"There's a button. Press it?", "/[waitkey(64)]/[close()]"};

    ST1510() {
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
                System.println("回転盤");
                if (Runtime.getFlags(8019, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.nwin(this.kaitenban);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        Sound.streamPlay(1195002, 48000);
                        Runtime.enable(65536);
                        this.player.rotY(10, 180.0f, true);
                        System.sleep(10);
                        this.player.mtn(25, 1, 1.0f, true);
                        System.sleep(40);
                        System.sleep(20);
                        this.kaiten_move = 1;
                        this.lo = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.lo = 0;
                return;
            }
            case 1: {
                if (this.lo2) {
                    Runtime.setPlayerControl(false);
                    this.lo = 1;
                    System.println("上のエレベータ");
                    this.nwin(this.mj);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            Runtime.enable(65536);
                            this.player.rotY(10, 0.0f, true);
                            System.sleep(10);
                            this.eleh_move = 2;
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                return;
            }
            case 2: {
                if (this.elel_move != 0) break;
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("下のエレベータ");
                this.player.getTranslate();
                if (this.player.py >= 127.0f) {
                    this.elel_ichi = 0;
                }
                if (this.elel_ichi == 0) {
                    this.nwin(this.elel_down);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            this.elel_move = 1;
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                if (this.elel_ichi == 1) {
                    System.println("11111111111111111111111111111111111111");
                    this.nwin(this.elel_up);
                    this.yesno();
                    switch (this.selected) {
                        case 0: {
                            this.elel_move = 2;
                            this.lo = 0;
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.lo = 0;
                    return;
                }
                System.println("どこにいるの？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？");
                Runtime.setPlayerControl(true);
                this.lo = 0;
            }
            case 3: {
                System.println("lo2 = true");
                this.lo2 = true;
                break;
            }
            case 4: {
                this.cam0.setMode(0);
                break;
            }
            case 5: {
                this.cam0.setMode(0);
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
                if (Runtime.getFlags(175, 1) == 0) {
                    Runtime.setFlags(175, 1, 1);
                    System.println("イベント2054:ボスバトル");
                    Runtime.jumpEvent(2540);
                    break;
                }
                Runtime.jumpCF(1529, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 0.0f, 127.5f, -24.0f, 0.0f);
        this.teiten1.SetBgm(196620);
        this.teiten2 = new Uwamono(28690, 0.0f, 127.6f, 7.5f, 0.0f);
        this.teiten2.SetBgm(196622);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, -17.4f, 0.0f);
        this.teiten3.SetBgm(196623);
        this.teiten4 = new Uwamono(28690, 0.0f, 127.5f, 15.4f, 0.0f);
        this.teiten4.SetBgm(196621);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 4.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -4.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.hokori01 = new Effect(1490, 0.0f, -3.5f, -19.1f, 0.0f);
        this.hokori01.noAttach(false);
        this.hokori01.disp(false);
        this.light01 = new Effect(1539, 3.73f, 1.28f, -12.5f, 0.0f);
        this.light01.noAttach(false);
        this.light01.setRotate(90.0f, 0.0f, 0.0f);
        this.light01.setScale(0.3f, 0.3f, 0.3f);
        this.light01.disp(false);
        this.light02 = new Effect(1499, 0.0f, 128.5f, 6.0f, 0.0f);
        this.light02.noAttach(false);
        this.light02.disp(false);
        this.light04 = new Effect(1498, 0.0f, 162.9f, -25.5f, 0.0f);
        this.light04.noAttach(false);
        this.light04.disp(true);
        this.light05 = new Effect(1716, 3.65f, 1.97f, -13.6f, 0.0f);
        this.light05.setScale(1.05f, 0.5f, 1.0f);
        this.light05.disp(true);
        float f = 2.3f;
        this.light06 = new Effect(1603, -2.3f, f, 3.7f, 0.0f);
        this.light06.setScale(1.0f, 3.0f, 1.0f);
        this.light07 = new Effect(1604, -2.3f, f, 3.7f, 0.0f);
        this.light07.setScale(1.0f, 3.0f, 1.0f);
        this.light08 = new Effect(1603, -2.3f, f, 8.4f, 0.0f);
        this.light08.setScale(1.0f, 3.0f, 1.0f);
        this.light09 = new Effect(1604, -2.3f, f, 8.4f, 0.0f);
        this.light09.setScale(1.0f, 3.0f, 1.0f);
        this.light10 = new Effect(1603, 2.4f, f, 3.7f, 0.0f);
        this.light10.setScale(1.0f, 3.0f, 1.0f);
        this.light11 = new Effect(1604, 2.4f, f, 3.7f, 0.0f);
        this.light11.setScale(1.0f, 3.0f, 1.0f);
        this.light12 = new Effect(1603, 2.4f, f, 8.4f, 0.0f);
        this.light12.setScale(1.0f, 3.0f, 1.0f);
        this.light13 = new Effect(1604, 2.4f, f, 8.4f, 0.0f);
        this.light13.setScale(1.0f, 3.0f, 1.0f);
        this.light14 = new Effect(1679, 0.0f, 127.5f, 6.0f, 0.0f);
        this.light14.noAttach(false);
        this.light14.disp(false);
        this.light19 = new Effect(1733, 0.0f, 127.5f, 6.0f, 0.0f);
        this.light19.noAttach(false);
        this.light19.disp(false);
        this.light15 = new Effect(1680, -4.6f, 21.2f, -18.0f, 0.0f);
        this.light15.noAttach(false);
        this.light15.disp(false);
        this.light16 = new Effect(1680, -11.3f, 6.2f, -18.0f, 0.0f);
        this.light16.noAttach(false);
        this.light16.disp(false);
        this.light17 = new Effect(1680, 8.5f, 1.4f, -18.0f, 0.0f);
        this.light17.noAttach(false);
        this.light17.disp(false);
        this.light18 = new Effect(1680, 11.3f, 14.4f, -18.0f, 0.0f);
        this.light18.noAttach(false);
        this.light18.disp(false);
        this.light20 = new Effect(1732, 0.0f, 0.0f, -17.0f, 0.0f);
        this.light20.noAttach(false);
        this.light20.disp(false);
        this.light20.setScale(3.0f, 3.0f, 3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.player.setID(2);
        this.cam0.setFog(0, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(1, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(2, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(3, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(4, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(5, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(6, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(7, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(8, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(9, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam0.setFog(10, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(0, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(1, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(2, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(3, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(4, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(5, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(6, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(7, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(8, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(9, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        this.cam1.setFog(10, 10.0f, 100.0f, 0.1f, 0.8f, 50, 50, 50, 0);
        Runtime.setDefocusQuick(0, 1, 1500, 1);
        Runtime.setDefocusQuick(1, 1, 2000, 1);
        Runtime.setDefocusQuick(2, 1, 2500, 1);
        Runtime.setDefocusQuick(3, 1, 3000, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, 0.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 0.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestal(4, -20.55f, 141.34f, 14.25f, 40.0f, -65.8f, 345.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(4, 1);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 0.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestal(7, 20.75f, 141.34f, 14.25f, 40.0f, -65.8f, 15.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.cam0.setCFPedestal(8, 4.4f, 0.144f, -4.105f, 60.0f, 28.25f, 16.22f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFPedestal(9, 0.0f, 132.5f, 19.806631f, 40.0f, -23.473303f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(9, 1);
        this.cam0.setCFPedestal(10, 0.0f, 4.6967688f, 15.169842f, 40.0f, -15.803791f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(10, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(10, 1);
        float[] fArray = new float[108];
        fArray[0] = -2.7f;
        fArray[1] = 127.6f;
        fArray[2] = -8.7f;
        fArray[3] = -1.0f;
        fArray[4] = -6.4f;
        fArray[5] = 127.6f;
        fArray[6] = -7.4f;
        fArray[8] = -9.4f;
        fArray[9] = 127.6f;
        fArray[10] = -5.57f;
        fArray[11] = 1.0f;
        fArray[12] = -11.85f;
        fArray[13] = 127.6f;
        fArray[14] = -3.46f;
        fArray[15] = 2.0f;
        fArray[16] = -13.65f;
        fArray[17] = 127.6f;
        fArray[18] = -0.3f;
        fArray[19] = 3.0f;
        fArray[20] = -14.85f;
        fArray[21] = 127.6f;
        fArray[22] = 3.33f;
        fArray[23] = 4.0f;
        fArray[24] = -14.85f;
        fArray[25] = 127.6f;
        fArray[26] = 6.78f;
        fArray[27] = 5.0f;
        fArray[28] = -14.25f;
        fArray[29] = 127.6f;
        fArray[30] = 11.6f;
        fArray[31] = 6.0f;
        fArray[32] = -12.57f;
        fArray[33] = 127.6f;
        fArray[34] = 14.78f;
        fArray[35] = 7.0f;
        fArray[36] = -10.54f;
        fArray[37] = 127.6f;
        fArray[38] = 16.72f;
        fArray[39] = 8.0f;
        fArray[40] = -8.27f;
        fArray[41] = 127.6f;
        fArray[42] = 18.9f;
        fArray[43] = 9.0f;
        fArray[44] = -5.23f;
        fArray[45] = 127.6f;
        fArray[46] = 20.19f;
        fArray[47] = 10.0f;
        fArray[48] = -2.33f;
        fArray[49] = 127.6f;
        fArray[50] = 20.99f;
        fArray[51] = 11.0f;
        fArray[52] = 2.36f;
        fArray[53] = 127.6f;
        fArray[54] = 20.99f;
        fArray[55] = 12.0f;
        fArray[56] = 6.39f;
        fArray[57] = 127.6f;
        fArray[58] = 19.76f;
        fArray[59] = 13.0f;
        fArray[60] = 9.18f;
        fArray[61] = 127.6f;
        fArray[62] = 18.16f;
        fArray[63] = 14.0f;
        fArray[64] = 11.31f;
        fArray[65] = 127.6f;
        fArray[66] = 15.9f;
        fArray[67] = 15.0f;
        fArray[68] = 13.11f;
        fArray[69] = 127.6f;
        fArray[70] = 13.4f;
        fArray[71] = 16.0f;
        fArray[72] = 14.62f;
        fArray[73] = 127.6f;
        fArray[74] = 9.83f;
        fArray[75] = 17.0f;
        fArray[76] = 14.87f;
        fArray[77] = 127.6f;
        fArray[78] = 5.79f;
        fArray[79] = 18.0f;
        fArray[80] = 14.17f;
        fArray[81] = 127.6f;
        fArray[82] = 1.62f;
        fArray[83] = 19.0f;
        fArray[84] = 13.02f;
        fArray[85] = 127.6f;
        fArray[86] = -1.47f;
        fArray[87] = 20.0f;
        fArray[88] = 10.86f;
        fArray[89] = 127.6f;
        fArray[90] = -3.94f;
        fArray[91] = 21.0f;
        fArray[92] = 8.59f;
        fArray[93] = 127.6f;
        fArray[94] = -5.99f;
        fArray[95] = 22.0f;
        fArray[96] = 5.45f;
        fArray[97] = 127.6f;
        fArray[98] = -7.79f;
        fArray[99] = 23.0f;
        fArray[100] = 2.88f;
        fArray[101] = 127.6f;
        fArray[102] = -8.29f;
        fArray[103] = 24.0f;
        fArray[104] = 0.14f;
        fArray[105] = 127.6f;
        fArray[106] = -9.11f;
        fArray[107] = 25.0f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16398, 1, 1, 7, 3, -2.7f, 127.6f, -8.7f, 90.0f, fArray2);
        float[] fArray3 = new float[]{-2.7f, 127.6f, -8.7f, 0.14f, 127.6f, -9.11f, 2.88f, 127.6f, -8.29f, 5.45f, 127.6f, -7.79f, 8.59f, 127.6f, -5.99f, 10.86f, 127.6f, -3.94f, 13.02f, 127.6f, -1.47f, 14.17f, 127.6f, 1.62f, 14.87f, 127.6f, 5.79f, 14.62f, 127.6f, 9.83f, 13.11f, 127.6f, 13.4f, 11.31f, 127.6f, 15.9f, 9.18f, 127.6f, 18.16f, 6.39f, 127.6f, 19.76f, 2.36f, 127.6f, 20.99f, -2.33f, 127.6f, 20.99f, -5.23f, 127.6f, 20.19f, -8.27f, 127.6f, 18.9f, -10.54f, 127.6f, 16.72f, -12.57f, 127.6f, 14.78f, -14.25f, 127.6f, 11.6f, -14.85f, 127.6f, 6.78f, -14.85f, 127.6f, 3.33f, -13.65f, 127.6f, -0.3f, -11.85f, 127.6f, -3.46f, -9.4f, 127.6f, -5.57f, -6.4f, 127.6f, -7.4f};
        this.enemy1.setParams(fArray3);
        this.enemy1.setGroup(0, 1, 1, 2);
        float[] fArray4 = new float[108];
        fArray4[0] = 13.02f;
        fArray4[1] = 127.6f;
        fArray4[2] = -1.47f;
        fArray4[3] = -1.0f;
        fArray4[4] = 10.86f;
        fArray4[5] = 127.6f;
        fArray4[6] = -3.94f;
        fArray4[8] = 8.59f;
        fArray4[9] = 127.6f;
        fArray4[10] = -5.99f;
        fArray4[11] = 1.0f;
        fArray4[12] = 5.45f;
        fArray4[13] = 127.6f;
        fArray4[14] = -7.79f;
        fArray4[15] = 2.0f;
        fArray4[16] = 2.88f;
        fArray4[17] = 127.6f;
        fArray4[18] = -8.29f;
        fArray4[19] = 3.0f;
        fArray4[20] = 0.14f;
        fArray4[21] = 127.6f;
        fArray4[22] = -9.11f;
        fArray4[23] = 4.0f;
        fArray4[24] = -2.7f;
        fArray4[25] = 127.6f;
        fArray4[26] = -8.7f;
        fArray4[27] = 5.0f;
        fArray4[28] = -6.4f;
        fArray4[29] = 127.6f;
        fArray4[30] = -7.4f;
        fArray4[31] = 6.0f;
        fArray4[32] = -9.4f;
        fArray4[33] = 127.6f;
        fArray4[34] = -5.57f;
        fArray4[35] = 7.0f;
        fArray4[36] = -11.85f;
        fArray4[37] = 127.6f;
        fArray4[38] = -3.46f;
        fArray4[39] = 8.0f;
        fArray4[40] = -13.65f;
        fArray4[41] = 127.6f;
        fArray4[42] = -0.3f;
        fArray4[43] = 9.0f;
        fArray4[44] = -14.85f;
        fArray4[45] = 127.6f;
        fArray4[46] = 3.33f;
        fArray4[47] = 10.0f;
        fArray4[48] = -14.85f;
        fArray4[49] = 127.6f;
        fArray4[50] = 6.78f;
        fArray4[51] = 11.0f;
        fArray4[52] = -14.25f;
        fArray4[53] = 127.6f;
        fArray4[54] = 11.6f;
        fArray4[55] = 12.0f;
        fArray4[56] = -12.57f;
        fArray4[57] = 127.6f;
        fArray4[58] = 14.78f;
        fArray4[59] = 13.0f;
        fArray4[60] = -10.54f;
        fArray4[61] = 127.6f;
        fArray4[62] = 16.72f;
        fArray4[63] = 14.0f;
        fArray4[64] = -8.27f;
        fArray4[65] = 127.6f;
        fArray4[66] = 18.9f;
        fArray4[67] = 15.0f;
        fArray4[68] = -5.23f;
        fArray4[69] = 127.6f;
        fArray4[70] = 20.19f;
        fArray4[71] = 16.0f;
        fArray4[72] = -2.33f;
        fArray4[73] = 127.6f;
        fArray4[74] = 20.99f;
        fArray4[75] = 17.0f;
        fArray4[76] = 2.36f;
        fArray4[77] = 127.6f;
        fArray4[78] = 20.99f;
        fArray4[79] = 18.0f;
        fArray4[80] = 6.39f;
        fArray4[81] = 127.6f;
        fArray4[82] = 19.76f;
        fArray4[83] = 19.0f;
        fArray4[84] = 9.18f;
        fArray4[85] = 127.6f;
        fArray4[86] = 18.16f;
        fArray4[87] = 20.0f;
        fArray4[88] = 11.31f;
        fArray4[89] = 127.6f;
        fArray4[90] = 15.9f;
        fArray4[91] = 21.0f;
        fArray4[92] = 13.11f;
        fArray4[93] = 127.6f;
        fArray4[94] = 13.4f;
        fArray4[95] = 22.0f;
        fArray4[96] = 14.62f;
        fArray4[97] = 127.6f;
        fArray4[98] = 9.83f;
        fArray4[99] = 23.0f;
        fArray4[100] = 14.87f;
        fArray4[101] = 127.6f;
        fArray4[102] = 5.79f;
        fArray4[103] = 24.0f;
        fArray4[104] = 14.17f;
        fArray4[105] = 127.6f;
        fArray4[106] = 1.62f;
        fArray4[107] = 25.0f;
        float[] fArray5 = fArray4;
        this.enemy2 = new NpcEnemy(16398, 2, 1, 7, 3, 13.02f, 127.6f, -1.47f, 180.0f, fArray5);
        float[] fArray6 = new float[]{13.02f, 127.6f, -1.47f, 14.17f, 127.6f, 1.62f, 14.87f, 127.6f, 5.79f, 14.62f, 127.6f, 9.83f, 13.11f, 127.6f, 13.4f, 11.31f, 127.6f, 15.9f, 9.18f, 127.6f, 18.16f, 6.39f, 127.6f, 19.76f, 2.36f, 127.6f, 20.99f, -2.33f, 127.6f, 20.99f, -5.23f, 127.6f, 20.19f, -8.27f, 127.6f, 18.9f, -10.54f, 127.6f, 16.72f, -12.57f, 127.6f, 14.78f, -14.25f, 127.6f, 11.6f, -14.85f, 127.6f, 6.78f, -14.85f, 127.6f, 3.33f, -13.65f, 127.6f, -0.3f, -11.85f, 127.6f, -3.46f, -9.4f, 127.6f, -5.57f, -6.4f, 127.6f, -7.4f, -2.7f, 127.6f, -8.7f, 0.14f, 127.6f, -9.11f, 2.88f, 127.6f, -8.29f, 5.45f, 127.6f, -7.79f, 8.59f, 127.6f, -5.99f, 10.86f, 127.6f, -3.94f};
        this.enemy2.setParams(fArray6);
        this.enemy2.setGroup(0, 1, 1, 2);
        float[] fArray7 = new float[108];
        fArray7[0] = -12.57f;
        fArray7[1] = 127.6f;
        fArray7[2] = 14.78f;
        fArray7[3] = -1.0f;
        fArray7[4] = -10.54f;
        fArray7[5] = 127.6f;
        fArray7[6] = 16.72f;
        fArray7[8] = -8.27f;
        fArray7[9] = 127.6f;
        fArray7[10] = 18.9f;
        fArray7[11] = 1.0f;
        fArray7[12] = -5.23f;
        fArray7[13] = 127.6f;
        fArray7[14] = 20.19f;
        fArray7[15] = 2.0f;
        fArray7[16] = -2.33f;
        fArray7[17] = 127.6f;
        fArray7[18] = 20.99f;
        fArray7[19] = 3.0f;
        fArray7[20] = 2.36f;
        fArray7[21] = 127.6f;
        fArray7[22] = 20.99f;
        fArray7[23] = 4.0f;
        fArray7[24] = 6.39f;
        fArray7[25] = 127.6f;
        fArray7[26] = 19.76f;
        fArray7[27] = 5.0f;
        fArray7[28] = 9.18f;
        fArray7[29] = 127.6f;
        fArray7[30] = 18.16f;
        fArray7[31] = 6.0f;
        fArray7[32] = 11.31f;
        fArray7[33] = 127.6f;
        fArray7[34] = 15.9f;
        fArray7[35] = 7.0f;
        fArray7[36] = 13.11f;
        fArray7[37] = 127.6f;
        fArray7[38] = 13.4f;
        fArray7[39] = 8.0f;
        fArray7[40] = 14.62f;
        fArray7[41] = 127.6f;
        fArray7[42] = 9.83f;
        fArray7[43] = 9.0f;
        fArray7[44] = 14.87f;
        fArray7[45] = 127.6f;
        fArray7[46] = 5.79f;
        fArray7[47] = 10.0f;
        fArray7[48] = 14.17f;
        fArray7[49] = 127.6f;
        fArray7[50] = 1.62f;
        fArray7[51] = 11.0f;
        fArray7[52] = 13.02f;
        fArray7[53] = 127.6f;
        fArray7[54] = -1.47f;
        fArray7[55] = 12.0f;
        fArray7[56] = 10.86f;
        fArray7[57] = 127.6f;
        fArray7[58] = -3.94f;
        fArray7[59] = 13.0f;
        fArray7[60] = 8.59f;
        fArray7[61] = 127.6f;
        fArray7[62] = -5.99f;
        fArray7[63] = 14.0f;
        fArray7[64] = 5.45f;
        fArray7[65] = 127.6f;
        fArray7[66] = -7.79f;
        fArray7[67] = 15.0f;
        fArray7[68] = 2.88f;
        fArray7[69] = 127.6f;
        fArray7[70] = -8.29f;
        fArray7[71] = 16.0f;
        fArray7[72] = 0.14f;
        fArray7[73] = 127.6f;
        fArray7[74] = -9.11f;
        fArray7[75] = 17.0f;
        fArray7[76] = -2.7f;
        fArray7[77] = 127.6f;
        fArray7[78] = -8.7f;
        fArray7[79] = 18.0f;
        fArray7[80] = -6.4f;
        fArray7[81] = 127.6f;
        fArray7[82] = -7.4f;
        fArray7[83] = 19.0f;
        fArray7[84] = -9.4f;
        fArray7[85] = 127.6f;
        fArray7[86] = -5.57f;
        fArray7[87] = 20.0f;
        fArray7[88] = -11.85f;
        fArray7[89] = 127.6f;
        fArray7[90] = -3.46f;
        fArray7[91] = 21.0f;
        fArray7[92] = -13.65f;
        fArray7[93] = 127.6f;
        fArray7[94] = -0.3f;
        fArray7[95] = 22.0f;
        fArray7[96] = -14.85f;
        fArray7[97] = 127.6f;
        fArray7[98] = 3.33f;
        fArray7[99] = 23.0f;
        fArray7[100] = -14.85f;
        fArray7[101] = 127.6f;
        fArray7[102] = 6.78f;
        fArray7[103] = 24.0f;
        fArray7[104] = -14.25f;
        fArray7[105] = 127.6f;
        fArray7[106] = 11.6f;
        fArray7[107] = 25.0f;
        float[] fArray8 = fArray7;
        this.enemy3 = new NpcEnemy(16398, 3, 1, 7, 3, -12.57f, 127.6f, 14.78f, 180.0f, fArray8);
        float[] fArray9 = new float[]{-12.57f, 127.6f, 14.78f, -14.25f, 127.6f, 11.6f, -14.85f, 127.6f, 6.78f, -14.85f, 127.6f, 3.33f, -13.65f, 127.6f, -0.3f, -11.85f, 127.6f, -3.46f, -9.4f, 127.6f, -5.57f, -6.4f, 127.6f, -7.4f, -2.7f, 127.6f, -8.7f, 0.14f, 127.6f, -9.11f, 2.88f, 127.6f, -8.29f, 5.45f, 127.6f, -7.79f, 8.59f, 127.6f, -5.99f, 10.86f, 127.6f, -3.94f, 13.02f, 127.6f, -1.47f, 14.17f, 127.6f, 1.62f, 14.87f, 127.6f, 5.79f, 14.62f, 127.6f, 9.83f, 13.11f, 127.6f, 13.4f, 11.31f, 127.6f, 15.9f, 9.18f, 127.6f, 18.16f, 6.39f, 127.6f, 19.76f, 2.36f, 127.6f, 20.99f, -2.33f, 127.6f, 20.99f, -5.23f, 127.6f, 20.19f, -8.27f, 127.6f, 18.9f, -10.54f, 127.6f, 16.72f};
        this.enemy3.setParams(fArray9);
        this.enemy3.setGroup(0, 1, 1, 2);
        this.eleh = new Mapunits();
        this.eleh.mapUnit(146);
        this.eleh.start(4, null);
        this.eleh.setTranslate(this.eleh.px, 160.0f, this.eleh.pz);
        this.elel = new Mapunits();
        this.elel.mapUnit(148);
        this.elel.start(4, null);
        this.elel.setTranslate(this.elel.px, 127.5f, this.elel.pz);
        this.elel2 = new Mapunits();
        this.elel2.mapUnit(147);
        this.elel2.start(4, null);
        this.elel2.setTranslate(this.elel2.px, 128.1f, this.elel2.pz);
        this.elel3 = new Mapunits();
        this.elel3.mapUnit(149);
        this.elel3.start(4, null);
        this.elel3.setTranslate(this.elel3.px, 127.5f, this.elel3.pz);
        this.elv = new Unit();
        this.elv.initElevator(148, 100.1f, 127.5f);
        this.elv.setArgs(12, 127.6f);
        this.kaiten = new Mapunits();
        this.kaiten.mapUnit(158);
        this.kaiten.start(4, null);
        this.locked = new Mapunits();
        this.locked.mapUnit(151);
        this.locked.start(4, null);
        this.open = new Mapunits();
        this.open.mapUnit(150);
        this.open.start(4, null);
        this.open.getTranslate();
        this.open.setTranslate(this.open.px, this.open.py, this.open.pz - 0.01f);
        this.eleh.start(1, "idle");
        this.item1 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 190);
        this.box01 = new Uwamono(76, 20);
        this.box02 = new Uwamono(90, 20);
        this.box03 = new Uwamono(157, 1);
        this.box04 = new Uwamono(159, 1, this.item1);
        this.tA = new Uwamono(28672, 0.0f, 126.5f, 8.12f, 0.0f);
        this.tA.SetSize(3.0f, 1.0f, 0.7f);
        this.tB = new Uwamono(28672, 0.0f, 127.0f, 4.0f, 0.0f);
        this.tB.SetSize(3.0f, 1.0f, 0.6f);
        Stage.setVisible(96, false);
        Stage.setVisible(95, false);
        this.doorA = new Uwamono(144, 40, '\u0001');
        new Uwamono(145, 40, '\u0002', this.doorA);
        this.doorA.SetDoorType('\u0002');
        if (Runtime.getFlags(8017, 1) == 1) {
            System.println("1520から来た");
            this.lo2 = true;
            this.elel_ichi = 1;
            this.eleh.getTranslate();
            this.elel.getTranslate();
            this.elel2.getTranslate();
            this.elel3.getTranslate();
            this.light02.getTranslate();
            this.tA.getTranslate();
            this.tB.getTranslate();
            this.elel.setTranslate(this.elel.px, this.elel.py - 127.5f, this.elel.pz);
            this.elel2.setTranslate(this.elel2.px, this.elel2.py - 127.5f - 0.6f, this.elel2.pz);
            this.elel3.setTranslate(this.elel3.px, this.elel3.py - 127.5f + 0.6f, this.elel3.pz);
            this.light02.setTranslate(this.light02.px, this.light02.py - 127.5f, this.light02.pz);
            this.light14.setTranslate(this.light14.px, this.light14.py - 127.5f, this.light14.pz);
            this.tA.setTranslate(this.tA.px, this.tA.py - 127.5f + 0.6f, this.tA.pz);
            this.tB.setTranslate(this.tB.px, this.tB.py - 127.5f - 0.6f, this.tB.pz);
            this.elv.setArgs(12, 0.0f);
            this.eleh.setTranslate(this.eleh.px, 126.55f, this.eleh.pz);
            this.light04.setTranslate(this.light04.px, 129.45f, this.light04.pz);
            this.doorA.DoorOpen();
            this.enemy1.dispRadar(false);
            this.enemy2.dispRadar(false);
            this.enemy3.dispRadar(false);
        }
        if (Runtime.getFlags(8019, 1) == 1) {
            Stage.setVisible(151, false);
            this.kaiten.setRotate(0.0f, 0.0f, 90.0f);
            this.player.setID(1);
            this.light05.disp(false);
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST1510.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST1510.waitPage(this.win, 64);
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
            this.setElevatorMode(1);
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
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
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
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
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

        void idle() {
            ST1510.this.elel.getTranslate();
            ST1510.this.elel2.getTranslate();
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 120.0f;
            float f5 = f4 + 30.0f;
            float f6 = 33.199997f / f4 / f4;
            float f7 = 0.0f;
            float f8 = 360.0f;
            float f9 = 0.0f;
            float f10 = 121.8f / (f8 - 94.245f) / 4.0f;
            float f11 = 0.0075f;
            float f12 = 0.0f;
            float f13 = 0.0f;
            float f14 = 0.0f;
            float f15 = 0.0f;
            while (true) {
                float f16;
                f2 += 1.0f;
                if (ST1510.this.eleh_move == 1) {
                    if (Runtime.getFlags(8017, 1) == 0) {
                        if (f3 == 0.0f) {
                            Sound.effectPlay(196741);
                            Runtime.enable(65536);
                            Runtime.disable(524288);
                        }
                        if (f3 >= 0.0f && f3 < f4) {
                            ST1510.this.player.getTranslate();
                            ST1510.this.eleh.getTranslate();
                            ST1510.this.light04.getTranslate();
                            f16 = 2.0f * f6 * (f4 - f3);
                            ST1510.this.player.setTranslate(ST1510.this.eleh.px, ST1510.this.eleh.py - f16 + 1.0f, ST1510.this.eleh.pz);
                            ST1510.this.eleh.setTranslate(ST1510.this.eleh.px, ST1510.this.eleh.py - f16, ST1510.this.eleh.pz);
                            ST1510.this.light04.setTranslate(ST1510.this.light04.px, ST1510.this.light04.py - f16, ST1510.this.light04.pz);
                        }
                        if (f3 == f4) {
                            ST1510.this.doorA.DoorOpen();
                            Runtime.disable(65536);
                            ST1510.this.eleh_move = 0;
                            Runtime.enable(524288);
                            Runtime.setFlags(8017, 1, 1);
                        }
                        f3 += 1.0f;
                    } else {
                        ST1510.this.eleh_move = 0;
                    }
                } else if (ST1510.this.eleh_move == 2) {
                    if (f5 == f4 + 30.0f) {
                        ST1510.this.doorA.DoorClose();
                        Runtime.disable(524288);
                    }
                    if (f5 == f4) {
                        Sound.effectPlay(196742);
                    }
                    if (f5 <= f4 && f5 > 0.0f) {
                        ST1510.this.player.getTranslate();
                        ST1510.this.eleh.getTranslate();
                        ST1510.this.light04.getTranslate();
                        f16 = 2.0f * f6 * (f4 - f5);
                        ST1510.this.player.setTranslate(ST1510.this.player.px, ST1510.this.eleh.py + f16 + 1.0f, ST1510.this.player.pz);
                        ST1510.this.eleh.setTranslate(ST1510.this.eleh.px, ST1510.this.eleh.py + f16, ST1510.this.eleh.pz);
                        ST1510.this.light04.setTranslate(ST1510.this.light04.px, ST1510.this.light04.py + f16, ST1510.this.light04.pz);
                    }
                    if (f5 == 30.0f) {
                        ST1510.this.fade.call(0);
                    }
                    if (f5 == 0.0f) {
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        Runtime.setFlags(8018, 1, 1);
                        Runtime.enable(524288);
                        Runtime.jumpCF(1499, 1);
                    }
                    f5 -= 1.0f;
                }
                if (ST1510.this.elel_move == 1) {
                    System.println("下");
                    ST1510.this.elel.getTranslate();
                    ST1510.this.elel2.getTranslate();
                    ST1510.this.elel3.getTranslate();
                    ST1510.this.light02.getTranslate();
                    ST1510.this.light14.getTranslate();
                    ST1510.this.tA.getTranslate();
                    ST1510.this.tB.getTranslate();
                    ST1510.this.player.getTranslate();
                    if (f7 == 0.0f) {
                        Sound.streamPlay(1195016, 48000);
                        ST1510.this.light02.disp(true);
                        Runtime.disable(524288);
                        ST1510.this.light14 = new Effect(1679, 0.0f, 127.5f, 6.0f, 0.0f);
                        ST1510.this.light14.noAttach(false);
                        ST1510.this.light14.disp(false);
                        Runtime.enable(65536);
                    }
                    if (f7 < 20.0f) {
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py + f11, ST1510.this.elel3.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py + f11, ST1510.this.tA.pz);
                    }
                    if (f7 == 65.0f) {
                        ST1510.this.light14.disp(true);
                    }
                    if (f7 >= 30.0f && f7 < 60.0f) {
                        f9 = ST1510.this.elel.py - Math.cos((f7 - 30.0f) * 3.0f * 3.14f / 180.0f) * f10;
                        f13 = Math.cos((f7 - 30.0f) * 3.0f * 3.14f / 180.0f) * f10;
                        ST1510.this.elel.setTranslate(ST1510.this.elel.px, f9, ST1510.this.elel.pz);
                        ST1510.this.elv.setArgs(12, f9);
                        ST1510.this.player.setTranslate(ST1510.this.player.px, f9, ST1510.this.player.pz);
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py - f13, ST1510.this.elel2.pz);
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py - f13, ST1510.this.elel3.pz);
                        ST1510.this.light02.setTranslate(ST1510.this.light02.px, ST1510.this.light02.py - f13, ST1510.this.light02.pz);
                        ST1510.this.light14.setTranslate(ST1510.this.light14.px, ST1510.this.light14.py - f13, ST1510.this.light14.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py - f13, ST1510.this.tA.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py - f13, ST1510.this.tB.pz);
                    }
                    if (f7 >= 60.0f && f7 < f8 - 60.0f) {
                        f9 = ST1510.this.elel.py - f10;
                        ST1510.this.elel.setTranslate(ST1510.this.elel.px, f9, ST1510.this.elel.pz);
                        ST1510.this.elv.setArgs(12, f9);
                        ST1510.this.player.setTranslate(ST1510.this.player.px, f9, ST1510.this.player.pz);
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py - f10, ST1510.this.elel2.pz);
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py - f10, ST1510.this.elel3.pz);
                        ST1510.this.light02.setTranslate(ST1510.this.light02.px, ST1510.this.light02.py - f10, ST1510.this.light02.pz);
                        ST1510.this.light14.setTranslate(ST1510.this.light14.px, ST1510.this.light14.py - f10, ST1510.this.light14.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py - f10, ST1510.this.tA.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py - f10, ST1510.this.tB.pz);
                    }
                    if (f7 >= f8 - 60.0f && f7 < f8 - 30.0f) {
                        f9 = ST1510.this.elel.py - Math.cos(((f7 - (f8 - 60.0f)) * 3.0f + 90.0f) * 3.14f / 180.0f) * f10;
                        f13 = Math.cos(((f7 - (f8 - 60.0f)) * 3.0f + 90.0f) * 3.14f / 180.0f) * f10;
                        ST1510.this.elel.setTranslate(ST1510.this.elel.px, f9, ST1510.this.elel.pz);
                        ST1510.this.elv.setArgs(12, f9);
                        ST1510.this.player.setTranslate(ST1510.this.player.px, f9, ST1510.this.player.pz);
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py - f13, ST1510.this.elel2.pz);
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py - f13, ST1510.this.elel3.pz);
                        ST1510.this.light02.setTranslate(ST1510.this.light02.px, ST1510.this.light02.py - f13, ST1510.this.light02.pz);
                        ST1510.this.light14.setTranslate(ST1510.this.light14.px, ST1510.this.light14.py - f13, ST1510.this.light14.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py - f13, ST1510.this.tA.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py - f13, ST1510.this.tB.pz);
                    }
                    if (f7 == f8 - 30.0f) {
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                    }
                    if (f7 >= f8 - 25.0f && f7 < f8 - 5.0f) {
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py - f11, ST1510.this.elel2.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py - f11, ST1510.this.tB.pz);
                    }
                    if (f7 == 30.0f) {
                        ST1510.this.cam0.setMode(-1);
                        ST1510.this.enemy1.dispRadar(false);
                        ST1510.this.enemy2.dispRadar(false);
                        ST1510.this.enemy3.dispRadar(false);
                    }
                    if (f7 >= 30.0f && f7 < f8 - 30.0f) {
                        ST1510.this.cam0.setRotate(-72.1f, 0.0f, 0.0f);
                        ST1510.this.cam0.setTranslate(6.0f - f7 / 55.0f, f9 + 15.3f + f7 / 40.0f, 14.76f);
                    }
                    if (f7 == f8 - 1.0f) {
                        ST1510.this.cam0.setRotate(-15.803791f, 0.0f, 0.0f);
                        ST1510.this.cam0.setTranslate(0.0f, 4.6967688f, 15.169842f);
                    }
                    if (f7 == f8 - 30.0f - 30.0f) {
                        f12 = ST1510.this.elel.py + f10;
                    }
                    if ((f7 += 0.25f) >= f8) {
                        ST1510.this.elel_ichi = 1;
                        f7 = 0.0f;
                        ST1510.this.elel_move = 0;
                        ST1510.this.light02.disp(false);
                        Runtime.enable(524288);
                    }
                }
                if (ST1510.this.elel_move == 2) {
                    ST1510.this.elel.getTranslate();
                    ST1510.this.elel2.getTranslate();
                    ST1510.this.elel3.getTranslate();
                    ST1510.this.light02.getTranslate();
                    ST1510.this.light14.getTranslate();
                    ST1510.this.tA.getTranslate();
                    ST1510.this.tB.getTranslate();
                    ST1510.this.player.getTranslate();
                    if (f7 == 0.0f) {
                        Sound.streamPlay(1195016, 48000);
                        ST1510.this.light02.disp(true);
                        Runtime.disable(524288);
                        ST1510.this.light19 = new Effect(1733, 0.0f, 0.0f, 6.0f, 0.0f);
                        ST1510.this.light19.noAttach(false);
                        ST1510.this.light19.disp(false);
                        Runtime.enable(65536);
                    }
                    if (f7 < 20.0f) {
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py + f11, ST1510.this.elel2.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py + f11, ST1510.this.tB.pz);
                    }
                    if (f7 == 30.0f) {
                        ST1510.this.light19.disp(true);
                    }
                    if (f7 >= 30.0f && f7 < 60.0f) {
                        f9 = ST1510.this.elel.py + Math.cos((f7 - 30.0f) * 3.0f * 3.14f / 180.0f) * f10;
                        f13 = Math.cos((f7 - 30.0f) * 3.0f * 3.14f / 180.0f) * f10;
                        ST1510.this.elel.setTranslate(ST1510.this.elel.px, f9, ST1510.this.elel.pz);
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py + f13, ST1510.this.elel2.pz);
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py + f13, ST1510.this.elel3.pz);
                        ST1510.this.elv.setArgs(12, f9);
                        ST1510.this.player.setTranslate(ST1510.this.player.px, f9, ST1510.this.player.pz);
                        ST1510.this.light02.setTranslate(ST1510.this.light02.px, ST1510.this.light02.py + f13, ST1510.this.light02.pz);
                        ST1510.this.light19.setTranslate(ST1510.this.light19.px, ST1510.this.light19.py + f13, ST1510.this.light19.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py + f13, ST1510.this.tA.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py + f13, ST1510.this.tB.pz);
                    }
                    if (f7 >= 60.0f && f7 < f8 - 60.0f) {
                        f9 = ST1510.this.elel.py + f10;
                        ST1510.this.elel.setTranslate(ST1510.this.elel.px, f9, ST1510.this.elel.pz);
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py + f10, ST1510.this.elel2.pz);
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py + f10, ST1510.this.elel3.pz);
                        ST1510.this.elv.setArgs(12, f9);
                        ST1510.this.player.setTranslate(ST1510.this.player.px, f9, ST1510.this.player.pz);
                        ST1510.this.light02.setTranslate(ST1510.this.light02.px, ST1510.this.light02.py + f10, ST1510.this.light02.pz);
                        ST1510.this.light19.setTranslate(ST1510.this.light19.px, ST1510.this.light19.py + f10, ST1510.this.light19.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py + f10, ST1510.this.tA.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py + f10, ST1510.this.tB.pz);
                    }
                    if (f7 >= f8 - 60.0f && f7 < f8 - 30.0f) {
                        f9 = ST1510.this.elel.py + Math.cos(((f7 - (f8 - 60.0f)) * 3.0f + 90.0f) * 3.14f / 180.0f) * f10;
                        f13 = Math.cos(((f7 - (f8 - 60.0f)) * 3.0f + 90.0f) * 3.14f / 180.0f) * f10;
                        ST1510.this.elel.setTranslate(ST1510.this.elel.px, f9, ST1510.this.elel.pz);
                        ST1510.this.elel2.setTranslate(ST1510.this.elel2.px, ST1510.this.elel2.py + f13, ST1510.this.elel2.pz);
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py + f13, ST1510.this.elel3.pz);
                        ST1510.this.elv.setArgs(12, f9);
                        ST1510.this.player.setTranslate(ST1510.this.player.px, f9, ST1510.this.player.pz);
                        ST1510.this.light02.setTranslate(ST1510.this.light02.px, ST1510.this.light02.py + f13, ST1510.this.light02.pz);
                        ST1510.this.light19.setTranslate(ST1510.this.light19.px, ST1510.this.light19.py + f13, ST1510.this.light19.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py + f13, ST1510.this.tA.pz);
                        ST1510.this.tB.setTranslate(ST1510.this.tB.px, ST1510.this.tB.py + f13, ST1510.this.tB.pz);
                    }
                    if (f7 == f8 - 30.0f) {
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                    }
                    if (f7 >= f8 - 25.0f && f7 < f8 - 5.0f) {
                        ST1510.this.elel3.setTranslate(ST1510.this.elel3.px, ST1510.this.elel3.py - f11, ST1510.this.elel3.pz);
                        ST1510.this.tA.setTranslate(ST1510.this.tA.px, ST1510.this.tA.py - f11, ST1510.this.tA.pz);
                    }
                    if (f7 == 30.0f) {
                        ST1510.this.cam0.setMode(-1);
                    }
                    if (f7 >= 30.0f && f7 < f8 - 30.0f) {
                        ST1510.this.cam0.setRotate(-72.1f, 0.0f, 0.0f);
                        ST1510.this.cam0.setTranslate(-6.0f + (f8 - f7) / 55.0f, f9 + 15.3f + (f8 - f7) / 40.0f, 14.76f);
                    }
                    if (f7 == f8 - 1.0f) {
                        ST1510.this.cam0.setRotate(-23.473303f, 0.0f, 0.0f);
                        ST1510.this.cam0.setTranslate(0.0f, 132.5f, 19.806631f);
                        ST1510.this.enemy1.dispRadar(true);
                        ST1510.this.enemy2.dispRadar(true);
                        ST1510.this.enemy3.dispRadar(true);
                    }
                    if ((f7 += 0.25f) >= f8) {
                        ST1510.this.light02.disp(false);
                        Runtime.enable(524288);
                        ST1510.this.elel_ichi = 0;
                        f7 = 0.0f;
                        ST1510.this.elel_move = 0;
                    }
                }
                if (ST1510.this.kaiten_move == 1) {
                    if (f14 == 0.0f) {
                        Runtime.disable(524288);
                        ST1510.this.light20.disp(true);
                    }
                    if (f14 == 0.0f) {
                        ST1510.this.light15.disp(true);
                    }
                    if (f14 == 40.0f) {
                        ST1510.this.light15.disp(false);
                    }
                    if (f14 == 30.0f) {
                        ST1510.this.light16.disp(true);
                    }
                    if (f14 == 70.0f) {
                        ST1510.this.light16.disp(false);
                    }
                    if (f14 == 100.0f) {
                        ST1510.this.light17.disp(true);
                    }
                    if (f14 == 140.0f) {
                        ST1510.this.light17.disp(false);
                    }
                    if (f14 == 110.0f) {
                        ST1510.this.light18.disp(true);
                    }
                    if (f14 == 150.0f) {
                        ST1510.this.light18.disp(false);
                    }
                    if (f14 > 0.0f && f14 <= 30.0f) {
                        ST1510.this.kaiten.setRotate(0.0f, 0.0f, f15 += f14 * 2.0f / 300.0f);
                    }
                    if (f14 > 30.0f && f14 <= 450.0f) {
                        ST1510.this.kaiten.setRotate(0.0f, 0.0f, f15 += 0.2f);
                    }
                    if (f14 > 450.0f && f14 <= 480.0f) {
                        ST1510.this.kaiten.setRotate(0.0f, 0.0f, f15 += (480.0f - f14) * 2.0f / 300.0f);
                    }
                    if (f14 > 500.0f && f14 <= 530.0f) {
                        ST1510.this.locked.setRotate((f14 - 380.0f) * 6.0f, 0.0f, 0.0f);
                        ST1510.this.open.setRotate((f14 - 380.0f) * 6.0f + 180.0f, 0.0f, 0.0f);
                        ST1510.this.light05.disp(false);
                    }
                    if (f14 == 510.0f) {
                        ST1510.this.open.getTranslate();
                        ST1510.this.open.setTranslate(ST1510.this.open.px, ST1510.this.open.py, ST1510.this.open.pz + 0.01f);
                        ST1510.this.locked.getTranslate();
                        ST1510.this.locked.setTranslate(ST1510.this.locked.px, ST1510.this.locked.py - 0.03f, ST1510.this.locked.pz - 0.03f);
                    }
                    if (f14 == 370.0f) {
                        ST1510.this.hokori01.disp(true);
                    }
                    if (f14 > 400.0f && f14 <= 700.0f) {
                        ST1510.this.hokori01.getTranslate();
                        ST1510.this.hokori01.setTranslate(ST1510.this.hokori01.px, ST1510.this.hokori01.py, ST1510.this.hokori01.pz + 0.05f);
                    }
                    if ((f14 += 1.0f) == 500.0f) {
                        Runtime.setFlags(8019, 1, 1);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        Runtime.enable(524288);
                        ST1510.this.player.setID(1);
                    }
                    if (f14 == 720.0f) {
                        f14 = 0.0f;
                        ST1510.this.kaiten_move = 0;
                    }
                }
                if (Runtime.getFlags(8019, 1) == 0) {
                    if (f == 0.0f) {
                        ST1510.this.light01.disp(true);
                    }
                    if (f == 1.0f) {
                        ST1510.this.light01.disp(false);
                    }
                    if (f == 8.0f) {
                        ST1510.this.light01.disp(true);
                    }
                    if (f == 9.0f) {
                        ST1510.this.light01.disp(false);
                    }
                } else {
                    ST1510.this.light01.disp(false);
                }
                if ((f += 1.0f) == 60.0f) {
                    f = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

