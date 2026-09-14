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
import xeno.map.MC_PRO05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Input;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0852
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_PRO05_PRJ {
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
    Effect fade;
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
    int npc8talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono item1;
    Uwamono item2;
    Uwamono item3;
    Uwamono item4;
    Uwamono item5;
    Uwamono item6;
    MAPUnit mu;
    Effect light03;
    Effect red01;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    int lo = 0;
    Light light = new Light(0);
    int page;

    ST0852() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        if (this.lo == 1) {
            return;
        }
        switch (n2) {
            case 0: {
                return;
            }
            case 1: {
                return;
            }
            case 2: {
                return;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            case 0: {
                Runtime.jumpCF(840, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(860, 1);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        Stage.setColor(1.35f, 1.35f, 1.35f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.425f, 0.425f, 0.425f);
        Runtime.setIdLightCol(1, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 2, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(1, 3, 0.3f, 0.3f, 0.3f);
        Stage.setVisible(-1, true);
        this.red01 = new Effect(1551, 0.0f, 0.0f, 0.0f, 0.0f);
        this.light03 = new Effect(1055, -3.14f, 1.35f, 7.35f, 0.0f);
        this.light03.setRotate(15.0f, -90.0f, 0.0f);
        this.light03.setScale(1.5f, 1.0f, 1.0f);
        this.light03.disp(true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.player.dispRadar(false);
        Stage.setVisible(71, false);
        Stage.setVisible(70, false);
        Stage.setVisible(69, false);
        Stage.setVisible(5, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 100.0f, 100.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.enemy1 = new NPC_NORMAL(16645, 1, 0, 0, 3, -0.04f, 0.0f, -13.2f, 0.0f);
        this.enemy1.dispRadar(false);
        this.enemy2 = new NPC_NORMAL(16646, 2, 0, 0, 5, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy2.setInvalidID(1);
        this.enemy2.dispRadar(false);
        this.enemy3 = new NPC_NORMAL(16646, 3, 0, 0, 5, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy3.setInvalidID(1);
        this.enemy3.dispRadar(false);
        this.enemy4 = new NPC_NORMAL(16646, 4, 0, 0, 5, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy4.setInvalidID(1);
        this.enemy4.dispRadar(false);
        this.enemy5 = new NPC_NORMAL(16646, 5, 0, 0, 5, 0.0f, 0.0f, 0.0f, 0.0f);
        this.enemy5.setInvalidID(1);
        this.enemy5.dispRadar(false);
        this.mu = new Mapunits();
        this.mu.mapUnit(38);
        this.mu.start(1, "event");
        this.doorA = new Uwamono(75, 42, '\u0001');
        new Uwamono(76, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(72, 42, '\u0001');
        new Uwamono(73, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(74, 40, '\u0001');
        this.doorC.SetDoorType('\u0002');
        this.doorA.SetDoorType('\u0002');
        this.doorA.DoorOpen();
        Runtime.disable(524288);
        this.player.setTranslate(9.4f, 0.54f, -4.16f);
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
            this.dispRadar(false);
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
        }

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class NPC_EVENT
            extends Enepc {
        NPC_EVENT() {
        }

        void init() {
        }

        public void talk() {
        }
    }

    class Mapunits
            extends MAPUnit {
        int time = 0;
        int endtime = 0;
        float dx = 0.0f;
        float dy = 0.0f;
        float dz = 0.0f;
        float dx2 = 0.0f;
        float dy2 = 0.0f;
        float dz2 = 0.0f;
        float dx3 = 0.0f;
        float dy3 = 0.0f;
        float dz3 = 0.0f;
        float dx4 = 0.0f;
        float dy4 = 0.0f;
        float dz4 = 0.0f;
        float dx5 = 0.0f;
        float dy5 = 0.0f;
        float dz5 = 0.0f;
        int mokuteki = 0;
        float[][] route1;
        int routep1;

        Mapunits() {
            float[][] fArrayArray = new float[2][];
            float[] fArray = new float[4];
            fArray[0] = -0.04f;
            fArray[2] = 7.2f;
            fArray[3] = 300.0f;
            fArrayArray[0] = fArray;
            fArrayArray[1] = new float[]{-9.0f, 1.0f, 8.5f, 25.0f};
            this.route1 = fArrayArray;
            this.routep1 = this.route1[0].length;
        }

        public void event() {
            Runtime.setPlayerControl(false);
            ST0852.this.cam0.setMode(-1);
            ST0852.this.camEV = Camera.create(1);
            ST0852.this.camEV.setTranslate(0.0f, 4.7f, -0.6f);
            ST0852.this.camEV.setRotate(-28.6f, 0.0f, 0.0f);
            ST0852.this.camEV.setFov(63.0f);
            ST0852.this.camEV.change();
            int n = 30;
            float f = 0.0f;
            ST0852.this.enemy1.setMotion(0, 1);
            int n2 = 27;
            ST0852.this.enemy2.setMotion(0, n2);
            ST0852.this.enemy3.setMotion(0, n2);
            ST0852.this.enemy4.setMotion(0, n2);
            ST0852.this.enemy5.setMotion(0, n2);
            int n3 = 60;
            Input input = Input.create(0);
            boolean bl = false;
            boolean bl2 = false;
            while (true) {
                if (this.time == 0) {
                    Sound.streamPlay(1195006, 48000);
                    this.mokuteki = 0;
                    ST0852.this.enemy1.getTranslate();
                    this.dx = (this.route1[0][0] - ST0852.this.enemy1.px) / this.route1[0][3];
                    this.dy = (this.route1[0][1] - ST0852.this.enemy1.py) / this.route1[0][3];
                    this.dz = (this.route1[0][2] - ST0852.this.enemy1.pz) / this.route1[0][3];
                    float[] fArray = new float[8];
                    fArray[2] = 5.44f;
                    fArray[3] = 7.52f;
                    fArray[4] = 360.0f;
                    fArray[5] = 2.95f;
                    fArray[6] = 1.5f;
                    fArray[7] = 9.21f;
                    float[] fArray2 = fArray;
                    float[] fArray3 = new float[8];
                    fArray3[1] = -23.04f;
                    fArray3[4] = 540.0f;
                    fArray3[5] = 5.52f;
                    fArray3[6] = 69.82f;
                    float[] fArray4 = fArray3;
                    ST0852.this.camEV = Camera.create(1);
                    ST0852.this.camEV.transSPL(fArray2, 0, 1, 360);
                    ST0852.this.camEV.rotateSPL(fArray4, 0, 1, 540);
                    ST0852.this.camEV.change();
                }
                if (this.time > 0 && this.time <= 300) {
                    ST0852.this.enemy1.setTranslate(ST0852.this.enemy1.px + this.dx, ST0852.this.enemy1.py + this.dy, ST0852.this.enemy1.pz + this.dz);
                    ST0852.this.enemy2.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4) * 3.14f / 180.0f) * 1.5f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4) * 3.14f / 180.0f) * 1.5f);
                    ST0852.this.enemy3.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 90) * 3.14f / 180.0f) * 1.5f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 90) * 3.14f / 180.0f) * 1.5f);
                    ST0852.this.enemy4.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f) * 1.5f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f) * 1.5f);
                    ST0852.this.enemy5.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f) * 1.5f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f) * 1.5f);
                }
                if (this.time == 30) {
                    ST0852.this.doorA.DoorClose();
                }
                if (this.time == 301) {
                    n2 = 0;
                    ST0852.this.enemy1.setMotion(0, 4);
                    ST0852.this.enemy2.setMotion(0, n2);
                    ST0852.this.enemy3.setMotion(0, n2);
                    ST0852.this.enemy4.setMotion(0, n2);
                    ST0852.this.enemy5.setMotion(0, n2);
                }
                if (this.time == 331) {
                    ST0852.this.light03.disp(false);
                }
                if (this.time == 334) {
                    ST0852.this.light03.disp(true);
                }
                if (this.time == 337) {
                    ST0852.this.light03.disp(false);
                }
                if (this.time == 340) {
                    ST0852.this.light03.disp(true);
                }
                if (this.time == 360) {
                    ST0852.this.doorC.DoorOpen();
                }
                if (this.time > 300 && this.time <= 360 + n3) {
                    f = 1.5f;
                    ST0852.this.enemy2.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4) * 3.14f / 180.0f) * f);
                    ST0852.this.enemy3.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 90) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 90) * 3.14f / 180.0f) * f);
                    ST0852.this.enemy4.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f) * f);
                    ST0852.this.enemy5.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f) * f);
                }
                if (this.time == 360 + n3) {
                    ST0852.this.enemy2.setInvalidID(0);
                    ST0852.this.enemy3.setInvalidID(0);
                    ST0852.this.enemy4.setInvalidID(0);
                    ST0852.this.enemy5.setInvalidID(0);
                    ST0852.this.enemy3.getTranslate();
                    this.dx3 = (this.route1[1][0] - ST0852.this.enemy3.px) / this.route1[1][3];
                    this.dy3 = (this.route1[1][1] - ST0852.this.enemy3.py) / this.route1[1][3];
                    this.dz3 = (this.route1[1][2] - ST0852.this.enemy3.pz) / this.route1[1][3];
                }
                if (this.time == 340 + n3) {
                    ST0852.this.enemy3.setMotion(0, 3);
                }
                if (360 + n3 < this.time && this.time <= 385 + n3) {
                    ST0852.this.enemy3.setTranslate(ST0852.this.enemy3.px + this.dx3, ST0852.this.enemy3.py + this.dy3, ST0852.this.enemy3.pz + this.dz3);
                    ST0852.this.enemy2.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4) * 3.14f / 180.0f));
                    ST0852.this.enemy4.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f));
                    ST0852.this.enemy5.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f));
                }
                if (this.time == 385 + n3) {
                    ST0852.this.enemy2.setMotion(0, 3);
                }
                if (this.time == 385 + n3) {
                    ST0852.this.enemy2.getTranslate();
                    this.dx2 = (this.route1[1][0] - ST0852.this.enemy2.px) / this.route1[1][3];
                    this.dy2 = (this.route1[1][1] - ST0852.this.enemy2.py) / this.route1[1][3];
                    this.dz2 = (this.route1[1][2] - ST0852.this.enemy2.pz) / this.route1[1][3];
                }
                if (385 + n3 < this.time && this.time <= 410 + n3) {
                    ST0852.this.enemy2.setTranslate(ST0852.this.enemy2.px + this.dx2, ST0852.this.enemy2.py + this.dy2, ST0852.this.enemy2.pz + this.dz2);
                    ST0852.this.enemy4.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f));
                    ST0852.this.enemy5.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 270) * 3.14f / 180.0f));
                }
                if (this.time == 365 + n3) {
                    ST0852.this.enemy5.setMotion(0, 3);
                }
                if (this.time == 410 + n3) {
                    ST0852.this.enemy5.getTranslate();
                    this.dx5 = (this.route1[1][0] - ST0852.this.enemy5.px) / this.route1[1][3];
                    this.dy5 = (this.route1[1][1] - ST0852.this.enemy5.py) / this.route1[1][3];
                    this.dz5 = (this.route1[1][2] - ST0852.this.enemy5.pz) / this.route1[1][3];
                }
                if (410 + n3 < this.time && this.time <= 435 + n3) {
                    ST0852.this.enemy5.setTranslate(ST0852.this.enemy5.px + this.dx5, ST0852.this.enemy5.py + this.dy5, ST0852.this.enemy5.pz + this.dz5);
                    ST0852.this.enemy4.setTranslate(ST0852.this.enemy1.px + Math.sin((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f) * f, 2.0f, ST0852.this.enemy1.pz + Math.cos((float) ((this.time + n) * 4 + 180) * 3.14f / 180.0f));
                }
                if (this.time == 435 + n3) {
                    ST0852.this.enemy4.setMotion(0, 3);
                }
                if (this.time == 435 + n3) {
                    ST0852.this.enemy4.getTranslate();
                    this.dx4 = (this.route1[1][0] - ST0852.this.enemy4.px) / this.route1[1][3];
                    this.dy4 = (this.route1[1][1] - ST0852.this.enemy4.py) / this.route1[1][3];
                    this.dz4 = (this.route1[1][2] - ST0852.this.enemy4.pz) / this.route1[1][3];
                }
                if (435 + n3 < this.time && this.time <= 460 + n3) {
                    ST0852.this.enemy4.setTranslate(ST0852.this.enemy4.px + this.dx4, ST0852.this.enemy4.py + this.dy4, ST0852.this.enemy4.pz + this.dz4);
                }
                if (this.time == 480 + n3) break;
                ++this.time;
                System.sleep(1);
            }
            ST0852.this.fade.call(0);
            System.sleep(30);
            ST0852.this.cam0.setMode(0);
            Runtime.setPlayerControl(true);
            Runtime.jumpCF(860, 1);
        }
    }
}

