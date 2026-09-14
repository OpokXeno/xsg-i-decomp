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
        this.msg.print("ＳＣＥ０２０１８：仮のものです");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＦＬＡＧ　　：ＥＶ０２０１８＿Ｆ；ＥＶ０２０１９＿Ｆ；ＥＶ０２０２０＿Ｆ；ＥＶ０２０２１＿Ｆ；ＥＶ０２０２２＿Ｆ；ＥＶ０２０２３＿Ｆ；ＥＶ０２０２４＿Ｆ；ＥＶ０２０２５＿Ｆ；ＥＶ０２０２６＿Ｆ；ＥＶ０２０２７＿Ｆ；ＥＶ０２０２８＿Ｆ；");
        this.Xenvkeywait();
        System.sleep(1);
        this.msg.print("ＸＥＶＥＪＮＡＭＥ　：ＣＦＪ２＿１３０￥ｎＸＥＶＥＪＰＯＩＮＴ：ＰＯＩＮＴ２＿１３０");
        this.Xenvkeywait();
        System.sleep(1);
    }

    void Xenvplaythread() {
        this.Xenvplaymain();
        this.Xenvmainthreadendflag = true;
    }

    public void cleanup() {
        System.println("Event Out");
        System.println("XEVEFLAG:EV02018_F;EV02019_F;EV02020_F;EV02021_F;EV02022_F;EV02023_F;EV02024_F;EV02025_F;EV02026_F;EV02027_F;EV02028_F;");
        Runtime.setFlags(129, 1, 1);
        Runtime.setFlags(130, 1, 1);
        Runtime.setFlags(131, 1, 1);
        Runtime.setFlags(132, 1, 1);
        Runtime.setFlags(133, 1, 1);
        Runtime.setFlags(134, 1, 1);
        Runtime.setFlags(135, 1, 1);
        Runtime.setFlags(136, 1, 1);
        Runtime.setFlags(137, 1, 1);
        Runtime.setFlags(140, 1, 1);
        Runtime.setFlags(141, 1, 1);
        System.println("XEVEJNAME:CFJ2_130 XEVEJPOINT:POINT2_130");
        Runtime.jumpCF(516, 4);
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

