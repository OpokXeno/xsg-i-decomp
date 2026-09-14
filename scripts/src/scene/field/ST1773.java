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
import xeno.map.MC_DYU07_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1773
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU07_PRJ {
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
    Unit unit1;
    Effect light01;
    Effect light02;
    Effect light03;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc1btalked = 0;
    int npc2talked = 0;
    int npc2btalked = 0;
    int npc3talked = 0;
    int npc3btalked = 0;
    int npc4talked = 0;
    int npc4btalked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect fade;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    int page;
    String[] U01_00CN = new String[]{"That giant structure...Isn't that the Song of Nephilim, the cause of the Miltian Conflict?", "/[waitkey(1)]/[clear()]", "They say that song is what made the Realians go out of control.", "/[waitkey(1)]/[clear()]", "Will that tragedy be repeated again?", "/[waitkey(64)]/[close()]"};
    String[] U01_00CJ = new String[]{"Little Master, that's the Song of Nephilim, right?", "/[waitkey(1)]/[clear()]", "Wasn't it destroyed during the Miltian Conflict?", "/[waitkey(1)]/[clear()]", "Why is that thing here now, and who's operating that thing?", "/[waitkey(64)]/[close()]"};
    String[] U02_00CN = new String[]{"It's amazing! That giant colony is Vector's D#23mmerung, right?", "/[waitkey(1)]/[clear()]", "I think the Kukai Foundation is amazing too, but that's on a whole other scale.", "/[waitkey(64)]/[close()]"};
    String[] U02_00CJ = new String[]{"Man, did you see that too, Little Master?", "/[waitkey(1)]/[clear()]", "The D#23mmerung is amazing, isn't it? Everything seems to be on a totally different scale.", "/[waitkey(64)]/[close()]"};
    String[] U03_00CN = new String[]{"This seems to be turning into something enormous.", "/[waitkey(1)]/[clear()]", "The Gnosis, the Federation, and now even Vector is here. What the heck do you think is going on?", "/[waitkey(64)]/[close()]"};
    String[] U03_00CJ = new String[]{"Why did the D#23mmerung go to the trouble of coming to this sector of space?", "/[waitkey(1)]/[clear()]", "It's odd for Headquarters itself to come on a rescue operation.", "/[waitkey(64)]/[close()]"};
    String[] U04_00CN = new String[]{"Hey, the colony is okay, right?!", "/[waitkey(1)]/[clear()]", "I don't want to go home, only to find that home's not there anymore.", "/[waitkey(64)]/[close()]"};
    String[] U04_00CJ = new String[]{"Little Master, are you all right? Are you hurt?", "/[waitkey(1)]/[clear()]", "It's dangerous to wander around now, so it'd be better to stay here with me.", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "Right before the last stage. 【temp】", "/[waitkey(64)]/[close()]"};

    ST1773() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(7128, 1) != 1 || Runtime.getFlags(3221, 1) != 0) break;
                    Runtime.setPlayerControl(false);
                    this.EF05.disp(false);
                    Sound.effectPlay(6);
                    Runtime.addItemWin(10, 16);
                    Runtime.setFlags(3221, 1, 1);
                    Runtime.setPlayerControl(true);
                    break;
                }
            }
        }
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U02_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U02_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC3a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U03_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U03_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC4a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U04_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1853, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1853, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 0.0f, 0.0f, 1.0f, 0.0f);
        this.teiten1.SetBgm(196618);
        this.teiten2 = new Uwamono(28690, -8.5f, 0.135f, 4.5f, 0.0f);
        this.teiten2.SetBgm(196618);
        this.teiten3 = new Uwamono(28690, 8.5f, 0.135f, 4.5f, 0.0f);
        this.teiten3.SetBgm(196618);
        Sound.effectPlay(196618, 30, 64);
        Stage.setVisible(-1, true);
        this.EF01 = new Effect(1578, 0);
        this.EF01.disp(true);
        this.EF01.setClip(true);
        this.EF02 = new Effect(1493, 1);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1493, 2);
        this.EF03.disp(true);
        this.EF03.setClip(true);
        this.EF04 = new Effect(1493, 3);
        this.EF04.disp(true);
        this.EF04.setClip(true);
        if (Runtime.getFlags(7128, 1) == 1 && Runtime.getFlags(3221, 1) == 0) {
            this.EF05 = new Effect(1018, 13.0f, 0.3f, 0.75f, 0.0f);
            this.EF05.disp(true);
            this.EF05.setClip(true);
        }
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(11, false);
        Stage.setVisible(37, false);
        Stage.setVisible(38, false);
        Stage.setVisible(39, false);
        Stage.setVisible(40, false);
        Stage.setVisible(41, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 2, 0.7f, 0.7f, 0.7f);
        Runtime.setIdLightCol(1, 3, 0.7f, 0.7f, 0.7f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 1, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 2, 0.55f, 0.55f, 0.65f);
        Runtime.setIdLightCol(2, 3, 0.55f, 0.55f, 0.65f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, -20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 20.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 20.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, -20.0f, 0.0f, 14.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.npc1 = new NPC_NORMAL(1028, 11, 0, 0, 3, -7.411f, 1.0f, -9.14f, 225.0f);
        this.npc1.disableDTKFlag(131075);
        this.npc1.enableDTKFlag(12);
        this.npc1.setInvalidID(1);
        this.npc2 = new NPC_NORMAL(780, 12, 0, 0, 11, -14.2f, 0.898f, 4.833f, 45.0f);
        this.npc2.disableDTKFlag(131075);
        this.npc2.enableDTKFlag(12);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 10);
        this.npc3 = new Enepc();
        this.npc3.init(1025, 10, 0.0f, 0.113f, -3.0f, 90.0f);
        this.npc3.id = 13;
        this.npc3.setParams(0, 3, 13, 10);
        float[] fArray = new float[24];
        fArray[1] = 0.113f;
        fArray[2] = -3.0f;
        fArray[3] = 3.0f;
        fArray[4] = 0.113f;
        fArray[5] = -2.0f;
        fArray[6] = 4.0f;
        fArray[7] = 0.113f;
        fArray[8] = 1.0f;
        fArray[9] = 3.0f;
        fArray[10] = 0.113f;
        fArray[11] = 4.0f;
        fArray[13] = 0.113f;
        fArray[14] = 5.0f;
        fArray[15] = -3.0f;
        fArray[16] = 0.113f;
        fArray[17] = 4.0f;
        fArray[18] = -4.0f;
        fArray[19] = 0.113f;
        fArray[20] = 1.0f;
        fArray[21] = -3.0f;
        fArray[22] = 0.113f;
        fArray[23] = -2.0f;
        float[] fArray2 = fArray;
        this.npc3.setParams(fArray2);
        this.npc3.enableDTKFlag(12);
        this.npc4 = new Enepc();
        this.npc4.init(1547, 10, 7.537f, 1.0f, -6.095f, 190.0f);
        this.npc4.id = 14;
        this.npc4.setParams(0, 2, 14, 10);
        float[] fArray3 = new float[]{7.537f, 1.0f, -6.095f, 6.872f, 1.0f, -6.857f, 4.825f, 1.0f, -8.523f, 2.226f, 1.0f, -9.044f};
        this.npc4.setParams(fArray3);
        this.npc4.enableDTKFlag(12);
        this.npc1.talkto("TalkNPC1a");
        this.npc2.talkto("TalkNPC2a");
        this.npc3.talkto("TalkNPC3a");
        this.npc4.talkto("TalkNPC4a");
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

    class Obj
            extends Unit {
        Obj() {
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
    }
}

