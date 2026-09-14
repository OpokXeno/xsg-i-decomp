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

class ST1770
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
    Effect fade;
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
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    MAPUnit space;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    int page;
    String[] HYAKU_00 = new String[]{"Little Master,", "/[waitkey(1)]/[clear()]", "I cannot comprehend this scenery even though I have tried. Am I useless on this ship?", "/[waitkey(64)]/[close()]"};
    String[] HYAKU_01 = new String[]{"Will there come a day when even I can see this scenery as beautiful?", "/[waitkey(64)]/[close()]"};
    String[] HYAKU_02 = new String[]{"What does it mean that this scenery is beautiful?", "/[waitkey(1)]/[clear()]", "My eyes can only recognize it as a collection of numerical values and data.", "/[waitkey(64)]/[close()]"};
    String[] HYAKU_03 = new String[]{"The mission that we have been given is to observe all phenomena.", "/[waitkey(1)]/[clear()]", "Do we really need any other emotions?", "/[waitkey(64)]/[close()]"};
    String[] HYAKU_04 = new String[]{"Does the fact that I do not think this scenery is beautiful make me less of a person?", "/[waitkey(64)]/[close()]"};
    String[] REAL_00 = new String[]{"Oh, Little Master?", "/[waitkey(1)]/[clear()]", "What are you up to? Cruising for chicks again? I heard you're going after 100-Series these days.", "/[waitkey(64)]/[close()]"};
    String[] REAL_01 = new String[]{"Lucky you. It must be nice to be you.", "/[waitkey(64)]/[close()]"};
    String[] REAL_02 = new String[]{"We Realians have our personalities set to our respective environments. Those in the military are more like soldiers, and those in private sectors are more like civilians.", "/[waitkey(64)]/[close()]"};
    String[] CREW_00 = new String[]{"Little Master is so lucky. He's so popular. I can't even attract one person, you know?", "/[waitkey(64)]/[close()]"};
    String[] CREW_01 = new String[]{"Oh, Little M-Master! Ha ha ha, o-out for a walk? Uh, um, would you like some candy?", "/[waitkey(64)]/[close()]"};
    String[] CREW_02 = new String[]{"This is a place to be alone with your girlfriend.", "/[waitkey(1)]/[clear()]", "But there are only Realians onboard this ship. Man, I wish I had a girlfriend.", "/[waitkey(64)]/[close()]"};
    String[] CREW_03 = new String[]{"Maybe I'll ask one of the bridge's 100-Series.", "/[waitkey(1)]/[clear()]", "Having a Realian girlfriend is pretty trendy after all.", "/[waitkey(64)]/[close()]"};
    String[] P_00 = new String[]{"Oh, Little Master? Are you eating properly?", "/[waitkey(1)]/[clear()]", "You're growing boy right now, so you need to eat properly.", "/[waitkey(64)]/[close()]"};
    String[] P_01 = new String[]{"It sure is amazing.", "/[waitkey(1)]/[clear()]", "Seeing the scenery here makes me feel really glad that I work here on the Durandal. I wish my kids could see it just once.", "/[waitkey(64)]/[close()]"};
    String[] AL00 = new String[]{"/[label(Allen)]", "Come to think of it, Chief, aren't you tired too?", "/[waitkey(1)]/[clear()]", "They freed up one of the rooms in the Residential Area for us, so you can get some rest there.", "/[waitkey(1)]/[clear()]", "I was told it's the room with the red carpet. Shelley told me that you'd know as soon as you go in.", "/[waitkey(64)]/[close()]"};
    String[] AL01 = new String[]{"/[label(Allen)]", "This ship is amazing, isn't it? The onboard weaponry surpasses that of Federation battleships. And the amenities equal those of a luxury cruise liner.", "/[waitkey(1)]/[clear()]", "You could enjoy a leisurely voyage in this!", "/[waitkey(64)]/[close()]"};
    String[] KOS_00 = new String[]{"/[label(KOS-MOS)]", "I concur with your opinion.", "/[waitkey(1)]/[clear()]", "Without Dr. Mizrahi's research, our technology would not have developed as far as it has.", "/[waitkey(64)]/[close()]"};
    String[] MOM_00 = new String[]{"/[label(MOMO)]", "KOS-MOS is a nice person.", "/[waitkey(1)]/[clear()]", "Thank you for cheering me up. I'm okay now.", "/[waitkey(64)]/[close()]"};
    String[] S01_00 = new String[]{"Are you looking for someone? I see, you're looking for Allen? I believe he went towards the Foundation.", "/[waitkey(1)]/[clear()]", "If you hurry, I think you'll be able to catch him!", "/[waitkey(64)]/[close()]"};
    String[] S02_00 = new String[]{"Well, having an easygoing personality is such a problem.", "/[waitkey(1)]/[clear()]", "If I slack off once, it takes a lot for me to feel like working again.", "/[waitkey(1)]/[clear()]", "Man, I've certainly had a troublesome personality implanted into me.", "/[waitkey(64)]/[close()]"};
    String[] S03_00 = new String[]{"Man, talk about a cold sweat. Those Federation guys almost killed me.", "/[waitkey(1)]/[clear()]", "I don't even have a girlfriend yet. I don't want to die!", "/[waitkey(64)]/[close()]"};
    String[] S04_00 = new String[]{"Sheesh, that's why I hate soldiers!", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST1770() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(7128, 1) != 1) return;
                    if (Runtime.getFlags(3221, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.EF05.disp(false);
                    Sound.effectPlay(6);
                    Runtime.addItemWin(10, 16);
                    Runtime.setFlags(3221, 1, 1);
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 != 1) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3176, 1) != 0) return;
                if (Runtime.getFlags(304, 1) != 1) return;
                if (Runtime.getFlags(310, 1) != 0) return;
                if (Runtime.mailReplyCheck(49) == 1) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.mailArriveSet(53);
                    Runtime.setFlags(3176, 1, 1);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Read email\nDon't read email");
                    System.waitFor(this.menu);
                    System.sleep(10);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.mailExec(1);
                            Runtime.setPlayerControl(true);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.mailReplyCheck(49) == 2) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.mailArriveSet(54);
                    Runtime.setFlags(3176, 1, 1);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Read email\nDon't read email");
                    System.waitFor(this.menu);
                    System.sleep(10);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            Runtime.mailExec(1);
                            Runtime.setPlayerControl(true);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.mailReplyCheck(49) != 3) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgMAIL1, 0);
                Runtime.mailArriveSet(55);
                Runtime.setFlags(3176, 1, 1);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Read email\nDon't read email");
                System.waitFor(this.menu);
                System.sleep(10);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                return;
            }
        }
    }

    public void TalkALLEN(Enepc enepc) {
        if (this.npc5talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AL00, 0);
            System.waitFor(this.win);
            this.npc5talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AL01, 0);
            System.waitFor(this.win);
            this.npc5talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkKOSMOS(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.KOS_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkMOMO(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.MOM_00, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc1btalked == 0) {
                window.print(this.HYAKU_00, 0);
                System.waitFor(window);
                this.npc1btalked = 1;
            } else {
                window.print(this.HYAKU_01, 0);
                System.waitFor(window);
                this.npc1btalked = 0;
            }
        } else if (this.npc1talked == 0) {
            window.print(this.HYAKU_02, 0);
            System.waitFor(window);
            this.npc1talked = 1;
        } else if (this.npc1talked == 1) {
            window.print(this.HYAKU_03, 0);
            System.waitFor(window);
            this.npc1talked = 2;
        } else {
            window.print(this.HYAKU_04, 0);
            System.waitFor(window);
            this.npc1talked = 0;
        }
    }

    public void TalkNPC11(Enepc enepc, Window window) {
        window.print(this.S01_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc2btalked == 0) {
                window.print(this.REAL_00, 0);
                System.waitFor(window);
                this.npc2btalked = 1;
            } else {
                window.print(this.REAL_01, 0);
                System.waitFor(window);
                this.npc2btalked = 0;
            }
        } else {
            window.print(this.REAL_02, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC22(Enepc enepc, Window window) {
        window.print(this.S02_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            if (this.npc3btalked == 0) {
                window.print(this.CREW_00, 0);
                System.waitFor(window);
                this.npc3btalked = 1;
            } else {
                window.print(this.CREW_01, 0);
                System.waitFor(window);
                this.npc3btalked = 0;
            }
        } else if (this.npc3talked == 0) {
            window.print(this.CREW_02, 0);
            System.waitFor(window);
            this.npc3btalked = 1;
        } else {
            window.print(this.CREW_03, 0);
            System.waitFor(window);
            this.npc3btalked = 0;
        }
    }

    public void TalkNPC33(Enepc enepc, Window window) {
        window.print(this.S03_00, 0);
        System.waitFor(window);
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.P_00, 0);
            System.waitFor(window);
        } else {
            window.print(this.P_01, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC44(Enepc enepc, Window window) {
        window.print(this.S04_00, 0);
        System.waitFor(window);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1850, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1850, 2);
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
        if (Runtime.getFlags(3159, 1) == 0) {
            Runtime.setOutFriend(2);
            System.println("MOGUMOGUMOGUMOGUMOGUMOGUMOGUMOGUMOGUMOGU");
            Runtime.setFlags(3159, 1, 1);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 2);
            Runtime.setPartyData(0x1010004, 0);
            Runtime.setPartyData(65542, 0);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
        }
        Stage.setVisible(-1, true);
        if (Runtime.getFlags(305, 1) == 0) {
            this.EF02 = new Effect(1493, 1);
            this.EF02.disp(true);
            this.EF02.setClip(true);
            this.EF03 = new Effect(1493, 2);
            this.EF03.disp(true);
            this.EF03.setClip(true);
            this.EF04 = new Effect(1493, 3);
            this.EF04.disp(true);
            this.EF04.setClip(true);
        }
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
        if (Runtime.getFlags(310, 1) == 0) {
            Stage.setVisible(55, false);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        if (Runtime.getFlags(305, 1) == 0) {
            Stage.setColor(0.6f, 0.72f, 0.84f);
            this.light.setColor(0, 0.2f, 0.2f, 0.2f);
            this.light.setColor(1, 0.204f, 0.24f, 0.276f);
            this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
            this.light.setColor(2, 0.3f, 0.36f, 0.42f);
            this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
            this.light.setColor(3, 0.3f, 0.36f, 0.42f);
            this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
            Runtime.setIdLightCol(1, 0, 0.25f, 0.25f, 0.25f);
            Runtime.setIdLightCol(1, 1, 0.25f, 0.25f, 0.25f);
            Runtime.setIdLightCol(1, 2, 0.45f, 0.45f, 0.45f);
            Runtime.setIdLightCol(1, 3, 0.45f, 0.45f, 0.45f);
            Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
            Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
            Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
            Runtime.setIdLightCol(2, 0, 0.25f, 0.25f, 0.25f);
            Runtime.setIdLightCol(2, 1, 0.25f, 0.25f, 0.25f);
            Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.5f);
            Runtime.setIdLightCol(2, 3, 0.4f, 0.4f, 0.5f);
            Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
            Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
            Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
            this.light01 = new Effect(1682, 0.0f, -4.0f, 5.0f, 0.0f);
            this.light01.noAttach(false);
            this.light01.disp(true);
            this.light01.setScale(3.0f, 3.0f, 3.0f);
            this.light01.setClip(false);
            this.light02 = new Effect(1682, 0.0f, -4.0f, 5.0f, 0.0f);
            this.light02.noAttach(false);
            this.light02.disp(true);
            this.light02.setScale(3.0f, 3.0f, 3.0f);
            this.light02.setClip(false);
            Runtime.progressEffect(60);
        } else {
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
        }
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
        this.npc1 = new NPC_NORMAL(1028, 11, 0, 0, 3, -1.876f, 1.0f, -9.438f, 180.0f);
        this.npc1.disableDTKFlag(131075);
        this.npc1.enableDTKFlag(12);
        this.npc1.setInvalidID(1);
        this.npc2 = new NPC_NORMAL(780, 12, 0, 0, 11, -14.2f, 0.898f, 4.833f, 45.0f);
        this.npc2.disableDTKFlag(131075);
        this.npc2.enableDTKFlag(12);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 10);
        this.npc2.enableDTKFlag(4);
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
        if (Runtime.getFlags(346, 1) == 0) {
            this.npc1.talkto("TalkNPC1");
            this.npc2.talkto("TalkNPC2");
            this.npc3.talkto("TalkNPC3");
            this.npc4.talkto("TalkNPC4");
        } else {
            this.npc1.talkto("TalkNPC11");
            this.npc2.talkto("TalkNPC22");
            this.npc3.talkto("TalkNPC33");
            this.npc4.talkto("TalkNPC44");
        }
        if (Runtime.getFlags(304, 1) == 1 && Runtime.getFlags(310, 1) == 0) {
            this.npc5 = new NPC_NORMAL(263, 15, 0, 0, 20, -5.616f, 1.0f, -7.454f, -45.0f);
            this.npc5.disableDTKFlag(131082);
            this.npc5.enableDTKFlag(4);
            this.npc5.talkto("TalkALLEN");
            this.npc6 = new NPC_NORMAL(4, 16, 0, 0, 23, -6.657f, 1.0f, -9.763f, 200.0f);
            this.npc6.disableDTKFlag(131083);
            this.npc6.enableDTKFlag(4);
            this.npc6.setMotion(0, 27);
            this.npc6.talkto("TalkMOMO");
            this.npc7 = new NPC_NORMAL(2, 17, 0, 0, 26, -7.351f, 1.0f, -7.454f, 75.0f);
            this.npc7.disableDTKFlag(131082);
            this.npc7.enableDTKFlag(4);
            this.npc7.setMotion(0, 27);
            this.npc7.talkto("TalkKOSMOS");
        }
        this.space = new Mapunits();
        this.space.mapUnit(36);
        this.space.start(4, null);
        this.space.start(1, "idle");
        if (Runtime.getFlags(3188, 1) == 0) {
            this.player.setTranslate(-4.0f, 1.0f, -8.0f);
            Runtime.setFlags(3188, 1, 1);
        }
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

        void idle() {
            float f = 0.0f;
            while (true) {
                ST1770.this.space.setRotateY(-f);
                if ((f += 0.02f) == 360.0f) {
                    f = 0.0f;
                }
                System.sleep(1);
            }
        }
    }
}

