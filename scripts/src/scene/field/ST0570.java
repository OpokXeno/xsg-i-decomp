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

class ST0570
        extends Stage
        implements XenoConstants,
        CfConstants,
        JNT_Accesories,
        JNT_Human,
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
    Unit curry;
    Effect EFcurry;
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
    String[] msg01631D4F = new String[]{"/[label(Shion)]", "This used to be a vacant room, but I'm borrowing it to use as a maintenance lab for KOS-MOS.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D50 = new String[]{"/[label(MOMO)]", "Is that KOS-MOS' maintenance bed?", "/[waitkey(64)]/[close()]"};
    String[] msg11631D50 = new String[]{"/[label(Shion)]", "Yes, after being active for a set amount of time, she sleeps there and her data is uploaded to the company Headquarters. I can also feedback the activity data to update the O.S.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D51 = new String[]{"/[label(MOMO)]", "So KOS-MOS sleeps too?\n", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D52 = new String[]{"/[label(Shion)]", "Yes. Even if I disconnect her from the outside world, her central systems remain active.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D53 = new String[]{"/[label(MOMO)]", "I wonder what kind of dreams she sees?", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D54 = new String[]{"/[label(Shion)]", "Dreams...hmm...", "/[waitkey(1)]/[clear()]", "That's probably something only KOS-MOS would know.", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D55 = new String[]{"/[label(MOMO)]", "I hope she has sweet dreams.\n", "/[waitkey(64)]/[clear()]"};
    String[] msg01631D56 = new String[]{"/[label(Shion)]", "So do I.", "/[waitkey(64)]/[close()]"};
    String[] msg0462BD8A = new String[]{"/[label(KOS-MOS)]", "What is the matter, Shion? Is there an emergency?", "/[waitkey(64)]/[clear()]"};
    String[] msg0462BD8A1 = new String[]{"/[label(KOS-MOS)]", "What is the matter? If you are looking for Commander Cherenkov, he is not here.", "/[waitkey(64)]/[close()]"};
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

    ST0570() {
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
        ST0570.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_no(window);
        } else if (this.S2014B == 1) {
            this.Talk_no(window);
        } else if (this.S2014 == 1) {
            this.Talk_no(window);
        } else if (this.S2013B == 1) {
            this.Talk_npc8_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg0436E834, 0);
                ST0570.waitPage(window, 64);
                window.print(this.msg0436E837, 0);
                ST0570.waitPage(window, 64);
                window.print(this.msg0436E838, 0);
                ST0570.waitPage(window, 64);
                window.print(this.msg0436E839, 0);
                ST0570.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg043747E6, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg043747E7, 0);
        ST0570.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (this.S2057 == 1) {
            this.Talk_no(window);
        } else if (this.S2042 == 1) {
            this.Talk_no(window);
        } else if (this.S2040C == 1) {
            this.Talk_npc9_5(window);
        } else if (this.S2030 == 1) {
            this.Talk_npc9_4(window);
        } else if (this.S2028 == 1) {
            this.Talk_no(window);
        } else if (this.S2014B == 1) {
            this.Talk_npc9_3(window);
        } else if (this.S2014 == 1) {
            this.Talk_npc9_2(window);
        } else if (this.S2013B == 1) {
            this.Talk_npc9_1(window);
        } else {
            this.Talk_no(window);
        }
    }

    void Talk_npc9_1(Window window) {
        window.print(this.msg0162BD8A, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg0162BD8B, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg0162BD8C, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg0162BD8D, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg0162BD8E, 0);
        ST0570.waitPage(window, 64);
    }

    void Talk_npc9_2(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg0262BD8A, 0);
                ST0570.waitPage(window, 64);
                window.print(this.msg0262BD8B, 0);
                ST0570.waitPage(window, 64);
                window.print(this.msg0262BD8C, 0);
                ST0570.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg0262BD8D, 0);
                ST0570.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg01631D39, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg01631D3A, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg01631D3B, 0);
        ST0570.waitPage(window, 64);
    }

    void Talk_npc9_3(Window window) {
        window.print(this.msg0362BD8A, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg0362BD8B, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg0362BD8C, 0);
        ST0570.waitPage(window, 64);
        window.print(this.msg0262BD8E, 0);
        ST0570.waitPage(window, 64);
    }

    void Talk_npc9_4(Window window) {
    }

    void Talk_npc9_5(Window window) {
        if (Runtime.getLeader() == 1) {
            window.print(this.msg0462BD8A, 0);
            ST0570.waitPage(window, 64);
            window.print(this.msg0462BD8B, 0);
            ST0570.waitPage(window, 64);
            window.print(this.msg0462BD8C, 0);
            ST0570.waitPage(window, 64);
            window.print(this.msg0362BD8D, 0);
            ST0570.waitPage(window, 64);
            window.print(this.msg0362BD8E, 0);
            ST0570.waitPage(window, 64);
        } else {
            window.print(this.msg0462BD8A1, 0);
            ST0570.waitPage(window, 64);
        }
    }

    public void Talk_oyasumi(Enepc enepc, Window window) {
        window.print(this.msgoyasumi, 0);
        ST0570.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(620, 1);
                break;
            }
        }
    }

    void init() {
        if (Runtime.getFlags(124, 1) != 1 && Runtime.getFlags(122, 1) == 1) {
            switch (Runtime.mailAttachCheck(11)) {
                case 1: {
                    Runtime.setFlags(7144, 1, 1);
                }
            }
        }
        if (Runtime.getFlags(123, 1) == 0) {
            this.curry = new Obj();
            this.curry.init(24640, 0.0f, 0.0f, 0.0f, 0.0f);
            this.curry.start(4, null);
            this.curry.setTranslate(0.15f, 0.1f, -0.068f);
            this.curry.setRotate(260.0f, 90.0f, 0.0f);
            this.curry.setParent(this.player, 60);
            this.EFcurry = new Effect(1531, 0.0f, 0.0f, 0.0f, 0.0f);
            this.EFcurry.disp(true);
            this.EFcurry.setCaster(this.curry);
            this.EFcurry.noAttach(false);
        }
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
        if (this.S2057 == 1) {
            this.npcset_0();
        } else if (this.S2042 == 1) {
            this.npcset_0();
        } else if (this.S2040C == 1) {
            this.npcset_5();
        } else if (this.S2030 == 1) {
            this.npcset_4();
        } else if (this.S2028 == 1) {
            this.npcset_0();
        } else if (this.S2014B == 1) {
            this.npcset_3();
        } else if (this.S2014 == 1) {
            this.npcset_2();
        } else if (this.S2013B == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc8 = new NPC_NORMAL(279, 8, 0, 5, 12, 0.98f, 0.0f, -1.34f, 0.0f);
        this.npc8.talkto("Talk_npc8");
        this.npc9 = new NPC_NORMAL(2, 9, 0, 14, 3, -0.55f, 0.0f, 0.0f, 0.0f);
        this.npc10 = new NPC_NORMAL(2, 10, 0, 14, 3, -0.55f, 0.0f, -1.19f, 0.0f);
        this.npc11 = new NPC_NORMAL(2, 11, 0, 14, 3, -0.55f, 0.0f, 1.14f, 0.0f);
        this.npc9.talkto("Talk_oyasumi");
        this.npc10.talkto("Talk_oyasumi");
        this.npc11.talkto("Talk_oyasumi");
        this.npc9.setInvalidID(1);
        this.npc10.setInvalidID(1);
        this.npc11.setInvalidID(1);
        this.npc9.setVisible(false);
        this.npc10.setVisible(false);
        this.npc11.setVisible(false);
    }

    void npcset_10() {
    }

    void npcset_2() {
        this.npc9 = new NPC_NORMAL(2, 9, 0, 6, 3, 1.0f, 0.0f, -0.97f, 0.0f);
        this.npc9.setMotion(0, 9);
        this.npc9.talkto("Talk_npc9");
    }

    void npcset_3() {
        this.npc9 = new NPC_NORMAL(2, 9, 0, 6, 3, 1.0f, 0.0f, -0.97f, 0.0f);
        this.npc9.setMotion(0, 9);
        this.npc9.talkto("Talk_npc9");
    }

    void npcset_4() {
        this.npc6 = new NPC_NORMAL(4, 6, 0, 12, 5, 1.39f, 0.0f, 4.06f, 180.0f);
        this.npc7 = new NPC_NORMAL(6, 7, 0, 17, 7, 1.64f, 0.0f, 5.19f, 180.0f);
        this.npc10 = new NPC_NORMAL(1, 10, 0, 13, 10, 0.17f, 0.0f, 4.11f, 180.0f);
        this.npc6.setVisible(false);
        this.npc6.enableDTKFlag(262144);
        this.npc6.kickEnepc(4, 1);
        this.npc6.disableDTKFlag(131072);
        this.npc6.disableDTKFlag(65536);
        this.npc7.setVisible(false);
        this.npc7.enableDTKFlag(262144);
        this.npc7.kickEnepc(4, 1);
        this.npc7.disableDTKFlag(131072);
        this.npc7.disableDTKFlag(65536);
        this.npc10.setVisible(false);
        this.npc10.enableDTKFlag(262144);
        this.npc10.kickEnepc(4, 1);
        this.npc10.disableDTKFlag(131072);
        this.npc10.disableDTKFlag(65536);
        if (Runtime.getFlags(7022, 1) == 1) {
            return;
        }
        this.npc11 = new NPC_NORMAL(1, 11, 0, 13, 10, 100.0f, 100.0f, 100.0f, 0.0f);
        this.npc11.talkto("TalkNPC11");
        this.npc11.setInvalidID(1);
        this.npc11.disableDTKFlag(131072);
        this.npc11.disableDTKFlag(8);
        this.npc11.start(1, "ANNAI");
        this.player.setTranslate(100.0f, 0.0f, 100.0f);
    }

    void npcset_5() {
        this.npc9 = new NPC_NORMAL(2, 9, 0, 6, 3, 1.0f, 0.0f, -0.97f, 0.0f);
        this.npc9.setMotion(0, 9);
        this.npc9.talkto("Talk_npc9");
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

        void ANNAI() {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7022, 1, 1);
            Runtime.enable(65536);
            ST0570.this.cam0.setMode(-1);
            ST0570.this.EV_Camera01();
            ST0570.this.npc10.kickEnepc(0, 9);
            ST0570.this.npc6.kickEnepc(1, 27);
            ST0570.this.npc7.kickEnepc(0, 27);
            ST0570.this.npc6.setVisible(true);
            ST0570.this.npc7.setVisible(true);
            ST0570.this.npc10.setVisible(true);
            System.sleep(10);
            ST0570.this.win = Window.create();
            ST0570.this.win.setSize(4, 45);
            ST0570.this.win.setLocation(15, 305);
            ST0570.this.win.print(ST0570.this.msg01631D4F, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc6.kickEnepc(0, 9);
            ST0570.this.win.print(ST0570.this.msg01631D50, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc6.kickEnepc(1, 1);
            ST0570.this.npc10.kickEnepc(1, 1);
            ST0570.this.npc7.kickEnepc(0, 27);
            ST0570.this.npc6.moveEnepc(15, 0.197f, 1.84f, 40);
            ST0570.this.npc6.moveEnepc(17, 200.0f, 0.1f, 10);
            ST0570.this.EV_Camera02();
            System.sleep(10);
            ST0570.this.npc10.moveEnepc(15, 0.09f, 2.94f, 40);
            System.sleep(40);
            ST0570.this.npc10.kickEnepc(0, 9);
            ST0570.this.npc6.kickEnepc(1, 10);
            ST0570.this.win = Window.create();
            ST0570.this.win.setSize(4, 45);
            ST0570.this.win.setLocation(15, 305);
            ST0570.this.win.print(ST0570.this.msg11631D50, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc6.kickEnepc(1, 9);
            ST0570.this.win.print(ST0570.this.msg01631D51, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc10.kickEnepc(0, 9);
            ST0570.this.win.print(ST0570.this.msg01631D52, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc6.kickEnepc(1, 10);
            ST0570.this.win.print(ST0570.this.msg01631D53, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc10.kickEnepc(0, 7);
            ST0570.this.win.print(ST0570.this.msg01631D54, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc6.kickEnepc(1, 10);
            ST0570.this.win.print(ST0570.this.msg01631D55, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            ST0570.this.npc10.kickEnepc(0, 7);
            ST0570.this.win.print(ST0570.this.msg01631D56, 0);
            ST0570.waitPage(ST0570.this.win, 64);
            System.sleep(10);
            ST0570.this.fade.call(0);
            System.sleep(30);
            ST0570.this.npc6.setVisible(false);
            ST0570.this.npc7.setVisible(false);
            ST0570.this.npc10.setVisible(false);
            ST0570.this.npc11.setVisible(false);
            ST0570.this.player.setTranslate(0.17f, 0.0f, 4.11f);
            ST0570.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }
}

