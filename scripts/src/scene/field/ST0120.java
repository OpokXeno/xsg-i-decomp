import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK12_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0120
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK12_PRJ {
    Player player;
    Camera cam1;
    Camera cam2;
    Camera cam3;
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
    Enepc npc17;
    Enepc npc18;
    Enepc npc20;
    Enepc npc21;
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
    int talkFlag9;
    int talkFlag10;
    int talkFlag11;
    int talkFlag12;
    int talkFlag13;
    int talkFlag14;
    int talkFlag15;
    int talkFlag16;
    int talkFlag17;
    int talkFlag18;
    int talkFlag31;
    int touchFlag1;
    int touchFlag2;
    int touchFlag6;
    int lift0 = 0;
    int selected0 = 0;
    int peep = 0;
    int BUTTON_F = 0;
    int Loc_flg = 1;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono doorF;
    Uwamono doorG;
    Uwamono Glass;
    Uwamono obj01;
    Uwamono ueki01;
    Uwamono ueki02;
    int S014A;
    int S014B;
    int S015B;
    int YOBIDASI_F;
    int KAKUHEKI_F;
    int eveflag_1;
    int eveflag_2;
    int eveflag_3;
    int eveflag_4;
    int eveflag_5;
    Unit monitor1;
    Unit elv;
    Unit elv1;
    Unit elv2;
    Unit elv3;
    Uwamono itembox;
    Effect fade;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    MAPUnit ele;
    int epass = 1;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Uwamono teiten6;
    Uwamono teiten7;
    Uwamono teiten8;
    Uwamono teiten9;
    Uwamono SAVE;
    int smd = 0;
    int page;
    String[] msg10738331 = new String[]{"/[label()]", "Those \"Seraphim Sisters\" sure got some nice pipes! Their moves could use a little work though.", "/[waitkey(64)]/[close()]"};
    String[] msg1073E2DE = new String[]{"/[label()]", "Wish I could'a made it to their concert.", "/[waitkey(64)]/[close()]"};
    String[] msg10738338 = new String[]{"/[label()]", "Hey, whaddya do that for? I was watchin' that program.", "/[waitkey(64)]/[close()]"};
    String[] msg1073833A = new String[]{"/[label(Shion)]", "Whoops, I'm sorry.", "/[waitkey(64)]/[close()]"};
    String[] msg105DEF8B = new String[]{"/[label()]", "Why dontcha go to another info station?!", "/[waitkey(1)]/[clear()]", "This station's reserved for video programming in this time slot, and everyone's lookin' forward to it.", "/[waitkey(64)]/[close()]"};
    String[] msg125C0C55 = new String[]{"/[label()]", "This central elevator goes directly to the bridge.", "/[waitkey(1)]/[clear()]", "You're Chief Uzuki of Vector, right? The Captain is expecting you. Please hurry.", "/[waitkey(64)]/[close()]"};
    String[] msg125C0C56 = new String[]{"/[label()]", "If you have business on the bridge, please use this central elevator.", "/[waitkey(64)]/[close()]"};
    String[] msg013C6513 = new String[]{"/[label()]", "Uh, what? N-no, I'm not doing anything. Oh, I meant, I-I'm working, of course.", "/[waitkey(64)]/[close()]"};
    String[] msg013CC4C0 = new String[]{"/[label()]", "Uh, let's see. Where did I put it? Hmm...Oh yes, that's right I remember now.", "/[waitkey(64)]/[close()]"};
    String[] msg00ED5B85 = new String[]{"/[label()]", "Geez, I don't believe this...", "/[waitkey(1)]/[clear()]", "Why would anyone go out of their way and come into this room instead of just passing by?", "/[waitkey(64)]/[close()]"};
    String[] msg00EE3E9C = new String[]{"/[label()]", "And here I was, thinking that we finally found a place where we could be alone.", "/[waitkey(64)]/[close()]"};
    String[] msg001803F3 = new String[]{"/[label()]", "Sheesh, look at those two. They've been making out in there the whole time. Get a room, for crying out loud.", "/[waitkey(64)]/[close()]"};
    String[] msg001803F4 = new String[]{"/[label()]", "Damn, now I can't go inside! How am I supposed to get any work done?", "/[waitkey(64)]/[close()]"};
    String[] msg001803F5 = new String[]{"/[label()]", "What? Quit bugging me, will you? Things are heating up in there, so be quiet!", "/[waitkey(64)]/[close()]"};
    String[] msg001803F6 = new String[]{"/[label()]", "Man, things were finally getting hot and heavy, but you had to butt in, didn't you?", "/[waitkey(64)]/[close()]"};
    String[] msg0048B31C = new String[]{"/[label()]", "Excuse me, have you seen Lieutenant Virgil?", "/[waitkey(64)]/[clear()]"};
    String[] msg0048B31D = new String[]{"/[label(Shion)]", "Yes, I just saw him in the Realian Infirmary.", "/[waitkey(64)]/[clear()]"};
    String[] msg0048B31E = new String[]{"/[label()]", "What? The Realian Infirmary? That's strange. Why would he go there? I thought he was a DME addict.", "/[waitkey(64)]/[clear()]"};
    String[] msg0048B31E1 = new String[]{"/[label(Shion)]", "Ah, so he is a DME addict.", "/[waitkey(64)]/[close()]"};
    String[] msg0048B320 = new String[]{"/[label(Shion)]", "Doesn't DME addiction result from the consumption of Realian body tissue? Its symptoms are similar to poisoning, right? How did the Lieutenant become an addict?", "/[waitkey(64)]/[clear()]"};
    String[] msg0048B321 = new String[]{"/[label()]", "I don't know the specifics, but I heard there was an incident during the Miltian Conflict, and that the symptoms followed soon after.", "/[waitkey(1)]/[clear()]", "There's so much mystery surrounding that man.", "/[waitkey(64)]/[close()]"};
    String[] msg00491222 = new String[]{"/[label()]", "I heard that his hatred for Realians is due to DME toxicosis...but that still doesn't explain his extreme hatred of them.", "/[waitkey(64)]/[close()]"};
    String[] msg004912CE = new String[]{"/[label()]", "The holograms they have here are of the best quality.", "/[waitkey(1)]/[clear()]", "You know, the other day, we had a Gnosis holo, and the entire crew panicked. It was utter chaos.", "/[waitkey(64)]/[close()]"};
    String[] msg004912CD = new String[]{"/[label()]", "The holograms are controled via the console panel.", "/[waitkey(1)]/[clear()]", "But I'd recommend you not fiddle with it, or they could dock your pay, like they did to me.", "/[waitkey(64)]/[close()]"};
    String[] msg002C83E6 = new String[]{"/[label()]", "I can't believe how laid-back they are.", "/[waitkey(1)]/[clear()]", "We don't know when we'll be under attack, but those guys are totally oblivious. I've never fought Gnosis before, but I think I have a good grasp of how\nterrifying they are.", "/[waitkey(1)]/[clear()]", "Not even this state-of-the-art ship will stand a chance against them.", "/[waitkey(64)]/[close()]"};
    String[] msg002C83E7 = new String[]{"/[label()]", "It's been repeating the same footage over and over, but if you try to change it, that Marine over there goes into a rage.", "/[waitkey(1)]/[clear()]", "You can tell they don't let him out often...", "/[waitkey(64)]/[close()]"};
    String[] msg002C83E8 = new String[]{"/[label()]", "This is such a waste of my break.", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D50 = new String[]{"/[label()]", "I'm sorry,\n", "I'll add it in later!", "/[waitkey(64)]/[close()]"};
    String[] msgNO2 = new String[]{"There are no lines for the 2nd time yet.", "/[waitkey(64)]/[close()]"};
    String[] msgYOBIDASI = new String[]{"/[label(Ship Broadcast)]", "Chief Uzuki of Vector First R&D Division, you have a package from Vector HQ.", "/[waitkey(1)]/[clear()]", "Please claim your package at the A.G.W.S. hangar as soon as possible.", "/[waitkey(64)]/[close()]"};
    String[] msgSTOP = new String[]{"/[label(Warning)]", "This gate is currently closed. Please use another gate.", "/[waitkey(64)]/[close()]"};
    String[] msgKAKUHEKI_TALK = new String[]{"/[label()]", "Whenever I look at this switch, I hear a voice...", "/[waitkey(1)]/[clear()]", "\"Press me.\"\n", "\"Hurry up and press me.\"\n", "\"I dare you to press me!\"", "/[waitkey(1)]/[clear()]", "Can't you hear it too?", "/[waitkey(64)]/[close()]"};
    String[] msg00000001 = new String[]{"/[label()]", "Why do people install doors? Is it to show a rejection of things that are different, or is it to create anticipation of the unknown space?", "/[waitkey(64)]/[close()]"};
    String[] Loc_6_0 = new String[]{"/[label()]", "There is a switch. Press it?", "/[waitkey(64)]/[close()]"};
    String[] elev_1 = new String[]{"/[label(Shion)]", "Oh, it's an elevator.", "/[waitkey(64)]/[close()]"};
    String[] msg10000001 = new String[]{"/[label()]", "What's so great about Realians?! There's no way you can trust those puppets!", "/[waitkey(1)]/[clear()]", "You know what happened in Miltia, don't you? Who knows, the ones onboard this ship might go crazy too!", "/[waitkey(64)]/[close()]"};
    String[] msg10000002 = new String[]{"/[label()]", "Hey, enough is enough already. Don't blame them for everything.", "/[waitkey(64)]/[clear()]"};
    String[] msg10000003 = new String[]{"Shuddap! I've had enough of your \"Equal rights for Realians\" and \"It goes against humanity\" crap!", "/[waitkey(64)]/[clear()]"};
    String[] msg10000004 = new String[]{"The Realians on this ship aren't like the old models that were assigned to Miltia. An incident like that will never happen again.", "/[waitkey(64)]/[clear()]"};
    String[] msg10000005 = new String[]{"How can you be so sure? After all, they're nothing but combat weapons!", "/[waitkey(64)]/[close()]"};
    String[] msg10000006 = new String[]{"/[label()]", "His family was murdered by Realians on Miltia. He's been prejudiced against Realians since then.", "/[waitkey(64)]/[close()]"};
    String[] SUB_01 = new String[]{"Discovered Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_02 = new String[]{"It is marked as Segment Address No. 16.", "/[waitkey(64)]/[close()]"};
    String[] SUB_03 = new String[]{"Segment Address No. 16, decoding complete.", "/[waitkey(64)]/[close()]"};
    String[] Info_00 = new String[]{"Go to the bridge?", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0120() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-11.726f, 0.866f, 20.297f);
        this.camEV.setRotate(0.094f, 29.359f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        float[] fArray = new float[]{1.0f, 28.327f, 1.189f, 0.813f, 90.0f, 28.077f, 1.189f, -0.104f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -1.889f;
        fArray2[2] = 105.243f;
        fArray2[4] = 90.0f;
        fArray2[5] = -1.899f;
        fArray2[6] = 105.243f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 2, 90);
        this.camEV.rotateSPL(fArray3, 1, 2, 90);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-13.996f, 1.89f, 21.34f);
        this.camEV.setRotate(-11.305f, -0.399f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.773f, 3.875f, 22.734f);
        this.camEV.setRotate(-19.853f, 67.597f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.836f, 4.319f, 22.343f);
        this.camEV.setRotate(-23.798f, -70.278f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    void Kakuheki_Close() {
        Runtime.enable(65536);
        this.player.mtn(16, 1, 1.0f, true);
        System.sleep(50);
        this.doorD.DoorClose();
        System.sleep(20);
        this.cam0.setMode(0);
        Runtime.setPlayerControl(true);
        Runtime.disable(65536);
    }

    void Kakuheki_Open() {
        Runtime.enable(65536);
        this.player.mtn(16, 1, 1.0f, true);
        System.sleep(50);
        this.doorD.DoorOpen();
        System.sleep(20);
        this.cam0.setMode(0);
        Runtime.setPlayerControl(true);
        Runtime.disable(65536);
    }

    public void KickEvent(int n, int n2) {
        this.YOBIDASI_F = Runtime.getFlags(7011, 1);
        this.KAKUHEKI_F = Runtime.getFlags(7013, 1);
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                if (this.epass == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.Info_00, 0);
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            this.cam0.setMode(-1);
                            this.EV_Camera02();
                            Runtime.enable(65536);
                            this.player.mtn(2, 9, 1.0f, true);
                            this.player.move(10, -14.0f, 15.0f, true);
                            System.sleep(15);
                            this.player.rotY(15, 0.0f, true);
                            System.sleep(20);
                            Sound.effectPlay(196716);
                            Runtime.disable(65536);
                            this.doorE.SetDoorType('\u0002');
                            this.doorE.DoorClose();
                            System.sleep(30);
                            this.elv.setArgs(12, 5.0f);
                            System.sleep(30);
                            System.sleep(30);
                            Runtime.setFlags(7042, 1, 1);
                            System.println("よね１");
                            this.fade.call(0);
                            System.sleep(30);
                            Runtime.setPlayerControl(true);
                            Runtime.jumpCF(110, 1);
                            break block0;
                        }
                    }
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    this.player.move(40, -14.0f, 17.03f, true);
                    System.sleep(40);
                    Runtime.disable(65536);
                    this.doorE.SetDoorType('\u0002');
                    this.doorE.DoorClose();
                    System.sleep(20);
                    this.doorE.SetDoorType('\u0004');
                    Runtime.setPlayerControl(true);
                    break;
                }
                this.epass = 1;
                break;
            }
            case 1: {
                if (this.YOBIDASI_F == 0) {
                    return;
                }
                Runtime.setPlayerControl(false);
                Runtime.setFlags(7011, 1, 0);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgYOBIDASI, 0);
                ST0120.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                break;
            }
            case 2: {
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
                ST0120.waitPage(this.win, 64);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 3: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Loc_6_0, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        if (this.Loc_flg == 0) {
                            Runtime.enable(65536);
                            this.player.setTranslate(26.518f, 0.025999f, 0.87f);
                            this.player.setRotate(0.0f, 270.0f, 0.0f);
                            this.cam0.setMode(-1);
                            this.EV_Camera01();
                            this.player.mtn(26, 1, 1.0f, true);
                            System.sleep(20);
                            Sound.effectPlay(196751);
                            System.sleep(70);
                            Sound.effectPlay(196752);
                            System.sleep(20);
                            this.EF03.disp(false);
                            this.npc20.setVisible(true);
                            this.npc21.setVisible(true);
                            this.npc20.kickEnepc(10, 80, 30);
                            this.npc21.kickEnepc(10, 80, 30);
                            this.npc3.moveEnepc(17, 270.0f, 0.0f, 10);
                            this.npc3.disableDTKFlag(2);
                            this.moooon();
                            System.sleep(25);
                            System.sleep(5);
                            this.EF03.disp(true);
                            this.cam0.setMode(0);
                            this.player.setTranslate(26.0f, 0.025999f, 1.0329556f);
                            Runtime.setPlayerControl(true);
                            Runtime.disable(65536);
                            this.Loc_flg = 1;
                            this.BUTTON_F = 0;
                            break;
                        }
                        Runtime.enable(65536);
                        this.player.setTranslate(26.518f, 0.025999f, 0.87f);
                        this.player.setRotate(0.0f, 270.0f, 0.0f);
                        this.cam0.setMode(-1);
                        this.EV_Camera01();
                        this.player.mtn(26, 1, 1.0f, true);
                        System.sleep(20);
                        Sound.effectPlay(196751);
                        System.sleep(80);
                        this.EF03.disp(false);
                        this.npc20.kickEnepc(10, 0, 10);
                        this.npc21.kickEnepc(10, 0, 10);
                        this.moooon2();
                        System.sleep(10);
                        this.npc20.setVisible(false);
                        this.npc21.setVisible(false);
                        System.sleep(25);
                        this.npc3.enableDTKFlag(2);
                        this.EF03.disp(true);
                        this.cam0.setMode(0);
                        this.player.setTranslate(26.0f, 0.025999f, 1.0329556f);
                        Runtime.setPlayerControl(true);
                        Runtime.disable(65536);
                        this.Loc_flg = 0;
                        this.BUTTON_F = 0;
                        break;
                    }
                    default: {
                        Runtime.setPlayerControl(true);
                        this.BUTTON_F = 0;
                    }
                }
                this.BUTTON_F = 0;
                break;
            }
            case 4: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Loc_6_0, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        if (this.KAKUHEKI_F == 0) {
                            Runtime.setPlayerControl(false);
                            this.cam0.setMode(-1);
                            this.EV_Camera04();
                            this.player.setTranslate(-0.479f, 0.0f, 19.29f);
                            this.player.setRotate(0.0f, 180.0f, 0.0f);
                            this.Kakuheki_Close();
                            Runtime.setFlags(7013, 1, 1);
                            this.BUTTON_F = 0;
                            return;
                        }
                        if (this.KAKUHEKI_F != 1) break;
                        Runtime.setPlayerControl(false);
                        Runtime.enable(65536);
                        this.cam0.setMode(-1);
                        this.EV_Camera04();
                        this.player.setTranslate(-0.479f, 0.0f, 19.29f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.Kakuheki_Open();
                        Runtime.setFlags(7013, 1, 0);
                        this.BUTTON_F = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                this.BUTTON_F = 0;
                break;
            }
            case 5: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.Loc_6_0, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        if (this.KAKUHEKI_F == 0) {
                            Runtime.setPlayerControl(false);
                            this.cam0.setMode(-1);
                            this.EV_Camera03();
                            this.player.setTranslate(6.514f, 0.0f, 19.328f);
                            this.player.setRotate(0.0f, 180.0f, 0.0f);
                            this.Kakuheki_Close();
                            Runtime.setFlags(7013, 1, 1);
                            this.BUTTON_F = 0;
                            return;
                        }
                        if (this.KAKUHEKI_F != 1) break;
                        Runtime.setPlayerControl(false);
                        this.cam0.setMode(-1);
                        this.EV_Camera03();
                        this.player.setTranslate(6.514f, 0.0f, 19.328f);
                        this.player.setRotate(0.0f, 180.0f, 0.0f);
                        this.Kakuheki_Open();
                        Runtime.setFlags(7013, 1, 0);
                        this.BUTTON_F = 0;
                        return;
                    }
                }
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                this.BUTTON_F = 0;
                break;
            }
            case 6: {
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                if (Runtime.getFlags(3216, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    Sound.effectPlay(55);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_01, 0);
                    System.waitFor(this.win);
                    Runtime.setFlags(3216, 1, 1);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3236, 1) == 0) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.SUB_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (Runtime.getFlags(3296, 1) != 0) break;
                Runtime.setPlayerControl(false);
                Sound.effectPlay(56);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.SUB_03, 0);
                System.waitFor(this.win);
                Runtime.setFlags(3296, 1, 1);
                Runtime.setPlayerControl(true);
                this.BUTTON_F = 0;
                break;
            }
            case 7: {
                if (this.epass == 0) break;
                System.println("on!!");
                this.epass = 0;
                break;
            }
            case 8: {
                if (Runtime.getFlags(26, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7065, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(23, 1) != 1 || Runtime.getFlags(7057, 2) != 2) break;
                Runtime.mailArriveSet(12);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7065, 1, 1);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Read email\nDon't read email");
                System.waitFor(this.menu);
                System.sleep(10);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Runtime.mailExec(1);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                    default: {
                        Runtime.setPlayerControl(true);
                        break;
                    }
                }
            }
            case 9: {
                if (this.smd == 0) {
                    Sound.sequenceStop(0, 1000);
                    Sound.sequencePlay(1, 127);
                    this.smd = 1;
                    break;
                }
                return;
            }
            case 10: {
                if (this.smd == 1) {
                    Sound.sequenceStop(1, 1000);
                    Sound.sequencePlay(0, 127);
                    this.smd = 0;
                    break;
                }
                return;
            }
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        this.Talk_npc1_1(window);
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        this.Talk_npc10_1(window);
    }

    void Talk_npc10_1(Window window) {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                window.print(this.msg013C6513, 0);
                ST0120.waitPage(window, 64);
                this.npc10.kickEnepc(7, 2);
                this.npc9.setMotion(0, 10);
                this.peep = 10;
                return;
            }
        }
        window.print(this.msg013CC4C0, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc10_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        this.Talk_npc11_1(window);
    }

    void Talk_npc11_1(Window window) {
        switch (this.peep) {
            case 0: {
                ++this.talkFlag11;
                switch (this.talkFlag11) {
                    case 1: {
                        window.print(this.msg001803F3, 0);
                        ST0120.waitPage(window, 64);
                        return;
                    }
                    case 2: {
                        window.print(this.msg001803F4, 0);
                        ST0120.waitPage(window, 64);
                        return;
                    }
                }
                window.print(this.msg001803F5, 0);
                ST0120.waitPage(window, 64);
                return;
            }
            case 10: {
                window.print(this.msg001803F6, 0);
                ST0120.waitPage(window, 64);
                return;
            }
        }
    }

    void Talk_npc11_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        this.Talk_npc12_1(window);
    }

    void Talk_npc12_1(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                window.print(this.msg002C83E7, 0);
                ST0120.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg002C83E8, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc12_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc13(Enepc enepc, Window window) {
        this.Talk_npc13_1(window);
    }

    void Talk_npc13_1(Window window) {
        window.print(this.msg10000001, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc13_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc14(Enepc enepc, Window window) {
        this.Talk_npc14_1(window);
    }

    void Talk_npc14_1(Window window) {
        ++this.talkFlag14;
        switch (this.talkFlag14) {
            case 1: {
                window.print(this.msg10000002, 0);
                ST0120.waitPage(window, 64);
                window.print(this.msg10000003, 0);
                ST0120.waitPage(window, 64);
                window.print(this.msg10000004, 0);
                ST0120.waitPage(window, 64);
                window.print(this.msg10000005, 0);
                ST0120.waitPage(window, 64);
                this.npc14.enableDTKFlag(4);
                return;
            }
        }
        window.print(this.msg10000006, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc14_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc15(Enepc enepc, Window window) {
        window.print(this.msgKAKUHEKI_TALK, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc15_1(Window window) {
        ++this.talkFlag15;
        switch (this.talkFlag15) {
            case 1: {
                window.print(this.msgKAKUHEKI_TALK, 0);
                ST0120.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000001, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc15_2(Window window) {
    }

    public void Talk_npc16(Enepc enepc, Window window) {
        this.Talk_npc16_1(window);
    }

    void Talk_npc16_1(Window window) {
        window.print(this.msg57AD5D50, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc16_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc17(Enepc enepc, Window window) {
        this.Talk_npc17_1(window);
    }

    void Talk_npc17_1(Window window) {
        window.print(this.msg57AD5D50, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc17_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc18(Enepc enepc, Window window) {
        this.Talk_npc18_1(window);
    }

    void Talk_npc18_1(Window window) {
        window.print(this.msg57AD5D50, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc18_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        window.print(this.msg002C83E6, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        this.Talk_npc2_1(window);
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                return;
            }
        }
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        if (this.Loc_flg == 0) {
            ++this.talkFlag31;
            switch (this.talkFlag31) {
                case 1: {
                    window.print(this.msg10738338, 0);
                    ST0120.waitPage(window, 64);
                    return;
                }
            }
            window.print(this.msg105DEF8B, 0);
            ST0120.waitPage(window, 64);
            --this.talkFlag31;
            --this.talkFlag31;
            return;
        }
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg10738331, 0);
                ST0120.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg1073E2DE, 0);
        ST0120.waitPage(window, 64);
        --this.talkFlag3;
        --this.talkFlag3;
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        this.Talk_npc4_1(window);
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                return;
            }
        }
    }

    void Talk_npc4_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        this.Talk_npc5_1(window);
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg004912CE, 0);
                ST0120.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg004912CD, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc6_2(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg125C0C55, 0);
                ST0120.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg125C0C56, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc6_2(Window window) {
        window.print(this.msg125C0C56, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        this.Talk_npc7_1(window);
    }

    void Talk_npc7_1(Window window) {
    }

    void Talk_npc7_2(Window window) {
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        this.Talk_npc8_1(window);
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                window.print(this.msg0048B31C, 0);
                ST0120.waitPage(window, 64);
                window.print(this.msg0048B31D, 0);
                ST0120.waitPage(window, 64);
                window.print(this.msg0048B31E, 0);
                ST0120.waitPage(window, 64);
                window.print(this.msg0048B31E1, 0);
                ST0120.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0048B320, 0);
                ST0120.waitPage(window, 64);
                window.print(this.msg0048B321, 0);
                ST0120.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00491222, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        this.Talk_npc9_1(window);
    }

    void Talk_npc9_1(Window window) {
        ++this.talkFlag9;
        switch (this.talkFlag9) {
            case 1: {
                window.print(this.msg00ED5B85, 0);
                ST0120.waitPage(window, 64);
                this.npc10.kickEnepc(7, 2);
                this.npc9.setMotion(0, 10);
                this.peep = 10;
                return;
            }
        }
        window.print(this.msg00EE3E9C, 0);
        ST0120.waitPage(window, 64);
    }

    void Talk_npc9_2(Window window) {
        window.print(this.msgNO2, 0);
        ST0120.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        switch (n) {
            case 0: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(100, 2);
                break;
            }
            case 2: {
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(100, 3);
                break;
            }
            case 3: {
                if (this.S015B != 1) break;
                this.fade.call(0);
                System.sleep(30);
                Runtime.jumpCF(40, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 24.0f, 0.0f, 22.0f, 0.0f);
        this.teiten1.SetBgm(196634);
        this.teiten2 = new Uwamono(28690, 22.0f, 0.0f, -20.0f, 0.0f);
        this.teiten2.SetBgm(196634);
        this.teiten3 = new Uwamono(28690, 30.0f, 0.0f, 1.0f, 0.0f);
        this.teiten3.SetBgm(196634);
        this.teiten4 = new Uwamono(28690, 25.0f, 0.0f, 1.0f, 0.0f);
        this.teiten4.SetBgm(196635);
        this.teiten5 = new Uwamono(28690, 20.5f, 0.0f, 13.0f, 0.0f);
        this.teiten5.SetBgm(196635);
        this.teiten6 = new Uwamono(28690, 20.5f, 0.0f, -9.0f, 0.0f);
        this.teiten6.SetBgm(196635);
        this.teiten7 = new Uwamono(28690, 18.0f, 0.0f, 1.0f, 0.0f);
        this.teiten7.SetBgm(196635);
        this.teiten8 = new Uwamono(28690, -14.0f, 0.0f, 16.0f, 0.0f);
        this.teiten8.SetBgm(196642);
        this.teiten9 = new Uwamono(28690, 18.0f, 0.0f, 1.0f, 0.0f);
        this.teiten9.SetBgm(196754);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setVisible(-1, true);
        this.SAVE = new Uwamono(28678, 14.5f, 0.0f, -24.0f);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFLockX(3, 21.0f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.01f, 0.01f);
        this.cam0.setCFLockX(5, 3.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.01f, 0.01f);
        this.cam0.setCFPedestal(7, 10.724f, 7.143f, -21.846f, 62.91f, -69.846f, -9.719f, 0.0f, 2.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(7, 1);
        this.cam0.setCFPedestal(8, 27.6133f, 5.6059f, 4.9391f, 62.358f, -60.2295f, 0.2437f, 0.0f, 2.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(8, 1);
        this.cam0.setCFPedestal(9, -6.92025f, 4.6719f, 28.7121f, 40.0f, -20.5988f, -30.9395f, 0.0f, 2.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(9, 1);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.cam0.setCFPedestal(11, 9.149f, 2.5f, -20.874f, 40.0f, -11.187f, 345.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(11, 0.01f, 0.01f);
        this.cam0.setCFAngle(12, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(12, 0.01f, 0.01f);
        this.cam0.setCFAngle(13, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(13, 100.0f, 100.0f);
        if (Runtime.getFlags(7042, 1) == 0) {
            this.elv = new Unit();
            this.elv.initElevator(126, 0.041666668f, 0.0f);
            this.elv.setArgs(1, 0, 1);
            this.elv.setArgs(12, 0.0f);
        } else {
            this.elv = new Unit();
            this.elv.initElevator(126, 0.041666668f, 5.0f);
            this.elv.setArgs(1, 0, 1);
        }
        Stage.setVisible(0, false);
        Stage.setVisible(1, false);
        Stage.setVisible(2, false);
        Stage.setVisible(3, false);
        Stage.setVisible(4, false);
        Stage.setVisible(5, false);
        Stage.setVisible(8, false);
        this.itembox = new Uwamono(28677, 10.0f, 0.0f, -31.0f, 180.0f, 1);
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.tobira_2();
        } else {
            this.tobira_1();
        }
        new Uwamono(170, 84);
        new Uwamono(111, 31);
        new Uwamono(112, 21);
        new Uwamono(162, 126);
        new Uwamono(166, 21);
        this.ueki01 = new Uwamono(169, 84);
        this.ueki01.SetSize(0.5f, 2.0f, 0.5f);
        this.ueki02 = new Uwamono(168, 84);
        this.ueki02.SetSize(0.5f, 2.0f, 0.5f);
        this.obj01 = new Uwamono(167, 6);
        this.obj01.SetSize(2.5f, 0.5f, 2.5f);
        this.monitor1 = new Object();
        this.monitor1.init(24613, 18.1f, 2.0f, 1.0f, 90.0f);
        this.monitor1.setArgs(0, 0.0f, 0.5f, 2.72f, 2.38f);
        this.monitor1.setArgs(1, 18001, 0, 256, 128);
        this.monitor1.setArgs(2, 100, 0, 0, -1);
        this.monitor1.setArgs(3, 0.0f, 0.0f, 0.0f, 0.0f);
        Stage.setVisible(109, false);
        this.monitor1.signal(1);
        this.monitor1.setScale(1.2f, 0.8f, 1.0f);
        this.EF01 = new Effect(1011, 0);
        this.EF01.disp(true);
        this.EF01.setLocation(6, 0);
        this.EF02 = new Effect(1011, 0);
        this.EF02.disp(true);
        this.EF02.setLocation(6, 1);
        this.EF03 = new Effect(1056, 0);
        this.EF03.disp(true);
        this.EF03.setLocation(6, 2);
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
        this.S014A = Runtime.getFlags(21, 1);
        this.S014B = Runtime.getFlags(22, 1);
        this.S015B = Runtime.getFlags(23, 1);
        this.YOBIDASI_F = Runtime.getFlags(7011, 1);
        if (this.S015B == 1) {
            this.npcset_1();
        } else {
            this.npcset_1();
        }
    }

    void moooon() {
        float f = 0.0f;
        float f2 = 0.0f;
        this.monitor1.signal(1);
        this.teiten9.SetBgm(196754);
        while (f <= 5.0f) {
            this.monitor1.setScale(1.2f, f2 + f * 2.7f * 6.0f / 100.0f, 1.0f);
            f += 1.0f;
            System.sleep(1);
        }
    }

    void moooon2() {
        float f = 1.0f;
        float f2 = 0.0f;
        this.teiten9.SetBgm(-1);
        Sound.effectPlay(196753);
        Sound.effectStop(196754);
        while (f >= 0.0f) {
            this.monitor1.setScale(1.2f, f2 + f * 2.7f * 6.0f / 100.0f, 1.0f);
            f -= 1.0f;
            System.sleep(1);
        }
        this.monitor1.signal(0);
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(526, 1, 0, 5, 7, 19.8f, 0.0f, 21.42f, 110.0f);
        this.npc3 = new NPC_NORMAL(526, 3, 0, 17, 7, 21.0f, 0.0f, -0.02f, 270.0f);
        this.npc5 = new NPC_NORMAL(519, 5, 0, 14, 3, 26.41f, 0.0f, 2.86f, 240.0f);
        this.npc6 = new NPC_NORMAL(526, 6, 0, 17, 7, -15.421f, 0.0f, 17.327f, 0.0f);
        this.npc8 = new NPC_NORMAL(523, 8, 0, 7, 12, -4.3f, 0.0f, -27.56f, 90.0f);
        this.npc9 = new NPC_NORMAL(523, 9, 0, 13, 5, 8.931f, 0.0f, -24.5f, 110.0f);
        this.npc10 = new NPC_NORMAL(519, 10, 0, 14, 3, 9.489f, 0.0f, -24.791f, 295.0f);
        this.npc11 = new NPC_NORMAL(519, 11, 0, 14, 3, 6.85f, 0.0f, -16.3f, 190.0f);
        this.npc12 = new NPC_NORMAL(523, 12, 0, 14, 12, 24.25f, 0.0f, -1.01f, 290.0f);
        this.npc13 = new NPC_NORMAL(519, 13, 0, 76, 8, -13.08f, 0.0f, 23.39f, 90.0f);
        this.npc14 = new NPC_NORMAL(523, 14, 0, 13, 12, -11.65f, 0.0f, 22.59f, 300.0f);
        this.npc15 = new NPC_NORMAL(519, 15, 0, 14, 3, 0.08f, 0.0f, 19.52f, 200.0f);
        this.npc3.setMotion(0, 10);
        this.npc3.disableDTKFlag(2);
        this.npc5.setMotion(0, 9);
        this.npc6.disableDTKFlag(2);
        this.npc6.enableDTKFlag(4);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(262148);
        this.npc9.setMotion(0, 9);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(262148);
        this.npc10.setMotion(0, 10);
        this.npc11.disableDTKFlag(2);
        this.npc11.setMotion(0, 10);
        this.npc12.setMotion(0, 10);
        this.npc12.disableDTKFlag(3);
        this.npc12.enableDTKFlag(4);
        this.npc13.setMotion(0, 12);
        this.npc13.disableDTKFlag(3);
        this.npc14.setMotion(0, 9);
        this.npc14.disableDTKFlag(3);
        this.npc15.setMotion(0, 9);
        this.npc15.disableDTKFlag(3);
        this.npc15.enableDTKFlag(4);
        this.npc20 = new NPC_NORMAL(1592, 20, 0, 13, 15, 19.0f, 1.8f, 0.7f, 35.0f);
        this.npc20.kickEnepc(10, 80, 0);
        this.npc20.setTP(500);
        this.npc20.setInvalidID(1);
        this.npc20.setMotion(0, 10);
        this.npc20.setShadow(0, 0);
        this.npc20.dispRadar(false);
        this.npc20.disableDTKFlag(131072);
        this.npc20.disableDTKFlag(65536);
        this.npc21 = new NPC_NORMAL(1595, 21, 0, 13, 15, 19.0f, 1.8f, 1.3f, 110.0f);
        this.npc21.kickEnepc(10, 80, 0);
        this.npc21.setTP(500);
        this.npc21.setInvalidID(1);
        this.npc21.setMotion(0, 9);
        this.npc21.setShadow(0, 0);
        this.npc21.dispRadar(false);
        this.npc21.disableDTKFlag(131072);
        this.npc21.disableDTKFlag(65536);
        this.npc1.talkto("Talk_npc1");
        this.npc3.talkto("Talk_npc3");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        this.npc13.talkto("Talk_npc13");
        this.npc14.talkto("Talk_npc14");
        this.npc15.talkto("Talk_npc15");
        if (Runtime.getFlags(7042, 1) == 1) {
            this.npc18 = new NPC_NORMAL(519, 11, 0, 0, 7, 100.0f, 100.0f, 100.0f, 0.0f);
            this.npc18.setInvalidID(1);
            this.npc18.start(1, "Down");
        }
    }

    void tobira_1() {
        this.doorA = new Uwamono(22, 42, '\u0001');
        new Uwamono(23, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(26, 42, '\u0001');
        new Uwamono(27, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(28, 42, '\u0001');
        new Uwamono(29, 42, '\u0002', this.doorC);
        this.doorD = new Uwamono(101, 42, '\u0001');
        new Uwamono(100, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(165, 40, '\u0001');
        new Uwamono(164, 40, '\u0001', this.doorE);
        this.doorF = new Uwamono(13, 40, '\u0004');
        this.doorG = new Uwamono(9, 40, '\u0001');
        this.doorA.SetDoorType('\u0002');
        this.doorF.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorG.SetDoorType('\u0004');
        if (Runtime.getFlags(7013, 1) == 0) {
            this.doorD.DoorOpen();
        } else if (Runtime.getFlags(7013, 1) == 1) {
            this.doorD.DoorClose();
        }
        if (Runtime.getFlags(7042, 1) == 1) {
            this.doorE.SetDoorType('\u0002');
        } else {
            this.doorE.SetDoorType('\u0004');
        }
    }

    void tobira_2() {
        this.doorA = new Uwamono(22, 42, '\u0001');
        new Uwamono(23, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(26, 42, '\u0001');
        new Uwamono(27, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(28, 42, '\u0001');
        new Uwamono(29, 42, '\u0002', this.doorC);
        this.doorD = new Uwamono(101, 42, '\u0001');
        new Uwamono(100, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(165, 40, '\u0001');
        new Uwamono(164, 40, '\u0001', this.doorE);
        this.doorF = new Uwamono(13, 40, '\u0004');
        this.doorG = new Uwamono(9, 40, '\u0001');
        this.doorD.SetDoorType('\u0002');
        this.doorF.SetDoorType('\u0002');
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorG.SetDoorType('\u0004');
        if (Runtime.getFlags(7013, 1) == 0) {
            this.doorD.DoorOpen();
        } else if (Runtime.getFlags(7013, 1) == 1) {
            this.doorD.DoorClose();
        }
        if (Runtime.getFlags(7042, 1) == 1) {
            this.doorE.SetDoorType('\u0002');
        } else {
            this.doorE.SetDoorType('\u0004');
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
            this.setElevatorMode(1);
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

        void Down() {
            Sound.effectPlay(196716);
            ST0120.this.cam0.setMode(-1);
            ST0120.this.EV_Camera00();
            ST0120.this.player.setRotateY(5.0f);
            Runtime.setPlayerControl(false);
            ST0120.this.player.setLocation(1, 6);
            ST0120.this.elv.setArgs(12, 0.0f);
            System.sleep(120);
            Sound.effectStop(196716);
            Sound.effectPlay(196717);
            ST0120.this.doorE.DoorOpen();
            Runtime.setFlags(7042, 1, 0);
            ST0120.this.cam0.setMode(0);
            Runtime.enable(65536);
            ST0120.this.player.mtn(2, 9, 1.0f, true);
            ST0120.this.player.move(40, -14.0f, 17.03f, true);
            System.sleep(40);
            Runtime.disable(65536);
            ST0120.this.doorE.DoorClose();
            System.sleep(20);
            ST0120.this.doorE.SetDoorType('\u0004');
            Runtime.setPlayerControl(true);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void Up() {
            int n = 0;
            while (n < 30) {
                ST0120.this.ele.getTranslate();
                ST0120.this.ele.setTranslate(this.px, this.py + 0.005f * (float) n, this.pz);
                System.sleep(1);
                ++n;
            }
            this.stop();
        }
    }
}

