import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Light;
import xeno.Scene;
import xeno.Sound;
import xeno.Stage;
import xeno.XenoConstants;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Spline;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCE02039
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants {
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
    static final int Twohand = 0;
    static final int Rhand = 16;
    static final int Lhand = 32;
    static final int Open = 0;
    static final int Close = 1;
    Light light = new Light(0);
    Chrs mobj001 = new Chrs(20482);
    Effect eft0;
    boolean CameraStart_exec = false;
    Thread camera_thread = Thread.create();
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;

    SCE02039() {
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
        System.println("XEVEFLAG:EV02039_F");
        Runtime.setFlags(155, 1, 1);
        System.println("XEVEJNAME:SCE02040A");
        Runtime.jumpEvent(2400);
    }

    public void cleanupOriginal() {
        Runtime.jumpEvent(2400);
    }

    void init() {
        System.methodSignal(1);
        Runtime.setLocation(1600);
        this.cam0 = Camera.create(0);
        this.cam1 = Camera.create(1);
        this.eft0 = new Effect(1451, 0.0f, 0.0f, 0.0f, 0.0f);
        this.eft0.setScale(0.1f, 0.1f, 0.1f);
        this.eft0.setCaster(this.mobj001);
        this.eft0.disp(true);
        this.eft0.setTranslate(0.0f, -0.03f, -0.06f);
        this.eft0.setScale(0.1f, 0.1f, 0.1f);
    }

    void initialize() {
    }

    static void main() {
    }

    void play() {
        this.tool.CameraTool();
        this.tool.CaptureTool();
        this.tool.Timechk();
        Runtime.setDefocusQuick(0, 0, 0, 0);
        Runtime.setDefocusQuick(1, 0, 0, 0);
        Runtime.setDefocusQuick(2, 0, 0, 0);
        Runtime.setDefocusQuick(3, 0, 0, 0);
        Sound.streamPlay(1290034, 48000);
        this.light.setColor(0, 0.25f, 0.25f, 0.25f);
        this.light.setColor(1, 0.8f, 0.8f, 0.8f);
        this.light.setDirection2(1, 0.494f, 0.249f, 0.833f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.676f, 0.707f, -0.207f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, -0.544f, -0.369f, -0.753f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.mobj001.start(1, "mobj001act2");
        this.mobj001.setScale(100.0f, 100.0f, 100.0f);
        this.CameraStart("cut1");
        System.sleep(240);
        this.mobj001.start(1, "mobj001act2");
        this.mobj001.setScale(100.0f, 100.0f, 100.0f);
        this.CameraStart("cut2");
        System.sleep(240);
        this.tool.Timechk_SceneEnd();
    }

    class Camerawork
            extends Camera {
        Camerawork() {
        }

        void cut1() {
            Runtime.setDefocusQuick(0, 1, 70880, 1);
            Runtime.setDefocusQuick(1, 1, 62688, 1);
            SCE02039.this.BaseCam.setTranslate(10.97f, 9.41f, 119.46f);
            SCE02039.this.BaseCam.setRotate(-10.19f, -54.36f, 0.0f);
            SCE02039.this.BaseCam.setFov(49.04f);
            SCE02039.this.BaseCam.change();
            float[] fArray = new float[]{1.0f, 10.97f, 9.41f, 119.46f, 180.0f, 10.97f, 9.41f, 119.46f};
            float[] fArray2 = new float[8];
            fArray2[0] = 1.0f;
            fArray2[1] = -10.19f;
            fArray2[2] = -54.36f;
            fArray2[4] = 180.0f;
            fArray2[5] = -10.19f;
            fArray2[6] = -16.85f;
            float[] fArray3 = fArray2;
            SCE02039.this.BaseCam.transSPL(fArray, 0, 1, 180);
            SCE02039.this.BaseCam.rotateSPL(fArray3, 0, 1, 180);
            SCE02039.this.BaseCam.setFov(49.04f);
            SCE02039.this.tool.STCameraPRO_on();
        }

        void cut2() {
            SCE02039.this.tool.STCamera_off();
            SCE02039.this.BaseCam.setTranslate(3.24f, 3.27f, -10.32f);
            SCE02039.this.BaseCam.setRotate(-2.53f, -131.95f, 0.0f);
            SCE02039.this.BaseCam.setFov(49.04f);
        }
    }

    class Chrs
            extends Chr {
        Spline posSPL = Spline.create();

        public Chrs(int n) {
            this.init(n);
            this.setShadow(0, 0);
        }

        void mobj001act1() {
            this.setRotate(0.0f, 180.0f, 0.0f);
            float[] fArray = new float[]{1.0f, 27.8f, -2.4f, 31.9f, 240.0f, 27.8f, -2.4f, 24.3f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 240);
            this.move(this.posSPL, 0, true);
        }

        void mobj001act2() {
            this.setRotate(0.0f, 180.0f, 0.0f);
            float[] fArray = new float[]{1.0f, 40.4f, -0.3f, 67.3f, 300.0f, 40.4f, -0.3f, 25.4f};
            this.posSPL.setCtrlVertex(fArray, 0, 2, 300);
            this.move(this.posSPL, 0, true);
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
    }
}

