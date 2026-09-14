import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0581
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS08_PRJ {
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
    Unit Bed;
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
    int BUTTON_F = 0;
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
    Uwamono doorB;
    Uwamono doorC;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect eve00;
    Effect EF01;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;
    String[] msg011C5348 = new String[]{"/[label(Shion)]", "Oh, chaos! Were you able to talk with KOS-MOS?", "/[waitkey(64)]/[clear()]"};
    String[] msg011C5349 = new String[]{"/[label(chaos)]", "No, she seemed to be asleep, so I didn't want to bother her. I'll try again later.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534A = new String[]{"/[label(Shion)]", "Oh, that's too bad.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534B = new String[]{"/[label(chaos)]", "But she was quite beautiful, even asleep.", "/[waitkey(64)]/[clear()]"};
    String[] msg011C534C = new String[]{"/[label(Shion)]", "Why thank you.", "/[waitkey(64)]/[close()]"};
    String[] msg0436E833 = new String[]{"/[label(Cherenkov)]", "The build of this ship is rather impressive for a passenger freighter. With this much equipment, it could even stand up to a military landing ship. It seems the Captain is quite a deceiver.", "/[waitkey(64)]/[close()]"};
    String[] msg1436E833 = new String[]{"/[label(Cherenkov)]", "You're finally here. Listen, the enemy mother ship is back there. Don't let your guard down!", "/[waitkey(64)]/[close()]"};
    String[] msg01631D47 = new String[]{"/[label(Shion)]", "I wonder if this is a vacant room?", "/[waitkey(64)]/[clear()]"};
    String[] msg11631D47 = new String[]{"/[label(Shion)]", "Perfect, we can set up your maintenance chair in here, Ziggy.", "/[waitkey(64)]/[clear()]"};
    String[] msg21631D47 = new String[]{"/[label(Shion)]", "It's a little drab in here, but is that okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D48 = new String[]{"/[label(Ziggy)]", "Sure, that will be fine.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D49 = new String[]{"/[label(MOMO)]", "What?! I can't stay here too? I want to be with Ziggy!", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D4A = new String[]{"/[label(Shion)]", "Hmm...but look, this room isn't that big.", "/[waitkey(64)]/[clear()]"};
    String[] msg11631D4A = new String[]{"/[label(Shion)]", "Also, I doubt anything will happen, but it'll be too late if something goes wrong.", "/[waitkey(64)]/[close()]"};
    String[] msg01631D4B = new String[]{"/[label(MOMO)]", "?", "\n", "Ziggy, what did she mean? What might go wrong?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D4C = new String[]{"/[label(Ziggy)]", "Hmm...well, she means, take good care of yourself.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D4D = new String[]{"/[label(MOMO)]", "??", "/[waitkey(64)]/[close()]"};
    String[] msg010FE371 = new String[]{"/[label(Shion)]", "Oh, hi Ziggy! What are you doing?", "/[waitkey(64)]/[clear()]"};
    String[] msg010FE372 = new String[]{"/[label(Ziggy)]", "Oh, I thought I would at least build my own bed.", "/[waitkey(64)]/[clear()]"};
    String[] msg010FE373 = new String[]{"/[label(Shion)]", "Wow, you're pretty handy.", "/[waitkey(64)]/[clear()]"};
    String[] msg010FE374 = new String[]{"/[label(Ziggy)]", "Though it is funny that a machine like me is preparing a bed for himself.", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37B = new String[]{"/[label(Ziggy)]", "By the way, didn't you need something?", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37C = new String[]{"/[label(Shion)]", "Oh, right! Commander Cherenkov is missing. Will you help me look for him?", "/[waitkey(64)]/[close()]"};
    String[] msg011FE37E = new String[]{"/[label(Ziggy)]", "The Commander can take care of himself. I do not think you need to be overly concerned.", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE37F = new String[]{"/[label(Shion)]", "But in the event something did happen, I would feel more secure if you were with me.", "/[waitkey(64)]/[clear()]"};
    String[] msg011FE381 = new String[]{"/[label(Ziggy)]", "All right. We cannot have him delaying our departure. I will help you look for him.", "/[waitkey(64)]/[close()]"};
    String[] msgdenji = new String[]{"/[label(Warning)]", "Safety lock of electromagnetic floor confirmed.", "/[waitkey(1)]/[clear()]", "The anti-intruder program cannot be activated at this time.", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};

    ST0581() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.996f, 1.815f, 5.338f);
        this.camEV.setRotate(-11.034f, -31.018f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.434f, 1.079f, 3.966f);
        this.camEV.setRotate(5.316f, -38.858f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.123f, 2.903f, -2.573f);
        this.camEV.setRotate(-23.822f, -214.337f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.244f, 1.751f, 1.464f);
        this.camEV.setRotate(-7.562f, -78.058f, 0.0f);
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
            case 3: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgdenji, 0);
                ST0581.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    void Talk_no() {
        this.win.print(this.msgtalk_no, 0);
        ST0581.waitPage(this.win, 64);
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0581.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc4_1(Window window) {
    }

    public void Talk_npc7(Enepc enepc) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no();
        } else {
            this.Talk_no();
        }
    }

    void Talk_npc7_1() {
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_no(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc8_1(Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(551, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(541, 3);
                break;
            }
            case 2: {
                Runtime.jumpCF(621, 2);
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
        if (Runtime.getFlags(301, 1) == 1) {
            this.Bed = new Obj();
            this.Bed.init(20579, 0.5f, -0.2f, 0.0f, 0.0f);
            this.player.setID(2);
        } else {
            this.player.setID(1);
        }
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(0, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 2.8f, 0.0f, -7.0f, 0.0f);
        this.teiten1.SetBgm(196625);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.03f, 0.03f);
        this.cam0.setCFAngle(2, -28.0f, -20.0f, 0.0f, 5.5f, 40.0f);
        this.cam0.setCFHokan(2, 0.03f, 0.03f);
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
        this.eve00 = new Effect(1444, 0);
        this.eve00.disp(true);
        this.EF01 = new Effect(1011, 2.825f, 1.2f, -6.6f, 0.0f);
        this.EF01.setRotate(-130.0f, 180.0f, 0.0f);
        this.EF01.setScale(0.35f, 0.35f, 1.5f);
        this.EF01.disp(true);
        new Uwamono(28678, -2.0f, 0.0f, -4.5f);
        this.doorA = new Uwamono(11, 40, '\u0001');
        this.doorB = new Uwamono(21, 40, '\u0001');
        this.doorC = new Uwamono(4, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
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

