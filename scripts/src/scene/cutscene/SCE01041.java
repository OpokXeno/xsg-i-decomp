import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.PlayControl;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.map.MC_ELS01_PRJ;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01041
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01041,
        MC_ELS01_PRJ,
        JNT_Human,
        JNT_Accesories,
        FLSmatehws,
        FLShammer,
        FLStonny,
        FLSkosmos_h {
    STool tool = new STool();
    Camera ManualCam = Camera.create(0);
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    FaceChr hammer = new FaceChr(278);
    FaceChr kosmos = new FaceChr(31);
    FaceChr tonny = new FaceChr(277);
    FaceChr mathews = new FaceChr(275);
    Chrs mobj064a = new Chrs(20545);
    Chrs mobj064b = new Chrs(20545);
    Chrs mobj064c = new Chrs(20545);
    Chrs mobj064d = new Chrs(20545);
    Chrs mobj064e = new Chrs(20545);
    Chrs mobj064f = new Chrs(20545);
    Chrs mobj064g = new Chrs(20545);
    Chrs mobj064h = new Chrs(20545);
    Chrs mobj064i = new Chrs(20545);
    Chrs mobj064j = new Chrs(20545);
    Chrs mobj065a = new Chrs(20546);
    Chrs mobj065b = new Chrs(20546);
    Chrs mobj065c = new Chrs(20546);
    Chrs mobj065d = new Chrs(20546);
    Chrs mobj065e = new Chrs(20546);
    Chrs mobj066a = new Chrs(20547);
    Chrs mobj066b = new Chrs(20547);
    Chrs mobj066c = new Chrs(20547);
    Chrs mobj066d = new Chrs(20547);
    Chrs mobj066e = new Chrs(20547);
    Chrs mobj067a = new Chrs(20548);
    Chrs mobj067b = new Chrs(20548);
    Chrs mobj067c = new Chrs(20548);
    Chrs mobj067d = new Chrs(20548);
    Chrs mobj067e = new Chrs(20548);
    Chrs mobj067f = new Chrs(20548);
    Chrs mobj021 = new Chrs(20502);
    Chrs mobj001 = new Chrs(20482);
    Chrs mobj075 = new Chrs(20556);
    Space space2;
    Thread space_auto = Thread.create(this, "space_camchase");
    PlayControl pc;
    Light light = new Light(0);
    Thread Cutchk_thread;
    float[] shadowFilter = new float[4];
    Monitor fm1;
    Monitor fm2;
    Monitor fm3;
    Monitor fm4;
    Monitor fm5;
    Monitor fm6;
    Monitor fm7;
    Monitor em1;
    Monitor em2;
    Monitor em3;
    Monitor em4;
    Monitor em5;
    Monitor em6;
    Monitor em7;
    Monitor tonny_fm;
    Monitor masyu_fm;
    Effect eft0;
    Effect eft1a;
    Effect eft1b;
    Effect eft1c;
    Effect eft1d;
    Effect eft1e;
    Effect eft1f;
    Effect eft1g;
    Effect eft1h;
    Effect eft1i;
    Effect eft1j;
    Effect eft1k;
    Effect eft1l;
    Effect eft1m;
    Effect eft1n;
    Effect eft1o;
    Effect eft1p;
    Effect eft1q;
    Effect eft1r;
    Effect eft2;
    Effect eft3;
    Effect eft_mobj075;
    Unit cigar;
    Thread eft_thread = Thread.create(this, "eft_srv");
    int NextPCStart;
    int TotalCutTime;
    int BaseCutTime;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01041() {
    }

    void Chr_init(boolean bl) {
        this.hammer.start(4, null);
        this.kosmos.start(4, null);
        this.tonny.start(4, null);
        this.mathews.start(4, null);
        this.mobj064a.start(4, null);
        this.mobj064b.start(4, null);
        this.mobj064c.start(4, null);
        this.mobj064d.start(4, null);
        this.mobj064e.start(4, null);
        this.mobj064f.start(4, null);
        this.mobj064g.start(4, null);
        this.mobj064h.start(4, null);
        this.mobj064i.start(4, null);
        this.mobj064j.start(4, null);
        this.mobj065a.start(4, null);
        this.mobj065b.start(4, null);
        this.mobj065c.start(4, null);
        this.mobj065d.start(4, null);
        this.mobj065e.start(4, null);
        this.mobj066a.start(4, null);
        this.mobj066b.start(4, null);
        this.mobj066c.start(4, null);
        this.mobj066d.start(4, null);
        this.mobj066e.start(4, null);
        this.mobj067a.start(4, null);
        this.mobj067b.start(4, null);
        this.mobj067c.start(4, null);
        this.mobj067d.start(4, null);
        this.mobj067e.start(4, null);
        this.mobj067f.start(4, null);
        this.mobj001.start(4, null);
        this.mobj021.start(4, null);
        this.mobj075.start(4, null);
        this.hammer.setVisible(bl);
        this.kosmos.setVisible(bl);
        this.tonny.setVisible(bl);
        this.kosmos.setVisible(bl);
        this.mathews.setVisible(bl);
        this.mobj064a.setVisible(bl);
        this.mobj064b.setVisible(bl);
        this.mobj064c.setVisible(bl);
        this.mobj064d.setVisible(bl);
        this.mobj064e.setVisible(bl);
        this.mobj064f.setVisible(bl);
        this.mobj064g.setVisible(bl);
        this.mobj064h.setVisible(bl);
        this.mobj064i.setVisible(bl);
        this.mobj064j.setVisible(bl);
        this.mobj065a.setVisible(bl);
        this.mobj065b.setVisible(bl);
        this.mobj065c.setVisible(bl);
        this.mobj065d.setVisible(bl);
        this.mobj065e.setVisible(bl);
        this.mobj066a.setVisible(bl);
        this.mobj066b.setVisible(bl);
        this.mobj066c.setVisible(bl);
        this.mobj066d.setVisible(bl);
        this.mobj066e.setVisible(bl);
        this.mobj067a.setVisible(bl);
        this.mobj067b.setVisible(bl);
        this.mobj067c.setVisible(bl);
        this.mobj067d.setVisible(bl);
        this.mobj067e.setVisible(bl);
        this.mobj067f.setVisible(bl);
        this.mobj001.setVisible(bl);
        this.mobj021.setVisible(bl);
        this.mobj075.setVisible(bl);
    }

    void CutWork() {
    }

    void Focus_init() {
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
    }

    void Map_init(boolean bl) {
        int n = 0;
        while (n != 100) {
            Stage.setVisible(n, bl);
            ++n;
        }
        Stage.setVisible(6, false);
        this.chair(false);
        this.monitor(false);
        this.space2.setVisible(true);
        if (bl) {
            this.fm1.setScale(0.3f, 0.3f, 0.3f);
            this.em2.setScale(0.34f, 0.34f, 0.34f);
            this.tonny_fm.setScale(0.16f, 0.16f, 0.16f);
        } else {
            this.fm1.setScale(0.0f, 0.0f, 0.0f);
            this.em2.setScale(0.0f, 0.0f, 0.0f);
            this.tonny_fm.setScale(0.0f, 0.0f, 0.0f);
        }
    }

    void MotionPack_srvA(int n) {
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime <= n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move2");
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, -0.962f, 0.0f, -0.273f);
                    this.light.setColor(2, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(2, -0.424f, 0.001f, -0.906f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    Runtime.setDefocusQuick(0, 1, 752880, 1);
                    Runtime.setDefocusQuick(1, 1, 744688, 1);
                    System.sleep(160);
                    this.tool.SoundstreamPlay(141001);
                    this.tool.MSG(60, "マシューズ", "What a mess...");
                    this.tool.SoundstreamPlay(141002);
                    this.tool._MSG(80, "マシューズ", "Whoever it was,\nthey sure went all out.");
                    break;
                }
                case 240: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move2");
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, -0.962f, 0.0f, -0.273f);
                    this.light.setColor(2, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(2, -0.424f, 0.001f, -0.906f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, -0.615f, -0.494f, -0.615f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(65);
                    this.tool.SoundstreamPlay(141003);
                    this.tool.MSG(70, "マシューズ", "You think there's anything\nleft out there?");
                    this.tool.SoundstreamPlay(141004);
                    this.tool.MSG(40, "マシューズ", "Hey, Tony.");
                    this.tool.SoundstreamPlay(141005);
                    this.tool.MSG(120, "マシューズ", "How long before the\nFederation gets here?");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srvB(int n) {
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime != n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.45f, 0.45f, 0.45f);
                    this.light.setDirection2(1, 0.508f, 0.56f, -0.655f);
                    this.light.setColor(2, 0.23f, 0.23f, 0.23f);
                    this.light.setDirection2(2, -0.668f, 0.603f, -0.437f);
                    this.light.setColor(3, 0.17f, 0.17f, 0.17f);
                    this.light.setDirection2(3, -0.039f, -0.713f, -0.7f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    Runtime.setDefocusQuick(0, 0, 0, 0);
                    Runtime.setDefocusQuick(1, 0, 0, 0);
                    Runtime.setDefocusQuick(2, 0, 0, 0);
                    Runtime.setDefocusQuick(3, 0, 0, 0);
                    this.eft2.disp(false);
                    this.eft3.disp(false);
                    this.cigar.setVisible(true);
                    System.sleep(1);
                    this.PCextends_start(1, 199, 0.7f);
                    this.tool.SoundstreamPlay(141006);
                    this.tool.MSG(49, this.tonny.face, 1, "Well, let's see...");
                    this.tool.SoundstreamPlay(141007);
                    this.tool.MSG(120, this.tonny.face, 1, "The last SOS went out\na half-an-hour ago, so...");
                    this.tool.MSG(100, this.tonny.face, 1, "We've probably got at least\nthree hours, right?");
                    this.PCextends_end(n);
                    break;
                }
                case 200: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.48f, 0.48f, 0.48f);
                    this.light.setDirection2(1, -0.265f, 0.223f, -0.938f);
                    this.light.setColor(2, 0.38f, 0.38f, 0.38f);
                    this.light.setDirection2(2, 0.573f, 0.556f, 0.602f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, 0.36f, -0.329f, -0.873f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.hammer.look_eye_speed(2.0f);
                    this.hammer.look_eye_set(9.3f, 1.5f);
                    this.tool.FACE(this.tonny.face, 1);
                    System.sleep(25);
                    this.tool.SoundstreamPlay(141008);
                    this.tool.MSG(60, this.tonny.face, 1, "No sign of any nearby ships, either.");
                    this.tool.SoundstreamPlay(141009);
                    this.tool.MSG(30, this.tonny.face, 1, "Right, Hammer?");
                    break;
                }
                case 319: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, -0.078f, -0.452f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(320, 528, 1.3f);
                    this.tool.SoundstreamPlay(141010);
                    this.tool.sMSG(29, this.hammer.face, 3, "Yep.");
                    this.tool.SoundstreamPlay(141011);
                    this.tool.MSG(110, this.hammer.face, 3, "We're the only ship within\na 5,000 light-year radius.");
                    System.sleep(20);
                    this.PCextends_end(n);
                    break;
                }
                case 529: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141012);
                    this.tool.MSG(30, this.mathews.face, 1, "All right!");
                    break;
                }
                case 604: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 619: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.56f, 0.56f, 0.56f);
                    this.light.setDirection2(1, 0.591f, 0.492f, -0.639f);
                    this.light.setColor(2, 0.17f, 0.17f, 0.17f);
                    this.light.setDirection2(2, -0.63f, 0.769f, -0.105f);
                    this.light.setColor(3, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(3, -0.025f, -0.736f, -0.676f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.cigar.setVisible(false);
                    this.eft2.disp(false);
                    this.eft3.setForceLoop(false);
                    this.eft3.disp(true);
                    this.mathews.setVisible(10, false);
                    this.mathews.setVisible(12, true);
                    this.tool.SoundstreamPlay(141013);
                    this.tool.MSG(50, this.mathews.face, 1, 40, "That should be enough.");
                    this.tool.SoundstreamPlay(141014);
                    this.tool.MSG(70, this.mathews.face, 1, 60, "Let's grab anything that\nlooks salvageable.");
                    break;
                }
                case 769: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.mathews.setVisible(10, true);
                    this.mathews.setVisible(12, false);
                    this.tool.SoundstreamPlay(141015);
                    this.tool.sMSG(40, this.hammer.face, 1, 30, "You know, Captain...");
                    this.tool.SoundstreamPlay(141016);
                    this.tool.MSG(100, this.hammer.face, 1, 90, "I realize the deadline for\npaying back Master Gaignun\nis right around the corner,");
                    this.tool.MSG(80, this.hammer.face, 1, 70, "but don't you think\nthis is a little risky?");
                    this.tool.SoundstreamPlay(141017);
                    this.tool.sMSG(70, this.hammer.face, 7, 65, "If the Feds find us out,\nwe'd be lucky to get 10 years.");
                    break;
                }
                case 1069: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.75f, 0.596f, -0.286f);
                    this.light.setColor(2, 0.17f, 0.17f, 0.17f);
                    this.light.setDirection2(2, -0.63f, 0.769f, -0.105f);
                    this.light.setColor(3, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(3, -0.025f, -0.736f, -0.676f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141018);
                    this.tool.MSG(70, this.mathews.face, 5, 60, "That's if they find out.");
                    this.tool.SoundstreamPlay(141019);
                    this.tool.MSG(130, this.mathews.face, 5, 120, "You know the Feds aren't gonna\npatrol all the way out here in the\nmiddle of nowhere.");
                    break;
                }
                case 1309: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.75f, 0.596f, -0.286f);
                    this.light.setColor(2, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(2, -0.63f, 0.769f, -0.105f);
                    this.light.setColor(3, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(3, -0.025f, -0.736f, -0.676f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141020);
                    this.tool.MSG(110, this.hammer.face, 7, 100, "Well...I just don't like the idea of\nfeeding off the dead.");
                    this.tool.SoundstreamPlay(141021);
                    this.tool.sMSG(120, this.hammer.face, 3, 110, "I mean, what are we, space jackals?\nVultures? Hyenas?");
                    break;
                }
                case 1579: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.37f, 0.37f, 0.37f);
                    this.light.setDirection2(1, 0.571f, 0.464f, 0.678f);
                    this.light.setColor(2, 0.47f, 0.47f, 0.47f);
                    this.light.setDirection2(2, 0.61f, 0.563f, -0.557f);
                    this.light.setColor(3, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(3, 0.702f, -0.708f, 0.077f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141022);
                    this.tool.MSG(79, this.mathews.face, 5, "Ya moron!\nWhat kinda metaphor is that?");
                    this.tool.SoundstreamPlay(141023);
                    this.tool.MSG(70, this.mathews.face, 5, "Don't be comparing us to\nextinct animals!");
                    this.tool.SoundstreamPlay(141024);
                    this.tool.MSG(29, this.mathews.face, 5, "You'll jinx us!");
                    break;
                }
                case 1759: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(1, 0.571f, 0.463f, 0.678f);
                    this.light.setColor(2, 0.47f, 0.47f, 0.47f);
                    this.light.setDirection2(2, 0.628f, 0.53f, -0.57f);
                    this.light.setColor(3, 0.2f, 0.2f, 0.2f);
                    this.light.setDirection2(3, 0.518f, -0.765f, -0.382f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tonny.look_speed(2.0f);
                    this.tonny.look_eye_set(-23.1f, 0.0f);
                    this.tool.SoundstreamPlay(141025);
                    this.tool.MSG(140, this.mathews.face, 5, 130, "We're recyclers, dammit.\nEnvironmentally-friendly\nspace recyclers.");
                    break;
                }
                case 1909: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, -0.843f, 0.001f, -0.538f);
                    this.light.setColor(2, 0.23f, 0.23f, 0.23f);
                    this.light.setDirection2(2, 0.076f, 0.472f, 0.878f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, -0.134f, -0.844f, 0.519f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(1910, 2148, 0.85f);
                    this.tool.SoundstreamPlay(141026);
                    this.tool.MSG(109, this.tonny.face, 5, "Master Gaignun told us to stay away\nfrom any \"side jobs,\" remember?");
                    this.tool.SoundstreamPlay(141027);
                    this.tool.MSG(70, this.tonny.face, 5, "He said it hurts the\nFoundation's image.");
                    this.tonny.look_speed(2.0f);
                    this.tonny.look_eye_set(0.0f, 0.0f);
                    this.tool.SoundstreamPlay(141028);
                    this.tool.MSG(90, this.tonny.face, 5, "I don't know...");
                    this.PCextends_end(n);
                    break;
                }
                case 2149: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.48f, 0.48f, 0.48f);
                    this.light.setDirection2(1, -0.341f, 0.144f, -0.929f);
                    this.light.setColor(2, 0.38f, 0.38f, 0.38f);
                    this.light.setDirection2(2, 0.726f, 0.679f, -0.108f);
                    this.light.setColor(3, 0.28f, 0.28f, 0.28f);
                    this.light.setDirection2(3, -0.328f, -0.688f, -0.647f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(2150, 2388, 1.6f);
                    this.tool.SoundstreamPlay(141029);
                    this.tool.MSG(79, this.tonny.face, 3, 70, "Oh-hoh!\nWe got a wrecked ship here,\nthree o'clock!");
                    this.tool.SoundstreamPlay(141030);
                    this.tool.MSG(60, this.tonny.face, 3, 50, "Looks like a Ganymede class...");
                    this.PCextends_end(n);
                    break;
                }
                case 2389: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.75f, 0.596f, -0.286f);
                    this.light.setColor(2, 0.17f, 0.17f, 0.17f);
                    this.light.setDirection2(2, -0.63f, 0.769f, -0.105f);
                    this.light.setColor(3, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(3, -0.025f, -0.736f, -0.676f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(2390, 2583, 1.2f);
                    this.tool.SoundstreamPlay(141031);
                    this.tool.MSG(49, this.mathews.face, 3, "Nice work there, Tony.");
                    this.tool.SoundstreamPlay(141032);
                    this.tool.MSG(110, this.mathews.face, 3, "Good thing we were tapping the\nU.M.N. emergency channel...");
                    this.PCextends_end(n);
                    break;
                }
                case 2584: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.light.setDirection2(1, 0.338f, 0.189f, -0.922f);
                    this.light.setColor(2, 0.37f, 0.37f, 0.37f);
                    this.light.setDirection2(2, -0.72f, 0.609f, 0.332f);
                    this.light.setColor(3, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(3, -0.362f, -0.785f, -0.503f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(2585, 2799, 1.5f);
                    this.tool.SoundstreamPlay(141033);
                    this.tool.MSG(49, this.mathews.face, 3, 40, "Come on, pull in close.\nHurry!");
                    this.tool.SoundstreamPlay(141034);
                    this.tool.sMSG(50, this.hammer.face, 1, 40, "Yep, we're vultures...");
                    this.tool.SoundstreamPlay(141035);
                    this.tool.MSG(40, this.mathews.face, 5, 30, "What was that?");
                    this.PCextends_end(n);
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srvC(int n) {
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime != n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, -0.169f, 0.0f, 0.986f);
                    this.light.setColor(2, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(2, -0.528f, 0.834f, -0.161f);
                    this.light.setColor(3, 0.13f, 0.13f, 0.13f);
                    this.light.setDirection2(3, -0.643f, -0.469f, -0.605f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    Runtime.setDefocusQuick(0, 1, 752880, 1);
                    Runtime.setDefocusQuick(1, 1, 744688, 1);
                    this.tonny.look_eye_set(0.0f, 0.0f);
                    this.hammer.look_eye_set(0.0f, 0.0f);
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srvD1(int n) {
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime != n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(530);
                    this.kosmos.renderCommand(530);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.35f, 0.35f, 0.35f);
                    this.light.setDirection2(1, -0.738f, 0.339f, 0.584f);
                    this.light.setColor(2, 0.61f, 0.61f, 0.61f);
                    this.light.setDirection2(2, 0.643f, 0.764f, 0.055f);
                    this.light.setColor(3, 0.2f, 0.2f, 0.2f);
                    this.light.setDirection2(3, -0.199f, -0.762f, 0.616f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 91: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(512);
                    this.kosmos.renderCommand(512);
                    this.hammer.setVisible(12, false);
                    this.mathews.setVisible(9, false);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.723f, 0.189f, -0.664f);
                    this.light.setColor(2, 0.29f, 0.29f, 0.29f);
                    this.light.setDirection2(2, -0.781f, 0.608f, 0.143f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.43f, -0.859f, -0.276f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(92, 299, 1.1f);
                    this.hammer.look_eye_speed(2.0f);
                    this.hammer.look_eye_set(-4.2f, 2.7f);
                    this.tool.SoundstreamPlay(141036);
                    this.tool.sMSG(49, this.hammer.face, 3, "Spectrum matches up perfectly.");
                    this.hammer.look_eye_set(4.8f, 2.7f);
                    this.tool.SoundstreamPlay(141037);
                    this.tool.MSG(60, this.hammer.face, 3, "It's a geocrystal.");
                    this.tool.SoundstreamPlay(141039);
                    this.tool.sMSG(30, this.mathews.face, 3, "All right!");
                    this.tool.SoundstreamPlay(141040);
                    this.tool.MSG(40, this.mathews.face, 3, "Not bad.");
                    this.PCextends_end(n);
                    break;
                }
                case 300: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(512);
                    this.hammer.setVisible(12, true);
                    this.mathews.setVisible(9, true);
                    this.hammer.look_eye_speed(2.0f);
                    this.hammer.look_eye_set(0.0f, 0.0f);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.light.setDirection2(1, 0.338f, 0.189f, -0.922f);
                    this.light.setColor(2, 0.49f, 0.49f, 0.49f);
                    this.light.setDirection2(2, -0.934f, 0.0f, 0.356f);
                    this.light.setColor(3, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(3, -0.362f, -0.785f, -0.503f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141041);
                    this.tool.MSG(60, this.mathews.face, 3, 50, "Let's start with that one!");
                    break;
                }
                case 420: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.58f, 0.58f, 0.58f);
                    this.light.setDirection2(1, -0.347f, 0.194f, -0.917f);
                    this.light.setColor(2, 0.38f, 0.38f, 0.38f);
                    this.light.setDirection2(2, 0.573f, 0.556f, 0.602f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, 0.36f, -0.329f, -0.873f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 480: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.07f, 0.07f, 0.07f);
                    this.light.setColor(1, 0.62f, 0.62f, 0.62f);
                    this.light.setDirection2(1, 0.994f, 0.0f, 0.112f);
                    this.light.setColor(2, 0.21f, 0.21f, 0.21f);
                    this.light.setDirection2(2, 0.137f, 0.0f, 0.991f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, 0.36f, -0.329f, -0.873f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 540: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.05f, 0.05f, 0.05f);
                    this.light.setColor(1, 0.62f, 0.62f, 0.62f);
                    this.light.setDirection2(1, 0.994f, 0.0f, 0.112f);
                    this.light.setColor(2, 0.21f, 0.21f, 0.21f);
                    this.light.setDirection2(2, -0.394f, 0.014f, 0.919f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, 0.36f, -0.329f, -0.873f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 570: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.52f, 0.52f, 0.52f);
                    this.light.setDirection2(1, -0.17f, 0.0f, -0.985f);
                    this.light.setColor(2, 0.24f, 0.24f, 0.24f);
                    this.light.setDirection2(2, 0.618f, 0.786f, 0.027f);
                    this.light.setColor(3, 0.23f, 0.23f, 0.23f);
                    this.light.setDirection2(3, 0.049f, -0.922f, -0.384f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141042);
                    this.tool.MSG(14, this.tonny.face, 5, "　");
                    break;
                }
                case 585: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.light.setDirection2(1, -0.412f, 0.28f, -0.867f);
                    this.light.setColor(2, 0.38f, 0.38f, 0.38f);
                    this.light.setDirection2(2, -0.325f, 0.552f, 0.768f);
                    this.light.setColor(3, 0.2f, 0.2f, 0.2f);
                    this.light.setDirection2(3, -0.196f, -0.976f, 0.091f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(586, 674, 0.65f);
                    this.tonny.look_speed(10.0f);
                    this.tonny.look_eye_set(-23.1f, 0.0f);
                    System.sleep(30);
                    this.tool.SoundstreamPlay(141043);
                    this.tool.MSG(59, this.tonny.face, 5, "That's not a geocrystal,\nyou idiot!");
                    this.tool.SoundstreamPlay(141044);
                    this.tool._MSG(60, "トニー", "It's a corpse!");
                    this.tool.sFACE(40, this.tonny.face, 5);
                    this.tool.FACE(this.hammer.face, 7, 98);
                    this.PCextends_end(n);
                    break;
                }
                case 675: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.62f, 0.62f, 0.62f);
                    this.light.setDirection2(1, 0.759f, 0.21f, -0.616f);
                    this.light.setColor(2, 0.29f, 0.29f, 0.29f);
                    this.light.setDirection2(2, -0.781f, 0.608f, 0.143f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.43f, -0.86f, -0.276f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(676, 990, 1.15f);
                    this.tonny.look_speed(10.0f);
                    this.tonny.look_eye_set(0.0f, 0.0f);
                    System.sleep(10);
                    this.tool.SoundstreamPlay(141045);
                    this.tool.sMSG(99, this.mathews.face, 9, "Well...what'd you expect?\nThis is a battlefield.");
                    this.tool.SoundstreamPlay(141046);
                    this.tool.MSG(50, this.mathews.face, 9, "It's no big deal.");
                    this.tool.SoundstreamPlay(141047);
                    this.tool.MSG(60, this.mathews.face, 9, "Don't bother wasting fuel.");
                    this.tool.SoundstreamPlay(141048);
                    this.tool._MSG(60, "トニー", "Just keep going and\nlet it bounce off.");
                    this.tool.sFACE(50, this.mathews.face, 5);
                    this.PCextends_end(n);
                    break;
                }
                case 991: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.52f, 0.52f, 0.52f);
                    this.light.setDirection2(1, 0.389f, 0.242f, 0.889f);
                    this.light.setColor(2, 0.29f, 0.29f, 0.29f);
                    this.light.setDirection2(2, -0.94f, 0.001f, -0.341f);
                    this.light.setColor(3, 0.18f, 0.18f, 0.18f);
                    this.light.setDirection2(3, 0.081f, -0.812f, 0.578f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.24f, 0.24f, 0.24f);
                    this.kosmos.light.setColor(1, 0.71f, 0.71f, 0.71f);
                    this.kosmos.light.setDirection2(1, 0.723f, 0.191f, -0.664f);
                    this.kosmos.light.setColor(2, 0.52f, 0.52f, 0.52f);
                    this.kosmos.light.setDirection2(2, -0.781f, 0.608f, 0.143f);
                    this.kosmos.light.setColor(3, 0.33f, 0.33f, 0.33f);
                    this.kosmos.light.setDirection2(3, -0.43f, -0.86f, -0.276f);
                    this.tool.SoundstreamPlay(141049);
                    this.tool.sMSG(50, this.tonny.face, 3, "You're kidding, right?!");
                    this.tool.MSG(120, this.tonny.face, 3, "My dead grandma told me to\nbe respectful of the dead.");
                    this.tool.MSG(70, this.tonny.face, 3, "Sorry, but I don't want\nany bad karma.");
                    this.tool._MSG(50, "トニー", "You do it, Captain.");
                    this.tool.sFACE(25, this.tonny.face, 5);
                    break;
                }
                case 1260: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.723f, 0.189f, -0.664f);
                    this.light.setColor(2, 0.29f, 0.29f, 0.29f);
                    this.light.setDirection2(2, -0.781f, 0.608f, 0.143f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.43f, -0.859f, -0.276f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141053);
                    this.tool.MSG(40, this.mathews.face, 5, 30, "Ya moron...");
                    this.tool.SoundstreamPlay(141054);
                    this.tool.MSG(30, this.mathews.face, 5, 20, "What,");
                    this.tool.SoundstreamPlay(141055);
                    this.tool.MSG(80, this.mathews.face, 5, 70, "is that your grandfather out there?");
                    break;
                }
                case 1420: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.723f, 0.189f, -0.664f);
                    this.light.setColor(2, 0.29f, 0.29f, 0.29f);
                    this.light.setDirection2(2, -0.781f, 0.608f, 0.143f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.43f, -0.859f, -0.276f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141056);
                    this.tool.MSG(40, this.mathews.face, 5, 30, "Don't be ridiculous!");
                    this.tool.SoundstreamPlay(141057);
                    this.tool.MSG(80, this.mathews.face, 5, 70, "Just shut up\nand keep moving.");
                    break;
                }
                case 1560: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.tonny.hairStop(0, 1);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.61f, 0.61f, 0.61f);
                    this.light.setDirection2(1, 0.846f, 0.524f, -0.096f);
                    this.light.setColor(2, 0.18f, 0.18f, 0.18f);
                    this.light.setDirection2(2, -0.261f, 0.0f, -0.965f);
                    this.light.setColor(3, 0.39f, 0.39f, 0.39f);
                    this.light.setDirection2(3, 0.227f, -0.957f, -0.182f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.kosmos.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.kosmos.light.setDirection2(1, 0.096f, 0.0f, -0.995f);
                    this.kosmos.light.setColor(2, 0.35f, 0.35f, 0.35f);
                    this.kosmos.light.setDirection2(2, -0.431f, 0.477f, 0.766f);
                    this.kosmos.light.setColor(3, 0.22f, 0.22f, 0.22f);
                    this.kosmos.light.setDirection2(3, -0.605f, -0.786f, -0.128f);
                    this.tonny.setLightMode(1);
                    this.tonny.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.tonny.light.setColor(1, 0.52f, 0.52f, 0.52f);
                    this.tonny.light.setDirection2(1, 0.613f, 0.199f, 0.765f);
                    this.tonny.light.setColor(2, 0.33f, 0.33f, 0.33f);
                    this.tonny.light.setDirection2(2, -0.899f, 0.001f, -0.439f);
                    this.tonny.light.setColor(3, 0.18f, 0.18f, 0.18f);
                    this.tonny.light.setDirection2(3, 0.242f, -0.834f, 0.496f);
                    this.tool.SoundstreamPlay(141058);
                    this.tool.MSG(50, this.tonny.face, 7, "Man, not again!");
                    this.tool.SoundstreamPlay(141059);
                    this.tool.MSG(50, this.tonny.face, 7, "Captain, this is so typical of you.");
                    this.tool.SoundstreamPlay(141060);
                    this.tool.sMSG(80, this.tonny.face, 5, "Making us do all the dirty work!");
                    break;
                }
                case 1920: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.tonny.setLightMode(0);
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.1f, 0.1f, 0.1f);
                    this.light.setColor(1, 0.47f, 0.47f, 0.47f);
                    this.light.setDirection2(1, 0.712f, -0.643f, -0.283f);
                    this.light.setColor(2, 0.0f, 0.05f, 0.05f);
                    this.light.setDirection2(2, -0.122f, 0.992f, -0.023f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, 0.114f, -0.991f, -0.068f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141061);
                    this.tool.MSG(40, this.mathews.face, 5, "Ya moron!");
                    this.tool.SoundstreamPlay(141062);
                    this.tool.MSG(50, this.mathews.face, 5, "What the hell are you doing?");
                    this.tool.SoundstreamPlay(141063);
                    this.tool.MSG(60, this.mathews.face, 5, "I said bounce it off!");
                    this.tool.SoundstreamPlay(141064);
                    this.tool.MSG(80, this.tonny.face, 5, "Well, don't look at me!\nIt got stuck on its own.");
                    this.tool.SoundstreamPlay(141065);
                    this.tool.MSG(40, this.tonny.face, 5, "It's not my fault!");
                    this.tool.SoundstreamPlay(141066);
                    this.tool.MSG(58, this.mathews.face, 5, "Whatever, just get rid of it.");
                    break;
                }
                case 2250: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.hairStop(0, 1);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.59f, 0.59f, 0.59f);
                    this.light.setDirection2(1, 0.134f, 0.544f, -0.828f);
                    this.light.setColor(2, 0.25f, 0.25f, 0.25f);
                    this.light.setDirection2(2, -0.424f, 0.025f, 0.905f);
                    this.light.setColor(3, 0.2f, 0.2f, 0.2f);
                    this.light.setDirection2(3, -0.808f, 0.378f, -0.452f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141067);
                    this.tool.sMSG(90, this.hammer.face, 3, "Heeeey, it's a girl...");
                    this.tool.SoundstreamPlay(141068);
                    this.tool.MSG(59, this.hammer.face, 3, "She might be cute, you know.\nHeh, heh...");
                    break;
                }
                case 2400: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.649f, 0.522f, 0.554f);
                    this.light.setColor(2, 0.42f, 0.42f, 0.42f);
                    this.light.setDirection2(2, -0.865f, 0.333f, -0.374f);
                    this.light.setColor(3, 0.18f, 0.18f, 0.18f);
                    this.light.setDirection2(3, -0.415f, -0.292f, 0.862f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(2401, 2639, 1.5f);
                    this.tool.SoundstreamPlay(141070);
                    this.tool.MSG(89, this.mathews.face, 5, "Heh, heh my ass,\nyou sicko!");
                    this.tool.SoundstreamPlay(141071);
                    this.tool.MSG(70, this.mathews.face, 5, "Who the hell cares how she looks?\nShe's dead!");
                    this.PCextends_end(n);
                    break;
                }
                case 2640: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.tonny.hairStop(0, 1);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.58f, 0.58f, 0.58f);
                    this.light.setDirection2(1, -0.708f, 0.001f, -0.706f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, 0.105f, 0.307f, 0.946f);
                    this.light.setColor(3, 0.17f, 0.17f, 0.17f);
                    this.light.setDirection2(3, -0.244f, -0.731f, 0.637f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141072);
                    this.tool.MSG(30, this.tonny.face, 3, "Exactly.");
                    this.tool.SoundstreamPlay(141073);
                    this.tool.MSG(50, this.tonny.face, 3, "Besides, she's facing the other way.");
                    this.tool.SoundstreamPlay(141074);
                    this.tool.MSG(70, this.tonny.face, 3, "It's a shame we can't meet\nface to face...");
                    break;
                }
                case 2805: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.1f, 0.1f, 0.1f);
                    this.light.setColor(1, 0.42f, 0.42f, 0.42f);
                    this.light.setDirection2(1, 0.435f, -0.885f, -0.165f);
                    this.light.setColor(2, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(2, -0.122f, 0.992f, -0.023f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, 0.114f, -0.991f, -0.068f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 2870: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.05f, 0.05f, 0.05f);
                    this.light.setColor(1, 0.48f, 0.48f, 0.48f);
                    this.light.setDirection2(1, 0.435f, -0.885f, -0.165f);
                    this.light.setColor(2, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(2, -0.122f, 0.992f, -0.023f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, 0.114f, -0.991f, -0.068f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.05f, 0.05f, 0.05f);
                    this.kosmos.light.setColor(1, 0.48f, 0.48f, 0.48f);
                    this.kosmos.light.setDirection2(1, 0.435f, -0.885f, -0.165f);
                    this.kosmos.light.setColor(2, 0.0f, 0.0f, 0.0f);
                    this.kosmos.light.setDirection2(2, -0.122f, 0.992f, -0.023f);
                    this.kosmos.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.kosmos.light.setDirection2(3, 0.114f, -0.991f, -0.068f);
                    this.kosmos.look_eye_speed(10.0f);
                    this.kosmos.look_eye_set(7.2f, 6.0f);
                    break;
                }
                case 2940: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(530);
                    this.kosmos.renderCommand(530);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.light.setDirection2(1, 0.096f, 0.028f, -0.995f);
                    this.light.setColor(2, 0.47f, 0.47f, 0.47f);
                    this.light.setDirection2(2, -0.431f, 0.477f, 0.766f);
                    this.light.setColor(3, 0.22f, 0.22f, 0.22f);
                    this.light.setDirection2(3, -0.605f, -0.786f, -0.128f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.light.setColor(0, 0.05f, 0.05f, 0.05f);
                    this.kosmos.light.setColor(1, 0.55f, 0.55f, 0.55f);
                    this.kosmos.light.setDirection2(1, 0.132f, -0.577f, -0.806f);
                    this.kosmos.light.setColor(2, 0.44f, 0.44f, 0.44f);
                    this.kosmos.light.setDirection2(2, -0.025f, -0.926f, 0.377f);
                    this.kosmos.light.setColor(3, 0.33f, 0.33f, 0.33f);
                    this.kosmos.light.setDirection2(3, 0.159f, -0.141f, -0.977f);
                    this.kosmos.look_eye_set(0.0f, 0.0f);
                    this.tool.MSG(60, "ハマー&トニー", "　");
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srvD2(int n) {
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime != n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(512);
                    this.kosmos.renderCommand(512);
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.46f, 0.46f, 0.46f);
                    this.light.setDirection2(1, 0.66f, -0.743f, 0.112f);
                    this.light.setColor(2, 0.22f, 0.22f, 0.22f);
                    this.light.setDirection2(2, 0.09f, -0.996f, 0.02f);
                    this.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(3, -0.708f, -0.001f, -0.707f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141078);
                    this.tool.MSG(70, this.kosmos.face, 1, "Please open your\ncommunications line.");
                    this.tool.SoundstreamPlay(141079);
                    this.tool.MSG(50, this.kosmos.face, 1, "I need to speak with you.");
                    this.tool.SoundstreamPlay(141080);
                    this.tool.MSG(50, this.kosmos.face, 1, "Frequency, 2020.");
                    break;
                }
                case 179: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(1, 0.155f, 0.477f, -0.865f);
                    this.light.setColor(2, 0.23f, 0.23f, 0.23f);
                    this.light.setDirection2(2, -0.59f, 0.542f, 0.598f);
                    this.light.setColor(3, 0.23f, 0.23f, 0.23f);
                    this.light.setDirection2(3, -0.83f, -0.527f, -0.182f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(180, 388, 0.9f);
                    this.tool.SoundstreamPlay(141081);
                    this.tool.MSG(89, this.hammer.face, 5, 80, "A talking corpse!");
                    this.tool.SoundstreamPlay(141083);
                    this.tool.MSG(50, this.mathews.face, 5, 40, "Ya moron!");
                    this.tool.SoundstreamPlay(141084);
                    this.tool.MSG(30, this.mathews.face, 5, 20, "Open your eyes!");
                    this.tool.SoundstreamPlay(141085);
                    this.tool.MSG(50, this.mathews.face, 5, 40, "That ain't no corpse!");
                    this.PCextends_end(n);
                    break;
                }
                case 389: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(530);
                    this.kosmos.renderCommand(530);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.27f, 0.27f, 0.27f);
                    this.light.setDirection2(1, 0.187f, 0.0f, -0.982f);
                    this.light.setColor(2, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(2, 0.852f, 0.0f, 0.524f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, -0.115f, -0.834f, -0.539f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141082);
                    this.tool.MSG(80, this.tonny.face, 7, "This ain't happening,\nthis ain't happening...");
                    this.tool.SoundstreamPlay(141086);
                    this.tool.sMSG(22, this.tonny.face, 1, "　");
                    break;
                }
                case 494: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(512);
                    this.kosmos.renderCommand(512);
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.46f, 0.46f, 0.46f);
                    this.light.setDirection2(1, 0.66f, -0.743f, 0.112f);
                    this.light.setColor(2, 0.22f, 0.22f, 0.22f);
                    this.light.setDirection2(2, 0.09f, -0.996f, 0.02f);
                    this.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(3, -0.708f, -0.001f, -0.707f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(495, 598, 0.9f);
                    this.tool.SoundstreamPlay(141087);
                    this.tool.MSG(69, this.mathews.face, 1, "It's a...Realian,\nor a cyborg...");
                    this.tool.SoundstreamPlay(141088);
                    this.tool.MSG(40, this.mathews.face, 1, "Something like that.");
                    this.PCextends_end(n);
                    break;
                }
                case 599: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.61f, 0.61f, 0.61f);
                    this.light.setDirection2(1, -0.051f, 0.311f, -0.949f);
                    this.light.setColor(2, 0.38f, 0.38f, 0.38f);
                    this.light.setDirection2(2, -0.58f, 0.549f, 0.602f);
                    this.light.setColor(3, 0.17f, 0.17f, 0.17f);
                    this.light.setDirection2(3, -0.721f, -0.638f, -0.271f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(600, 898, 0.9f);
                    this.tool.SoundstreamPlay(141089);
                    this.tool.MSG(134, this.hammer.face, 7, "B-but I've never heard of a Realian\nthat can operate out in space?!");
                    this.tool.SoundstreamPlay(141090);
                    this.tool.MSG(110, this.mathews.face, 5, "Well...it's probably a military robot\nor something.");
                    this.tool.SoundstreamPlay(141091);
                    this.tool.MSG(50, this.mathews.face, 5, "Just open the channel.");
                    System.sleep(20);
                    this.PCextends_end(n);
                    break;
                }
                case 899: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.66f, 0.66f, 0.66f);
                    this.light.setDirection2(1, 0.329f, 0.025f, -0.944f);
                    this.light.setColor(2, 0.31f, 0.31f, 0.31f);
                    this.light.setDirection2(2, -0.625f, 0.316f, 0.714f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, -0.721f, -0.638f, -0.271f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(50);
                    this.tool.SoundstreamPlay(141092);
                    this.tool.sMSG(130, this.mathews.face, 1, "Ahem...I am Captain Matthews\nof the tramp freighter Elsa.");
                    this.tool.MSG(80, this.mathews.face, 1, "We received your SOS signal earlier.");
                    System.sleep(15);
                    break;
                }
                case 1198: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141093);
                    this.tool.MSG(190, this.mathews.face, 1, "It happened to be within range of our\nnavigational path, so we rushed here\nto your rescue...");
                    this.hammer.look_eye_speed(2.0f);
                    this.hammer.look_eye_set(12.0f, 0.0f);
                    this.tool.SoundstreamPlay(141094);
                    this.tool.sMSG(50, this.hammer.face, 1, "Huh? Rescue?!");
                    break;
                }
                case 1513: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.66f, 0.66f, 0.66f);
                    this.light.setDirection2(1, 0.329f, 0.025f, -0.944f);
                    this.light.setColor(2, 0.42f, 0.42f, 0.42f);
                    this.light.setDirection2(2, -0.625f, 0.316f, 0.714f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, -0.721f, -0.638f, -0.271f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(1514, 1662, 0.9f);
                    this.tool.SoundstreamPlay(141096);
                    this.tool.MSG(69, this.mathews.face, 3, " ");
                    this.tool.MSG(90, this.mathews.face, 3, "So...is there anything we can do\nto assist you?");
                    this.PCextends_end(n);
                    break;
                }
                case 1663: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.57f, 0.57f, 0.57f);
                    this.light.setDirection2(1, 0.793f, 0.277f, 0.543f);
                    this.light.setColor(2, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(2, 0.614f, 0.553f, -0.563f);
                    this.light.setColor(3, 0.19f, 0.19f, 0.19f);
                    this.light.setDirection2(3, 0.325f, -0.793f, -0.515f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.kosmos.light.setColor(1, 0.46f, 0.46f, 0.46f);
                    this.kosmos.light.setDirection2(1, 0.66f, -0.743f, 0.112f);
                    this.kosmos.light.setColor(2, 0.22f, 0.22f, 0.22f);
                    this.kosmos.light.setDirection2(2, 0.09f, -0.996f, 0.02f);
                    this.kosmos.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.kosmos.light.setDirection2(3, -0.708f, -0.001f, -0.707f);
                    this.hammer.look_eye_speed(2.0f);
                    this.hammer.look_eye_set(12.0f, 0.0f);
                    this.tool.SoundstreamPlay(141098);
                    this.tool.MSG(50, this.hammer.face, 1, 40, "What do you mean, rescue...?");
                    this.tool.SoundstreamPlay(141099);
                    this.tool.MSG(60, this.hammer.face, 1, 50, "What about our side job?");
                    this.tool.SoundstreamPlay(141100);
                    this.tool.MSG(40, this.mathews.face, 5, 30, "Relax, ya moron...");
                    this.tool.SoundstreamPlay(141101);
                    this.tool.MSG(110, this.mathews.face, 5, "We're gonna grab what we can while\ntaking care of this little rescue.");
                    this.tool.SoundstreamPlay(141102);
                    this.tool.MSG(40, this.mathews.face, 5, 30, "With this mess,");
                    break;
                }
                case 1978: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.look_eye_speed(2.0f);
                    this.hammer.look_eye_set(0.0f, 0.0f);
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.46f, 0.46f, 0.46f);
                    this.light.setDirection2(1, 0.66f, -0.743f, 0.112f);
                    this.light.setColor(2, 0.22f, 0.22f, 0.22f);
                    this.light.setDirection2(2, 0.09f, -0.996f, 0.02f);
                    this.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(3, -0.708f, -0.001f, -0.707f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(1979, 2127, 0.6f);
                    this.tool.SoundstreamPlay(141103);
                    this.tool.MSG(69, this.mathews.face, 5, "I bet there's no one\nleft alive, anyway.");
                    this.tool.SoundstreamPlay(141104);
                    this.tool.MSG(50, this.kosmos.face, 1, "I will make this brief.");
                    this.tool.SoundstreamPlay(141105);
                    this.tool.MSG(110, this.kosmos.face, 1, "I request that you gate jump\nto Second Miltia immediately.");
                    this.PCextends_end(n);
                    break;
                }
                case 2128: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.58f, 0.58f, 0.58f);
                    this.light.setDirection2(1, 0.73f, 0.512f, -0.453f);
                    this.light.setColor(2, 0.25f, 0.25f, 0.25f);
                    this.light.setDirection2(2, 0.322f, 0.412f, 0.852f);
                    this.light.setColor(3, 0.03f, 0.03f, 0.03f);
                    this.light.setDirection2(3, 0.792f, -0.571f, -0.218f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.kosmos.light.setColor(1, 0.46f, 0.46f, 0.46f);
                    this.kosmos.light.setDirection2(1, 0.66f, -0.743f, 0.112f);
                    this.kosmos.light.setColor(2, 0.22f, 0.22f, 0.22f);
                    this.kosmos.light.setDirection2(2, 0.09f, -0.996f, 0.02f);
                    this.kosmos.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.kosmos.light.setDirection2(3, -0.707f, -0.033f, -0.706f);
                    System.sleep(1);
                    this.PCextends_start(2129, 2239, 0.5f);
                    this.tool.SoundstreamPlay(141106);
                    this.tool.MSG(29, this.mathews.face, 5, " ");
                    this.tool.SoundstreamPlay(141107);
                    this.tool.MSG(30, this.mathews.face, 5, "Say what?!");
                    this.tool.SoundstreamPlay(141108);
                    this.tool.MSG(70, this.mathews.face, 5, "Oh, yeah,");
                    this.tool.SoundstreamPlay(141109);
                    this.tool.MSG(70, this.mathews.face, 5, "keep dreaming, sweetheart!");
                    this.PCextends_end(n);
                    break;
                }
                case 2240: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.58f, 0.58f, 0.58f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(2241, 2292, 0.8f);
                    this.tool.SoundstreamPlay(141110);
                    this.tool.MSG(59, this.mathews.face, 5, "We can't leave yet,");
                    this.PCextends_end(n);
                    break;
                }
                case 2293: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.58f, 0.58f, 0.58f);
                    this.light.setDirection2(1, 0.73f, 0.512f, -0.453f);
                    this.light.setColor(2, 0.25f, 0.25f, 0.25f);
                    this.light.setDirection2(2, 0.322f, 0.412f, 0.852f);
                    this.light.setColor(3, 0.03f, 0.03f, 0.03f);
                    this.light.setDirection2(3, 0.792f, -0.571f, -0.218f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.kosmos.light.setColor(1, 0.46f, 0.46f, 0.46f);
                    this.kosmos.light.setDirection2(1, 0.66f, -0.743f, 0.112f);
                    this.kosmos.light.setColor(2, 0.22f, 0.22f, 0.22f);
                    this.kosmos.light.setDirection2(2, 0.09f, -0.996f, 0.02f);
                    this.kosmos.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.kosmos.light.setDirection2(3, -0.707f, -0.033f, -0.706f);
                    System.sleep(1);
                    this.PCextends_start(2294, 2397, 0.45f);
                    this.tool.SoundstreamPlay(141111);
                    this.tool.MSG(59, this.mathews.face, 3, "we still got work to do.");
                    this.tool.SoundstreamPlay(141112);
                    this.tool.MSG(60, this.mathews.face, 3, "And besides...\nSecond Miltia?");
                    this.tool.sMSG(100, this.mathews.face, 5, "Have you any idea how much it would\ncost to travel that far?");
                    this.PCextends_end(n);
                    break;
                }
                case 2398: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.46f, 0.46f, 0.46f);
                    this.light.setDirection2(1, 0.66f, -0.743f, 0.112f);
                    this.light.setColor(2, 0.22f, 0.22f, 0.22f);
                    this.light.setDirection2(2, 0.09f, -0.996f, 0.02f);
                    this.light.setColor(3, 0.6f, 0.6f, 0.6f);
                    this.light.setDirection2(3, -0.707f, -0.033f, -0.706f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(2399, 2517, 0.7f);
                    this.tool.SoundstreamPlay(141113);
                    this.tool.MSG(39, this.kosmos.face, 1, "There is no need for concern.");
                    this.tool.SoundstreamPlay(141114);
                    this.tool.MSG(120, this.kosmos.face, 1, "We will cover all U.M.N. gate\nfees incurred.");
                    this.PCextends_end(n);
                    break;
                }
                case 2518: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(530);
                    this.kosmos.renderCommand(530);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.light.setDirection2(1, 0.096f, 0.0f, -0.995f);
                    this.light.setColor(2, 0.38f, 0.38f, 0.38f);
                    this.light.setDirection2(2, -0.431f, 0.477f, 0.766f);
                    this.light.setColor(3, 0.22f, 0.22f, 0.22f);
                    this.light.setDirection2(3, -0.605f, -0.786f, -0.128f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.05f, 0.05f, 0.05f);
                    this.kosmos.light.setColor(1, 0.55f, 0.55f, 0.55f);
                    this.kosmos.light.setDirection2(1, 0.148f, 0.17f, -0.974f);
                    this.kosmos.light.setColor(2, 0.24f, 0.24f, 0.24f);
                    this.kosmos.light.setDirection2(2, 0.048f, -0.706f, 0.706f);
                    this.kosmos.light.setColor(3, 0.09f, 0.09f, 0.09f);
                    this.kosmos.light.setDirection2(3, 0.996f, 0.0f, -0.086f);
                    System.sleep(1);
                    this.PCextends_start(2519, 2697, 0.6f);
                    this.tool.SoundstreamPlay(141115);
                    this.tool.MSG(79, this.mathews.face, 1, "You expect us to believe that?");
                    this.tool.SoundstreamPlay(141116);
                    this.tool.MSG(160, this.mathews.face, 1, "Even if that were true,\nwe've got other business\nto attend to right now.");
                    this.tool.SoundstreamPlay(141117);
                    this.tool.sMSG(70, this.mathews.face, 5, "We don't have that kind of time.");
                    this.PCextends_end(n);
                    break;
                }
                case 2698: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(512);
                    this.kosmos.renderCommand(512);
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.47f, 0.49f, 0.49f);
                    this.light.setDirection2(1, 0.44f, -0.732f, -0.52f);
                    this.light.setColor(2, 0.16f, 0.18f, 0.18f);
                    this.light.setDirection2(2, 0.092f, -0.993f, -0.069f);
                    this.light.setColor(3, 0.33f, 0.33f, 0.33f);
                    this.light.setDirection2(3, -0.642f, 0.449f, -0.621f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    System.sleep(1);
                    this.PCextends_start(2699, 3613, 0.9f);
                    this.tool.SoundstreamPlay(141118);
                    this.tool.MSG(49, this.kosmos.face, 1, "My time is limited as well.");
                    this.tool.SoundstreamPlay(141119);
                    this.tool.MSG(108, this.kosmos.face, 1, "Failure to comply will result\nin the destruction of this window.");
                    System.sleep(2);
                    this.PCextends_end(n);
                    Runtime.mpeg2("1041_1");
                    break;
                }
                case 3614: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.hammer.renderCommand(512);
                    this.kosmos.renderCommand(512);
                    Sound.streamPlay(1190085, 48000);
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.light.setDirection2(1, 0.096f, 0.028f, -0.995f);
                    this.light.setColor(2, 0.4f, 0.4f, 0.4f);
                    this.light.setDirection2(2, -0.431f, 0.477f, 0.766f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, 0.676f, 0.341f, 0.653f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 3673: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.34f, 0.36f, 0.36f);
                    this.light.setDirection2(1, 0.979f, 0.0f, -0.204f);
                    this.light.setColor(2, 0.14f, 0.15f, 0.15f);
                    this.light.setDirection2(2, 0.849f, -0.524f, -0.069f);
                    this.light.setColor(3, 0.5f, 0.5f, 0.5f);
                    this.light.setDirection2(3, -0.745f, 0.001f, -0.667f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    break;
                }
                case 3763: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.64f, 0.64f, 0.64f);
                    this.light.setDirection2(1, 0.085f, 0.244f, -0.966f);
                    this.light.setColor(2, 0.35f, 0.35f, 0.35f);
                    this.light.setDirection2(2, -0.2f, 0.848f, 0.492f);
                    this.light.setColor(3, 0.28f, 0.28f, 0.28f);
                    this.light.setDirection2(3, -0.605f, -0.786f, -0.128f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141127);
                    this.tool.MSG(50, this.mathews.face, 7, "...Daahhh!\nOkay, okay!!");
                    break;
                }
                case 3823: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.MSG(50, this.mathews.face, 7, "We'll do it, we'll do it...");
                    this.tool.MSG(50, this.mathews.face, 7, "Don't be so rash.");
                    break;
                }
                case 3928: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.35f, 0.37f, 0.37f);
                    this.light.setDirection2(1, 0.876f, -0.34f, -0.342f);
                    this.light.setColor(2, 0.12f, 0.13f, 0.13f);
                    this.light.setDirection2(2, 0.947f, 0.309f, 0.087f);
                    this.light.setColor(3, 0.54f, 0.54f, 0.54f);
                    this.light.setDirection2(3, -0.379f, -0.0f, -0.925f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setVisible(22, true);
                    this.kosmos.setVisible(23, true);
                    this.kosmos.setVisible(24, false);
                    this.kosmos.setVisible(25, false);
                    System.sleep(1);
                    this.PCextends_start(3929, 4007, 0.4f);
                    this.tool.SoundstreamPlay(141128);
                    this.tool.MSG(79, this.kosmos.face, 1, "Had you accommodated me\nfrom the outset,");
                    this.tool.MSG(100, this.kosmos.face, 1, "we would have saved one minute,\n45 seconds.");
                    this.PCextends_end(n);
                    break;
                }
                case 4008: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141129);
                    this.tool.MSG(60, this.kosmos.face, 1, "I am coming onboard now.");
                    this.tool.MSG(60, this.kosmos.face, 1, "Please open the cargo bay.");
                    this.tool.SoundstreamPlay(141130);
                    this.tool.MSG(58, this.mathews.face, 7, "Ah...right.");
                    break;
                }
                case 4188: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.827f, 0.409f, -0.386f);
                    this.light.setColor(2, 0.32f, 0.32f, 0.32f);
                    this.light.setDirection2(2, -0.51f, 0.855f, 0.095f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141131);
                    this.tool.MSG(30, this.kosmos.face, 1, "One more thing...");
                    this.tool.SoundstreamPlay(141132);
                    this.tool.MSG(50, this.mathews.face, 7, "W...uh, what?");
                    break;
                }
                case 4368: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.37f, 0.37f, 0.37f);
                    this.light.setDirection2(1, 0.764f, -0.489f, -0.42f);
                    this.light.setColor(2, 0.16f, 0.16f, 0.16f);
                    this.light.setDirection2(2, 0.17f, 0.786f, -0.594f);
                    this.light.setColor(3, 0.48f, 0.48f, 0.48f);
                    this.light.setDirection2(3, -0.726f, -0.681f, -0.094f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.tool.SoundstreamPlay(141133);
                    this.tool.MSG(135, this.kosmos.face, 1, "Do not accelerate in an attempt\nto knock me off.");
                    this.tool.SoundstreamPlay(141134);
                    this.tool.MSG(70, this.kosmos.face, 1, "If you do so, I will destroy the engine,\nand the entire ship with it.");
                    break;
                }
                case 4578: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.light.setColor(0, 0.15f, 0.15f, 0.15f);
                    this.light.setColor(1, 0.53f, 0.53f, 0.53f);
                    this.light.setDirection2(1, 0.621f, 0.468f, -0.629f);
                    this.light.setColor(2, 0.35f, 0.35f, 0.35f);
                    this.light.setDirection2(2, -0.545f, 0.837f, 0.044f);
                    this.light.setColor(3, 0.26f, 0.26f, 0.26f);
                    this.light.setDirection2(3, -0.078f, -0.451f, -0.889f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    this.kosmos.setLightMode(1);
                    this.kosmos.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.kosmos.light.setColor(1, 0.33f, 0.35f, 0.35f);
                    this.kosmos.light.setDirection2(1, 0.784f, -0.608f, -0.125f);
                    this.kosmos.light.setColor(2, 0.2f, 0.21f, 0.21f);
                    this.kosmos.light.setDirection2(2, 0.044f, -0.399f, -0.916f);
                    this.kosmos.light.setColor(3, 0.52f, 0.52f, 0.52f);
                    this.kosmos.light.setDirection2(3, -0.999f, 0.008f, 0.044f);
                    System.sleep(1);
                    this.PCextends_start(4579, 4772, 1.3f);
                    this.tool.SoundstreamPlay(141135);
                    this.tool.MSG(49, this.mathews.face, 7, "　");
                    this.tool.SoundstreamPlay(141136);
                    this.tool.MSG(60, this.hammer.face, 1, "She's got us read...");
                    this.tool.SoundstreamPlay(141137);
                    this.tool.MSG(30, this.mathews.face, 7, "Shut up!");
                    this.PCextends_end(n);
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void MotionPack_srvE(int n) {
        this.tool.Timechk_srv_totaltime = 0;
        while (this.tool.Timechk_srv_totaltime != n) {
            switch (this.tool.Timechk_srv_totaltime) {
                case 0: {
                    this.tool.Timechk_CutChange();
                    this.space2.start(1, "move");
                    this.kosmos.setLightMode(0);
                    this.light.setColor(0, 0.0f, 0.0f, 0.0f);
                    this.light.setColor(1, 0.57f, 0.58f, 0.58f);
                    this.light.setDirection2(1, 0.92f, 0.392f, 0.002f);
                    this.light.setColor(2, 0.09f, 0.1f, 0.1f);
                    this.light.setDirection2(2, 0.576f, 0.147f, -0.804f);
                    this.light.setColor(3, 0.0f, 0.0f, 0.0f);
                    this.light.setDirection2(3, -0.999f, 0.008f, 0.044f);
                    Stage.setColor(1.0f, 1.0f, 1.0f);
                    Runtime.setDefocusQuick(0, 1, 38880, 1);
                    Runtime.setDefocusQuick(1, 1, 30688, 1);
                    Runtime.setDefocusQuick(2, 1, 22496, 1);
                    break;
                }
            }
            System.sleep(1);
        }
    }

    void PCextends_end(int n) {
        this.TotalCutTime = this.tool.Timechk_srv_totaltime - this.TotalCutTime;
        this.tool.Timechk_srv_totaltime = this.NextPCStart;
        this.pc.init(1, 0, this.NextPCStart + 1, n, 1.0f);
        this.pc.start();
        System.println("------------ＭＰ速度変更は正常に終了しました。");
        Runtime.setRegister(0, this.TotalCutTime);
        Runtime.setRegister(1, this.tool.CutNo);
        Runtime.setRegister(2, this.BaseCutTime);
        System.println("------------カット/[$1]が（/[$2]→/[$0]）フレームに変更されました。");
        if (this.BaseCutTime <= this.TotalCutTime) {
            Runtime.setRegister(0, this.TotalCutTime - this.BaseCutTime);
            System.println("------------/[$0]フレームの増加です。");
        } else {
            Runtime.setRegister(0, this.BaseCutTime - this.TotalCutTime);
            System.println("------------/[$0]フレームの減少です。");
        }
    }

    void PCextends_start(int n, int n2, float f) {
        this.NextPCStart = n2;
        this.TotalCutTime = this.tool.Timechk_srv_totaltime;
        this.BaseCutTime = n2 - n;
        this.pc.init(1, 0, n, n2, f);
        this.pc.start();
        Runtime.setRegister(0, n);
        Runtime.setRegister(1, n2);
        Runtime.setRegister(2, f);
        System.println("------------ＭＰの速度を変更します(/[$0]-/[$1]) ×/[#2]");
    }

    void Xenvmainthreadmain() {
        while (true) {
            int n;
            if ((n = this.Xpad1P.getButton()) == 79) {
                this.Xenvmainthreadendflag = true;
            }
            System.sleep(1);
        }
    }

    void Xenvplaymain() {
        this.Xenvmainthread = Thread.create(this, "Xenvmainthreadmain");
        this.Xenvmainthread.start();
        this.Xenvplaythread = Thread.create(this, "Xenvplaythread");
        this.Xenvplaythread.start();
        while (!this.Xenvmainthreadendflag) {
            System.sleep(1);
        }
    }

    void Xenvplaythread() {
        this.Xenvplaymain();
        this.Xenvmainthreadendflag = true;
    }

    void chair(boolean bl) {
        Stage.setVisible(37, bl);
        Stage.setVisible(38, bl);
        Stage.setVisible(39, bl);
        Stage.setVisible(40, bl);
        Stage.setVisible(41, bl);
        Stage.setVisible(42, bl);
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01041_F");
        Runtime.setFlags(54, 1, 1);
        System.println("XEVEJNAME:SCE01042");
        Runtime.jumpEvent(1420);
    }

    public void cleanupOriginal() {
        Runtime.setFlags(54, 1, 1);
        Runtime.jumpEvent(1420);
    }

    void eft_srv() {
        this.eft1a.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1a.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1a.setScale(3.4f, 3.4f, 3.4f);
        this.eft1a.disp(true);
        System.sleep(5);
        this.eft1b = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1b.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1b.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1b.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1c = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1c.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1c.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1c.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1d = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1d.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1d.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1d.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1e = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1e.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1e.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1e.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1f = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1f.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1f.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1f.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1g = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1g.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1g.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1g.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1h = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1h.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1h.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1h.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1i = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1i.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1i.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1i.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1j = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1j.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1j.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1j.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1k = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1k.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1k.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1k.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1l = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1l.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1l.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1l.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1m = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1m.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1m.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1m.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1n = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1n.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1n.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1n.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1o = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1o.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1o.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1o.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1p = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1p.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1p.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1p.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1q = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1q.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1q.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1q.setScale(3.4f, 3.4f, 3.4f);
        System.sleep(5);
        this.eft1r = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1r.setTranslate(-9.8f, 4.1f, 33.3f);
        this.eft1r.setRotate(0.0f, 0.0f, 0.0f);
        this.eft1r.setScale(3.4f, 3.4f, 3.4f);
    }

    void init() {
        Runtime.setLocation(46);
        this.eft1a = new Effect(1456, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft1a.disp(false);
        this.eft_mobj075 = new Effect(1553, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft_mobj075.setScale(0.1f, 0.1f, 0.1f);
        this.eft_mobj075.setCaster(this.mobj075);
        this.eft_mobj075.setMotion(true);
        this.eft_mobj075.noAttach(false);
        this.eft_mobj075.disp(false);
        this.eft_mobj075.setTranslate(0.0f, -0.76f, -0.22f);
        this.eft_mobj075.setRotate(0.0f, 0.0f, -0.0f);
        this.cigar = new Unit();
        this.cigar.init(24614, 0.0f, 0.0f, 0.0f, 0.0f);
        this.cigar.setParent(this.mathews, 60);
        this.cigar.setTranslate(0.15f, -0.07f, -0.02f);
        this.cigar.setRotate(-110.0f, 60.0f, 90.0f);
        this.cigar.setScale(8.0f, 8.0f, 8.0f);
        this.eft2 = new Effect(1533, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft2.setScale(1.0f, 1.0f, 1.0f);
        this.eft2.setCaster(this.cigar);
        this.eft2.setTranslate(0.0f, 0.04f, 0.01f);
        this.eft2.setRotate(0.0f, 0.0f, 0.0f);
        this.eft2.setScale(0.1f, 0.1f, 0.1f);
        this.eft2.disp(false);
        this.eft3 = new Effect(1535, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft3.setScale(1.0f, 1.0f, 1.0f);
        this.eft3.setTranslate(0.0f, 0.0f, 0.0f);
        this.eft3.setCaster(this.mathews);
        this.eft3.setTranslate(-2.98f, 1.54f, 2.92f);
        this.eft3.setRotate(0.0f, 180.0f, 0.0f);
        this.eft3.setScale(0.3f, 0.3f, 0.3f);
        this.eft3.setForceLoop(true);
        this.eft3.disp(false);
        this.space2 = new Space();
        this.space2.init(20615);
        this.space2.setRotate(316.99f, -210.0f, 259.99f);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.pc = PlayControl.create();
        this.pc.loadCamera("c01s41B.cam");
        this.pc.init(1, 4);
        this.eft0 = new Effect(1451, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft0.setScale(0.1f, 0.1f, 0.1f);
        this.eft0.setCaster(this.mobj001);
        this.eft0.disp(true);
        this.eft0.setTranslate(0.0f, -0.03f, -0.06f);
        this.eft0.setScale(0.1f, 0.1f, 0.1f);
        this.fm1 = new Monitor();
        this.fm1.setArgs(0, 0.0f, 0.5f, 1.6f, 1.4f);
        this.fm1.setArgs(1, 20064, 0, 128, 112);
        this.fm1.setArgs(2, 77, 0, 15, -1);
        this.fm1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.fm1.signal(1);
        this.fm1.setScale(0.3f, 0.3f, 0.3f);
        this.fm1.setTranslate(-3.03f, 0.537f, 0.7f);
        this.fm1.setRotate(0.0f, 3.15f, 0.0f);
        this.em2 = new Monitor();
        this.em2.setArgs(0, 0.0f, 0.5f, 1.84f, 0.91f);
        this.em2.setArgs(1, 20036, 0, 128, 112);
        this.em2.setArgs(2, 52, 0, 15, -1);
        this.em2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.em2.signal(1);
        this.em2.setScale(0.34f, 0.34f, 0.34f);
        this.em2.setTranslate(0.0f, -0.31f, -4.4f);
        this.em2.setRotate(0.0f, 0.0f, -0.0f);
        this.tonny_fm = new Monitor();
        this.tonny_fm.setArgs(0, 0.0f, 0.0f, 1.36f, 0.72f);
        this.tonny_fm.setArgs(1, 20005, 0, 128, 112);
        this.tonny_fm.setArgs(2, 67, 0, 15, -1);
        this.tonny_fm.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.tonny_fm.signal(1);
        this.tonny_fm.setScale(0.16f, 0.16f, 0.16f);
        this.tonny_fm.setTranslate(-0.0f, -0.3f, -4.0f);
        this.tonny_fm.setRotate(-22.08f, -0.0f, 0.0f);
    }

    void initialize() {
    }

    static void main() {
    }

    void map_under(boolean bl) {
        Stage.setVisible(43, bl);
        Stage.setVisible(46, bl);
    }

    void monitor(boolean bl) {
        Stage.setVisible(52, bl);
        Stage.setVisible(53, bl);
        Stage.setVisible(54, bl);
        Stage.setVisible(55, bl);
        Stage.setVisible(56, bl);
        Stage.setVisible(57, bl);
        Stage.setVisible(51, bl);
    }

    void play() {
        this.tool.loadarc(this.mathews.face, "FLSmatehws.fpk");
        this.tool.loadarc(this.tonny.face, "FLStonny.fpk");
        this.tool.loadarc(this.hammer.face, "FLShammer.fpk");
        this.tool.loadarc(this.kosmos.face, "FLSkosmos_h.fpk");
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        this.tool.CaptureTool();
        this.tool.Timechk();
        this.tool.SoundstreamDebug_init(999999);
        Sound.streamPlay(1190083, 48000);
        this.tonny.renderCommand(530);
        this.mathews.renderCommand(530);
        this.hammer.renderCommand(512);
        this.kosmos.renderCommand(512);
        this.mobj021.renderCommand(512);
        this.tonny.setMotionFlags(0x2000000, true);
        this.hammer.setMotionFlags(0x2000000, true);
        this.Focus_init();
        this.Chr_init(true);
        this.Map_init(false);
        this.hammer.setVisible(false);
        this.mathews.setVisible(false);
        this.tonny.setVisible(false);
        this.kosmos.setVisible(false);
        this.mobj001.mtn(283, 0, 540, 8, 8, 1.0f, true);
        this.mobj064a.mtn(258, 0, 540, 8, 8, 1.0f, true);
        this.mobj064b.mtn(259, 0, 540, 8, 8, 1.0f, true);
        this.mobj064c.mtn(260, 0, 540, 8, 8, 1.0f, true);
        this.mobj064d.mtn(261, 0, 540, 8, 8, 1.0f, true);
        this.mobj064e.mtn(262, 0, 540, 8, 8, 1.0f, true);
        this.mobj064f.mtn(263, 0, 540, 8, 8, 1.0f, true);
        this.mobj064g.mtn(264, 0, 540, 8, 8, 1.0f, true);
        this.mobj064h.mtn(265, 0, 540, 8, 8, 1.0f, true);
        this.mobj064i.mtn(266, 0, 540, 8, 8, 1.0f, true);
        this.mobj064j.mtn(267, 0, 540, 8, 8, 1.0f, true);
        this.mobj065a.mtn(268, 0, 540, 8, 8, 1.0f, true);
        this.mobj065b.mtn(269, 0, 540, 8, 8, 1.0f, true);
        this.mobj065c.mtn(270, 0, 540, 8, 8, 1.0f, true);
        this.mobj065d.mtn(271, 0, 540, 8, 8, 1.0f, true);
        this.mobj065e.mtn(272, 0, 540, 8, 8, 1.0f, true);
        this.mobj066a.mtn(273, 0, 540, 8, 8, 1.0f, true);
        this.mobj066b.mtn(274, 0, 540, 8, 8, 1.0f, true);
        this.mobj066c.mtn(275, 0, 540, 8, 8, 1.0f, true);
        this.mobj066d.mtn(276, 0, 540, 8, 8, 1.0f, true);
        this.mobj066e.mtn(277, 0, 540, 8, 8, 1.0f, true);
        this.mobj067a.mtn(278, 0, 540, 8, 8, 1.0f, true);
        this.mobj067b.mtn(279, 0, 540, 8, 8, 1.0f, true);
        this.mobj067c.mtn(280, 0, 540, 8, 8, 1.0f, true);
        this.mobj067d.mtn(281, 0, 540, 8, 8, 1.0f, true);
        this.mobj067e.mtn(282, 0, 540, 8, 8, 1.0f, true);
        this.mobj067f.setVisible(false);
        this.mobj021.setVisible(false);
        this.mobj075.setVisible(false);
        this.cigar.setVisible(false);
        this.mobj001.start(5, null);
        this.mobj064a.start(5, null);
        this.mobj064b.start(5, null);
        this.mobj064c.start(5, null);
        this.mobj064d.start(5, null);
        this.mobj064e.start(5, null);
        this.mobj064f.start(5, null);
        this.mobj064g.start(5, null);
        this.mobj064h.start(5, null);
        this.mobj064i.start(5, null);
        this.mobj064j.start(5, null);
        this.mobj065a.start(5, null);
        this.mobj065b.start(5, null);
        this.mobj065c.start(5, null);
        this.mobj065d.start(5, null);
        this.mobj065e.start(5, null);
        this.mobj066a.start(5, null);
        this.mobj066b.start(5, null);
        this.mobj066c.start(5, null);
        this.mobj066d.start(5, null);
        this.mobj066e.start(5, null);
        this.mobj067a.start(5, null);
        this.mobj067b.start(5, null);
        this.mobj067c.start(5, null);
        this.mobj067d.start(5, null);
        this.mobj067e.start(5, null);
        this.mobj001.renderCommand(32);
        this.pc.loadCamera("c01s41A.cam");
        this.pc.start();
        this.MotionPack_srvA(540);
        this.Focus_init();
        this.Chr_init(true);
        this.Map_init(true);
        this.hammer.mtn(284, 0, 2800, 8, 8, 1.0f, true);
        this.mathews.mtn(285, 0, 2800, 8, 8, 1.0f, true);
        this.mobj064a.mtn(286, 0, 2800, 8, 8, 1.0f, true);
        this.mobj064b.mtn(287, 0, 2800, 8, 8, 1.0f, true);
        this.mobj064c.setVisible(false);
        this.mobj064d.setVisible(false);
        this.mobj064e.setVisible(false);
        this.mobj064f.setVisible(false);
        this.mobj064g.setVisible(false);
        this.mobj064h.setVisible(false);
        this.mobj064i.setVisible(false);
        this.mobj064j.setVisible(false);
        this.mobj065a.mtn(288, 0, 2800, 8, 8, 1.0f, true);
        this.mobj065b.mtn(289, 0, 2800, 8, 8, 1.0f, true);
        this.mobj065c.setVisible(false);
        this.mobj065d.setVisible(false);
        this.mobj065e.setVisible(false);
        this.mobj066a.mtn(290, 0, 2800, 8, 8, 1.0f, true);
        this.mobj066b.mtn(291, 0, 2800, 8, 8, 1.0f, true);
        this.mobj066c.mtn(292, 0, 2800, 8, 8, 1.0f, true);
        this.mobj066d.setVisible(false);
        this.mobj066e.setVisible(false);
        this.mobj067a.mtn(293, 0, 2800, 8, 8, 1.0f, true);
        this.mobj067b.mtn(294, 0, 2800, 8, 8, 1.0f, true);
        this.mobj067c.setVisible(false);
        this.mobj067d.setVisible(false);
        this.mobj067e.setVisible(false);
        this.mobj067f.setVisible(false);
        this.mobj021.mtn(295, 0, 2800, 8, 8, 1.0f, true);
        this.tonny.mtn(296, 0, 2800, 8, 8, 1.0f, true);
        this.kosmos.setVisible(false);
        this.mobj001.setVisible(false);
        this.mobj075.setVisible(false);
        this.eft0.disp(false);
        this.cigar.setVisible(false);
        this.hammer.start(5, null);
        this.mathews.start(5, null);
        this.mobj064a.start(5, null);
        this.mobj064b.start(5, null);
        this.mobj065a.start(5, null);
        this.mobj065b.start(5, null);
        this.mobj066a.start(5, null);
        this.mobj066b.start(5, null);
        this.mobj066c.start(5, null);
        this.mobj067a.start(5, null);
        this.mobj067b.start(5, null);
        this.mobj021.start(5, null);
        this.tonny.start(5, null);
        this.mobj001.renderCommand(0);
        this.pc.loadCamera("c01s41B.cam");
        this.pc.init(1, 4);
        this.pc.start();
        this.MotionPack_srvB(2800);
        this.Focus_init();
        this.Chr_init(true);
        this.Map_init(false);
        this.hammer.setVisible(false);
        this.mathews.setVisible(false);
        this.tonny.setVisible(false);
        this.kosmos.setVisible(false);
        this.mobj064a.mtn(298, 0, 240, 8, 8, 1.0f, true);
        this.mobj064b.mtn(299, 0, 240, 8, 8, 1.0f, true);
        this.mobj064c.mtn(300, 0, 240, 8, 8, 1.0f, true);
        this.mobj064d.mtn(301, 0, 240, 8, 8, 1.0f, true);
        this.mobj064e.mtn(302, 0, 240, 8, 8, 1.0f, true);
        this.mobj064f.setVisible(false);
        this.mobj064g.setVisible(false);
        this.mobj064h.setVisible(false);
        this.mobj064i.setVisible(false);
        this.mobj064j.setVisible(false);
        this.mobj065a.mtn(303, 0, 240, 8, 8, 1.0f, true);
        this.mobj065b.mtn(304, 0, 240, 8, 8, 1.0f, true);
        this.mobj065c.mtn(305, 0, 240, 8, 8, 1.0f, true);
        this.mobj065d.mtn(306, 0, 240, 8, 8, 1.0f, true);
        this.mobj065e.mtn(307, 0, 240, 8, 8, 1.0f, true);
        this.mobj066a.mtn(308, 0, 240, 8, 8, 1.0f, true);
        this.mobj066b.mtn(309, 0, 240, 8, 8, 1.0f, true);
        this.mobj066c.mtn(310, 0, 240, 8, 8, 1.0f, true);
        this.mobj066d.mtn(311, 0, 240, 8, 8, 1.0f, true);
        this.mobj066e.mtn(312, 0, 240, 8, 8, 1.0f, true);
        this.mobj067a.mtn(313, 0, 240, 8, 8, 1.0f, true);
        this.mobj067b.mtn(314, 0, 240, 8, 8, 1.0f, true);
        this.mobj067c.mtn(315, 0, 240, 8, 8, 1.0f, true);
        this.mobj067d.mtn(316, 0, 240, 8, 8, 1.0f, true);
        this.mobj067e.mtn(317, 0, 240, 8, 8, 1.0f, true);
        this.mobj067f.mtn(318, 0, 240, 8, 8, 1.0f, true);
        this.mobj001.mtn(319, 0, 240, 8, 8, 1.0f, true);
        this.mobj021.setVisible(false);
        this.mobj075.setVisible(false);
        this.eft0.disp(true);
        this.cigar.setVisible(false);
        this.mobj064a.start(5, null);
        this.mobj064b.start(5, null);
        this.mobj064c.start(5, null);
        this.mobj064d.start(5, null);
        this.mobj064e.start(5, null);
        this.mobj065a.start(5, null);
        this.mobj065b.start(5, null);
        this.mobj065c.start(5, null);
        this.mobj065d.start(5, null);
        this.mobj065e.start(5, null);
        this.mobj066a.start(5, null);
        this.mobj066b.start(5, null);
        this.mobj066c.start(5, null);
        this.mobj066d.start(5, null);
        this.mobj066e.start(5, null);
        this.mobj067a.start(5, null);
        this.mobj067b.start(5, null);
        this.mobj067c.start(5, null);
        this.mobj067d.start(5, null);
        this.mobj067e.start(5, null);
        this.mobj067f.start(5, null);
        this.mobj001.start(5, null);
        this.mobj001.renderCommand(32);
        this.pc.loadCamera("c01s41C.cam");
        this.pc.init(1, 4);
        this.eft_thread.start();
        this.pc.start();
        this.MotionPack_srvC(240);
        this.eft_thread.stop();
        this.eft1a.disp(false);
        this.eft1b.disp(false);
        this.eft1c.disp(false);
        this.eft1d.disp(false);
        this.eft1e.disp(false);
        this.eft1f.disp(false);
        this.eft1g.disp(false);
        this.eft1h.disp(false);
        this.eft1i.disp(false);
        this.eft1j.disp(false);
        this.eft1k.disp(false);
        this.eft1l.disp(false);
        this.eft1m.disp(false);
        this.eft1n.disp(false);
        this.eft1o.disp(false);
        this.eft1p.disp(false);
        this.eft1q.disp(false);
        this.eft1r.disp(false);
        this.Focus_init();
        this.Chr_init(true);
        this.Map_init(true);
        this.hammer.mtn(320, 0, 3060, 8, 8, 1.0f, true);
        this.kosmos.mtn(322, 0, 3060, 8, -1073741816, 1.0f, true);
        this.mathews.mtn(325, 0, 3060, 8, 8, 1.0f, true);
        this.tonny.mtn(327, 0, 3060, 8, 8, 1.0f, true);
        this.mobj064a.mtn(329, 0, 3060, 8, 8, 1.0f, true);
        this.mobj064b.mtn(330, 0, 3060, 8, 8, 1.0f, true);
        this.mobj064c.mtn(331, 0, 3060, 8, 8, 1.0f, true);
        this.mobj064d.mtn(332, 0, 3060, 8, 8, 1.0f, true);
        this.mobj064e.setVisible(false);
        this.mobj064f.setVisible(false);
        this.mobj064g.setVisible(false);
        this.mobj064h.setVisible(false);
        this.mobj064i.setVisible(false);
        this.mobj064j.setVisible(false);
        this.mobj065a.mtn(333, 0, 3060, 8, 8, 1.0f, true);
        this.mobj065b.mtn(334, 0, 3060, 8, 8, 1.0f, true);
        this.mobj065c.mtn(335, 0, 3060, 8, 8, 1.0f, true);
        this.mobj065d.mtn(336, 0, 3060, 8, 8, 1.0f, true);
        this.mobj065e.setVisible(false);
        this.mobj066a.mtn(337, 0, 3060, 8, 8, 1.0f, true);
        this.mobj066b.mtn(338, 0, 3060, 8, 8, 1.0f, true);
        this.mobj066c.mtn(339, 0, 3060, 8, 8, 1.0f, true);
        this.mobj066d.mtn(340, 0, 3060, 8, 8, 1.0f, true);
        this.mobj066e.setVisible(false);
        this.mobj067a.mtn(341, 0, 3060, 8, 8, 1.0f, true);
        this.mobj067b.mtn(342, 0, 3060, 8, 8, 1.0f, true);
        this.mobj067c.mtn(343, 0, 3060, 8, 8, 1.0f, true);
        this.mobj067d.mtn(344, 0, 3060, 8, 8, 1.0f, true);
        this.mobj067e.setVisible(false);
        this.mobj067f.setVisible(false);
        this.mobj021.mtn(345, 0, 3060, 8, 8, 1.0f, true);
        this.mobj001.setVisible(false);
        this.mobj075.setVisible(false);
        this.eft0.disp(false);
        this.cigar.setVisible(false);
        this.hammer.start(5, null);
        this.kosmos.start(5, null);
        this.mathews.start(5, null);
        this.tonny.start(5, null);
        this.mobj064a.start(5, null);
        this.mobj064b.start(5, null);
        this.mobj064c.start(5, null);
        this.mobj064d.start(5, null);
        this.mobj065a.start(5, null);
        this.mobj065b.start(5, null);
        this.mobj065c.start(5, null);
        this.mobj065d.start(5, null);
        this.mobj066a.start(5, null);
        this.mobj066b.start(5, null);
        this.mobj066c.start(5, null);
        this.mobj066d.start(5, null);
        this.mobj067a.start(5, null);
        this.mobj067b.start(5, null);
        this.mobj067c.start(5, null);
        this.mobj067d.start(5, null);
        this.mobj021.start(5, null);
        this.mobj001.renderCommand(0);
        this.pc.loadCamera("c01s41D1.cam");
        this.pc.init(1, 4);
        this.pc.start();
        this.MotionPack_srvD1(3060);
        this.Focus_init();
        this.Chr_init(true);
        this.Map_init(true);
        this.hammer.mtn(321, 0, 4953, 8, 8, 1.0f, true);
        this.kosmos.mtn(323, 0, 4953, 8, -1073741816, 1.0f, true);
        this.mathews.mtn(326, 0, 4953, 8, 8, 1.0f, true);
        this.tonny.mtn(328, 0, 4953, 8, 8, 1.0f, true);
        this.mobj064a.mtn(346, 0, 4953, 8, 8, 1.0f, true);
        this.mobj064b.setVisible(false);
        this.mobj064c.setVisible(false);
        this.mobj064d.setVisible(false);
        this.mobj064e.setVisible(false);
        this.mobj064f.setVisible(false);
        this.mobj064g.setVisible(false);
        this.mobj064h.setVisible(false);
        this.mobj064i.setVisible(false);
        this.mobj064j.setVisible(false);
        this.mobj065a.mtn(347, 0, 4953, 8, 8, 1.0f, true);
        this.mobj065b.mtn(348, 0, 4953, 8, 8, 1.0f, true);
        this.mobj065c.setVisible(false);
        this.mobj065d.setVisible(false);
        this.mobj065e.setVisible(false);
        this.mobj066a.mtn(349, 0, 4953, 8, 8, 1.0f, true);
        this.mobj066b.mtn(350, 0, 4953, 8, 8, 1.0f, true);
        this.mobj066c.setVisible(false);
        this.mobj066d.setVisible(false);
        this.mobj066e.setVisible(false);
        this.mobj067a.mtn(351, 0, 4953, 8, 8, 1.0f, true);
        this.mobj067b.setVisible(false);
        this.mobj067c.setVisible(false);
        this.mobj067d.setVisible(false);
        this.mobj067e.setVisible(false);
        this.mobj067f.setVisible(false);
        this.mobj021.mtn(352, 0, 4953, 8, 8, 1.0f, true);
        this.mobj001.setVisible(false);
        this.mobj075.setVisible(false);
        this.eft0.disp(false);
        this.cigar.setVisible(false);
        this.hammer.start(5, null);
        this.kosmos.start(5, null);
        this.mathews.start(5, null);
        this.tonny.start(5, null);
        this.mobj064a.start(5, null);
        this.mobj065a.start(5, null);
        this.mobj065b.start(5, null);
        this.mobj066a.start(5, null);
        this.mobj066b.start(5, null);
        this.mobj067a.start(5, null);
        this.mobj021.start(5, null);
        this.pc.loadCamera("c01s41D2.cam");
        this.pc.init(1, 4);
        this.pc.start();
        this.MotionPack_srvD2(4953);
        this.Focus_init();
        this.Chr_init(true);
        this.Map_init(false);
        this.kosmos.mtn(353, 0, 500, 8, -1073741816, 1.0f, true);
        this.mobj001.mtn(369, 0, 500, 8, 8, 1.0f, true);
        this.mobj075.mtn(370, 0, 500, 8, 8, 1.0f, true);
        this.hammer.setVisible(false);
        this.tonny.setVisible(false);
        this.mathews.setVisible(false);
        this.mobj064a.setVisible(false);
        this.mobj064b.setVisible(false);
        this.mobj064c.setVisible(false);
        this.mobj064d.setVisible(false);
        this.mobj064e.setVisible(false);
        this.mobj064f.setVisible(false);
        this.mobj064g.setVisible(false);
        this.mobj064h.setVisible(false);
        this.mobj064i.setVisible(false);
        this.mobj064j.setVisible(false);
        this.mobj065a.setVisible(false);
        this.mobj065b.setVisible(false);
        this.mobj065c.setVisible(false);
        this.mobj065d.setVisible(false);
        this.mobj065e.setVisible(false);
        this.mobj066a.setVisible(false);
        this.mobj066b.setVisible(false);
        this.mobj066c.setVisible(false);
        this.mobj066d.setVisible(false);
        this.mobj066e.setVisible(false);
        this.mobj067a.setVisible(false);
        this.mobj067b.setVisible(false);
        this.mobj067c.setVisible(false);
        this.mobj067d.setVisible(false);
        this.mobj067e.setVisible(false);
        this.mobj067f.setVisible(false);
        this.mobj021.setVisible(false);
        this.eft0.disp(false);
        this.cigar.setVisible(false);
        this.eft_mobj075.disp(true);
        this.mobj001.start(5, null);
        this.mobj075.start(5, null);
        this.kosmos.start(5, null);
        this.mobj001.renderCommand(32);
        this.pc.loadCamera("c01s41E.cam");
        this.pc.init(1, 4);
        this.pc.start();
        this.MotionPack_srvE(470);
        this.tool.Timechk_SceneEnd();
    }

    void space_camchase() {
        while (true) {
            this.space2.setTranslate(this.BaseCam.getTranslateX(), this.BaseCam.getTranslateY(), this.BaseCam.getTranslateZ());
            System.sleep(1);
        }
    }

    class Chrs
            extends Chr {
        public Chrs(int n) {
            this.init(n);
            this.setShadow(0, 0);
            this.setMotNoUpdate(2);
        }
    }

    class FaceChr
            extends Chr {
        Chr face;

        public FaceChr(int n) {
            this.init(n + 0x1000000, 0.0f, 0.0f, 0.0f, 0.0f);
            this.face = this.getChild(0x1000000);
            this.setMotNoUpdate(2);
        }
    }

    class Monitor
            extends Unit {
        float sz;
        int alpha;

        public Monitor() {
            this.init(24613);
        }

        public Monitor(int n, float f, float f2, float f3, float f4) {
            this.init(n, f, f2, f3, f4);
        }

        void moveloop() {
            float f = 0.0f;
            while (true) {
                this.setTranslate(1.0f + Math.sin(f) * 0.02f, 0.6f + Math.cos(f) * 0.02f, -2.23f);
                f += 0.1f;
                System.sleep(1);
            }
        }

        void off_a() {
            this.alpha = 96;
            while (this.alpha <= 0) {
                this.alpha -= 6;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 0, 0, 0, -1);
        }

        void off_s() {
            this.setScale(0.0f, 0.0f, 0.0f);
        }

        void on1() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on2() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on3() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            System.sleep(10);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on4() {
            this.alpha = 0;
            this.setArgs(2, this.alpha, 0, 0, -1);
            this.setScale(1.0f, 1.0f, 1.0f);
            System.sleep(20);
            System.sleep(10);
            System.sleep(10);
            while (this.alpha <= 48) {
                this.alpha += 3;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }

        void on_a() {
            this.alpha = 0;
            while (this.alpha <= 96) {
                this.alpha += 6;
                this.setArgs(2, this.alpha, 0, 0, -1);
                System.sleep(1);
            }
            this.setArgs(2, 96, 0, 0, -1);
        }
    }

    class Space
            extends Unit {
        Space() {
        }

        void CamChase() {
            this.setVisible(true);
            this.setScale(1.6f, 1.6f, 1.6f);
            while (true) {
                this.setTranslate(SCE01041.this.BaseCam.getTranslateX(), SCE01041.this.BaseCam.getTranslateY(), SCE01041.this.BaseCam.getTranslateZ());
                System.sleep(1);
            }
        }

        void CamChase2() {
            this.setVisible(true);
            this.setScale(1.6f, 1.6f, 1.6f);
            while (true) {
                this.setTranslate(SCE01041.this.ManualCam.getTranslateX(), SCE01041.this.ManualCam.getTranslateY(), SCE01041.this.ManualCam.getTranslateZ());
                float f = 40.0f / SCE01041.this.ManualCam.getFov();
                System.println("/[#0]");
                System.sleep(10);
            }
        }

        void invisible() {
            System.println("space invisible.");
            this.setVisible(false);
            this.setScale(0.0f, 0.0f, 0.0f);
        }

        void move() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz + 0.03f);
                System.sleep(1);
            }
        }

        void move2() {
            this.setTranslate(0.0f, 0.0f, 0.0f);
            while (true) {
                this.setTranslate(this.px, this.py, this.pz - 0.03f);
                System.sleep(1);
            }
        }
    }
}

