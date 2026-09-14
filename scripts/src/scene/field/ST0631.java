import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS13_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0631
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS13_PRJ {
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
    Enepc npc9;
    Enepc npc10;
    Enepc npc11;
    Unit unit1;
    Unit Pod1;
    Unit Pod2;
    Unit Pod3;
    Unit Pod4;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
    int touchFlag1;
    int touchFlag2;
    int S2009;
    int S2013;
    int S2013B;
    int S2014;
    int S2014B;
    int S2028;
    int MOMO;
    int ZIGGY;
    int S2030;
    int S2040C;
    int S2042;
    int S2057;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono itembox;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    int page;
    String[] msg01631D59 = new String[]{"/[label(Shion)]", "This is Hangar 2.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D5A = new String[]{"/[label(MOMO)]", "Umm...it got scratched up all over the place during that fight. Do you think it will be okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D5B = new String[]{"/[label(Shion)]", "Oh, don't let it bother you.", "/[waitkey(64)]/[clear()]"};
    String[] msg11631D5B = new String[]{"/[label(Shion)]", "It's pointless to worry about it because it's just going to be repossessed for his debts anyhow.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D5C = new String[]{"/[label(MOMO)]", "Repossessed?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D5D = new String[]{"/[label(Ziggy)]", "It means that the Captain has not paid back money he borrowed.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D5E = new String[]{"/[label(MOMO)]", "Oh...? Is he in that much financial trouble?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D5F = new String[]{"/[label(Shion)]", "Yes, apparently so.", "/[waitkey(64)]/[close()]"};
    String[] msg011FE37B = new String[]{"/[label(Ziggy)]", "What is it?", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37C = new String[]{"/[label(Shion)]", "Commander Cherenkov has gone missing. Will you help me look for him?", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37E = new String[]{"/[label(Ziggy)]", "The Commander should be okay. I do not think you need to worry so much.", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37F = new String[]{"/[label(Shion)]", "But there is a chance something did happen. I would feel more secure if you were here.", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE381 = new String[]{"/[label(Ziggy)]", "All right.\n", "I'll help you look for him. We cannot have him delaying our departure.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};

    ST0631() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.012f, 2.363f, -14.005f);
        this.camEV.setRotate(-9.195f, -180.839f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.06f, 1.197f, 2.971f);
        this.camEV.setRotate(-1.488f, 208.458f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.98f, 4.443f, -17.176f);
        this.camEV.setRotate(-13.703f, -224.96f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            default:
        }
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0631.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc7_1(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(621, 3);
                break;
            }
        }
    }

    void init() {
        this.S2009 = Runtime.getFlags(115, 1);
        this.S2013 = Runtime.getFlags(122, 1);
        this.S2013B = Runtime.getFlags(123, 1);
        this.S2014 = Runtime.getFlags(124, 1);
        this.S2014B = Runtime.getFlags(125, 1);
        this.S2028 = Runtime.getFlags(141, 1);
        this.MOMO = Runtime.getFlags(7014, 1);
        this.S2030 = Runtime.getFlags(143, 1);
        this.S2040C = Runtime.getFlags(158, 1);
        this.ZIGGY = Runtime.getFlags(7015, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S2057 = Runtime.getFlags(179, 1);
        Stage.setVisible(-1, true);
        this.Pod1 = new Unit();
        this.Pod1.init(20493, -11.5f, 0.611f, -10.5f, 90.0f);
        this.Pod2 = new Unit();
        this.Pod2.init(20493, 11.5f, 0.611f, -10.5f, -90.0f);
        this.Pod3 = new Unit();
        this.Pod3.init(20493, -11.5f, 0.611f, -5.5f, 90.0f);
        this.Pod4 = new Unit();
        this.Pod4.init(20493, 11.5f, 0.611f, -5.5f, -90.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(307, false);
        Stage.setVisible(308, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 10.0f, 0.5f, -8.0f, 0.0f);
        this.teiten1.SetBgm(196632);
        this.teiten2 = new Uwamono(28690, -10.0f, 0.5f, -8.0f, 0.0f);
        this.teiten2.SetBgm(196632);
        this.teiten3 = new Uwamono(28690, 0.0f, -1.9f, 20.5f, 0.0f);
        this.teiten3.SetBgm(196633);
        this.teiten3.SetBgmType('\u0001');
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.2f, 0.2f, 0.2f);
        this.light.setColor(1, 0.2f, 0.2f, 0.2f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.55f, 0.55f, 0.55f);
        Runtime.setIdLightCol(1, 3, 0.55f, 0.55f, 0.55f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setDefocusQuick(0, 1, 7000, 1);
        Runtime.setDefocusQuick(1, 1, 6000, 1);
        Runtime.setDefocusQuick(2, 1, 5000, 1);
        Runtime.setDefocusQuick(3, 1, 8000, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 35.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -20.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 20.0f, 0.0f, 8.0f, 35.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 17.0f, 35.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        new Uwamono(67, 31);
        new Uwamono(66, 31);
        this.itembox = new Uwamono(28677, 0.0f, -1.886f, 19.05f, 0.0f, 118);
        this.itembox.SetSymbol(28684);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = Integer.MIN_VALUE;
        this.fadeOut.args[1] = 20;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 20;
        this.fadeIn.args[2] = 0;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.doorA = new Uwamono(286, 42, '\u0001');
        new Uwamono(285, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(301, 1) == 1) {
            this.npcset_0();
        } else {
            this.npcset_0();
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
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

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
        }
    }
}

