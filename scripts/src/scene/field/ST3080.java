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
import xeno.map.MC_GNK14_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST3080
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_GNK14_PRJ {
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
    Enepc shi;
    Enepc zig;
    Enepc jun;
    Enepc cha;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect E06;
    Effect E07;
    Effect E08;
    Effect E09;
    Effect E10;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Light light = new Light(0);
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
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    int page;
    String[] Info_00 = new String[]{"Press the switch?", "/[waitkey(64)]/[close()]"};
    String[] Info_01 = new String[]{"The lock has been removed.", "/[waitkey(64)]/[close()]"};
    String[] CHA_00 = new String[]{"/[label(chaos)]", "This one's a list of sick and wounded soldiers. Looks like it's from during the Miltian Conflict.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_00 = new String[]{"/[label(Ziggy)]", "They were transferred to Miltia for treatment, just like those Realians we saw earlier. An odd coincidence.", "/[waitkey(64)]/[close()]"};
    String[] SHI_00 = new String[]{"/[label(Shion)]", "Those don't look like ordinary wounds.", "/[waitkey(1)]/[clear()]", "There're only fragments of the data left, but it looks like something more serious, like mental illness.", "/[waitkey(64)]/[close()]"};
    String[] JUN_00 = new String[]{"/[label(Jr.)]", "You can tell all that from these fragments? ", "/[waitkey(64)]/[close()]"};
    String[] SHI_01 = new String[]{"/[label(Shion)]", "Yes.", "/[waitkey(1)]/[clear()]", "The list of medications here are all ones used to treat psychological disorders.", "/[waitkey(64)]/[close()]"};
    String[] JUN_01 = new String[]{"/[label(Jr.)]", "I see.", "/[waitkey(1)]/[clear()]", "I guess there's a reason why you're a chief engineer at Vector.", "/[waitkey(64)]/[close()]"};
    String[] SHI_02 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] JUN_02 = new String[]{"/[label(Jr.)]", "What?", "/[waitkey(1)]/[clear()]", "Did I say something weird?", "/[waitkey(64)]/[close()]"};
    String[] SHI_03 = new String[]{"/[label(Shion)]", "What?", "/[waitkey(1)]/[clear()]", "No...it's nothing.", "/[waitkey(64)]/[close()]"};
    String[] CHA_01 = new String[]{"/[label(chaos)]", "...", "/[waitkey(64)]/[close()]"};
    String[] SYS_00 = new String[]{"There is a list of sick and wounded soldiers that were transferred from Miltia. It seems they were receiving treatment for psychological disorders.", "/[waitkey(64)]/[close()]"};

    ST3080() {
    }

    void EOB(int n) {
        if (n == 2) {
            System.println("WINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWINWIN");
            Runtime.setFlags(3185, 1, 1);
        }
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.336f, 7.311f, 2.39f);
        this.camEV.setRotate(-40.813f, -35.939f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.712f, 2.215f, -11.62f);
        this.camEV.setRotate(-10.603f, -47.719f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        float[] fArray = new float[]{1.0f, 1.826f, 2.079f, -17.709f, 60.0f, 1.826f, 1.631f, -17.709f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -14.976f;
        fArray2[2] = -165.259f;
        fArray2[4] = 60.0f;
        fArray2[5] = -14.976f;
        fArray2[6] = -165.259f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 60);
        this.camEV.rotateSPL(fArray3, 1, 3, 60);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.171f, 1.887f, -18.036f);
        this.camEV.setRotate(-14.356f, 170.758f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.584f, 1.567f, -17.979f);
        this.camEV.setRotate(-11.076f, 224.916f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera05() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.556f, 1.407f, -17.156f);
        this.camEV.setRotate(-18.035f, 149.336f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.052f, 1.983f, -18.579f);
        this.camEV.setRotate(-27.575f, 170.534f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera07() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.574f, 2.175f, -17.484f);
        this.camEV.setRotate(-34.815f, -191.938f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3089, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Info_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.cam0.setMode(-1);
                            this.EV_Camera00();
                            Runtime.enable(65536);
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(30);
                            Sound.effectPlay(196741);
                            System.sleep(30);
                            this.E03.disp(true);
                            Sound.streamPlay(1195014, 48000);
                            System.sleep(120);
                            Runtime.setFlags(3089, 1, 1);
                            this.E04.disp(false);
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.jumpCF(68657, 1);
                            Runtime.setPlayerControl(true);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    return;
                }
            }
            return;
        }
        if (n2 != 1) return;
        switch (n) {
            case 100: {
                if (Runtime.getFlags(3142, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.shi.kickEnepc(4, 1);
                    this.zig.kickEnepc(4, 1);
                    this.jun.kickEnepc(4, 1);
                    this.cha.kickEnepc(4, 1);
                    this.fade1.call(0);
                    System.sleep(60);
                    this.player.setTranslate(100.0f, 0.0f, 100.0f);
                    this.shi.setLocation(4, 0);
                    this.jun.setLocation(4, 1);
                    this.zig.setLocation(4, 2);
                    this.cha.setLocation(4, 3);
                    this.cam0.setMode(-1);
                    this.EV_Camera01();
                    this.fade2.call(0);
                    System.sleep(60);
                    System.sleep(30);
                    this.EV_Camera03();
                    this.cha.kickEnepc(9, 12);
                    this.zig.kickEnepc(9, 14);
                    this.cha.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.CHA_00, 0);
                    System.waitFor(this.win);
                    this.zig.kickEnepc(0, 7);
                    System.sleep(40);
                    this.zig.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.ZIG_00, 0);
                    System.waitFor(this.win);
                    this.EV_Camera04();
                    this.cha.kickEnepc(9, 11);
                    this.zig.kickEnepc(9, 11);
                    this.shi.kickEnepc(0, 7);
                    System.sleep(40);
                    this.shi.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SHI_00, 0);
                    System.waitFor(this.win);
                    this.jun.kickEnepc(9, 11);
                    this.jun.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.JUN_00, 0);
                    System.waitFor(this.win);
                    this.shi.kickEnepc(9, 13);
                    this.shi.kickEnepc(0, 7);
                    System.sleep(40);
                    this.shi.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SHI_01, 0);
                    System.waitFor(this.win);
                    this.EV_Camera05();
                    this.jun.kickEnepc(0, 7);
                    System.sleep(50);
                    this.jun.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.JUN_01, 0);
                    System.waitFor(this.win);
                    this.EV_Camera06();
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SHI_02, 0);
                    System.waitFor(this.win);
                    this.jun.kickEnepc(0, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.JUN_02, 0);
                    System.waitFor(this.win);
                    this.EV_Camera02();
                    this.shi.kickEnepc(0, 8);
                    System.sleep(40);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.SHI_03, 0);
                    System.waitFor(this.win);
                    this.shi.kickEnepc(9, -1);
                    this.cha.kickEnepc(9, 11);
                    System.sleep(60);
                    this.EV_Camera07();
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.CHA_01, 0);
                    System.waitFor(this.win);
                    this.fade1.call(0);
                    System.sleep(60);
                    this.cam0.setMode(0);
                    this.player.setTranslate(2.0f, 0.0f, -15.0f);
                    this.shi.setTranslate(100.0f, 100.0f, 100.0f);
                    this.zig.setTranslate(100.0f, 100.0f, 100.0f);
                    this.jun.setTranslate(100.0f, 100.0f, 100.0f);
                    this.cha.setTranslate(100.0f, 100.0f, 100.0f);
                    Runtime.setFlags(3142, 1, 1);
                    this.fade2.call(0);
                    System.sleep(60);
                    Runtime.setPlayerControl(true);
                    return;
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.SYS_00, 0);
                System.waitFor(this.win);
                Runtime.setPlayerControl(true);
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(68596, 2);
                break;
            }
            case 1: {
                Runtime.jumpCF(68586, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -16.0f, 0.0f, -5.0f, 0.0f);
        this.teiten1.SetBgm(196616);
        this.teiten2 = new Uwamono(28690, -12.5f, 0.0f, -14.5f, 0.0f);
        this.teiten2.SetBgm(196616);
        this.teiten3 = new Uwamono(28690, -5.5f, 0.0f, -11.0f, 0.0f);
        this.teiten3.SetBgm(196616);
        this.teiten4 = new Uwamono(28690, 6.4f, 0.0f, -10.5f, 0.0f);
        this.teiten4.SetBgm(196616);
        this.teiten5 = new Uwamono(28690, 16.0f, 0.0f, -5.0f, 0.0f);
        this.teiten5.SetBgm(196616);
        this.teiten6 = new Uwamono(28690, 0.0f, 0.0f, -2.5f, 0.0f);
        this.teiten6.SetBgm(196616);
        Stage.setVisible(-1, true);
        this.E01 = new Effect(1643, -3.205f, 1.501f, -12.974f, 0.0f);
        this.E01.disp(true);
        this.E01.setClip(true);
        this.E01.noAttach(false);
        this.E02 = new Effect(1643, 3.201f, 1.501f, -12.974f, 0.0f);
        this.E02.disp(true);
        this.E02.setClip(true);
        this.E02.noAttach(false);
        if (Runtime.getFlags(3089, 1) == 0) {
            this.E03 = new Effect(1642, 1.843f, 1.252f, -1.127f, 0.0f);
            this.E03.disp(false);
            this.E03.setClip(true);
            this.E03.setScale(1.0f, 0.5f, 1.0f);
            this.E03.noAttach(false);
        } else {
            this.E03 = new Effect(1642, 1.843f, 1.252f, -1.127f, 0.0f);
            this.E03.disp(true);
            this.E03.setClip(true);
            this.E03.setScale(1.0f, 0.5f, 1.0f);
            this.E03.noAttach(false);
            Runtime.progressEffect(30);
        }
        if (Runtime.getFlags(3089, 1) == 0) {
            this.E04 = new Effect(1417, 0.0f, 1.025f, -2.643f, 0.0f);
            this.E04.disp(true);
            this.E04.setClip(true);
            this.E04.setScale(1.3f, 1.3f, 1.3f);
            this.E04.setRotate(270.0f, 0.0f, 0.0f);
            this.E04.noAttach(false);
        } else {
            this.E04 = new Effect(1417, 0.0f, 1.025f, -2.643f, 0.0f);
            this.E04.disp(false);
            this.E04.setClip(true);
            this.E04.setScale(1.3f, 1.3f, 1.3f);
            this.E04.setRotate(270.0f, 0.0f, 0.0f);
            this.E04.noAttach(false);
        }
        this.E05 = new Effect(1760, -0.8f, 2.172f, -18.64f, 0.0f);
        this.E05.disp(true);
        this.E05.setClip(true);
        this.E05.noAttach(false);
        this.E06 = new Effect(1760, 1.006f, 2.172f, -18.64f, 0.0f);
        this.E06.disp(true);
        this.E06.setClip(true);
        this.E06.noAttach(false);
        this.E07 = new Effect(1760, 2.812f, 2.172f, -18.64f, 0.0f);
        this.E07.disp(true);
        this.E07.setClip(true);
        this.E07.noAttach(false);
        this.E08 = new Effect(1760, 4.618f, 2.172f, -18.64f, 0.0f);
        this.E08.disp(true);
        this.E08.setClip(true);
        this.E08.noAttach(false);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 60;
        this.fade1.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.25f, 0.25f, 0.25f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.4f, 0.4f, 0.4f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 0, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 1, 0.2f, 0.2f, 0.2f);
        Runtime.setIdLightCol(1, 2, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightCol(1, 3, 0.35f, 0.35f, 0.35f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(2, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 1, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(2, 2, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightCol(2, 3, 0.45f, 0.45f, 0.45f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -2.0f);
        Runtime.setIdLightCol(3, 0, 0.3f, 0.3f, 0.3f);
        Runtime.setIdLightCol(3, 1, 0.3f, 0.35f, 0.35f);
        Runtime.setIdLightCol(3, 2, 0.5f, 0.55f, 0.55f);
        Runtime.setIdLightCol(3, 3, 0.5f, 0.55f, 0.55f);
        Runtime.setIdLightVec(3, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(3, 2, 0.0f, 1.0f, 2.0f);
        Runtime.setIdLightVec(3, 3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(2, 100.0f, 100.0f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 15.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFPedestal(6, -2.0f, 4.3f, 4.3f, 45.0f, -22.0f, -10.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFPedestalHokan(6, 1);
        if (Runtime.getFlags(3142, 1) == 0) {
            this.shi = new NPC_NORMAL(1, 11, 0, 3, 11, 100.0f, 100.0f, 100.0f, 0.0f);
            this.shi.setInvalidID(1);
            this.shi.setMotion(1, 9);
            this.zig = new NPC_NORMAL(6, 12, 0, 3, 9, 100.0f, 100.0f, 100.0f, 0.0f);
            this.zig.setInvalidID(1);
            this.zig.setMotion(1, 9);
            this.jun = new NPC_NORMAL(5, 13, 0, 3, 8, 100.0f, 100.0f, 100.0f, 0.0f);
            this.jun.setInvalidID(1);
            this.jun.setMotion(1, 9);
            this.cha = new NPC_NORMAL(3, 14, 0, 3, 10, 100.0f, 100.0f, 100.0f, 0.0f);
            this.cha.setInvalidID(1);
            this.cha.setMotion(1, 9);
        }
        this.enemy1 = new Enepc();
        this.enemy1.init(16396, 3, -9.16f, 0.0f, -4.81f, 90.0f);
        this.enemy1.id = 1;
        this.enemy1.setGroup(3, 3, 5, 5);
        float[] fArray = new float[28];
        fArray[0] = -9.16f;
        fArray[2] = -4.81f;
        fArray[3] = 1.0f;
        fArray[4] = -10.362f;
        fArray[6] = -4.421f;
        fArray[7] = 2.0f;
        fArray[8] = -11.529f;
        fArray[10] = -3.821f;
        fArray[11] = 3.0f;
        fArray[12] = -12.661f;
        fArray[14] = -3.112f;
        fArray[15] = 4.0f;
        fArray[16] = -13.97f;
        fArray[18] = -2.37f;
        fArray[19] = 5.0f;
        fArray[20] = -14.995f;
        fArray[22] = -1.981f;
        fArray[23] = 6.0f;
        fArray[24] = -16.41f;
        fArray[26] = -1.945f;
        fArray[27] = -1.0f;
        float[] fArray2 = fArray;
        this.enemy1.setParams(1, 1, 1, 3, fArray2);
        float[] fArray3 = new float[12];
        fArray3[0] = -9.16f;
        fArray3[2] = -4.81f;
        fArray3[3] = -10.362f;
        fArray3[5] = -4.421f;
        fArray3[6] = -11.529f;
        fArray3[8] = -3.821f;
        fArray3[9] = -12.661f;
        fArray3[11] = -3.112f;
        float[] fArray4 = fArray3;
        this.enemy1.setParams(fArray4);
        this.enemy1.kickEnepc(10, 85, 0);
        if (Runtime.getFlags(3185, 1) == 0) {
            this.enemy2 = new Enepc();
            this.enemy2.init(16403, 22, 0.0f, 0.0f, 0.0f, 270.0f);
            this.enemy2.id = 2;
            this.enemy2.setGroup(6, 6, 6, 6);
            float[] fArray5 = new float[28];
            fArray5[0] = -10.0f;
            fArray5[2] = -14.0f;
            fArray5[3] = -1.0f;
            fArray5[4] = -9.0f;
            fArray5[6] = -14.0f;
            fArray5[8] = -8.0f;
            fArray5[10] = -14.0f;
            fArray5[11] = 1.0f;
            fArray5[12] = -7.0f;
            fArray5[14] = -14.0f;
            fArray5[15] = 2.0f;
            fArray5[16] = -6.0f;
            fArray5[18] = -14.0f;
            fArray5[19] = 3.0f;
            fArray5[20] = -5.0f;
            fArray5[22] = -14.0f;
            fArray5[23] = 4.0f;
            fArray5[24] = -4.0f;
            fArray5[26] = -15.0f;
            fArray5[27] = 5.0f;
            float[] fArray6 = fArray5;
            this.enemy2.setParams(1, 0, 2, 22, fArray6);
        }
        if (Runtime.getFlags(3185, 1) == 0) {
            new Uwamono(0, 13, this.enemy2);
        } else {
            new Uwamono(0, 13);
        }
        this.doorA = new Uwamono(12, 40, '\u0001');
        new Uwamono(11, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(8, 40, '\u0001');
        new Uwamono(7, 40, '\u0001', this.doorB);
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(10, 40, '\u0001');
        new Uwamono(9, 40, '\u0001', this.doorC);
        this.doorC.SetDoorType('\u0004');
        new Uwamono(28674, -12.3f, 0.0f, -6.3f, 0.0f);
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

