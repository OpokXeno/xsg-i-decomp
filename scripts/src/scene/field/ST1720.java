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
import xeno.map.MC_DYU02_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST1720
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU02_PRJ {
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
    Enepc GH1;
    Enepc GH2;
    Enepc GH3;
    Enepc GH4;
    Enepc GH5;
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
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc6talked = 0;
    int npc7talked = 0;
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    int page;

    ST1720() {
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
                Runtime.jumpCF(1760, 2);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 3, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 2, 0.45f, 0.53f, 0.53f);
        Runtime.setIdLightCol(2, 3, 0.45f, 0.53f, 0.53f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(3, 0, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 1, 0.25f, 0.25f, 0.25f);
        Runtime.setIdLightCol(3, 2, 0.4f, 0.43f, 0.43f);
        Runtime.setIdLightCol(3, 3, 0.4f, 0.43f, 0.43f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 0.0f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        F f = new F(20626);
        F f2 = new F(20627);
        F f3 = new F(20628);
        F f4 = new F(20629);
        F f5 = new F(20581);
        f.setTranslate(3.0f, 0.9f, -2.0f);
        f.setRotate(0.0f, 75.0f, 0.0f);
        f2.setTranslate(-3.0f, 0.84f, -5.0f);
        f2.setRotate(0.0f, 150.0f, 0.0f);
        f3.setTranslate(3.0f, 0.69f, -5.0f);
        f3.setRotate(0.0f, 225.0f, 0.0f);
        f4.setTranslate(-3.0f, 0.84f, -2.0f);
        f4.setRotate(0.0f, 300.0f, 0.0f);
        f5.setTranslate(0.0f, 0.5f, -10.5f);
        f5.setRotate(0.0f, 0.0f, 0.0f);
        f.argy = 0.5f;
        f.argz = 0.2f;
        f.argrx = 0.6f;
        f.argry = 0.3f;
        f.argrz = 0.2f;
        f.fpmax = 0.1f;
        f.frmax = 5.0f;
        f.start(1, "fl");
        f2.argx = 0.1f;
        f2.argy = 0.5f;
        f2.argz = 0.8f;
        f2.argrx = 0.1f;
        f2.argry = 0.4f;
        f2.argrz = 0.4f;
        f2.fpmax = 0.1f;
        f2.frmax = 5.0f;
        f2.start(1, "fl");
        f3.argx = 0.3f;
        f3.argy = 0.5f;
        f3.argz = 0.7f;
        f3.argrx = 0.7f;
        f3.argry = 0.2f;
        f3.argrz = 0.4f;
        f3.fpmax = 0.1f;
        f3.frmax = 5.0f;
        f3.start(1, "fl");
        f4.argx = 0.2f;
        f4.argy = 0.4f;
        f4.argz = 0.6f;
        f4.argrx = 0.7f;
        f4.argry = 0.1f;
        f4.argrz = 0.3f;
        f4.fpmax = 0.1f;
        f4.frmax = 5.0f;
        f4.start(1, "fl");
        f5.argx = 0.1f;
        f5.argy = 0.4f;
        f5.argz = 0.6f;
        f5.argrx = 0.7f;
        f5.argry = 0.1f;
        f5.argrz = 0.3f;
        f5.fpmax = 0.1f;
        f5.frmax = 5.0f;
        f5.start(1, "fl");
        f.dispRadar(false);
        f2.dispRadar(false);
        f3.dispRadar(false);
        f4.dispRadar(false);
        f5.dispRadar(false);
        this.doorA = new Uwamono(14, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
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

    class F
            extends Chr {
        float basex;
        float basey;
        float basez;
        float baserx;
        float basery;
        float baserz;
        float cx;
        float cy;
        float cz;
        float crx;
        float cry;
        float crz;
        float r1;
        float r2;
        float r3;
        float r4;
        float r5;
        float r6;
        float argx;
        float argy;
        float argz;
        float argrx;
        float argry;
        float argrz;
        float fpmax;
        float frmax;

        public F(int n) {
            this.init(n);
        }

        void fl() {
            this.getTranslate();
            this.getRotate();
            this.basex = this.px;
            this.basey = this.py;
            this.basez = this.pz;
            this.baserx = this.rx;
            this.basery = this.ry;
            this.baserz = this.rz;
            this.r1 = this.argx;
            this.r2 = this.argy;
            this.r3 = this.argz;
            this.r4 = this.argrx;
            this.r5 = this.argry;
            this.r6 = this.argrz;
            while (true) {
                this.cx = Math.sin(Math.toRadians(this.r1)) * this.fpmax;
                this.cy = Math.cos(Math.toRadians(this.r2)) * this.fpmax;
                this.cz = Math.sin(Math.toRadians(this.r3)) * this.fpmax;
                this.crx = Math.cos(Math.toRadians(this.r4)) * this.frmax;
                this.cry = Math.sin(Math.toRadians(this.r5)) * this.frmax;
                this.crz = Math.cos(Math.toRadians(this.r6)) * this.frmax;
                this.px = this.basex + this.cx;
                this.py = this.basey + this.cy;
                this.pz = this.basez + this.cz;
                this.rx = this.baserx + this.crx;
                this.ry = this.basery + this.cry;
                this.rz = this.baserz + this.crz;
                this.setTranslate();
                this.setRotate();
                this.r1 += this.argx;
                this.r2 += this.argy;
                this.r3 += this.argz;
                this.r4 += this.argrx;
                this.r5 += this.argry;
                this.r6 += this.argrz;
                System.sleep(1);
            }
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
    }
}

