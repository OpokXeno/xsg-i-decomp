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
import xeno.map.MC_VOK23_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0230
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK23_PRJ {
    Player player;
    int selected;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Menu menu;
    Window win;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int evflg = 0;
    boolean enemy1_move = false;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc momo;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono trap4;
    Uwamono trap5;
    Uwamono door2f;
    Uwamono item1;
    Uwamono item2;
    Uwamono itembox1;
    Uwamono itembox2;
    Uwamono itembox3;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Unit hasigo;
    Effect fade;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    int S03;
    int itemget;
    int F07;
    Light light = new Light(0);
    Unit kidou;
    float[] gnoFilter = new float[4];
    int button_flg;
    int page;
    String[] msg_shion_0 = new String[]{"/[label(Shion)]", "KOS-MOS, I'll explain a bit about this place.\n", "/[waitkey(64)]/[clear()]"};
    String[] msg_shion_00 = new String[]{"/[label(Shion)]", "This place is an imaginary space constructed within the network. It's modeled off of some ruins from early 2000 A.D. Your mission is to fight the main target within\nthe dilapidated building. The target is marked with the red Vector box.", "/[waitkey(64)]/[clear()]"};
    String[] msg_shion_3_2 = new String[]{"/[label(Shion)]", "There are objects in here that can be detonated. Some are effective if you set them off near the enemy. Use them to your advantage.", "/[waitkey(64)]/[clear()]"};
    String[] msg_shion_4 = new String[]{"/[label(Shion)]", "Now then, let's start the mission.\n", "/[waitkey(64)]/[close()]"};
    String[] msg_shion_5 = new String[]{"/[label(Shion)]", "All right then, let's start the mission!\n", "/[waitkey(64)]/[close()]"};
    String[] msg_shion_6 = new String[]{"/[label(Shion)]", "Oh, also, if there are two or more objects you can select from, use the L1 and R1 Buttons to choose and shoot.", "/[waitkey(64)]/[close()]"};
    String[] msg_shion_7_1 = new String[]{"/[label(Shion)]", "There are many destroyable objects on the map. When you're in range, a blue target cursor will appear. Shoot at them using the □ Button.", "/[waitkey(64)]/[clear()]"};
    String[] msg_shion_7_2 = new String[]{"If there are several destroyable objects grouped together, use the L1/R1 Buttons to highlight in blue the one to destroy, then shoot it.", "/[waitkey(64)]/[close()]"};
    String[] msg_kosmos_0 = new String[]{"/[label(KOS-MOS)]", "Okay, whenever you're ready.\n", "/[waitkey(64)]/[clear()]"};
    String[] msg_momo_0 = new String[]{"/[label(MOMO (Special Appearance))]", "I'd like to help out today, Shion. I'm ready.\n", "/[waitkey(64)]/[close()]"};
    String[] dummy = new String[]{"/[label(Shion)]", "Temp message during debugging.\n", "/[waitkey(64)]/[close()]"};

    ST0230() {
    }

    float Arctan(float f) {
        float f2 = f;
        float f3 = f;
        int n = 3;
        while (n < 20) {
            f3 = -f3 * f * f;
            float f4 = f3 / (float) n;
            f2 += f4;
            n += 2;
        }
        f2 = f2 * 180.0f / 3.14f;
        return f2;
    }

    void EOB(int n) {
    }

    void EV_Camera_0() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.97f, -0.55f, 2.35f);
        this.camEV.setRotate(0.0f, 33.0f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera_1() {
        this.camEV.setTranslate(-6.24f, 13.4f, 7.37f);
        this.camEV.setRotate(-29.0f, 33.0f, 0.0f);
        this.camEV.setFov(30.0f);
    }

    void Final_init(int n) {
        switch (n) {
            case 1: {
                this.enemy1.kickEnepc(10, 100, 0);
                break;
            }
            case 2: {
                this.enemy2.kickEnepc(10, 100, 0);
                break;
            }
            case 3: {
                this.enemy3.kickEnepc(10, 100, 0);
                break;
            }
            case 11: {
                this.momo.kickEnepc(4, 1);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        Runtime.enable(262144);
        switch (n2) {
            case 0: {
                if (this.button_flg == 1) {
                    return;
                }
                this.button_flg = 1;
                System.println("KickEvent_case0!!!!!!!!!!!!!!");
                this.itemget = Runtime.getFlags(1005, 1);
                if (this.itemget != 0) break;
                Sound.effectPlay(6);
                Runtime.setFlags(1005, 1, 1);
                Runtime.addItemWin(0, 1);
                break;
            }
            case 1: {
                if (Runtime.getFlags(1013, 1) != 0) break;
                Runtime.setFlags(1013, 1, 1);
                Runtime.setPlayerControl(false);
                System.println("2Fのターゲット選択");
                Runtime.enable(65536);
                this.player.move(30, -7.948f, 3.7f, true);
                this.player.mtn(2, 1, 1.0f, true);
                System.sleep(33);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                System.sleep(1);
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.mtn(11, 1, 1.0f, true);
                this.msg_2F();
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                break;
            }
        }
        Runtime.disable(262144);
        this.button_flg = 0;
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                if (this.enemy1_move) break;
                this.enemy1_move = true;
                System.println("トラップが壊れた");
                break;
            }
            case 2: {
                if (this.enemy1_move) break;
                this.enemy1_move = true;
                System.println("ドアが壊れた");
                this.player.getTranslate();
                this.enemy1.kickEnepc(4, 1);
                System.sleep(10);
                this.enemy1.moveEnepc(17, 90.0f, 1.0f, 5);
                this.enemy1.kickEnepc(1, 1);
                this.enemy1.moveEnepc(15, -8.0f, 0.4f, 40);
                System.sleep(35);
                this.enemy1.moveEnepc(17, 45.0f, 0.0f, 5);
                System.sleep(5);
                this.enemy1.kickEnepc(1, 3);
                this.movepc(this.enemy1, 15);
                this.enemy1.kickEnepc(4, 0);
                break;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        Runtime.setFlags(1014, 1, 0);
        System.println("フラグFUJI_14初期化");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S03 == 1) {
                    Runtime.jumpCF(241, 5);
                    return;
                }
                Runtime.jumpCF(240, 5);
                break;
            }
            case 1: {
                if (this.S03 == 1) {
                    Runtime.jumpCF(241, 6);
                    return;
                }
                Runtime.jumpCF(240, 6);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 7.751f, 0.0f, 6.093f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, -7.005f, 0.0f, -5.338f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, -10.145f, 0.0f, -13.0f, 0.0f);
        this.teiten3.SetBgm(196609);
        this.teiten4 = new Uwamono(28690, -10.242f, 0.0f, 7.618f, 0.0f);
        this.teiten4.SetBgm(196614);
        Stage.setVisible(-1, true);
        this.hasigo = new Mapunits();
        this.hasigo.mapUnit(259);
        this.hasigo.start(4, null);
        this.hasigo.setTranslate(0.0f, -0.1f, 0.0f);
        this.hasigo.setRotate(0.0f, 0.0f, 0.0f);
        this.light.setColor(0, 0.64f, 0.64f, 0.64f);
        this.light.setColor(1, 0.83f, 0.83f, 0.83f);
        this.light.setDirection2(1, 0.817f, 0.552f, -0.168f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.69f, 0.69f, 0.69f);
        this.light.setDirection2(2, 0.824f, 0.529f, -0.204f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.54f, 0.54f, 0.54f);
        this.light.setDirection2(3, 0.996f, -0.0f, -0.094f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 1, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightCol(1, 2, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightCol(1, 3, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(1, 3, 0.0f, 1.0f, -0.5f);
        Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(2, 1, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightCol(2, 2, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightCol(2, 3, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(2, 3, 0.0f, 1.0f, -0.5f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(5, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(6, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(7, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(8, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(9, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(10, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(11, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(12, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFPedestal(1, -3.6143699f, 11.554572f, 10.688438f, 40.0f, -14.041767f, 390.48288f, 0.0f, 2.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(1, 1);
        this.cam0.setCFPedestal(2, -8.2274f, 14.3819f, 8.9127f, 43.2f, -73.2657f, 350.572f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(2, 1);
        this.cam0.setCFAngle(3, -28.0f, 345.0f, 0.0f, 15.0f, 30.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestal(4, -4.5294f, 9.1108f, -2.73789f, 42.5598f, -19.2338f, 378.6274f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 30.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 15.0f, 30.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 6.0f, 30.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 345.0f, 0.0f, 15.0f, 30.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.enemy1 = new Enepc();
        this.enemy1.init(18177, 9, -11.67f, 9.03f, -0.005f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(0, 0, 0, 0);
        this.enemy1.setBatEvent(12);
        float[] fArray = new float[28];
        fArray[0] = -11.67f;
        fArray[1] = 9.03f;
        fArray[2] = -0.005f;
        fArray[3] = -1.0f;
        fArray[4] = -10.0f;
        fArray[5] = 9.03f;
        fArray[6] = 0.47f;
        fArray[8] = -7.047f;
        fArray[9] = 9.03f;
        fArray[10] = 0.47f;
        fArray[11] = 1.0f;
        fArray[12] = -7.047f;
        fArray[13] = 9.03f;
        fArray[14] = 2.634f;
        fArray[15] = 2.0f;
        fArray[16] = -7.047f;
        fArray[17] = 9.03f;
        fArray[18] = 5.459f;
        fArray[19] = 3.0f;
        fArray[20] = -7.047f;
        fArray[21] = 9.03f;
        fArray[22] = 6.82f;
        fArray[23] = 4.0f;
        fArray[24] = -8.928f;
        fArray[25] = 9.03f;
        fArray[26] = 5.459f;
        fArray[27] = 4.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 9, fArray2);
        this.enemy2 = new Enepc();
        this.enemy2.init(17665, 5, 1.988f, 0.0f, -14.036f, 0.0f);
        this.enemy2.id = 2;
        this.enemy2.setGroup(2, 2, 2, 2);
        this.enemy2.setBatEvent(12);
        float[] fArray3 = new float[72];
        fArray3[0] = 1.988f;
        fArray3[2] = -14.036f;
        fArray3[3] = -1.0f;
        fArray3[4] = -2.023f;
        fArray3[6] = -14.036f;
        fArray3[8] = -1.908f;
        fArray3[9] = -0.0f;
        fArray3[10] = -10.502f;
        fArray3[11] = 1.0f;
        fArray3[12] = -2.827f;
        fArray3[14] = -6.34f;
        fArray3[15] = 2.0f;
        fArray3[16] = -2.937647f;
        fArray3[18] = -5.017918f;
        fArray3[19] = 3.0f;
        fArray3[20] = -3.562626f;
        fArray3[22] = 0.963647f;
        fArray3[23] = 4.0f;
        fArray3[24] = -3.228541f;
        fArray3[26] = 5.706091f;
        fArray3[27] = 5.0f;
        fArray3[28] = -0.658117f;
        fArray3[30] = 10.281687f;
        fArray3[31] = 6.0f;
        fArray3[32] = 1.96524f;
        fArray3[34] = 8.904668f;
        fArray3[35] = 7.0f;
        fArray3[36] = 3.616433f;
        fArray3[38] = 4.454323f;
        fArray3[39] = 8.0f;
        fArray3[40] = 2.739454f;
        fArray3[42] = 0.571341f;
        fArray3[43] = 9.0f;
        fArray3[44] = 2.768592f;
        fArray3[46] = -2.679425f;
        fArray3[47] = 10.0f;
        fArray3[48] = 3.691245f;
        fArray3[50] = -5.793843f;
        fArray3[51] = 11.0f;
        fArray3[52] = 6.444053f;
        fArray3[54] = -7.234153f;
        fArray3[55] = 12.0f;
        fArray3[56] = -5.171f;
        fArray3[58] = 7.744f;
        fArray3[59] = 6.0f;
        fArray3[60] = -7.476f;
        fArray3[62] = 7.744f;
        fArray3[63] = 14.0f;
        fArray3[64] = 3.736555f;
        fArray3[66] = -14.036f;
        fArray3[68] = 3.736555f;
        fArray3[70] = -17.036f;
        fArray3[71] = 16.0f;
        float[] fArray4 = fArray3;
        this.enemy2.setParams(2, 0, 2, 5, fArray4);
        float[] fArray5 = new float[24];
        fArray5[0] = 1.988f;
        fArray5[2] = -14.036f;
        fArray5[3] = 3.665f;
        fArray5[5] = -12.727f;
        fArray5[6] = 4.796f;
        fArray5[8] = -14.029f;
        fArray5[9] = 3.613f;
        fArray5[11] = -15.624f;
        fArray5[12] = 1.988f;
        fArray5[14] = -14.036f;
        fArray5[15] = -1.452f;
        fArray5[17] = -14.036f;
        fArray5[18] = -4.392f;
        fArray5[20] = -14.036f;
        fArray5[21] = -1.452f;
        fArray5[23] = -14.036f;
        float[] fArray6 = fArray5;
        this.enemy2.setParams(fArray6);
        this.enemy3 = new Enepc();
        this.enemy3.init(17665, 5, -2.827f, 0.0f, -6.34f, 0.0f);
        this.enemy3.id = 3;
        this.enemy3.setGroup(1, 1, 1, 1);
        this.enemy3.setBatEvent(12);
        float[] fArray7 = new float[72];
        fArray7[0] = -2.827f;
        fArray7[2] = -6.34f;
        fArray7[3] = -1.0f;
        fArray7[4] = -2.937647f;
        fArray7[6] = -5.017918f;
        fArray7[8] = -3.562626f;
        fArray7[10] = 0.963647f;
        fArray7[11] = 1.0f;
        fArray7[12] = -3.228541f;
        fArray7[14] = 5.706091f;
        fArray7[15] = 2.0f;
        fArray7[16] = -0.658117f;
        fArray7[18] = 10.281687f;
        fArray7[19] = 3.0f;
        fArray7[20] = 1.96524f;
        fArray7[22] = 8.904668f;
        fArray7[23] = 4.0f;
        fArray7[24] = 3.616433f;
        fArray7[26] = 4.454323f;
        fArray7[27] = 5.0f;
        fArray7[28] = 2.739454f;
        fArray7[30] = 0.571341f;
        fArray7[31] = 6.0f;
        fArray7[32] = 2.768592f;
        fArray7[34] = -2.679425f;
        fArray7[35] = 7.0f;
        fArray7[36] = 3.691245f;
        fArray7[38] = -5.793843f;
        fArray7[39] = 8.0f;
        fArray7[40] = 6.444053f;
        fArray7[42] = -7.234153f;
        fArray7[43] = 9.0f;
        fArray7[44] = -1.908f;
        fArray7[45] = -0.0f;
        fArray7[46] = -10.502f;
        fArray7[48] = -2.023f;
        fArray7[50] = -14.036f;
        fArray7[51] = 11.0f;
        fArray7[52] = 1.988f;
        fArray7[54] = -14.036f;
        fArray7[55] = 12.0f;
        fArray7[56] = -5.171f;
        fArray7[58] = 7.744f;
        fArray7[59] = 3.0f;
        fArray7[60] = -7.476f;
        fArray7[62] = 7.744f;
        fArray7[63] = 14.0f;
        fArray7[64] = 3.736555f;
        fArray7[66] = -14.036f;
        fArray7[67] = 13.0f;
        fArray7[68] = 3.736555f;
        fArray7[70] = -17.036f;
        fArray7[71] = 16.0f;
        float[] fArray8 = fArray7;
        this.enemy3.setParams(3, 0, 3, 5, fArray8);
        float[] fArray9 = new float[42];
        fArray9[0] = -2.827f;
        fArray9[2] = -6.34f;
        fArray9[3] = -1.736f;
        fArray9[5] = -4.941f;
        fArray9[6] = -2.827f;
        fArray9[8] = -3.63f;
        fArray9[9] = -2.827f;
        fArray9[11] = -1.036f;
        fArray9[12] = -2.827f;
        fArray9[14] = 1.47f;
        fArray9[15] = -2.827f;
        fArray9[17] = 4.063f;
        fArray9[18] = -3.962f;
        fArray9[20] = 5.821f;
        fArray9[21] = -2.827f;
        fArray9[23] = 7.327f;
        fArray9[24] = -1.619f;
        fArray9[26] = 5.821f;
        fArray9[27] = -2.827f;
        fArray9[29] = 4.063f;
        fArray9[30] = -2.827f;
        fArray9[32] = 1.47f;
        fArray9[33] = -2.827f;
        fArray9[35] = -1.036f;
        fArray9[36] = -2.827f;
        fArray9[38] = -3.63f;
        fArray9[39] = -4.086f;
        fArray9[41] = -4.941f;
        float[] fArray10 = fArray9;
        this.enemy3.setParams(fArray10);
        this.item1 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 3);
        new Uwamono(8, 0, this.item1);
        new Uwamono(244, 4);
        new Uwamono(243, 18);
        new Uwamono(154, 39);
        new Uwamono(158, 39);
        this.door2f = new Uwamono(158, 39);
        this.itembox1 = new Uwamono(28677, -10.586f, 9.03f, 2.284f, 270.0f, 4);
        this.trap1 = new Uwamono(28673, 0.434f, -1.423f, -3.064f, 0.0f);
        this.trap2 = new Uwamono(28674, -10.083f, 9.053f, 1.32f, 0.0f);
        this.trap2.SetCallNo(1);
        this.door2f.SetCallNo(2);
        this.EF01 = new Effect(1515, 10.443f, 0.0f, -4.684f, 0.0f);
        this.EF01.disp(true);
        this.EF01.setScale(0.4f, 0.8f, 0.4f);
        this.EF01.setClip(true);
        this.EF02 = new Effect(1402, 7.751f, 0.0f, 6.093f, 0.0f);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1406, -13.145f, 3.0f, -14.699f, 0.0f);
        this.EF03.disp(true);
        this.EF03.setScale(2.0f, 2.0f, 2.0f);
        this.EF03.setClip(true);
        this.EF04 = new Effect(1402, -7.005f, 0.0f, -5.338f, 0.0f);
        this.EF04.disp(true);
        this.EF04.setScale(0.6f, 0.5f, 0.3f);
        this.EF04.setClip(true);
        this.EF05 = new Effect(1571, -5.5f, 8.2f, -17.5f, 0.0f);
        this.EF05.disp(true);
        this.EF05.setRotate(90.0f, 0.0f, 0.0f);
        this.EF05.setClip(true);
        this.EF06 = new Effect(1571, 0.5f, 8.2f, -17.5f, 0.0f);
        this.EF06.disp(true);
        this.EF06.setRotate(90.0f, 0.0f, 0.0f);
        this.EF06.setClip(true);
        this.EF07 = new Effect(1571, 9.0f, 7.0f, -16.4f, 0.0f);
        this.EF07.disp(true);
        this.EF07.setRotate(90.0f, 0.0f, 90.0f);
        this.EF07.setClip(true);
        Runtime.progressEffect(150);
        this.enemy1.kickEnepc(19, 1, 0, 926, 1);
        this.S03 = Runtime.getFlags(3, 1);
        this.itemget = Runtime.getFlags(8, 1);
        this.F07 = Runtime.getFlags(1007, 1);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.kidou = new Mapunits();
        this.kidou.mapUnit(68);
        if (this.F07 == 0) {
            this.kidou.start(4, null);
            this.kidou.start(1, "Msg");
        } else {
            this.kidou.start(4, null);
            Runtime.setPlayerControl(true);
        }
    }

    void movepc(Enepc enepc, int n) {
        float f;
        enepc.getTranslate();
        float f2 = enepc.px - this.player.px;
        float f3 = enepc.pz - this.player.pz;
        if (f2 == 0.0f) {
            f = f3 > 0.0f ? 0.0f : 180.0f;
        } else if (f3 == 0.0f) {
            f = f2 > 0.0f ? 90.0f : 270.0f;
        } else {
            float f4 = f2 / f3;
            if (f4 > 1.0f || f4 < -1.0f) {
                f4 = f3 / f2;
                f = 90.0f - this.Arctan(f4);
                if (f2 > 0.0f) {
                    f += 180.0f;
                }
            } else {
                f = this.Arctan(f4);
                if (f3 > 0.0f) {
                    f += 180.0f;
                }
            }
        }
        if (f < 0.0f) {
            f = 360.0f + f;
        }
        if (f > 90.0f && f < 270.0f) {
            this.enemy1.moveEnepc(17, f, 1.0f, 5);
        } else {
            this.enemy1.moveEnepc(17, f, -1.0f, 5);
        }
        enepc.moveEnepc(15, this.player.px, this.player.pz, n);
        System.sleep(n);
    }

    void msg_2F() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg_shion_6, 0);
        ST0230.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
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

        void shion_start() {
            float f = 0.0f;
            while (f < 1.0f) {
                ST0230.this.gnoFilter[0] = 0.0f + f;
                ST0230.this.gnoFilter[1] = 0.0f;
                ST0230.this.gnoFilter[2] = 0.0f;
                ST0230.this.gnoFilter[3] = 0.0f;
                ST0230.this.player.setFilter(2);
                ST0230.this.player.setFilterParam(ST0230.this.gnoFilter);
                System.sleep(1);
                f += 0.01f;
            }
        }
    }

    class Obj
            extends Unit {
        Obj() {
        }
    }

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(4, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(4, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(4, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(4, 16);
        }

        void init() {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Msg() {
            Runtime.charAllRecovery();
            System.println("*********初期状態**************");
            System.println("*********全回復しました**************");
            Runtime.setPlayerControl(false);
            ST0230.this.win = Window.create();
            ST0230.this.win.print(ST0230.this.msg_shion_0, 0);
            ST0230.waitPage(ST0230.this.win, 64);
            ST0230.this.win.print(ST0230.this.msg_shion_00, 0);
            ST0230.waitPage(ST0230.this.win, 64);
            ST0230.this.win.print(ST0230.this.msg_shion_3_2, 0);
            ST0230.this.cam0.setMode(-1);
            ST0230.this.EV_Camera_0();
            System.sleep(30);
            ST0230.this.EV_Camera_1();
            ST0230.waitPage(ST0230.this.win, 64);
            ST0230.this.cam0.setMode(0);
            ST0230.this.win.print(ST0230.this.msg_shion_4, 0);
            ST0230.waitPage(ST0230.this.win, 64);
            ST0230.this.selected = 2;
            while (ST0230.this.selected != 0) {
                ST0230.this.menu = Menu.create();
                ST0230.this.menu.addItem("Yes, understood\nPlease explain destroyable objects");
                System.waitFor(ST0230.this.menu);
                ST0230.this.selected = ST0230.this.menu.getSelected();
                if (ST0230.this.selected == 0) {
                    ST0230.this.win = Window.create();
                    ST0230.this.win.print(ST0230.this.msg_shion_5, 0);
                    ST0230.waitPage(ST0230.this.win, 64);
                }
                if (ST0230.this.selected != 1) continue;
                ST0230.this.win = Window.create();
                ST0230.this.win.print(ST0230.this.msg_shion_7_1, 0);
                ST0230.this.cam0.setMode(-1);
                ST0230.this.EV_Camera_0();
                ST0230.waitPage(ST0230.this.win, 64);
                ST0230.this.win.print(ST0230.this.msg_shion_7_2, 0);
                ST0230.this.EV_Camera_1();
                ST0230.waitPage(ST0230.this.win, 64);
                ST0230.this.cam0.setMode(0);
            }
            Runtime.setFlags(1007, 1, 1);
            Runtime.setPlayerControl(true);
        }
    }
}

