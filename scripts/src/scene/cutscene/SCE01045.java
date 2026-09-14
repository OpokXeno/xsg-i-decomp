import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.XenoConstants;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01045
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01045,
        FLSfried {
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    static final int BGOBJ_monitor = 41;
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    Light light = new Light(0);
    FaceChr kebin = new FaceChr(265);
    FaceChr fried = new FaceChr(273);
    Chrs mobj026 = new Chrs(20507);
    boolean CameraStart_exec = false;
    Thread camera_thread = Thread.create();
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01045() {
    }

    void CameraStart(String string) {
        if (this.CameraStart_exec) {
            this.camera_thread.stop();
        }
        this.CameraStart_exec = true;
        this.camera_thread.setTarget(this.camerawork, string);
        this.camera_thread.start();
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

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:CHAPTER1_F");
        Runtime.setFlags(100, 1, 1);
        System.println("XEVEJNAME:SCE02000");
        Runtime.jumpEvent(2000);
    }

    void init() {
        System.methodSignal(1);
        Runtime.setLocation(1000);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.mobj026.setTranslate(-2.59f, 0.81f, -1.13f);
        this.mobj026.setRotate(0.0f, 69.0f, 0.0f);
        this.kebin.setVisible(1, false);
        this.kebin.setVisible(2, false);
        this.kebin.setVisible(3, false);
        this.kebin.setVisible(4, false);
        this.kebin.setVisible(5, false);
    }

    void initialize() {
    }

    static void main() {
    }

    void play() {
        this.tool.loadarc(this.fried.face, "FLSfried.fpk");
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        Sound.streamPlay(1190064, 48000);
        this.tool.Timechk_SceneStart();
        this.tool.SoundstreamDebug_init(999999);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.37f, 0.61f, 0.63f);
        this.light.setDirection2(1, 0.344f, 0.43f, 0.835f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.31f, 0.53f, 0.54f);
        this.light.setDirection2(2, 0.48f, -0.877f, 0.035f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.5f, 0.65f, 0.61f);
        this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.kebin.start(1, "act1k");
        this.fried.start(1, "act1f");
        this.mobj026.start(1, "mobj026loop");
        this.camerawork.cut1();
        System.sleep(20);
        this.tool.SoundstreamPlay(199001);
        this.tool.MSG(50, "ケビン", "Master Wilhelm.");
        this.tool.SoundstreamPlay(199002);
        this.tool.MSG(60, "ケビン", "I have a report from KOS-MOS.");
        this.tool.SoundstreamPlay(199003);
        this.tool.MSG(180, "ケビン", "Shion Uzuki and Allen Ridgeley have\nboth joined up with KOS-MOS.");
        this.tool.SoundstreamPlay(199004);
        this.tool.MSG(30, this.fried.face, 1, "I see.");
        System.sleep(20);
        this.tool.SoundstreamPlay(199005);
        this.tool.MSG(160, "ケビン", "This is fortunate, especially since\nthere's an unidentified ship closing in\non the battlefield.");
        System.sleep(30);
        this.fried.start(1, "act2f");
        this.camerawork.cut2();
        System.sleep(20);
        this.tool.SoundstreamPlay(199006);
        this.tool.MSG(160, this.fried.face, 1, "And if the ship were to threaten\nthe girl, KOS-MOS would protect her...");
        System.sleep(30);
        this.tool.SoundstreamPlay(199007);
        this.tool.MSG(40, this.fried.face, 1, "Is that not right?");
        System.sleep(20);
        this.tool.MSG(90, "ケビン", "Yes.\nThat would be the prime directive.");
        System.sleep(30);
        this.fried.start(1, "act2bf");
        this.mobj026.start(1, "mobj026loop");
        this.camerawork.cut2b();
        this.tool.SoundstreamPlay(199010);
        this.tool.MSG(141, this.fried.face, 1, "Either way, it was a wise decision to\npull back KOS-MOS.");
        System.sleep(15);
        this.tool.SoundstreamPlay(199011);
        this.tool.MSG(150, this.fried.face, 1, "There's no need for us to continue\nserving them, or the Federation,\nany longer.");
        System.sleep(15);
        this.tool.SoundstreamPlay(199012);
        this.tool.MSG(120, this.fried.face, 1, "Besides, the data for the Rhine Maiden\nis now complete.");
        System.sleep(30);
        this.fried.start(1, "act3f");
        this.CameraStart("cut3");
        this.tool.SoundstreamPlay(199013);
        this.tool.MSG(180, this.fried.face, 1, "All phenomena are moving forward as\nspecified by this Compass of Order.");
        System.sleep(20);
        this.tool.SoundstreamPlay(199014);
        this.tool.MSG(30, this.fried.face, 1, "As for the rest...");
        System.sleep(60);
        this.camerawork.cut4();
        this.tool.SoundstreamPlay(199015);
        this.tool.MSG(160, this.fried.face, 1, "Gather the necessary factors\nand wait for the other one\nto awaken...");
        this.tool.Timechk_SceneEnd();
    }

    class Units
            extends Unit {
        public Units(int n) {
            this.init(n);
        }
    }

    class Chrs
            extends Chr {
        public Chrs(int n) {
            this.init(n);
            this.setShadow(0, 0);
        }

        void mobj026loop() {
            this.mtn(261, 0, 299, 8, 8, 0.2f, true);
        }
    }

    class FaceChr
            extends Chr {
        Chr face;

        public FaceChr(int n) {
            this.init(n + 0x1000000, 0.0f, 0.0f, 0.0f, 0.0f);
            this.face = this.getChild(0x1000000);
            this.setShadow(4, 16);
        }

        void act1f() {
            this.setTranslate(-1.99f, -0.01f, -1.94f);
            this.setRotate(0.0f, 50.0f, 0.0f);
            this.mtn(258, 0, 120, 8, 8, 1.0f, true);
        }

        void act1k() {
            this.setTranslate(1.59f, 0.0f, 1.7f);
            this.setRotate(0.0f, 220.0f, 0.0f);
            this.mtn(257, 0, 120, 8, 8, 1.0f, true);
        }

        void act2bf() {
            this.mtn(259, 100, 330, 8, 8, 0.4f, true);
        }

        void act2cf() {
            this.mtn(259, 200, 330, 8, 8, 0.5f, true);
        }

        void act2f() {
            this.mtn(259, 0, 330, 8, 8, 0.8f, true);
        }

        void act3f() {
            this.mtn(260, 0, 416, 8, 8, 0.8f, true);
        }
    }

    class Camerawork {
        Camerawork() {
        }

        public void cut1() {
            SCE01045.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, 1.46f, 0.87f, 5.89f, 33.08f, 1.53f, 0.87f, 5.44f, 100.0f, 2.18f, 0.87f, 5.02f};
            float[] fArray2 = new float[]{1.0f, 4.92f, 30.16f, -0.0f, 33.08f, 4.92f, 25.58f, -0.0f, 100.0f, 4.92f, 25.6f, -0.0f};
            SCE01045.this.BaseCam.transSPL(fArray, 1, 2, 550);
            SCE01045.this.BaseCam.rotateSPL(fArray2, 0, 2, 550);
            SCE01045.this.BaseCam.setFov(31.68f);
            SCE01045.this.BaseCam.change();
        }

        public void cut2() {
            SCE01045.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -1.52f, 0.9f, -0.89f, 390.0f, -1.48f, 0.9f, -0.91f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 12.42f;
            fArray2[2] = 24.2f;
            fArray2[4] = 390.0f;
            fArray2[5] = 12.42f;
            fArray2[6] = 24.2f;
            float[] fArray3 = fArray2;
            SCE01045.this.BaseCam.transSPL(fArray, 1);
            SCE01045.this.BaseCam.rotateSPL(fArray3, 0);
            SCE01045.this.BaseCam.setFov(26.8f);
        }

        public void cut2b() {
            SCE01045.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -2.61f, 0.97f, -0.5f, 500.0f, -2.65f, 0.97f, -0.51f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -3.68f;
            fArray2[2] = -14.06f;
            fArray2[4] = 500.0f;
            fArray2[5] = -3.68f;
            fArray2[6] = -14.06f;
            float[] fArray3 = fArray2;
            SCE01045.this.BaseCam.transSPL(fArray, 0);
            SCE01045.this.BaseCam.rotateSPL(fArray3, 0);
            SCE01045.this.BaseCam.setFov(26.8f);
        }

        public void cut2c() {
            SCE01045.this.tool.Timechk_CutChange();
            SCE01045.this.BaseCam.setTranslate(-1.34f, 1.13f, -2.06f);
            SCE01045.this.BaseCam.setRotate(0.82f, 119.5f, 0.0f);
            SCE01045.this.BaseCam.setFov(26.88f);
        }

        public void cut3() {
            SCE01045.this.tool.Timechk_CutChange();
            SCE01045.this.BaseCam.setTranslate(-1.39f, 1.1f, -2.06f);
            SCE01045.this.BaseCam.setRotate(1.08f, 118.68f, 0.0f);
            SCE01045.this.BaseCam.setFov(26.88f);
            System.sleep(150);
            float[] fArray = new float[]{1.0f, -1.39f, 1.1f, -2.06f, 100.0f, -1.39f, 1.1f, -2.06f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = 1.08f;
            fArray2[2] = 118.68f;
            fArray2[4] = 100.0f;
            fArray2[5] = 12.0f;
            fArray2[6] = 102.96f;
            float[] fArray3 = fArray2;
            SCE01045.this.BaseCam.transSPL(fArray, 0, 3, 100);
            SCE01045.this.BaseCam.rotateSPL(fArray3, 0, 3, 100);
        }

        public void cut4() {
            SCE01045.this.tool.Timechk_CutChange();
            float[] fArray = new float[]{1.0f, -26.91f, 8.36f, 8.66f, 160.0f, -27.08f, 8.36f, 8.12f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -13.2f;
            fArray2[2] = -72.34f;
            fArray2[4] = 160.0f;
            fArray2[5] = -13.2f;
            fArray2[6] = -72.34f;
            float[] fArray3 = fArray2;
            SCE01045.this.BaseCam.transSPL(fArray, 0, 3, 160);
            SCE01045.this.BaseCam.rotateSPL(fArray3, 0, 3, 160);
            SCE01045.this.BaseCam.setFov(26.88f);
        }
    }
}

