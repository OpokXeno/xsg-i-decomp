import xeno.Scene;
import xeno.XenoConstants;
import xeno.plan.EventConstants;
import xeno.util.Input;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;
import xeno.vm.Thread;

class SCEDummy
        extends Scene
        implements Xbufnum,
        XenoConstants,
        EventConstants {
    Window win;
    int menuSelected;
    int selectMenu;
    Input Xpad1P = Input.create(0);
    Thread Xenvmainthread;
    Thread Xenvplaythread;
    boolean Xenvmainthreadendflag = false;
    int Xedge1 = 0;

    SCEDummy() {
    }

    void XenvPad_get() {
        this.Xedge1 = this.Xpad1P.getEdge();
    }

    void Xenvkeywait() {
        this.XenvPad_get();
        while ((this.Xedge1 & 0x20) == 0) {
            this.XenvPad_get();
            System.sleep(1);
        }
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
        this.msg.print("ＳＣＥ０３０３５：仮のものです");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＦＬＡＧ　　：ＥＶ０３０３５＿Ｆ；ＥＶ０３０３６Ａ＿Ｆ；ＥＶ０３０３６Ｂ＿Ｆ；ＥＶ０３０３６Ｃ＿Ｆ；ＥＶ０３０３６Ｄ＿Ｆ；ＥＶ０３０３６Ｅ＿Ｆ；ＥＶ０３０３６Ｆ＿Ｆ；ＥＶ０３０３６Ｇ＿Ｆ；ＥＶ０３０３６Ｈ＿Ｆ；ＥＶ０３０３３Ａ＿Ｆ；");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＪＮＡＭＥ　：ＣＦＪ３＿１６０￥ｎＸＥＶＥＪＰＯＩＮＴ：ＰＯＩＮＴ３＿１６０");
        this.Xenvkeywait();
        System.sleep(1);
    }

    void Xenvplaythread() {
        this.Xenvplaymain();
        this.Xenvmainthreadendflag = true;
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV03035_F;EV03036A_F;EV03036B_F;EV03036C_F;EV03036D_F;EV03036E_F;EV03036F_F;EV03036G_F;EV03036H_F;EV03033A_F;");
        Runtime.setFlags(352, 1, 1);
        Runtime.setFlags(353, 1, 1);
        Runtime.setFlags(354, 1, 1);
        Runtime.setFlags(355, 1, 1);
        Runtime.setFlags(356, 1, 1);
        Runtime.setFlags(357, 1, 1);
        Runtime.setFlags(358, 1, 1);
        Runtime.setFlags(359, 1, 1);
        Runtime.setFlags(360, 1, 1);
        Runtime.setFlags(348, 1, 1);
        System.println("XEVEJNAME:CFJ3_160 XEVEJPOINT:POINT3_160");
        Runtime.jumpCF(1712, 2);
    }

    void init() {
    }

    void initialize() {
    }

    static void main() {
    }

    void play() {
    }
}

