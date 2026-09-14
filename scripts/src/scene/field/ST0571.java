import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS07_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0571
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS07_PRJ {
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
    Enepc npc21;
    Unit Tub;
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
    int talkFlag9;
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
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    int page;
    String[] msg0162BD8A = new String[]{"/[label(Shion)]", "Morning, KOS-MOS. How do you feel?", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8B = new String[]{"/[label(KOS-MOS)]", "Good morning, Shion. All systems are normal.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8C = new String[]{"/[label(Shion)]", "Good. I'm glad you're well.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8D = new String[]{"/[label(KOS-MOS)]", "My systems are simply functioning within their specified parameters. Being well is not a phrase that is applicable to me.", "/[waitkey(64)]/[clear()]"};
    String[] msg0162BD8E = new String[]{"/[label(Shion)]", "Oh, you could at least play along with the morning greetings!", "/[waitkey(64)]/[close()]"};
    String[] msg0262BD8A = new String[]{"/[label(KOS-MOS)]", "What is the matter, Shion?", "/[waitkey(64)]/[clear()]"};
    String[] msg0262BD8B = new String[]{"/[label(Shion)]", "Oh, no, it's nothing. Something's not right with the catapult, so I came to check it out.", "/[waitkey(64)]/[clear()]"};
    String[] msg0262BD8C = new String[]{"/[label(KOS-MOS)]", "Shion, I believe you already realize this, but this is not the catapult.", "/[waitkey(1)]/[clear()]", "If you are to going to inspect it, shouldn't you go to the catapult deck at the bow?", "/[waitkey(64)]/[clear()]"};
    String[] msg0262BD8D = new String[]{"/[label(Shion)]", "Thanks, I know that already. You didn't have to tell me.", "/[waitkey(64)]/[close()]"};
    String[] msg01631D39 = new String[]{"/[label(Shion)]", "I came here because I was worried about you.", "/[waitkey(1)]/[clear()]", "Really, why can't you understand my parental concerns for you?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D3A = new String[]{"/[label(KOS-MOS)]", "/[label(KOS-MOS)]", "I am grateful for your concern for me.", "/[waitkey(1)]/[clear()]", "But right now, I believe inspecting the catapult has a higher priority.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D3B = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg0362BD8A = new String[]{"/[label(KOS-MOS)]", "Shion, have you finished inspecting the catapult?", "/[waitkey(64)]/[clear()]"};
    String[] msg0362BD8B = new String[]{"/[label(Shion)]", "What? Yes, I'm done. There doesn't seem to be any problems.", "/[waitkey(64)]/[clear()]"};
    String[] msg0362BD8C = new String[]{"/[label(KOS-MOS)]", "I see.", "/[waitkey(1)]/[clear()]", "In that case, I think you should hurry and report your findings to the Captain, rather than wander around here.", "/[waitkey(64)]/[clear()]"};
    String[] msg0262BD8E = new String[]{"/[label(Shion)]", "...I know that.", "/[waitkey(64)]/[close()]"};
    String[] msg01631D4F = new String[]{"/[label(Shion)]", "This is KOS-MOS' maintenance lab.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D50 = new String[]{"/[label(MOMO)]", "So this is KOS-MOS' room...", "/[waitkey(64)]/[clear()]"};
    String[] msg11631D50 = new String[]{"/[label(MOMO)]", "Oh, so is that her bed?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D51 = new String[]{"/[label(Shion)]", "Yes, well, something like that.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D52 = new String[]{"/[label(MOMO)]", "She's so lucky! I hope I get a room of my own soon too!", "/[waitkey(64)]/[close()]"};
    String[] msg0462BD8A = new String[]{"/[label(KOS-MOS)]", "What is the matter, Shion? Is there an emergency?", "/[waitkey(64)]/[clear()]"};
    String[] msg0462BD8B = new String[]{"/[label(SHION)]", "Commander Cherenkov has been missing. I'm going to go look for him.", "/[waitkey(64)]/[clear()]"};
    String[] msg0462BD8C = new String[]{"/[label(KOS-MOS)]", "Shall I join you in your search?", "/[waitkey(64)]/[clear()]"};
    String[] msg0362BD8D = new String[]{"/[label(Shion)]", "Oh, that's okay. It's in the city, so we can't do anything flashy anyway. I'll call you if I need you, so get some rest.", "/[waitkey(64)]/[clear()]"};
    String[] msg0362BD8E = new String[]{"/[label(KOS-MOS)]", "Affirmative.", "/[waitkey(64)]/[close()]"};
    String[] msg0436E834 = new String[]{"/[label(Cherenkov)]", "Oh, that curry was well...it was very good.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E837 = new String[]{"/[label(Shion)]", "Commander, just say \"thank-you-for-dinner.\"", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E838 = new String[]{"/[label(Cherenkov)]", "Oh, right, t-thank you for...dinner.", "/[waitkey(64)]/[clear()]"};
    String[] msg0436E839 = new String[]{"/[label(Shion)]", "You're welcome. I hope you really liked it!", "/[waitkey(64)]/[close()]"};
    String[] msg043747E6 = new String[]{"/[label(Shion)]", "Commander, I know what happened on the Woglinde was truly horrific. But this is a civilian passenger freighter, and we are simply here as guests. I don't think you\nneed to be such a stickler to the ways of the military.", "/[waitkey(64)]/[clear()]"};
    String[] msg043747E7 = new String[]{"/[label(Cherenkov)]", "I see...That's true. I'll try to be more careful.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgoyasumi = new String[]{"/[label(Shion)]", "(She's sleeping soundly.)\n", "Good night, KOS-MOS.", "/[waitkey(64)]/[close()]"};

    ST0571() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.675f, 0.951f, 0.115f);
        this.camEV.setRotate(1.952f, -151.195f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.57f, 2.135f, -2.357f);
        this.camEV.setRotate(-18.782f, -154.218f, 0.0f);
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
        ST0571.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                return;
            }
        }
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc9_1(Window window) {
    }

    public void Talk_oyasumi(Enepc enepc, Window window) {
        window.print(this.msgoyasumi, 0);
        ST0571.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(621, 1);
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
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(0, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 1.0f, 0.0f, -6.7f, 0.0f);
        this.teiten1.SetBgm(196624);
        this.teiten2 = new Uwamono(28690, -0.5f, 0.0f, 0.0f, 0.0f);
        this.teiten2.SetBgm(196636);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(1, 1.0f, 1.0f);
        this.Tub = new Obj();
        this.Tub.init(20555, -0.5f, 0.0f, 0.0f, 0.0f);
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
        this.doorA = new Uwamono(9, 40, '\u0001');
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

