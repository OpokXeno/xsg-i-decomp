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
        this.msg.print("ＳＣＥ０１０２２：仮のものです");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＦＬＡＧ　　：ＥＶ０１０２２＿Ｆ；ＥＶ０１０２３＿Ｆ；ＥＶ０１０２４＿Ｆ；ＥＶ０１０２５＿Ｆ；ＥＶ０１０２６＿Ｆ；ＥＶ０１０２７＿Ｆ；");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＪＮＡＭＥ　：ＣＦＪ１＿１４０￥ｎＸＥＶＥＪＰＯＩＮＴ：ＰＯＩＮＴ＿１４０");
        this.Xenvkeywait();
        System.sleep(1);
    }

    void Xenvplaythread() {
        this.Xenvplaymain();
        this.Xenvmainthreadendflag = true;
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV01022_F;EV01023_F;EV01024_F;EV01025_F;EV01026_F;EV01027_F;");
        Runtime.setFlags(30, 1, 1);
        Runtime.setFlags(31, 1, 1);
        Runtime.setFlags(32, 1, 1);
        Runtime.setFlags(33, 1, 1);
        Runtime.setFlags(34, 1, 1);
        Runtime.setFlags(35, 1, 1);
        System.println("XEVEJNAME:CFJ1_140 XEVEJPOINT:POINT_140");
        Runtime.jumpCF(320, 2);
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

