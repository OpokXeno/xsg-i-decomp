import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_KUK06B_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2201
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK06B_PRJ {
    Player player;
    Camera cam0;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int BUTTON_F = 0;
    Uwamono doorA;
    Enepc HASIGO;
    Light light = new Light(0);
    Enepc npc1;
    Enepc npc2;
    int talkFlag1;
    int talkFlag2;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect light04;
    Effect light05;
    Effect light06;
    Effect light07;
    Effect light08;
    Effect light10;
    Effect light09;
    Effect photo;
    Uwamono item01;
    Uwamono item02;
    int page;
    String[] msg50001 = new String[]{"/[label()]", "My house got wrecked, so I'm staying here.", "/[waitkey(1)]/[clear()]", "But you know, I couldn't just sit there being depressed while that little girl worked so hard.", " That's why I thought I'd at least help repair the hotel!", "/[waitkey(64)]/[close()]"};
    String[] msgkey = new String[]{"/[label()]", "'Surrounded by loving parents.'", "/[waitkey(64)]/[close()]"};

    ST2201() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                this.player.getTranslate();
                if (this.player.py < 5.0f) {
                    return;
                }
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgkey, 0);
                ST2201.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    void Talk_npc1_1(Window window) {
        window.print(this.msg50001, 0);
        ST2201.waitPage(window, 64);
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("襲撃後宿屋１Ｆ・２");
                Runtime.jumpCF(2051, 2);
                break;
            }
            case 1: {
                System.println("襲撃後外観街１・５");
                Runtime.jumpCF(2171, 5);
                break;
            }
            case 2: {
                System.println("襲撃後外観街１・９");
                Runtime.jumpCF(2171, 9);
                break;
            }
            case 3: {
                System.println("襲撃後酒場２Ｆ・２");
                Runtime.jumpCF(2041, 2);
                break;
            }
        }
    }

    void init() {
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(1, 1, -0.25f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(2, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(2, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(3, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 3, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(4, 0, 0.415f, 0.415f, 0.395f);
        Runtime.setIdLightCol(4, 1, 0.415f, 0.415f, 0.395f);
        Runtime.setIdLightCol(4, 2, 0.415f, 0.415f, 0.395f);
        Runtime.setIdLightCol(4, 3, 0.415f, 0.415f, 0.395f);
        Runtime.setIdLightVec(4, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(4, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(4, 3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(5, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(5, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(5, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(5, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(5, 3, 0.0f, -1.0f, -3.0f);
        this.light01 = new Effect(1405, 2.3f, 8.3f, -4.2f, 0.0f);
        this.light02 = new Effect(1405, -1.0f, 7.4f, 0.8f, 0.0f);
        this.light03 = new Effect(1405, 8.7f, 1.0f, -4.3f, 0.0f);
        this.light04 = new Effect(1405, 1.6f, 2.9f, -2.0f, 0.0f);
        this.light05 = new Effect(1405, -1.5f, 3.1f, -3.5f, 0.0f);
        this.light06 = new Effect(1405, -10.5f, 2.5f, 3.0f, 0.0f);
        this.light07 = new Effect(1405, -10.5f, 2.5f, 0.0f, 0.0f);
        this.light08 = new Effect(1405, -10.5f, 2.5f, -3.0f, 0.0f);
        this.light01.setScale(0.5f, 0.5f, 0.5f);
        this.light02.setScale(0.2f, 0.2f, 0.2f);
        this.light03.setScale(0.5f, 0.5f, 0.5f);
        this.light04.setScale(0.3f, 0.3f, 0.3f);
        this.light06.setScale(0.5f, 0.5f, 0.5f);
        this.light07.setScale(0.5f, 0.5f, 0.5f);
        this.light08.setScale(0.5f, 0.5f, 0.5f);
        Runtime.progressEffect(60);
        this.photo = new Effect(1542, -2.546f, 7.791f, -5.965f, 0.0f);
        this.photo.setScale(1.55f, 1.85f, 1.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(34, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.02f, 0.02f);
        this.cam0.setCFPedestal(4, 2.8693266f, 9.023886f, -4.656635f, 51.760002f, -71.67927f, -39.059605f, 0.0f, 2.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 5.5f, 40.0f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
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
        new Uwamono(28672, 4.8f, 5.1f, -3.6f, 0.0f);
        this.item01 = new Uwamono(28684, 0.0f, 0.0f, 0.0f, 0.0f, 258);
        this.item02 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 259);
        new Uwamono(0, 44, this.item02);
        new Uwamono(56, 64, this.item01);
        this.HASIGO = new NPC_NORMAL(519, 11, 0, 0, 5, 100.0f, 0.0f, 100.0f, 0.0f);
        this.HASIGO.kickEnepc(4, 2);
        this.HASIGO.setInvalidID(1);
        this.HASIGO.setVisible(false);
        this.HASIGO.dispRadar(false);
        this.HASIGO.disableDTKFlag(131072);
        this.HASIGO.disableDTKFlag(65536);
        this.HASIGO.kickEnepc(19, 1, 0, 630, 1);
        this.npc1 = new NPC_NORMAL(527, 1, 0, 14, 9, -10.1f, 0.0f, -4.45f, 200.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 4);
        this.npc1.setInvalidID(1);
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

