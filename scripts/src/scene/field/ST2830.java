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
import xeno.map.MC_UTA13_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2830
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTA13_PRJ {
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
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Unit unit1;
    Effect light01;
    Effect light02;
    Effect light03;
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
    Uwamono doorA;
    Uwamono box01;
    Uwamono box02;
    Uwamono box03;
    Uwamono box04;
    Uwamono box05;
    Uwamono box06;
    Uwamono box07;
    Uwamono box08;
    Uwamono box09;
    Uwamono box10;
    Uwamono box11;
    Uwamono box12;
    Uwamono box13;
    Uwamono box14;
    Uwamono box15;
    Uwamono box16;
    Uwamono box17;
    Uwamono tA;
    Uwamono tB;
    Uwamono tC;
    Uwamono tD;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono item6;
    Uwamono item7;
    Uwamono item8;
    Uwamono item9;
    Uwamono item10;
    int migi = 0;
    int hidari = 0;
    boolean rakka = false;
    int lo = 0;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Effect fade;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] rakka_serifu = new String[]{"There's a hole. Jump down?", "/[waitkey(64)]/[close()]"};
    String[] hasigo = new String[]{"There is something above and you cannot climb any further.", "/[waitkey(64)]/[close()]"};

    ST2830() {
    }

    void Final_init(int n) {
    }

    public void HashigoTop(int n) {
        switch (n) {
            case 1: {
                if (Runtime.getFlags(8100, 1) != 0) break;
                Runtime.setPlayerControl(false);
                this.nwin(this.hasigo);
                Runtime.setPlayerControl(true);
                break;
            }
        }
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
                this.player.getTranslate();
                if (this.player.py < 15.0f) {
                    return;
                }
                if (!this.rakka) {
                    return;
                }
                System.println("1");
                this.enemy7.kickEnepc(4, 2);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                this.lo = 1;
                this.nwin(this.rakka_serifu);
                this.yesno();
                switch (this.selected) {
                    case 0: {
                        float f = 0.0f;
                        float f2 = 0.0f;
                        float f3 = (-9.15f - this.player.px) / 30.0f;
                        float f4 = (1.1f - this.player.pz) / 30.0f;
                        Runtime.enable(65536);
                        this.player.mtn(23, 1, 1.0f, true);
                        System.sleep(10);
                        while (true) {
                            this.player.getTranslate();
                            f2 = (-9.8f * f / 30.0f + 5.0f) * f / 30.0f + 18.0f;
                            Runtime.setRegister(1, f);
                            System.println("time: /[#1]");
                            if (f < 30.0f) {
                                this.player.setTranslate(this.player.px + f3, f2, this.player.pz + f4);
                            } else if (f2 >= 12.0f) {
                                this.player.setTranslate(this.player.px, f2, this.player.pz);
                            } else {
                                this.player.setTranslate(-9.15f, 12.0f, 1.1f);
                            }
                            if (f == 20.0f) {
                                this.player.mtn(24, 1, 1.0f, true);
                            }
                            if (f == 60.0f) break;
                            f += 1.0f;
                            System.sleep(1);
                        }
                        Runtime.disable(65536);
                        this.lo = 0;
                        Runtime.setPlayerControl(true);
                        this.enemy7.kickEnepc(4, 0);
                        return;
                    }
                }
                this.enemy7.kickEnepc(4, 0);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                return;
            }
        }
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                System.println("1");
                System.println("右");
                this.hantei(1);
                break;
            }
            case 2: {
                System.println("2");
                System.println("右");
                this.hantei(1);
                break;
            }
            case 3: {
                System.println("3");
                System.println("右");
                this.hantei(1);
                break;
            }
            case 4: {
                System.println("4");
                System.println("右");
                this.hantei(1);
                break;
            }
            case 5: {
                System.println("5");
                System.println("右");
                this.hantei(1);
                break;
            }
            case 6: {
                System.println("6");
                System.println("右");
                this.hantei(1);
                break;
            }
            case 7: {
                System.println("7");
                System.println("左");
                this.hantei(2);
                break;
            }
            case 8: {
                System.println("8");
                System.println("左");
                this.hantei(2);
                break;
            }
            case 9: {
                System.println("9");
                System.println("左");
                this.hantei(2);
                break;
            }
            case 10: {
                System.println("10");
                System.println("左");
                this.hantei(2);
                break;
            }
            case 11: {
                System.println("11");
                System.println("左");
                this.hantei(2);
                break;
            }
            case 12: {
                System.println("12");
                System.println("左");
                this.hantei(2);
                break;
            }
            case 13: {
                System.println("13");
                this.enemy1.kickEnepc(19, 2, 0, 1280, 1);
                Runtime.setFlags(8100, 1, 1);
                break;
            }
            case 14: {
                System.println("14");
                this.rakka = true;
                break;
            }
            case 15: {
                System.println("15");
                break;
            }
            case 16: {
                System.println("16");
                break;
            }
            case 17: {
                System.println("17");
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(68356, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68376, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(2870, 11);
                break;
            }
        }
    }

    void hantei(int n) {
        this.player.setID(1);
        this.tA.setTranslate(8.0f, 0.0f, 0.0f);
        this.tB.setTranslate(2.0f, 0.0f, 0.0f);
        this.tC.setTranslate(-10.0f, 0.0f, 0.0f);
        this.tD.setTranslate(-16.0f, 0.0f, 0.0f);
        if (n == 1) {
            ++this.migi;
        }
        if (n == 2) {
            ++this.hidari;
        }
        if (this.migi == 3) {
            System.println("右 2 階");
            System.sleep(10);
            this.player.setID(2);
            this.tA.setTranslate(8.0f, -4.0f, 0.0f);
            this.tB.setTranslate(2.0f, -4.0f, 0.0f);
        }
        if (this.migi == 6) {
            System.println("右 1 階");
            System.sleep(10);
            this.player.setID(2);
            this.tA.setTranslate(8.0f, 6.0f, 0.0f);
            this.tB.setTranslate(2.0f, 6.0f, 0.0f);
        }
        if (this.hidari == 3) {
            System.println("左 2 階");
            System.sleep(10);
            this.player.setID(2);
            this.tC.setTranslate(-10.0f, -4.0f, 0.0f);
            this.tD.setTranslate(-16.0f, -4.0f, 0.0f);
        }
        if (this.hidari == 6) {
            System.println("左 1 階");
            System.sleep(10);
            this.player.setID(2);
            this.tC.setTranslate(-10.0f, 6.0f, 0.0f);
            this.tD.setTranslate(-16.0f, 6.0f, 0.0f);
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 5.0f, -2.0f, 4.0f, 0.0f);
        this.teiten1.SetBgm(196613);
        this.teiten2 = new Uwamono(28690, -13.0f, -2.0f, 4.0f, 0.0f);
        this.teiten2.SetBgm(196613);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(2, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setShootUwaCheck(false);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 20.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, 0.0f, 10.0f, 0.0f, 40.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, 0.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, 0.0f, -10.0f, 0.0f, 40.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFAngle(9, -28.0f, -15.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        float[] fArray = new float[12];
        fArray[0] = -18.0f;
        fArray[3] = -1.0f;
        fArray[4] = -18.0f;
        fArray[6] = 0.5f;
        fArray[8] = -18.0f;
        fArray[10] = -0.5f;
        float[] fArray2 = fArray;
        this.enemy1 = new NpcEnemy(16898, 1, 1, 19, 3, -18.0f, 0.0f, 0.0f, 0.0f, fArray2);
        this.enemy1.setGroup(0, 0, 2, 2);
        float[] fArray3 = new float[12];
        fArray3[0] = 14.0f;
        fArray3[2] = 2.0f;
        fArray3[3] = -1.0f;
        fArray3[4] = 17.0f;
        fArray3[6] = 4.0f;
        fArray3[8] = 10.0f;
        fArray3[10] = 1.0f;
        float[] fArray4 = fArray3;
        this.enemy2 = new NpcEnemy(16899, 2, 1, 24, 5, 14.0f, 0.0f, 2.0f, 0.0f, fArray4);
        this.enemy2.setGroup(1, 1, 2, 2);
        float[] fArray5 = new float[12];
        fArray5[0] = -18.0f;
        fArray5[1] = 6.0f;
        fArray5[2] = -1.0f;
        fArray5[3] = -1.0f;
        fArray5[4] = -18.0f;
        fArray5[5] = 6.0f;
        fArray5[6] = 0.68f;
        fArray5[8] = -25.8f;
        fArray5[9] = 6.0f;
        fArray5[10] = 0.68f;
        fArray5[11] = 1.0f;
        float[] fArray6 = fArray5;
        this.enemy3 = new NpcEnemy(16900, 3, 1, 29, 7, -18.0f, 6.0f, -1.0f, 0.0f, fArray6);
        this.enemy3.setGroup(3, 3, 4, 4);
        float[] fArray7 = new float[12];
        fArray7[0] = 14.0f;
        fArray7[1] = 18.0f;
        fArray7[2] = 0.7f;
        fArray7[3] = -1.0f;
        fArray7[4] = 10.0f;
        fArray7[5] = 18.0f;
        fArray7[6] = 0.7f;
        fArray7[8] = 18.0f;
        fArray7[9] = 18.0f;
        fArray7[10] = 0.7f;
        float[] fArray8 = fArray7;
        this.enemy4 = new NpcEnemy(16899, 4, 1, 24, 5, 14.0f, 18.0f, 0.7f, 0.0f, fArray8);
        this.enemy4.setGroup(6, 7, 7, 10);
        float[] fArray9 = new float[12];
        fArray9[0] = -18.0f;
        fArray9[1] = 18.0f;
        fArray9[2] = -4.0f;
        fArray9[3] = -1.0f;
        fArray9[4] = -18.0f;
        fArray9[5] = 18.0f;
        fArray9[6] = 0.7f;
        fArray9[8] = -26.0f;
        fArray9[9] = 18.0f;
        fArray9[10] = 0.7f;
        fArray9[11] = 1.0f;
        float[] fArray10 = fArray9;
        this.enemy6 = new NpcEnemy(16898, 6, 1, 19, 3, -18.0f, 18.0f, -4.0f, 0.0f, fArray10);
        this.enemy6.setGroup(6, 6, 9, 10);
        float[] fArray11 = new float[24];
        fArray11[0] = -4.0f;
        fArray11[1] = 18.0f;
        fArray11[3] = -1.0f;
        fArray11[5] = 18.0f;
        fArray11[9] = 18.0f;
        fArray11[10] = -3.1f;
        fArray11[11] = 1.0f;
        fArray11[12] = -7.0f;
        fArray11[13] = 18.0f;
        fArray11[16] = -8.0f;
        fArray11[17] = 18.0f;
        fArray11[18] = -1.2f;
        fArray11[19] = 3.0f;
        fArray11[20] = -8.0f;
        fArray11[21] = 18.0f;
        fArray11[22] = -3.0f;
        fArray11[23] = 4.0f;
        float[] fArray12 = fArray11;
        this.enemy7 = new NpcEnemy(16900, 7, 1, 29, 7, -4.0f, 18.0f, 0.0f, 0.0f, fArray12);
        this.enemy7.setGroup(6, 8, 8, 9);
        this.enemy1.kickEnepc(19, 1, 0, 1850, 1);
        this.enemy1.kickEnepc(19, 3, 0, 1850, 1);
        if (Runtime.getFlags(8100, 1) == 0) {
            this.enemy1.kickEnepc(19, 2, 0, 1050, 1);
        } else {
            this.enemy1.kickEnepc(19, 2, 0, 1290, 1);
        }
        this.item3 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 346);
        this.item5 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 348);
        this.item8 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 351);
        this.item9 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 352);
        this.item10 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 353);
        this.box01 = new Uwamono(10, 4);
        this.box01.SetCallNo(1);
        this.box01.SetGravity(true);
        this.box02 = new Uwamono(27, 4);
        this.box02.SetCallNo(2);
        this.box02.SetGravity(true);
        this.box03 = new Uwamono(28, 4);
        this.box03.SetCallNo(3);
        this.box03.SetGravity(true);
        this.box04 = new Uwamono(29, 4);
        this.box04.SetCallNo(4);
        this.box04.SetGravity(true);
        this.box05 = new Uwamono(30, 4);
        this.box05.SetCallNo(5);
        this.box05.SetGravity(true);
        this.box06 = new Uwamono(31, 4);
        this.box06.SetCallNo(6);
        this.box06.SetGravity(true);
        this.box07 = new Uwamono(86, 4);
        this.box07.SetCallNo(7);
        this.box07.SetGravity(true);
        this.box08 = new Uwamono(87, 4);
        this.box08.SetCallNo(8);
        this.box08.SetGravity(true);
        this.box09 = new Uwamono(88, 4);
        this.box09.SetCallNo(9);
        this.box09.SetGravity(true);
        this.box10 = new Uwamono(89, 4);
        this.box10.SetCallNo(10);
        this.box10.SetGravity(true);
        this.box11 = new Uwamono(90, 4);
        this.box11.SetCallNo(11);
        this.box11.SetGravity(true);
        this.box12 = new Uwamono(91, 4);
        this.box12.SetCallNo(12);
        this.box12.SetGravity(true);
        if (Runtime.getFlags(8100, 1) == 0) {
            this.box13 = new Uwamono(108, 5);
            this.box13.SetCallNo(13);
            this.box13.SetBgm(196609);
        } else {
            Stage.setVisible(108, false);
        }
        this.box14 = new Uwamono(109, 13, this.item8);
        this.box14.SetCallNo(14);
        this.box14.SetHitKind('\u0002');
        this.box14.SetSize(2.0f, 2.0f, 2.0f);
        this.box16 = new Uwamono(124, 4);
        this.box16.SetCallNo(16);
        this.box16.SetGravity(true);
        this.box16.SetBroken(false);
        this.box16.SetSize(0.01f, 0.0f, 0.01f);
        this.box17 = new Uwamono(125, 4);
        this.box17.SetCallNo(17);
        this.box17.SetGravity(true);
        this.box17.SetBroken(false);
        this.box17.SetSize(0.01f, 0.0f, 0.01f);
        Stage.setVisible(110, false);
        this.tA = new Uwamono(28672, 8.0f, 0.0f, -0.0f, 0.0f);
        this.tA.SetSize(0.4f, 8.0f, 4.0f);
        this.tB = new Uwamono(28672, 2.0f, 0.0f, 0.0f, 0.0f);
        this.tB.SetSize(0.4f, 8.0f, 4.0f);
        this.tC = new Uwamono(28672, -10.0f, 0.0f, 0.0f, 0.0f);
        this.tC.SetSize(0.4f, 8.0f, 4.0f);
        this.tD = new Uwamono(28672, -16.0f, 0.0f, 0.0f, 0.0f);
        this.tD.SetSize(0.4f, 8.0f, 4.0f);
        new Uwamono(126, 10, this.item3);
        new Uwamono(127, 10, this.item5);
        new Uwamono(128, 10, this.item9);
        new Uwamono(34, 39, this.item10);
        this.item1 = new Uwamono(28677, -1.0f, 12.0f, -5.01f, 180.0f, 426);
        this.item1.SetSymbol(28686);
        this.item1.SetCallNo(1);
        new Uwamono(28675, 19.0f, 18.0f, 0.2f, 180.0f);
        new Uwamono(28675, 15.1f, 0.0f, -0.7f, 180.0f);
        new Uwamono(28675, -17.4f, 6.0f, 0.5f, 180.0f);
        new Uwamono(28675, -16.8f, 0.0f, 2.1f, 180.0f);
        this.player.setID(1);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3237, 1, 1);
                break;
            }
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2830.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2830.waitPage(this.win, 64);
    }

    int super_getFlags(int n, int n2) {
        int n3 = 0;
        int n4 = 0;
        int n5 = n2 - 1;
        while (n5 >= 0) {
            n4 = Runtime.getFlags(n + n5, 1);
            n3 += (n4 <<= n5);
            --n5;
        }
        Runtime.setRegister(1, (float) n3);
        System.println("super_getFlags: /[#1]");
        return n3;
    }

    void super_setFlags(int n, int n2, int n3) {
        Runtime.setRegister(1, (float) n3);
        System.println("super_setFlags: /[#1]");
        int n4 = 0;
        int n5 = 0;
        while (n5 < n2) {
            n4 = n3;
            n4 >>= n5;
            Runtime.setFlags(n + n5, 1, n4 &= 1);
            ++n5;
        }
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
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
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
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
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
    }
}

