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
import xeno.map.MC_VOK09_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0090
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK09_PRJ {
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
    Unit Star;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npctalk1;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int touchFlag1;
    Uwamono doorA;
    int after_talk_1;
    int eveflag_1;
    int eveflag_2;
    Effect fade;
    int S014A;
    int S014B;
    int S015B;
    Light light = new Light(0);
    Uwamono teiten1;
    int page;
    String[] msg2D2C3660 = new String[]{"/[label()]", "Hm? What is it? Do you want something from me?", "/[waitkey(1)]/[clear()]", "Who could imagine that a woman as beautiful as you would appear in such a messy room. You're truly a \"trash in a dunghill!\"", "/[waitkey(64)]/[clear()]"};
    String[] msg2D2C3661 = new String[]{"/[label(Shion)]", "Umm...are you saying I am \"trash?\"", "/[waitkey(64)]/[clear()]"};
    String[] msg2D2C3662 = new String[]{"/[label()]", "That's right, \"trash!\"", "/[waitkey(64)]/[clear()]"};
    String[] msg2D2C3663 = new String[]{"/[label(Shion)]", "Um, are you sure you don't mean to say \"jewel?\"", "/[waitkey(64)]/[clear()]"};
    String[] msg2D2C3664 = new String[]{"/[label()]", "Oh, well you could say that, too! Well, well. \"Even Homer sometimes bobs.\" Everyone slips up.", "/[waitkey(64)]/[clear()]"};
    String[] msg2D2C3665 = new String[]{"/[label(Shion)]", "\"Even Homer sometimes nods?\"", "/[waitkey(64)]/[clear()]"};
    String[] msg2D2C3666 = new String[]{"/[label()]", "W-wait, is that how it goes?", "/[waitkey(64)]/[close()]"};
    String[] msg2D2C9611 = new String[]{"/[label()]", "Oops. That's strange. I swear I had it right when I taught it to that Realian.", "/[waitkey(64)]/[clear()]"};
    String[] msg2D2C9612 = new String[]{"/[label(Shion)]", "...\n", "(That poor Realian.)", "/[waitkey(64)]/[close()]"};
    String[] msg7469BEA8 = new String[]{"/[label()]", "So we meet again. I'm so lucky to meet a woman as beautiful as you, twice. You and I must be bound by fate, for \"fate will bring together those a thousand meals apart.\"", "/[waitkey(64)]/[clear()]"};
    String[] msg7469BEA9 = new String[]{"/[label(Shion)]", "\"Meals,\" you say...?", "/[waitkey(64)]/[clear()]"};
    String[] msg7469BEAA = new String[]{"/[label()]", "Yes, \"meals!\"", "/[waitkey(64)]/[clear()]"};
    String[] msg7469BEAB = new String[]{"/[label(Shion)]", "I think you mean \"miles,\" don't you?", "/[waitkey(64)]/[clear()]"};
    String[] msg7469BEAC = new String[]{"/[label()]", "...", "/[waitkey(64)]/[close()]"};

    ST0090() {
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

    public void Talk_npc1(Enepc enepc, Window window) {
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg2D2C3660, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3661, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3662, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3663, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3664, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3665, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3666, 0);
                ST0090.waitPage(window, 64);
                Runtime.setFlags(7001, 1, 1);
                return;
            }
        }
        window.print(this.msg2D2C9611, 0);
        ST0090.waitPage(window, 64);
        window.print(this.msg2D2C9612, 0);
        ST0090.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        this.after_talk_1 = Runtime.getFlags(7001, 1);
        switch (this.after_talk_1) {
            case 1: {
                window.print(this.msg7469BEA8, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg7469BEA9, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg7469BEAA, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg7469BEAB, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg7469BEAC, 0);
                ST0090.waitPage(window, 64);
                return;
            }
        }
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg2D2C3660, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3661, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3662, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C3663, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C9611, 0);
                ST0090.waitPage(window, 64);
                window.print(this.msg2D2C9612, 0);
                ST0090.waitPage(window, 64);
                Runtime.setFlags(7001, 1, 1);
                return;
            }
        }
        window.print(this.msg2D2C9611, 0);
        ST0090.waitPage(window, 64);
        window.print(this.msg2D2C9612, 0);
        ST0090.waitPage(window, 64);
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(70, 3);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 3.0f, 0.0f, 3.0f, 0.0f);
        this.teiten1.SetBgm(196627);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 3.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.Star = new Mapunits();
        this.Star.mapUnit(30);
        this.Star.start(4, null);
        this.Star.setTranslate(this.Star.px, this.Star.py + 300.0f, this.Star.pz);
        Stage.setVisible(1, false);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        new Uwamono(28678, -2.0f, 0.0f, 0.0f);
        this.doorA = new Uwamono(9, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.npcset_2();
        } else {
            this.npcset_1();
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(526, 1, 0, 5, 3, 0.3f, 0.0f, -2.1f, 0.0f);
        this.npc1.talkto("Talk_npc1");
    }

    void npcset_2() {
        this.npc1 = new NPC_NORMAL(526, 1, 0, 5, 3, 0.3f, 0.0f, -2.1f, 0.0f);
        this.npc1.talkto("Talk_npc1");
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
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

