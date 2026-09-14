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
import xeno.map.MC_VOK21_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0210
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK21_PRJ {
    Player player;
    Camera cam1;
    Camera cam2;
    Camera cam3;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc enemy0;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
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
    int touchFlag3;
    int touchFlag4;
    int touchFlag5;
    int touchFlag6;
    int touchFlag7;
    int touchFlag8;
    Uwamono doorA;
    Effect fade;
    Unit kidou;
    int F10;
    int F11;
    int F12;
    float[] gnoFilter = new float[4];
    int work0;
    int work1;
    int work2;
    Light light = new Light(0);
    int page;
    String[] sys_0 = new String[]{"\n", "      \"Virtual Tutorial\"\n", "", "/[waitkey(64)]/[clear()]"};
    String[] sys_1 = new String[]{"During quests, you'll trigger a battle when you come in contact with an enemy. Here you'll need to talk to KOS-MOS, select from the menu, then approach the soldier to start the battle.", "/[waitkey(64)]/[close()]"};
    String[] allen_0 = new String[]{"/[label(Allen)]", "Chief, which program shall I boot up?", "/[waitkey(64)]/[close()]"};
    String[] allen_1 = new String[]{"/[label(Allen)]", "You want to move on to real combat? All right then, I'll transport you to your original location after the battle is over. Are you ready?", "/[waitkey(64)]/[close()]"};
    String[] allen_2 = new String[]{"/[label(Allen)]", "Oh yeah, Chief, almost forgot to mention about the body flashing phenomenon.", "/[waitkey(64)]/[clear()]"};
    String[] allen_3 = new String[]{"/[label(Allen)]", "After a battle, you'll flash for a few seconds. During this time, you won't trigger a new battle, even if you come in contact with the enemy. Remember this, okay?", "/[waitkey(64)]/[close()]"};
    String[] kosmos_1 = new String[]{"/[label(KOS-MOS)]", "Target input complete.", "/[waitkey(64)]/[close()]"};
    String[] kosmos_2 = new String[]{"/[label(KOS-MOS)]", "Changing targets.", "/[waitkey(64)]/[close()]"};
    String[] kosmos_3 = new String[]{"/[label(KOS-MOS)]", "Shion, please select \"Switch to Real Combat\" when you are ready.", "/[waitkey(64)]/[close()]"};
    String[] kosmos_5 = new String[]{"/[label(KOS-MOS)]", "The battle program has been set. Entering battle mode. Please approach the target.", "/[waitkey(64)]/[close()]"};
    String[] kosmos_6 = new String[]{"/[label(KOS-MOS)]", "Battle program has been set.", "/[waitkey(64)]/[clear()]"};
    String[] event = new String[]{"/[label(KOS-MOS)]", "Moving on to actual combat.", "/[waitkey(64)]/[close()]"};
    String[] jump = new String[]{"/[label(For Debugging)]", "Skip battle and activate scene 1-02.", "/[waitkey(64)]/[close()]"};

    ST0210() {
    }

    void EOB(int n) {
        System.println("*********戦闘に勝利しました**************");
        this.enemy5.kickEnepc(7, 79);
        System.println("*********エネミーをエンカウントなしにしました**************");
        Runtime.charAllRecovery();
        System.println("*********全回復しました**************");
        if (Runtime.getFlags(1019, 1) == 0) {
            Runtime.setFlags(1019, 1, 1);
            Runtime.setPlayerControl(false);
            System.sleep(75);
            System.println("*********アレンの補足メッセージを出しました**************");
            this.win = Window.create();
            this.win.print(this.allen_2, 0);
            ST0210.waitPage(this.win, 64);
            this.win.print(this.allen_3, 0);
            ST0210.waitPage(this.win, 64);
            Runtime.setPlayerControl(true);
        }
    }

    void EOB_Always() {
        System.println("*********戦闘を終了しました**************");
        this.enemy5.kickEnepc(7, 79);
        System.println("*********エネミーをエンカウントなしにしました**************");
        Runtime.charAllRecovery();
        System.println("*********全回復しました**************");
        if (Runtime.getFlags(1019, 1) == 0) {
            Runtime.setFlags(1019, 1, 1);
            Runtime.setPlayerControl(false);
            System.sleep(75);
            System.println("*********アレンの補足メッセージを出しました**************");
            this.win = Window.create();
            this.win.print(this.allen_2, 0);
            ST0210.waitPage(this.win, 64);
            this.win.print(this.allen_3, 0);
            ST0210.waitPage(this.win, 64);
            Runtime.setPlayerControl(true);
        }
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

    void NewBatlleMenu() {
        this.menu = Menu.create();
        this.menu.addItem("Normal Attacks\nTech Attacks\nSub-Menus\nSwitch to Real Combat");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.enemy5.kickEnepc(7, 80);
                this.enemy5.setGroup(0);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.print(this.kosmos_5, 0);
                ST0210.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                return;
            }
            case 1: {
                this.enemy5.kickEnepc(7, 80);
                this.enemy5.setGroup(1);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.print(this.kosmos_5, 0);
                ST0210.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                return;
            }
            case 2: {
                this.enemy5.kickEnepc(7, 80);
                this.enemy5.setGroup(2);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.print(this.kosmos_5, 0);
                ST0210.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                return;
            }
            case 3: {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.print(this.kosmos_6, 0);
                ST0210.waitPage(this.win, 64);
                this.win.print(this.allen_1, 0);
                ST0210.waitPage(this.win, 64);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                if (this.selected == 0) {
                    System.println("*********バトルイベントを起動します**************");
                    System.println("*********アレンの補足メッセージフラグをたてました**************");
                    Runtime.setFlags(1019, 1, 1);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.jumpEvent(1900);
                } else {
                    System.println("*********とりあえず何もしません**************");
                    Runtime.setPlayerControl(true);
                }
                Runtime.setPlayerControl(true);
                return;
            }
        }
        this.enemy5.kickEnepc(7, 79);
        System.println("*********エネミーをエンカウントなしにしました**************");
        this.win = Window.create();
        this.win.print(this.kosmos_3, 0);
        ST0210.waitPage(this.win, 64);
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.F11 == 1) {
            System.println("*********一回、実戦編を終了しています**************");
            return;
        }
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.kosmos_1, 0);
                ST0210.waitPage(window, 64);
                this.NewBatlleMenu();
                return;
            }
        }
        window.print(this.kosmos_2, 0);
        ST0210.waitPage(window, 64);
        this.NewBatlleMenu();
    }

    public void Touch_npc1(Enepc enepc, Window window) {
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
    }

    void init() {
        this.light.setColor(0, 0.5f, 0.5f, 0.5f);
        this.light.setColor(1, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 0.834f, 0.551f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -0.872f, -0.49f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Stage.setVisible(-1, true);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 12345, 1);
        Runtime.setDefocusQuick(1, 1, 23456, 1);
        this.cam0.setFog(1, 12.0f, 18.0f, 0.0f, 0.5f, 255, 255, 255, 255);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 25.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.npcset_1();
        this.F10 = Runtime.getFlags(1010, 1);
        this.F11 = Runtime.getFlags(1011, 1);
        this.F12 = Runtime.getFlags(1012, 1);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (this.F12 == 0) {
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 2);
            Runtime.setPartyData(65542, 2);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setFriend(1);
            Runtime.setFriend(2);
            Runtime.resetFriend(4);
            Runtime.resetFriend(6);
            Runtime.resetFriend(5);
            Runtime.resetFriend(3);
            Runtime.setFlags(1012, 1, 1);
            System.println("パーティ初期セット");
        }
        this.kidou = new Mapunits();
        this.kidou.mapUnit(0);
        if (this.F10 == 0) {
            this.kidou.start(4, null);
            this.kidou.start(1, "Msg");
        } else {
            this.kidou.start(4, null);
            Runtime.setPlayerControl(true);
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(14, 1, 0, 14, 3, 1.77f, 0.0f, 1.629f, -90.0f);
        this.npc1.talkto("Talk_npc1");
        this.npc1.disableDTKFlag(8);
        this.enemy5 = new Enepc();
        this.enemy5.init(18177, 4, 0.98f, 0.0f, -2.4f, 0.0f);
        this.enemy5.id = 15;
        this.enemy5.setGroup(0, 0, 0, 0);
        this.enemy5.setTogetherWith(-2, -1, -1, -1);
        this.enemy5.setParams(0, 79, 15, 5);
        this.enemy5.setBatEvent(12);
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

        void Msg() {
            Runtime.setPlayerControl(false);
            ST0210.this.win = Window.create();
            ST0210.this.win.print(ST0210.this.sys_0, 0);
            ST0210.waitPage(ST0210.this.win, 64);
            ST0210.this.win.print(ST0210.this.sys_1, 0);
            ST0210.waitPage(ST0210.this.win, 64);
            Runtime.setFlags(1010, 1, 1);
            Runtime.setPlayerControl(true);
        }
    }

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }

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

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
        }
    }
}

