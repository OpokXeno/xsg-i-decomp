import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS16_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0660
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS16_PRJ {
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
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Light light = new Light(0);
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    int page;

    ST0660() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (Runtime.getFlags(141, 1) == 0 && Runtime.getFlags(142, 1) == 0) {
                    System.println("57_OFF");
                    Runtime.setFlags(3057, 1, 0);
                    Runtime.jumpCF(550, 5);
                    break;
                }
                if (Runtime.getFlags(141, 1) == 1 && Runtime.getFlags(142, 1) == 0) {
                    System.println("55_OFF");
                    Runtime.setFlags(3055, 1, 0);
                    Runtime.jumpCF(690, 5);
                    break;
                }
                if (Runtime.getFlags(142, 1) != 1 || Runtime.getFlags(142, 1) != 1) break;
                Runtime.jumpCF(551, 5);
                break;
            }
            case 1: {
                if (Runtime.getFlags(141, 1) == 0 && Runtime.getFlags(142, 1) == 0) {
                    System.println("58_OFF");
                    Runtime.setFlags(3058, 1, 0);
                    Runtime.jumpCF(550, 6);
                    break;
                }
                if (Runtime.getFlags(141, 1) == 1 && Runtime.getFlags(142, 1) == 0) {
                    System.println("56_OFF");
                    Runtime.setFlags(3056, 1, 0);
                    Runtime.jumpCF(690, 6);
                    break;
                }
                if (Runtime.getFlags(142, 1) != 1 || Runtime.getFlags(142, 1) != 1) break;
                Runtime.jumpCF(551, 6);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        Runtime.setFlags(3060, 1, 1);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(-28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.doorA = new Uwamono(0, 40, '\u0002');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(1, 40, '\u0004');
        this.doorB.SetSize(0.25f, 2.25f, 1.5f);
        this.doorB.SetDoorType('\u0004');
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
}

