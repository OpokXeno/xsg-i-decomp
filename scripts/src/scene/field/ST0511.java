import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_ELS01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0511
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_ELS01_PRJ {
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
    Enepc npc9;
    Enepc npc10;
    Enepc npc11;
    Enepc npc20;
    Enepc npc21;
    Unit unit1;
    Unit Star_1;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
    int touchFlag1;
    int touchFlag2;
    int S2009;
    int S2013;
    int S2013B;
    int S2014;
    int S2014B;
    int S2028;
    int MOMO;
    int ZIGGY;
    int S2030;
    int S2040C;
    int S2042;
    int S2043;
    int S2057;
    MAPUnit moni_00;
    MAPUnit doa20;
    MAPUnit doa21;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    int page;
    String[] msg05000001 = new String[]{"/[label(Matthews)]", "What? You want to go back to the Dock Colony?", "/[waitkey(1)]/[clear()]", "Well, I'll take you anywhere as long as I get paid.", "/[waitkey(64)]/[close()]"};
    String[] msgikude = new String[]{"/[label(Matthews)]", "Tony! Get the Elsa ready for launch. We're going back to the Dock Colony!", "/[waitkey(64)]/[close()]"};
    String[] msgikude2 = new String[]{"/[label(Matthews)]", "Tony! We're launching the Elsa. Hurry up!", "/[waitkey(64)]/[close()]"};
    String[] msghayosei = new String[]{"/[label(Matthews)]", "What? Not going?", "/[waitkey(1)]/[clear()]", "Man, don't come talk to me if you've got no business. We're getting irritated here without any decent work.", "/[waitkey(64)]/[close()]"};
    String[] msg05000002 = new String[]{"/[label(Tony)]", "What? Forget to buy something?", "/[waitkey(1)]/[clear()]", "Hurry up, okay? The Captain looks like he's ready to go on a rampage at anytime.", "/[waitkey(64)]/[close()]"};
    String[] msg001 = new String[]{"/[label(Matthews)]", "What? Something happen?", "/[waitkey(1)]/[clear()]", "MOMO? No, she hasn't come this way. Maybe she's\ntaking a walk around the Durandal?", "/[waitkey(64)]/[close()]"};
    String[] msg002 = new String[]{"/[label(Matthews)]", "Yo, it's rare to see you by yourself. Where's everyone else?", "/[waitkey(64)]/[close()]"};
    String[] msg003 = new String[]{"/[label(Matthews)]", "Hey! I can't believe you guys are safe!", "/[waitkey(1)]/[clear()]", "KOS-MOS? Oh, the maintenance room is still okay.", "/[waitkey(64)]/[close()]"};
    String[] msg004 = new String[]{"/[label(Matthews)]", "Hey! Don't tell me you're going to go out?! It's full of Gnosis out there! This is no time for taking a leisurely cruise!", "/[waitkey(1)]/[clear()]", "All right, all right, I guess common sense doesn't work on you. So? Where do you want to go?", "/[waitkey(64)]/[close()]"};
    String[] msg005 = new String[]{"/[label(Matthews)]", "Going to the Song of Nephilim, right?", "/[waitkey(1)]/[clear()]", "Just so you know, I don't intend to leave the Song of Nephilim until we've rescued MOMO. You better get ready!", "/[waitkey(64)]/[close()]"};
    String[] msg0051 = new String[]{"/[label(Matthews)]", "Man, I can't believe I'm stuck doing this. You better fork over a lot of hazard pay!", "/[waitkey(64)]/[close()]"};
    String[] msg0052 = new String[]{"/[label(Matthews)]", "What? Aren't you ready yet!? We don't have time. Hurry up!", "/[waitkey(64)]/[close()]"};
    String[] msg006 = new String[]{"/[label(Matthews)]", "What's wrong? Did you rescue MOMO?", "/[waitkey(1)]/[clear()]", "I'll wait here for you, so hurry up and go rescue her!", "/[waitkey(64)]/[close()]"};
    String[] msg0062 = new String[]{"/[label(Matthews)]", "Hey, just because you rescued MOMO doesn't mean we can relax. We can't just leave the Song of Nephilim here as it is, can we?", "/[waitkey(1)]/[clear()]", "I'll wait here for you, so get back in there and finish it off!", "/[waitkey(64)]/[close()]"};
    String[] msg007 = new String[]{"/[label(Matthews)]", "Hey, you just gonna leave that Albedo guy there?! You're certainly easygoing considering the critical situation we're in!", "/[waitkey(1)]/[clear()]", "So? Where do you want to go? Hurry up and tell me.", "/[waitkey(64)]/[close()]"};
    String[] msg0073 = new String[]{"/[label(Matthews)]", "We're ready to go anytime. I could take you to the Proto Merkabah right now if you want.", "/[waitkey(64)]/[close()]"};
    String[] msg0072 = new String[]{"/[label(Matthews)]", "All right, but I ain't taking any responsibility!", "/[waitkey(64)]/[close()]"};
    String[] msg008 = new String[]{"/[label(Matthews)]", "What is it? You want to go back to the Durandal?", "/[waitkey(64)]/[close()]"};
    String[] msgNOTGO = new String[]{"/[label(Matthews)]", "Hey, now, don't tell me you want to go somewhere else. Cause too bad for you, but we won't be able to go anywhere until we reach the Kukai Foundation.", "/[waitkey(64)]/[close()]"};
    String[] msg0071 = new String[]{"/[label(Matthews)]", "What? Not going? Then hurry up and go kick that Albedo guy's butt!", "/[waitkey(64)]/[close()]"};
    String[] msgtalk_no = new String[]{"If this shows up, it's a mistake!", "/[waitkey(64)]/[close()]"};
    String[] msgT001 = new String[]{"/[label(Tony)]", "What's wrong?", "/[waitkey(1)]/[clear()]", "Don't worry, they'll take you to the Kukai Foundation. But of course, if I was piloting the Durandal, we'd already be there by now.", "/[waitkey(64)]/[close()]"};
    String[] msgT002 = new String[]{"/[label(Tony)]", "You want to go somewhere? Then tell the Captain. If I left port without permission, Little Master would make quite a stink.", "/[waitkey(64)]/[close()]"};
    String[] msgT0021 = new String[]{"/[label(Tony)]", "If you want to go somewhere, tell the Captain. I can't leave port without his permission, even by your orders, Little Master.", "/[waitkey(64)]/[close()]"};
    String[] msgT003 = new String[]{"/[label(Tony)]", "MOMO's not around? Dunno, haven't seen her.", "/[waitkey(64)]/[close()]"};
    String[] msgT004 = new String[]{"/[label(Tony)]", "What? You going off to play? Man, lucky you! I can't leave the Elsa since we're on standby.", "/[waitkey(64)]/[close()]"};
    String[] msgT005 = new String[]{"/[label(Tony)]", "Yo, looks like you're safe. Well, I figured you'd manage somehow. Try to make those hardheaded fools understand as fast as you can, okay?", "/[waitkey(64)]/[close()]"};
    String[] msgT006 = new String[]{"/[label(Tony)]", "It's turning really nasty out there. We should be safe in here for now, I think. Well, if things get dangerous, I'm gonna bust out of here even if I have to break\nthrough the bulkhead.", "/[waitkey(64)]/[close()]"};
    String[] msgT007 = new String[]{"/[label(Tony)]", "I hear MOMO's in trouble. I'll take you anywhere you want.", "/[waitkey(64)]/[close()]"};
    String[] msgT008 = new String[]{"/[label(Tony)]", "Did you find MOMO? Don't you worry. I won't abandon you guys and run away, no matter what happens.", "/[waitkey(64)]/[close()]"};
    String[] msgT0081 = new String[]{"/[label(Tony)]", "You don't have to worry. I won't abandon you guys and run away, no matter what happens.", "/[waitkey(64)]/[close()]"};
    String[] msgT009 = new String[]{"/[label(Tony)]", "Leave it to me! There's nowhere the Elsa can't go. I'm with you all the way now.", "/[waitkey(64)]/[close()]"};
    String[] msgH001 = new String[]{"/[label(Hammer)]", "What's the matter? Oh, did you get lonely from not seeing me? Don't worry, the Elsa's currently under standby orders, so you can come see me anytime.", "/[waitkey(64)]/[close()]"};
    String[] msgH002 = new String[]{"/[label(Hammer)]", "What's up? You want to go somewhere? Then you'd better tell the Captain. Oh, but make sure you don't get ripped off.", "/[waitkey(64)]/[close()]"};
    String[] msgH003 = new String[]{"/[label(Hammer)]", "MOMO? Haven't seen her. At times like this, you could locate her right away by asking MOMO. But without MOMO around, I dunno what to do.", "/[waitkey(64)]/[close()]"};
    String[] msgH004 = new String[]{"/[label(Hammer)]", "Oh? What are you doing alone? Allen? He's not here. He's probably eating his frustration away again.", "/[waitkey(64)]/[close()]"};
    String[] msgH005 = new String[]{"/[label(Hammer)]", "Oh, you're safe! I'm so glad. It was a close call for the Elsa, too, but we locked all the hatches before the soldiers got in. Then again, our situation now is the\nsame as having been captured.", "/[waitkey(64)]/[close()]"};
    String[] msgH006 = new String[]{"/[label(Hammer)]", "We're in trouble, big trouble! The citizens, the Foundation, and the Gnosis are in a huge bind! Ahh, I don't know what's what!", "/[waitkey(64)]/[close()]"};
    String[] msgH007 = new String[]{"/[label(Hammer)]", "I heard. MOMO's in big trouble, right?! Don't worry, I'm ready. I'll go through a lake of fire for MOMO! Then again, we're out in space, aren't we?", "/[waitkey(64)]/[close()]"};
    String[] msgH008 = new String[]{"/[label(Hammer)]", "Well? Have you found MOMO? Don't give up. Keep at it!!", "/[waitkey(64)]/[close()]"};
    String[] msgH0081 = new String[]{"/[label(Hammer)]", "All that's left is to stop the Song of Nephilim, right?! Don't give up. Keep at it!!", "/[waitkey(64)]/[close()]"};
    String[] msgH009 = new String[]{"/[label(Hammer)]", "Sure, I'm ready. I couldn't ask for anything more than to be able to save the world! We'll be heroes if we defeat him!", "/[waitkey(64)]/[close()]"};
    String[] msgC001 = new String[]{"/[label(Hammer)]", "Oh? What's the matter? Weren't you resting in your room?", "/[waitkey(1)]/[clear()]", "Allen? I think I saw him heading toward the Foundation just now.", "/[waitkey(64)]/[close()]"};
    String[] msgA001 = new String[]{"/[label(Allen)]", "I'll just get in the way, so I'm not going to go with you. Please rescue MOMO though, all right? You have to!", "/[waitkey(64)]/[close()]"};
    String[] msgA002 = new String[]{"/[label(Allen)]", "Don't worry, I'll stay here and watch the Captain and the others to make sure they don't split. Please don't worry about that and concentrate on rescuing MOMO.", "/[waitkey(64)]/[close()]"};
    String[] msgA003 = new String[]{"/[label(Allen)]", "Leave the fort to me. You know backup support is my forte. If you're going to do it, do it to the end, okay? Doing something halfway is the worst thing you could do!", "/[waitkey(64)]/[close()]"};

    ST0511() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.061f, 3.277f, -11.353f);
        this.camEV.setRotate(-13.158f, -181.279f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            default:
        }
    }

    void Talk_no(Window window) {
        window.print(this.msgtalk_no, 0);
        ST0511.waitPage(window, 64);
    }

    public void Talk_npc1(Enepc enepc) {
        if (Runtime.getFlags(389, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK2();
            } else if (Runtime.getFlags(7088, 1) == 1) {
                this.Talk_npc1_TEN();
            } else {
                this.Talk_npc1_8();
            }
        } else if (Runtime.getFlags(375, 1) == 1) {
            this.Talk_npc1_72();
        } else if (Runtime.getFlags(374, 1) == 1) {
            this.Talk_npc1_7();
        } else if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc1_6();
        } else if (Runtime.getFlags(362, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_5();
            }
        } else if (Runtime.getFlags(360, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_5();
            }
        } else if (Runtime.getFlags(346, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_1();
            }
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.Talk_npc1_4();
        } else if (Runtime.getFlags(314, 1) == 1) {
            if (Runtime.getFlags(7086, 1) == 1) {
                this.Talk_npc1_DOCK();
            } else {
                this.Talk_npc1_1();
            }
        } else if (Runtime.getFlags(7162, 1) == 1) {
            this.Talk_npc1_1();
        } else if (Runtime.getFlags(310, 1) == 1) {
            this.Talk_npc1_3();
        } else if (Runtime.getFlags(304, 1) == 1) {
            this.Talk_npc1_0();
        } else if (Runtime.getFlags(303, 1) == 1) {
            this.Talk_npc1_2();
        } else if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_npc1_0();
        } else {
            this.Talk_npc1_0();
        }
    }

    void Talk_npc1_0() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgNOTGO, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_1() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg05000001, 0);
        ST0511.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes, please\nNo, not right now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(33);
                Runtime.setFlags(7086, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9051, 1);
                this.cam0.setMode(0);
                this.npc1.kickEnepc(9, -1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_2() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg001, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_3() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg002, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_4() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg003, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_5() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg004, 0);
        ST0511.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("I want to go to the Dock Colony\nNo, not right now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(33);
                Runtime.setFlags(7086, 1, 1);
                Runtime.jumpCF(9051, 1);
                this.cam0.setMode(0);
                this.npc1.kickEnepc(9, -1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_52() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg004, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_6() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg005, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Go to the Song of Nephilim\nGo to the Dock Colony\nStay here");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0051, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7087, 1, 1);
                Runtime.setFlags(374, 1, 1);
                Runtime.jumpEvent(3461);
                return;
            }
            case 1: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0052, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9051, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0052, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_7() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg006, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_72() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0062, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_8() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0073, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Go to Proto Merkabah\nI want to go to the Dock Colony\nNo, not right now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                if (Runtime.getFlags(390, 1) == 0) {
                    this.npc1.kickEnepc(9, -1);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgikude2, 0);
                    ST0511.waitPage(this.win, 64);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setFlags(7088, 1, 1);
                    Runtime.setFlags(390, 1, 1);
                    System.println("フラグオン！");
                    Runtime.jumpEvent(3543);
                    return;
                }
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude2, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7088, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9055, 1);
            }
            case 1: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0072, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9051, 1);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0072, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_DOCK() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg008, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes, please\nNo, not yet");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude2, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 0);
                if (Runtime.getFlags(7131, 2) == 1) {
                    Runtime.setFlags(7131, 2, 2);
                } else if (Runtime.getFlags(7131, 2) == 2) {
                    Runtime.setFlags(7131, 2, 3);
                }
                System.println("フラグオン！");
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(9053, 1);
                } else if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(9054, 1);
                } else {
                    Runtime.jumpCF(9052, 1);
                }
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msghayosei, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_DOCK2() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0073, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Go to Proto Merkabah\nI want to go to the Durandal\nNo, not right now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                if (Runtime.getFlags(390, 1) == 0) {
                    this.npc1.kickEnepc(9, -1);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.msgikude2, 0);
                    ST0511.waitPage(this.win, 64);
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setFlags(7086, 1, 0);
                    Runtime.setFlags(7088, 1, 1);
                    Runtime.setFlags(390, 1, 1);
                    if (Runtime.getFlags(7131, 2) == 1) {
                        Runtime.setFlags(7131, 2, 2);
                    } else if (Runtime.getFlags(7131, 2) == 2) {
                        Runtime.setFlags(7131, 2, 3);
                    }
                    System.println("フラグオン！");
                    Runtime.jumpEvent(3543);
                    return;
                }
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgikude2, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 0);
                Runtime.setFlags(7088, 1, 1);
                System.println("フラグオン！");
                if (Runtime.getFlags(7131, 2) == 1) {
                    Runtime.setFlags(7131, 2, 2);
                } else if (Runtime.getFlags(7131, 2) == 2) {
                    Runtime.setFlags(7131, 2, 3);
                }
                Runtime.jumpCF(9055, 1);
            }
            case 1: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0072, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7086, 1, 0);
                if (Runtime.getFlags(7131, 2) == 1) {
                    Runtime.setFlags(7131, 2, 2);
                } else if (Runtime.getFlags(7131, 2) == 2) {
                    Runtime.setFlags(7131, 2, 3);
                }
                System.println("フラグオン！");
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(9053, 1);
                } else if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(9054, 1);
                } else {
                    Runtime.jumpCF(9052, 1);
                }
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0072, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    void Talk_npc1_TEN() {
        this.npc1.kickEnepc(9, 100);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg007, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("I want to go to the Dock Colony\nI want to go to the Durandal\nNo, not right now");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0072, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7088, 1, 0);
                Runtime.setFlags(7086, 1, 1);
                System.println("フラグオン！");
                Runtime.jumpCF(9051, 1);
                return;
            }
            case 1: {
                this.npc1.kickEnepc(9, -1);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0072, 0);
                ST0511.waitPage(this.win, 64);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(7088, 1, 0);
                System.println("フラグオン！");
                if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                    Runtime.jumpCF(9053, 1);
                } else if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                    Runtime.jumpCF(9054, 1);
                } else {
                    Runtime.jumpCF(9052, 1);
                }
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0071, 0);
        ST0511.waitPage(this.win, 64);
        this.npc1.kickEnepc(9, -1);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (Runtime.getFlags(389, 1) == 1) {
            this.Talk_npc2_8(window);
        } else if (Runtime.getFlags(375, 1) == 1) {
            this.Talk_npc2_72(window);
        } else if (Runtime.getFlags(374, 1) == 1) {
            this.Talk_npc2_7(window);
        } else if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc2_6(window);
        } else if (Runtime.getFlags(362, 1) == 1) {
            this.Talk_npc2_52(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc2_5(window);
        } else if (Runtime.getFlags(346, 1) == 1) {
            this.Talk_npc2_1(window);
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.Talk_npc2_4(window);
        } else if (Runtime.getFlags(314, 1) == 1) {
            this.Talk_npc2_1(window);
        } else if (Runtime.getFlags(7162, 1) == 1) {
            this.Talk_npc2_1(window);
        } else if (Runtime.getFlags(310, 1) == 1) {
            this.Talk_npc2_3(window);
        } else if (Runtime.getFlags(304, 1) == 1) {
            this.Talk_npc2_0(window);
        } else if (Runtime.getFlags(303, 1) == 1) {
            this.Talk_npc2_2(window);
        } else if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_npc2_0(window);
        } else {
            this.Talk_npc2_0(window);
        }
    }

    void Talk_npc2_0(Window window) {
        window.print(this.msgT001, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_1(Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.msgT0021, 0);
            ST0511.waitPage(window, 64);
        } else {
            window.print(this.msgT002, 0);
            ST0511.waitPage(window, 64);
        }
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msgT003, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
        window.print(this.msgT004, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_4(Window window) {
        window.print(this.msgT005, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_5(Window window) {
        window.print(this.msgT006, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_52(Window window) {
        window.print(this.msgT006, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_6(Window window) {
        window.print(this.msgT007, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_7(Window window) {
        window.print(this.msgT008, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_72(Window window) {
        window.print(this.msgT0081, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc2_8(Window window) {
        window.print(this.msgT009, 0);
        ST0511.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (Runtime.getFlags(389, 1) == 1) {
            this.Talk_npc3_8(window);
        } else if (Runtime.getFlags(375, 1) == 1) {
            this.Talk_npc3_72(window);
        } else if (Runtime.getFlags(374, 1) == 1) {
            this.Talk_npc3_7(window);
        } else if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc3_6(window);
        } else if (Runtime.getFlags(362, 1) == 1) {
            this.Talk_npc3_52(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc3_5(window);
        } else if (Runtime.getFlags(346, 1) == 1) {
            this.Talk_npc3_1(window);
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.Talk_npc3_4(window);
        } else if (Runtime.getFlags(314, 1) == 1) {
            this.Talk_npc3_1(window);
        } else if (Runtime.getFlags(7162, 1) == 1) {
            this.Talk_npc3_1(window);
        } else if (Runtime.getFlags(310, 1) == 1) {
            this.Talk_npc3_3(window);
        } else if (Runtime.getFlags(304, 1) == 1) {
            this.Talk_npc3_0(window);
        } else if (Runtime.getFlags(303, 1) == 1) {
            this.Talk_npc3_2(window);
        } else if (Runtime.getFlags(301, 1) == 1) {
            this.Talk_npc3_0(window);
        } else {
            this.Talk_npc3_0(window);
        }
    }

    void Talk_npc3_0(Window window) {
        window.print(this.msgH001, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msgH002, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msgH003, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        window.print(this.msgH004, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_4(Window window) {
        window.print(this.msgH005, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_5(Window window) {
        window.print(this.msgH006, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_52(Window window) {
        window.print(this.msgH006, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_6(Window window) {
        window.print(this.msgH007, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_7(Window window) {
        window.print(this.msgH008, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_72(Window window) {
        window.print(this.msgH0081, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc3_8(Window window) {
        window.print(this.msgH009, 0);
        ST0511.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (Runtime.getFlags(346, 1) == 1) {
            this.Talk_npc4_0(window);
        } else if (Runtime.getFlags(346, 1) == 1) {
            this.Talk_npc4_0(window);
        } else {
            this.Talk_npc4_0(window);
        }
    }

    void Talk_npc4_0(Window window) {
        window.print(this.msgC001, 0);
        ST0511.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (Runtime.getFlags(389, 1) == 1) {
            this.Talk_npc5_3(window);
        } else if (Runtime.getFlags(374, 1) == 1) {
            this.Talk_npc5_2(window);
        } else if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc5_1(window);
        } else {
            this.Talk_npc5_0(window);
        }
    }

    void Talk_npc5_0(Window window) {
        window.print(this.msgA001, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msgA001, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msgA002, 0);
        ST0511.waitPage(window, 64);
    }

    void Talk_npc5_3(Window window) {
        window.print(this.msgA003, 0);
        ST0511.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(521, 1);
                break;
            }
        }
    }

    void init() {
        this.S2009 = Runtime.getFlags(115, 1);
        this.S2013 = Runtime.getFlags(122, 1);
        this.S2013B = Runtime.getFlags(123, 1);
        this.S2014 = Runtime.getFlags(124, 1);
        this.S2014B = Runtime.getFlags(125, 1);
        this.S2028 = Runtime.getFlags(141, 1);
        this.MOMO = Runtime.getFlags(7014, 1);
        this.S2030 = Runtime.getFlags(143, 1);
        this.S2040C = Runtime.getFlags(158, 1);
        this.ZIGGY = Runtime.getFlags(7015, 1);
        this.S2042 = Runtime.getFlags(162, 1);
        this.S2057 = Runtime.getFlags(179, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.teiten1 = new Uwamono(28690, 0.0f, -1.35f, -2.6f, 0.0f);
        this.teiten1.SetBgm(196609);
        this.teiten2 = new Uwamono(28690, 0.0f, -0.5f, 0.0f, 0.0f);
        this.teiten2.SetBgm(196609);
        this.teiten3 = new Uwamono(28690, -3.3f, -0.5f, 1.2f, 0.0f);
        this.teiten3.SetBgm(196610);
        this.teiten4 = new Uwamono(28690, 3.3f, -0.5f, 1.2f, 0.0f);
        this.teiten4.SetBgm(196610);
        this.teiten5 = new Uwamono(28690, 3.0f, 0.0f, 6.0f, 0.0f);
        this.teiten5.SetBgm(196610);
        this.teiten6 = new Uwamono(28690, -3.0f, 0.0f, 7.0f, 0.0f);
        this.teiten6.SetBgm(196611);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 6.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, -10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 10.0f, 0.0f, 10.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.fadeOut = new Effect(0);
        this.fadeOut.args[0] = Integer.MIN_VALUE;
        this.fadeOut.args[1] = 20;
        this.fadeOut.args[2] = 1;
        this.fadeIn = new Effect(0);
        this.fadeIn.args[0] = Integer.MIN_VALUE;
        this.fadeIn.args[1] = 20;
        this.fadeIn.args[2] = 0;
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.doorA = new Uwamono(4, 40, '\u0001');
        new Uwamono(5, 40, '\u0001', this.doorA);
        this.doorA.SetDoorType('\u0004');
        if (Runtime.getFlags(7088, 1) == 1) {
            Stage.setVisible(68, false);
            Stage.setVisible(6, false);
        } else if (Runtime.getFlags(388, 1) == 1) {
            Stage.setVisible(68, false);
            Stage.setVisible(6, false);
            this.moni_00 = new Mapunit();
            this.moni_00.init(65);
            this.moni_00.start(4, null);
            this.moni_00.setTranslate(0.0f, -300.0f, 0.0f);
            this.doa20 = new Mapunit();
            this.doa20.init(66);
            this.doa20.start(4, null);
            this.doa20.setTranslate(0.0f, -300.0f, 0.0f);
            this.doa21 = new Mapunit();
            this.doa21.init(67);
            this.doa21.start(4, null);
            this.doa21.setTranslate(0.0f, -300.0f, 0.0f);
        } else if (Runtime.getFlags(374, 1) != 1) {
            Stage.setVisible(68, false);
            Stage.setVisible(6, false);
            this.moni_00 = new Mapunit();
            this.moni_00.init(65);
            this.moni_00.start(4, null);
            this.moni_00.setTranslate(0.0f, -300.0f, 0.0f);
            this.doa20 = new Mapunit();
            this.doa20.init(66);
            this.doa20.start(4, null);
            this.doa20.setTranslate(0.0f, -300.0f, 0.0f);
            this.doa21 = new Mapunit();
            this.doa21.init(67);
            this.doa21.start(4, null);
            this.doa21.setTranslate(0.0f, -300.0f, 0.0f);
        }
        if (Runtime.getFlags(389, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(374, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(373, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(346, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(326, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(315, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(314, 1) == 1) {
            this.npcset_2();
        } else if (Runtime.getFlags(310, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(304, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(303, 1) == 1) {
            this.npcset_1();
        } else if (Runtime.getFlags(301, 1) == 1) {
            this.npcset_1();
        } else {
            this.npcset_0();
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(275, 1, 0, 14, 3, -2.02f, -0.5f, 1.66f, 180.0f);
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc1.disableDTKFlag(3);
        this.npc1.enableDTKFlag(4);
        this.npc1.setMotion(0, 10);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
    }

    void npcset_2() {
        this.npc2 = new NPC_NORMAL(277, 2, 0, 76, 16, 0.007f, -1.1f, -3.65f, 180.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setInvalidID(1);
        this.npc2.setMotion(0, 13);
        this.npc2.talkto("Talk_npc2");
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

    class Mapunit
            extends MAPUnit {
        Mapunit() {
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

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
        }
    }
}

