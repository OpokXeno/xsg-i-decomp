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
import xeno.map.MC_VOK25_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0250
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK25_PRJ {
    Player player;
    Camera cam1;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Menu menu;
    Window win;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono trap1;
    Uwamono trap2;
    Uwamono trap3;
    Uwamono trap4;
    Uwamono trap5;
    Uwamono teiten1;
    int S03;
    int B01;
    int F09;
    Effect fade;
    Light light = new Light(0);
    Unit kidou;
    int button_flg;
    int page;
    String[] msg_allen_0 = new String[]{"/[label(Allen)]", "Chief.\n", "The Target drones have been changed.", "/[waitkey(64)]/[clear()]"};
    String[] msg_allen_1 = new String[]{"/[label(Allen)]", "The next target responds only to sound. It will approach you if you make a loud noise, but it won't notice you if you're merely in its field of vision!", "/[waitkey(64)]/[close()]"};

    ST0250() {
    }

    void Final_init(int n) {
        switch (n) {
            case 1: {
                this.enemy1.kickEnepc(4, 2);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        System.println("KickEvent!!!!!!!!!!!!");
        switch (n2) {
            case 0: {
                this.B01 = Runtime.getFlags(1020, 1);
                Runtime.setRegister(0, this.B01);
                System.println("B01: /[$0]");
                if (this.B01 == 0) {
                    Runtime.setFlags(1020, 1, 1);
                    System.println("Event On");
                    Runtime.setPlayerControl(false);
                    this.fade.call(0);
                    System.sleep(30);
                    Sound.effectStop(196612);
                    Runtime.setPlayerControl(true);
                    Runtime.jumpEvent(1901);
                    return;
                }
                System.println("Event Pass");
                break;
            }
        }
    }

    void NotYet() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print("工事中です/[wait(30)]/[close()]");
    }

    void broken(int n) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S03 == 1) {
                    Runtime.jumpCF(241, 2);
                    return;
                }
                Runtime.jumpCF(240, 2);
                break;
            }
            case 1: {
                if (this.S03 == 1) {
                    Runtime.jumpCF(241, 3);
                    return;
                }
                Runtime.jumpCF(240, 3);
                break;
            }
            case 2: {
                if (this.S03 == 1) {
                    Runtime.jumpCF(241, 4);
                    return;
                }
                Runtime.jumpCF(240, 4);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 3.5f, 0.0f, 8.25f, 0.0f);
        this.teiten1.SetBgm(196619);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.78f, 0.78f, 0.78f);
        this.light.setDirection2(1, -0.023f, 1.0f, 0.008f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.38f, 0.38f, 0.38f);
        this.light.setDirection2(2, -0.015f, 0.81f, 0.586f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.45f, 0.45f, 0.45f);
        this.light.setDirection2(3, 0.024f, -0.0f, -1.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 0, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 1, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 2, 0.15f, 0.15f, 0.15f);
        Runtime.setIdLightCol(2, 3, 0.15f, 0.15f, 0.15f);
        this.S03 = Runtime.getFlags(3, 1);
        this.B01 = Runtime.getFlags(1020, 1);
        this.F09 = Runtime.getFlags(1009, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setFog(1, 8.5f, 18.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setFog(2, 8.5f, 18.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setFog(3, 8.5f, 18.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setFog(4, 8.5f, 18.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setFog(5, 8.5f, 18.0f, 0.0f, 0.4f, 255, 255, 255, 255);
        this.cam0.setFog(6, 8.5f, 18.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setFog(7, 8.3f, 15.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setFog(8, 10.5f, 18.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setFog(9, 8.5f, 18.0f, 0.0f, 0.3f, 255, 255, 255, 255);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 375.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 345.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 0.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFLockX(6, 5.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFLockX(7, 4.75f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, -3.5f);
        this.cam0.setCFAngle(9, -28.0f, 335.0f, 0.0f, 4.5f, 40.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.enemy1 = new Enepc();
        this.enemy1.init(16385, 3, -11.67f, 9.03f, -0.005f, 0.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(1, 1, 1, 1);
        this.enemy1.setParams(0, 31, 1, 3);
        this.enemy1.dispRadar(false);
        this.enemy1.setInvalidID(1);
        this.enemy1.kickEnepc(19, 1, 0, 475, 1);
        new Uwamono(17, 0);
        this.doorA = new Uwamono(19, 40, '\u0001');
        this.doorB = new Uwamono(20, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        new Uwamono(28678, 2.14f, 6.0f, -7.213f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.kidou = new Mapunits();
        this.kidou.mapUnit(21);
        if (this.F09 == 0 & this.S03 == 1) {
            this.kidou.start(4, null);
            this.kidou.start(1, "Msg");
        } else {
            this.kidou.start(4, null);
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Msg() {
            Runtime.setPlayerControl(false);
            ST0250.this.win = Window.create();
            ST0250.this.win.print(ST0250.this.msg_allen_0, 0);
            ST0250.waitPage(ST0250.this.win, 64);
            ST0250.this.win.print(ST0250.this.msg_allen_1, 0);
            ST0250.waitPage(ST0250.this.win, 64);
            Runtime.setFlags(1009, 1, 1);
            Runtime.setPlayerControl(true);
        }
    }
}

