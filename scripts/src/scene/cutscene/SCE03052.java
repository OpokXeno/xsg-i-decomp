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
        this.msg.print("ＳＣＥ０３０５２：仮のものです");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＦＬＡＧ　　：ＥＶ０３０５２＿Ｆ；ＥＶ０３０５３＿Ｆ；ＥＶ０３０５４＿Ｆ；ＥＶ０３０５４Ｂ１＿Ｆ；");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＪＮＡＭＥ　：ＣＦＪ３＿２４０￥ｎＸＥＶＥＪＰＯＩＮＴ：ＰＯＩＮＴ３＿２４０");
        this.Xenvkeywait();
        System.sleep(1);
    }

    void Xenvplaythread() {
        this.Xenvplaymain();
        this.Xenvmainthreadendflag = true;
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV03052_F;EV03053_F;EV03054_F;EV03054B1_F;");
        Runtime.setFlags(386, 1, 1);
        Runtime.setFlags(387, 1, 1);
        Runtime.setFlags(388, 1, 1);
        Runtime.setFlags(389, 1, 1);
        System.println("XEVEJNAME:CFJ3_240 XEVEJPOINT:POINT3_240");
        Runtime.jumpCF(1713, 3);
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

