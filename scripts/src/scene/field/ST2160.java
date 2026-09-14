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
import xeno.map.MC_KUK16_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2160
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK16_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Unit unit;
    Unit Kidou;
    Menu menu;
    Window win;
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
    Enepc npc20;
    Enepc npc21;
    Enepc EXP1;
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
    int count = 0;
    int selected = 0;
    Uwamono doorA;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
    Light light = new Light(0);
    Effect gaitou1;
    Effect gaitou2;
    Effect gaitou3;
    int page;
    String[] msgSHION1 = new String[]{"/[label(Shion)]", "I think I'll rest a little at an inn before going back to the Durandal.", "/[waitkey(64)]/[close()]"};
    String[] msgCHAOS1 = new String[]{"/[label(chaos)]", "We should let Shion rest before going back to the Durandal.", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO1 = new String[]{"/[label(MOMO)]", "Shion doesn't seem well, so I think it would be better if she rested at an inn.", "/[waitkey(64)]/[close()]"};
    String[] msgZIGGY1 = new String[]{"/[label(Ziggy)]", "We cannot let Shion push herself too hard. We should her rest at an inn.", "/[waitkey(64)]/[closelear()]"};
    String[] msgJR1 = new String[]{"/[label(Jr.)]", "We can't let Shion push herself too much. Guess we ought to take a break at an inn.", "/[waitkey(64)]/[close()]"};
    String[] msgGO_DYU = new String[]{"/[label()]", "This ship is exclusively for direct flights to the Durandal. Go to the Durandal?", "/[waitkey(64)]/[close()]"};
    String[] msgTEST = new String[]{"/[label()]", "Hey, hey, there's work going on here! Don't come in, it's dangerous.", "/[waitkey(64)]/[close()]"};
    String[] msg10001 = new String[]{"/[label()]", "This port is exclusively for departures and arrivals of shuttles between the Durandal and the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] msg11001 = new String[]{"/[label()]", "The Gnosis are attacking! Quit dragging your feet and get in line for the evacuation launches!", "/[waitkey(64)]/[close()]"};
    String[] msg12001 = new String[]{"/[label()]", "Looks like the Gnosis in the city have been driven back. It's safe here now I think...", "/[waitkey(64)]/[close()]"};
    String[] msg20001 = new String[]{"/[label()]", "Those of you taking the shuttle to the Durandal, please board the vessel.", "/[waitkey(64)]/[close()]"};
    String[] msg21001 = new String[]{"/[label()]", "It is dangerous, so please stand in a uniform line! People in the back, don't push the people up front! Everyone will be evacuated. Please stay calm!", "/[waitkey(64)]/[close()]"};
    String[] msg22001 = new String[]{"/[label()]", "The Gnosis in the city have been driven back, but there are swarms of Gnosis pressing towards us outside the Foundation. Please take shelter here until all the\nGnosis have been eliminated.", "/[waitkey(64)]/[close()]"};
    String[] msg30001 = new String[]{"/[label()]", "I came here from Second Miltia as a maintenance engineer for the Environmental Bug when the Foundation was being built. Even after the construction was\nover, I remained on the Foundation. This is a great place, and everyone here enjoys life.", "/[waitkey(64)]/[close()]"};
    String[] msg31001 = new String[]{"/[label()]", "Our city...", "/[waitkey(1)]/[close()]", "Oh, how can this be...? You guys better run quickly. I will remain here. I will share the fate of this city.", "/[waitkey(64)]/[close()]"};
    String[] msg32001 = new String[]{"/[label()]", "It seems like we've managed to survive somehow, this city and I.", "/[waitkey(64)]/[close()]"};
    String[] msg40001 = new String[]{"/[label()]", "Beyond the launch is the Durandal. It's like a fairy-tale castle. Isn't it wonderful?\n", "/[waitkey(64)]/[close()]"};
    String[] msg41001 = new String[]{"/[label()]", "Aaaaah! I'm sick of this!!", "/[waitkey(64)]/[close()]"};
    String[] msg42001 = new String[]{"/[label()]", "Hey, did you see? Did you see that? Gnosis are super creepy.", "/[waitkey(64)]/[close()]"};
    String[] msg50001 = new String[]{"/[label()]", "The Life Recycling Act gave rise to many people with abnormal powers.", "/[waitkey(1)]/[clear()]", "These people became the target of persecution and discrimination because their abilities, outer appearances, and air about them were a little different.", "/[waitkey(1)]/[clear()]", "Normal people were probably afraid of the possibility that they could eventually be ruled by these people.", "/[waitkey(64)]/[close()]"};
    String[] msg50002 = new String[]{"/[label()]", "A law that was created to make people happier ended up making people suffer.", "/[waitkey(1)]/[clear()]", "After the Species Preservation Act was instituted, those types of instances stopped happening on the surface, but the situation hasn't really changed.", "/[waitkey(1)]/[clear()]", "I was looking for the place where I fit in.", "/[waitkey(64)]/[close()]"};
    String[] msg50003 = new String[]{"/[label()]", "Everything has changed drastically since I came here, however.", "/[waitkey(1)]/[clear()]", "It soothed my scarred and cowardly heart.", "/[waitkey(1)]/[clear()]", "I learned that people can respect one another.", "/[waitkey(64)]/[close()]"};
    String[] msg50004 = new String[]{"/[label()]", "I think Realians are treated the same.", "/[waitkey(1)]/[clear()]", "Perhaps the Realians feel the same way.", "/[waitkey(64)]/[close()]"};
    String[] msg51001 = new String[]{"/[label()]", "Help! Please! Forgive me! I don't want to die!", "/[waitkey(64)]/[close()]"};
    String[] msg52001 = new String[]{"/[label()]", "Please pardon me for losing it back there. I get like that when I get scared.", "/[waitkey(64)]/[close()]"};
    String[] msg60001 = new String[]{"/[label()]", "Before coming here, both she and I were shunned by society.", "/[waitkey(1)]/[clear()]", "I can understand that they felt threatened by people with special powers, but we never had any intentions to use our powers against them.", "/[waitkey(64)]/[close()]"};
    String[] msg60002 = new String[]{"/[label()]", "/[waitkey(64)]/[close()]", "I didn't become like this by choice.", "/[waitkey(1)]/[clear()]", "I didn't understand why everyone treated me...I used to resent the world...no, I hated people.", "/[waitkey(1)]/[clear()]", "It was Representative Director Gaignun and Little Master that opened my hardened heart.", "/[waitkey(1)]/[clear()]", "They taught us the meaning of life.", "/[waitkey(64)]/[close()]"};
    String[] msg61001 = new String[]{"/[label()]", "Damn! Those monsters totally wrecked our city!", "/[waitkey(64)]/[close()]"};
    String[] msg62001 = new String[]{"/[label()]", "I witnessed it! It's amazing that you actually drove away the Gnosis!", "/[waitkey(64)]/[close()]"};
    String[] msg71001 = new String[]{"/[label()]", "The city is swarming with Gnosis. We better get out of here or we'll be in danger too!", "/[waitkey(64)]/[close()]"};
    String[] msg72001 = new String[]{"/[label()]", "Staying put was the right thing to do. Well, it was too crowded for us to move anyway.", "/[waitkey(64)]/[close()]"};
    String[] msg81001 = new String[]{"/[label()]", "I won't eat snacks I haven't paid for anymore! I promise to help my mother! And I'll do homework! So please, god, help!", "/[waitkey(64)]/[close()]"};
    String[] msg82001 = new String[]{"/[label()]", "God, you granted my wish, didn't you? Thank you.", "/[waitkey(1)]/[clear()]", "Then I better keep the promises I made to you too. Well, how about I start tomorrow...", "/[waitkey(64)]/[close()]"};
    String[] msg91001 = new String[]{"/[label()]", "My kid sister, Liza, was killed by t-the monster...right in front of my eyes. Oh, how could this happen?!", "/[waitkey(64)]/[close()]"};
    String[] msg92001 = new String[]{"/[label()]", "Liza...there was nothing I could do.", "/[waitkey(1)]/[clear()]", "I was so afraid, I left you behind as you were being attacked to save myself...", "/[waitkey(1)]/[clear()]", "Please forgive me...\n", "*sob...*", "/[waitkey(64)]/[close()]"};
    String[] msg101001 = new String[]{"/[label()]", "It's times like these that you have to stay calm, cool, and collected. Please remain calm and follow the police officers' instructions.", "/[waitkey(64)]/[close()]"};
    String[] msg102001 = new String[]{"/[label()]", "If it came down to it, I was prepared to protect the citizens, even if I had to sacrifice my own life.", "/[waitkey(1)]/[clear()]", "But it seems most of the Gnosis were wiped out before\nI got here.", "/[waitkey(64)]/[close()]"};
    String[] msg111001 = new String[]{"/[label()]", "What are we going to do?! If I don't hurry and run, those Gnosis monsters will come!", "/[waitkey(64)]/[close()]"};
    String[] msg112001 = new String[]{"/[label()]", "I was lucky that the launch pad was crowded and\nI couldn't move.", "/[waitkey(1)]/[clear()]", "If I'd made one wrong move, I might have been attacked by a Gnosis! I could have died!", "/[waitkey(64)]/[close()]"};
    String[] msg121001 = new String[]{"/[label()]", "It's over! It's no good! The Gnosis are going to kill us all!", "/[waitkey(64)]/[close()]"};
    String[] msg122001 = new String[]{"/[label()]", "Oh, thank you god. You watched over me because I never failed to do my daily prayers.", "/[waitkey(64)]/[close()]"};
    String[] msg131001 = new String[]{"/[label()]", "We should be safe here...please don't cry anymore.", "/[waitkey(64)]/[close()]"};
    String[] msg132001 = new String[]{"/[label()]", "It's okay now. All the scary things are gone.", "/[waitkey(64)]/[close()]"};
    String[] msg141001 = new String[]{"/[label()]", "I'm scared, Mommy...", "/[waitkey(64)]/[close()]"};
    String[] msg142001 = new String[]{"/[label()]", "Are they gone now? Are the scary things really gone?", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_11 = new String[]{"/[label(Shion)]", "Oh? How strange...", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_2 = new String[]{"/[label(Ziggy)]", "Is something the matter?", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_12 = new String[]{"/[label(Shion)]", "MOMO isn't here. Where could she have gone?", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_21 = new String[]{"/[label(chaos)]", "Huh? That's odd...", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_22 = new String[]{"/[label(chaos)]", "MOMO isn't here. Where could she have gone?", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_31 = new String[]{"/[label(Jr.)]", "Hmm? Weird...", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_32 = new String[]{"/[label(Jr.)]", "MOMO's gone. Where'd she go?", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_41 = new String[]{"/[label(Ziggy)]", "Something is wrong...", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_42 = new String[]{"/[label(Ziggy)]", "I don't see MOMO anywhere. Where did she go?", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_51 = new String[]{"/[label(KOS-MOS)]", "I have no readings of the 100-Series prototype.", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_52 = new String[]{"/[label(KOS-MOS)]", "I believe she no longer at this location.", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_NO1 = new String[]{"/[label(Shion)]", "I don't think she would go back to the city.", "/[waitkey(1)]/[clear()]", "We might have missed her along the way, so we better make a trip back to the Durandal.", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_NO2 = new String[]{"/[label(chaos)]", "I don't think she would go back to the city.", "/[waitkey(1)]/[clear()]", "We might have missed her along the way, so maybe we should go back to the Durandal?", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_NO3 = new String[]{"/[label(Jr.)]", "I don't think she'd go back to the city.", "/[waitkey(1)]/[clear()]", "We might've missed her along the way. Guess we ought to head back to the Durandal.", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_NO4 = new String[]{"/[label(Ziggy)]", "I find it hard to believe that she went back into the city.", "/[waitkey(1)]/[clear()]", "Did we miss her somehow? We better make a trip back to the Durandal.", "/[waitkey(64)]/[close()]"};
    String[] msgMOMO_NO5 = new String[]{"/[label(KOS-MOS)]", "There are no signs that she returned to the city.", "/[waitkey(1)]/[clear()]", "Before we start a full-scale search, it would be best to return to the Durandal.", "/[waitkey(64)]/[close()]"};

    ST2160() {
    }

    void EV_Camera00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-15.956f, 4.079f, 16.742f);
        this.camEV.setRotate(-1.3473f, -52.659f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-18.461f, 8.591f, 13.971f);
        this.camEV.setRotate(-12.374f, -424.074f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-11.419f, 6.063f, 14.477f);
        this.camEV.setRotate(-14.838f, -49.422f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void EV_Camera03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-8.644f, 2.571f, 11.315f);
        this.camEV.setRotate(-13.796f, 30.957f, 0.0f);
        this.camEV.setFov(34.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
        switch (n) {
            case 32: {
                if (Runtime.getFlags(7138, 1) != 0) break;
                this.EXP1.kickEnepc(4, 1);
                this.EXP1.kickEnepc(3, 1, 45, 45, 1, 100);
                break;
            }
        }
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                this.player.getTranslate();
                if (this.player.py < 2.9f) {
                    return;
                }
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgGO_DYU, 0);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Yes\nNo");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        Sound.streamPlay(1195010, 48000);
                        this.EXP1.kickEnepc(0, 2);
                        System.sleep(45);
                        this.EXP1.kickEnepc(3, 2, 45, 45, 1, 100);
                        this.cam0.setMode(-1);
                        Runtime.enable(65536);
                        this.player.setTranslate(-100.0f, -0.0f, -1.0f);
                        Runtime.disable(65536);
                        System.sleep(40);
                        this.EV_Camera01();
                        System.sleep(30);
                        this.EXP1.kickEnepc(0, 0);
                        System.sleep(70);
                        this.fade.call(0);
                        System.sleep(25);
                        this.EXP1.kickEnepc(3, 0, 100, 100, 1, 100);
                        System.sleep(5);
                        Runtime.setPlayerControl(true);
                        Runtime.setFlags(3108, 1, 1);
                        Runtime.setFlags(7138, 1, 1);
                        if (Runtime.getFlags(7163, 1) == 1) {
                            Runtime.setFlags(7163, 1, 0);
                        }
                        if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(362, 1) == 0 && Runtime.getFlags(389, 1) == 0) {
                            Runtime.jumpCF(1802, 4);
                            break;
                        }
                        if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(362, 1) == 1 && Runtime.getFlags(389, 1) == 0) {
                            if (Runtime.getFlags(3191, 1) == 1) {
                                Runtime.setFlags(3191, 1, 0);
                                Runtime.jumpCF(1802, 4);
                                break;
                            }
                            Runtime.jumpCF(1805, 4);
                            break;
                        }
                        if (Runtime.getFlags(360, 1) == 1 && Runtime.getFlags(362, 1) == 1 && Runtime.getFlags(389, 1) == 1) {
                            Runtime.jumpCF(1803, 4);
                            break;
                        }
                        Runtime.jumpCF(1800, 4);
                        break;
                    }
                    default: {
                        Runtime.enable(65536);
                        this.player.mtn(2, 9, 1.0f, true);
                        this.player.move(30, -1.61f, 7.46f, true);
                        System.sleep(35);
                        Runtime.disable(65536);
                        Runtime.setPlayerControl(true);
                        break;
                    }
                }
            }
            case 1: {
                if (Runtime.getFlags(3162, 1) != 1) break;
                Runtime.setPlayerControl(false);
                this.fade.call(0);
                System.sleep(30);
                this.player.look_char(this.npc21);
                this.npc20.setVisible(true);
                this.npc21.setVisible(true);
                Runtime.enable(65536);
                this.player.setTranslate(-11.08f, 0.45f, 6.96f);
                this.cam0.setMode(-1);
                this.EV_Camera03();
                this.player.rotY(20, 320.0f, true);
                System.sleep(30);
                this.player.look_char(this.npc20);
                System.sleep(30);
                this.player.look_char(this.npc21);
                System.sleep(30);
                this.player.mtn(11, 9, 1.0f, true);
                this.TALK_1();
                this.player.look_char(this.npc20);
                this.TALK_2();
                System.sleep(15);
                this.fade.call(0);
                System.sleep(30);
                this.cam0.setMode(0);
                this.player.look_default();
                Runtime.setFlags(3163, 1, 1);
                Runtime.setFlags(3162, 1, 0);
                Runtime.setFlags(7163, 1, 1);
                this.npc20.setVisible(false);
                this.npc21.setVisible(false);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                break;
            }
            case 2: {
                if (Runtime.getFlags(7163, 1) != 1) break;
                Runtime.setPlayerControl(false);
                this.TALK_3();
                Runtime.enable(65536);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(30, -25.74f, 0.16f, true);
                System.sleep(35);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    void TALK_1() {
        if (Runtime.getLeader() == 1) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_11, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 3) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_21, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 5) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_31, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 6) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_41, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 2) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_51, 0);
            ST2160.waitPage(this.win, 64);
        }
    }

    void TALK_2() {
        if (Runtime.getLeader() == 1) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_12, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 3) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_22, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 5) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_32, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 6) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_42, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 2) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_52, 0);
            ST2160.waitPage(this.win, 64);
        }
    }

    void TALK_3() {
        if (Runtime.getLeader() == 1) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_NO1, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 3) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_NO2, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 5) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_NO3, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 6) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_NO4, 0);
            ST2160.waitPage(this.win, 64);
        } else if (Runtime.getLeader() == 2) {
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgMOMO_NO5, 0);
            ST2160.waitPage(this.win, 64);
        }
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc1_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    public void Talk_npc10(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc10_2(window);
        } else {
            this.Talk_npc10_1(window);
        }
    }

    void Talk_npc10_1(Window window) {
        window.print(this.msg101001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc10_2(Window window) {
        window.print(this.msg102001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc11_2(window);
        } else {
            this.Talk_npc11_1(window);
        }
    }

    void Talk_npc11_1(Window window) {
        window.print(this.msg111001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc11_2(Window window) {
        window.print(this.msg112001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc12_2(window);
        } else {
            this.Talk_npc12_1(window);
        }
    }

    void Talk_npc12_1(Window window) {
        window.print(this.msg121001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc12_2(Window window) {
        window.print(this.msg122001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc13(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc13_2(window);
        } else {
            this.Talk_npc13_1(window);
        }
    }

    void Talk_npc13_1(Window window) {
        window.print(this.msg131001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc13_2(Window window) {
        window.print(this.msg132001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc14(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc14_2(window);
        } else {
            this.Talk_npc14_1(window);
        }
    }

    void Talk_npc14_1(Window window) {
        window.print(this.msg141001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc14_2(Window window) {
        window.print(this.msg142001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        window.print(this.msg10001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        window.print(this.msg11001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc1_3(Window window) {
        window.print(this.msg12001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc2_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msg20001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msg21001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
        window.print(this.msg22001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc3_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc3_2(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        window.print(this.msg30001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc3_2(Window window) {
        window.print(this.msg31001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        window.print(this.msg32001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc4_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc4_2(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msg40001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        window.print(this.msg41001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc4_3(Window window) {
        window.print(this.msg42001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc5_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc5_2(window);
        } else {
            this.Talk_npc5_1(window);
        }
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg50001, 0);
                ST2160.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg50002, 0);
                ST2160.waitPage(window, 64);
                return;
            }
            case 3: {
                window.print(this.msg50003, 0);
                ST2160.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg50004, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        window.print(this.msg51001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc5_3(Window window) {
        window.print(this.msg52001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc6_3(window);
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.Talk_npc6_2(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msg60001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc6_2(Window window) {
        window.print(this.msg61001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc6_3(Window window) {
        window.print(this.msg62001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc7_2(window);
        } else {
            this.Talk_npc7_1(window);
        }
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msg71001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        window.print(this.msg72001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc8_2(window);
        } else {
            this.Talk_npc8_1(window);
        }
    }

    void Talk_npc8_1(Window window) {
        window.print(this.msg81001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc8_2(Window window) {
        window.print(this.msg82001, 0);
        ST2160.waitPage(window, 64);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (Runtime.getFlags(373, 1) == 1) {
            this.Talk_npc9_2(window);
        } else {
            this.Talk_npc9_1(window);
        }
    }

    void Talk_npc9_1(Window window) {
        window.print(this.msg91001, 0);
        ST2160.waitPage(window, 64);
    }

    void Talk_npc9_2(Window window) {
        window.print(this.msg92001, 0);
        ST2160.waitPage(window, 64);
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("ミニマップ・１");
                Runtime.jumpCF(2130, 1);
                break;
            }
        }
    }

    void init() {
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.425f, 0.425f, 0.425f);
        this.light.setColor(1, 0.425f, 0.425f, 0.425f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.425f, 0.425f, 0.425f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.425f, 0.425f, 0.425f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 1, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 2, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightCol(1, 3, 0.375f, 0.375f, 0.375f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, -12.5f, 0.0f, 10.75f, 35.0f);
        this.cam0.setCFHokan(1, 0.02f, 0.02f);
        this.cam0.setCFAngle(2, -28.0f, -12.5f, 0.0f, 13.5f, 35.0f);
        this.cam0.setCFHokan(2, 0.02f, 0.02f);
        this.cam0.setCFPedestal(3, -33.02979f, 7.3910017f, 18.252514f, 27.051203f, -15.364027f, 331.79648f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 100.0f, 100.0f);
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
        this.gaitou1 = new Effect(1405, -26.098f, 5.7f, 10.267f, 0.0f);
        this.gaitou1.setScale(0.8f, 0.25f, 1.5f);
        this.gaitou1.setRotate(40.0f, 0.0f, 0.0f);
        this.gaitou2 = new Effect(1405, -16.074f, 5.7f, 10.267f, 0.0f);
        this.gaitou2.setScale(0.8f, 0.25f, 1.5f);
        this.gaitou2.setRotate(40.0f, 0.0f, 0.0f);
        this.gaitou3 = new Effect(1405, -10.796f, 5.7f, 10.267f, 0.0f);
        this.gaitou3.setScale(0.8f, 0.25f, 1.5f);
        this.gaitou3.setRotate(40.0f, 0.0f, 0.0f);
        if (Runtime.getFlags(373, 1) == 1) {
            this.npcset_3();
        } else if (Runtime.getFlags(360, 1) == 1) {
            this.npcset_2();
        } else if (Runtime.getFlags(301, 1) == 1) {
            this.npcset_1();
        } else {
            this.npcset_1();
        }
        this.doorA = new Uwamono(61, 40, '\u0001');
        this.doorA.SetDoorType('\u0001');
        this.doorA.SetDoorScope(1.0f);
        this.doorA.SetSize(1.0f, 1.0f, 1.0E-4f);
        if (Runtime.getFlags(7138, 1) == 1) {
            this.EXP1 = new Enepc();
            this.EXP1.init(20491, 32, 0.0f, 4.0f, 5.25f, 270.0f);
            this.EXP1.id = 32;
            this.EXP1.setParams(0, 79, 32, 22);
            this.EXP1.setInvalidID(1);
            this.EXP1.disableDTKFlag(65536);
            this.EXP1.disableDTKFlag(131072);
            this.EXP1.dispRadar(false);
        } else {
            this.EXP1 = new Enepc();
            this.EXP1.init(20491, 32, 0.0f, 2.3f, 5.25f, 270.0f);
            this.EXP1.id = 32;
            this.EXP1.setParams(0, 79, 32, 22);
            this.EXP1.setInvalidID(1);
            this.EXP1.disableDTKFlag(65536);
            this.EXP1.disableDTKFlag(131072);
            this.EXP1.dispRadar(false);
        }
        if (Runtime.getFlags(7138, 1) == 1) {
            this.Kidou = new Mapunits();
            this.Kidou.mapUnit(2);
            this.Kidou.start(4, null);
            this.Kidou.start(1, "FROM_DYU");
        }
    }

    void npcset_0() {
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(1289, 1, 0, 14, 11, -22.585f, 0.45f, 0.24f, 0.0f);
        this.npc2 = new NPC_NORMAL(1289, 2, 0, 14, 11, -10.34f, 0.45f, 2.92f, 270.0f);
        this.npc3 = new NPC_NORMAL(1549, 3, 0, 14, 16, -17.34f, 0.45f, 5.98f, 180.0f);
        this.npc4 = new NPC_NORMAL(1585, 4, 0, 0, 15, -22.86f, 0.45f, 3.92f, 0.0f);
        this.npc5 = new NPC_NORMAL(1592, 5, 0, 14, 11, -13.26f, 0.45f, 0.6f, 340.0f);
        this.npc6 = new NPC_NORMAL(1561, 6, 0, 14, 12, -13.75f, 0.45f, 1.49f, 120.0f);
        this.npc1.disableDTKFlag(2);
        this.npc1.enableDTKFlag(4);
        this.npc2.disableDTKFlag(2);
        this.npc2.enableDTKFlag(4);
        this.npc3.disableDTKFlag(2);
        this.npc3.enableDTKFlag(4);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 9);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 10);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
    }

    void npcset_2() {
        this.npc2 = new NPC_NORMAL(1289, 2, 0, 14, 11, -9.93f, 0.45f, 4.59f, 270.0f);
        this.npc3 = new NPC_NORMAL(1549, 3, 0, 14, 18, -17.34f, 0.45f, 5.98f, 180.0f);
        this.npc5 = new NPC_NORMAL(1592, 5, 0, 14, 26, -13.26f, 0.45f, 0.6f, 340.0f);
        this.npc6 = new NPC_NORMAL(1561, 6, 0, 14, 12, -13.75f, 0.45f, 1.49f, 160.0f);
        this.npc7 = new NPC_NORMAL(1543, 7, 0, 14, 25, -10.61f, 0.45f, 4.74f, 90.0f);
        this.npc8 = new NPC_NORMAL(1567, 8, 0, 14, 17, -11.69f, 0.45f, 4.72f, 70.0f);
        this.npc9 = new NPC_NORMAL(1593, 9, 0, 14, 26, -13.26f, 0.45f, 4.73f, 120.0f);
        this.npc10 = new NPC_NORMAL(1288, 10, 0, 14, 25, -12.14f, 0.45f, 5.41f, 200.0f);
        this.npc11 = new NPC_NORMAL(1591, 11, 0, 14, 18, -23.37f, 0.44f, 5.37f, 100.0f);
        this.npc12 = new NPC_NORMAL(1592, 12, 0, 14, 18, -22.32f, 0.45f, 5.3f, 340.0f);
        this.npc2.disableDTKFlag(2);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 9);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 6);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 27);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 9);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 9);
        this.npc8.disableDTKFlag(3);
        this.npc8.setMotion(0, 27);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setMotion(0, 27);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 9);
        this.npc11.disableDTKFlag(3);
        this.npc11.setMotion(0, 2);
        this.npc12.disableDTKFlag(3);
        this.npc12.enableDTKFlag(4);
        this.npc12.setMotion(0, 3);
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        if (Runtime.getFlags(3162, 1) == 1) {
            this.npc20 = new NPC_NORMAL(6, 20, 0, 14, 25, -26.74f, 0.45f, 0.25f, 0.0f);
            this.npc21 = new NPC_NORMAL(6, 21, 0, 14, 25, -10.23f, 0.45f, 11.42f, 0.0f);
            this.npc20.enableDTKFlag(262144);
            this.npc20.setVisible(false);
            this.npc20.disableDTKFlag(131072);
            this.npc20.disableDTKFlag(65536);
            this.npc20.setInvalidID(1);
            this.npc21.setVisible(false);
            this.npc21.enableDTKFlag(262144);
            this.npc21.disableDTKFlag(131072);
            this.npc21.disableDTKFlag(65536);
            this.npc21.setInvalidID(1);
        } else {
            this.npc1 = new NPC_NORMAL(1289, 1, 0, 20, 13, -22.585f, 0.45f, 0.24f, 0.0f);
            this.npc4 = new NPC_NORMAL(1585, 4, 0, 20, 14, -22.86f, 0.45f, 3.92f, 0.0f);
            this.npc13 = new NPC_NORMAL(1576, 13, 0, 14, 18, -14.19f, 0.45f, 4.53f, 340.0f);
            this.npc14 = new NPC_NORMAL(1555, 14, 0, 14, 11, -14.5f, 0.45f, 5.2f, 130.0f);
            this.npc13.disableDTKFlag(3);
            this.npc13.setMotion(0, 2);
            this.npc13.look_char(this.npc14);
            this.npc14.disableDTKFlag(3);
            this.npc14.look_char(this.npc13);
            this.npc1.talkto("Talk_npc1");
            this.npc4.talkto("Talk_npc4");
            this.npc13.talkto("Talk_npc13");
            this.npc14.talkto("Talk_npc14");
        }
    }

    void npcset_3() {
        this.npc2 = new NPC_NORMAL(1289, 2, 0, 14, 11, -9.93f, 0.45f, 4.59f, 270.0f);
        this.npc3 = new NPC_NORMAL(1549, 3, 0, 14, 16, -17.34f, 0.45f, 5.98f, 180.0f);
        this.npc5 = new NPC_NORMAL(1592, 5, 0, 14, 11, -13.26f, 0.45f, 0.6f, 340.0f);
        this.npc6 = new NPC_NORMAL(1561, 6, 0, 14, 12, -13.75f, 0.45f, 1.49f, 160.0f);
        this.npc7 = new NPC_NORMAL(1543, 7, 0, 14, 25, -10.61f, 0.45f, 4.74f, 90.0f);
        this.npc8 = new NPC_NORMAL(1567, 8, 0, 14, 18, -11.69f, 0.45f, 4.72f, 70.0f);
        this.npc9 = new NPC_NORMAL(1593, 9, 0, 14, 26, -13.26f, 0.45f, 4.73f, 120.0f);
        this.npc10 = new NPC_NORMAL(1288, 10, 0, 14, 25, -12.14f, 0.45f, 5.41f, 200.0f);
        this.npc11 = new NPC_NORMAL(1591, 11, 0, 14, 18, -23.37f, 0.44f, 5.37f, 100.0f);
        this.npc12 = new NPC_NORMAL(1592, 12, 0, 14, 18, -22.32f, 0.45f, 5.3f, 340.0f);
        this.npc2.disableDTKFlag(2);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 9);
        this.npc3.disableDTKFlag(2);
        this.npc3.enableDTKFlag(4);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 9);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 9);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 9);
        this.npc8.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc8.setMotion(0, 3);
        this.npc9.disableDTKFlag(3);
        this.npc9.enableDTKFlag(4);
        this.npc9.setMotion(0, 27);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 9);
        this.npc11.disableDTKFlag(3);
        this.npc11.setMotion(0, 2);
        this.npc12.disableDTKFlag(3);
        this.npc12.enableDTKFlag(4);
        this.npc12.setMotion(0, 3);
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc9.talkto("Talk_npc9");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc12.talkto("Talk_npc12");
        if (Runtime.getFlags(3162, 1) == 1) {
            this.npc20 = new NPC_NORMAL(6, 20, 0, 14, 25, -26.74f, 0.45f, 0.25f, 0.0f);
            this.npc21 = new NPC_NORMAL(6, 21, 0, 14, 25, -10.23f, 0.45f, 11.42f, 0.0f);
            this.npc20.enableDTKFlag(262144);
            this.npc20.setVisible(false);
            this.npc20.setInvalidID(1);
            this.npc20.disableDTKFlag(131072);
            this.npc20.disableDTKFlag(65536);
            this.npc21.setVisible(false);
            this.npc21.enableDTKFlag(262144);
            this.npc21.setInvalidID(1);
            this.npc21.disableDTKFlag(131072);
            this.npc21.disableDTKFlag(65536);
        } else {
            this.npc1 = new NPC_NORMAL(1289, 1, 0, 7, 13, -22.585f, 0.45f, 0.24f, 0.0f);
            this.npc4 = new NPC_NORMAL(1585, 4, 0, 20, 14, -22.86f, 0.45f, 3.92f, 0.0f);
            this.npc13 = new NPC_NORMAL(1576, 13, 0, 14, 18, -14.19f, 0.45f, 4.53f, 340.0f);
            this.npc14 = new NPC_NORMAL(1555, 14, 0, 14, 11, -14.5f, 0.45f, 5.2f, 130.0f);
            this.npc13.disableDTKFlag(3);
            this.npc13.setMotion(0, 2);
            this.npc13.look_char(this.npc14);
            this.npc14.disableDTKFlag(3);
            this.npc14.look_char(this.npc13);
            this.npc1.talkto("Talk_npc1");
            this.npc4.talkto("Talk_npc4");
            this.npc13.talkto("Talk_npc13");
            this.npc14.talkto("Talk_npc14");
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
        }
    }

    class Mapunits
            extends MAPUnit {
        Mapunits() {
        }

        void FROM_DYU() {
            Runtime.disable(524288);
            Runtime.setPlayerControl(false);
            ST2160.this.player.setTranslate(100.0f, 0.0f, 100.0f);
            ST2160.this.cam0.setMode(-1);
            ST2160.this.EV_Camera02();
            Sound.streamPlay(1195009, 48000);
            int n = 0;
            ST2160.this.EXP1.getTranslate();
            ST2160.this.EXP1.getRotate();
            while (true) {
                if (n >= 0 && n < 60) {
                    ST2160.this.EXP1.setTranslate(ST2160.this.EXP1.px, ST2160.this.EXP1.py - 0.028333334f, ST2160.this.EXP1.pz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
            System.sleep(61);
            System.sleep(60);
            Runtime.enable(65536);
            ST2160.this.player.setTranslate(-1.61f, 3.0f, 6.0f);
            System.sleep(5);
            ST2160.this.EXP1.kickEnepc(4, 1);
            ST2160.this.EXP1.kickEnepc(0, 1);
            System.sleep(45);
            ST2160.this.EXP1.kickEnepc(3, 1, 45, 45, 1, 100);
            System.sleep(30);
            ST2160.this.player.mtn(2, 9, 1.0f, true);
            ST2160.this.player.move(30, -1.61f, 7.46f, true);
            System.sleep(30);
            ST2160.this.cam0.setMode(0);
            Runtime.disable(65536);
            Runtime.setFlags(7138, 1, 0);
            Runtime.setPlayerControl(true);
            Runtime.enable(524288);
        }
    }
}

