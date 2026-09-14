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
import xeno.map.MC_PRO01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0810
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO01_PRJ {
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
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Enepc enemy9;
    Unit unit1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc0talked = 0;
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
    Uwamono lift;
    Uwamono bgm;
    MAPUnit cube1;
    Effect smoke01;
    Effect smoke02;
    Effect smoke03;
    Effect fire01;
    Effect ef01;
    Effect ef02;
    Effect ef03;
    Effect fade;
    Unit airplane;
    Unit U_TIC;
    Unit car1;
    Unit car2;
    Unit car3;
    Unit car4;
    Unit car5;
    Unit car6;
    Unit AGWS1;
    Unit AGWS2;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    boolean move = false;
    Light light = new Light(0);
    float[] gnoFilter;
    int lo;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    int page;
    String[] sirei;
    String[] npc1_1;
    String[] npc1_2;
    String[] npc2_1;
    String[] npc2_2;
    String[] npc2_3;
    String[] agws;
    String[] npc0_1;
    String[] npc0_2;
    String[] npc0_3;
    String[] dassyutu;
    int b_flg;
    String[] EVS;

    ST0810() {
        float[] fArray = new float[4];
        fArray[1] = 112.0f;
        fArray[2] = 30.0f;
        this.gnoFilter = fArray;
        this.lo = 0;
        this.sirei = new String[]{"'→Command Room'", "/[waitkey(64)]/[close()]"};
        this.npc1_1 = new String[]{"/[label(Soldier C)]", "...I wish my shift would end so I can get relieved.", "/[waitkey(64)]/[close()]"};
        this.npc1_2 = new String[]{"/[label(Soldier C)]", "Man, I'm sleepy.", "/[waitkey(64)]/[close()]"};
        this.npc2_1 = new String[]{"/[label(Soldier D)]", "Hm?", "/[waitkey(64)]/[close()]"};
        this.npc2_2 = new String[]{"/[label(Soldier D)]", "...Was that a fly?", "/[waitkey(64)]/[close()]"};
        this.npc2_3 = new String[]{"/[label(Soldier D)]", "*Smack*", "/[waitkey(1)]/[clear()]", "Damn, it got away.", "/[waitkey(64)]/[close()]"};
        this.agws = new String[]{"/[label(Ziggurat 8)]", "(U-TIC Organization's multi-purpose A.G.W.S., Type Mercurio.)", "/[waitkey(1)]/[clear()]", "(It's going to be tough to take this on by myself.)", "/[waitkey(64)]/[close()]"};
        this.npc0_1 = new String[]{"/[label(Soldier A)]", "The Realian that Commander Margulis acquired supposedly possesses incredible powers.", "/[waitkey(64)]/[close()]"};
        this.npc0_2 = new String[]{"/[label(Soldier B)]", "That artificial humanoid that looks like a kid? It's gotta be just a rumor.", "/[waitkey(64)]/[close()]"};
        this.npc0_3 = new String[]{"/[label(Soldier A)]", "But Commander Margulis seems to be quite attached to it.", "/[waitkey(1)]/[clear()]", "It's supposedly locked up in solitary confinement in the area past the shrine.", "/[waitkey(64)]/[close()]"};
        this.dassyutu = new String[]{"/[label(Ziggurat 8)]", "(A small shuttle for two...It looks like it could be useful for our escape.)", "/[waitkey(64)]/[close()]"};
        this.EVS = new String[]{"Exit the EVS (Environmental Simulator)?", "/[waitkey(64)]/[close()]"};
    }

    void Final_init(int n) {
        System.println("FINAL_INIT");
        switch (n) {
            case 5: {
                this.enemy5.kickEnepc(4, 1);
                this.enemy5.kickEnepc(0, 0);
                break;
            }
            case 6: {
                this.enemy6.kickEnepc(4, 1);
                this.enemy6.kickEnepc(1, 1);
                break;
            }
        }
    }

    public void HashigoBottom(int n) {
        switch (n) {
            case 0: {
                System.println("0");
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(869, 3);
                break;
            }
            default: {
                System.println("d");
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
            case 2: {
                Runtime.setPlayerControl(false);
                this.lo = 1;
                System.println("→司令室");
                this.nwin(this.sirei);
                this.lo = 0;
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void TalkNPC1(Enepc enepc) {
        ++this.npc0talked;
        if (this.npc0talked == 1) {
            this.nwin(this.npc0_1);
        }
        if (this.npc0talked == 2) {
            this.nwin(this.npc0_2);
        }
        if (this.npc0talked == 3) {
            this.nwin(this.npc0_3);
            this.npc0talked = 0;
        }
    }

    void TalkNPC3(Enepc enepc) {
        ++this.npc2talked;
        if (this.npc2talked == 1) {
            this.enemy3.kickEnepc(4, 2);
            this.nwin(this.npc1_1);
            this.enemy3.kickEnepc(4, 0);
        }
        if (this.npc2talked == 3) {
            this.enemy3.kickEnepc(4, 2);
            this.nwin(this.npc1_2);
            this.enemy3.kickEnepc(4, 0);
        }
        if (this.npc2talked == 4) {
            this.npc2talked = 0;
        }
    }

    void TalkNPC4(Enepc enepc) {
        ++this.npc1talked;
        if (this.npc1talked == 3) {
            this.enemy4.kickEnepc(4, 2);
            this.nwin(this.npc2_1);
            this.enemy4.kickEnepc(4, 0);
        }
        if (this.npc1talked == 6) {
            this.enemy4.kickEnepc(4, 2);
            this.nwin(this.npc2_2);
            this.enemy4.kickEnepc(4, 0);
        }
        if (this.npc1talked == 9) {
            this.enemy4.kickEnepc(4, 2);
            this.nwin(this.npc2_3);
            this.enemy4.kickEnepc(4, 0);
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.setFlags(8003, 1, 0);
                Runtime.jumpCF(829, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(879, 2);
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
        ST0810.waitPage(this.win, 64);
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
        float[] fArray;
        float[] fArray2;
        new Uwamono(28734, -9.6f, -1.6f, 15.4f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.teiten1 = new Uwamono(28690, 10.0f, 1.15f, -20.5f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten2 = new Uwamono(28690, -9.5f, 0.0f, -11.35f, 0.0f);
        this.teiten2.SetBgm(196616);
        this.teiten3 = new Uwamono(28690, -8.75f, -0.8f, 3.25f, 0.0f);
        this.teiten3.SetBgm(196616);
        this.teiten4 = new Uwamono(28690, -8.75f, -1.97f, 7.5f, 0.0f);
        this.teiten4.SetBgm(196616);
        if (Runtime.getFlags(8066, 1) == 0) {
            Runtime.setFlags(8066, 1, 1);
            Runtime.setFriend(6);
            Runtime.setOutFriend(1);
            Runtime.setOutFriend(2);
            Runtime.setPartyData(0x1010000, 5);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 0);
            Runtime.setPartyData(65542, 0);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 6);
        }
        Stage.setColor(1.56f, 1.56f, 1.56f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, -0.3f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(1, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 1, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 2, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightCol(2, 3, 0.385f, 0.385f, 0.385f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 1.0f, -0.5f, 0.5f);
        Runtime.setIdLightVec(2, 3, 1.0f, -0.5f, -0.5f);
        Runtime.setIdLightCol(3, 0, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightCol(3, 1, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightCol(3, 2, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightCol(3, 3, 0.175f, 0.175f, 0.175f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(4, 1, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(4, 2, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(4, 3, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 1.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 1.0f, 1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.42f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 1, 0.42f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 2, 0.42f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 3, 0.42f, 0.375f, 0.375f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.75f, 1.0f, -0.75f);
        Runtime.setIdLightVec(5, 3, -0.75f, -1.0f, -0.75f);
        Stage.setVisible(-1, true);
        this.fire01 = new Effect(1406, 10.0f, 3.5f, -20.5f, 0.0f);
        this.smoke01 = new Effect(1571, -12.0f, -3.0f, 3.5f, 0.0f);
        this.smoke01.noAttach(false);
        this.smoke01.setRotate(0.0f, -90.0f, 0.0f);
        this.smoke02 = new Effect(1571, -11.7f, -3.0f, 1.5f, 0.0f);
        this.smoke02.noAttach(false);
        this.smoke02.setRotate(0.0f, -90.0f, 0.0f);
        this.smoke03 = new Effect(1571, -12.2f, -3.0f, 6.0f, 0.0f);
        this.smoke03.noAttach(false);
        this.smoke03.setRotate(0.0f, -90.0f, 0.0f);
        Runtime.progressEffect(60);
        Stage.setEffectRender(0);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 340.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 7.5f, 40.0f);
        this.cam0.setCFHokan(4, 0.02f, 0.02f);
        this.cam0.setCFAngle(5, -28.0f, 345.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 7.5f, 40.0f);
        this.cam0.setCFHokan(6, 0.015f, 0.015f);
        this.cam0.setCFPedestal(7, 4.5166483f, 8.431637f, -0.1465397f, 45.5548f, -31.481018f, 311.0034f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 100.0f, 100.0f);
        this.cam0.setCFAngle(8, -28.0f, -15.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        if (Runtime.getFlags(104, 1) == 0) {
            this.enemy1 = new NPC_NORMAL(17153, 1, 0, 0, 7, -6.8f, -1.98f, 10.84f, -45.0f);
            this.enemy1.talkto("TalkNPC1");
            this.enemy1.disableDTKFlag(4);
            this.enemy1.disableDTKFlag(2);
            this.enemy1.enableDTKFlag(1);
            this.enemy1.disableDTKFlag(131072);
            this.enemy1.enableDTKFlag(65536);
            this.enemy2 = new NPC_NORMAL(17153, 2, 0, 0, 3, -7.5f, -1.6f, 12.0f, 135.0f);
            this.enemy2.talkto("TalkNPC1");
            this.enemy2.disableDTKFlag(4);
            this.enemy2.disableDTKFlag(2);
            this.enemy2.enableDTKFlag(1);
            this.enemy2.disableDTKFlag(131072);
            this.enemy2.enableDTKFlag(65536);
            this.enemy3 = new NPC_NORMAL(17153, 3, 0, 1, 3, -11.7f, 0.0f, -6.9f, 90.0f);
            this.enemy3.talkto("TalkNPC3");
            this.enemy3.disableDTKFlag(4);
            this.enemy3.disableDTKFlag(2);
            this.enemy3.disableDTKFlag(1);
            this.enemy3.disableDTKFlag(131072);
            this.enemy3.enableDTKFlag(65536);
            this.enemy4 = new NPC_NORMAL(17153, 4, 0, 1, 3, -8.4f, 0.03f, -1.6f, 0.0f);
            this.enemy4.talkto("TalkNPC4");
            this.enemy4.disableDTKFlag(4);
            this.enemy4.disableDTKFlag(2);
            this.enemy4.disableDTKFlag(1);
            this.enemy4.disableDTKFlag(131072);
            this.enemy4.enableDTKFlag(65536);
        } else {
            float[] fArray3 = new float[16];
            fArray3[0] = -9.5f;
            fArray3[1] = -1.61f;
            fArray3[2] = 11.5f;
            fArray3[3] = -1.0f;
            fArray3[4] = -4.0f;
            fArray3[5] = -1.6f;
            fArray3[6] = 12.0f;
            fArray3[8] = -4.0f;
            fArray3[10] = -5.0f;
            fArray3[11] = 1.0f;
            fArray3[12] = -4.0f;
            fArray3[14] = -17.3f;
            fArray3[15] = 2.0f;
            fArray2 = fArray3;
            this.enemy1 = new NPC_NORMAL(17153, 1, 0, 37, 3, -9.5f, -1.61f, 11.5f, 135.0f, fArray2);
            this.enemy1.setGroup(0, 0, 0, 0);
            float[] fArray4 = new float[16];
            fArray4[0] = -2.0f;
            fArray4[1] = 0.03f;
            fArray4[2] = -0.2f;
            fArray4[3] = -1.0f;
            fArray4[4] = -4.0f;
            fArray4[6] = -5.0f;
            fArray4[8] = -4.0f;
            fArray4[9] = -1.6f;
            fArray4[10] = 12.0f;
            fArray4[11] = 1.0f;
            fArray4[12] = -4.0f;
            fArray4[14] = -17.3f;
            fArray4[15] = 1.0f;
            fArray = fArray4;
            this.enemy2 = new NPC_NORMAL(17153, 2, 0, 37, 3, -2.0f, 0.03f, -0.2f, -90.0f, fArray);
            this.enemy2.setGroup(0, 0, 0, 0);
            float[] fArray5 = new float[16];
            fArray5[0] = -11.7f;
            fArray5[2] = -6.9f;
            fArray5[3] = -1.0f;
            fArray5[4] = -4.0f;
            fArray5[6] = -5.0f;
            fArray5[8] = -4.0f;
            fArray5[9] = -1.6f;
            fArray5[10] = 12.0f;
            fArray5[11] = 1.0f;
            fArray5[12] = -4.0f;
            fArray5[14] = -17.3f;
            fArray5[15] = 1.0f;
            float[] fArray6 = fArray5;
            this.enemy3 = new NPC_NORMAL(17153, 3, 0, 38, 3, -11.7f, 0.0f, -6.9f, 90.0f, fArray6);
            this.enemy3.setGroup(0, 0, 0, 0);
            float[] fArray7 = new float[16];
            fArray7[0] = -8.4f;
            fArray7[1] = 0.03f;
            fArray7[2] = -1.6f;
            fArray7[3] = -1.0f;
            fArray7[4] = -4.0f;
            fArray7[6] = -5.0f;
            fArray7[8] = -4.0f;
            fArray7[9] = -1.6f;
            fArray7[10] = 12.0f;
            fArray7[11] = 1.0f;
            fArray7[12] = -4.0f;
            fArray7[14] = -17.3f;
            fArray7[15] = 1.0f;
            float[] fArray8 = fArray7;
            this.enemy4 = new NPC_NORMAL(17153, 4, 0, 38, 3, -8.4f, 0.03f, -1.6f, 0.0f, fArray8);
            this.enemy4.setGroup(0, 0, 0, 0);
        }
        this.enemy5 = new NPC_NORMAL(17409, 5, 0, 6, 6, -4.0f, 0.25f, -22.75f, 0.0f);
        this.enemy5.setInvalidID(1);
        this.enemy6 = new NPC_NORMAL(17411, 6, 0, 6, 6, 1.0f, 0.25f, -22.75f, 0.0f);
        this.enemy6.setInvalidID(1);
        this.enemy6.kickEnepc(19, 1, 0, 700, 1);
        this.enemy8 = new Enepc();
        this.enemy8.init(17153, 3, 17.3f, 2.0f, -9.98f, 0.0f);
        this.enemy8.id = 8;
        this.enemy8.setGroup(1, 1, 1, 1);
        fArray2 = new float[]{17.3f, 2.0f, -9.98f, 1.0f};
        this.enemy8.setParams(0, 3, 8, 3, fArray2);
        fArray = new float[]{17.3f, 2.0f, -9.98f, 17.3f, 2.0f, -5.92f, 17.3f, 2.0f, -2.4f};
        this.enemy8.setParams(fArray);
        this.car1 = new Unit();
        this.car1.init(20552, 0.0f, 0.0f, -1.0f, 180.0f);
        this.car1.renderCommand(530);
        this.car2 = new Unit();
        this.car2.init(20552, 2.5f, 0.0f, -1.0f, 180.0f);
        this.car2.renderCommand(530);
        this.car3 = new Unit();
        this.car3.init(20553, 5.0f, 0.0f, -1.0f, 180.0f);
        this.car3.renderCommand(530);
        this.car4 = new Unit();
        this.car4.init(20553, 7.5f, 0.0f, -1.0f, 180.0f);
        this.car4.renderCommand(530);
        this.car5 = new Unit();
        this.car5.init(20552, 10.0f, 0.0f, -1.0f, 0.0f);
        this.car5.renderCommand(530);
        this.airplane = new Mapunits();
        this.airplane.init(20497, -16.14f, -5.65f, -15.4f, 0.0f);
        this.airplane.renderCommand(18);
        this.U_TIC = new Mapunits();
        this.U_TIC.init(20591, -16.14f, -5.65f, -15.4f, 0.0f);
        this.U_TIC.renderCommand(18);
        this.cube1 = new Mapunits();
        this.cube1.mapUnit(73);
        this.cube1.start(4, null);
        this.item1 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 55);
        this.item2 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 36);
        this.item3 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 37);
        this.item4 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 38);
        new Uwamono(204, 10, this.item1);
        new Uwamono(77, 22, this.item3);
        new Uwamono(78, 22);
        new Uwamono(79, 32, this.item2);
        new Uwamono(83, 32);
        this.lift = new Uwamono(203, 26, this.item4);
        this.doorA = new Uwamono(114, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(104, 1) == 0) {
            this.cube1.start(1, "idle");
        }
        if (Runtime.getFlags(104, 1) == 0) {
            this.player.setFilter(3);
            this.player.setFilterParam(this.gnoFilter);
        }
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST0810.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST0810.waitPage(this.win, 64);
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }
    }

    class NPC_EVENT
            extends Enepc {
        NPC_EVENT() {
        }

        void init() {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            int n = 300;
            int n2 = 600;
            float f5 = 1.460001f / (float) n2 / (float) n2;
            float f6 = -7.05f / (float) n2 / (float) n2;
            float f7 = 49.0f / (float) n2 / (float) n2;
            while (true) {
                f = Math.sin(f3 * 3.14f / 180.0f);
                ST0810.this.U_TIC.setRotate(f / 2.0f, f / 2.0f, f);
                ST0810.this.airplane.setRotate(f / 2.0f, f / 2.0f, f);
                if (f3 == 360.0f) {
                    f3 = 0.0f;
                }
                ST0810.this.U_TIC.getTranslate();
                ST0810.this.airplane.getTranslate();
                float f8 = 2.0f * f5 * ((float) n2 - f4);
                float f9 = 2.0f * f6 * ((float) n2 - f4);
                float f10 = 2.0f * f7 * ((float) n2 - f4);
                f2 += 1.0f;
                f3 += 1.0f;
                f4 += 1.0f;
                System.sleep(1);
            }
        }
    }
}

