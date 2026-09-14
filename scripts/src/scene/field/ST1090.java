import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_UTK09_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1090
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK09_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    Uwamono doorA;
    Uwamono doorF;
    Uwamono K1;
    Uwamono item1;
    Enepc enemy1;
    Light light = new Light(0);
    boolean EnterCheck = false;
    Effect fade;
    int MONBAN = Runtime.getFlags(6028, 1);
    int SUB_KOW = Runtime.getFlags(6029, 1);
    int SEGMENT_A = Runtime.getFlags(3211, 1);
    int SEGMENT_B = Runtime.getFlags(3291, 1);
    int SEGMENT_C = Runtime.getFlags(3231, 1);
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 11.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 11.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 11, decoding complete.", "/[waitkey(64)]/[close()]"};

    ST1090() {
    }

    void DefaultTalk(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
    }

    void EOB(int n) {
        if (n == 1) {
            Runtime.setFlags(6028, 1, 1);
            this.MONBAN = Runtime.getFlags(6028, 1);
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (n2 == 0 && !this.EnterCheck) {
            if (this.MONBAN == 0) {
                this.enemy1.kickEnepc(4, 2);
            }
            this.EnterCheck = true;
            Runtime.setPlayerControl(false);
            if (this.SEGMENT_A == 0) {
                System.println("セグメントＡフラグ・オン");
                Sound.effectPlay(55);
                this.DefaultTalk(this.SUB_01);
                Runtime.setFlags(3211, 1, 1);
                this.SEGMENT_A = Runtime.getFlags(3211, 1);
                this.EnterCheck = false;
                Runtime.setPlayerControl(true);
                if (this.MONBAN == 0) {
                    this.enemy1.kickEnepc(4, 0);
                }
                return;
            }
            if (this.SEGMENT_B == 0) {
                if (this.SEGMENT_C == 0) {
                    this.DefaultTalk(this.SUB_02);
                } else {
                    System.println("セグメントＢフラグ・オン");
                    Sound.effectPlay(56);
                    this.DefaultTalk(this.SUB_03);
                    this.doorF.SetDoorType('\u0004');
                    Runtime.setFlags(3291, 1, 1);
                    this.SEGMENT_B = Runtime.getFlags(3291, 1);
                }
            }
            this.EnterCheck = false;
            Runtime.setPlayerControl(true);
            if (this.MONBAN == 0) {
                this.enemy1.kickEnepc(4, 0);
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1050, 5);
                break;
            }
            case 1: {
                Runtime.setFlags(6029, 1, 1);
                Runtime.jumpCF(1070, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.doorA = new Uwamono(57, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorF = new Uwamono(60, 40, '\u0004');
        if (this.SEGMENT_B == 0) {
            this.doorF.SetDoorType('\u0002');
        } else {
            this.doorF.SetDoorType('\u0004');
        }
        float[] fArray = new float[16];
        fArray[0] = 1.4f;
        fArray[2] = -1.5f;
        fArray[3] = -1.0f;
        fArray[6] = 1.0f;
        fArray[8] = 1.2f;
        fArray[10] = 1.0f;
        fArray[12] = -2.7f;
        fArray[14] = 0.75f;
        fArray[15] = 1.0f;
        float[] fArray2 = fArray;
        if (this.MONBAN == 0) {
            this.enemy1 = new Enepc();
            this.enemy1.init(17153, 3, 1.4f, 0.0f, -1.5f, 270.0f);
            this.enemy1.id = 1;
            this.enemy1.setGroup(0, 0, 0, 0);
            this.enemy1.setParams(0, 2, 1, 3, fArray2);
            this.enemy1.setBatEvent(10);
        }
        if (this.SUB_KOW == 0) {
            this.item1 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 141);
            this.K1 = new Uwamono(59, 5, this.item1);
            this.K1.SetCallNo(1);
        } else {
            Stage.setVisible(59, false);
        }
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
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
}

