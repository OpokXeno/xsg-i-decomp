import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.MAPUnit;
import xeno.Sound;
import xeno.Stage;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_UTK08_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1080
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_UTK08_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int selected = 0;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc enemy1;
    Enepc dummy;
    Effect Obj600;
    Effect Obj601;
    int talkFlag1 = 1;
    int talkFlag2 = 1;
    int talkFlag3 = 3;
    int lateFlag = 1;
    Uwamono doorA;
    Light light = new Light(0);
    Effect fade;
    Effect fade_eve;
    MAPUnit isu1;
    MAPUnit isu2;
    MAPUnit isu3;
    MAPUnit isu4;
    int flag_height = 1;
    int flag_width = 3;
    boolean flash1 = false;
    boolean flash2 = false;
    int NO3_CARDKEY = Runtime.checkItem(10, 3);
    int CF_CONTE_END = Runtime.getFlags(6037, 1);
    int IN1080 = Runtime.getFlags(6039, 1);
    int page;
    String[] NPC1_TALK1 = new String[]{"/[label(Durandal Crew Member 1)]", "I got attacked the moment I entered the room. It was a good thing this guy was weak. But...please be careful when you enter a new area.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_TALK2 = new String[]{"/[label(Durandal Crew Member 1)]", "It seems the U-TIC ship's bridge is beyond Door No. 3. But they closed it on us when we tried to charge in.", "/[waitkey(1)]/[clear()]", "It seems you need /[color(0x329bbe)]Card No. 3/[color(0x808080)] to open it, but I don't know where it's located...", "/[waitkey(64)]/[close()]"};
    String[] NPC2_TALK1 = new String[]{"/[label(Durandal Crew Member 2)]", "I don't know if the soldiers on the U-TIC ship are thoroughly prepared or cowardly, but they set up ambushes and hide behind things.", "/[waitkey(1)]/[clear()]", "We have had a lot of unexpected encounters. Don't let your guard down, Little Master.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_TALK2 = new String[]{"/[label(Durandal Crew Member 2)]", "Man, this guy's giving me a headache. This soldier knows the location of the key card, but he refuses to talk.", "/[waitkey(64)]/[close()]"};
    String[] LATE_TALK1 = new String[]{"Hello, Little Master. I'm sorry, but I can't drop what I'm doing right now. Looks like you got the key card. Good, good.", "/[waitkey(64)]/[close()]"};
    String[] LATE_TALK2 = new String[]{"What? That prisoner? He disappeared when I looked away for a second.", "/[waitkey(1)]/[clear()]", "I don't think he called additional reinforcements -- I think he probably just got in an escape pod and ran away.", "/[waitkey(64)]/[close()]"};
    String[] LATE_TALK3 = new String[]{"I am currently reporting to Shelley that Little Master has headed to the bridge.", "/[waitkey(64)]/[close()]"};
    String[] LATE_TALK4 = new String[]{"You know where the bridge is, right? Go out this room and go west along the fifth corridor, and it should be beyond Door No. 3.", "/[waitkey(1)]/[clear()]", "Once you're ready, go in and take control over the bridge quickly. You never know if more enemy fleets might transfer in.", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(1)]/[clear()]", "I am sorry that I do not have time to turn around.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_Q1 = new String[]{"/[label(Durandal Crew Member 1)]", "Where's /[color(0x329bbe)]Card No. 3/[color(0x808080)]...? He refuses to speak, even when I ask him in a normal tone. At times like this, it's important to ask wholeheartedly! Would you like me to ask him politely to make it easy for him to answer?", "/[waitkey(64)]/[close()]"};
    String[] NPC1_Q2 = new String[]{"/[label(Durandal Crew Member 1)]", "He may not be willing to talk, but it is important to have a heart when interrogating. Would you like me to ask him politely?", "/[waitkey(64)]/[close()]"};
    String[] NPC1_Q3 = new String[]{"/[label(Durandal Crew Member 1)]", "Would you like to try asking him politely again without rushing?", "/[waitkey(64)]/[close()]"};
    String[] NPC1_Q4 = new String[]{"/[label(Durandal Crew Member 1)]", "Would you like to try asking him one more time to make it easy for him to answer?", "/[waitkey(64)]/[close()]"};
    String[] NPC1_Q5 = new String[]{"/[label(Durandal Crew Member 1)]", "Would you like to wait a while until he's in a condition to talk again?", "/[waitkey(64)]/[close()]"};
    String[] NPC1_Q6 = new String[]{"/[label(Durandal Crew Member 1)]", "He appears as though he's ready to talk. Shall we start up again?", "/[waitkey(64)]/[close()]"};
    String[] NPC2_Q1 = new String[]{"/[label(Durandal Crew Member 2)]", "If he still will not talk after asking him calmly, then I suppose we have no choice but to use physical force on him. But before we try that, may I try questioning him\nin a way that is easy for him to understand?", "/[waitkey(64)]/[close()]"};
    String[] NPC2_Q2 = new String[]{"/[label(Durandal Crew Member 2)]", "If you are too nice, he will walk all over you. You have to talk to him in a way so he knows the position he's in.\nI can ask him a bit more roughly, right?", "/[waitkey(64)]/[close()]"};
    String[] NPC2_Q3 = new String[]{"/[label(Durandal Crew Member 2)]", "He still won't talk. Should we ask him more forcefully?", "/[waitkey(64)]/[close()]"};
    String[] NPC2_Q4 = new String[]{"/[label(Durandal Crew Member 2)]", "We must make him talk. We're wasting precious time that we do not have. Is it okay to rough him up during the interrogation?", "/[waitkey(64)]/[close()]"};
    String[] NPC2_Q5 = new String[]{"/[label(Durandal Crew Member 2)]", "It's not working. Shall we resume the interrogation once he has calmed down?", "/[waitkey(64)]/[close()]"};
    String[] NPC2_Q6 = new String[]{"/[label(Durandal Crew Member 2)]", "Oh! Looks like he's ready to talk. Let us resume!", "/[waitkey(64)]/[close()]"};
    String[] NPC1_A = new String[]{"/[label(Durandal Crew Member 1)]", "He didn't take me seriously at all. I think it would be better if we waited until he cools off.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_BCD = new String[]{"/[label(Durandal Crew Member 1)]", "Umm, pardon me, but would you happen to know the location of /[color(0x329bbe)]Card No. 3/[color(0x808080)]?", "/[waitkey(1)]/[clear()]", "If possible...only if it's possible, of course, but would you mind sharing this information with us? I'm sorry to trouble you like this.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_E = new String[]{"/[label(Durandal Crew Member 1)]", "You are being too mean. Taking away his spirit will not make him talk. I think it would be best to give it a little time.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_F = new String[]{"/[label(Durandal Crew Member 1)]", "He is not taking me seriously. Perhaps I asked him too nicely. Should I give it some time before we try again?", "/[waitkey(64)]/[close()]"};
    String[] NPC1_GHI = new String[]{"/[label(Durandal Crew Member 1)]", "Would you happen to know where /[color(0x329bbe)]Card No. 3/[color(0x808080)] is? We'll have all sorts of trouble if we don't find it. Would you please tell us? We won't do anything bad with it.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_J = new String[]{"/[label(Durandal Crew Member 1)]", "It seems I was a bit too hasty. I think it would be best to resume the interrogation later.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_K = new String[]{"/[label(Durandal Crew Member 1)]", "I thought things were going well, but...I should have been bolder. I suppose we should resume the interrogation after a little while.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_LMN = new String[]{"/[label(Durandal Crew Member 1)]", "You look like you're getting a bit tired. If you tell us where /[color(0x329bbe)]Card No. 3/[color(0x808080)] is, this interrogation will be over. Do you still not feel like talking?", "/[waitkey(64)]/[close()]"};
    String[] NPC1_O = new String[]{"/[label(Durandal Crew Member 1)]", "Perhaps I was too harsh? It'd be best to let some time pass.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_P = new String[]{"/[label(Durandal Crew Member 1)]", "I thought I was being insistent enough. We have no choice. We have to be patient and keep at it.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_QRS = new String[]{"/[label(Durandal Crew Member 1)]", "You are such a nice person. I can tell by the look in your eyes. When you think of your wounded friends, your conscience keeps you from telling us the location of /[color(0x329bbe)]Card No. 3/[color(0x808080)], correct? I understand. We will wait until you feel comfortable telling us.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_T = new String[]{"/[label(Durandal Crew Member 1)]", "I said the wrong thing at the end. It looks like he closed himself off from us. We should probably wait for him to get back into a better mood.", "/[waitkey(64)]/[close()]"};
    String[] NPC1_U = new String[]{"/[label(Durandal Crew Member 1)]", "We're counting on you, Little Master. Please leave this to us and hurry on to the bridge.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_A = new String[]{"/[label(Durandal Crew Member 2)]", "He is completely impudent. It's because you were too nice.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_BCD = new String[]{"/[label(Durandal Crew Member 2)]", "Hey, hey, hey!", "/[waitkey(1)]/[clear()]", "You keep this up and my famous \"Iron Fist\" is gonna\nland a critical hit!", "/[waitkey(1)]/[clear()]", "I won't tell anyone else! Come on, spill your guts before you get hurt!", "/[waitkey(64)]/[close()]"};
    String[] NPC2_E = new String[]{"/[label(Durandal Crew Member 2)]", "Great, he's sulking now. Man...well, at this rate, he probably won't talk to us unless we leave him alone for a while.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_F = new String[]{"/[label(Durandal Crew Member 2)]", "He won't take you seriously because you're being too nice! He's our prisoner, so you can be more insistent. I guess we have to leave him alone for a while.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_GHI = new String[]{"/[label(Durandal Crew Member 2)]", "Hey, hey, hey!", "/[waitkey(1)]/[clear()]", "You deaf or something? Didn't I tell you to hurry up and talk?! Tell us where /[color(0x329bbe)]Card No. 3/[color(0x808080)] is before you get hurt!", "/[waitkey(64)]/[close()]"};
    String[] NPC2_J = new String[]{"/[label(Durandal Crew Member 2)]", "He is sulking now. U-TIC soldiers are pretty pitiful. He probably won't talk for a while.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_K = new String[]{"/[label(Durandal Crew Member 2)]", "I thought he might talk, but no such luck. I guess we should wait a while, then ask him more forcefully.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_LMN = new String[]{"/[label(Durandal Crew Member 2)]", "I'm a patient guy, but if you don't start talking soon, I'm gonna quit being a nice guy.", "/[waitkey(1)]/[clear()]", "If you seriously plan on not talking, you better decide whether you want to keep your elbows or your knees.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_O = new String[]{"/[label(Durandal Crew Member 2)]", "I talked a little tough and just look at him. We'll have to leave him alone for a while.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_P = new String[]{"/[label(Durandal Crew Member 2)]", "Man, it's because you were too nice to him at the end. If you'd just told him straight, he would've talked. Well, I guess we'll just have to wait and question him again.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_QRS = new String[]{"/[label(Durandal Crew Member 2)]", "My patience is wearing thin. Tell us the location of /[color(0x329bbe)]Card No. 3/[color(0x808080)]. So how about it? Are you gonna tell us or not? It's about time we get things straight.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_T = new String[]{"/[label(Durandal Crew Member 2)]", "Boy, this guy is refusing to talk to the bitter end. I guess we'll just have to be persistent.", "/[waitkey(64)]/[close()]"};
    String[] NPC2_U = new String[]{"/[label(Durandal Crew Member 2)]", "All right! Now we know where the bridge and the key card are. The rest is up to you, Little Master!", "/[waitkey(64)]/[close()]"};
    String[] ENE_A = new String[]{"/[label(Soldier)]", "Hah! Like I said already, you guys ain't gonna get jack out of me!", "/[waitkey(64)]/[close()]"};
    String[] ENE_BCD = new String[]{"/[label(Soldier)]", "/[color(0x329bbe)]Card No. 3/[color(0x808080)]? W-who knows?", "/[waitkey(64)]/[close()]"};
    String[] ENE_E = new String[]{"/[label(Soldier)]", "...\n", "/[waitkey(64)]/[close()]"};
    String[] ENE_F = new String[]{"/[label(Soldier)]", "I'm not gonna tell you jack!", "/[waitkey(64)]/[close()]"};
    String[] ENE_GHI = new String[]{"/[label(Soldier)]", "Don't know.", "/[waitkey(64)]/[close()]"};
    String[] ENE_J = new String[]{"/[label(Soldier)]", "...\n", "/[waitkey(64)]/[close()]"};
    String[] ENE_K = new String[]{"/[label(Soldier)]", "Like I said already, you guys ain't gonna get jack out of me!", "/[waitkey(64)]/[close()]"};
    String[] ENE_LMN = new String[]{"/[label(Soldier)]", "Even if I did know...I wouldn't tell you.", "/[waitkey(64)]/[close()]"};
    String[] ENE_O = new String[]{"/[label(Soldier)]", "...\n", "/[waitkey(64)]/[close()]"};
    String[] ENE_P = new String[]{"/[label(Soldier)]", "I'm not answering any questions.", "/[waitkey(64)]/[close()]"};
    String[] ENE_QRS = new String[]{"/[label(Soldier)]", "The location of /[color(0x329bbe)]Card No. 3/[color(0x808080)] is...", "/[waitkey(1)]/[clear()]", "No, I can't tell you.", "/[waitkey(64)]/[close()]"};
    String[] ENE_T = new String[]{"/[label(Soldier)]", "...\n", "/[waitkey(64)]/[close()]"};
    String[] ENE_U = new String[]{"/[label(Soldier)]", "I guess I have no choice...", "/[color(0x329bbe)]Card No. 3/[color(0x808080)] is in Commander Margulis' private room.", "/[waitkey(1)]/[clear()]", "To get to his room, go all the way around the fifth corridor. It's beyond the sixth corridor.", "/[waitkey(64)]/[close()]"};

    ST1080() {
    }

    int DefaultMenu(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(2, 30);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    void DefaultTalk(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        System.waitFor(this.win);
    }

    void DefaultTalk2(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
    }

    void Final_init(int n) {
        System.println("FINAL INIT !!!!!!!!!!!!!!!!!!!!!!!!!");
        switch (n) {
            case 11: {
                System.println("case 11");
                this.npc1.kickEnepc(9, 13);
                break;
            }
            case 12: {
                System.println("case 12");
                this.npc2.kickEnepc(9, 13);
                break;
            }
        }
    }

    void Flash(int n) {
        this.fade_eve.call(0);
        this.flag_height = 1;
        this.flag_width = 3;
        this.talkFlag1 = 3;
        this.talkFlag2 = 3;
        this.flash1 = false;
        this.flash2 = false;
        int n2 = 0;
        while (n2 <= 60) {
            if (n2 == 50) {
                this.enemy1.setMotion(0, 5);
            } else if (n2 == 60) {
                this.enemy1.kickEnepc(9, -1);
            }
            System.sleep(1);
            ++n2;
        }
        System.sleep(40);
        Sound.effectPlay(19);
        System.sleep(20);
        if (n == 1) {
            this.npc1.kickEnepc(9, 13);
            this.DefaultTalk(this.NPC1_Q6);
            this.npc1.kickEnepc(9, -1);
        } else if (n == 2) {
            this.npc2.kickEnepc(9, 13);
            this.DefaultTalk(this.NPC2_Q6);
            this.npc2.kickEnepc(9, -1);
        }
    }

    public void Late_npc(Enepc enepc, Window window) {
        System.println("Late_NPC");
        if (this.lateFlag == 1) {
            window.print(this.LATE_TALK1, 0);
            ST1080.waitPage(window, 64);
            ++this.lateFlag;
        } else if (this.lateFlag == 2 && this.IN1080 == 1) {
            window.print(this.LATE_TALK2, 0);
            ST1080.waitPage(window, 64);
            ++this.lateFlag;
        } else if (this.lateFlag == 2 && this.IN1080 == 0) {
            window.print(this.LATE_TALK3, 0);
            ST1080.waitPage(window, 64);
            this.lateFlag += 2;
        } else if (this.lateFlag == 3) {
            window.print(this.LATE_TALK3, 0);
            ST1080.waitPage(window, 64);
            ++this.lateFlag;
        } else {
            window.print(this.LATE_TALK4, 0);
            ST1080.waitPage(window, 64);
            ++this.lateFlag;
        }
    }

    public void Talk_ene1(Enepc enepc, Window window) {
        System.println("ENE1_TALK");
        if (this.CF_CONTE_END == 1) {
            window.print(this.ENE_U, 0);
            ST1080.waitPage(window, 64);
            return;
        }
        switch (this.flag_height) {
            case 1: {
                if (this.flag_width == 1) {
                    window.print(this.ENE_A, 0);
                    this.enemy1.kickEnepc(9, 14);
                    ST1080.waitPage(window, 64);
                    break;
                }
                if (this.flag_width == 5) {
                    window.print(this.ENE_E, 0);
                    ST1080.waitPage(window, 64);
                    break;
                }
                window.print(this.ENE_BCD, 0);
                ST1080.waitPage(window, 64);
                break;
            }
            case 2: {
                if (this.flag_width == 1) {
                    window.print(this.ENE_F, 0);
                    this.enemy1.kickEnepc(9, 14);
                    ST1080.waitPage(window, 64);
                    break;
                }
                if (this.flag_width == 5) {
                    window.print(this.ENE_J, 0);
                    ST1080.waitPage(window, 64);
                    break;
                }
                window.print(this.ENE_GHI, 0);
                ST1080.waitPage(window, 64);
                break;
            }
            case 3: {
                if (this.flag_width == 1) {
                    window.print(this.ENE_K, 0);
                    this.enemy1.kickEnepc(9, 14);
                    ST1080.waitPage(window, 64);
                    break;
                }
                if (this.flag_width == 5) {
                    window.print(this.ENE_O, 0);
                    ST1080.waitPage(window, 64);
                    break;
                }
                window.print(this.ENE_LMN, 0);
                ST1080.waitPage(window, 64);
                break;
            }
            default: {
                if (this.flag_width == 1) {
                    window.print(this.ENE_P, 0);
                    this.enemy1.kickEnepc(9, 14);
                    ST1080.waitPage(window, 64);
                    break;
                }
                if (this.flag_width == 5) {
                    window.print(this.ENE_T, 0);
                    ST1080.waitPage(window, 64);
                    break;
                }
                window.print(this.ENE_QRS, 0);
                ST1080.waitPage(window, 64);
            }
        }
    }

    public void Talk_npc1(Enepc var1_1) {
        System.println("NPC1_TALK1");
        if (this.CF_CONTE_END == 1) {
            this.DefaultTalk(this.NPC1_U);
            return;
        }
        switch (this.talkFlag1) {
            case 1: {
                this.DefaultTalk(this.NPC1_TALK1);
                this.npc1.kickEnepc(9, 13);
                ++this.talkFlag1;
                return;
            }
            case 2: {
                this.DefaultTalk(this.NPC1_TALK2);
                this.npc1.kickEnepc(9, 13);
                ++this.talkFlag1;
                return;
            }
            case 3: {
                if (this.CF_CONTE_END != 1) **GOTO lbl20
                this.DefaultTalk(this.NPC1_U);
                **GOTO lbl146
                lbl20:

                if (this.flag_width != 1 || this.flash1) **GOTO lbl35
                this.flash1 = true;
                switch (this.flag_height) {
                    case 1: {
                        this.DefaultTalk(this.NPC1_A);
                        break;
                    }
                    case 2: {
                        this.DefaultTalk(this.NPC1_F);
                        break;
                    }
                    case 3: {
                        this.DefaultTalk(this.NPC1_K);
                        break;
                    }
                    case 4: {
                        this.DefaultTalk(this.NPC1_P);
                        break;
                    }
                    lbl35:

                    if (this.flag_width == 1 && this.flash1) {
                        this.talkFlag2 = 3;
                        if (this.DefaultMenu(this.NPC1_Q5) != 0) break;
                        this.Flash(1);
                        break;
                    }
                    if (this.flag_width != 5 || this.flash1) **GOTO lbl55
                    this.flash1 = true;
                    switch (this.flag_height) {
                        case 1: {
                            this.DefaultTalk(this.NPC1_E);
                            break;
                        }
                        case 2: {
                            this.DefaultTalk(this.NPC1_J);
                            break;
                        }
                        case 3: {
                            this.DefaultTalk(this.NPC1_O);
                            break;
                        }
                        case 4: {
                            this.DefaultTalk(this.NPC1_T);
                            break;
                        }
                        lbl55:

                        if (this.flag_width == 5 && this.flash1) {
                            if (this.DefaultMenu(this.NPC1_Q5) != 0) break;
                            this.Flash(1);
                            break;
                        }
                        if (this.flag_height == 1) {
                            if (this.DefaultMenu(this.NPC1_Q1) != 0) break;
                            this.npc1.kickEnepc(9, 13);
                            this.DefaultTalk(this.NPC1_BCD);
                            this.enemy1.kickEnepc(9, 11);
                            if (this.flag_width == 4) {
                                System.println("ADVANCE!");
                                this.DefaultTalk(this.ENE_GHI);
                                this.flag_height = 2;
                                this.enemy1.kickEnepc(9, -1);
                                Sound.effectPlay(12);
                                break;
                            }
                            if (this.flag_width == 2) {
                                System.println("fail A");
                                this.DefaultTalk(this.ENE_A);
                                this.talkFlag2 = 3;
                                this.flag_width = 1;
                                this.enemy1.kickEnepc(9, 14);
                                Sound.effectPlay(34);
                                break;
                            }
                            this.DefaultTalk(this.ENE_BCD);
                            --this.flag_width;
                            this.enemy1.setMotion(0, 5);
                            this.enemy1.kickEnepc(9, -1);
                            break;
                        }
                        if (this.flag_height == 2) {
                            if (this.DefaultMenu(this.NPC1_Q2) != 0) break;
                            this.npc1.kickEnepc(9, 13);
                            this.DefaultTalk(this.NPC1_GHI);
                            this.enemy1.kickEnepc(9, 11);
                            if (this.flag_width == 2) {
                                System.println("fail F");
                                this.DefaultTalk(this.ENE_F);
                                this.talkFlag2 = 3;
                                this.flag_width = 1;
                                this.enemy1.kickEnepc(9, 14);
                                Sound.effectPlay(34);
                                break;
                            }
                            this.DefaultTalk(this.ENE_GHI);
                            --this.flag_width;
                            this.enemy1.setMotion(0, 5);
                            this.enemy1.kickEnepc(9, -1);
                            break;
                        }
                        if (this.flag_height == 3) {
                            if (this.DefaultMenu(this.NPC1_Q3) != 0) break;
                            this.npc1.kickEnepc(9, 13);
                            this.DefaultTalk(this.NPC1_LMN);
                            this.enemy1.kickEnepc(9, 11);
                            if (this.flag_width == 2) {
                                System.println("fail K");
                                this.DefaultTalk(this.ENE_K);
                                this.talkFlag2 = 3;
                                this.flag_width = 1;
                                this.enemy1.kickEnepc(9, 14);
                                Sound.effectPlay(34);
                                break;
                            }
                            this.DefaultTalk(this.ENE_LMN);
                            --this.flag_width;
                            this.enemy1.setMotion(0, 5);
                            this.enemy1.kickEnepc(9, -1);
                            break;
                        }
                        if (this.flag_height != 4 || this.DefaultMenu(this.NPC1_Q4) != 0) break;
                        this.npc1.kickEnepc(9, 13);
                        this.DefaultTalk(this.NPC1_QRS);
                        this.enemy1.kickEnepc(9, 11);
                        if (this.flag_width == 2) {
                            System.println("fail P");
                            this.DefaultTalk(this.ENE_P);
                            this.talkFlag2 = 3;
                            this.flag_width = 1;
                            this.enemy1.kickEnepc(9, 14);
                            Sound.effectPlay(34);
                            break;
                        }
                        if (this.flag_width == 4) {
                            System.println("THE END!");
                            Sound.effectPlay(29);
                            this.DefaultTalk(this.ENE_U);
                            this.enemy1.setMotion(0, 5);
                            this.enemy1.kickEnepc(9, -1);
                            Runtime.setFlags(6037, 1, 1);
                            this.CF_CONTE_END = Runtime.getFlags(6037, 1);
                            break;
                        }
                        this.DefaultTalk(this.ENE_QRS);
                        --this.flag_width;
                        this.enemy1.setMotion(0, 5);
                        this.enemy1.kickEnepc(9, -1);
                    }
                    break;
                }
                lbl146:

                System.println("NPC1_LOOKAT_13");
                this.npc1.kickEnepc(9, 13);
                return;
            }
        }
    }

    public void Talk_npc2(Enepc var1_1) {
        System.println("NPC2_TALK");
        if (this.CF_CONTE_END == 1) {
            this.DefaultTalk(this.NPC2_U);
            return;
        }
        switch (this.talkFlag2) {
            case 1: {
                this.DefaultTalk(this.NPC2_TALK1);
                this.npc2.kickEnepc(9, 13);
                ++this.talkFlag2;
                return;
            }
            case 2: {
                this.DefaultTalk(this.NPC2_TALK2);
                this.npc2.kickEnepc(9, 13);
                ++this.talkFlag2;
                return;
            }
            case 3: {
                if (this.CF_CONTE_END != 1) **GOTO lbl20
                this.DefaultTalk(this.NPC2_U);
                **GOTO lbl156
                lbl20:

                if (this.flag_width != 1 || this.flash2) **GOTO lbl35
                this.flash2 = true;
                switch (this.flag_height) {
                    case 1: {
                        this.DefaultTalk(this.NPC2_A);
                        break;
                    }
                    case 2: {
                        this.DefaultTalk(this.NPC2_F);
                        break;
                    }
                    case 3: {
                        this.DefaultTalk(this.NPC2_K);
                        break;
                    }
                    case 4: {
                        this.DefaultTalk(this.NPC2_P);
                        break;
                    }
                    lbl35:

                    if (this.flag_width == 1 && this.flash2) {
                        if (this.DefaultMenu(this.NPC2_Q5) != 0) break;
                        this.Flash(2);
                        break;
                    }
                    if (this.flag_width != 5 || this.flash2) **GOTO lbl54
                    this.flash2 = true;
                    switch (this.flag_height) {
                        case 1: {
                            this.DefaultTalk(this.NPC2_E);
                            break;
                        }
                        case 2: {
                            this.DefaultTalk(this.NPC2_J);
                            break;
                        }
                        case 3: {
                            this.DefaultTalk(this.NPC2_O);
                            break;
                        }
                        case 4: {
                            this.DefaultTalk(this.NPC2_T);
                            break;
                        }
                        lbl54:

                        if (this.flag_width == 5 && this.flash2) {
                            if (this.DefaultMenu(this.NPC2_Q5) != 0) break;
                            this.Flash(2);
                            break;
                        }
                        if (this.flag_height == 1) {
                            if (this.DefaultMenu(this.NPC2_Q1) == 0) {
                                this.npc2.kickEnepc(9, 13);
                                this.DefaultTalk(this.NPC2_BCD);
                                this.enemy1.kickEnepc(9, 12);
                                if (this.flag_width == 4) {
                                    System.println("fail E");
                                    this.talkFlag1 = 3;
                                    this.flag_width = 5;
                                    this.DefaultTalk2(this.ENE_E);
                                    System.sleep(30);
                                    this.enemy1.setMotion(0, 6);
                                    System.sleep(40);
                                    Sound.effectPlay(34);
                                    System.sleep(5);
                                    System.waitFor(this.win);
                                } else {
                                    this.DefaultTalk(this.ENE_BCD);
                                    ++this.flag_width;
                                }
                            }
                            this.enemy1.kickEnepc(9, -1);
                            break;
                        }
                        if (this.flag_height == 2) {
                            if (this.DefaultMenu(this.NPC2_Q2) == 0) {
                                this.npc2.kickEnepc(9, 13);
                                this.DefaultTalk(this.NPC2_GHI);
                                this.enemy1.kickEnepc(9, 12);
                                if (this.flag_width == 3) {
                                    System.println("ADVANCE!");
                                    this.DefaultTalk(this.ENE_LMN);
                                    this.flag_height = 3;
                                    this.enemy1.setMotion(0, 5);
                                    Sound.effectPlay(12);
                                } else if (this.flag_width == 4) {
                                    System.println("fail J");
                                    this.talkFlag1 = 3;
                                    this.flag_width = 5;
                                    this.DefaultTalk2(this.ENE_J);
                                    System.sleep(30);
                                    this.enemy1.setMotion(0, 6);
                                    System.sleep(40);
                                    Sound.effectPlay(34);
                                    System.sleep(5);
                                    System.waitFor(this.win);
                                } else {
                                    this.DefaultTalk(this.ENE_GHI);
                                    ++this.flag_width;
                                }
                            }
                            this.enemy1.kickEnepc(9, -1);
                            break;
                        }
                        if (this.flag_height == 3) {
                            if (this.DefaultMenu(this.NPC2_Q3) == 0) {
                                this.npc2.kickEnepc(9, 13);
                                this.DefaultTalk(this.NPC2_LMN);
                                this.enemy1.kickEnepc(9, 12);
                                if (this.flag_width == 2) {
                                    System.println("ADVANCE!");
                                    this.DefaultTalk(this.ENE_QRS);
                                    this.flag_height = 4;
                                    this.enemy1.setMotion(0, 5);
                                    Sound.effectPlay(12);
                                } else if (this.flag_width == 4) {
                                    System.println("fail O");
                                    this.talkFlag1 = 3;
                                    this.flag_width = 5;
                                    this.DefaultTalk2(this.ENE_O);
                                    System.sleep(30);
                                    this.enemy1.setMotion(0, 6);
                                    System.sleep(40);
                                    Sound.effectPlay(34);
                                    System.sleep(5);
                                    System.waitFor(this.win);
                                } else {
                                    this.DefaultTalk(this.ENE_LMN);
                                    ++this.flag_width;
                                }
                            }
                            this.enemy1.kickEnepc(9, -1);
                            break;
                        }
                        if (this.flag_height != 4) break;
                        if (this.DefaultMenu(this.NPC2_Q4) == 0) {
                            this.npc2.kickEnepc(9, 13);
                            this.DefaultTalk(this.NPC2_QRS);
                            this.enemy1.kickEnepc(9, 12);
                            if (this.flag_width == 4) {
                                System.println("fail T");
                                this.talkFlag1 = 3;
                                this.flag_width = 5;
                                this.DefaultTalk2(this.ENE_T);
                                System.sleep(30);
                                this.enemy1.setMotion(0, 6);
                                System.sleep(40);
                                Sound.effectPlay(34);
                                System.sleep(5);
                                System.waitFor(this.win);
                            } else {
                                this.DefaultTalk(this.ENE_QRS);
                                ++this.flag_width;
                            }
                        }
                        this.enemy1.kickEnepc(9, -1);
                    }
                    break;
                }
                lbl156:

                System.println("NPC2_LOOKAT_13");
                this.npc2.kickEnepc(9, 13);
                return;
            }
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1050, 4);
                break;
            }
        }
    }

    void init() {
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade_eve = new Effect(0);
        this.fade_eve.args[0] = -268435456;
        this.fade_eve.args[1] = 45;
        this.fade_eve.args[2] = 0;
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Stage.setVisible(1, false);
        this.doorA = new Uwamono(0, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        if (this.NO3_CARDKEY == 0) {
            System.println("捕虜確認");
            Runtime.setFlags(6039, 1, 1);
            this.npc1 = new NPC_NORMAL(1027, 11, 0, 0, 4, -0.75f, 0.0f, -0.75f, 160.0f);
            this.npc1.talkto("Talk_npc1");
            this.npc1.enableDTKFlag(4);
            this.npc1.disableDTKFlag(2);
            this.npc1.disableDTKFlag(8);
            this.npc1.setMotion(3, 10);
            this.npc2 = new NPC_NORMAL(1026, 12, 0, 0, 4, 0.75f, 0.0f, -0.75f, 200.0f);
            this.npc2.talkto("Talk_npc2");
            this.npc2.enableDTKFlag(4);
            this.npc2.disableDTKFlag(2);
            this.npc2.disableDTKFlag(8);
            this.enemy1 = new NPC_NORMAL(778, 13, 0, 0, 6, 0.0f, 0.0f, -2.0f, 0.0f);
            this.enemy1.talkto("Talk_ene1");
            this.enemy1.enableDTKFlag(4);
            this.enemy1.disableDTKFlag(8);
            this.enemy1.disableDTKFlag(131072);
            this.enemy1.disableDTKFlag(1);
            this.enemy1.disableDTKFlag(2);
            this.enemy1.setMotion(0, 5);
            this.dummy = new NPC_NORMAL(778, 14, 0, 0, 4, 0.15f, 0.0f, -2.05f, 0.0f);
            this.dummy.setVisible(false);
            this.dummy.kickEnepc(10, 60, 0);
            this.dummy.dispRadar(false);
            this.dummy.disableDTKFlag(65536);
            this.dummy.setInvalidID(1);
        } else {
            this.player.setID(2);
            this.Obj600 = new Effect(1441, 0);
            this.Obj600.setScale(0.725f, 0.725f, 0.725f);
            this.Obj600.disp(true);
            this.Obj601 = new Effect(1639, 1);
            this.Obj601.setScale(0.5f, 0.8f, 0.5f);
            this.Obj601.disp(true);
            this.isu1 = new MAPUnit();
            this.isu1.mapUnit(19);
            this.isu1.start(4, null);
            this.isu1.getTranslate();
            this.isu1.setTranslate(this.isu1.px + 0.4f, this.isu1.py, this.isu1.pz - 0.4f);
            this.isu2 = new MAPUnit();
            this.isu2.mapUnit(20);
            this.isu2.start(4, null);
            this.isu2.getTranslate();
            this.isu2.setTranslate(this.isu2.px + 0.4f, this.isu2.py, this.isu2.pz - 0.4f);
            this.isu3 = new MAPUnit();
            this.isu3.mapUnit(21);
            this.isu3.start(4, null);
            this.isu3.getTranslate();
            this.isu3.setTranslate(this.isu3.px + 0.4f, this.isu3.py, this.isu3.pz - 0.4f);
            this.isu4 = new MAPUnit();
            this.isu4.mapUnit(22);
            this.isu4.start(4, null);
            this.isu4.getTranslate();
            this.isu4.setTranslate(this.isu4.px + 0.4f, this.isu4.py, this.isu4.pz - 0.4f);
            this.npc3 = new NPC_NORMAL(1026, 15, 0, 0, 7, 2.45f, -0.065f, -1.245f, 135.0f);
            this.npc3.setInvalidID(1);
            this.npc3.disableDTKFlag(4);
            this.npc3.disableDTKFlag(2);
            this.npc3.disableDTKFlag(1);
            this.npc3.setMotion(0, 6);
            this.npc3.talkto("Late_npc");
        }
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.35f, 0.35f, 0.35f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.5f, 0.5f, 0.5f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Runtime.setIdLightCol(1, 0, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 1, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 2, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightCol(1, 3, 0.275f, 0.275f, 0.275f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 3.0f);
        Runtime.setIdLightVec(1, 3, 0.0f, -1.0f, -3.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 40.0f);
        this.cam0.setCFHokan(1, 100.0f, 100.0f);
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

        void init() {
        }

        public void talk(Window window) {
        }
    }

    class NPC_EVENT
            extends Enepc {
        NPC_EVENT() {
        }

        void init() {
        }
    }
}

