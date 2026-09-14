import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK14B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0400
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK14B_PRJ {
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
    Uwamono doorA;
    Uwamono doorB;
    Uwamono kon01;
    Uwamono kon02;
    Uwamono kon03;
    Uwamono kon04;
    Uwamono item01;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    int page;
    String[] msg25C86B6C = new String[]{"/[label(Holgar)]", "Oh, it's you...S-Shion. It's d-dangerous here...Get out of here...hurry...", "/[waitkey(64)]/[close()]"};
    String[] msg25C86B6D = new String[]{"/[label(Shion)]", "Holgar! Hang in there!!", "/[waitkey(64)]/[close()]"};
    String[] msg25C86B6E = new String[]{"/[label(Holgar)]", "Never mind, hu-rry...Do-n't...you...die...Shi-o-n...", "/[waitkey(64)]/[close()]"};
    String[] msg25C86B6F = new String[]{"/[label(Shion)]", "No, Holgar...", "/[waitkey(64)]/[close()]"};
    String[] msgOther1 = new String[]{"/[label(Holgar)]", "It's d-dangerous here...Hurry and get out...", "/[waitkey(64)]/[close()]"};
    String[] msgOther2 = new String[]{"/[label(Shion)]", "Hang in there!!", "/[waitkey(64)]/[close()]"};
    String[] msgOther3 = new String[]{"/[label(Holgar)]", "Never mind, hu-rry...You have to...ru...", "/[waitkey(64)]/[close()]"};
    String[] msgOther4 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg25C86B70 = new String[]{"/[label(Holgar)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg57ADD19F = new String[]{"He's dead.", "/[waitkey(64)]/[close()]"};
    String[] msgDRILL01 = new String[]{"/[label(Shion)]", "Holgar...", "/[waitkey(64)]/[clear()]", "...? There's something shiny...", "/[waitkey(64)]/[close()]"};
    String[] msgOther5 = new String[]{"/[label(Shion)]", "...? There's something shiny...", "/[waitkey(64)]/[close()]"};
    String[] msgDRILL02 = new String[]{"You got a Drill Passport!", "/[waitkey(64)]/[close()]"};

    ST0400() {
    }

    void Final_init(int n) {
    }

    public void TalkNPC1(Enepc enepc) {
        if (Runtime.getFlags(3006, 1) == 0) {
            if (Runtime.getFlags(4003, 1) == 1) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg25C86B6C, 0);
                ST0400.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg25C86B6D, 0);
                ST0400.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg25C86B6E, 0);
                ST0400.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg25C86B6F, 0);
                ST0400.waitPage(this.win, 64);
                this.npc1.kickEnepc(4, 2);
                this.npc1.disableDTKFlag(131072);
                this.npc1.disableDTKFlag(1);
                this.npc1.disableDTKFlag(2);
                Runtime.setFlags(3006, 1, 1);
                Runtime.setFlags(4074, 1, 1);
                return;
            }
            if (Runtime.getFlags(4003, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgOther1, 0);
                ST0400.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgOther2, 0);
                ST0400.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgOther3, 0);
                ST0400.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgOther4, 0);
                ST0400.waitPage(this.win, 64);
                this.npc1.kickEnepc(4, 2);
                this.npc1.disableDTKFlag(131072);
                this.npc1.disableDTKFlag(1);
                this.npc1.disableDTKFlag(2);
                Runtime.setFlags(3006, 1, 1);
                Runtime.setFlags(4074, 1, 1);
                return;
            }
        } else if (Runtime.getFlags(4001, 1) == 0) {
            if (Runtime.getFlags(4003, 1) == 1) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgDRILL01, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("I should take it\nI should just leave it");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.EF06.disp(false);
                        Sound.effectPlay(6);
                        Runtime.addItemWin(10, 15);
                        Runtime.setFlags(4001, 1, 1);
                        return;
                    }
                    case 1: {
                        return;
                    }
                }
                return;
            }
            if (Runtime.getFlags(4003, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgOther5, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("I should take it\nI should just leave it");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.EF06.disp(false);
                        Sound.effectPlay(6);
                        Runtime.addItemWin(10, 15);
                        Runtime.setFlags(4001, 1, 1);
                        return;
                    }
                    case 1: {
                        return;
                    }
                }
                return;
            }
        } else if (Runtime.getFlags(4001, 1) == 1) {
            this.npc1.kickEnepc(4, 2);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msg25C86B70, 0);
            ST0400.waitPage(this.win, 64);
            return;
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        window.print(this.msg57ADD19F, 0);
        ST0400.waitPage(window, 64);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(65887, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(65876, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 0.8f, 0.0f, -10.5f, 0.0f);
        this.teiten1.SetBgm(196643);
        this.teiten2 = new Uwamono(28690, 2.0f, 0.0f, -2.0f, 0.0f);
        this.teiten2.SetBgm(196644);
        this.teiten3 = new Uwamono(28690, -1.0f, 0.0f, -1.0f, 0.0f);
        this.teiten3.SetBgm(196644);
        this.teiten4 = new Uwamono(28690, 5.0f, 0.0f, -12.0f, 90.0f);
        this.teiten4.SetBgm(196638);
        this.teiten4.SetBgmType('\u0001');
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.EF01 = new Effect(1403, -1.15f, -2.6f, 0.0f, 0.0f);
        this.EF01.disp(true);
        this.EF01.setClip(true);
        this.EF02 = new Effect(1401, 1.73f, -1.0f, -1.31f, 0.0f);
        this.EF02.disp(true);
        this.EF02.setClip(true);
        this.EF03 = new Effect(1402, 0.222f, -1.0f, -10.367f, 0.0f);
        this.EF03.disp(true);
        this.EF03.setClip(true);
        this.EF04 = new Effect(1528, 2.0f, 0.0f, -13.3f, 180.0f);
        this.EF04.disp(true);
        this.EF04.setClip(true);
        this.EF05 = new Effect(1528, -4.3f, 0.0f, -21.2f, -90.0f);
        this.EF05.disp(true);
        this.EF05.setClip(true);
        if (Runtime.getFlags(4001, 1) == 0) {
            this.EF06 = new Effect(1018, 2.0f, 0.3f, -13.1f, 0.0f);
            this.EF06.disp(true);
            this.EF06.setClip(true);
        }
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(122, false);
        Stage.setVisible(123, false);
        Stage.setVisible(124, false);
        Stage.setVisible(125, false);
        Stage.setVisible(126, false);
        Stage.setVisible(127, false);
        Stage.setVisible(144, false);
        Stage.setVisible(138, false);
        Stage.setVisible(3, false);
        Stage.setVisible(136, false);
        Stage.setVisible(147, false);
        Stage.setVisible(146, false);
        Stage.setVisible(145, false);
        Stage.setVisible(142, false);
        Stage.setVisible(141, false);
        Stage.setVisible(140, false);
        Stage.setVisible(139, false);
        Stage.setVisible(52, false);
        Stage.setVisible(137, false);
        Stage.setVisible(53, false);
        Stage.setVisible(34, false);
        Stage.setVisible(35, false);
        Stage.setVisible(37, false);
        Stage.setVisible(38, false);
        Stage.setVisible(135, false);
        Stage.setVisible(63, false);
        Stage.setVisible(61, false);
        Stage.setVisible(62, false);
        Stage.setVisible(143, false);
        Stage.setVisible(84, false);
        Stage.setVisible(115, false);
        Stage.setVisible(109, false);
        Stage.setVisible(81, false);
        Stage.setVisible(121, false);
        Stage.setVisible(163, false);
        Stage.setVisible(156, false);
        Stage.setVisible(165, false);
        Stage.setVisible(112, false);
        Stage.setVisible(111, false);
        Stage.setVisible(98, false);
        Stage.setVisible(97, false);
        Stage.setVisible(85, false);
        Stage.setVisible(87, false);
        Stage.setVisible(86, false);
        Stage.setVisible(96, false);
        Stage.setVisible(95, false);
        Stage.setVisible(88, false);
        Stage.setVisible(99, false);
        Stage.setVisible(94, false);
        Stage.setVisible(89, false);
        Stage.setVisible(92, false);
        Stage.setVisible(93, false);
        Stage.setVisible(91, false);
        Stage.setVisible(90, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.npc1 = new NPC_NORMAL(1616, 1, 0, 8, 4, 2.0f, 0.0f, -13.3f, 180.0f);
        this.npc1.talkto("TalkNPC1");
        this.npc1.disableDTKFlag(8);
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(1);
        this.npc1.disableDTKFlag(2);
        this.npc1.setMotion(0, 2);
        this.npc2 = new NPC_NORMAL(537, 2, 0, 8, 4, -4.3f, 0.0f, -21.2f, -90.0f);
        this.npc2.talkto("TalkNPC2");
        this.npc2.disableDTKFlag(8);
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(1);
        this.npc2.disableDTKFlag(2);
        this.npc2.setMotion(0, 4);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 370.0f, 0.0f, 18.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 20.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.doorA = new Uwamono(134, 42, '\u0001');
        new Uwamono(133, 42, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(132, 42, '\u0001');
        new Uwamono(131, 42, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.kon01 = new Uwamono(28712, 4.703f, 0.0f, -8.837f, -36.0f);
        this.kon01.SetParticle(628);
        this.kon01.SetBroken(true);
        this.kon02 = new Uwamono(28712, 4.652f, 0.0f, -10.031f, 0.0f);
        this.kon02.SetParticle(628);
        this.kon02.SetBroken(true);
        this.kon03 = new Uwamono(28712, 3.658f, 0.0f, -9.454f, 0.0f);
        this.kon03.SetParticle(628);
        this.kon03.SetBroken(true);
        this.item01 = new Uwamono(28677, 3.649f, 0.0f, -9.0f, 45.0f, 26);
        this.kon04 = new Uwamono(28712, 3.649f, 0.0f, -8.25f, 45.0f, this.item01);
        this.kon04.SetParticle(628);
        this.kon04.SetBroken(true);
        this.item01.SetSymbol(28683);
        this.kon01.SetSize(1.8f, 2.0f, 1.8f);
        this.kon02.SetSize(1.8f, 2.0f, 1.8f);
        this.kon03.SetSize(1.8f, 2.0f, 1.8f);
        this.kon04.SetSize(1.8f, 2.0f, 1.8f);
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
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

    class People
            extends Enepc {
        People() {
        }

        void init() {
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
}

