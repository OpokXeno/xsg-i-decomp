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
import xeno.map.MC_KUK07B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2210
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK07B_PRJ {
    int LADDER = Runtime.getFlags(6044, 1);
    int STAIRS = Runtime.getFlags(6045, 1);
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int CAT_LOOK = 0;
    Uwamono doorA;
    Enepc HASIGO;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Uwamono douzou;
    Uwamono ura;
    Unit ladder;
    Unit stairs;
    boolean EnterCheck = false;
    Uwamono Atari;
    Uwamono Atari2;
    Uwamono Atari3;
    Uwamono Atari4;
    Uwamono Atari5;
    Light light = new Light(0);
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Effect fire01;
    Effect fire02;
    Effect fire03;
    Effect fire04;
    Effect fire05;
    Effect fire06;
    Effect fire07;
    Effect fire08;
    Effect fire09;
    Effect fire10;
    MAPUnit s_unit;
    Uwamono co01;
    Uwamono co02;
    Effect button1A;
    Effect button1B;
    Effect button2A;
    Effect button2B;
    Uwamono Kuruma_Atari;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Uwamono teiten10;
    Uwamono teiten12;
    Uwamono teiten56;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Uwamono itembox;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    int page;
    String[] NPC_TALK1 = new String[]{"Jump?\n", "/[waitkey(64)]/[close()]"};
    String[] Q1 = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};
    String[] msghelp5 = new String[]{"/[label()]", "Ho ho ho. You saved me.", "/[waitkey(1)]/[clear()]", "I was about to get turned into a bronze statue too.", "/[waitkey(1)]/[clear()]", "Thanks a bunch!", "/[waitkey(64)]/[close()]"};
    String[] msghelp6 = new String[]{"/[label()]", "Meow!", "/[waitkey(64)]/[close()]"};
    String[] msghelp7 = new String[]{"/[label()]", "I was so scared! I was hiding here when the monsters came, and then I couldn't get out!", "/[waitkey(1)]/[clear()]", "I'll never run away from home again! I promise I'll listen to what Mom says from now on! ", "Waaaah!", "/[waitkey(64)]/[close()]"};
    String[] msgCAT_LOOK = new String[]{"/[label()]", "Hiss!\n", "Hiss!\n", "Hissssss!!", "/[waitkey(64)]/[close()]"};
    String[] msg0help2 = new String[]{"/[label()]", "13 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg1help2 = new String[]{"/[label()]", "12 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg2help2 = new String[]{"/[label()]", "11 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg3help2 = new String[]{"/[label()]", "10 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg4help2 = new String[]{"/[label()]", "9 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg5help2 = new String[]{"/[label()]", "8 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg6help2 = new String[]{"/[label()]", "7 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg7help2 = new String[]{"/[label()]", "6 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg8help2 = new String[]{"/[label()]", "5 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg9help2 = new String[]{"/[label()]", "4 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg10help2 = new String[]{"/[label()]", "3 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg11help2 = new String[]{"/[label()]", "2 more people left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg12help2 = new String[]{"/[label()]", "1 more person left to go!", "/[waitkey(64)]/[close()]"};
    String[] msg13helpok = new String[]{"/[label()]", "Everyone has been rescued!", "/[waitkey(64)]/[close()]"};

    ST2210() {
    }

    void EOB(int n) {
        if (n == 11) {
            Runtime.setFlags(8114, 1, 1);
        }
        if (n == 12) {
            Runtime.setFlags(8115, 1, 1);
        }
        if (n == 15) {
            Runtime.setFlags(8104, 1, 1);
        }
        if (n == 16) {
            Runtime.setFlags(8105, 1, 1);
        }
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(17.653f, 3.147f, 14.452f);
        this.camEV.setRotate(-25.5f, 38.679f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.818f, 13.439f, -9.539f);
        this.camEV.setRotate(-22.491f, -65.959f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.276f, 13.599f, -10.779f);
        this.camEV.setRotate(-19.882f, -30.7f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.093f, 12.706f, -11.535f);
        this.camEV.setRotate(-11.951f, -35.769f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera05() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.005f, 11.835f, 20.439f);
        this.camEV.setRotate(-31.866f, -33.955f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Ene_Restart() {
        if (Runtime.getFlags(8114, 1) == 0) {
            this.enemy1.kickEnepc(4, 0);
        }
        if (Runtime.getFlags(8115, 1) == 0) {
            this.enemy2.kickEnepc(4, 0);
        }
        this.enemy3.kickEnepc(4, 0);
        this.enemy4.kickEnepc(4, 0);
        if (Runtime.getFlags(8104, 1) == 0) {
            this.enemy5.kickEnepc(4, 0);
        }
        if (Runtime.getFlags(8105, 1) == 0) {
            this.enemy6.kickEnepc(4, 0);
        }
    }

    void Ene_Stop() {
        if (Runtime.getFlags(8114, 1) == 0) {
            this.enemy1.kickEnepc(4, 2);
        }
        if (Runtime.getFlags(8115, 1) == 0) {
            this.enemy2.kickEnepc(4, 2);
        }
        this.enemy3.kickEnepc(4, 2);
        this.enemy4.kickEnepc(4, 2);
        if (Runtime.getFlags(8104, 1) == 0) {
            this.enemy5.kickEnepc(4, 2);
        }
        if (Runtime.getFlags(8105, 1) == 0) {
            this.enemy6.kickEnepc(4, 2);
        }
    }

    void Ene_Vfalse() {
        if (Runtime.getFlags(8114, 1) == 0) {
            this.enemy1.setVisible(false);
        }
        if (Runtime.getFlags(8115, 1) == 0) {
            this.enemy2.setVisible(false);
        }
        this.enemy3.setVisible(false);
        this.enemy4.setVisible(false);
        if (Runtime.getFlags(8104, 1) == 0) {
            this.enemy5.setVisible(false);
        }
        if (Runtime.getFlags(8105, 1) == 0) {
            this.enemy6.setVisible(false);
        }
    }

    void Ene_Vtrue() {
        if (Runtime.getFlags(8114, 1) == 0) {
            this.enemy1.setVisible(true);
        }
        if (Runtime.getFlags(8115, 1) == 0) {
            this.enemy2.setVisible(true);
        }
        this.enemy3.setVisible(true);
        this.enemy4.setVisible(true);
        if (Runtime.getFlags(8104, 1) == 0) {
            this.enemy5.setVisible(true);
        }
        if (Runtime.getFlags(8105, 1) == 0) {
            this.enemy6.setVisible(true);
        }
    }

    void Final_init(int n) {
    }

    public void HashigoBottom(int n) {
        switch (n) {
            case 2: {
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                System.println("クリーニング屋・５");
                Runtime.jumpCF(67766, 5);
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
        if (n2 == 0 && !this.EnterCheck) {
            this.player.getTranslate();
            if (this.player.py < 3.5f || this.player.py > 7.0f) {
                return;
            }
            System.println("ハシゴの上げ下げ");
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Q1, 0);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Press\nDon't press\n");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            if (this.selected == 0) {
                Sound.effectPlay(58);
                if (this.LADDER == 0 && this.STAIRS == 0) {
                    System.println("A");
                    this.button2A.disp(false);
                    this.button2B.disp(true);
                    Sound.effectPlay(196743);
                    float f = 3.2f;
                    while (f >= 0.0f) {
                        this.ladder.setTranslate(8.25f, f, 12.15f);
                        System.sleep(1);
                        f -= 0.1f;
                    }
                    this.player.setID(2);
                    Runtime.setFlags(6044, 1, 1);
                    this.LADDER = Runtime.getFlags(6044, 1);
                } else if (this.LADDER == 1 && this.STAIRS == 0) {
                    System.println("B");
                    this.button2A.disp(true);
                    this.button2B.disp(false);
                    Sound.effectPlay(196743);
                    float f = 0.0f;
                    while (f <= 3.2f) {
                        this.ladder.setTranslate(8.25f, f, 12.15f);
                        System.sleep(1);
                        f += 0.1f;
                    }
                    this.player.setID(1);
                    Runtime.setFlags(6044, 1, 0);
                    this.LADDER = Runtime.getFlags(6044, 1);
                } else if (this.LADDER == 0 && this.STAIRS == 1) {
                    System.println("C");
                    this.button2A.disp(false);
                    this.button2B.disp(true);
                    Sound.effectPlay(196743);
                    float f = 3.2f;
                    while (f >= 0.0f) {
                        this.ladder.setTranslate(8.25f, f, 12.15f);
                        System.sleep(1);
                        f -= 0.1f;
                    }
                    this.player.setID(4);
                    Runtime.setFlags(6044, 1, 1);
                    this.LADDER = Runtime.getFlags(6044, 1);
                } else if (this.LADDER == 1 && this.STAIRS == 1) {
                    System.println("D");
                    this.button2A.disp(true);
                    this.button2B.disp(false);
                    Sound.effectPlay(196743);
                    float f = 0.0f;
                    while (f <= 3.2f) {
                        this.ladder.setTranslate(8.25f, f, 12.15f);
                        System.sleep(1);
                        f += 0.1f;
                    }
                    this.player.setID(3);
                    Runtime.setFlags(6044, 1, 0);
                    this.LADDER = Runtime.getFlags(6044, 1);
                }
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
        } else if (n2 == 1 && !this.EnterCheck) {
            this.player.getTranslate();
            if (this.player.py < 3.5f || this.player.py > 7.0f) {
                return;
            }
            System.println("回転床の上げ下げ");
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Q1, 0);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Press\nDon't press\n");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            if (this.selected == 0) {
                System.sleep(30);
                Sound.effectPlay(58);
                this.cam0.setMode(-1);
                this.EV_Camera05();
                System.sleep(30);
                Sound.effectPlay(196744);
                float f = 0.020799987f;
                float f2 = 0.0035000006f;
                if (this.LADDER == 0 && this.STAIRS == 0) {
                    System.println("E");
                    this.button1A.disp(false);
                    this.button1B.disp(true);
                    float f3 = 0.0f;
                    while (f3 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -f3);
                        this.stairs.setTranslate(5.049f - f * f3, 5.795f + f2 * f3, 12.0f);
                        System.sleep(1);
                        f3 += 1.0f;
                    }
                    this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
                    this.player.setID(3);
                    Runtime.setFlags(6045, 1, 1);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                } else if (this.LADDER == 1 && this.STAIRS == 0) {
                    System.println("F");
                    this.button1A.disp(false);
                    this.button1B.disp(true);
                    float f4 = 0.0f;
                    while (f4 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -f4);
                        this.stairs.setTranslate(5.049f - f * f4, 5.795f + f2 * f4, 12.0f);
                        System.sleep(1);
                        f4 += 1.0f;
                    }
                    this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
                    this.player.setID(4);
                    Runtime.setFlags(6045, 1, 1);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                } else if (this.LADDER == 0 && this.STAIRS == 1) {
                    System.println("G");
                    this.button1A.disp(true);
                    this.button1B.disp(false);
                    float f5 = 0.0f;
                    while (f5 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -30.0f + f5);
                        this.stairs.setTranslate(4.425f + f * f5, 5.9f - f2 * f5, 12.0f);
                        System.sleep(1);
                        f5 += 1.0f;
                    }
                    this.stairs.setTranslate(5.049f, 5.795f, 12.0f);
                    this.player.setID(1);
                    Runtime.setFlags(6045, 1, 0);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                } else if (this.LADDER == 1 && this.STAIRS == 1) {
                    System.println("H");
                    this.button1A.disp(true);
                    this.button1B.disp(false);
                    float f6 = 0.0f;
                    while (f6 <= 30.0f) {
                        this.stairs.setRotate(0.0f, 0.0f, -30.0f + f6);
                        this.stairs.setTranslate(4.425f + f * f6, 5.9f - f2 * f6, 12.0f);
                        System.sleep(1);
                        f6 += 1.0f;
                    }
                    this.stairs.setTranslate(5.049f, 5.795f, 12.0f);
                    this.player.setID(2);
                    Runtime.setFlags(6045, 1, 0);
                    this.STAIRS = Runtime.getFlags(6045, 1);
                }
            }
            this.EnterCheck = false;
            System.sleep(30);
            this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
        } else if (n2 == 2) {
            System.println("serial = 2");
            this.player.getTranslate();
            if (this.player.py > 9.0f) {
                System.println("自然落下");
                Runtime.setPlayerControl(false);
                this.player.setTranslate(this.player.px - 0.35f, this.player.py, this.player.pz);
                System.sleep(10);
                Runtime.setPlayerControl(true);
            }
        } else if (n2 == 3) {
            this.player.getTranslate();
            if (this.player.py < 10.0f) {
                return;
            }
            if (this.CAT_LOOK == 1) {
                return;
            }
            if (Runtime.getFlags(7114, 1) == 1) {
                return;
            }
            this.Ene_Stop();
            Runtime.setPlayerControl(false);
            this.cam0.setMode(-1);
            this.EV_Camera04();
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgCAT_LOOK, 0);
            ST2210.waitPage(this.win, 64);
            System.sleep(15);
            this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            ++this.CAT_LOOK;
            this.Ene_Restart();
        }
    }

    public void Talk_npc1(Enepc enepc) {
        this.Ene_Stop();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(14.42f, 0.0f, 11.14f);
        this.player.rotY(1, 90.0f, true);
        this.npc1.moveEnepc(17, 0.0f, 0.1f, 2);
        System.sleep(2);
        Runtime.disable(65536);
        this.Ene_Vfalse();
        this.cam0.setMode(-1);
        this.EV_Camera01();
        this.player.look_char(this.npc1);
        this.npc1.look_char(this.player);
        System.sleep(10);
        this.npc1.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp5, 0);
        ST2210.waitPage(this.win, 64);
        this.npc1.look_default();
        this.npc1.disableDTKFlag(2);
        this.npc1.kickEnepc(9, -1);
        this.npc1.kickEnepc(1, 3);
        this.npc1.moveEnepc(17, 0.0f, 0.1f, 10);
        System.sleep(8);
        this.npc1.moveEnepc(15, 14.7f, 16.08f, 40);
        System.sleep(50);
        this.fade.call(0);
        System.sleep(30);
        this.Ene_Vtrue();
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc1.disableDTKFlag(131072);
        this.npc1.disableDTKFlag(65536);
        this.npc1.setVisible(false);
        ++this.count;
        Runtime.setFlags(7113, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
        this.Ene_Restart();
    }

    public void Talk_npc2(Enepc enepc) {
        this.Ene_Stop();
        this.Ene_Vfalse();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(-3.23f, 11.3f, -14.32f);
        this.player.rotY(1, 90.0f, true);
        this.npc2.moveEnepc(17, 270.0f, 0.1f, 7);
        System.sleep(2);
        this.cam0.setMode(-1);
        this.EV_Camera03();
        this.player.look_char(this.npc2);
        this.npc2.look_char(this.player);
        this.npc2.kickEnepc(1, 2);
        System.sleep(60);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp6, 0);
        ST2210.waitPage(this.win, 64);
        this.npc2.look_default();
        this.npc2.disableDTKFlag(2);
        this.npc2.kickEnepc(9, -1);
        this.npc2.kickEnepc(1, 1);
        this.npc2.moveEnepc(17, 90.0f, 0.1f, 20);
        System.sleep(16);
        this.npc2.moveEnepc(15, 0.56f, -14.62f, 100);
        System.sleep(120);
        this.fade.call(0);
        System.sleep(30);
        this.cam0.setMode(0);
        this.player.look_default();
        this.npc2.disableDTKFlag(131072);
        this.npc2.disableDTKFlag(65536);
        this.npc2.setVisible(false);
        ++this.count;
        Runtime.setFlags(7114, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
        this.Ene_Vtrue();
        this.Ene_Restart();
    }

    public void Talk_npc20(Enepc enepc, Window window) {
    }

    public void Talk_npc3(Enepc enepc) {
        this.Ene_Stop();
        System.sleep(1);
        this.fade.call(0);
        System.sleep(28);
        Runtime.enable(65536);
        this.player.setTranslate(10.68f, 10.8f, -11.54f);
        this.player.rotY(1, 60.0f, true);
        System.sleep(2);
        Runtime.disable(65536);
        this.cam0.setMode(-1);
        this.EV_Camera02();
        this.Ene_Vfalse();
        this.player.look_char(this.npc3);
        System.sleep(10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghelp7, 0);
        ST2210.waitPage(this.win, 64);
        this.npc3.disableDTKFlag(2);
        this.npc3.kickEnepc(9, -1);
        this.npc3.kickEnepc(1, 3);
        this.npc3.moveEnepc(15, 8.35f, -10.99f, 40);
        System.sleep(50);
        this.fade.call(0);
        System.sleep(30);
        this.cam0.setMode(0);
        this.Ene_Vtrue();
        this.player.look_default();
        this.npc3.disableDTKFlag(131072);
        this.npc3.disableDTKFlag(65536);
        this.npc3.setVisible(false);
        ++this.count;
        Runtime.setFlags(7115, 1, 1);
        Runtime.setFlags(7092, 4, this.count);
        this.countdown();
        this.Ene_Restart();
    }

    public void broken(int n) {
        switch (n) {
            case 1: {
                System.println("case1");
                this.npc1.setInvalidID(0);
                Runtime.setFlags(7164, 1, 1);
                break;
            }
            case 2: {
                System.println("case1");
                this.npc3.enableDTKFlag(131072);
                this.npc3.enableDTKFlag(65536);
                break;
            }
        }
    }

    void countdown() {
        if (Runtime.getFlags(7092, 4) == 14) {
            return;
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        switch (Runtime.getFlags(7092, 4)) {
            case 0: {
                this.win.print(this.msg0help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 1: {
                this.win.print(this.msg1help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 2: {
                this.win.print(this.msg2help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 3: {
                this.win.print(this.msg3help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 4: {
                this.win.print(this.msg4help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 5: {
                this.win.print(this.msg5help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 6: {
                this.win.print(this.msg6help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 7: {
                this.win.print(this.msg7help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 8: {
                this.win.print(this.msg8help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 9: {
                this.win.print(this.msg9help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 10: {
                this.win.print(this.msg10help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 11: {
                this.win.print(this.msg11help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 12: {
                this.win.print(this.msg12help2, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
            case 13: {
                this.win.print(this.msg13helpok, 0);
                ST2210.waitPage(this.win, 64);
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("襲撃後・外観街１");
                Runtime.jumpCF(67706, 1);
                break;
            }
            case 1: {
                System.println("襲撃後・外観街７");
                Runtime.jumpCF(67706, 7);
                break;
            }
            case 2: {
                System.println("襲撃後倉庫・１");
                Runtime.jumpCF(67776, 1);
                break;
            }
            case 3: {
                System.println("襲撃後倉庫・２");
                Runtime.jumpCF(67776, 2);
                break;
            }
            case 4: {
                System.println("クリーニング屋１");
                Runtime.jumpCF(67766, 1);
                break;
            }
            case 5: {
                System.println("襲撃後・パン屋４");
                Runtime.jumpCF(67756, 4);
                break;
            }
            case 6: {
                System.println("襲撃後・パン屋５");
                Runtime.jumpCF(67756, 5);
                break;
            }
            case 7: {
                System.println("襲撃後・パン屋１");
                Runtime.jumpCF(67756, 1);
                break;
            }
            case 8: {
                System.println("襲撃後・パン屋３");
                Runtime.jumpCF(67756, 3);
                break;
            }
            case 9: {
                System.println("襲撃後・パン屋２");
                Runtime.jumpCF(67756, 2);
                break;
            }
            case 10: {
                System.println("クリーニング屋２");
                Runtime.jumpCF(67766, 2);
                break;
            }
            case 11: {
                System.println("襲撃後倉庫・４");
                Runtime.jumpCF(67776, 4);
                break;
            }
            case 12: {
                System.println("クリーニング屋３");
                Runtime.jumpCF(67766, 3);
                break;
            }
            case 13: {
                System.println("襲撃後倉庫・３");
                Runtime.jumpCF(67776, 3);
                break;
            }
            case 14: {
                System.println("ミニマップ");
                Runtime.jumpCF(2130, 2);
                break;
            }
        }
    }

    void init() {
        float[] fArray;
        this.count = Runtime.getFlags(7092, 4);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.275f, 0.275f, 0.275f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.275f, 0.275f, 0.275f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.275f, 0.275f, 0.275f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.345f, 0.345f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.345f, 0.345f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.345f, 0.345f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.345f, 0.345f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 2, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 3, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.32f, 0.32f, 0.25f);
        Runtime.setIdLightCol(4, 1, 0.32f, 0.32f, 0.25f);
        Runtime.setIdLightCol(4, 2, 0.32f, 0.32f, 0.25f);
        Runtime.setIdLightCol(4, 3, 0.32f, 0.32f, 0.25f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(5, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(5, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(5, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(6, 0, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(6, 1, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(6, 2, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightCol(6, 3, 0.325f, 0.325f, 0.325f);
        Runtime.setIdLightVec(6, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(6, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(6, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(7, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(7, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(7, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(7, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(7, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(7, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(7, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(8, 0, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightCol(8, 1, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightCol(8, 2, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightCol(8, 3, 0.315f, 0.315f, 0.315f);
        Runtime.setIdLightVec(8, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(8, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(8, 3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFAngle(4, -28.0f, -12.5f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.015f, 0.015f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.02f, 0.02f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.02f, 0.02f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.015f, 0.015f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.02f, 0.02f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(9, 0.02f, 0.02f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.02f, 0.02f);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.02f, 0.02f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 12.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.02f, 0.02f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(13, 0.015f, 0.015f);
        this.cam0.setCFAngle(14, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(14, 0.02f, 0.02f);
        this.cam0.setCFAngle(15, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(15, 0.02f, 0.02f);
        this.cam0.setCFPedestal(16, 16.485668f, 13.60734f, 25.085058f, 29.759653f, -29.232653f, 34.92518f, 0.0f, 2.0f);
        this.cam0.setCFHokan(16, 100.0f, 100.0f);
        this.cam0.setCFPedestal(17, -8.388614f, 19.13557f, 6.524935f, 35.520004f, -42.413883f, 36.31963f, 0.0f, 2.0f);
        this.cam0.setCFHokan(17, 100.0f, 100.0f);
        this.cam0.setCFAngle(18, -28.0f, 0.0f, 0.0f, 11.0f, 40.0f);
        this.cam0.setCFHokan(18, 0.015f, 0.015f);
        this.cam0.setCFAngle(19, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(19, 100.0f, 100.0f);
        this.cam0.setCFAngle(20, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(20, 100.0f, 100.0f);
        this.cam0.setCFAngle(21, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(21, 100.0f, 100.0f);
        this.cam0.setCFPedestal(22, 5.3421884f, 16.479866f, -3.0894277f, 43.19987f, -40.830788f, -32.078743f, 0.0f, 2.0f);
        this.cam0.setCFHokan(22, 100.0f, 100.0f);
        this.cam0.setCFAngle(23, -28.0f, -30.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(23, 0.015f, 0.015f);
        this.cam0.setCFAngle(24, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(24, 0.02f, 0.02f);
        this.cam0.setCFLockX(24, -18.5f);
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
        this.Atari = new Uwamono(28672, 20.7f, 1.54f, 12.5f, 0.0f);
        this.Atari.SetSize(2.0f, 3.0f, 2.0f);
        this.Atari2 = new Uwamono(28672, -10.53f, 4.76f, 2.7f, 0.0f);
        this.Atari2.SetSize(2.0f, 3.0f, 2.0f);
        this.Atari3 = new Uwamono(28672, -7.14f, 7.2f, 6.13f, 0.0f);
        this.Atari3.SetSize(2.0f, 3.0f, 2.0f);
        this.Atari4 = new Uwamono(28672, 10.06f, 7.2f, -1.54f, 0.0f);
        this.Atari4.SetSize(2.0f, 3.0f, 2.0f);
        this.Atari5 = new Uwamono(28672, 20.35f, 2.63f, 10.96f, 0.0f);
        this.Atari5.SetSize(2.0f, 3.0f, 2.0f);
        this.button1A = new Effect(1539, -6.5f, 4.86f, 12.18f, 0.0f);
        this.button1A.setScale(0.425f, 0.25f, 0.275f);
        this.button1A.setRotate(-35.0f, 0.0f, 0.0f);
        this.button1B = new Effect(1542, -6.5f, 4.86f, 12.18f, 0.0f);
        this.button1B.setScale(0.425f, 0.25f, 0.275f);
        this.button1B.setRotate(-35.0f, 0.0f, 0.0f);
        this.button2A = new Effect(1539, 5.996f, 4.84f, 12.18f, 0.0f);
        this.button2A.setScale(0.425f, 0.25f, 0.275f);
        this.button2A.setRotate(-50.0f, 0.0f, 0.0f);
        this.button2B = new Effect(1542, 5.996f, 4.84f, 12.18f, 0.0f);
        this.button2B.setScale(0.425f, 0.25f, 0.275f);
        this.button2B.setRotate(-55.0f, 0.0f, 0.0f);
        this.Kuruma_Atari = new Uwamono(28672, -20.75f, 0.0f, 16.5f, 0.0f);
        this.Kuruma_Atari.SetSize(3.0f, 1.0f, 5.0f);
        this.Kuruma_Atari.setRotate(0.0f, 35.0f, 0.0f);
        this.ladder = new Unit();
        this.ladder.mapUnit(169);
        this.ladder.start(4, null);
        this.stairs = new Unit();
        this.stairs.mapUnit(181);
        this.stairs.start(4, null);
        if (this.STAIRS == 0 && this.LADDER == 0) {
            System.println("STAIRS == 0 && LADDER == 0");
            this.button1A.disp(true);
            this.button1B.disp(false);
            this.button2A.disp(true);
            this.button2B.disp(false);
            this.ladder.setTranslate(8.25f, 3.2f, 12.15f);
            this.player.setID(1);
        } else if (this.STAIRS == 0 && this.LADDER == 1) {
            System.println("STAIRS == 0 && LADDER == 1");
            this.button1A.disp(true);
            this.button1B.disp(false);
            this.button2A.disp(false);
            this.button2B.disp(true);
            this.ladder.setTranslate(8.25f, 0.0f, 12.15f);
            this.player.setID(2);
        } else if (this.STAIRS == 1 && this.LADDER == 0) {
            System.println("STAIRS == 1 && LADDER == 0");
            this.button1A.disp(false);
            this.button1B.disp(true);
            this.button2A.disp(true);
            this.button2B.disp(false);
            this.ladder.setTranslate(8.25f, 3.2f, 12.15f);
            this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
            this.stairs.setRotate(0.0f, 0.0f, -30.0f);
            this.player.setID(3);
        } else if (this.STAIRS == 1 && this.LADDER == 1) {
            System.println("STAIRS == 1 && LADDER == 1");
            this.button1A.disp(false);
            this.button1B.disp(true);
            this.button2A.disp(false);
            this.button2B.disp(true);
            this.ladder.setTranslate(8.25f, 0.0f, 12.15f);
            this.stairs.setTranslate(4.425f, 5.9f, 12.0f);
            this.stairs.setRotate(0.0f, 0.0f, -30.0f);
            this.player.setID(4);
        }
        this.itembox = new Uwamono(28677, -2.71f, 10.8f, 7.66f, 180.0f, 422);
        this.itembox.SetSymbol(28686);
        this.itembox.SetCallNo(1);
        if (Runtime.getFlags(7113, 1) == 0) {
            if (Runtime.getFlags(7164, 1) == 0) {
                this.npc1 = new NPC_NORMAL(1552, 1, 0, 14, 3, 14.99f, 2.9f, 11.0f, 200.0f);
                this.npc1.setInvalidID(1);
                this.npc1.disableDTKFlag(3);
                this.npc1.enableDTKFlag(262144);
                this.npc1.talkto("Talk_npc1");
                this.npc1.setMotion(0, 27);
            } else {
                this.npc1 = new NPC_NORMAL(1552, 1, 0, 14, 3, 14.99f, 0.0f, 11.0f, 200.0f);
                this.npc1.setInvalidID(1);
                this.npc1.disableDTKFlag(3);
                this.npc1.enableDTKFlag(262144);
                this.npc1.talkto("Talk_npc1");
                this.npc1.setMotion(0, 27);
            }
        }
        if (Runtime.getFlags(7114, 1) == 0) {
            this.npc2 = new NPC_NORMAL(1611, 2, 0, 14, 7, -2.79f, 11.29f, -14.66f, 90.0f);
            this.npc2.setInvalidID(1);
            this.npc2.disableDTKFlag(3);
            this.npc2.enableDTKFlag(262144);
            this.npc2.talkto("Talk_npc2");
            this.npc2.setMotion(0, 27);
        }
        if (Runtime.getFlags(7115, 1) == 0) {
            this.npc3 = new NPC_NORMAL(1602, 3, 0, 14, 6, 12.18f, 10.9f, -11.1f, 270.0f);
            this.npc3.setInvalidID(1);
            this.npc3.disableDTKFlag(3);
            this.npc3.enableDTKFlag(262144);
            this.npc3.setMotion(0, 28);
            this.npc3.disableDTKFlag(131072);
            this.npc3.disableDTKFlag(65536);
            this.npc3.talkto("Talk_npc3");
            this.npc3.setInvalidID(1);
        }
        this.fire01 = new Effect(1402, 20.9f, 1.2f, 12.1f, 0.0f);
        this.fire02 = new Effect(1402, 20.3f, 2.8f, 10.5f, 0.0f);
        this.fire03 = new Effect(1402, -7.1f, 7.5f, 6.1f, 0.0f);
        this.fire04 = new Effect(1402, -10.4f, 5.0f, 2.5f, 0.0f);
        this.fire04.setScale(1.3f, 1.3f, 1.3f);
        this.fire05 = new Effect(1402, 9.9f, 7.5f, -1.0f, 0.0f);
        this.fire06 = new Effect(1402, 9.0f, 7.5f, -3.0f, 0.0f);
        this.fire08 = new Effect(1402, -2.0f, 0.0f, 15.1f, 0.0f);
        this.fire09 = new Effect(1402, 3.2f, 8.5f, -5.0f, 0.0f);
        this.fire10 = new Effect(1402, -19.7f, 0.7f, 17.3f, 0.0f);
        Runtime.progressEffect(30);
        this.co01 = new Uwamono(28672, -2.0f, 0.0f, 15.1f, 0.0f);
        this.co01.SetHitKind('\u0001');
        this.co01.SetSize(1.5f, 1.0f, 1.5f);
        this.co02 = new Uwamono(28672, 9.0f, 7.5f, -3.0f, 0.0f);
        this.co02.SetHitKind('\u0001');
        this.co02.SetSize(1.5f, 1.0f, 1.5f);
        float[] fArray2 = new float[12];
        fArray2[0] = 14.8f;
        fArray2[2] = 13.5f;
        fArray2[3] = -1.0f;
        fArray2[4] = 14.3f;
        fArray2[6] = 13.5f;
        fArray2[8] = 15.3f;
        fArray2[10] = 13.5f;
        float[] fArray3 = fArray2;
        this.enemy1 = new NpcEnemy(16402, 11, 1, 138, 13, 14.8f, 0.0f, 13.5f, 180.0f, fArray3);
        this.enemy1.setGroup(3, 3, 4, 4);
        this.enemy1.setMotion(0, 27);
        if (Runtime.getFlags(8114, 1) == 1) {
            this.enemy1.kickEnepc(4, 2);
            this.enemy1.setVisible(false);
        }
        float[] fArray4 = new float[12];
        fArray4[0] = 2.6f;
        fArray4[2] = 17.3f;
        fArray4[3] = -1.0f;
        fArray4[4] = 1.1f;
        fArray4[6] = 17.3f;
        fArray4[8] = 2.1f;
        fArray4[10] = 17.3f;
        float[] fArray5 = fArray4;
        this.enemy2 = new NpcEnemy(16400, 12, 2, 126, 20, 2.6f, 0.0f, 17.3f, 0.0f, fArray5);
        this.enemy2.setGroup(5, 5, 6, 6);
        this.enemy2.kickEnepc(10, 50, 0);
        float[] fArray6 = new float[9];
        fArray6[0] = 2.6f;
        fArray6[2] = 17.3f;
        fArray6[3] = 1.0f;
        fArray6[5] = 17.3f;
        fArray6[8] = 17.3f;
        float[] fArray7 = fArray6;
        this.enemy2.setParams(fArray7);
        if (Runtime.getFlags(8115, 1) == 1) {
            this.enemy2.kickEnepc(4, 2);
            this.enemy2.setVisible(false);
        }
        float[] fArray8 = new float[12];
        fArray8[0] = -11.0f;
        fArray8[2] = 13.75f;
        fArray8[3] = -1.0f;
        fArray8[4] = -14.7f;
        fArray8[6] = 13.75f;
        fArray8[8] = -15.7f;
        fArray8[10] = 13.75f;
        float[] fArray9 = fArray8;
        this.enemy3 = new NpcEnemy(16401, 13, 3, 106, 16, -11.0f, 0.0f, 13.75f, 0.0f, fArray9);
        this.enemy3.setGroup(0, 0, 0, 0);
        float[] fArray10 = new float[9];
        fArray10[0] = -11.0f;
        fArray10[2] = 13.75f;
        fArray10[3] = -15.2f;
        fArray10[5] = 13.75f;
        fArray10[6] = -18.0f;
        fArray10[8] = 13.75f;
        float[] fArray11 = fArray10;
        this.enemy3.setParams(fArray11);
        float[] fArray12 = new float[12];
        fArray12[0] = 2.4f;
        fArray12[1] = 7.21f;
        fArray12[2] = -7.8f;
        fArray12[3] = -1.0f;
        fArray12[4] = 2.9f;
        fArray12[5] = 7.21f;
        fArray12[6] = -7.8f;
        fArray12[8] = 1.9f;
        fArray12[9] = 7.21f;
        fArray12[10] = -7.8f;
        float[] fArray13 = fArray12;
        this.enemy4 = new NpcEnemy(16395, 14, 4, 131, 22, 3.7f, 7.21f, -7.8f, 0.0f, fArray13);
        this.enemy4.setGroup(7, 7, 7, 7);
        this.enemy4.kickEnepc(10, 50, 0);
        float[] fArray14 = new float[]{3.7f, 7.21f, -7.8f, -3.8f, 7.21f, -7.8f};
        this.enemy4.setParams(fArray14);
        if (Runtime.getFlags(8104, 1) == 0) {
            float[] fArray15 = new float[12];
            fArray15[0] = 0.6f;
            fArray15[1] = 11.3f;
            fArray15[2] = -14.5f;
            fArray15[3] = -1.0f;
            fArray15[4] = 0.1f;
            fArray15[5] = 11.3f;
            fArray15[6] = -14.5f;
            fArray15[8] = 1.1f;
            fArray15[9] = 11.3f;
            fArray15[10] = -14.5f;
            fArray = fArray15;
            this.enemy5 = new NpcEnemy(16392, 15, 5, 137, 18, 0.6f, 11.3f, -14.5f, 270.0f, fArray);
            this.enemy5.setGroup(1, 1, 2, 2);
            this.enemy5.setMotion(0, 27);
        }
        if (Runtime.getFlags(8105, 1) == 0) {
            float[] fArray16 = new float[12];
            fArray16[0] = 7.4f;
            fArray16[1] = 10.81f;
            fArray16[2] = -11.5f;
            fArray16[3] = -1.0f;
            fArray16[4] = 7.9f;
            fArray16[5] = 10.81f;
            fArray16[6] = -11.5f;
            fArray16[8] = 6.9f;
            fArray16[9] = 10.81f;
            fArray16[10] = -11.5f;
            fArray = fArray16;
            this.enemy6 = new NpcEnemy(16392, 16, 6, 117, 18, 7.4f, 10.81f, -11.5f, 270.0f, fArray);
            this.enemy6.setGroup(1, 1, 1, 1);
        }
        this.s_unit = new Mapunits();
        this.s_unit.mapUnit(118);
        this.s_unit.start(4, null);
        this.s_unit.start(1, "idle");
        this.enemy2.kickEnepc(19, 1, 0, 420, 0);
        this.enemy2.kickEnepc(19, 2, 0, 770, 1);
        this.enemy2.kickEnepc(19, 3, 0, 420, 1);
        new Uwamono(28675, -5.6f, 7.2f, -6.9f, 180.0f);
        if (Runtime.getFlags(7115, 1) == 0) {
            this.ura = new Uwamono(94, 39, this.npc3);
            this.ura.SetCallNo(2);
        } else {
            new Uwamono(94, 39);
        }
        new Uwamono(85, 39);
        new Uwamono(95, 40);
        if (Runtime.getFlags(7164, 1) == 0) {
            this.douzou = new Uwamono(180, 5);
            this.douzou.SetCallNo(1);
        } else {
            Stage.setVisible(180, false);
        }
        this.teiten12 = new Uwamono(28690, 20.6f, 1.75f, 11.3f, 0.0f);
        this.teiten12.SetBgm(196614);
        this.teiten3 = new Uwamono(28690, -7.1f, 7.25f, 6.1f, 0.0f);
        this.teiten3.SetBgm(196614);
        this.teiten4 = new Uwamono(28690, -10.4f, 5.0f, 2.5f, 0.0f);
        this.teiten4.SetBgm(196614);
        this.teiten56 = new Uwamono(28690, 9.45f, 7.2f, -1.75f, 0.0f);
        this.teiten56.SetBgm(196614);
        this.teiten8 = new Uwamono(28690, 0.0f, 0.0f, 15.1f, 0.0f);
        this.teiten8.SetBgm(196614);
        this.teiten9 = new Uwamono(28690, 3.2f, 8.5f, -5.0f, 0.0f);
        this.teiten9.SetBgm(196614);
        this.teiten10 = new Uwamono(28690, -18.0f, 0.0f, 17.0f, 0.0f);
        this.teiten10.SetBgm(196614);
    }

    void itemget(int n) {
        switch (n) {
            case 1: {
                Runtime.setFlags(3233, 1, 1);
                break;
            }
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }
    }

    class NpcEnemy
            extends Enepc {
        NpcEnemy(int n, int n2, int n3, int n4, int n5) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        NpcEnemy(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, n5, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void idle() {
            float f = 0.0f;
            boolean bl = true;
            ST2210.this.player.getTranslate();
            if (ST2210.this.player.py > 5.0f) {
                bl = false;
                ST2210.this.enemy4.kickEnepc(4, 1);
                if (Runtime.getFlags(8104, 1) == 0) {
                    ST2210.this.enemy5.kickEnepc(4, 1);
                }
                if (Runtime.getFlags(8105, 1) == 0) {
                    ST2210.this.enemy6.kickEnepc(4, 1);
                }
            } else {
                ST2210.this.enemy3.kickEnepc(4, 1);
            }
            System.sleep(1);
            while (true) {
                ST2210.this.player.getTranslate();
                if (ST2210.this.player.py > 5.0f) {
                    if (!bl) {
                        System.println("上に移動");
                        ST2210.this.enemy3.kickEnepc(4, 1);
                        ST2210.this.enemy4.kickEnepc(4, 0);
                        ST2210.this.enemy3.setVisible(false);
                        ST2210.this.enemy4.setVisible(true);
                        if (Runtime.getFlags(8104, 1) == 0) {
                            ST2210.this.enemy5.kickEnepc(4, 0);
                            ST2210.this.enemy5.setVisible(true);
                        }
                        if (Runtime.getFlags(8105, 1) == 0) {
                            ST2210.this.enemy6.kickEnepc(4, 0);
                            ST2210.this.enemy6.setVisible(true);
                        }
                        if (Runtime.getFlags(7114, 1) == 0) {
                            ST2210.this.npc2.setVisible(true);
                        }
                        if (Runtime.getFlags(7115, 1) == 0) {
                            ST2210.this.npc3.setVisible(true);
                        }
                        ST2210.this.fire01.disp(false);
                        ST2210.this.fire02.disp(false);
                        ST2210.this.fire10.disp(false);
                        ST2210.this.fire06.disp(true);
                        ST2210.this.fire09.disp(true);
                    }
                    bl = true;
                } else {
                    if (bl) {
                        System.println("下に移動");
                        ST2210.this.enemy4.kickEnepc(4, 1);
                        ST2210.this.enemy3.kickEnepc(4, 0);
                        ST2210.this.enemy4.setVisible(false);
                        if (Runtime.getFlags(8104, 1) == 0) {
                            ST2210.this.enemy5.kickEnepc(4, 1);
                            ST2210.this.enemy5.setVisible(false);
                        }
                        if (Runtime.getFlags(8105, 1) == 0) {
                            ST2210.this.enemy6.kickEnepc(4, 1);
                            ST2210.this.enemy6.setVisible(false);
                        }
                        ST2210.this.enemy3.setVisible(true);
                        if (Runtime.getFlags(7114, 1) == 0) {
                            ST2210.this.npc2.setVisible(false);
                        }
                        if (Runtime.getFlags(7115, 1) == 0) {
                            ST2210.this.npc3.setVisible(false);
                        }
                        ST2210.this.fire01.disp(true);
                        ST2210.this.fire02.disp(true);
                        ST2210.this.fire10.disp(true);
                        ST2210.this.fire06.disp(false);
                        ST2210.this.fire09.disp(false);
                    }
                    bl = false;
                }
                System.sleep(1);
            }
        }
    }
}

