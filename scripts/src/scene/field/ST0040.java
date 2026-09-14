import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK04_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0040
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK04_PRJ {
    Player player;
    Camera cam0;
    Camera cam1;
    Camera cam2;
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
    Enepc npc12;
    Enepc npc13;
    Enepc npc14;
    Enepc npc15;
    Enepc npc16;
    Enepc npc20;
    Unit[] unit;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int entrance;
    int BUTTON_F = 0;
    int CloseAfter = 0;
    int talkFlag1;
    int talkFlag2;
    int talkFlag3;
    int talkFlag4;
    int talkFlag5;
    int talkFlag6;
    int talkFlag7;
    int talkFlag8;
    int talkFlag9;
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    int HelpFlag1 = 0;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int sw01 = 0;
    int selected0 = 0;
    int selected1 = 0;
    int selected2 = 0;
    int exit0 = 0;
    Uwamono door01;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono doorH;
    Uwamono doorI;
    Uwamono doorJ;
    Uwamono doorK;
    Uwamono trap2;
    Uwamono ueki;
    Uwamono obj01;
    boolean npc1flg = false;
    boolean npc2flg = false;
    int S07A1;
    int S07A2;
    int S07A3;
    int S07C;
    int S010;
    int S011;
    int S012;
    int S013;
    int S015B;
    int SHION_TALK_1;
    int SHION_TALK_2;
    int SHION_TALK_3;
    int SHION_ROOM;
    Unit Crane;
    Unit monitor1;
    Unit monitor2;
    Unit batu;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Effect EF01;
    Effect EF02;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    int smd = 0;
    int page;
    String[] msg1513578D = new String[]{"/[label(Nobutake)]", "I am Petty Officer Nobutake. I apologize for any inconvenience caused by the construction on the path to the Vector sector. Please bear with us.", "/[waitkey(64)]/[close()]"};
    String[] msg1513578D1 = new String[]{"/[label(Nobutake)]", "In accordance with the C.R. orders, we are exchanging the high-energy sealed nanomaterial. It is safe as long as there is no activation signal. Of course, we are still working with explosives here, so please be careful.", "/[waitkey(64)]/[close()]"};
    String[] msg1513B739 = new String[]{"/[label(Nobutake)]", "Essentially, this is like food for the nanomachines that automatically repair minor damage to the ship. The stabilizer material for repairs is one thing, but the\nhigh molecular weight compound that the nanomachines use for fuel is unstable and dangerous.", "/[waitkey(64)]/[close()]"};
    String[] msg0051D1D6 = new String[]{"/[label()]", "It's going to blow up all sorts of things...heeheehee.", "/[waitkey(64)]/[close()]"};
    String[] msg02E896E4 = new String[]{"/[label()]", "W-wait a minute. You're from Vector, aren't you? You're a scientist, right? My colleague's in the back. Could you please try reasoning with him? He won't come out.\nHe's just been cooped up.", "/[waitkey(64)]/[close()]"};
    String[] msg02E8F691 = new String[]{"/[label()]", "Please. You don't have to say anything technical. Just listen to what he has to say. If he can get a scientist or a higher ranking official to listen, I think he'll calm\ndown. Please.", "/[waitkey(64)]/[close()]"};
    String[] msg02E979F6 = new String[]{"/[label()]", "T-thanks for listening to what he had to say.", "/[waitkey(64)]/[close()]"};
    String[] msg02E979F7 = new String[]{"/[label()]", "Oh, thanks. But it would have been even better if you could have listened more enthusiastically.", "/[waitkey(64)]/[close()]"};
    String[] msg0105EF11 = new String[]{"/[label()]", "Did you know?", "/[waitkey(1)]/[clear()]", "Several workers lost their lives during the retrieval. I saw it. When they touched that thing, they vanished instantaneously. Is it really okay to transport\nsomething so dangerous out in the open?", "/[waitkey(64)]/[close()]"};
    String[] msg01064EBD = new String[]{"/[label()]", "Those higher-ups probably care more about that tombstone than our lives. But if they're going to abuse us like that, the least they could do is tell us a lie\nthat would put our worries to rest.", "/[waitkey(1)]/[clear()]", "They won't say anything about this mission. It's like telling us to be afraid.", "/[waitkey(64)]/[close()]"};
    String[] msg0106D222 = new String[]{"/[label()]", "Thanks for listening to my griping. Hey, open the containers around here when you get a chance. If you find something, go ahead and take it.", "/[waitkey(64)]/[close()]"};
    String[] msg0106D223 = new String[]{"/[label()]", "Sorry. Getting that off my chest really helped.", "/[waitkey(64)]/[close()]"};
    String[] msg0106D200 = new String[]{"/[label()]", "Stay away from me. Just leave me alone, okay?", "/[waitkey(64)]/[close()]"};
    String[] msg015EDE82 = new String[]{"/[label()]", "Oh, Chief Uzuki, Lieutenant Caspase wants to talk to you about tuning-up the Realians.", "/[waitkey(1)]/[clear()]", "I hear the Lieutenant was just a military doctor after being commissioned, so he can't do anything by himself. It must be tough for you, getting called in\nlike this all the time.", "/[waitkey(64)]/[clear()]"};
    String[] msg015EDE83 = new String[]{"/[label(Shion)]", "Not at all, I enjoy talking with the Realians!", "/[waitkey(64)]/[close()]"};
    String[] msg015F3E2F = new String[]{"/[label()]", "After you finish, give me a holler. Let's go get some coffee together.", "/[waitkey(64)]/[close()]"};
    String[] msg015FC195 = new String[]{"/[label()]", "Me? I'm done with work for the day. A mangy Marine, a friend of the Lieutenant or something, is lurking around our work area. He's so distracting!", "/[waitkey(64)]/[close()]"};
    String[] msg015FC1951 = new String[]{"/[label()]", "Oh, Chief Uzuki! You're still not done working? Really, Vector people must have it tough.", "/[waitkey(1)]/[clear()]", "Oh well. Maybe I should ask Allen out instead?", "/[waitkey(64)]/[close()]"};
    String[] msg01C66155 = new String[]{"/[label(Shion)]", "S-say, weren't you there when I left my room this morning?", "/[waitkey(64)]/[clear()]"};
    String[] msg01C66156 = new String[]{"/[label()]", "N-no, no way! I just be readin' the info board here!", "/[waitkey(64)]/[close()]"};
    String[] msg01C6C103 = new String[]{"/[label(Shion)]", "...Really?", "/[waitkey(64)]/[clear()]"};
    String[] msg01C6C104 = new String[]{"/[label()]", "That be right. ", "I never be lookin' into yer room or be gettin' interested in bad ways or be lookin' to hit on ya or nothing!", "/[waitkey(64)]/[clear()]"};
    String[] msg01C6C105 = new String[]{"/[label(Shion)]", "...\n", "...", "/[waitkey(64)]/[close()]"};
    String[] msg14676900 = new String[]{"/[label()]", "I'm very sorry, but that door is not open to traffic right now.", "/[waitkey(1)]/[clear()]", "This damn annoying old crane started acting up. Until maintenance is finished, no one is allowed to even walk by here. And please don't touch anything.", "/[waitkey(1)]/[clear()]", "This thing might throw its entire load if someone presses the wrong switch.", "/[waitkey(64)]/[close()]"};
    String[] msg14676901 = new String[]{"/[label()]", "Sorry, but keep your distance from this crane. This damn annoying old hunk of junk started acting up.", "/[waitkey(1)]/[clear()]", "This thing might throw its entire load if someone presses the wrong switch.", "/[waitkey(64)]/[close()]"};
    String[] msg1467C8AD = new String[]{"/[label()]", "I don't mean to sound like a broken record, but I'll say it again.", "/[waitkey(1)]/[clear()]", "Don't ever touch the switch on this guy. It'll drop the pallet with the containers, and cause big havoc.", "/[waitkey(64)]/[close()]"};
    String[] msg12C14D0F = new String[]{"/[label()]", "Sorry, but you won't be able to go this way for a while.", "/[waitkey(64)]/[close()]"};
    String[] msg12C14D0F1 = new String[]{"/[label()]", "We've opened up the walkway again, but we haven't finished repairing the crane yet.", "/[waitkey(64)]/[close()]"};
    String[] msg12C1ACBC = new String[]{"It just happened to coincide with the crane's malfunction. Real sorry about that.", "/[waitkey(64)]/[close()]"};
    String[] msg12C1ACBC1 = new String[]{"/[label()]", "I've got no idea when the repairs will be completed. Real sorry about that.", "/[waitkey(64)]/[close()]"};
    String[] msg0086708D = new String[]{"/[label()]", "Oh, Miss Uzuki. Do you know anything about the Gnosis?", "/[waitkey(64)]/[close()]"};
    String[] msg0079A801 = new String[]{"/[label()]", "Before I was assigned here, they showed me records of the Gnosis.", "/[waitkey(1)]/[clear()]", "I was shocked. I thought it was all just a fairy-tale.", "/[waitkey(64)]/[close()]"};
    String[] msg0071B84D = new String[]{"/[label()]", "Did you know? The Gnosis can appear anywhere and everywhere, even in space or inside a ship.", "/[waitkey(1)]/[clear()]", "Just thinking about it makes me so scared, I can't sleep. I haven't gotten decent sleep ever since I got assigned on this ship.", "/[waitkey(64)]/[close()]"};
    String[] msg0088B2CE = new String[]{"/[label()]", "Some of my colleagues still don't believe it, and those that do are so frightened they can't do anything.", "/[waitkey(1)]/[clear()]", "I wonder what they really are? Are they really just like the military video says?", "/[waitkey(64)]/[close()]"};
    String[] msgNO2 = new String[]{"There are no lines for the 2nd time yet!", "/[waitkey(64)]/[close()]"};
    String[] msgSHION_1 = new String[]{"/[label(Shion)]", "Uh...oh, I should bring the Realian service data with me.", "/[waitkey(1)]/[clear()]", "I think I left it in my room. I better go get it.", "/[waitkey(1)]/[clear()]", "Let's see, if I remember correctly, the room is...straight down this corridor.", "/[waitkey(64)]/[close()]"};
    String[] msgSHION_11 = new String[]{"Let's see, if I remember correctly, the room is...straight down this corridor.", "/[waitkey(64)]/[close()]"};
    String[] msgSHION_2 = new String[]{"/[label(Shion)]", "Whoops...I passed it. What the heck am I doing?", "/[waitkey(64)]/[close()]"};
    String[] msgSHION_3 = new String[]{"/[label(Shion)]", "I have to go back to my room to get the data.", "/[waitkey(64)]/[clear()]"};
    String[] msgSHION_31 = new String[]{"What am I doing...?", "/[waitkey(64)]/[close()]"};
    String[] msgSTOP = new String[]{"/[label(Warning)]", "This gate is currently sealed. Please use an alternate gate.", "/[waitkey(64)]/[close()]"};
    String[] DOOR = new String[]{"Press partition switch?\n", "/[waitkey(64)]/[close()]"};
    String[] msgmushi = new String[]{"/[label()]", "On the battlefield, you must always be calm.", "/[waitkey(1)]/[clear()]", "We soldiers are trained so that we can stay cool at all times.", "/[waitkey(64)]/[close()]"};
    String[] msgmushi2 = new String[]{"/[label()]", "It's impossible to surprise me!", "/[waitkey(64)]/[close()]"};
    String[] msgHELP1 = new String[]{"/[label()]", "Whoa! What the heck happened?! Someone, let me outta here!", "/[waitkey(64)]/[close()]"};
    String[] msgHELP2 = new String[]{"/[label()]", "No! I don't want to die yet! Help me!", "/[waitkey(64)]/[close()]"};
    String[] msgikari = new String[]{"/[label()]", "Stop picking on me! I can't take it! Mommaaah!", "/[waitkey(64)]/[close()]"};
    String[] msgkakuheki1 = new String[]{"/[label()]", "Oh, the red switch there is the switch to open and close the bulkhead. Please don't mess with it.", "/[waitkey(64)]/[close()]"};
    String[] msgkakuheki2 = new String[]{"/[label()]", "Oh, now you've done it. I told you not to mess with it...", "/[waitkey(1)]/[clear()]", "Please put it back the way it was. I'm the one that gets yelled at.", "/[waitkey(64)]/[close()]"};
    String[] msg0000001 = new String[]{"/[label()]", "Sigh...shafted again. And to think, I even snuck out without telling my boss...", "/[waitkey(1)]/[clear()]", "Lately, it's always like this...", "/[waitkey(64)]/[close()]"};
    String[] msg0000002 = new String[]{"/[label()]", "But you know, just waiting for him like this is kind of like being with him too, ", "because all I do is think about him.", "/[waitkey(64)]/[close()]"};
    String[] msgtrap1 = new String[]{"/[label()]", "Why", "/[wait(40)]/[clear()]", "won't this thing", "/[wait(40)]/[clear()]", "work", "/[wait(40)]/[clear()]", "properly?!", "/[waitkey(64)]/[close()]"};
    String[] Dummy_1 = new String[]{"Chain event, (first half is complete).\n", "In this version, it will jump to the second half.\n", "/[waitkey(64)]/[close()]"};
    String[] msgMAP = new String[]{"/[label()]", "'Ship Map\n", "Current Location: Corridor 1'", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment address No. 18.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as segment address No. 18.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment address No. 18, decoding is complete.", "/[waitkey(64)]/[close()]"};
    String[] Tomiyama_1 = new String[]{"/[label(Shion)]", "Oh, if I leave the bulkhead closed, there's no way to go through it.", "/[waitkey(64)]/[close()]"};

    ST0040() {
    }

    void EV_Camera() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.63f, 1.698f, 16.256f);
        this.camEV.setRotate(-2.775f, 334.239f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(11.645f, 1.771f, -18.843f);
        this.camEV.setRotate(-5.395f, 168.479f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera2() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.36f, 3.371f, 19.988f);
        this.camEV.setRotate(-27.535f, 179.377f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera3() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.991f, 3.115f, -24.506f);
        this.camEV.setRotate(-19.035f, 202.018f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera4() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-11.417f, 4.74f, 23.057f);
        this.camEV.setRotate(-22.195f, 47.936f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera5() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.802f, 2.035f, -15.233f);
        this.camEV.setRotate(-0.261f, 90.499f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void Kakuheki_Close() {
        int n = 10;
        this.doorE.DoorClose();
        System.sleep(n);
        this.doorD.DoorClose();
        System.sleep(n);
        this.doorC.DoorClose();
        System.sleep(n);
        this.doorB.DoorClose();
        System.sleep(n);
        this.doorA.DoorClose();
    }

    void Kakuheki_Open() {
        int n = 10;
        this.doorA.DoorOpen();
        System.sleep(n);
        this.doorB.DoorOpen();
        System.sleep(n);
        this.doorC.DoorOpen();
        System.sleep(n);
        this.doorD.DoorOpen();
        System.sleep(n);
        this.doorE.DoorOpen();
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                switch (this.S011) {
                    case 0: {
                        this.fadeIn.call(0);
                        System.sleep(23);
                        Runtime.setFlags(18, 1, 1);
                        Runtime.jumpEvent(1110);
                        return;
                    }
                    default: {
                        return;
                    }
                }
            }
            case 1: {
                this.SHION_TALK_2 = Runtime.getFlags(7007, 1);
                if (Runtime.checkItem(10, 8) == 1) {
                    return;
                }
                switch (this.SHION_TALK_2) {
                    case 0: {
                        Runtime.setFlags(7007, 1, 1);
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.player.mtn(11, 1, 1.0f, true);
                        this.win = Window.create();
                        this.win.print(this.msgSHION_1, 0);
                        ST0040.waitPage(this.win, 64);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        return;
                    }
                    default: {
                        return;
                    }
                }
            }
            case 2: {
                if (Runtime.checkItem(10, 8) != 0) return;
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.cam0.setMode(-1);
                this.EV_Camera4();
                System.sleep(5);
                this.player.mtn(9, 1, 1.0f, true);
                this.win = Window.create();
                this.win.print(this.msgSHION_3, 0);
                ST0040.waitPage(this.win, 64);
                this.player.mtn(11, 1, 1.0f, true);
                this.win.print(this.msgSHION_31, 0);
                ST0040.waitPage(this.win, 64);
                System.sleep(5);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                return;
            }
            case 3: {
                if (Runtime.checkItem(10, 8) == 1) {
                    return;
                }
                this.SHION_TALK_3 = Runtime.getFlags(7008, 1);
                switch (this.SHION_TALK_3) {
                    case 0: {
                        Runtime.setFlags(7008, 1, 1);
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.cam0.setMode(-1);
                        this.EV_Camera3();
                        this.player.mtn(11, 1, 1.0f, true);
                        this.win = Window.create();
                        this.win.print(this.msgSHION_2, 0);
                        ST0040.waitPage(this.win, 64);
                        System.sleep(5);
                        this.cam0.setMode(0);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        return;
                    }
                    default: {
                        return;
                    }
                }
            }
            case 4: {
                if (this.S015B == 1) {
                    return;
                }
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgSTOP, 0);
                ST0040.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                return;
            }
            case 5: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                switch (Runtime.getFlags(7010, 1)) {
                    case 0: {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.DOOR, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.cam0.setMode(-1);
                                Stage.setVisible(36, true);
                                Stage.setVisible(37, true);
                                Stage.setVisible(38, true);
                                Stage.setVisible(39, true);
                                Stage.setVisible(40, true);
                                Stage.setVisible(41, true);
                                this.EV_Camera();
                                Runtime.enable(65536);
                                this.player.setTranslate(14.81f, 0.0f, 10.108f);
                                this.player.setRotate(0.0f, 270.0f, 0.0f);
                                this.player.mtn(26, 1, 1.0f, true);
                                System.sleep(30);
                                Sound.effectPlay(196741);
                                System.sleep(10);
                                Sound.streamPlay(196011, 48000);
                                System.sleep(382);
                                Sound.effectPlay(196742);
                                this.Kakuheki_Close();
                                System.sleep(30);
                                this.cam0.setMode(0);
                                Stage.setVisible(36, false);
                                Stage.setVisible(37, false);
                                Stage.setVisible(38, false);
                                Stage.setVisible(39, false);
                                Stage.setVisible(40, false);
                                Stage.setVisible(41, false);
                                this.npc12.setVisible(false);
                                this.npc20.setVisible(true);
                                Runtime.setPlayerControl(true);
                                Runtime.disable(65536);
                                Runtime.setFlags(7010, 1, 1);
                                this.BUTTON_F = 0;
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        return;
                    }
                    case 1: {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.DOOR, 0);
                        System.waitFor(this.win);
                        this.menu = Menu.create();
                        this.menu.addItem("Yes\nNo");
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.cam0.setMode(-1);
                                Stage.setVisible(36, true);
                                Stage.setVisible(37, true);
                                Stage.setVisible(38, true);
                                Stage.setVisible(39, true);
                                Stage.setVisible(40, true);
                                Stage.setVisible(41, true);
                                this.EV_Camera();
                                Runtime.enable(65536);
                                this.player.setTranslate(14.81f, 0.0f, 10.108f);
                                this.player.setRotate(0.0f, 270.0f, 0.0f);
                                this.player.mtn(26, 1, 1.0f, true);
                                System.sleep(30);
                                Sound.effectPlay(196741);
                                System.sleep(10);
                                Sound.streamPlay(196021, 48000);
                                System.sleep(349);
                                Sound.effectPlay(196742);
                                System.sleep(10);
                                this.Kakuheki_Open();
                                System.sleep(30);
                                this.cam0.setMode(0);
                                Stage.setVisible(36, false);
                                Stage.setVisible(37, false);
                                Stage.setVisible(38, false);
                                Stage.setVisible(39, false);
                                Stage.setVisible(40, false);
                                Stage.setVisible(41, false);
                                Runtime.setPlayerControl(true);
                                Runtime.disable(65536);
                                Runtime.setFlags(7010, 1, 0);
                                this.CloseAfter = 1;
                                this.BUTTON_F = 0;
                                return;
                            }
                        }
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        return;
                    }
                }
                this.BUTTON_F = 0;
                return;
            }
            case 6: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                switch (Runtime.getFlags(7010, 1)) {
                    case 1: {
                        Runtime.setPlayerControl(false);
                        switch (this.HelpFlag1) {
                            case 0: {
                                ++this.HelpFlag1;
                                this.win = Window.create();
                                this.win.setSize(4, 45);
                                this.win.setLocation(15, 15);
                                this.win.print(this.msgHELP1, 0);
                                ST0040.waitPage(this.win, 64);
                                Runtime.setPlayerControl(true);
                                this.BUTTON_F = 0;
                                return;
                            }
                        }
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 15);
                        this.win.print(this.msgHELP2, 0);
                        ST0040.waitPage(this.win, 64);
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                        --this.HelpFlag1;
                        return;
                    }
                }
                this.BUTTON_F = 0;
                return;
            }
            case 7: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(3218, 1) == 0) {
                    Sound.effectPlay(55);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_01, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3218, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                } else if (Runtime.getFlags(3238, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                } else if (Runtime.getFlags(3298, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(56);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_03, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3298, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                } else {
                    this.BUTTON_F = 0;
                    return;
                }
                this.BUTTON_F = 0;
                return;
            }
            case 8: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                Stage.setVisible(119, false);
                this.monitor1.signal(1);
                this.monitor2.signal(1);
                this.cam0.setMode(-1);
                this.EV_Camera5();
                this.win = Window.create();
                this.win.print(this.msgMAP, 0);
                ST0040.waitPage(this.win, 64);
                this.monitor1.signal(0);
                this.monitor2.signal(0);
                System.sleep(20);
                this.cam0.setMode(0);
                Stage.setVisible(119, true);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                return;
            }
            case 10: {
                if (this.smd != 0) return;
                Sound.sequenceStop(0, 1000);
                Sound.sequencePlay(1, 127);
                this.smd = 1;
                return;
            }
            case 11: {
                if (this.smd != 1) return;
                Sound.sequenceStop(1, 1000);
                Sound.sequencePlay(0, 127);
                this.smd = 0;
                return;
            }
            case 12: {
                if (Runtime.getFlags(7010, 1) == 0) {
                    return;
                }
                Runtime.setPlayerControl(false);
                this.player.getTranslate();
                this.player.setTranslate(this.player.px, this.player.py, this.player.pz - 0.3f);
                Runtime.enable(65536);
                this.player.mtn(28, 1, 1.0f, true);
                this.cam0.setMode(-1);
                this.EV_Camera06();
                System.sleep(15);
                Stage.setVisible(36, true);
                Stage.setVisible(37, true);
                Stage.setVisible(38, true);
                Stage.setVisible(39, true);
                Stage.setVisible(40, true);
                Stage.setVisible(41, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Tomiyama_1, 0);
                System.waitFor(this.win);
                System.sleep(15);
                Stage.setVisible(36, false);
                Stage.setVisible(37, false);
                Stage.setVisible(38, false);
                Stage.setVisible(39, false);
                Stage.setVisible(40, false);
                Stage.setVisible(41, false);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                return;
            }
        }
    }

    void NotYet() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print("工事中です/[wait(30)]/[close()]");
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc1_1(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc10_2(window);
        } else {
            this.Talk_npc10_1(window);
        }
    }

    void Talk_npc10_1(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg12C14D0F, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg12C1ACBC, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc10_2(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg12C14D0F1, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg12C1ACBC1, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        if (Runtime.getFlags(7010, 1) == 1) {
            window.print(this.msgkakuheki2, 0);
            ST0040.waitPage(window, 64);
        } else {
            window.print(this.msgkakuheki1, 0);
            ST0040.waitPage(window, 64);
        }
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        switch (this.CloseAfter) {
            case 0: {
                this.Talk_npc12_1(window);
                return;
            }
        }
        this.Talk_npc12_2(window);
    }

    void Talk_npc12_1(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                this.npc12.kickEnepc(1, 3);
                window.print(this.msgmushi, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        this.npc12.kickEnepc(1, 3);
        window.print(this.msgmushi2, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc12_2(Window window) {
        window.print(this.msgikari, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg1513578D1, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1513B739, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        if (Runtime.getFlags(7029, 1) == 0) {
            if (Runtime.getFlags(7030, 1) == 0) {
                window.print(this.msg0106D200, 0);
                ST0040.waitPage(window, 64);
                return;
            }
            ++this.talkFlag2;
            switch (this.talkFlag2) {
                case 1: {
                    window.print(this.msg0105EF11, 0);
                    ST0040.waitPage(window, 64);
                    Runtime.setFlags(7031, 1, 1);
                    return;
                }
                case 2: {
                    window.print(this.msg01064EBD, 0);
                    ST0040.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msg0106D222, 0);
            ST0040.waitPage(window, 64);
            this.npc2.kickEnepc(7, 14);
            Runtime.setFlags(7029, 1, 1);
            Runtime.setFlags(7031, 1, 0);
            return;
        }
        this.npc2.kickEnepc(7, 14);
        window.print(this.msg0106D223, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        if (Runtime.getFlags(7029, 1) == 0) {
            if (Runtime.getFlags(7031, 1) == 0) {
                ++this.talkFlag3;
                switch (this.talkFlag3) {
                    case 1: {
                        window.print(this.msg02E896E4, 0);
                        ST0040.waitPage(window, 64);
                        Runtime.setFlags(7030, 1, 1);
                        return;
                    }
                }
                window.print(this.msg02E8F691, 0);
                ST0040.waitPage(window, 64);
                return;
            }
            window.print(this.msg02E979F7, 0);
            ST0040.waitPage(window, 64);
            Runtime.setFlags(7029, 1, 1);
            return;
        }
        window.print(this.msg02E979F6, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg0000001, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0000002, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msgtrap1, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc) {
        this.Talk_npc6_1();
    }

    void Talk_npc6_1() {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0086708D, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes, I'm quite familiar with the Gnosis\nNot really");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msg0079A801, 0);
                        ST0040.waitPage(this.win, 64);
                        return;
                    }
                }
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg0071B84D, 0);
                ST0040.waitPage(this.win, 64);
                return;
            }
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg0088B2CE, 0);
        ST0040.waitPage(this.win, 64);
    }

    void Talk_npc6_2() {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgNO2, 0);
        ST0040.waitPage(this.win, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc7_2(window);
        } else {
            this.Talk_npc7_1(window);
        }
    }

    void Talk_npc7_1(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg14676900, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1467C8AD, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                window.print(this.msg14676901, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1467C8AD, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        window.print(this.msg0051D1D6, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0040.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc9_2(window);
        } else {
            this.Talk_npc9_1(window);
        }
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg015EDE82, 0);
                ST0040.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg015EDE83, 0);
                ST0040.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                window.print(this.msg015F3E2F, 0);
                ST0040.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg015FC195, 0);
        ST0040.waitPage(window, 64);
    }

    void Talk_npc9_2(Window window) {
        window.print(this.msg015FC1951, 0);
        ST0040.waitPage(window, 64);
    }

    public void Touch_npc1(Enepc enepc, Window window) {
        this.npc1.enableDTKFlag(1);
        this.npc1.disableDTKFlag(131072);
        this.npc1.kickEnepc(0, 9);
        window.print(this.msg1513578D, 0);
        ST0040.waitPage(window, 64);
        this.npc1.kickEnepc(7, 56);
    }

    public void Touch_npc3(Enepc enepc, Window window) {
        this.npc3.enableDTKFlag(1);
        this.npc3.disableDTKFlag(131072);
        this.npc3.kickEnepc(0, 9);
        window.print(this.msg02E896E4, 0);
        ST0040.waitPage(window, 64);
        Runtime.setFlags(7030, 1, 1);
        ++this.talkFlag3;
        this.npc3.kickEnepc(7, 68);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                if (this.S015B != 1) break;
                Runtime.jumpCF(120, 4);
                break;
            }
            case 1: {
                if (this.SHION_ROOM == 1) {
                    Runtime.setFlags(24, 1, 1);
                    Runtime.setFlags(25, 1, 1);
                    Runtime.setFlags(7009, 1, 0);
                    Runtime.jumpEvent(1170);
                    break;
                }
                Runtime.jumpCF(50, 1);
                break;
            }
            case 2: {
                if (this.S013 == 0) {
                    Runtime.setFlags(20, 1, 1);
                    Runtime.jumpEvent(1130);
                    break;
                }
                Runtime.jumpCF(60, 2);
                break;
            }
            case 3: {
                Runtime.jumpCF(30, 2);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -4.0f, 2.0f, 15.5f, 0.0f);
        this.teiten1.SetBgm(196618);
        this.teiten2 = new Uwamono(28690, 13.0f, 0.0f, -26.5f, 0.0f);
        this.teiten2.SetBgm(196618);
        this.teiten3 = new Uwamono(28690, 5.0f, -1.0f, 66.5f, 0.0f);
        this.teiten3.SetBgm(196619);
        this.teiten4 = new Uwamono(28690, 1.0f, 2.0f, -52.5f, 0.0f);
        this.teiten4.SetBgm(196619);
        this.teiten5 = new Uwamono(28690, 3.5f, 2.0f, -45.0f, 0.0f);
        this.teiten5.SetBgm(196640);
        this.teiten6 = new Uwamono(28690, 3.5f, 2.0f, -48.0f, 0.0f);
        this.teiten6.SetBgm(196640);
        this.teiten7 = new Uwamono(28690, 8.0f, -1.0f, 58.0f, 0.0f);
        this.teiten7.SetBgm(196640);
        this.teiten8 = new Uwamono(28690, 8.0f, -1.0f, 61.0f, 0.0f);
        this.teiten8.SetBgm(196640);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setIdLightCol(1, 0, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(1, 1, 0.47f, 0.47f, 0.47f);
        Runtime.setIdLightCol(1, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(1, 3, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -0.5f);
        Runtime.setIdLightCol(2, 0, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightCol(2, 1, 0.47f, 0.47f, 0.47f);
        Runtime.setIdLightCol(2, 2, 0.4f, 0.4f, 0.4f);
        Runtime.setIdLightCol(2, 3, 1.0f, 1.0f, 1.0f);
        Runtime.setIdLightVec(2, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(2, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(2, 3, 0.0f, -1.0f, -0.5f);
        Stage.setVisible(-1, true);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 7.19f, 1.8f, -15.17f, 90.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor1.setArgs(1, 18002, 0, 512, 260);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.monitor2 = new Object();
        this.monitor2.init(24613, 7.19f, 1.8f, -15.17f, 90.0f);
        this.monitor2.setArgs(0, 0.0f, 0.5f, 2.72f, 1.38f);
        this.monitor2.setArgs(1, 18005, 0, 256, 130);
        this.monitor2.setArgs(2, 64, 0, 1, -10);
        this.monitor2.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        this.Crane = new Unit();
        this.Crane.init(20580, 7.34f, -1.0f, 59.23f, 0.0f);
        if (Runtime.getFlags(23, 1) == 0) {
            this.batu = new Unit();
            this.batu.mapUnit(184);
            this.batu.start(4, null);
            this.batu.setTranslate(4.9f, 2.0f, -56.15f);
            this.batu.setRotate(0.0f, 255.0f, 0.0f);
        }
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setFog(1, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(2, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(3, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(4, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(5, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(6, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(7, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(8, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(9, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(10, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(11, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(12, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(13, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(14, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(15, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setFog(16, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFLockX(1, 9.0f);
        this.cam0.setCFAngle(2, -28.0f, 335.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 5.5f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFLockX(4, 10.0f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFLockX(7, 3.5f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestal(9, 16.848f, 2.755f, 14.868f, 53.12f, -30.0f, 397.915f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(9, 1);
        this.cam0.setCFPedestal(10, 16.848f, 2.755f, 14.868f, 53.12f, -30.0f, 397.915f, 0.0f, 2.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(10, 1);
        this.cam0.setCFAngle(11, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFLockX(12, 7.0f);
        Stage.setVisible(36, false);
        Stage.setVisible(37, false);
        Stage.setVisible(38, false);
        Stage.setVisible(39, false);
        Stage.setVisible(40, false);
        Stage.setVisible(41, false);
        Stage.setVisible(139, false);
        Stage.setVisible(140, false);
        Stage.setVisible(141, false);
        Stage.setVisible(50, false);
        Stage.setVisible(42, false);
        Stage.setVisible(43, false);
        Stage.setVisible(44, false);
        Stage.setVisible(45, false);
        Stage.setVisible(46, false);
        Stage.setVisible(47, false);
        Stage.setVisible(173, false);
        Stage.setVisible(48, false);
        Stage.setVisible(49, false);
        Stage.setVisible(51, false);
        Stage.setVisible(36, false);
        Stage.setVisible(139, false);
        Stage.setVisible(140, false);
        Stage.setVisible(141, false);
        Stage.setVisible(48, false);
        Stage.setVisible(49, false);
        Stage.setVisible(51, false);
        this.entrance = Runtime.getEntrance();
        if (this.entrance >= 0) {
            Runtime.setRegister(0, this.entrance);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, this.entrance);
        }
        new Uwamono(125, 21);
        new Uwamono(147, 4);
        new Uwamono(148, 4);
        new Uwamono(153, 24);
        new Uwamono(180, 24);
        new Uwamono(181, 24);
        new Uwamono(182, 24);
        new Uwamono(198, 0);
        this.obj01 = new Uwamono(199, 6);
        this.obj01.SetSize(2.5f, 0.5f, 2.5f);
        this.ueki = new Uwamono(200, 1);
        this.ueki.SetSize(0.5f, 2.0f, 0.5f);
        new Uwamono(201, 84);
        new Uwamono(126, 31);
        this.trap2 = new Uwamono(28674, 5.6f, -1.0f, 38.0f, 0.0f);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        this.EF01 = new Effect(1012, 1);
        this.EF01.disp(true);
        this.EF02 = new Effect(1011, 0);
        this.EF02.disp(true);
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
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
        this.S015B = Runtime.getFlags(23, 1);
        this.S011 = Runtime.getFlags(18, 1);
        this.S013 = Runtime.getFlags(20, 1);
        this.SHION_TALK_1 = Runtime.getFlags(7006, 1);
        this.SHION_TALK_2 = Runtime.getFlags(7007, 1);
        this.SHION_TALK_3 = Runtime.getFlags(7008, 1);
        this.SHION_ROOM = Runtime.getFlags(7009, 1);
        if (this.S015B == 1) {
            this.tobira_3();
        } else if (Runtime.checkItem(10, 8) == 1) {
            this.tobira_2();
        } else {
            this.tobira_1();
        }
        this.npcset_1();
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(527, 1, 0, 56, 5, 10.11f, 0.0f, 62.23f, 180.0f);
        this.npc2 = new NPC_NORMAL(519, 2, 0, 14, 5, 0.6f, 0.0f, 45.3f, 90.0f);
        this.npc3 = new NPC_NORMAL(520, 3, 0, 68, 5, 6.49f, -1.0f, 50.03f, 30.0f);
        this.npc4 = new NPC_NORMAL(523, 4, 0, 14, 8, 12.4f, 0.0f, 19.9f, 300.0f);
        this.npc5 = new NPC_NORMAL(524, 5, 0, 14, 12, 6.2f, 0.0f, 37.94f, 270.0f);
        this.npc6 = new NPC_NORMAL(523, 6, 0, 2, 8, 11.8f, 0.0f, -31.2f, -90.0f);
        this.npc7 = new NPC_NORMAL(527, 7, 0, 14, 5, 5.56f, 2.1f, -48.15f, 270.0f);
        this.npc8 = new NPC_NORMAL(527, 8, 0, 13, 12, 5.9f, 0.0f, 65.92f, -90.0f);
        this.npc9 = new NPC_NORMAL(534, 9, 0, 7, 8, -9.0f, 2.1f, 16.5f, -90.0f);
        this.npc10 = new NPC_NORMAL(520, 10, 0, 14, 5, 4.73f, 1.5f, -40.91f, 0.0f);
        this.npc11 = new NPC_NORMAL(523, 11, 0, 13, 26, 15.33f, 0.0f, 9.32f, 180.0f);
        this.npc12 = new NPC_NORMAL(526, 12, 0, 14, 10, 8.65f, 0.0f, 3.55f, 90.0f);
        this.npc20 = new NPC_NORMAL(526, 20, 0, 14, 10, 8.65f, 0.0f, 3.55f, 270.0f);
        this.npc1.disableDTKFlag(8);
        this.npc1.enableDTKFlag(393216);
        this.npc1.setMotion(0, 14);
        this.npc2.enableDTKFlag(393216);
        this.npc2.disableDTKFlag(8);
        this.npc2.setMotion(0, 10);
        if (Runtime.getFlags(7030, 1) == 0) {
            this.npc3.enableDTKFlag(393216);
            this.npc3.disableDTKFlag(8);
            this.npc3.disableDTKFlag(1);
            this.npc3.touchto("Touch_npc3");
        } else {
            this.npc3.enableDTKFlag(262144);
            this.npc3.disableDTKFlag(8);
            this.npc3.disableDTKFlag(1);
        }
        this.npc4.setMotion(0, 13);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc5.disableDTKFlag(3);
        this.npc5.setMotion(0, 4);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 10);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 1);
        this.npc10.setMotion(0, 10);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 1);
        this.npc12.disableDTKFlag(3);
        this.npc12.enableDTKFlag(4);
        this.npc12.enableDTKFlag(262144);
        this.npc12.setMotion(0, 1);
        this.npc20.setMotion(0, 5);
        this.npc20.disableDTKFlag(3);
        if (Runtime.getFlags(7010, 1) == 1) {
            this.npc12.setVisible(false);
        } else {
            this.npc20.setVisible(false);
        }
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        this.npc1.touchto("Touch_npc1");
    }

    void npcset_2() {
    }

    void tobira_1() {
        this.doorA = new Uwamono(107, 42, '\u0001');
        new Uwamono(108, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(109, 42, '\u0001');
        new Uwamono(110, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(111, 42, '\u0001');
        new Uwamono(112, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(113, 42, '\u0001');
        new Uwamono(114, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(115, 42, '\u0001');
        new Uwamono(116, 42, '\u0001', this.doorE);
        this.doorF = new Uwamono(65, 42, '\u0001');
        new Uwamono(66, 42, '\u0001', this.doorF);
        this.doorG = new Uwamono(69, 42, '\u0001');
        new Uwamono(70, 42, '\u0001', this.doorG);
        this.doorH = new Uwamono(170, 42, '\u0001');
        new Uwamono(161, 42, '\u0001', this.doorH);
        this.doorI = new Uwamono(58, 40, '\u0001');
        this.doorJ = new Uwamono(61, 40, '\u0004');
        this.doorK = new Uwamono(54, 40, '\u0003');
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorE.SetDoorType('\u0002');
        this.doorJ.SetDoorType('\u0002');
        this.doorF.SetDoorType('\u0002');
        this.doorG.SetDoorType('\u0002');
        this.doorH.SetDoorType('\u0004');
        this.doorI.SetDoorType('\u0004');
        this.doorK.SetDoorType('\u0004');
        this.doorA.SetDoorRange(3.0f);
        this.doorB.SetDoorRange(3.0f);
        this.doorC.SetDoorRange(3.0f);
        this.doorD.SetDoorRange(3.0f);
        this.doorE.SetDoorRange(3.0f);
        if (Runtime.getFlags(7010, 1) == 0) {
            this.doorA.DoorOpen();
            this.doorB.DoorOpen();
            this.doorC.DoorOpen();
            this.doorD.DoorOpen();
            this.doorE.DoorOpen();
        } else if (Runtime.getFlags(7010, 1) == 1) {
            this.doorA.DoorClose();
            this.doorB.DoorClose();
            this.doorC.DoorClose();
            this.doorD.DoorClose();
            this.doorE.DoorClose();
        }
    }

    void tobira_2() {
        this.doorA = new Uwamono(107, 42, '\u0001');
        new Uwamono(108, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(109, 42, '\u0001');
        new Uwamono(110, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(111, 42, '\u0001');
        new Uwamono(112, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(113, 42, '\u0001');
        new Uwamono(114, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(115, 42, '\u0001');
        new Uwamono(116, 42, '\u0001', this.doorE);
        this.doorF = new Uwamono(65, 42, '\u0001');
        new Uwamono(66, 42, '\u0001', this.doorF);
        this.doorG = new Uwamono(69, 42, '\u0001');
        new Uwamono(70, 42, '\u0001', this.doorG);
        this.doorH = new Uwamono(170, 42, '\u0001');
        new Uwamono(161, 42, '\u0001', this.doorH);
        this.doorI = new Uwamono(58, 40, '\u0001');
        this.doorJ = new Uwamono(61, 40, '\u0004');
        this.doorK = new Uwamono(54, 40, '\u0003');
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorE.SetDoorType('\u0002');
        this.doorJ.SetDoorType('\u0002');
        this.doorF.SetDoorType('\u0002');
        this.doorG.SetDoorType('\u0004');
        this.doorH.SetDoorType('\u0004');
        this.doorI.SetDoorType('\u0004');
        this.doorK.SetDoorType('\u0004');
        this.doorA.SetDoorRange(3.0f);
        this.doorB.SetDoorRange(3.0f);
        this.doorC.SetDoorRange(3.0f);
        this.doorD.SetDoorRange(3.0f);
        this.doorE.SetDoorRange(3.0f);
        if (Runtime.getFlags(7010, 1) == 0) {
            this.doorA.DoorOpen();
            this.doorB.DoorOpen();
            this.doorC.DoorOpen();
            this.doorD.DoorOpen();
            this.doorE.DoorOpen();
        } else if (Runtime.getFlags(7010, 1) == 1) {
            this.doorA.DoorClose();
            this.doorB.DoorClose();
            this.doorC.DoorClose();
            this.doorD.DoorClose();
            this.doorE.DoorClose();
        }
    }

    void tobira_3() {
        this.doorA = new Uwamono(107, 42, '\u0001');
        new Uwamono(108, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(109, 42, '\u0001');
        new Uwamono(110, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(111, 42, '\u0001');
        new Uwamono(112, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(113, 42, '\u0001');
        new Uwamono(114, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(115, 42, '\u0001');
        new Uwamono(116, 42, '\u0001', this.doorE);
        this.doorF = new Uwamono(65, 42, '\u0001');
        new Uwamono(66, 42, '\u0001', this.doorF);
        this.doorG = new Uwamono(69, 42, '\u0001');
        new Uwamono(70, 42, '\u0001', this.doorG);
        this.doorH = new Uwamono(170, 42, '\u0001');
        new Uwamono(161, 42, '\u0001', this.doorH);
        this.doorI = new Uwamono(58, 40, '\u0001');
        this.doorJ = new Uwamono(61, 40, '\u0004');
        this.doorK = new Uwamono(54, 40, '\u0003');
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorE.SetDoorType('\u0002');
        this.doorJ.SetDoorType('\u0002');
        this.doorF.SetDoorType('\u0004');
        this.doorG.SetDoorType('\u0004');
        this.doorH.SetDoorType('\u0004');
        this.doorI.SetDoorType('\u0004');
        this.doorK.SetDoorType('\u0004');
        this.doorA.SetDoorRange(3.0f);
        this.doorB.SetDoorRange(3.0f);
        this.doorC.SetDoorRange(3.0f);
        this.doorD.SetDoorRange(3.0f);
        this.doorE.SetDoorRange(3.0f);
        if (Runtime.getFlags(7010, 1) == 0) {
            this.doorA.DoorOpen();
            this.doorB.DoorOpen();
            this.doorC.DoorOpen();
            this.doorD.DoorOpen();
            this.doorE.DoorOpen();
        } else if (Runtime.getFlags(7010, 1) == 1) {
            this.doorA.DoorClose();
            this.doorB.DoorClose();
            this.doorC.DoorClose();
            this.doorD.DoorClose();
            this.doorE.DoorClose();
        }
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

    class Object
            extends Unit {
        Object() {
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

