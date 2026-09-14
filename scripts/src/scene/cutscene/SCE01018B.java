import xeno.Camera;
import xeno.Chr;
import xeno.Light;
import xeno.Monitor;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.XenoConstants;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE01018B
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants,
        Pack01018B,
        FLSshion_h {
    public shion shion;
    public pendant pendant;
    STool tool = new STool();
    Camera BaseCam = Camera.create(1);
    Camera cam0;
    Camera cam1;
    Window win;
    int menuSelected;
    int selectMenu;
    Camerawork camerawork = new Camerawork();
    float Location_Z = 60.0f;
    float Location_Y = -1.0f;
    static final int Chand_R = 20;
    static final int Chand_L = 26;
    static final int BGOBJ_monitor = 41;
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    Light light = new Light(0);
    Monitor fm1;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE01018B() {
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
        System.println("XEVEFLAG:EV01018B_F");
        Runtime.setFlags(26, 1, 1);
        System.println("XEVEJNAME:SCE01019");
        Runtime.jumpEvent(1190);
    }

    public void cleanupOriginal() {
        System.println("Event Out");
        Runtime.jumpEvent(1190);
    }

    void init() {
        Runtime.setLocation(20);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
    }

    void initialize() {
    }

    static void main() {
    }

    void play() {
        this.tool.loadarc(this.shion.face, "FLSshion_h.fpk");
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        this.tool.Timechk_SceneStart();
        this.tool.SoundstreamDebug_init(999999);
        Sound.streamPlay(1190037, 48000);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, -0.612f, 0.647f, 0.455f);
        this.light.setColor(2, 0.64f, 0.64f, 0.64f);
        this.light.setDirection2(2, 0.918f, 0.341f, 0.202f);
        this.light.setColor(3, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(3, -0.15f, -0.841f, 0.52f);
        Stage.setColor(0.85f, 0.85f, 0.85f);
        this.shion.start(1, "act1");
        this.pendant.start(1, "act1");
        this.camerawork.cut1();
        System.sleep(150);
        this.shion.start(1, "act1b");
        this.pendant.start(1, "act1b");
        System.sleep(1);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(1, 0.788f, 0.583f, -0.197f);
        this.light.setColor(2, 0.31f, 0.31f, 0.31f);
        this.light.setDirection2(2, 0.757f, 0.239f, 0.608f);
        this.light.setColor(3, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(3, -0.003f, -0.998f, 0.07f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.camerawork.cut1b();
        this.tool.FACE(this.shion.face, 48);
        System.sleep(54);
        this.shion.start(1, "act2");
        this.pendant.start(1, "act2");
        System.sleep(1);
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, -0.636f, 0.001f, -0.772f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.099f, 0.411f, 0.906f);
        this.light.setColor(3, 0.33f, 0.33f, 0.33f);
        this.light.setDirection2(3, -0.424f, -0.511f, 0.748f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.camerawork.cut2();
        System.sleep(20);
        this.tool.SoundstreamPlay(118026);
        this.tool.MSG(40, this.shion.face, 47, 32, "Good night...");
        this.shion.face.mtn(48, 50, 53, 0, 0, 0.2f, false);
        this.shion.face.start(4, null);
        System.sleep(30);
        this.shion.start(1, "act3");
        this.camerawork.cut3();
        this.light.setColor(0, 0.15f, 0.15f, 0.15f);
        this.light.setColor(1, 0.75f, 0.75f, 0.75f);
        this.light.setDirection2(1, -0.401f, 0.644f, -0.652f);
        this.light.setColor(2, 0.41f, 0.41f, 0.41f);
        this.light.setDirection2(2, -0.502f, 0.338f, 0.796f);
        this.light.setColor(3, 0.19f, 0.19f, 0.19f);
        this.light.setDirection2(3, -0.423f, -0.639f, 0.642f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        System.sleep(30);
        int[] nArray = new int[2];
        nArray[1] = 43456;
        int[] nArray2 = nArray;
        Runtime.setDefocus(0, 2, nArray2);
        int[] nArray3 = new int[6];
        nArray3[0] = 1;
        nArray3[1] = -32635392;
        int[] nArray4 = nArray3;
        Runtime.setDefocus(13, 3, nArray4);
        Runtime.setDefocus(14, 3, nArray4);
        Runtime.setDefocus(15, 3, nArray4);
        System.sleep(60);
        this.tool.Timechk_SceneEnd();
    }

    class shion
            extends Chr {
        Chr face;

        shion() {
        }

        void act1() {
            this.setTranslate(1.26f, -0.0f, 2.05f);
            this.setRotate(0.0f, 276.0f, 0.0f);
            this.mtn(257, 0, 150, 0, 8, 1.0f, true);
        }

        void act1b() {
            this.setTranslate(1.26f, 0.0f, 2.06f);
            this.setRotate(0.0f, 276.0f, 0.0f);
            this.mtn(259, 0, 90, 0, 8, 0.3f, true);
        }

        void act2() {
            this.mtn(259, 0, 90, 0, 8, 1.0f, true);
        }

        void act3() {
            this.setTranslate(1.05f, -0.0f, 2.67f);
            this.setRotate(0.0f, 276.0f, 0.0f);
            this.mtn(261, 0, 120, 0, 8, 1.0f, true);
        }

        void init() {
            this.init(0x1000031);
            this.face = this.getChild(0x1000000);
            this.setTranslate(1.26f, -0.0f, 2.05f);
            this.setRotate(0.0f, 276.0f, 0.0f);
        }
    }

    class pendant
            extends Chr {
        pendant() {
        }

        void act1() {
            this.setTranslate(1.26f, 0.04f, 2.05f);
            this.setRotate(0.0f, 276.0f, 0.0f);
            this.mtn(258, 0, 150, 0, 8, 1.0f, true);
        }

        void act1b() {
            this.setTranslate(1.26f, 0.04f, 2.05f);
            this.setRotate(0.0f, 276.0f, 0.0f);
            this.mtn(260, 0, 90, 0, 8, 0.3f, true);
        }

        void act2() {
            this.setTranslate(1.26f, 0.04f, 2.05f);
            this.setRotate(0.0f, 276.0f, 0.0f);
            this.mtn(260, 0, 90, 0, 8, 1.0f, true);
        }

        void init() {
            this.init(24589);
        }
    }

    class Camerawork {
        Camerawork() {
        }

        public void cut1() {
            SCE01018B.this.tool.Timechk_CutChange();
            SCE01018B.this.BaseCam.setTranslate(1.97f, 0.97f, 3.07f);
            SCE01018B.this.BaseCam.setRotate(-7.5f, 45.88f, 0.0f);
            SCE01018B.this.BaseCam.setFov(23.68f);
            SCE01018B.this.BaseCam.change();
        }

        public void cut1b() {
            SCE01018B.this.tool.Timechk_CutChange();
            SCE01018B.this.BaseCam.setTranslate(1.17f, 1.0f, 2.16f);
            SCE01018B.this.BaseCam.setRotate(-16.06f, 40.06f, 0.0f);
            SCE01018B.this.BaseCam.setFov(22.4f);
            float[] fArray = new float[]{1.0f, 24.4f, 54.0f, 22.4f};
            SCE01018B.this.BaseCam.fovSPL(fArray, 0);
        }

        public void cut2() {
            SCE01018B.this.BaseCam.setTranslate(0.64f, 0.97f, 2.99f);
            SCE01018B.this.BaseCam.setRotate(3.38f, -29.48f, 0.0f);
            SCE01018B.this.BaseCam.setFov(23.68f);
        }

        public void cut3() {
            SCE01018B.this.BaseCam.setTranslate(-0.3f, 0.3f, 3.89f);
            SCE01018B.this.BaseCam.setRotate(7.24f, -65.76f, 0.0f);
            SCE01018B.this.BaseCam.setFov(23.68f);
        }
    }
}

