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
import xeno.map.MC_KUK11_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST2110
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_KUK11_PRJ {
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int button_flg = 0;
    int Kaiza = 0;
    int Syouyou = 0;
    int Miru = 0;
    int Limit = 0;
    int niwa = 0;
    int destroy = 0;
    int o8 = 0;
    int eldx = 0;
    int npc1xtalked = 0;
    int npc2xtalked = 0;
    int npc3xtalked = 0;
    int npc4xtalked = 0;
    int npc5xtalked = 0;
    int npc6xtalked = 0;
    Enepc Hak;
    Enepc Sco;
    Enepc Kin;
    Enepc Go1;
    Enepc Mat;
    Enepc npc6;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Enepc enemy6;
    Enepc enemy7;
    Enepc enemy8;
    Uwamono doorA;
    Unit Kidou;
    Unit Make;
    Unit Kidou1;
    Unit Eturan;
    Unit Dai_1;
    Unit Dai_2;
    Unit Arm_1;
    Unit Arm_2;
    Unit Arm_3;
    Unit Arm_4;
    Unit Arm_5;
    Unit Arm_6;
    Unit OArm_r;
    Unit OArm_l;
    Unit Otu;
    Unit Drl;
    Unit Bust;
    Unit Punch;
    Unit Hasami_a;
    Unit Hasami_b;
    Unit Eye;
    Unit Dx_ude;
    Unit Dx_ago;
    Unit Dx_emb;
    Unit Kuru_r;
    Unit Kuru_l;
    Unit R1;
    Unit R2;
    Unit R3;
    Unit R4;
    Unit R5;
    Unit Otete1;
    Unit Otete2;
    Unit Annyo_r;
    Unit Annyo_l;
    Unit Atama;
    Unit Mune;
    Unit Ooi;
    Uwamono COL;
    boolean EnterCheck = false;
    Uwamono KOW01;
    Uwamono KOW02;
    Uwamono KOW03;
    Uwamono KOW04;
    Uwamono KOW05;
    Uwamono KOW06;
    Uwamono KOW07;
    Uwamono KOW08;
    Uwamono KOW09;
    Uwamono KOW10;
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    Effect E01;
    Effect E02;
    Effect E03;
    Effect E04;
    Effect E05;
    Effect E06;
    Effect E07a;
    Effect E07b;
    Effect E08;
    Effect E09;
    Effect E10;
    Effect E11;
    Effect E12;
    Effect E13;
    Effect E14;
    Effect E15;
    Effect fade;
    Effect fade1;
    Effect fade2;
    Light light = new Light(0);
    int ohanasi = 0;
    int tukuru = 0;
    int ohanasi_sinkou = 0;
    int page;
    String[] Sub_title_01 = new String[]{"[Professor's Giant Robot Training Hall]\n", "Chapter 1", "/[waitkey(1)]/[clear()]", "Genius scientist appears!!\n", "Episode:\n", "His name is...", "/[waitkey(64)]/[close()]"};
    String[] Sub_title_02 = new String[]{"[Professor's Giant Robot Training Hall]\n", "Chapter 2", "/[waitkey(1)]/[clear()]", "Episode:\n", "Foundation Robot Academy!", "/[waitkey(64)]/[close()]"};
    String[] Sub_title_03 = new String[]{"[Professor's Giant Robot Training Hall]\n", "Chapter 3", "/[waitkey(1)]/[clear()]", "Episode:\n", "Hyper Assistant Scott Appears!!", "/[waitkey(64)]/[close()]"};
    String[] Sub_title_04 = new String[]{"[Professor's Giant Robot Training Hall]\n", "Chapter 4", "/[waitkey(1)]/[clear()]", "Episode:\n", "Crisis!! A Dark Shadow Befalls the Robot Lab?!", "/[waitkey(64)]/[close()]"};
    String[] Sub_title_05a = new String[]{"[Professor's Giant Robot Training Hall]\n", "Final Chapter-Part 1", "/[waitkey(1)]/[clear()]", "Episode:\n", "A Friendship in Crisis!", "/[waitkey(64)]/[close()]"};
    String[] Sub_title_05b = new String[]{"[Professor's Giant Robot Training Hall]\n", "Final Chapter-Part 2", "/[waitkey(1)]/[clear()]", "Episode:\n", "Friendships are Forever...", "/[waitkey(1)]/[clear()]", "The Messenger of Love, Justice, and Courage!!\n", "Erde Kaiser is Here!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_00 = new String[]{"/[label(Shion)]", "This place permeates some kind of fondness for the past...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_00 = new String[]{"/[label(Ziggy)]", "This place is really rundown.", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_00 = new String[]{"/[label(chaos)]", "This atmosphere...it feels sort of familiar. In a sense, I feel safe here...", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_00 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_00 = new String[]{"/[label(MOMO)]", "This seems like a fun place.", "/[waitkey(64)]/[close()]"};
    String[] JR_1_00 = new String[]{"/[label(Jr.)]", "What the heck is this place? When the hell did they put something like this on the Foundation?!", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_00 = new String[]{"/[label(Mysterious Old Man)]", "What's going on?! This is no place for the likes of you.\nNo siiir!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_01 = new String[]{"/[label(Shion)]", "Excuse me, what is this place?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_01 = new String[]{"/[label(Ziggy)]", "Hey, old man, what are you doing here?", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_01 = new String[]{"/[label(chaos)]", "Excuse me, what is this place for?", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_01 = new String[]{"/[label(KOS-MOS)]", "What is manufactured in this place?", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_01 = new String[]{"/[label(MOMO)]", "Excuse me, what do you do here, sir?", "/[waitkey(64)]/[close()]"};
    String[] JR_1_01 = new String[]{"/[label(Jr.)]", "Hey, Mister. What are you doing here?", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_01 = new String[]{"/[label(Mysterious Old Man)]", "Ahem! Very good of you to ask!!", "/[waitkey(1)]/[clear()]", "This is the amazing, stupendous, absolutely fantabulous, Foundation Robot Academy!!", "/[waitkey(1)]/[clear()]", "...\n", "/[waitkey(1)]/[clear()]", "...\n", "/[waitkey(1)]/[clear()]", "...\n", "/[waitkey(1)]/[clear()]", "But this is no time for me to be talking to you.", "/[waitkey(1)]/[clear()]", "No siiiiir!!!!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_02 = new String[]{"/[label(Shion)]", "It sort of seems like an A.G.W.S. factory, but it's a little different.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_02 = new String[]{"/[label(Ziggy)]", "It wouldn't be possible to manufacture A.G.W.S. here.", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_02 = new String[]{"/[label(chaos)]", "It sort of seems like an A.G.W.S. factory...maybe not quite.", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_02 = new String[]{"/[label(KOS-MOS)]", "An outdated facility. It is not possible to manufacture A.G.W.S. here.", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_02 = new String[]{"/[label(MOMO)]", "It sort of seems like an A.G.W.S. factory, but maybe it's a little different.", "/[waitkey(64)]/[close()]"};
    String[] JR_1_02 = new String[]{"/[label(Jr.)]", "Considering the way the facility looks, it doesn't seem like it's an A.G.W.S. factory.", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_02 = new String[]{"/[label(Mysterious Old Man)]", "A.G.W.S. you say?!", "/[waitkey(1)]/[clear()]", "Don't you dare compare this to those children's\ntoys. No siiir!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_03 = new String[]{"/[label(Shion)]", "Children's toys...", "/[waitkey(1)]/[clear()]", "Then what in the world do you make here?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_03 = new String[]{"/[label(Ziggy)]", "Children's toys, huh?", "/[waitkey(1)]/[clear()]", "Then what is it that you make here?", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_03 = new String[]{"/[label(chaos)]", "Children's toys?", "/[waitkey(1)]/[clear()]", "Then what in the world do you make here?", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_03 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(1)]/[clear()]", "Then what do you manufacture here?", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_03 = new String[]{"/[label(MOMO)]", "You consider them as children's toys?", "/[waitkey(1)]/[clear()]", "Then what in the world do you make here?", "/[waitkey(64)]/[close()]"};
    String[] JR_1_03 = new String[]{"/[label(Jr.)]", "Children's toys? Humph, aren't we the big shot.", "/[waitkey(1)]/[clear()]", "Well in that case, what the heck do you make here?", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_03 = new String[]{"/[label(Mysterious Old Man)]", "Hmm? All you keep doing is asking questions.", "/[waitkey(1)]/[clear()]", "You're all some kind of spies, aren't you?! Who do you work for?!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_04 = new String[]{"/[label(Shion)]", "Spies? We're nothing of the sort!", "/[waitkey(1)]/[clear()]", "I have a little interest in machines is all, so I thought I'd ask.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_04 = new String[]{"/[label(Ziggy)]", "Spies? You're a funny old man.", "/[waitkey(1)]/[clear()]", "I'm just a little interested in machines, so I thought I'd ask.", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_04 = new String[]{"/[label(chaos)]", "Spies? We aren't anything like that.", "/[waitkey(1)]/[clear()]", "I'm just a little interested in machines, so I thought I'd ask.", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_04 = new String[]{"/[label(KOS-MOS)]", "...\n", "/[waitkey(1)]/[clear()]", "Spying? It is more like something on the order of \"curiosity\" in human terms.", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_04 = new String[]{"/[label(MOMO)]", "Spies? We're nothing of the sort!", "/[waitkey(1)]/[clear()]", "I just have some interest in machines, so I thought I'd ask.", "/[waitkey(64)]/[close()]"};
    String[] JR_1_04 = new String[]{"/[label(Jr.)]", "Spies? What're you talking about, old man?!", "/[waitkey(1)]/[clear()]", "I just asked because I have a little interest in machines!", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_04 = new String[]{"/[label(Mysterious Old Man)]", "Hmm...interested in machines, you say.", "/[waitkey(1)]/[clear()]", "All right then! I'll tell you! I'm building an invincible giant\nrobot!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_05 = new String[]{"/[label(Shion)]", "Giant robot?!", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_05 = new String[]{"/[label(Ziggy)]", "Giant robot?!", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_05 = new String[]{"/[label(chaos)]", "Giant robot?!", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_05 = new String[]{"/[label(KOS-MOS)]", "Giant robot?!", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_05 = new String[]{"/[label(MOMO)]", "Giant robot?!", "/[waitkey(64)]/[close()]"};
    String[] JR_1_05 = new String[]{"/[label(Jr.)]", "Giant robot?!", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_05 = new String[]{"/[label(Mysterious Old Man)]", "No, you got it wrong!!", "/[waitkey(1)]/[clear()]", "/[code(18)]/[code(20)]/[code(20)]IN-VIN-CIBLE/[code(18)]/[code(16)]/[code(16)] giant robot!    ", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_06 = new String[]{"/[label(Shion)]", "Invincible giant robot...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_06 = new String[]{"/[label(Ziggy)]", "Invincible giant robot?", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_06 = new String[]{"/[label(chaos)]", "Invincible giant robot, huh?", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_06 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_06 = new String[]{"/[label(MOMO)]", "Invincible giant robot?", "/[waitkey(64)]/[close()]"};
    String[] JR_1_06 = new String[]{"/[label(Jr.)]", "Invincible giant robot, huh?", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_06 = new String[]{"/[label(Mysterious Old Man)]", "You don't believe me!! Then I'll tell you an extra special secret!", "/[waitkey(1)]/[clear()]", "...\n", "...\n", "...\n", "Wanna hear it?", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_07 = new String[]{"/[label(Shion)]", "That's okay.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_07 = new String[]{"/[label(Ziggy)]", "No.", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_07 = new String[]{"/[label(chaos)]", "I'll decline.", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_07 = new String[]{"/[label(KOS-MOS)]", "That will not be necessary.", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_07 = new String[]{"/[label(MOMO)]", "...That's okay.", "/[waitkey(64)]/[close()]"};
    String[] JR_1_07 = new String[]{"/[label(Jr.)]", "No thanks.", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_07 = new String[]{"/[label(Mysterious Old Man)]", "Listen.", "/[waitkey(1)]/[clear()]", "The hopes and dreams mankind lost have been scattered across the universe!!", "/[waitkey(1)]/[clear()]", "Likewise, there are giant robot parts that we have forgotten in the past floating around aimlessly in the universe!!", "/[waitkey(1)]/[clear()]", "Do you understand?", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_08 = new String[]{"/[label(Shion)]", "I don't understand.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_08 = new String[]{"/[label(Ziggy)]", "It makes no sense.", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_08 = new String[]{"/[label(chaos)]", "I don't understand.", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_08 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_08 = new String[]{"/[label(MOMO)]", "I do not understand.", "/[waitkey(64)]/[close()]"};
    String[] JR_1_08 = new String[]{"/[label(Jr.)]", "What...?", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_08 = new String[]{"/[label(Mysterious Old Man)]", "Figure it out.", "/[waitkey(1)]/[clear()]", "So I am gathering together those dream fragments in\nan attempt to bring the giant robot back into this world!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_09 = new String[]{"/[label(Shion)]", "Dream fragments scattered across the universe...what a romantic notion.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_09 = new String[]{"/[label(Ziggy)]", "Heh, you're quite the romanticist.", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_09 = new String[]{"/[label(chaos)]", "Dream fragments scattered across the universe...that's sort of poetic.", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_09 = new String[]{"/[label(KOS-MOS)]", "Dream fragments scattered across the universe...this is an illogical human notion.", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_09 = new String[]{"/[label(MOMO)]", "Dream fragments scattered across the universe...how romantic.", "/[waitkey(64)]/[close()]"};
    String[] JR_1_09 = new String[]{"/[label(Jr.)]", "Dream fragments scattered across the universe...pretty slick, old man!", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_09 = new String[]{"/[label(Mysterious Old Man)]", "Right?", "/[waitkey(1)]/[clear()]", "For being modern young people, you show some promise.", "/[waitkey(1)]/[clear()]", "All right, that settles it! I've decided to entrust you\nwith a part of my dream!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_10 = new String[]{"/[label(Shion)]", "Entrust us? With what?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_10 = new String[]{"/[label(Ziggy)]", "Entrust? With what?", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_10 = new String[]{"/[label(chaos)]", "Entrust us? With what?", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_10 = new String[]{"/[label(KOS-MOS)]", "Entrust? What will you entrust us with?", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_10 = new String[]{"/[label(MOMO)]", "Entrust? What do you mean?", "/[waitkey(64)]/[close()]"};
    String[] JR_1_10 = new String[]{"/[label(Jr.)]", "Entrust us? What do you mean?", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_10 = new String[]{"/[label(Mysterious Old Man)]", "I want you to find the robot parts that I've been in search of for years!!", "/[waitkey(1)]/[clear()]", "My research has revealed that there are six kinds of robot parts in the world.", "/[waitkey(1)]/[clear()]", "Of course, you won't be doing it for free! If you find them for me, I'll prepare a special reward for you!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_11 = new String[]{"/[label(Shion)]", "(He's pretty pushy...)\n", "Fine, if we find these robot parts, we'll bring them to you!", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_11 = new String[]{"/[label(Ziggy)]", "(What a pushy old man.)\n", "Roger that. If we find these robot parts, we will bring them to you.", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_11 = new String[]{"/[label(chaos)]", "(Hmm, he's sort of pushy.)\n", "All right! If we find some robot parts, we'll bring them to you!", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_11 = new String[]{"/[label(KOS-MOS)]", "Understood. Orders from Shion will take priority, however. If this is acceptable, we will look for the robot parts.", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_11 = new String[]{"/[label(MOMO)]", "(He is rather pushy.)\n", "Okay! If we find some robot parts, we will bring them to you!", "/[waitkey(64)]/[close()]"};
    String[] JR_1_11 = new String[]{"/[label(Jr.)]", "(What a pushy guy.)\n", "Fine, if we find some robot parts, we'll bring them to you!", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_11 = new String[]{"/[label(Mysterious Old Man)]", "It's a promise!!", "/[waitkey(1)]/[clear()]", "Oops, I almost forgot to say this! Some robot parts are useless by themselves!", "/[waitkey(1)]/[clear()]", "It'd be helpful if you come back here each time you find some robot parts!", "/[waitkey(64)]/[close()]"};
    String[] SHI_1_12 = new String[]{"/[label(Shion)]", "Come back here each time...All right.", "/[waitkey(1)]/[clear()]", "By the way, what is your name?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_1_12 = new String[]{"/[label(Ziggy)]", "Come back here each time. Understood.", "/[waitkey(1)]/[clear()]", "By the way, what's your name?", "/[waitkey(64)]/[close()]"};
    String[] CHA_1_12 = new String[]{"/[label(chaos)]", "Come back here each time, huh? All right.", "/[waitkey(1)]/[clear()]", "By the way, what is your name?", "/[waitkey(64)]/[close()]"};
    String[] KOS_1_12 = new String[]{"/[label(KOS-MOS)]", "Come back here each time. Understood.", "/[waitkey(1)]/[clear()]", "What is your name?", "/[waitkey(64)]/[close()]"};
    String[] MOM_1_12 = new String[]{"/[label(MOMO)]", "I should come back here each time then? All right.", "/[waitkey(1)]/[clear()]", "By the way, what's your name?", "/[waitkey(64)]/[close()]"};
    String[] JR_1_12 = new String[]{"/[label(Jr.)]", "Come back here each time. All right, fine.", "/[waitkey(1)]/[clear()]", "By the way, old man, you got a name?", "/[waitkey(64)]/[close()]"};
    String[] BAKA_1_12 = new String[]{"/[label(Professor)]", "Me?", "/[waitkey(1)]/[clear()]", "Professor!", "/[waitkey(64)]/[close()]"};
    String[] SHI_2_00 = new String[]{"/[label(Shion)]", "Professor! We've found what you wanted!!", "/[waitkey(64)]/[close()]"};
    String[] CHA_2_00 = new String[]{"/[label(chaos)]", "Professor! We found some robot parts!", "/[waitkey(64)]/[close()]"};
    String[] KOS_2_00 = new String[]{"/[label(KOS-MOS)]", "We located some robot parts.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_2_00 = new String[]{"/[label(Ziggy)]", "We brought some robot parts that we found.", "/[waitkey(64)]/[close()]"};
    String[] MOM_2_00 = new String[]{"/[label(MOMO)]", "We found some robot parts, so we brought them!!", "/[waitkey(64)]/[close()]"};
    String[] JUN_2_00 = new String[]{"/[label(Jr.)]", "We found some robot parts, so we brought them for you!", "/[waitkey(64)]/[close()]"};
    String[] HAK_2_08 = new String[]{"/[label(Professor)]", "Well!! You already found some?!", "/[waitkey(1)]/[clear()]", "This is much faster than I'd anticipated!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_2_01 = new String[]{"/[label(Shion)]", "Much faster than you anticipated? We went through a lot of trouble, you know!", "/[waitkey(1)]/[clear()]", "You made us a promise! You're going to give us a special reward, right?", "/[waitkey(64)]/[close()]"};
    String[] CHA_2_01 = new String[]{"/[label(chaos)]", "We ran into quite a few difficulties, but we managed to find them!", "/[waitkey(1)]/[clear()]", "You promised us. You're going to give us a special reward, right?", "/[waitkey(64)]/[close()]"};
    String[] KOS_2_01 = new String[]{"/[label(KOS-MOS)]", "I was built to carry out my missions faithfully.", "/[waitkey(1)]/[clear()]", "Now, please give us the special reward, as was agreed.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_2_01 = new String[]{"/[label(Ziggy)]", "It was easy.", "/[waitkey(1)]/[clear()]", "Now, let's have the special reward, as was agreed upon.", "/[waitkey(64)]/[close()]"};
    String[] MOM_2_01 = new String[]{"/[label(MOMO)]", "We worked hard!", "/[waitkey(1)]/[clear()]", "So...um, what is the special reward that you promised...?", "/[waitkey(64)]/[close()]"};
    String[] JUN_2_01 = new String[]{"/[label(Jr.)]", "You make it sound like we got them easily, but it was actually pretty tough!", "/[waitkey(1)]/[clear()]", "Now, where's the special reward that we agreed on? You better give it to us like you promised.", "/[waitkey(64)]/[close()]"};
    String[] HAK_2_09 = new String[]{"/[label(Professor)]", "Oh? What is this about?", "/[waitkey(64)]/[close()]"};
    String[] SHI_2_02 = new String[]{"/[label(Shion)]", "(Sheesh, he's playing dumb...)\n", "All right then. This deal is off!", "/[waitkey(1)]/[clear()]", "I'm sure these parts should fetch a good price at a junk store.", "/[waitkey(64)]/[close()]"};
    String[] CHA_2_02 = new String[]{"/[label(chaos)]", "(Why is he playing dumb?)\n", "I understand.", "/[waitkey(1)]/[clear()]", "It's too bad that we will have to sell these rare robot parts to a junk store.", "/[waitkey(64)]/[close()]"};
    String[] KOS_2_02 = new String[]{"/[label(KOS-MOS)]", "You are in breach of contract.", "/[waitkey(1)]/[clear()]", "Very well then. We will sell these robot parts to a junk store.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_2_02 = new String[]{"/[label(Ziggy)]", "You broke our agreement.", "/[waitkey(1)]/[clear()]", "I see. We will sell these robot parts to a junk store.", "/[waitkey(64)]/[close()]"};
    String[] MOM_2_02 = new String[]{"/[label(MOMO)]", "What? You were lying?", "/[waitkey(1)]/[clear()]", "I see...we will just give these robot parts away to whoever wants them.", "/[waitkey(64)]/[close()]"};
    String[] JUN_2_02 = new String[]{"/[label(Jr.)]", "Hey! You tricked us?!", "/[waitkey(1)]/[clear()]", "Oh well...we'll go pawn these robot parts to a junk store or something.", "/[waitkey(64)]/[close()]"};
    String[] HAK_2_10 = new String[]{"/[label(Professor)]", "Wait, wait! It was just a little joke!", "/[waitkey(1)]/[clear()]", "But the thing is...", "/[waitkey(64)]/[close()]"};
    String[] SHI_2_03 = new String[]{"/[label(Shion)]", "But?", "/[waitkey(64)]/[close()]"};
    String[] CHA_2_03 = new String[]{"/[label(chaos)]", "?", "/[waitkey(64)]/[close()]"};
    String[] KOS_2_03 = new String[]{"/[label(KOS-MOS)]", "You have more demands?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_2_03 = new String[]{"/[label(Ziggy)]", "There's more?", "/[waitkey(64)]/[close()]"};
    String[] MOM_2_03 = new String[]{"/[label(MOMO)]", "But...but what?", "/[waitkey(64)]/[close()]"};
    String[] JUN_2_03 = new String[]{"/[label(Jr.)]", "What is it, old man?! You still want something?!", "/[waitkey(64)]/[close()]"};
    String[] HAK_2_11 = new String[]{"/[label(Professor)]", "The factory is so dirty, I don't feel like working!!", "/[waitkey(1)]/[clear()]", "I have a small favor to ask. Can you help me clean up the factory?", "/[waitkey(64)]/[close()]"};
    String[] SHI_2_04 = new String[]{"/[label(Shion)]", "Help you? You're going to make us do more work?!", "/[waitkey(64)]/[close()]"};
    String[] CHA_2_04 = new String[]{"/[label(chaos)]", "Help you? What do you want us to do?", "/[waitkey(64)]/[close()]"};
    String[] KOS_2_04 = new String[]{"/[label(KOS-MOS)]", "Clean the factory? Would it be fine if I use my own method?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_2_04 = new String[]{"/[label(Ziggy)]", "Cleaning? I don't mind helping, if you don't care what happens.", "/[waitkey(64)]/[close()]"};
    String[] MOM_2_04 = new String[]{"/[label(MOMO)]", "Cleaning? What should I clean?", "/[waitkey(64)]/[close()]"};
    String[] JUN_2_04 = new String[]{"/[label(Jr.)]", "Cleaning?! What the heck are we supposed to clean?!", "/[waitkey(64)]/[close()]"};
    String[] HAK_2_12 = new String[]{"/[label(Professor)]", "You people have Connection Gear, right?", "/[waitkey(1)]/[clear()]", "Use the Vaporizer Plug-in to blast away the clutter in the factory. Bam, bam, bam!!", "/[waitkey(1)]/[clear()]", "If you do that for me, there's no stopping my creativity...", "/[waitkey(64)]/[close()]"};
    String[] SHI_2_05 = new String[]{"/[label(Shion)]", "(He's so self-centered...)\n", "All right!!", "/[waitkey(1)]/[clear()]", "But in exchange, we're holding you to your promise!!", "/[waitkey(64)]/[close()]"};
    String[] CHA_2_05 = new String[]{"/[label(chaos)]", "(He's awfully rude...)\n", "I understand.", "/[waitkey(1)]/[clear()]", "But in exchange, we're going to hold you to your promise.", "/[waitkey(64)]/[close()]"};
    String[] KOS_2_05 = new String[]{"/[label(KOS-MOS)]", "Very well then.", "/[waitkey(1)]/[clear()]", "But please make sure you do not violate our agreement.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_2_05 = new String[]{"/[label(Ziggy)]", "Understood.", "/[waitkey(1)]/[clear()]", "But in exchange, we will take our compensation.", "/[waitkey(64)]/[close()]"};
    String[] MOM_2_05 = new String[]{"/[label(MOMO)]", "I understand.", "/[waitkey(1)]/[clear()]", "Please keep your promise!", "/[waitkey(64)]/[close()]"};
    String[] JUN_2_05 = new String[]{"/[label(Jr.)]", "Well, I guess we have no choice!", "/[waitkey(1)]/[clear()]", "But you better keep your promise!", "/[waitkey(64)]/[close()]"};
    String[] HAK_2_13 = new String[]{"/[label(Professor)]", "Of course!! Lies and oversweet coffee are the two things I hate most!!", "/[waitkey(1)]/[clear()]", "I will transform the robot parts that you brought into a special reward!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_3_00 = new String[]{"/[label(Shion)]", "Oh? This place seems livelier.", "/[waitkey(64)]/[close()]"};
    String[] CHA_3_00 = new String[]{"/[label(chaos)]", "Hmm. This place seems somewhat livelier.", "/[waitkey(64)]/[close()]"};
    String[] KOS_3_00 = new String[]{"/[label(KOS-MOS)]", "There is a slight change in the environment.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_3_00 = new String[]{"/[label(Ziggy)]", "Hmm? Things seem to be brighter than usual.", "/[waitkey(64)]/[close()]"};
    String[] MOM_3_00 = new String[]{"/[label(MOMO)]", "Oh? Something seems different than usual.", "/[waitkey(64)]/[close()]"};
    String[] JUN_3_00 = new String[]{"/[label(Jr.)]", "Hmm?! It's definitely livelier today!", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_03 = new String[]{"/[label(Professor)]", "Good of you to notice!!", "/[waitkey(1)]/[clear()]", "Actually, I have an assistant now!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_3_01 = new String[]{"/[label(Shion)]", "An assistant?", "/[waitkey(64)]/[close()]"};
    String[] CHA_3_01 = new String[]{"/[label(chaos)]", "An assistant...?", "/[waitkey(64)]/[close()]"};
    String[] KOS_3_01 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_3_01 = new String[]{"/[label(Ziggy)]", "Assistant?", "/[waitkey(64)]/[close()]"};
    String[] MOM_3_01 = new String[]{"/[label(MOMO)]", "An assistant?", "/[waitkey(64)]/[close()]"};
    String[] JUN_3_01 = new String[]{"/[label(Jr.)]", "Assistant?!", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_04 = new String[]{"/[label(Professor)]", "Argh!! Don't you know what an assistant means?!", "/[waitkey(1)]/[clear()]", "An assistant is a status symbol for a genius scientist!!", "/[waitkey(1)]/[clear()]", "The time has finally come for me to make my mark in the universe!", "/[waitkey(64)]/[close()]"};
    String[] SHI_3_02 = new String[]{"/[label(Shion)]", "Hmm...make your mark...?", "/[waitkey(64)]/[close()]"};
    String[] CHA_3_02 = new String[]{"/[label(chaos)]", "I see. Make your mark in the universe...", "/[waitkey(64)]/[close()]"};
    String[] KOS_3_02 = new String[]{"/[label(KOS-MOS)]", "Make your mark in the universe...?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_3_02 = new String[]{"/[label(Ziggy)]", "Make your mark in the universe...?", "/[waitkey(64)]/[close()]"};
    String[] MOM_3_02 = new String[]{"/[label(MOMO)]", "Make your mark in the universe...?", "/[waitkey(64)]/[close()]"};
    String[] JUN_3_02 = new String[]{"/[label(Jr.)]", "Hmm, make your mark in the universe? Well, aren't we the ambitious one!", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_05 = new String[]{"/[label(Professor)]", "What is this?! What is with your attitude?!", "/[waitkey(1)]/[clear()]", "So why did you come here today?", "/[waitkey(64)]/[close()]"};
    String[] SHI_3_03 = new String[]{"/[label(Shion)]", "Oh, that's right. We found more robot parts!", "/[waitkey(64)]/[close()]"};
    String[] CHA_3_03 = new String[]{"/[label(chaos)]", "Oh, right. We found some more robot parts!", "/[waitkey(64)]/[close()]"};
    String[] KOS_3_03 = new String[]{"/[label(KOS-MOS)]", "We located more robot parts.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_3_03 = new String[]{"/[label(Ziggy)]", "We brought some more robot parts that we found.", "/[waitkey(64)]/[close()]"};
    String[] MOM_3_03 = new String[]{"/[label(MOMO)]", "Oh! I forgot. We found robot parts again, so we brought them!!", "/[waitkey(64)]/[close()]"};
    String[] JUN_3_03 = new String[]{"/[label(Jr.)]", "Oh! That's right! We found some robot parts, so we brought them!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_06 = new String[]{"/[label(Professor)]", "Well, well...You guys are pretty good.", "/[waitkey(1)]/[clear()]", "We must show this to Assistant Scott too!", "/[waitkey(1)]/[clear()]", "Hey! Assistant Scott!!", "/[waitkey(64)]/[close()]"};
    String[] SCO_3_01 = new String[]{"/[label(Assistant Scott)]", "Yes, what is it, Professor?", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_07 = new String[]{"/[label(Professor)]", "These guys are my loyal underlings, and by my orders, they are collecting robot parts!!", "/[waitkey(1)]/[clear()]", "It seems they've brought a new robot part today!", "/[waitkey(64)]/[close()]"};
    String[] SCO_3_02 = new String[]{"/[label(Assistant Scott)]", "Wow! So you people are the professor's underlings?", "/[waitkey(64)]/[close()]"};
    String[] SHI_3_04 = new String[]{"/[label(Shion)]", "...Fine, if that's what he wants to call us.", "/[waitkey(1)]/[clear()]", "So what should I call you? Can I just call you Scott?", "/[waitkey(64)]/[close()]"};
    String[] CHA_3_04 = new String[]{"/[label(chaos)]", "...We'll leave it at that for now.\n", "/[waitkey(1)]/[clear()]", "So how should we refer to you? Would just Scott be all right?", "/[waitkey(64)]/[close()]"};
    String[] KOS_3_04 = new String[]{"/[label(KOS-MOS)]", "Currently, my only commander is Shion, but please consider us as such in order to save face.", "/[waitkey(1)]/[clear()]", "By the way, what is your name? If you do not have a specific rank, I shall refer to you simply as Scott.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_3_04 = new String[]{"/[label(Ziggy)]", "We'll just let the old man say what he wants.", "/[waitkey(1)]/[clear()]", "So what do we call you? Is just Scott all right?", "/[waitkey(64)]/[close()]"};
    String[] MOM_3_04 = new String[]{"/[label(MOMO)]", "I don't ever remember becoming the professor's underling...", "/[waitkey(1)]/[clear()]", "So what should I call you? Is Scott okay?", "/[waitkey(64)]/[close()]"};
    String[] JUN_3_04 = new String[]{"/[label(Jr.)]", "The guy sure says whatever he feels like.", "/[waitkey(1)]/[clear()]", "So what should we call you? Is Scott okay?", "/[waitkey(64)]/[close()]"};
    String[] SCO_3_03 = new String[]{"/[label(Assistant Scott)]", "Yes, please call me whatever you like.", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_08 = new String[]{"/[label(Professor)]", "No!! Assistant Scott is Assistant Scott!!", "/[waitkey(1)]/[clear()]", "Calling Assistant Scott just Scott would be like calling me Profe or Pro!", "/[waitkey(1)]/[clear()]", "And since I am neither Profe nor Pro, Assistant Scott is Assistant Scott!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_3_05 = new String[]{"/[label(Shion)]", "Yes, yes, all right. By the way, are you going to modify the robot parts for us today too?", "/[waitkey(64)]/[close()]"};
    String[] CHA_3_05 = new String[]{"/[label(chaos)]", "I-I see. By the way, will you be modifying the robot parts for us today too?", "/[waitkey(64)]/[close()]"};
    String[] KOS_3_05 = new String[]{"/[label(KOS-MOS)]", "That was an illogical argument. It is pointless to discuss this issue any further.", "/[waitkey(1)]/[clear()]", "We came today to have more robot parts modified.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_3_05 = new String[]{"/[label(Ziggy)]", "There's no talking sense with this old man.", "/[waitkey(1)]/[clear()]", "So you are going to modify the robot part for us today too, correct?", "/[waitkey(64)]/[close()]"};
    String[] MOM_3_05 = new String[]{"/[label(MOMO)]", "I'm sorry...", "/[waitkey(1)]/[clear()]", "By the way, we came to have more robot parts modified.", "/[waitkey(64)]/[close()]"};
    String[] JUN_3_05 = new String[]{"/[label(Jr.)]", "All right, all right!", "/[waitkey(1)]/[clear()]", "So you're going to modify more robot parts for us, right?", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_09 = new String[]{"/[label(Professor)]", "Yes!! That's right!!", "/[waitkey(1)]/[clear()]", "Then let's go! Assistant Scott!!", "/[waitkey(64)]/[close()]"};
    String[] SCO_4_01 = new String[]{"/[label(Assistant Scott)]", "That was the last straw!", "/[waitkey(64)]/[close()]"};
    String[] SHI_4_00 = new String[]{"/[label(Shion)]", "?!", "/[waitkey(1)]/[clear()]", "What's the matter?!", "/[waitkey(64)]/[close()]"};
    String[] CHA_4_00 = new String[]{"/[label(chaos)]", "Did something happen?!", "/[waitkey(64)]/[close()]"};
    String[] KOS_4_00 = new String[]{"/[label(KOS-MOS)]", "There seems to be a commotion. What happened?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_4_00 = new String[]{"/[label(Ziggy)]", "It's rather rowdy here. Is something the matter?", "/[waitkey(64)]/[close()]"};
    String[] MOM_4_00 = new String[]{"/[label(MOMO)]", "!!", "/[waitkey(1)]/[clear()]", "Did something happen?", "/[waitkey(64)]/[close()]"};
    String[] JUN_4_00 = new String[]{"/[label(Jr.)]", "Heh, look at them go! What happened?! Want me to lend you a hand too?", "/[waitkey(64)]/[close()]"};
    String[] HAK_4_01 = new String[]{"/[label(Professor)]", "What happened?! He's just an assistant, and he dares to defy me, the professor!!", "/[waitkey(64)]/[close()]"};
    String[] SCO_4_02 = new String[]{"/[label(Assistant Scott)]", "I said what I said because I'm worried about you!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_4_01 = new String[]{"/[label(Shion)]", "Hey! Just what happened?", "/[waitkey(64)]/[close()]"};
    String[] CHA_4_01 = new String[]{"/[label(chaos)]", "Both of you, calm down. Please tell us what happened!!", "/[waitkey(64)]/[close()]"};
    String[] KOS_4_01 = new String[]{"/[label(KOS-MOS)]", "It may be possible to bring things under control if you would explain the situation.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_4_01 = new String[]{"/[label(Ziggy)]", "Both of you, just calm down! What happened?", "/[waitkey(64)]/[close()]"};
    String[] MOM_4_01 = new String[]{"/[label(MOMO)]", "Please stop fighting!! What in the world is going on?", "/[waitkey(64)]/[close()]"};
    String[] JUN_4_01 = new String[]{"/[label(Jr.)]", "Enough, both of you! What the heck happened?", "/[waitkey(64)]/[close()]"};
    String[] HAK_4_02 = new String[]{"/[label(Professor)]", "Assistant Scott here has forgotten his position as an assistant ", "and is ordering me to quit drinking!", "/[waitkey(1)]/[clear()]", "You guys talk to him and help me put him in his place!", "/[waitkey(64)]/[close()]"};
    String[] SCO_4_03 = new String[]{"/[label(Assistant Scott)]", "I used to admire you, Professor. But lately, all you do is drink all day!", "/[waitkey(1)]/[clear()]", "You haven't been doing any research!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_4_02 = new String[]{"/[label(Shion)]", "...The professor is definitely in the wrong.", "/[waitkey(64)]/[close()]"};
    String[] CHA_4_02 = new String[]{"/[label(chaos)]", "...I think the professor is clearly in the wrong here.", "/[waitkey(64)]/[close()]"};
    String[] KOS_4_02 = new String[]{"/[label(KOS-MOS)]", "Under the circumstances, there is an 82.256% chance that the professor is at fault.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_4_02 = new String[]{"/[label(Ziggy)]", "...No matter how you look at it, isn't the professor the one at fault?", "/[waitkey(64)]/[close()]"};
    String[] MOM_4_02 = new String[]{"/[label(MOMO)]", "I don't know all the details, but from what I've heard, I feel that the professor is at fault.", "/[waitkey(64)]/[close()]"};
    String[] JUN_4_02 = new String[]{"/[label(Jr.)]", "Hey, hey! No matter how you look at it, you're the one at fault here, old man!", "/[waitkey(64)]/[close()]"};
    String[] HAK_4_03 = new String[]{"/[label(Professor)]", "Wha?! You agree with him?", "/[waitkey(1)]/[clear()]", "Grrr!! It's your fault for saying incoherent things, Assistant Scott!!", "/[waitkey(1)]/[clear()]", "You stupid tattletale!!", "/[waitkey(64)]/[close()]"};
    String[] SCO_4_04 = new String[]{"/[label(Assistant Scott)]", "Stupid? Did you just call me stupid?", "/[waitkey(1)]/[clear()]", "If I'm stupid, then you're stupid stupid!!", "/[waitkey(1)]/[clear()]", "You stupid, stupid professor!", "/[waitkey(64)]/[close()]"};
    String[] SHI_4_03 = new String[]{"/[label(Shion)]", "(Umm...they're both pretty similar.)", "/[waitkey(64)]/[close()]"};
    String[] CHA_4_03 = new String[]{"/[label(chaos)]", "(...)", "/[waitkey(64)]/[close()]"};
    String[] KOS_4_03 = new String[]{"/[label(KOS-MOS)]", "(...)", "/[waitkey(64)]/[close()]"};
    String[] ZIG_4_03 = new String[]{"/[label(Ziggy)]", "(This is too absurd for words.)", "/[waitkey(64)]/[close()]"};
    String[] MOM_4_03 = new String[]{"/[label(MOMO)]", "(I don't think that's going to help this situation...)", "/[waitkey(64)]/[close()]"};
    String[] JUN_4_03 = new String[]{"/[label(Jr.)]", "(What?! His reasoning makes absolutely no sense!)", "/[waitkey(64)]/[close()]"};
    String[] HAK_4_04 = new String[]{"/[label(Professor)]", "Now I'm mad!! I'm very, very mad now!!", "/[waitkey(1)]/[clear()]", "You're dismissed!!", "/[waitkey(64)]/[close()]"};
    String[] SCO_4_05 = new String[]{"/[label(Assistant Scott)]", "That's fine with me! I'm sick of being your assistant!", "/[waitkey(1)]/[clear()]", "You just go enjoy your liquor! Well then, if you will excuse me!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_4_04 = new String[]{"/[label(Shion)]", "Oh boy...Assistant Scott left, Professor. Are you sure you don't want to stop him?", "/[waitkey(64)]/[close()]"};
    String[] CHA_4_04 = new String[]{"/[label(chaos)]", "Assistant Scott left. Maybe you should have stopped him.", "/[waitkey(64)]/[close()]"};
    String[] KOS_4_04 = new String[]{"/[label(KOS-MOS)]", "Considering his importance and his low probability of returning on his own, I recommend that you try to stop him.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_4_04 = new String[]{"/[label(Ziggy)]", "He left. Are you sure you don't need to stop him?", "/[waitkey(64)]/[close()]"};
    String[] MOM_4_04 = new String[]{"/[label(MOMO)]", "Assistant Scott left. Shouldn't you stop him?", "/[waitkey(64)]/[close()]"};
    String[] JUN_4_04 = new String[]{"/[label(Jr.)]", "Uh-oh...that guy took off! Shouldn't you go after him?", "/[waitkey(64)]/[close()]"};
    String[] HAK_4_05 = new String[]{"/[label(Professor)]", "Just leave him alone! He'll eventually come crying back!!", "/[waitkey(1)]/[clear()]", "He'll beg to me, \"Professor, please forgive me!\" More importantly, you brought more robot parts today, right? I'll get right to work on those!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_4_05 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] CHA_4_05 = new String[]{"/[label(chaos)]", "...", "/[waitkey(64)]/[close()]"};
    String[] KOS_4_05 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_4_05 = new String[]{"/[label(Ziggy)]", "...", "/[waitkey(64)]/[close()]"};
    String[] MOM_4_05 = new String[]{"/[label(MOMO)]", "...", "/[waitkey(64)]/[close()]"};
    String[] JUN_4_05 = new String[]{"/[label(Jr.)]", "...", "/[waitkey(64)]/[close()]"};
    String[] SHI_5_00 = new String[]{"/[label(Shion)]", "Professor! We finally got it!! The last robot part!!", "/[waitkey(64)]/[close()]"};
    String[] CHA_5_00 = new String[]{"/[label(chaos)]", "Professor. We found the last robot part.", "/[waitkey(64)]/[close()]"};
    String[] KOS_5_00 = new String[]{"/[label(KOS-MOS)]", "Professor. We have successfully obtained the last robot part.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_5_00 = new String[]{"/[label(Ziggy)]", "Professor, we found the last robot part.", "/[waitkey(64)]/[close()]"};
    String[] MOM_5_00 = new String[]{"/[label(MOMO)]", "Professor! We located the last robot part!!", "/[waitkey(64)]/[close()]"};
    String[] JUN_5_00 = new String[]{"/[label(Jr.)]", "Old man! We brought the last robot part for you!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_01 = new String[]{"/[label(Professor)]", "Ooh! Good job!", "/[waitkey(1)]/[clear()]", "Now I can complete the giant robot that I've yearned for...", "/[waitkey(1)]/[clear()]", "*Sigh...*", "/[waitkey(64)]/[close()]"};
    String[] SHI_5_01 = new String[]{"/[label(Shion)]", "You don't seem very cheerful. Is it because of what happened with Assistant Scott?", "/[waitkey(64)]/[close()]"};
    String[] CHA_5_01 = new String[]{"/[label(chaos)]", "You don't seem very cheerful. Does it have something to do with Assistant Scott?", "/[waitkey(64)]/[close()]"};
    String[] KOS_5_01 = new String[]{"/[label(KOS-MOS)]", "I sense irregularities in your beta wave. Are you worried about your assistant?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_5_01 = new String[]{"/[label(Ziggy)]", "Hmm, you seem a bit down. Are you worried about that young man after all?", "/[waitkey(64)]/[close()]"};
    String[] MOM_5_01 = new String[]{"/[label(MOMO)]", "Professor, you seem rather down. Are you worried about Assistant Scott?", "/[waitkey(64)]/[close()]"};
    String[] JUN_5_01 = new String[]{"/[label(Jr.)]", "What's wrong, old man? You're awfully quiet.", "/[waitkey(1)]/[clear()]", "You're still worried about that guy, aren't you?", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_02 = new String[]{"/[label(Professor)]", "Wha! What are you talking about?! I've already forgotten about that ungrateful assistant!!", "/[waitkey(1)]/[clear()]", "More importantly, hurry up and hand over the last robot part! Let's complete that invincible robot that I've dreamed of!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_5_03 = new String[]{"/[label(Shion)]", "Hmm. Are you sure everything's okay?", "/[waitkey(64)]/[close()]"};
    String[] CHA_5_03 = new String[]{"/[label(chaos)]", "Hmm. Are you sure everything is all right?", "/[waitkey(64)]/[close()]"};
    String[] KOS_5_03 = new String[]{"/[label(KOS-MOS)]", "Understood.", "/[waitkey(1)]/[clear()]", "Just to confirm your answer, are you sure you do not want to commence a search for the assistant?", "/[waitkey(64)]/[close()]"};
    String[] ZIG_5_03 = new String[]{"/[label(Ziggy)]", "...", "/[waitkey(1)]/[clear()]", "Are you sure you don't want to search for that young man?", "/[waitkey(64)]/[close()]"};
    String[] MOM_5_03 = new String[]{"/[label(MOMO)]", "...", "/[waitkey(1)]/[clear()]", "Professor...are you really sure you don't want to look for Assistant Scott?", "/[waitkey(64)]/[close()]"};
    String[] JUN_5_03 = new String[]{"/[label(Jr.)]", "You don't have to act so tough, you know.", "/[waitkey(1)]/[clear()]", "Come on, old man, you really okay with leaving that guy alone?", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_03 = new String[]{"/[label(Professor)]", "It doesn't matter! Come!! Let's start working!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_5_04 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] CHA_5_04 = new String[]{"/[label(chaos)]", "...", "/[waitkey(64)]/[close()]"};
    String[] KOS_5_04 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_5_04 = new String[]{"/[label(Ziggy)]", "...", "/[waitkey(64)]/[close()]"};
    String[] MOM_5_04 = new String[]{"/[label(MOMO)]", "...", "/[waitkey(64)]/[close()]"};
    String[] JUN_5_04 = new String[]{"/[label(Jr.)]", "...", "/[waitkey(64)]/[close()]"};
    String[] SHI_6_000 = new String[]{"/[label(Shion)]", "Professor!! We've brought back Assistant Scott!!", "/[waitkey(1)]/[clear()]", "You can make your dream come true with him here, right?", "/[waitkey(64)]/[close()]"};
    String[] CHA_6_000 = new String[]{"/[label(chaos)]", "Professor. We found Assistant Scott!!", "/[waitkey(1)]/[clear()]", "Let's hurry up and make your dream come true!!", "/[waitkey(64)]/[close()]"};
    String[] KOS_6_000 = new String[]{"/[label(KOS-MOS)]", "We have found and restrained the target.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_6_000 = new String[]{"/[label(Ziggy)]", "We brought back the person you've been waiting for!", "/[waitkey(64)]/[close()]"};
    String[] MOM_6_000 = new String[]{"/[label(MOMO)]", "Professor! We brought back Assistant Scott.", "/[waitkey(1)]/[clear()]", "Now you can make your dream come true, right?", "/[waitkey(64)]/[close()]"};
    String[] JUN_6_000 = new String[]{"/[label(Jr.)]", "Old man! We brought your assistant guy!!", "/[waitkey(1)]/[clear()]", "Let's hurry up and make the giant robot!", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_000 = new String[]{"/[label(Assistant Scott)]", "Professor...\n", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_000 = new String[]{"/[label(Professor)]", "Hah! What a meddlesome thing to do!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_6_001 = new String[]{"/[label(Shion)]", "What? I don't think it's fair of you to say that!", "/[waitkey(64)]/[close()]"};
    String[] CHA_6_001 = new String[]{"/[label(chaos)]", "...And I was hoping you would be happy. That's too bad.", "/[waitkey(64)]/[close()]"};
    String[] KOS_6_001 = new String[]{"/[label(KOS-MOS)]", "It seems that it would be better to release him.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_6_001 = new String[]{"/[label(Ziggy)]", "That's putting it strongly.", "/[waitkey(1)]/[clear()]", "If you want, we could just take him back with us to the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] MOM_6_001 = new String[]{"/[label(MOMO)]", "Saying it like that...", "/[waitkey(1)]/[clear()]", "makes me a little sad.", "/[waitkey(64)]/[close()]"};
    String[] JUN_6_001 = new String[]{"/[label(Jr.)]", "Hey! That's going a bit too far!", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_001 = new String[]{"/[label(Assistant Scott)]", "...\n", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_001 = new String[]{"/[label(Professor)]", "Well, whatever! Come, Assistant Scott. Get to your station quickly!", "/[waitkey(1)]/[clear()]", "We have a big, once-in-a-lifetime job to do!!", "/[waitkey(64)]/[close()]"};
    String[] SHI_6_002 = new String[]{"/[label(Shion)]", "W-wait a minute. That isn't fair to Assistant Scott.", "/[waitkey(64)]/[close()]"};
    String[] CHA_6_002 = new String[]{"/[label(chaos)]", "Come on!! That isn't fair to Assistant Scott.", "/[waitkey(64)]/[close()]"};
    String[] KOS_6_002 = new String[]{"/[label(KOS-MOS)]", "In this instance, I do not sense any need for Assistant Scott to cooperate with the professor.", "/[waitkey(64)]/[close()]"};
    String[] ZIG_6_002 = new String[]{"/[label(Ziggy)]", "An utterly rude old man. You're still making demands to Assistant Scott?", "/[waitkey(64)]/[close()]"};
    String[] MOM_6_002 = new String[]{"/[label(MOMO)]", "That's terrible. I thought the professor was a nicer person!", "/[waitkey(64)]/[close()]"};
    String[] JUN_6_002 = new String[]{"/[label(Jr.)]", "Don't get egotistical, old man!", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_002 = new String[]{"/[label(Assistant Scott)]", "Yes, sir! Roger that, Professor!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_002 = new String[]{"/[label(Professor)]", "And you...", "/[waitkey(64)]/[close()]"};
    String[] SHI_6_003 = new String[]{"/[label(Shion)]", "??", "/[waitkey(64)]/[close()]"};
    String[] CHA_6_003 = new String[]{"/[label(chaos)]", "?", "/[waitkey(64)]/[close()]"};
    String[] KOS_6_003 = new String[]{"/[label(KOS-MOS)]", "...", "/[waitkey(64)]/[close()]"};
    String[] ZIG_6_003 = new String[]{"/[label(Ziggy)]", "...?", "/[waitkey(64)]/[close()]"};
    String[] MOM_6_003 = new String[]{"/[label(MOMO)]", "?", "/[waitkey(64)]/[close()]"};
    String[] JUN_6_003 = new String[]{"/[label(Jr.)]", "??", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_003 = new String[]{"/[label(Professor)]", "...I'm glad you're back.", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_004 = new String[]{"/[label(Professor)]", "Let's go, Assistant Scott!! Begin transformation!", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_005 = new String[]{"/[label(Professor)]", "(You've matured into a man in the short time that I hadn't seen you.)", "/[waitkey(1)]/[clear()]", "(This Academy should be safe in Assistant Scott's hands.)", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_003 = new String[]{"/[label(Assistant Scott)]", "Okay, Professor!! Set transformation!", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_004 = new String[]{"/[label(Assistant Scott)]", "(Professor, you're definitely the greatest.)", "/[waitkey(1)]/[clear()]", "(You really shine when you're making robots!)", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_006 = new String[]{"/[label(Professor)]", "Behold!! Love, courage, justice, victory, friendship, effort...", "/[waitkey(1)]/[clear()]", "The symbol of all such things, united and invincible, absolutely just, the \"Erde Kaiser\"!!", "/[waitkey(1)]/[clear()]", "Oh, just to let you know, but the Erde Kaiser is an Ether that only Shion can use! Keep that well in mind!!", "/[waitkey(64)]/[close()]"};
    String[] tHAK_00_a = new String[]{"/[label(Professor)]", "Then here we go!! Begin transformation!!", "/[waitkey(64)]/[close()]"};
    String[] tHAK_00_b = new String[]{"/[label(Professor)]", "Those are the robot parts that you brought!!", "/[waitkey(1)]/[clear()]", "Then here we go!! Begin transformation!!", "/[waitkey(64)]/[close()]"};
    String[] tSCO_00 = new String[]{"/[label(Assistant Scott)]", "Okay!! Set transformation!!", "/[waitkey(64)]/[close()]"};
    String[] tHAK_01 = new String[]{"/[label(Professor)]", "How's that?! The super weapon \"Throni Blade\" is complete!!", "/[waitkey(1)]/[clear()]", "We'll set this up as an Ether for Shion! Summon it during a battle and you'll blast away the enemies!!", "/[waitkey(64)]/[close()]"};
    String[] tHAK_02 = new String[]{"/[label(Professor)]", "How's that?! The \"Dominion Tank\" is complete!!", "/[waitkey(1)]/[clear()]", "We'll set this up as an Ether for Shion! Summon it during a battle and you'll blast away the enemies!!", "/[waitkey(64)]/[close()]"};
    String[] tHAK_03 = new String[]{"/[label(Professor)]", "How's that?! The super weapon \"Seraphim Bird\" is complete!!", "/[waitkey(1)]/[clear()]", "We'll set this up as an ether technique for Shion! Summon it during a battle and you'll blast away the enemies!!", "/[waitkey(64)]/[close()]"};
    String[] tHAK_04 = new String[]{"/[label(Professor)]", "How's that?! An essential section for the giant robot, the \"Kelbim Dragon\" is complete!!", "/[waitkey(1)]/[clear()]", "Just remember, the \"Kelbim Dragon\" is a joint part of the giant robot, so unfortunately, you won't be able to summon it during battles!", "/[waitkey(64)]/[close()]"};
    String[] tHAK_05 = new String[]{"/[label(Professor)]", "Behold!! Love, courage, justice, victory, friendship, effort...", "/[waitkey(1)]/[clear()]", "The symbol of all such things, united and invincible, absolutely just, the \"Erde Kaiser\"!!", "/[waitkey(64)]/[close()]"};
    String[] Con_TORO_00 = new String[]{"♪When a flame rises on the battlefield, those with evil hearts are certain to vanish instantly...", "/[waitkey(1)]/[clear()]", "The steel gale that tears through space-time, the \"Throni Blade\"!!", "/[waitkey(1)]/[clear()]", "See other developed mechs?\n", "/[waitkey(64)]/[close()]"};
    String[] Con_DOMI_00 = new String[]{"♪The dual colored spirits dwelling in the steel body. Justice and courage cuts down the enemy!!", "/[waitkey(1)]/[clear()]", "The golden lightning that shakes the earth, the \"Dominion Tank\"!!", "/[waitkey(1)]/[clear()]", "See other developed mechs?\n", "/[waitkey(64)]/[close()]"};
    String[] Con_SERA_00 = new String[]{"♪Hope and peace, sworn for eternity. If any dare to defile them, it will appear at the speed of light to eradicate the evil!!", "/[waitkey(1)]/[clear()]", "The billowing sonic wing, the \"Seraphim Bird\" is here!!", "/[waitkey(1)]/[clear()]", "See other developed mechs?\n", "/[waitkey(64)]/[close()]"};
    String[] Con_KERU_00 = new String[]{"♪There's no time for tears! We wish for children's smiles. Take back, take back, take back our mother Earth!!", "/[waitkey(1)]/[clear()]", "The guardian of the ages, the \"Kelbim Dragon\"!!", "/[waitkey(1)]/[clear()]", "See other developed mechs?\n", "/[waitkey(64)]/[close()]"};
    String[] Con_ELDK_00 = new String[]{"♪Hit them with our fiery fighting spirit!! Battle with the power of friendship!! Strength is your partner!! Strive for the future with your wonderful heart!!", "/[waitkey(1)]/[clear()]", "Invincible, indestructible, \"Erde Kaiser\"!!", "/[waitkey(1)]/[clear()]", "See other developed mechs?\n", "/[waitkey(64)]/[close()]"};
    String[] Con_F_00 = new String[]{"A timeworn console. It is a rather old model and does not seem to have been used for a long time.", "/[waitkey(64)]/[close()]"};
    String[] Con_F_01 = new String[]{"/[label(Assistant Scott's Voice)]", "I'll have it serviced by the next time you visit!", "/[waitkey(64)]/[close()]"};
    String[] Elmoni_00 = new String[]{"[Professor's Ultimate Selection]\n", "\"Elde Kaiser\"", "/[waitkey(1)]/[clear()]", "When the mother computer at Kukai Robot Academy senses that someone with a just heart is in danger, the four mechs: \"Throni Blade,\" \"Dominion Tank,\" \"Seraphim Bird,\" and \"Kelbim Dragon\" are transported to the battlefield from the professor and Assistant Scott's lab, and arrive at the battle as one giant robot.", "/[waitkey(1)]/[clear()]", "That is the \"Erde Kaiser\"!!", "/[waitkey(1)]/[clear()]", "The time it takes for the mechs to arrive at the battle is 0.04 nanoseconds.", "/[waitkey(1)]/[clear()]", "You can confirm the transport process during combat!!", "/[waitkey(64)]/[close()]"};
    String[] Elmoni_01 = new String[]{"[Professor's Ultimate Selection]", "/[waitkey(1)]/[clear()]", "\"Erde Kaiser\" (conceptional drawing of complete model)\n", "It seems to be a collaboration between Assistant Scott and the professor. It's power source is apparently...caring hearts.", "/[waitkey(64)]/[close()]"};
    String[] Robo4_1_00 = new String[]{"[Professor's Best Selection]\n", "Part 1", "/[waitkey(1)]/[clear()]", "Iron Man B Unit 8 (1/1)\n", "It's an old type of mecha,\n", "so it won't work without a remote control!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_1_01 = new String[]{"/[label(Professor's Voice)]", "♪B eight, B eight, B Unit 8!!♪\n", "You're going to operate the B Unit 8?", "/[waitkey(64)]/[close()]"};
    String[] Robo4_1_02 = new String[]{"/[label(Professor's Voice)]", "♪B eight, B eight, B Unit 8!!♪\n", "Today's battle is over!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_1_03 = new String[]{"[Professor's Best Selection]\n", "Part 1", "/[waitkey(1)]/[clear()]", "Iron Man B Unit 8 (1/1)\n", "It should work with some repairs.", "/[waitkey(64)]/[close()]"};
    String[] Robo4_1_04 = new String[]{"The lever on the B Unit 8 remote control\n", "is rusted and won't move.", "/[waitkey(64)]/[close()]"};
    String[] Robo4_2_00 = new String[]{"[Professor's Best Selection]\n", "Part 2", "/[waitkey(1)]/[clear()]", "Kaiser GO!! (1/2)", "/[waitkey(1)]/[clear()]", "the Kaiser GO!! too?!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_2_01 = new String[]{"Here goooeees!!\n", "Kaiseeeer, driiiil!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_2_02 = new String[]{"Kaiseeeer, cheeeest!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_2_03 = new String[]{"Kaiseeeer, pinceeeer!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_2_04 = new String[]{"Kaiser, puuuunch!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_2_05 = new String[]{"[Professor Best Selection]\n", "Part 2", "/[waitkey(1)]/[clear()]", "Kaiser GO!! (1/2)", "/[waitkey(1)]/[clear()]", "The Kaiser GO!!\n", "is being rested!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_2_06 = new String[]{"[Professor Best Selection]\n", "Part 2", "/[waitkey(1)]/[clear()]", "Kaiser GO!! (1/2)\n", "It should work with some repairs.", "/[waitkey(64)]/[close()]"};
    String[] Robo4_3_00 = new String[]{"[Professor Best Selection]\n", "Part 3", "/[waitkey(1)]/[clear()]", "Rising Sun (1/2)", "/[waitkey(1)]/[clear()]", "Believe in the Rising Sun!!\n", "The time of awakening is here!!\n", "Are you going to deploy the Rising Sun in real combat?", "/[waitkey(64)]/[close()]"};
    String[] Robo4_3_01a = new String[]{"/[label(Womanly Voice of Professor For Some Reason)]", "Professor, can you hear me?\n", "Evil is approaching!!\n", "Good luck!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_3_01 = new String[]{"/[label(Professor's Voice)]", "Roger!!", "/[waitkey(1)]/[clear()]", "Here goes the Rising Sun!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_3_02 = new String[]{"[Professor's Best Selection]\n", "Part 3", "/[waitkey(1)]/[clear()]", "Rising Sun (1/2)", "Rising Sun, withdrawing from battle.", "/[waitkey(64)]/[close()]"};
    String[] Robo4_3_03 = new String[]{"[Professor's Best Selection]\n", "Part 3", "/[waitkey(1)]/[clear()]", "Rising Sun (1/2)\n", "It should work with some repairs.", "/[waitkey(64)]/[close()]"};
    String[] Robo4_4_00 = new String[]{"[Professor's Best Selection]\n", "Part 4", "/[waitkey(1)]/[clear()]", "Element DX (1/3)", "/[waitkey(1)]/[clear()]", "Want to operate it now, don't you?", "/[waitkey(64)]/[close()]"};
    String[] Robo4_4_01 = new String[]{"/[label(Professor's Voice)]", "Sword of justice,\n", "blade of lightning!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_4_02 = new String[]{"/[label(Professor's Voice)]", "Red hot flames,\n", "fire wing!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_4_03 = new String[]{"/[label(Professor's Voice)]", "Soothing clear stream,\n", "healing water!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_4_04 = new String[]{"It may just be how I feel,", "/[waitkey(1)]/[clear()]", "but it seems HP & EP have\n", "been recovered!!", "/[waitkey(64)]/[close()]"};
    String[] Robo4_4_05 = new String[]{"/[waitkey(1)]/[clear()]", "[Professor's Best Selection]\n", "Part 4", "/[waitkey(1)]/[clear()]", "Element DX (1/3)", "/[waitkey(1)]/[clear()]", "It's being rested for tomorrow.", "/[waitkey(64)]/[close()]"};
    String[] Robo4_4_06 = new String[]{"[Professor's Best Selection]\n", "Part 4", "/[waitkey(1)]/[clear()]", "Element DX (1/3)\n", "It should work with some repairs.", "/[waitkey(64)]/[close()]"};
    String[] Hak1_00 = new String[]{"/[label(Professor)]", "A man's dream is a giant robot!!", "/[waitkey(64)]/[close()]"};
    String[] Hak1_01 = new String[]{"/[label(Professor)]", "You understand?! Some robot parts are useless if they are by themselves!!", "/[waitkey(1)]/[clear()]", "If you find robot parts somewhere, it'd be helpful if you come back here each time!", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_00 = new String[]{"/[label(Professor)]", "It's just that I don't feel like working when I'm in a dirty place.", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_01 = new String[]{"/[label(Professor)]", "Can't you clean it up a bit more?", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_02 = new String[]{"/[label(Professor)]", "Phew!! I'm starting to feel like working now!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_03 = new String[]{"/[label(Professor)]", "Close! Sooo close! If this place was just a bit cleaner, it would set my creativity on fire!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_04 = new String[]{"/[label(Professor)]", "Good job you guys! Now I can concentrate on my research!!", "/[waitkey(1)]/[clear()]", "By the way, you guys sure have a lot of free time.", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_05 = new String[]{"/[label(Professor)]", "Finally, the first step towards our dream! I'm going to do it! I'm going to do it!", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_06 = new String[]{"/[label(Professor)]", "Hey!! Where are you going?! You're not done cleaning yet!", "/[waitkey(64)]/[close()]"};
    String[] HAK_SOUJI_07 = new String[]{"/[label(Professor)]", "What now?! Don't you want the special reward?!", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_a0 = new String[]{"/[label(Professor)]", "Hmm...where could the rest of the robot parts be?", "/[waitkey(1)]/[clear()]", "When I think of getting them all together, it makes my heart sing!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_3_a1 = new String[]{"/[label(Professor)]", "We are making steady progress towards our ambition! Oh! Am I a greedy old geezer??", "/[waitkey(64)]/[close()]"};
    String[] SCO_3_a0 = new String[]{"/[label(Assistant Scott)]", "When I have the time, I plan on repairing the other robots that the professor has been working on.", "/[waitkey(64)]/[close()]"};
    String[] SCO_3_a1 = new String[]{"/[label(Assistant Scott)]", "The professor is a wonderful scientist, but he is a bit too fond of the bottle. But everyone has shortcomings, right?", "/[waitkey(64)]/[close()]"};
    String[] HAK_4_a0 = new String[]{"/[label(Professor)]", "I can do anything by myself! I've always been able to before. I don't need Assistant Scott!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_4_a1 = new String[]{"/[label(Professor)]", "Only a few robot parts remaining...", "/[waitkey(1)]/[clear()]", "I'll manage, even without Assistant Scott!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_a0 = new String[]{"/[label(Professor)]", "Now, this should be all the robot parts!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_a1 = new String[]{"/[label(Professor)]", "I'll manage, even without Assistant Scott!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_a2 = new String[]{"/[label(Professor)]", "Friendship and justice. I knew that one could never ever be apart from the other.", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_a3 = new String[]{"/[label(Professor)]", "I said some mean things to Assistant Scott. I wonder what he's doing now?", "/[waitkey(64)]/[close()]"};
    String[] HAK_5_a4 = new String[]{"/[label(Professor)]", "The factory is cluttered again, you say? Ever since Assistant Scott left, I haven't felt like cleaning.", "/[waitkey(64)]/[close()]"};
    String[] HAK_CCC_00 = new String[]{"/[label(Professor)]", "Wait a minute!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_CCC_01 = new String[]{"/[label(Professor)]", "Well, well, now that the six kinds of robot parts are finally all here, I'll set about completing the giant robot!!", "/[waitkey(1)]/[clear()]", "If Assistant Scott hadn't defied me, he would have been present at this historic moment.", "/[waitkey(1)]/[clear()]", "Oh well!! I'll just forget about him and get started!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_CCC_02 = new String[]{"/[label(Professor)]", "Now, just you watch! Set transformation!", "/[waitkey(64)]/[close()]"};
    String[] HAK_CCC_03 = new String[]{"/[label(Professor)]", "Wha?", "/[waitkey(64)]/[close()]"};
    String[] HAK_CCC_04 = new String[]{"/[label(Professor)]", "What? What? What?", "/[waitkey(64)]/[close()]"};
    String[] HAK_CCC_05 = new String[]{"/[label(Professor)]", "...I knew it. I knew that this would happen.", "/[waitkey(1)]/[clear()]", "The most important thing in creating a giant robot is the power of friendship!!", "/[waitkey(1)]/[clear()]", "After fighting so much with Assistant Scott, I'm unworthy of creating a giant robot.", "/[waitkey(64)]/[close()]"};
    String[] HAK_SS00 = new String[]{"/[label(Professor)]", "Let's go, Assistant Scott! Begin transformation!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_SS01 = new String[]{"/[label(Professor)]", "(You've matured into a man in the short time that I hadn't seen you.)", "/[waitkey(1)]/[clear()]", "(This Academy should be safe in Assistant Scott's hands.)", "/[waitkey(64)]/[close()]"};
    String[] SCO_SS00 = new String[]{"/[label(Assistant Scott)]", "Okay, Professor!! Set transformation!", "/[waitkey(64)]/[close()]"};
    String[] SCO_SS01 = new String[]{"/[label(Assistant Scott)]", "(Professor, you're definitely the greatest.)", "/[waitkey(1)]/[clear()]", "(You really shine when you're making robots!)", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_NT00 = new String[]{"/[label(Professor)]", "What do you think?! The incarnation of my dream!!", "/[waitkey(1)]/[clear()]", "The Erde Kaiser will never lose!!", "/[waitkey(64)]/[close()]"};
    String[] HAK_6_NT01 = new String[]{"/[label(Professor)]", "Heh heh heh, my ambitions know no end!!", "/[waitkey(1)]/[clear()]", "I'm going to start developing the next giant robot right away!!", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_NT00 = new String[]{"/[label(Assistant Scott)]", "A lot happened between the professor and me, but in the end, he is a great scientist!!", "/[waitkey(1)]/[clear()]", "I'll continue to work for him!!", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_NT01 = new String[]{"/[label(Assistant Scott)]", "If I could say just one thing, I'd want him to cut back a little on his drinking though.", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_NT02 = new String[]{"/[label(Assistant Scott)]", "Oh, that's right!! I forgot to tell you about the central console! I fixed it, so you can use it to look at the completed robots!!", "/[waitkey(1)]/[clear()]", "You can also view the lyrics to the songs that the professor stayed up all night composing.", "/[waitkey(64)]/[close()]"};
    String[] SCO_6_NT02a = new String[]{"/[label(Assistant Scott)]", "Oh, that's right!! I forgot to tell you about the [Professor's Ultimate Collection]!! I will have them properly serviced by the time you come again!!", "/[waitkey(1)]/[clear()]", "And one more thing! I made it so you can use the central console to look at the robot parts that have been made so far!!", "/[waitkey(64)]/[close()]"};
    String[] AFM_00 = new String[]{"/[label()]", "Wha?! What? Oh, it's you guys. Last time was...well, you know. You saw me at a bad time!", "/[waitkey(64)]/[close()]"};
    String[] AFM_01 = new String[]{"/[label()]", "It's not really to pay you back, but I heard that this factory is helping you out with whatever you need, so I decided I'd help too. That's why I'm here!!", "/[waitkey(64)]/[close()]"};
    String[] AFK_00 = new String[]{"/[label(King)]", "I heard that this place is messing around with interesting machines, so I dropped by...", "/[waitkey(1)]/[clear()]", "It sure surprised me!!", "/[waitkey(1)]/[clear()]", "If you have the skills to maintain these machines, you could build whatever kind of machine you wanted!!", "/[waitkey(64)]/[close()]"};
    String[] AFK_01 = new String[]{"/[label(King)]", "Hey! I'm going to stay here for a while and steal the geezer's tuning techniques!!", "/[waitkey(64)]/[close()]"};
    String[] AFG_00 = new String[]{"/[label(Callahan)]", "The junior gang members asked me if I wanted to work security on the Durandal, but...I feel that it suits me better to stay here and tinker with machines.", "/[waitkey(64)]/[close()]"};
    String[] AFG_01 = new String[]{"/[label(Callahan)]", "Are the Jr. gang members doing okay?", "/[waitkey(1)]/[clear()]", "It brings back memories of when I'd blast down the highways of the Foundation on my machine...", "/[waitkey(64)]/[close()]"};
    String[] AFP_00 = new String[]{"I overheard Assistant Scott's devotion to the giant robot.", "/[waitkey(64)]/[close()]"};
    String[] AFP_01 = new String[]{"And it made me want to help out Assistant Scott.", "/[waitkey(64)]/[close()]"};

    ST2110() {
    }

    void EV_Camera0100() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.593f, 4.431f, 1.437f);
        this.camEV.setRotate(-6.769f, -67.679f, 0.0f);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera01000() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(14.562f, 9.423f, 5.426f);
        this.camEV.setRotate(-40.009f, 66.499f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01001() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(16.066f, 2.863f, 1.537f);
        this.camEV.setRotate(-31.468f, 34.555f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01002() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(12.576f, 1.135f, 0.361f);
        this.camEV.setRotate(2.25f, -47.799f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01003() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(15.843f, 1.135f, 0.87f);
        this.camEV.setRotate(2.25f, 37.24f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01004() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(14.0f, 6.0f, 7.929f);
        this.camEV.setRotate(-28.529f, 0.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera01005() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(16.429f, 3.055f, 0.82f);
        this.camEV.setRotate(-34.729f, 48.419f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera0101() {
        float[] fArray = new float[]{1.0f, 0.593f, 4.431f, 1.437f, 60.0f, 0.593f, 2.095f, 1.437f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -6.769f;
        fArray2[2] = -67.679f;
        fArray2[4] = 60.0f;
        fArray2[5] = -6.769f;
        fArray2[6] = -67.679f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 1, 120);
        this.camEV.rotateSPL(fArray3, 1, 0, 120);
        this.camEV.setFov(30.0f);
        this.camEV.change();
    }

    void EV_Camera0102() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(13.064f, 2.191f, 0.458f);
        this.camEV.setRotate(-23.069f, -40.859f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02000() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.942f, 2.351f, -1.117f);
        this.camEV.setRotate(-22.029f, -42.059f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02001() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.851f, 3.047f, -5.474f);
        this.camEV.setRotate(-30.036f, 127.896f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02002() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.395f, 1.999f, -2.59f);
        this.camEV.setRotate(-28.529f, 38.279f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera02003() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.376f, 6.0f, 6.589f);
        this.camEV.setRotate(-28.52f, 0.0f, 0.0f);
        this.camEV.setFov(42.5f);
        this.camEV.change();
    }

    void EV_Camera03000() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.624f, 3.023f, -1.174f);
        this.camEV.setRotate(-29.889f, 60.619f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03001() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-3.006f, 2.287f, -5.937f);
        this.camEV.setRotate(-23.103f, 233.592f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03002() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.193f, 2.095f, -0.882f);
        this.camEV.setRotate(-23.409f, -15.859f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03003() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.0f, 12.944f, -3.069f);
        this.camEV.setRotate(-90.0f, -45.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03004() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-10.105f, 5.871f, 2.352f);
        this.camEV.setRotate(-21.909f, -59.559f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03005() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.508f, 2.607f, -4.962f);
        this.camEV.setRotate(-28.749f, 144.219f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera03006() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.099f, 2.159f, -1.235f);
        this.camEV.setRotate(-25.189f, -13.959f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04000() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(10.855f, 3.823f, 2.048f);
        this.camEV.setRotate(-37.089f, 56.319f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04001() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.213f, 1.487f, 1.441f);
        this.camEV.setRotate(-6.329f, -43.839f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04002() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.619f, 1.487f, 1.742f);
        this.camEV.setRotate(-2.849f, 22.219f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04003() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.41f, 2.767f, -4.8f);
        this.camEV.setRotate(-23.429f, 249.895f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera04004() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.618f, 2.063f, 1.097f);
        this.camEV.setRotate(-21.689f, 50.899f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera05000() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.958f, 1.84f, -0.52f);
        this.camEV.setRotate(-16.109f, -62.299f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera05001() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(9.016f, 2.351f, -0.886f);
        this.camEV.setRotate(-22.569f, 78.759f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06100() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.47f, 0.495f, -0.215f);
        this.camEV.setRotate(14.129f, -35.959f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06101() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(6.66f, 1.871f, -2.416f);
        this.camEV.setRotate(-26.829f, -104.119f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06102() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.697f, 2.063f, -0.757f);
        this.camEV.setRotate(-26.929f, 54.659f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06103() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.843f, 3.911f, 2.049f);
        this.camEV.setRotate(-31.996f, 331.595f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06104() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(4.575f, 9.103f, 4.093f);
        this.camEV.setRotate(-53.229f, -20.999f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_Camera06105() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(5.53f, 2.671f, 0.369f);
        this.camEV.setRotate(-29.769f, -27.239f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraEL01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(7.188f, 5.615f, -18.165f);
        this.camEV.setRotate(-31.869f, 32.899f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraEL02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.256f, 4.975f, -20.097f);
        this.camEV.setRotate(-21.689f, -31.419f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraEL03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-5.066f, -4.88f, -15.486f);
        this.camEV.setRotate(9.61f, -37.24f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraEL04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.173f, -0.24f, -16.596f);
        this.camEV.setRotate(30.01f, 32.999f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraEL05() {
        float[] fArray = new float[]{1.0f, -1.323f, 4.495f, -19.694f, 120.0f, -4.095f, 6.479f, -15.713f};
        float[] fArray2 = new float[8];
        fArray2[0] = 1.0f;
        fArray2[1] = -30.069f;
        fArray2[2] = -30.62f;
        fArray2[4] = 120.0f;
        fArray2[5] = -30.069f;
        fArray2[6] = -30.62f;
        float[] fArray3 = fArray2;
        this.camEV = Camera.create(1);
        this.camEV.transSPL(fArray, 1, 3, 120);
        this.camEV.rotateSPL(fArray3, 1, 3, 120);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraE_01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.537f, 9.91f, 13.504f);
        this.camEV.setRotate(-31.123f, -22.479f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraE_02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.842f, 4.999f, 6.426f);
        this.camEV.setRotate(-26.365f, -29.819f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraE_03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.005f, -1.559f, 11.003f);
        this.camEV.setRotate(3.714f, 15.199f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraG_01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-4.245f, 3.847f, 5.51f);
        this.camEV.setRotate(-29.099f, -44.039f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraG_02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.783f, 2.631f, 6.927f);
        this.camEV.setRotate(-11.439f, 46.959f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraG_03() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-2.676f, 0.679f, 4.906f);
        this.camEV.setRotate(-36.999f, -30.9f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraG_04() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-6.59f, -1.848f, 7.96f);
        this.camEV.setRotate(-5.299f, -37.719f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraHS0() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.538f, 2.319f, 0.567f);
        this.camEV.setRotate(-22.669f, -45.459f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraHS1() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(8.98f, 2.351f, -1.46f);
        this.camEV.setRotate(-12.749f, 88.459f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraO_00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.724f, -3.223f, 8.476f);
        this.camEV.setRotate(19.396f, 44.219f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_CameraRobo01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.0f, 11.0f, 8.815f);
        this.camEV.setRotate(-16.856f, 0.0f, 0.0f);
        this.camEV.setFov(35.0f);
        this.camEV.change();
    }

    void EV_CameraSS0() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.326f, 2.543f, -4.514f);
        this.camEV.setRotate(-17.989f, 103.459f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraSS1() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-8.307f, 2.799f, -1.325f);
        this.camEV.setRotate(-19.609f, -82.76f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraS_00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(2.836f, 2.447f, 4.936f);
        this.camEV.setRotate(-18.021f, 39.159f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraS_01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.246f, 5.159f, 5.392f);
        this.camEV.setRotate(-59.287f, -34.739f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraT00() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-0.032f, 5.991f, -7.019f);
        this.camEV.setRotate(-46.956f, -139.839f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraT01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(1.79f, 4.743f, -5.314f);
        this.camEV.setRotate(-37.636f, -244.337f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void EV_CameraT02() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(0.0f, 4.0f, 5.25f);
        this.camEV.setRotate(-5.5f, -2.0f, 0.0f);
        this.camEV.setFov(40.0f);
        this.camEV.change();
    }

    void Fifth() {
        this.cam0.setMode(-1);
        this.player.setShadow(0, 0);
        this.EV_Camera05000();
        Runtime.disable(524288);
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.look_char(this.Hak);
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(1, 0);
        this.Hak.kickEnepc(9, 100);
        this.nwin(this.Sub_title_05a);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_5_00);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_5_00);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_5_00);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_5_00);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_5_00);
        } else {
            this.nwin(this.JUN_5_00);
        }
        Runtime.disable(65536);
        this.nwin(this.HAK_5_01);
        this.Hak.kickEnepc(1, 0);
        Runtime.enable(65536);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_5_01);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_5_01);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_5_01);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_5_01);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_5_01);
        } else {
            this.nwin(this.JUN_5_01);
        }
        Runtime.disable(65536);
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_5_02);
        this.EV_Camera05001();
        Runtime.enable(65536);
        this.player.mtn(9, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_5_03);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_5_03);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_5_03);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_5_03);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_5_03);
        } else {
            this.nwin(this.JUN_5_03);
        }
        Runtime.disable(65536);
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 27);
        this.nwin(this.HAK_5_03);
        System.waitFor(this.win);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_5_04);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_5_04);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_5_04);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_5_04);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_5_04);
        } else {
            this.nwin(this.JUN_5_04);
        }
        Runtime.setPlayerControl(true);
        this.cam0.setMode(0);
        this.player.setShadow(4, 16);
    }

    void Final() {
        this.player.setShadow(0, 0);
        Stage.setVisible(5, false);
        this.Sco.kickEnepc(9, -1);
        this.player.look_char(this.Hak);
        Runtime.setPlayerControl(false);
        this.nwin(this.Sub_title_05b);
        this.cam0.setMode(-1);
        this.EV_Camera06100();
        System.println("イベントカメラ０【EV_Camera06100】");
        Runtime.enable(65536);
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_6_000);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_6_000);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_6_000);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_6_000);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_6_000);
        } else {
            this.nwin(this.JUN_6_000);
        }
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera06101();
        System.println("イベントカメラ１【EV_Camera06101】");
        this.nwin(this.SCO_6_000);
        this.EV_Camera06102();
        System.println("イベントカメラ２【EV_Camera06102】");
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(0, 8);
        System.sleep(30);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_6_000);
        this.Hak.kickEnepc(4, 1);
        this.EV_Camera06103();
        System.println("イベントカメラ３【EV_Camera06103】");
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_6_001);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_6_001);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_6_001);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_6_001);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_6_001);
        } else {
            this.nwin(this.JUN_6_001);
        }
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera06104();
        System.println("イベントカメラ４【EV_Camera06104】");
        System.waitFor(this.win);
        this.EV_Camera06102();
        System.println("イベントカメラ２【EV_Camera06102】");
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(0, 7);
        System.sleep(30);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_6_001);
        this.EV_Camera06103();
        System.println("イベントカメラ３【EV_Camera06103】");
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_6_002);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_6_002);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_6_002);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_6_002);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_6_002);
        } else {
            this.nwin(this.JUN_6_002);
        }
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera06101();
        System.println("イベントカメラ１【EV_Camera06101】");
        this.Sco.kickEnepc(4, 1);
        this.Sco.kickEnepc(0, 7);
        System.sleep(30);
        this.Sco.kickEnepc(1, 9);
        this.nwin(this.SCO_6_002);
        this.Sco.kickEnepc(4, 0);
        this.EV_Camera06105();
        System.println("イベントカメラ１【EV_Camera06105】");
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(9, 100);
        System.sleep(30);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_6_002);
        this.Hak.kickEnepc(4, 0);
        this.player.mtn(11, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_6_003);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_6_003);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_6_003);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_6_003);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_6_003);
        } else {
            this.nwin(this.JUN_6_003);
        }
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(0, 7);
        System.sleep(30);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_6_003);
        this.Hak.kickEnepc(9, -1);
        this.Hak.kickEnepc(4, 0);
        Runtime.setPlayerControl(true);
        Runtime.disable(65536);
        this.cam0.setMode(0);
        this.player.setShadow(4, 16);
    }

    void Final_init(int n) {
    }

    void First() {
        this.player.setShadow(0, 0);
        Runtime.disable(524288);
        Runtime.setPlayerControl(false);
        this.cam0.setMode(-1);
        this.EV_Camera0100();
        this.nwin(this.Sub_title_01);
        this.EV_Camera0101();
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_00);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_00);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_00);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_00);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_00);
        } else {
            this.nwin(this.JR_1_00);
        }
        this.Hak.kickEnepc(4, 3);
        this.Hak.kickEnepc(1, 3);
        this.Hak.move(90, 13.5f, -1.0f, true);
        System.sleep(95);
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(1, 27);
        this.EV_Camera0102();
        this.nwin(this.BAKA_1_00);
        Runtime.enable(65536);
        this.player.mtn(10, 9, 1.0f, true);
        System.sleep(30);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_01);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_01);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_01);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_01);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_01);
        } else {
            this.nwin(this.JR_1_01);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 7);
        System.sleep(30);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_01);
        this.Hak.kickEnepc(1, 27);
        this.EV_Camera01000();
        Runtime.enable(65536);
        this.player.mtn(11, 9, 1.0f, true);
        System.sleep(30);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_02);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_02);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_02);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_02);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_02);
        } else {
            this.nwin(this.JR_1_02);
        }
        System.sleep(30);
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera01001();
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_02);
        this.Hak.kickEnepc(1, 27);
        this.EV_Camera01002();
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_03);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_03);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_03);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_03);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_03);
        } else {
            this.nwin(this.JR_1_03);
        }
        System.waitFor(this.win);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(11, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_03);
        System.waitFor(this.win);
        this.Hak.kickEnepc(1, 29);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(9, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_04);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_04);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_04);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_04);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_04);
        } else {
            this.nwin(this.JR_1_04);
        }
        System.waitFor(this.win);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(1, 29);
        this.nwin(this.BAKA_1_04);
        this.Hak.kickEnepc(0, 7);
        System.sleep(30);
        this.Hak.kickEnepc(1, 29);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_05);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_05);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_05);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_05);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_05);
        } else {
            this.nwin(this.JR_1_05);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_05);
        this.Hak.kickEnepc(1, 29);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_06);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_06);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_06);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_06);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_06);
        } else {
            this.nwin(this.JR_1_06);
        }
        System.waitFor(this.win);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera01003();
        this.Hak.kickEnepc(1, 27);
        this.nwin(this.BAKA_1_06);
        this.cam0.setMode(0);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(9, 9, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_07);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_07);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_07);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_07);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_07);
        } else {
            this.nwin(this.JR_1_07);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_07);
        this.Hak.kickEnepc(1, 0);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(9, 9, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_08);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_08);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_08);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_08);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_08);
        } else {
            this.nwin(this.JR_1_08);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_08);
        this.Hak.kickEnepc(1, 29);
        this.EV_Camera01002();
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_09);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_09);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_09);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_09);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_09);
        } else {
            this.nwin(this.JR_1_09);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera01005();
        this.nwin(this.BAKA_1_09);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_10);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_10);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_10);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_10);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_10);
        } else {
            this.nwin(this.JR_1_10);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.nwin(this.BAKA_1_10);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(8, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_11);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_11);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_11);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_11);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_11);
        } else {
            this.nwin(this.JR_1_11);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_11);
        this.Hak.kickEnepc(1, 29);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(8, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_1_12);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_1_12);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_1_12);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_1_12);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_1_12);
        } else {
            this.nwin(this.JR_1_12);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.BAKA_1_12);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
        this.cam0.setMode(0);
        this.Hak.kickEnepc(4, 1);
        System.sleep(1);
        this.Hak.kickEnepc(4, 0);
        this.Hak.kickEnepc(7, 0);
        Runtime.enable(524288);
        this.player.setShadow(4, 16);
    }

    void Forth() {
        this.cam0.setMode(-1);
        this.player.setShadow(0, 0);
        this.EV_Camera04000();
        Runtime.disable(524288);
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(1, 28);
        this.nwin(this.Sub_title_04);
        this.Sco.kickEnepc(4, 1);
        this.Sco.kickEnepc(1, 9);
        this.nwin(this.SCO_4_01);
        this.Sco.kickEnepc(4, 0);
        this.player.mtn(4, 9, 1.0f, true);
        System.sleep(1);
        this.player.move(90, 8.41f, -0.899f, true);
        System.sleep(91);
        this.player.look_char(this.Hak);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_4_00);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_4_00);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_4_00);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_4_00);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_4_00);
        } else {
            this.nwin(this.JUN_4_00);
        }
        Runtime.disable(65536);
        this.EV_Camera04001();
        this.Hak.kickEnepc(9, 100);
        this.nwin(this.HAK_4_01);
        this.Sco.kickEnepc(4, 1);
        this.Sco.kickEnepc(1, 9);
        this.nwin(this.SCO_4_02);
        this.Sco.kickEnepc(1, 27);
        this.player.look_char(this.Sco);
        Runtime.enable(65536);
        this.player.mtn(9, 1, 1.0f, true);
        System.sleep(35);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_4_01);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_4_01);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_4_01);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_4_01);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_4_01);
        } else {
            this.nwin(this.JUN_4_01);
        }
        Runtime.disable(65536);
        this.EV_Camera04002();
        this.Hak.kickEnepc(9, 12);
        this.nwin(this.HAK_4_02);
        this.EV_Camera04003();
        this.Sco.kickEnepc(1, 9);
        this.nwin(this.SCO_4_03);
        this.Sco.kickEnepc(4, 0);
        this.player.look_char(this.Hak);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_4_02);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_4_02);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_4_02);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_4_02);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_4_02);
        } else {
            this.nwin(this.JUN_4_02);
        }
        this.EV_Camera04004();
        this.Hak.kickEnepc(9, 100);
        this.Hak.kickEnepc(1, 27);
        this.nwin(this.HAK_4_03);
        this.Hak.kickEnepc(9, 12);
        this.Sco.kickEnepc(4, 1);
        this.Sco.kickEnepc(1, 9);
        this.nwin(this.SCO_4_04);
        this.Sco.kickEnepc(1, 27);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_4_03);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_4_03);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_4_03);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_4_03);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_4_03);
        } else {
            this.nwin(this.JUN_4_03);
        }
        this.EV_Camera04002();
        this.Hak.kickEnepc(1, 0);
        this.nwin(this.HAK_4_04);
        this.player.look_char(this.Sco);
        this.nwin(this.SCO_4_05);
        this.Sco.kickEnepc(4, 0);
        System.sleep(1);
        this.Sco.kickEnepc(4, 3);
        this.Sco.kickEnepc(1, 3);
        System.sleep(1);
        this.Sco.move(90, 14.856f, 0.056f, true);
        System.sleep(91);
        this.Sco.kickEnepc(4, 0);
        System.sleep(1);
        this.Sco.kickEnepc(4, 2);
        this.Sco.setTranslate(100.0f, 100.0f, 100.0f);
        this.Sco.setVisible(false);
        this.player.look_char(this.Hak);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_4_04);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_4_04);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_4_04);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_4_04);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_4_04);
        } else {
            this.nwin(this.JUN_4_04);
        }
        this.Hak.kickEnepc(9, 100);
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_4_05);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_4_05);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_4_05);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_4_05);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_4_05);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_4_05);
        } else {
            this.nwin(this.JUN_4_05);
        }
        Runtime.setPlayerControl(true);
        this.cam0.setMode(0);
        this.player.setShadow(4, 16);
    }

    public void KickEvent(int n, int n2) {
        if (n2 == 0) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3305, 1) != 1) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Con_F_00, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    if (Runtime.getFlags(3398, 1) == 0) {
                        Runtime.setPlayerControl(false);
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.Con_F_01, 0);
                        System.waitFor(this.win);
                        Runtime.setPlayerControl(true);
                        return;
                    }
                    if (Runtime.getFlags(3398, 1) != 1) return;
                    Runtime.setPlayerControl(false);
                    this.cam0.setMode(-1);
                    this.EV_CameraRobo01();
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    if (this.Miru == 0) {
                        this.win.print(this.Con_TORO_00, 0);
                    } else if (this.Miru == 1) {
                        this.win.print(this.Con_DOMI_00, 0);
                    } else if (this.Miru == 2) {
                        this.win.print(this.Con_SERA_00, 0);
                    } else if (this.Miru == 3) {
                        this.win.print(this.Con_KERU_00, 0);
                    } else {
                        this.win.print(this.Con_ELDK_00, 0);
                    }
                    System.waitFor(this.win);
                    this.menu = Menu.create();
                    this.menu.addItem("Yes\nNo");
                    System.waitFor(this.menu);
                    this.selected = this.menu.getSelected();
                    switch (this.selected) {
                        case 0: {
                            if (this.Limit == 0) {
                                this.Dai_1.start(1, "Mirumiru");
                            } else {
                                this.Dai_1.start(1, "Rumirumi");
                            }
                            System.sleep(90);
                            Runtime.setPlayerControl(true);
                            this.cam0.setMode(0);
                            return;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    this.cam0.setMode(0);
                    return;
                }
            }
            return;
        }
        if (n2 == 1) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3398, 1) != 1) return;
                    System.println("2222222222");
                    return;
                }
            }
            return;
        }
        if (n2 == 2) {
            switch (n) {
                case 100: {
                    if (Runtime.getFlags(3398, 1) != 1) return;
                    System.println("33333");
                    return;
                }
            }
            return;
        }
        if (n2 == 3) {
            switch (n) {
                case 100: {
                    Runtime.getFlags(3398, 1);
                    return;
                }
            }
            return;
        }
        if (n2 == 4) {
            return;
        }
        if (n2 == 5) {
            return;
        }
        if (n2 == 6) {
            switch (n) {
                case 100: {
                    Runtime.getFlags(3398, 1);
                    return;
                }
            }
            return;
        }
        if (n2 != 7) return;
        switch (n) {
            case 100: {
                if (this.ohanasi_sinkou == 2) {
                    if (Runtime.getFlags(3399, 1) != 0) return;
                    Runtime.setPlayerControl(false);
                    Runtime.enable(65536);
                    this.player.mtn(28, 1, 1.0f, true);
                    this.Hak.kickEnepc(4, 1);
                    this.Hak.kickEnepc(1, 9);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    if (this.niwa == 1) {
                        this.win.print(this.HAK_SOUJI_07, 0);
                    } else {
                        this.win.print(this.HAK_SOUJI_06, 0);
                    }
                    System.waitFor(this.win);
                    Runtime.disable(65536);
                    System.sleep(1);
                    Runtime.enable(65536);
                    this.player.mtn(2, 9, 1.0f, true);
                    System.sleep(1);
                    this.player.move(60, 12.0f, 0.0f, true);
                    System.sleep(65);
                    this.Hak.kickEnepc(4, 0);
                    Runtime.disable(65536);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.getFlags(3400, 4) != 5) return;
                if (Runtime.getFlags(3155, 1) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.HAK_CCC_00, 0);
                System.waitFor(this.win);
                this.fade1.call(0);
                System.sleep(60);
                this.cam0.setMode(-1);
                this.EV_CameraT02();
                Runtime.enable(65536);
                this.player.setTranslate(1.273f, 0.0f, -8.27f);
                this.player.rotY(1, 190.0f, true);
                this.Hak.kickEnepc(4, 1);
                this.Hak.setTranslate(3.5f, 0.0f, -2.0f);
                this.Hak.setRotate(0.0f, 180.0f, 0.0f);
                this.R3.setTranslate(0.0f, 2.0f, -100.0f);
                this.R2.setTranslate(0.0f, 2.0f, -100.0f);
                this.R5.setTranslate(0.0f, 2.0f, -100.0f);
                this.R4.setTranslate(0.0f, 2.0f, -100.0f);
                this.R1.setTranslate(0.0f, 2.0f, -100.0f);
                this.fade2.call(0);
                System.sleep(60);
                this.player.mtn(28, 9, 1.0f, true);
                this.Hak.kickEnepc(0, 9);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.HAK_CCC_01, 0);
                System.waitFor(this.win);
                this.EV_CameraT00();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.HAK_CCC_02, 0);
                System.waitFor(this.win);
                this.Hak.kickEnepc(1, 30);
                System.sleep(60);
                this.E08.disp(true);
                Sound.effectPlay(196748);
                System.sleep(60);
                Sound.effectStop(196748);
                this.E09.disp(true);
                this.E10.disp(true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.HAK_CCC_03, 0);
                System.waitFor(this.win);
                Sound.effectPlay(196748);
                System.sleep(60);
                Sound.effectStop(196748);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.HAK_CCC_04, 0);
                System.waitFor(this.win);
                Sound.effectPlay(196748);
                System.sleep(60);
                Sound.effectStop(196748);
                this.E08.disp(false);
                this.E15.disp(true);
                System.sleep(60);
                this.Hak.kickEnepc(1, 27);
                System.sleep(60);
                this.Hak.kickEnepc(1, 9);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.HAK_CCC_05, 0);
                System.waitFor(this.win);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                this.cam0.setMode(0);
                Runtime.setFlags(3155, 1, 1);
                this.Hak.kickEnepc(4, 0);
                this.Hak.kickEnepc(7, 0);
            }
        }
    }

    void Robo_Ashi() {
        Runtime.setPlayerControl(false);
        this.fade1.call(0);
        System.sleep(60);
        this.R2.setTranslate(0.0f, -100.0f, -22.0f);
        this.Annyo_r.setTranslate(-2.5f, 0.0f, -22.0f);
        this.Annyo_l.setTranslate(2.5f, 0.0f, -22.0f);
        System.println("Robo_AshiRobo_AshiRobo_Ashi");
        Runtime.enable(65536);
        this.player.setTranslate(1.273f, 0.0f, -8.27f);
        this.player.rotY(1, 190.0f, true);
        this.Hak.kickEnepc(4, 1);
        this.Hak.setTranslate(3.5f, 0.0f, -2.25f);
        this.Hak.setRotate(0.0f, 180.0f, 0.0f);
        if (this.ohanasi_sinkou == 3 || this.ohanasi_sinkou == 4) {
            this.Sco.kickEnepc(4, 1);
            this.Sco.setTranslate(-3.5f, 0.0f, -2.25f);
            this.Sco.setRotate(0.0f, 180.0f, 0.0f);
        }
        this.cam0.setMode(-1);
        this.EV_CameraT02();
        this.fade2.call(0);
        System.sleep(60);
        if (this.ohanasi_sinkou == 3) {
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT01();
            this.Sco.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tSCO_00, 0);
            System.waitFor(this.win);
            this.Sco.kickEnepc(1, 30);
            System.sleep(60);
            this.E09.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT02();
            this.Make.start(1, "make_ashi");
            System.sleep(420);
            this.E10.disp(true);
        } else {
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.E10.disp(true);
            this.EV_CameraT02();
            this.Make.start(1, "make_ashi");
            System.sleep(420);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.tHAK_02, 0);
        System.waitFor(this.win);
        Runtime.etherTecSet(3);
        this.fade1.call(0);
        System.sleep(60);
        this.Hak.kickEnepc(4, 0);
        this.Hak.kickEnepc(7, 0);
        this.E08.disp(false);
        if (this.ohanasi_sinkou == 3) {
            this.Sco.kickEnepc(4, 0);
            this.Sco.kickEnepc(7, 0);
            this.E09.disp(false);
        }
        Runtime.enable(524288);
        this.cam0.setMode(0);
        this.fade2.call(0);
        System.sleep(60);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
    }

    void Robo_Atama() {
        Runtime.setPlayerControl(false);
        this.fade1.call(0);
        System.sleep(60);
        this.R2.setTranslate(0.0f, -100.0f, -22.0f);
        this.Atama.setTranslate(0.0f, 0.0f, -22.0f);
        System.println("Robo_AtamaRobo_AtamaRobo_Atama");
        Runtime.enable(65536);
        this.player.setTranslate(1.273f, 0.0f, -8.27f);
        this.player.rotY(1, 190.0f, true);
        this.Hak.kickEnepc(4, 1);
        this.Hak.setTranslate(3.5f, 0.0f, -2.25f);
        this.Hak.setRotate(0.0f, 180.0f, 0.0f);
        if (this.ohanasi_sinkou == 3) {
            this.Sco.kickEnepc(4, 1);
            this.Sco.setTranslate(-3.5f, 0.0f, -2.25f);
            this.Sco.setRotate(0.0f, 180.0f, 0.0f);
        }
        this.cam0.setMode(-1);
        this.EV_CameraT02();
        this.fade2.call(0);
        System.sleep(60);
        if (this.ohanasi_sinkou == 3) {
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT01();
            this.Sco.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tSCO_00, 0);
            System.waitFor(this.win);
            this.Sco.kickEnepc(1, 30);
            System.sleep(60);
            this.E09.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT02();
            this.Make.start(1, "make_atama");
            System.sleep(420);
            this.E10.disp(true);
        } else {
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.E10.disp(true);
            this.EV_CameraT02();
            this.Make.start(1, "make_atama");
            System.sleep(420);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.tHAK_03, 0);
        System.waitFor(this.win);
        Runtime.etherTecSet(1);
        this.fade1.call(0);
        System.sleep(60);
        this.Hak.kickEnepc(4, 0);
        this.Hak.kickEnepc(7, 0);
        this.E08.disp(false);
        if (this.ohanasi_sinkou == 3) {
            this.Sco.kickEnepc(4, 0);
            this.Sco.kickEnepc(7, 0);
            this.E09.disp(false);
        }
        Runtime.enable(524288);
        this.cam0.setMode(0);
        this.fade2.call(0);
        System.sleep(60);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
    }

    void Robo_Final() {
        this.R3.setTranslate(0.0f, 2.0f, -22.0f);
        this.R2.setTranslate(-25.0f, 2.0f, -22.0f);
        this.R5.setTranslate(-50.0f, 2.0f, -22.0f);
        this.R4.setTranslate(-75.0f, 2.0f, -22.0f);
        this.R1.setTranslate(-100.0f, 0.0f, -22.0f);
        System.println("Robo_FinalRobo_FinalRobo_Final");
    }

    void Robo_Karada() {
        Runtime.setPlayerControl(false);
        this.fade1.call(0);
        System.sleep(60);
        this.R4.setTranslate(0.0f, -100.0f, -22.0f);
        this.Mune.setTranslate(0.0f, 0.0f, -22.0f);
        System.println("Robo_KaradaRobo_KaradaRobo_Karada");
        Runtime.enable(65536);
        this.player.setTranslate(1.273f, 0.0f, -8.27f);
        this.player.rotY(1, 190.0f, true);
        this.Hak.kickEnepc(4, 1);
        this.Hak.setTranslate(3.5f, 0.0f, -2.25f);
        this.Hak.setRotate(0.0f, 180.0f, 0.0f);
        if (this.ohanasi_sinkou == 3) {
            this.Sco.kickEnepc(4, 1);
            this.Sco.setTranslate(-3.5f, 0.0f, -2.25f);
            this.Sco.setRotate(0.0f, 180.0f, 0.0f);
        }
        this.cam0.setMode(-1);
        this.EV_CameraT02();
        this.fade2.call(0);
        System.sleep(60);
        if (this.ohanasi_sinkou == 3) {
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT01();
            this.Sco.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tSCO_00, 0);
            System.waitFor(this.win);
            this.Sco.kickEnepc(1, 30);
            System.sleep(60);
            this.E09.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT02();
            this.Make.start(1, "make_karada");
            System.sleep(420);
            this.E10.disp(true);
        } else {
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.E10.disp(true);
            this.EV_CameraT02();
            this.Make.start(1, "make_karada");
            System.sleep(420);
        }
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.tHAK_04, 0);
        System.waitFor(this.win);
        this.fade1.call(0);
        System.sleep(60);
        this.Hak.kickEnepc(4, 0);
        this.Hak.kickEnepc(7, 0);
        this.E08.disp(false);
        if (this.ohanasi_sinkou == 3) {
            this.Sco.kickEnepc(4, 0);
            this.Sco.kickEnepc(7, 0);
            this.E09.disp(false);
        }
        Runtime.enable(524288);
        this.cam0.setMode(0);
        this.fade2.call(0);
        System.sleep(60);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
    }

    void Robo_Nashi() {
        System.println("Robo_NashiRobo_NashiRobo_Nashi");
    }

    void Robo_Ude() {
        if (this.ohanasi_sinkou == 3 || this.ohanasi_sinkou == 4) {
            Runtime.setPlayerControl(false);
            this.fade1.call(0);
            System.sleep(60);
            this.R3.setTranslate(0.0f, -100.0f, -22.0f);
            this.Otete1.setTranslate(-2.5f, 0.0f, -22.0f);
            this.Otete2.setTranslate(2.5f, 0.0f, -22.0f);
            System.println("Robo_UdeRobo_UdeRobo_Ude");
            Runtime.enable(65536);
            this.player.setTranslate(1.273f, 0.0f, -8.27f);
            this.player.rotY(1, 190.0f, true);
            this.Hak.kickEnepc(4, 1);
            this.Hak.setTranslate(3.5f, 0.0f, -2.25f);
            this.Hak.setRotate(0.0f, 180.0f, 0.0f);
            this.Sco.kickEnepc(4, 1);
            this.Sco.setTranslate(-3.5f, 0.0f, -2.25f);
            this.Sco.setRotate(0.0f, 180.0f, 0.0f);
            this.cam0.setMode(-1);
            this.EV_CameraT02();
            this.fade2.call(0);
            System.sleep(60);
            this.player.mtn(28, 9, 1.0f, true);
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT01();
            this.Sco.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tSCO_00, 0);
            System.waitFor(this.win);
            this.Sco.kickEnepc(1, 30);
            System.sleep(60);
            this.E09.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.EV_CameraT02();
            this.Make.start(1, "make_ude");
            System.sleep(420);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_01, 0);
            System.waitFor(this.win);
            Runtime.etherTecSet(2);
            this.fade1.call(0);
            System.sleep(60);
            this.Hak.kickEnepc(4, 0);
            this.Hak.kickEnepc(7, 0);
            this.Sco.kickEnepc(4, 0);
            this.Sco.kickEnepc(7, 0);
            this.cam0.setMode(0);
            Runtime.enable(524288);
            this.fade2.call(0);
            System.sleep(60);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.fade1.call(0);
            System.sleep(59);
            this.cam0.setMode(-1);
            this.EV_CameraT02();
            this.R3.setTranslate(0.0f, -100.0f, -22.0f);
            this.Otete1.setTranslate(-2.5f, 0.0f, -22.0f);
            this.Otete2.setTranslate(2.5f, 0.0f, -22.0f);
            System.println("Robo_UdeRobo_UdeRobo_Ude");
            System.sleep(1);
            Runtime.enable(65536);
            this.player.setTranslate(1.273f, 0.0f, -8.27f);
            this.player.rotY(1, 190.0f, true);
            this.Hak.kickEnepc(4, 1);
            this.Hak.setTranslate(3.5f, 0.0f, -2.25f);
            this.Hak.setRotate(0.0f, 180.0f, 0.0f);
            System.sleep(1);
            this.fade2.call(0);
            System.sleep(59);
            this.Hak.kickEnepc(1, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_00_b, 0);
            System.waitFor(this.win);
            this.EV_CameraT00();
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.E10.disp(true);
            this.EV_CameraT02();
            this.Make.start(1, "make_ude");
            System.sleep(420);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.tHAK_01, 0);
            System.waitFor(this.win);
            Runtime.etherTecSet(2);
            this.fade1.call(0);
            System.sleep(60);
            this.Hak.kickEnepc(4, 0);
            this.Hak.kickEnepc(7, 0);
            this.cam0.setMode(0);
            Runtime.enable(524288);
            this.fade2.call(0);
            System.sleep(60);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }

    void Robo_Zenbu() {
        Runtime.disable(524288);
        Runtime.setPlayerControl(false);
        this.fade1.call(0);
        System.sleep(60);
        this.cam0.setMode(-1);
        this.EV_CameraT02();
        this.R1.init(8710, 0.0f, -45.0f, -22.0f, 0.0f);
        this.R1.setScale(1.0f, 1.0f, 1.0f);
        System.println("Robo_ZenbuRobo_ZenbuRobo_Zenbu");
        Runtime.enable(65536);
        this.player.setTranslate(1.273f, 0.0f, -8.27f);
        this.player.rotY(1, 190.0f, true);
        this.Hak.kickEnepc(4, 1);
        this.Sco.kickEnepc(4, 1);
        this.Hak.setTranslate(3.5f, 0.0f, -2.25f);
        this.Sco.setTranslate(-3.5f, 0.0f, -2.25f);
        this.Hak.setRotate(0.0f, 180.0f, 0.0f);
        this.Sco.setRotate(0.0f, 180.0f, 0.0f);
        this.cam0.setMode(-1);
        this.EV_CameraHS0();
        this.fade2.call(0);
        System.sleep(60);
        this.Hak.kickEnepc(9, 12);
        this.Hak.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.HAK_SS00, 0);
        System.waitFor(this.win);
        this.Hak.kickEnepc(9, -1);
        this.Hak.kickEnepc(1, 30);
        System.sleep(60);
        this.E08.disp(true);
        Sound.effectPlay(196748);
        System.sleep(60);
        this.EV_CameraSS0();
        this.Sco.kickEnepc(9, 11);
        this.Sco.kickEnepc(1, 9);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.SCO_SS00, 0);
        System.waitFor(this.win);
        this.Sco.kickEnepc(9, -1);
        this.Sco.kickEnepc(1, 30);
        System.sleep(60);
        this.E09.disp(true);
        Sound.effectPlay(196748);
        System.sleep(60);
        this.EV_CameraHS1();
        this.Hak.kickEnepc(9, 12);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.HAK_SS01, 0);
        System.waitFor(this.win);
        this.Hak.kickEnepc(9, -1);
        this.EV_CameraSS1();
        this.Sco.kickEnepc(9, 11);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.SCO_SS01, 0);
        System.waitFor(this.win);
        this.Sco.kickEnepc(9, -1);
        Sound.effectStop(196748);
        this.EV_CameraT02();
        this.Make.start(1, "make_kansei");
        System.sleep(120);
        this.EV_CameraEL01();
        System.sleep(30);
        this.EV_CameraEL02();
        System.sleep(30);
        this.EV_CameraEL03();
        System.sleep(30);
        this.EV_CameraEL04();
        System.sleep(30);
        this.EV_CameraEL05();
        System.sleep(120);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.tHAK_05, 0);
        System.waitFor(this.win);
        Runtime.etherTecSet(4);
        this.fade1.call(0);
        System.sleep(60);
        this.Hak.kickEnepc(4, 0);
        this.Hak.kickEnepc(7, 0);
        this.Sco.kickEnepc(4, 0);
        this.Sco.kickEnepc(7, 0);
        this.cam0.setMode(0);
        Runtime.enable(524288);
        this.fade1.call(0);
        System.sleep(60);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
    }

    void Second() {
        this.player.setShadow(0, 0);
        this.cam0.setMode(-1);
        this.EV_Camera02000();
        System.println("イベントカメラ０【EV_Camera02000】");
        this.player.setTranslate(-0.377f, 0.0f, -2.24f);
        Runtime.disable(524288);
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.rotY(1, 180.0f, true);
        System.sleep(2);
        this.nwin(this.Sub_title_02);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_2_00);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_2_00);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_2_00);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_2_00);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_2_00);
        } else {
            this.nwin(this.JUN_2_00);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(11, 9, 1.0f, true);
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_2_08);
        this.Hak.kickEnepc(1, 10);
        this.EV_Camera02001();
        System.println("イベントカメラ１【EV_Camera02001】");
        this.player.mtn(28, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_2_01);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_2_01);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_2_01);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_2_01);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_2_01);
        } else {
            this.nwin(this.JUN_2_01);
        }
        this.EV_Camera02002();
        System.println("イベントカメラ２【EV_Camera02002】");
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 27);
        this.nwin(this.HAK_2_09);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.EV_Camera02001();
        System.println("イベントカメラ１【EV_Camera02001】");
        this.player.mtn(9, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(1, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_2_02);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_2_02);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_2_02);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_2_02);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_2_02);
        } else {
            this.nwin(this.JUN_2_02);
        }
        this.EV_Camera02000();
        System.println("イベントカメラ０【EV_Camera02000】");
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 27);
        this.nwin(this.HAK_2_10);
        this.Hak.kickEnepc(1, 9);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_2_03);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_2_03);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_2_03);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_2_03);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_2_03);
        } else {
            this.nwin(this.JUN_2_03);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera02003();
        System.println("イベントカメラ３【EV_Camera02003】");
        this.nwin(this.HAK_2_11);
        this.EV_Camera02001();
        System.println("イベントカメラ１【EV_Camera02001】");
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(10, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_2_04);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_2_04);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_2_04);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_2_04);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_2_04);
        } else {
            this.nwin(this.JUN_2_04);
        }
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.player.mtn(1, 9, 1.0f, true);
        this.EV_Camera02000();
        System.println("イベントカメラ０【EV_Camera02000】");
        this.nwin(this.HAK_2_12);
        Runtime.disable(65536);
        System.sleep(1);
        Runtime.enable(65536);
        this.EV_Camera02003();
        System.println("イベントカメラ３【EV_Camera02003】");
        this.player.mtn(8, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(1, 9, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_2_05);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_2_05);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_2_05);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_2_05);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_2_05);
        } else {
            this.nwin(this.JUN_2_05);
        }
        this.EV_Camera02000();
        System.println("イベントカメラ０【EV_Camera02000】");
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_2_13);
        this.cam0.setMode(0);
        this.Hak.kickEnepc(4, 0);
        this.Hak.kickEnepc(7, 0);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
        this.player.setShadow(4, 16);
        Runtime.enable(524288);
    }

    public void TalkHak1(Enepc enepc) {
        if (this.npc1talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Hak1_00, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Hak1_01, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak2(Enepc enepc) {
        if (Runtime.getFlags(3399, 1) == 0) {
            if (this.niwa == 0) {
                if (this.destroy < 3) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.HAK_SOUJI_00, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                } else if (this.destroy >= 3 && this.destroy < 6) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.HAK_SOUJI_01, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                } else if (this.destroy >= 6 && this.destroy < 8) {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.HAK_SOUJI_02, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                } else {
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 305);
                    this.win.print(this.HAK_SOUJI_03, 0);
                    System.waitFor(this.win);
                    Runtime.setPlayerControl(true);
                }
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.HAK_SOUJI_04, 0);
                System.waitFor(this.win);
                this.fade1.call(0);
                Runtime.setFlags(3399, 1, 1);
                if (this.tukuru == 0) {
                    this.Robo_Final();
                } else if (this.tukuru == 1) {
                    this.Robo_Ude();
                    Runtime.setFlags(3302, 1, 1);
                } else if (this.tukuru == 2) {
                    this.Robo_Ashi();
                    Runtime.setFlags(3304, 1, 1);
                } else if (this.tukuru == 3) {
                    this.Robo_Atama();
                    Runtime.setFlags(3301, 1, 1);
                } else if (this.tukuru == 4) {
                    this.Robo_Karada();
                    Runtime.setFlags(3303, 1, 1);
                } else if (this.tukuru == 5) {
                    this.Robo_Zenbu();
                    Runtime.setFlags(3305, 1, 1);
                } else {
                    this.Robo_Nashi();
                }
            }
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_SOUJI_05, 0);
            System.waitFor(this.win);
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak2a(Enepc enepc) {
        if (this.npc1talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Hak1_00, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.Hak1_01, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak3(Enepc enepc) {
        if (this.npc1talked == 0) {
            System.println("TALK_A");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_3_a0, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_3_a1, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak3a(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.HAK_SOUJI_05, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkHak4(Enepc enepc) {
        if (this.npc1talked == 0) {
            System.println("TALK_B");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_4_a0, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_4_a1, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak4a(Enepc enepc) {
        if (this.npc1talked == 0) {
            System.println("TALK_B");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_3_a0, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_3_a1, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak5(Enepc enepc) {
        System.println("TALK_C");
        if (Runtime.getFlags(3155, 1) == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_CCC_00, 0);
            System.waitFor(this.win);
            this.fade1.call(0);
            System.sleep(60);
            this.cam0.setMode(-1);
            this.EV_CameraT02();
            Runtime.enable(65536);
            this.player.setTranslate(1.273f, 0.0f, -8.27f);
            this.player.rotY(1, 190.0f, true);
            this.Hak.kickEnepc(4, 1);
            this.Hak.setTranslate(3.5f, 0.0f, -2.0f);
            this.Hak.setRotate(0.0f, 180.0f, 0.0f);
            this.R3.setTranslate(0.0f, 2.0f, -100.0f);
            this.R2.setTranslate(0.0f, 2.0f, -100.0f);
            this.R5.setTranslate(0.0f, 2.0f, -100.0f);
            this.R4.setTranslate(0.0f, 2.0f, -100.0f);
            this.R1.setTranslate(0.0f, 2.0f, -100.0f);
            this.fade2.call(0);
            System.sleep(60);
            this.player.mtn(28, 9, 1.0f, true);
            this.Hak.kickEnepc(0, 9);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_CCC_01, 0);
            System.waitFor(this.win);
            this.EV_CameraT00();
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_CCC_02, 0);
            System.waitFor(this.win);
            this.Hak.kickEnepc(1, 30);
            System.sleep(60);
            this.E08.disp(true);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.E09.disp(true);
            this.E10.disp(true);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_CCC_03, 0);
            System.waitFor(this.win);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_CCC_04, 0);
            System.waitFor(this.win);
            Sound.effectPlay(196748);
            System.sleep(60);
            Sound.effectStop(196748);
            this.E08.disp(false);
            this.E15.disp(true);
            System.sleep(60);
            this.Hak.kickEnepc(1, 27);
            System.sleep(60);
            this.Hak.kickEnepc(1, 9);
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_CCC_05, 0);
            System.waitFor(this.win);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
            this.cam0.setMode(0);
            Runtime.setFlags(3155, 1, 1);
            this.Hak.kickEnepc(4, 0);
            this.Hak.kickEnepc(7, 0);
        } else if (this.npc1talked == 0) {
            System.println("XXXXXXXXXXXXXXXXXXXX");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_5_a2, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else if (this.npc1talked == 1) {
            System.println("YYYYYYYYYYYYYYYYYYYYY");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_5_a3, 0);
            System.waitFor(this.win);
            this.npc1talked = 2;
            Runtime.setPlayerControl(true);
        } else {
            System.println("ZZZZZZZZZZZZZZZZZZZZZZZZ");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_5_a4, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak5a(Enepc enepc) {
        if (this.npc1talked == 0) {
            System.println("TALK_B");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_4_a0, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_4_a1, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak6(Enepc enepc) {
        System.println("TALK_D");
        if (this.npc1talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_6_NT00, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_6_NT01, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak6a(Enepc enepc) {
        if (this.npc1talked == 0) {
            System.println("XXXXXXXXXXXXXXXXXXXX");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_5_a2, 0);
            System.waitFor(this.win);
            this.npc1talked = 1;
            Runtime.setPlayerControl(true);
        } else if (this.npc1talked == 1) {
            System.println("YYYYYYYYYYYYYYYYYYYYY");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_5_a3, 0);
            System.waitFor(this.win);
            this.npc1talked = 2;
            Runtime.setPlayerControl(true);
        } else {
            System.println("ZZZZZZZZZZZZZZZZZZZZZZZZ");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.HAK_5_a4, 0);
            System.waitFor(this.win);
            this.npc1talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkHak7(Enepc enepc) {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.tHAK_00_b, 0);
        System.waitFor(this.win);
        Runtime.setPlayerControl(true);
    }

    public void TalkNPC3x(Enepc enepc) {
        if (this.npc3xtalked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFK_00, 0);
            System.waitFor(this.win);
            this.npc3xtalked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFK_01, 0);
            System.waitFor(this.win);
            this.npc3xtalked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC4x(Enepc enepc) {
        if (this.npc4xtalked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFG_00, 0);
            System.waitFor(this.win);
            this.npc4xtalked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFG_01, 0);
            System.waitFor(this.win);
            this.npc4xtalked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC5x(Enepc enepc) {
        if (this.npc5xtalked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFM_00, 0);
            System.waitFor(this.win);
            this.npc5xtalked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFM_01, 0);
            System.waitFor(this.win);
            this.npc5xtalked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkNPC6x(Enepc enepc) {
        if (this.npc6xtalked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFP_00, 0);
            System.waitFor(this.win);
            this.npc6xtalked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.AFP_01, 0);
            System.waitFor(this.win);
            this.npc6xtalked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkSco3(Enepc enepc) {
        if (this.npc2talked == 0) {
            System.println("TALK_A");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_3_a0, 0);
            System.waitFor(this.win);
            this.npc2talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_3_a1, 0);
            System.waitFor(this.win);
            this.npc2talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkSco4a(Enepc enepc) {
        if (this.npc2talked == 0) {
            System.println("TALK_A");
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_3_a0, 0);
            System.waitFor(this.win);
            this.npc2talked = 1;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_3_a1, 0);
            System.waitFor(this.win);
            this.npc2talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkSco6(Enepc enepc) {
        System.println("TALK_D");
        if (this.npc2talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_6_NT00, 0);
            System.waitFor(this.win);
            this.npc2talked = 1;
            Runtime.setPlayerControl(true);
        } else if (this.npc2talked == 1) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_6_NT01, 0);
            System.waitFor(this.win);
            this.npc2talked = 2;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_6_NT02a, 0);
            System.waitFor(this.win);
            this.npc2talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    public void TalkSco7(Enepc enepc) {
        if (this.npc2talked == 0) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_6_NT00, 0);
            System.waitFor(this.win);
            this.npc2talked = 1;
            Runtime.setPlayerControl(true);
        } else if (this.npc2talked == 1) {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_6_NT01, 0);
            System.waitFor(this.win);
            this.npc2talked = 2;
            Runtime.setPlayerControl(true);
        } else {
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.SCO_6_NT02, 0);
            System.waitFor(this.win);
            this.npc2talked = 0;
            Runtime.setPlayerControl(true);
        }
    }

    void Third() {
        this.cam0.setMode(-1);
        this.player.setShadow(0, 0);
        this.EV_Camera03000();
        Runtime.disable(524288);
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.rotY(1, 20.0f, true);
        System.sleep(2);
        this.nwin(this.Sub_title_03);
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_3_00);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_3_00);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_3_00);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_3_00);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_3_00);
        } else {
            this.nwin(this.JUN_3_00);
        }
        System.waitFor(this.win);
        Runtime.disable(65536);
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(1, 9);
        this.nwin(this.HAK_3_03);
        this.Hak.kickEnepc(4, 0);
        Runtime.enable(65536);
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_3_01);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_3_01);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_3_01);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_3_01);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_3_01);
        } else {
            this.nwin(this.JUN_3_01);
        }
        Runtime.disable(65536);
        this.EV_Camera03001();
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(1, 27);
        this.nwin(this.HAK_3_04);
        this.Hak.kickEnepc(4, 0);
        this.EV_Camera03002();
        Runtime.enable(65536);
        this.player.mtn(9, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_3_02);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_3_02);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_3_02);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_3_02);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_3_02);
        } else {
            this.nwin(this.JUN_3_02);
        }
        Runtime.disable(65536);
        this.EV_Camera03003();
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(0, 8);
        System.sleep(29);
        this.Hak.kickEnepc(1, 27);
        this.nwin(this.HAK_3_05);
        this.Hak.kickEnepc(4, 0);
        Runtime.enable(65536);
        this.player.mtn(8, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 1, 1.0f, true);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_3_03);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_3_03);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_3_03);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_3_03);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_3_03);
        } else {
            this.nwin(this.JUN_3_03);
        }
        Runtime.disable(65536);
        this.EV_Camera03004();
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(0, 7);
        System.sleep(29);
        this.Hak.kickEnepc(9, 12);
        this.Hak.kickEnepc(1, 9);
        Runtime.setPlayerControl(false);
        this.nwin(this.HAK_3_06);
        this.Hak.kickEnepc(4, 0);
        this.Sco.kickEnepc(4, 3);
        this.Sco.kickEnepc(1, 3);
        this.Sco.move(80, -1.024f, -1.847f, true);
        System.sleep(81);
        this.Sco.rotY(20, 135.0f, true);
        System.sleep(25);
        this.Sco.kickEnepc(4, 0);
        System.sleep(1);
        this.Sco.kickEnepc(4, 1);
        this.Sco.kickEnepc(0, 7);
        System.sleep(29);
        this.Sco.kickEnepc(1, 9);
        Runtime.setPlayerControl(false);
        this.nwin(this.SCO_3_01);
        this.Sco.kickEnepc(1, 0);
        this.EV_Camera03005();
        this.Hak.kickEnepc(4, 1);
        this.Hak.kickEnepc(1, 9);
        Runtime.setPlayerControl(false);
        this.nwin(this.HAK_3_07);
        this.Hak.kickEnepc(9, 100);
        this.Hak.kickEnepc(1, 0);
        this.Sco.kickEnepc(9, 100);
        this.Sco.kickEnepc(0, 9);
        Runtime.setPlayerControl(false);
        this.nwin(this.SCO_3_02);
        this.Sco.kickEnepc(1, 0);
        Runtime.enable(65536);
        this.player.mtn(8, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 1, 1.0f, true);
        this.player.look_char(this.Sco);
        Runtime.setPlayerControl(false);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_3_04);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_3_04);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_3_04);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_3_04);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_3_04);
        } else {
            this.nwin(this.JUN_3_04);
        }
        Runtime.disable(65536);
        this.Sco.kickEnepc(0, 9);
        Runtime.setPlayerControl(false);
        this.nwin(this.SCO_3_03);
        this.Sco.kickEnepc(1, 0);
        this.Sco.kickEnepc(9, 11);
        this.Hak.kickEnepc(1, 27);
        Runtime.setPlayerControl(false);
        this.nwin(this.HAK_3_08);
        this.Hak.kickEnepc(4, 0);
        this.EV_Camera03006();
        Runtime.enable(65536);
        this.player.look_char(this.Hak);
        this.player.mtn(8, 1, 1.0f, true);
        System.sleep(29);
        this.player.mtn(10, 1, 1.0f, true);
        Runtime.setPlayerControl(false);
        if (Runtime.getLeader() == 1) {
            this.nwin(this.SHI_3_05);
        } else if (Runtime.getLeader() == 6) {
            this.nwin(this.ZIG_3_05);
        } else if (Runtime.getLeader() == 3) {
            this.nwin(this.CHA_3_05);
        } else if (Runtime.getLeader() == 2) {
            this.nwin(this.KOS_3_05);
        } else if (Runtime.getLeader() == 4) {
            this.nwin(this.MOM_3_05);
        } else {
            this.nwin(this.JUN_3_05);
        }
        Runtime.disable(65536);
        Runtime.setPlayerControl(false);
        this.nwin(this.HAK_3_09);
        this.cam0.setMode(0);
        this.player.setShadow(4, 16);
    }

    void broken(int n) {
        switch (n) {
            case 1: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 2: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 3: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 4: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 5: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 6: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 7: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 8: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 9: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
            case 10: {
                if (this.destroy < 9) {
                    System.println("+++++++++++++++++++++++++++++++++++++++++++++");
                    ++this.destroy;
                    break;
                }
                System.println("############################################");
                this.niwa = 1;
                break;
            }
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.cam0.setMode(-1);
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                System.println("Basement Parts Shop【MC_KUK12】 2");
                if (Runtime.getFlags(3305, 1) == 1) {
                    Runtime.setFlags(3398, 1, 1);
                }
                Runtime.jumpCF(2120, 2);
                break;
            }
        }
    }

    void init() {
        int n;
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(0, 0.45f, 0.45f, 0.45f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(2, 0.0f, 1.0f, 3.0f);
        this.light.setColor(3, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(3, 0.0f, -1.0f, -3.0f);
        Stage.setVisible(-1, true);
        this.teiten1 = new Uwamono(28690, -13.0f, 0.0f, -6.0f, 0.0f);
        this.teiten1.SetBgm(196613);
        this.teiten2 = new Uwamono(28690, -5.25f, 0.0f, -6.5f, 0.0f);
        this.teiten2.SetBgm(196613);
        this.teiten3 = new Uwamono(28690, 5.25f, 0.0f, -6.5f, 0.0f);
        this.teiten3.SetBgm(196613);
        this.teiten4 = new Uwamono(28690, -15.5f, 0.0f, -0.85f, 0.0f);
        this.teiten4.SetBgm(196613);
        this.teiten5 = new Uwamono(28690, 3.5f, 0.0f, -3.0f, 0.0f);
        this.teiten5.SetBgm(196613);
        this.Dai_1 = new Mapunits();
        this.Dai_1.mapUnit(5);
        this.Dai_1.start(4, null);
        this.Dai_2 = new Mapunits();
        this.Dai_2.mapUnit(156);
        this.Dai_2.start(4, null);
        if (Runtime.getFlags(3305, 1) == 1) {
            Stage.setVisible(111, false);
            Stage.setVisible(116, false);
            Stage.setVisible(112, false);
            Stage.setVisible(115, false);
            Stage.setVisible(113, false);
            Stage.setVisible(114, false);
        } else {
            this.Arm_1 = new Mapunits();
            this.Arm_1.mapUnit(111);
            this.Arm_1.start(4, null);
            this.Arm_2 = new Mapunits();
            this.Arm_2.mapUnit(116);
            this.Arm_2.start(4, null);
            this.Arm_3 = new Mapunits();
            this.Arm_3.mapUnit(112);
            this.Arm_3.start(4, null);
            this.Arm_4 = new Mapunits();
            this.Arm_4.mapUnit(115);
            this.Arm_4.start(4, null);
            this.Arm_5 = new Mapunits();
            this.Arm_5.mapUnit(113);
            this.Arm_5.start(4, null);
            this.Arm_6 = new Mapunits();
            this.Arm_6.mapUnit(114);
            this.Arm_6.start(4, null);
        }
        this.OArm_r = new Mapunits();
        this.OArm_r.mapUnit(149);
        this.OArm_r.start(4, null);
        this.OArm_r.setRotate(0.0f, 0.0f, 5.0f);
        this.OArm_l = new Mapunits();
        this.OArm_l.mapUnit(150);
        this.OArm_l.start(4, null);
        this.OArm_l.setRotate(0.0f, 0.0f, -5.0f);
        this.Otu = new Mapunits();
        this.Otu.mapUnit(159);
        this.Otu.start(4, null);
        this.Drl = new Mapunits();
        this.Drl.mapUnit(160);
        this.Drl.start(4, null);
        this.Bust = new Mapunits();
        this.Bust.mapUnit(165);
        this.Bust.start(4, null);
        this.Punch = new Mapunits();
        this.Punch.mapUnit(127);
        this.Punch.start(4, null);
        this.Hasami_a = new Mapunits();
        this.Hasami_a.mapUnit(166);
        this.Hasami_a.start(4, null);
        this.Hasami_b = new Mapunits();
        this.Hasami_b.mapUnit(167);
        this.Hasami_b.start(4, null);
        this.Eye = new Mapunits();
        this.Eye.mapUnit(162);
        this.Eye.start(4, null);
        this.Eye.setRotate(20.0f, 0.0f, 0.0f);
        this.Dx_ude = new Mapunits();
        this.Dx_ude.mapUnit(151);
        this.Dx_ude.start(4, null);
        this.Dx_ago = new Mapunits();
        this.Dx_ago.mapUnit(146);
        this.Dx_ago.start(4, null);
        this.Dx_emb = new Mapunits();
        this.Dx_emb.mapUnit(163);
        this.Dx_emb.start(4, null);
        this.Kuru_r = new Mapunits();
        this.Kuru_r.mapUnit(22);
        this.Kuru_r.start(4, null);
        this.Kuru_l = new Mapunits();
        this.Kuru_l.mapUnit(97);
        this.Kuru_l.start(4, null);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade1 = new Effect(0);
        this.fade1.args[0] = -268435456;
        this.fade1.args[1] = 60;
        this.fade1.args[2] = 0;
        this.fade2 = new Effect(0);
        this.fade2.args[0] = -268435456;
        this.fade2.args[1] = 60;
        this.fade2.args[2] = 1;
        this.E01 = new Effect(1735, -2.407f, 1.098f, 3.341f, 0.0f);
        this.E01.disp(false);
        this.E01.setClip(true);
        this.E01.setRotate(0.0f, 0.0f, 0.0f);
        this.E01.noAttach(false);
        this.E04 = new Effect(1737, 5.661f, 5.269f, -0.383f, 0.0f);
        this.E04.disp(false);
        this.E04.setClip(true);
        this.E04.setRotate(0.0f, 0.0f, 0.0f);
        this.E04.noAttach(false);
        this.E05 = new Effect(1738, 0.0f, 0.1f, 0.0f, 0.0f);
        this.E05.disp(false);
        this.E05.setClip(true);
        this.E05.setRotate(0.0f, 0.0f, 0.0f);
        this.E05.noAttach(false);
        this.E06 = new Effect(1739, 6.0f, 0.0f, 3.5f, 0.0f);
        this.E06.disp(false);
        this.E06.setClip(true);
        this.E06.setRotate(0.0f, 0.0f, 0.0f);
        this.E06.noAttach(false);
        this.E07a = new Effect(1734, -6.163f, 0.066f, 1.645f, 0.0f);
        this.E07a.disp(false);
        this.E07a.setScale(0.1f, 0.1f, 0.1f);
        this.E07a.noAttach(true);
        this.E08 = new Effect(1694, 3.517f, 0.95f, -3.474f, 0.0f);
        this.E08.disp(false);
        this.E08.setClip(true);
        this.E08.setRotate(0.0f, 0.0f, 0.0f);
        this.E08.noAttach(false);
        this.E09 = new Effect(1694, -3.517f, 0.95f, -3.474f, 0.0f);
        this.E09.disp(false);
        this.E09.setClip(true);
        this.E09.setRotate(0.0f, 0.0f, 0.0f);
        this.E09.noAttach(false);
        this.E10 = new Effect(1695, 0.0f, 0.0f, 0.0f, 0.0f);
        this.E10.disp(false);
        this.E10.setClip(true);
        this.E10.setRotate(0.0f, 0.0f, 0.0f);
        this.E10.noAttach(false);
        this.E11 = new Effect(1401, 0.0f, -5.0f, -22.0f, 0.0f);
        this.E11.disp(false);
        this.E11.setClip(true);
        this.E11.noAttach(true);
        this.E11.setScale(10.0f, 10.0f, 10.0f);
        this.E12 = new Effect(1743, 1.165f, 1.87f, 3.025f, 0.0f);
        this.E12.disp(false);
        this.E12.setClip(true);
        this.E12.setRotate(0.0f, 0.0f, 0.0f);
        this.E12.setScale(0.25f, 0.25f, 0.25f);
        this.E12.noAttach(false);
        this.E13 = new Effect(1736, 1.638f, 0.847f, 3.383f, 0.0f);
        this.E13.disp(false);
        this.E13.setClip(true);
        this.E13.setRotate(0.0f, 0.0f, 0.0f);
        this.E13.noAttach(false);
        this.E14 = new Effect(1736, 0.709f, 0.847f, 3.383f, 0.0f);
        this.E14.disp(false);
        this.E14.setClip(true);
        this.E14.setRotate(0.0f, 0.0f, 0.0f);
        this.E14.noAttach(false);
        this.E15 = new Effect(1401, 3.517f, 0.95f, -3.474f, 0.0f);
        this.E15.disp(false);
        this.E15.setClip(true);
        this.E15.noAttach(true);
        if (Runtime.getFlags(3305, 1) == 1) {
            this.Kin = new NPC_NORMAL(1604, 13, 0, 0, 7, -13.161f, 0.0f, -4.093f, 0.0f);
            this.Kin.talkto("TalkNPC3x");
            this.Kin.disableDTKFlag(131082);
            this.Kin.enableDTKFlag(4);
            this.Go1 = new NPC_NORMAL(1609, 14, 0, 0, 9, -6.475f, -3.5f, 6.635f, 0.0f);
            this.Go1.talkto("TalkNPC4x");
            this.Go1.disableDTKFlag(131082);
            this.Go1.enableDTKFlag(4);
            this.Mat = new NPC_NORMAL(1599, 15, 0, 0, 11, 7.173f, -3.5f, 6.337f, 0.0f);
            this.Mat.talkto("TalkNPC5x");
            this.Mat.disableDTKFlag(131082);
            this.Mat.enableDTKFlag(4);
            this.Mat.setMotion(3, 29);
            this.Mat = new NPC_NORMAL(1606, 16, 0, 0, 33, 5.173f, -3.5f, 6.337f, 0.0f);
            this.Mat.talkto("TalkNPC6x");
            this.Mat.disableDTKFlag(131080);
            this.Mat.enableDTKFlag(4);
            this.Mat.setMotion(3, 29);
        }
        if ((n = Runtime.getEntrance()) >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        this.R3 = new Unit();
        this.R3.init(8712, 0.0f, 2.0f, -72.0f, 0.0f);
        this.R3.setRotate(45.0f, 0.0f, 0.0f);
        this.R2 = new Unit();
        this.R2.init(8714, 0.0f, 2.0f, -72.0f, 0.0f);
        this.R2.setRotate(this.R2.rx, this.R2.ry - 45.0f, this.R2.rz);
        this.R2.setScale(0.6f, 0.6f, 0.6f);
        this.R5 = new Unit();
        this.R5.init(8711, 0.0f, 2.0f, -72.0f, 0.0f);
        this.R5.setRotate(this.R5.rx + 45.0f, this.R5.ry - 45.0f, this.R5.rz);
        this.R5.setScale(2.0f, 2.0f, 2.0f);
        this.R4 = new Unit();
        this.R4.init(8713, 0.0f, 2.0f, -72.0f, 0.0f);
        this.R4.setScale(0.6f, 0.6f, 0.6f);
        this.R1 = new Unit();
        this.R1.init(8710, 0.0f, -3.0f, -72.0f, 0.0f);
        this.R1.setScale(0.5f, 0.5f, 0.5f);
        this.Otete1 = new Unit();
        this.Otete1.init(28726, -2.5f, 0.0f, -72.0f, 0.0f);
        this.Otete1.setRotate(0.0f, 45.0f, 0.0f);
        this.Otete1.setScale(5.0f, 5.0f, 5.0f);
        this.Otete2 = new Unit();
        this.Otete2.init(28726, 2.5f, 0.0f, -72.0f, 0.0f);
        this.Otete2.setRotate(0.0f, 45.0f, 0.0f);
        this.Otete2.setScale(5.0f, 5.0f, 5.0f);
        this.Annyo_r = new Unit();
        this.Annyo_r.init(28725, -2.5f, 2.0f, -72.0f, 0.0f);
        this.Annyo_r.setRotate(2.5f, 45.0f, 0.0f);
        this.Annyo_r.setScale(5.0f, 5.0f, 5.0f);
        this.Annyo_l = new Unit();
        this.Annyo_l.init(28725, 2.5f, 2.0f, -72.0f, 0.0f);
        this.Annyo_l.setRotate(-2.5f, 45.0f, 0.0f);
        this.Annyo_l.setScale(5.0f, 5.0f, 5.0f);
        this.Atama = new Unit();
        this.Atama.init(28723, 0.0f, 0.0f, -72.0f, 0.0f);
        this.Atama.setScale(5.0f, 5.0f, 5.0f);
        this.Mune = new Unit();
        this.Mune.init(28724, 0.0f, 0.0f, -72.0f, 0.0f);
        this.Mune.setScale(5.0f, 5.0f, 5.0f);
        if (Runtime.getFlags(3305, 1) == 0) {
            if (Runtime.getFlags(3400, 4) == 0) {
                this.KOW01 = new Uwamono(100, 48);
                this.KOW02 = new Uwamono(101, 48);
                this.KOW03 = new Uwamono(102, 13);
                this.KOW04 = new Uwamono(103, 16);
                this.KOW05 = new Uwamono(104, 13);
                this.KOW06 = new Uwamono(105, 13);
                this.KOW07 = new Uwamono(106, 16);
                this.KOW08 = new Uwamono(107, 13);
                this.KOW09 = new Uwamono(108, 16);
                this.KOW10 = new Uwamono(109, 16);
                this.KOW02.SetSize(1.65f, 2.0f, 1.5f);
            } else if (Runtime.getFlags(3400, 4) == 1) {
                System.println("壊れ物チェック２");
                this.KOW01 = new Uwamono(100, 48);
                this.KOW02 = new Uwamono(101, 48);
                this.KOW03 = new Uwamono(102, 13);
                this.KOW04 = new Uwamono(103, 16);
                this.KOW05 = new Uwamono(104, 13);
                this.KOW06 = new Uwamono(105, 13);
                this.KOW07 = new Uwamono(106, 16);
                this.KOW08 = new Uwamono(107, 13);
                this.KOW09 = new Uwamono(108, 16);
                this.KOW10 = new Uwamono(109, 16);
            } else if (Runtime.getFlags(3400, 4) == 2 || Runtime.getFlags(3400, 4) == 3) {
                System.println("壊れ物チェック３");
                Stage.setVisible(100, false);
                Stage.setVisible(101, false);
                Stage.setVisible(102, false);
                Stage.setVisible(103, false);
                Stage.setVisible(104, false);
                Stage.setVisible(105, false);
                Stage.setVisible(106, false);
                Stage.setVisible(107, false);
                Stage.setVisible(108, false);
                Stage.setVisible(109, false);
            } else if (Runtime.getFlags(3400, 4) == 4 || Runtime.getFlags(3400, 4) == 5) {
                System.println("壊れ物チェック４");
                this.KOW01 = new Uwamono(100, 48);
                this.KOW02 = new Uwamono(101, 48);
                this.KOW03 = new Uwamono(102, 13);
                this.KOW04 = new Uwamono(103, 16);
                this.KOW05 = new Uwamono(104, 13);
                this.KOW06 = new Uwamono(105, 13);
                this.KOW07 = new Uwamono(106, 16);
                this.KOW08 = new Uwamono(107, 13);
                this.KOW09 = new Uwamono(108, 16);
                this.KOW10 = new Uwamono(109, 16);
                this.KOW02.SetSize(1.65f, 2.0f, 1.5f);
            } else {
                System.println("壊れ物チェック５");
                Stage.setVisible(100, false);
                Stage.setVisible(101, false);
                Stage.setVisible(102, false);
                Stage.setVisible(103, false);
                Stage.setVisible(104, false);
                Stage.setVisible(105, false);
                Stage.setVisible(106, false);
                Stage.setVisible(107, false);
                Stage.setVisible(108, false);
                Stage.setVisible(109, false);
            }
        } else {
            System.println("壊れ物チェック６");
            Stage.setVisible(100, false);
            Stage.setVisible(101, false);
            Stage.setVisible(102, false);
            Stage.setVisible(103, false);
            Stage.setVisible(104, false);
            Stage.setVisible(105, false);
            Stage.setVisible(106, false);
            Stage.setVisible(107, false);
            Stage.setVisible(108, false);
            Stage.setVisible(109, false);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.cam0.setCFPedestalHokan(-1, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.015f, 0.015f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(2, 0.015f, 0.015f);
        this.cam0.setCFAngle(3, -28.0f, 20.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.015f, 0.015f);
        this.cam0.setCFAngle(4, -28.0f, -20.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(4, 0.015f, 0.015f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 7.0f, 40.0f);
        this.cam0.setCFHokan(5, 0.015f, 0.015f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 9.0f, 40.0f);
        this.cam0.setCFHokan(6, 0.015f, 0.015f);
        if (Runtime.getFlags(3400, 4) == 0) {
            this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 6.0f, 0.0f, -1.596f, 90.0f);
            this.Hak.enableDTKFlag(4);
            this.Hak.talkto("TalkHak1");
            this.player.setLocation(1, 0);
            System.println("ロボパーツ持ってるのにここを通るのか？");
            System.println("キャラクター初期化ＡＡＡＡＡＡＡＡＡＡＡＡＡＡＡＡＡＡＡＡ");
            this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
            this.COL.SetSize(4.25f, 1.0f, 2.8f);
            Stage.setVisible(158, false);
            Stage.setVisible(97, false);
            Stage.setVisible(157, true);
            Stage.setVisible(11, false);
            System.println("壊れ物チェック１");
            this.KOW01.SetBroken(false);
            this.KOW02.SetBroken(false);
            this.KOW03.SetBroken(false);
            this.KOW04.SetBroken(false);
            this.KOW05.SetBroken(false);
            this.KOW06.SetBroken(false);
            this.KOW07.SetBroken(false);
            this.KOW08.SetBroken(false);
            this.KOW09.SetBroken(false);
            this.KOW10.SetBroken(false);
            this.KOW02.SetSize(1.65f, 2.0f, 1.5f);
        } else if (Runtime.getFlags(3400, 4) == 1) {
            if (Runtime.checkItem(10, 56) != 0 || Runtime.checkItem(10, 57) != 0 && Runtime.checkItem(10, 58) != 0 || Runtime.checkItem(10, 60) != 0 && Runtime.checkItem(10, 61) != 0 || Runtime.checkItem(10, 60) != 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, -0.377f, 0.0f, -3.853f, 0.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak2");
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
                Stage.setVisible(11, false);
                this.KOW01.SetCallNo(1);
                this.KOW02.SetCallNo(2);
                this.KOW03.SetCallNo(3);
                this.KOW04.SetCallNo(4);
                this.KOW05.SetCallNo(5);
                this.KOW06.SetCallNo(6);
                this.KOW07.SetCallNo(7);
                this.KOW08.SetCallNo(8);
                this.KOW09.SetCallNo(9);
                this.KOW10.SetCallNo(10);
                this.KOW02.SetSize(1.65f, 2.0f, 1.5f);
            } else {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 0, 3, -0.377f, 0.0f, -3.853f, 0.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak2a");
                this.KOW01.SetBroken(false);
                this.KOW02.SetBroken(false);
                this.KOW03.SetBroken(false);
                this.KOW04.SetBroken(false);
                this.KOW05.SetBroken(false);
                this.KOW06.SetBroken(false);
                this.KOW07.SetBroken(false);
                this.KOW08.SetBroken(false);
                this.KOW09.SetBroken(false);
                this.KOW10.SetBroken(false);
                this.KOW02.SetSize(1.65f, 2.0f, 1.5f);
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
                Stage.setVisible(11, false);
            }
            System.println("キャラクター初期化ＢＢＢＢＢＢＢＢＢＢＢＢＢＢＢＢＢＢＢＢ");
        } else if (Runtime.getFlags(3400, 4) == 2) {
            if (Runtime.checkItem(10, 57) != 0 && Runtime.checkItem(10, 58) != 0 && Runtime.getFlags(3302, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 0.684f, 0.0f, -2.477f, 200.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak3");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, -10.046f, 0.0f, -1.033f, 0.0f);
                this.Sco.enableDTKFlag(4);
                this.Sco.talkto("TalkSco3");
                this.player.setTranslate(-0.021f, 0.0f, -3.683f);
                Stage.setVisible(157, false);
            } else if (Runtime.checkItem(10, 60) != 0 && Runtime.checkItem(10, 61) != 0 && Runtime.getFlags(3304, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 0.684f, 0.0f, -2.477f, 200.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak3");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, -10.046f, 0.0f, -1.033f, 0.0f);
                this.Sco.enableDTKFlag(4);
                this.Sco.talkto("TalkSco3");
                this.player.setTranslate(-0.021f, 0.0f, -3.683f);
                Stage.setVisible(157, false);
            } else if (Runtime.checkItem(10, 56) != 0 && Runtime.getFlags(3301, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 0.684f, 0.0f, -2.477f, 200.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak3");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, -10.046f, 0.0f, -1.033f, 0.0f);
                this.Sco.enableDTKFlag(4);
                this.Sco.talkto("TalkSco3");
                this.player.setTranslate(-0.021f, 0.0f, -3.683f);
                Stage.setVisible(157, false);
            } else if (Runtime.checkItem(10, 59) != 0 && Runtime.getFlags(3303, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 0.684f, 0.0f, -2.477f, 200.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak3");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, -10.046f, 0.0f, -1.033f, 0.0f);
                this.Sco.enableDTKFlag(4);
                this.Sco.talkto("TalkSco3");
                this.player.setTranslate(-0.021f, 0.0f, -3.683f);
                Stage.setVisible(157, false);
            } else {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 0, 3, 0.684f, 0.0f, -2.477f, 200.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak3a");
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
                Stage.setVisible(11, false);
            }
            System.println("キャラクター初期化ＣＣＣＣＣＣＣＣＣＣＣＣＣＣＣＣＣＣＣＣ");
        } else if (Runtime.getFlags(3400, 4) == 3) {
            if (Runtime.checkItem(10, 57) != 0 && Runtime.checkItem(10, 58) != 0 && Runtime.getFlags(3302, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 7.054f, 0.0f, -1.865f, 10.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak4");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, 7.388f, 0.0f, 0.262f, 190.0f);
                this.Sco.enableDTKFlag(4);
                this.player.setLocation(1, 0);
                Stage.setVisible(157, false);
            } else if (Runtime.checkItem(10, 60) != 0 && Runtime.checkItem(10, 61) != 0 && Runtime.getFlags(3304, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 7.054f, 0.0f, -1.865f, 10.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak4");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, 7.388f, 0.0f, 0.262f, 190.0f);
                this.Sco.enableDTKFlag(4);
                this.player.setLocation(1, 0);
                Stage.setVisible(157, false);
            } else if (Runtime.checkItem(10, 56) != 0 && Runtime.getFlags(3301, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 7.054f, 0.0f, -1.865f, 10.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak4");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, 7.388f, 0.0f, 0.262f, 190.0f);
                this.Sco.enableDTKFlag(4);
                this.player.setLocation(1, 0);
                Stage.setVisible(157, false);
            } else if (Runtime.checkItem(10, 59) != 0 && Runtime.getFlags(3303, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 7.054f, 0.0f, -1.865f, 10.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak4");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, 7.388f, 0.0f, 0.262f, 190.0f);
                this.Sco.enableDTKFlag(4);
                this.player.setLocation(1, 0);
                Stage.setVisible(157, false);
            } else {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 0, 3, 7.054f, 0.0f, -1.865f, 10.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak4a");
                this.Sco = new NPC_NORMAL(1615, 12, 0, 0, 5, 7.388f, 0.0f, 0.262f, 190.0f);
                this.Sco.enableDTKFlag(4);
                this.Sco.talkto("TalkSco4a");
                Stage.setVisible(157, false);
            }
            System.println("キャラクター初期化ＤＤＤＤＤＤＤＤＤＤＤＤＤＤＤＤＤＤＤＤ");
        } else if (Runtime.getFlags(3400, 4) == 4) {
            if (Runtime.checkItem(10, 57) != 0 && Runtime.checkItem(10, 58) != 0 && Runtime.getFlags(3302, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 4.973f, 0.0f, -1.106f, 160.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak5");
                this.player.setTranslate(6.061f, 0.0f, -2.089f);
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
            } else if (Runtime.checkItem(10, 60) != 0 && Runtime.checkItem(10, 61) != 0 && Runtime.getFlags(3304, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 4.973f, 0.0f, -1.106f, 160.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak5");
                this.player.setTranslate(6.061f, 0.0f, -2.089f);
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
            } else if (Runtime.checkItem(10, 56) != 0 && Runtime.getFlags(3301, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 4.973f, 0.0f, -1.106f, 160.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak5");
                this.player.setTranslate(6.061f, 0.0f, -2.089f);
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
            } else if (Runtime.checkItem(10, 59) != 0 && Runtime.getFlags(3303, 1) == 0) {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 4.973f, 0.0f, -1.106f, 160.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak5");
                this.player.setTranslate(6.061f, 0.0f, -2.089f);
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
            } else {
                this.Hak = new NPC_NORMAL(1603, 11, 0, 0, 3, 4.973f, 0.0f, -1.106f, 160.0f);
                this.Hak.enableDTKFlag(4);
                this.Hak.talkto("TalkHak5a");
                this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
                this.COL.SetSize(4.25f, 1.0f, 2.8f);
                Stage.setVisible(158, false);
                Stage.setVisible(97, false);
                Stage.setVisible(157, true);
            }
            System.println("キャラクター初期化ＥＥＥＥＥＥＥＥＥＥＥＥＥＥＥＥＥＥＥＥ");
        } else if (Runtime.getFlags(3400, 4) == 5 && Runtime.getFlags(3156, 1) == 1) {
            this.Hak = new NPC_NORMAL(1603, 11, 0, 3, 3, 6.0f, 0.0f, -1.596f, 90.0f);
            this.Hak.enableDTKFlag(4);
            this.Hak.talkto("TalkHak6");
            this.Sco = new NPC_NORMAL(1615, 12, 0, 3, 5, 7.5f, 0.0f, -2.223f, 295.0f);
            this.Sco.enableDTKFlag(4);
            this.player.setTranslate(7.6f, 0.0f, -1.65f);
            this.Sco.talkto("TalkSco6");
            System.println("キャラクター初期化ＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦ");
            Stage.setVisible(157, false);
        } else if (Runtime.getFlags(3400, 4) == 5 && Runtime.getFlags(3156, 1) == 0) {
            this.Hak = new NPC_NORMAL(1603, 11, 0, 0, 3, 6.0f, 0.0f, -1.596f, 90.0f);
            this.Hak.enableDTKFlag(4);
            this.Hak.talkto("TalkHak6a");
            System.println("キャラクター初期化ＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦＦ");
            this.COL = new Uwamono(28672, -3.5f, 0.0f, -3.5f);
            this.COL.SetSize(4.25f, 1.0f, 2.8f);
            Stage.setVisible(158, false);
            Stage.setVisible(97, false);
            Stage.setVisible(157, true);
        } else {
            System.println("キャラクター初期化ＧＧＧＧＧＧＧＧＧＧＧＧＧＧＧＧＧＧＧＧ");
            this.Hak = new NPC_NORMAL(1603, 11, 0, 0, 3, 6.0f, 0.0f, -1.596f, 90.0f);
            this.Hak.enableDTKFlag(4);
            this.Hak.talkto("TalkHak6");
            this.Sco = new NPC_NORMAL(1615, 12, 0, 0, 5, 7.5f, 0.0f, -2.223f, 295.0f);
            this.Sco.enableDTKFlag(4);
            Stage.setVisible(157, false);
            this.Sco.talkto("TalkSco7");
        }
        this.Kidou = new Mapunits();
        this.Kidou.mapUnit(148);
        this.Kidou.start(4, null);
        this.Kidou.start(1, "robo");
        this.Make = new Mapunits();
        this.Make.mapUnit(12);
        this.Make.start(4, null);
    }

    void nwin(String string) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(string);
        ST2110.waitPage(this.win, 64);
    }

    void nwin(String[] stringArray) {
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(stringArray, 0);
        ST2110.waitPage(this.win, 64);
    }

    static int waitPage(Window window, int n) {
        int n2;
        while ((n2 = window.getSignal()) < n) {
            System.sleep(1);
        }
        return n2;
    }

    void yesno() {
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
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

        void EEE1() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 60) {
                    ST2110.this.Dx_ude.setRotate(ST2110.this.Dx_ude.rx - 2.5f, ST2110.this.Dx_ude.ry, ST2110.this.Dx_ude.rz);
                }
                if (n == 60) break;
                ++n;
                System.sleep(1);
            }
        }

        void EEE2() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 30) {
                    ST2110.this.Dx_ago.setRotate(ST2110.this.Dx_ago.rx + 1.5f, ST2110.this.Dx_ago.ry, ST2110.this.Dx_ago.rz);
                }
                if (n == 30) break;
                ++n;
                System.sleep(1);
            }
        }

        void EEE3() {
            int n = 0;
            while (true) {
                if (n >= 0 && n < 30) {
                    ST2110.this.Dx_ago.setRotate(ST2110.this.Dx_ago.rx - 1.5f, ST2110.this.Dx_ago.ry, ST2110.this.Dx_ago.rz);
                }
                if (n >= 30 && n < 90) {
                    ST2110.this.Dx_ude.setRotate(ST2110.this.Dx_ude.rx + 2.5f, ST2110.this.Dx_ude.ry, ST2110.this.Dx_ude.rz);
                }
                if (n == 90) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setPlayerControl(true);
            ST2110.this.cam0.setMode(0);
            ST2110.this.button_flg = 0;
        }

        void KKK_1() {
            int n = 0;
            ST2110.this.Drl.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196757);
                }
                if (n >= 0 && n < 120) {
                    ST2110.this.Drl.setRotate(ST2110.this.Drl.rx, ST2110.this.Drl.ry + 30.0f, ST2110.this.Drl.rz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
        }

        void KKK_2() {
            int n = 0;
            ST2110.this.Bust.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196759);
                }
                if (n >= 0 && n < 120) {
                    ST2110.this.Bust.setRotate(ST2110.this.Bust.rx, ST2110.this.Bust.ry, ST2110.this.Bust.rz + 3.0f);
                }
                if (n == 120) {
                    ST2110.this.E01.disp(true);
                    Sound.effectPlay(196760);
                }
                if (n == 180) break;
                ++n;
                System.sleep(1);
            }
            ST2110.this.E01.disp(false);
        }

        void KKK_3() {
            int n = 0;
            ST2110.this.Hasami_a.getTranslate();
            ST2110.this.Hasami_b.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196768);
                }
                if (n >= 0 && n < 30) {
                    ST2110.this.Hasami_a.setRotate(ST2110.this.Hasami_a.rx - 1.5f, ST2110.this.Hasami_a.ry, ST2110.this.Hasami_a.rz);
                    ST2110.this.Hasami_b.setRotate(ST2110.this.Hasami_b.rx + 1.5f, ST2110.this.Hasami_b.ry, ST2110.this.Hasami_b.rz);
                }
                if (n >= 30 && n < 60) {
                    ST2110.this.Hasami_a.setRotate(ST2110.this.Hasami_a.rx + 1.5f, ST2110.this.Hasami_a.ry, ST2110.this.Hasami_a.rz);
                    ST2110.this.Hasami_b.setRotate(ST2110.this.Hasami_b.rx - 1.5f, ST2110.this.Hasami_b.ry, ST2110.this.Hasami_b.rz);
                }
                if (n >= 60 && n < 90) {
                    ST2110.this.Hasami_a.setRotate(ST2110.this.Hasami_a.rx - 1.5f, ST2110.this.Hasami_a.ry, ST2110.this.Hasami_a.rz);
                    ST2110.this.Hasami_b.setRotate(ST2110.this.Hasami_b.rx + 1.5f, ST2110.this.Hasami_b.ry, ST2110.this.Hasami_b.rz);
                }
                if (n >= 90 && n < 120) {
                    ST2110.this.Hasami_a.setRotate(ST2110.this.Hasami_a.rx + 1.5f, ST2110.this.Hasami_a.ry, ST2110.this.Hasami_a.rz);
                    ST2110.this.Hasami_b.setRotate(ST2110.this.Hasami_b.rx - 1.5f, ST2110.this.Hasami_b.ry, ST2110.this.Hasami_b.rz);
                }
                if (n == 120) break;
                ++n;
                System.sleep(1);
            }
        }

        void KKK_4() {
            int n = 0;
            ST2110.this.Punch.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196761);
                }
                if (n >= 0 && n < 15) {
                    ST2110.this.Punch.setTranslate(ST2110.this.Punch.px, ST2110.this.Punch.py - 0.13333334f, ST2110.this.Punch.pz);
                }
                if (n >= 15 && n < 45) {
                    ST2110.this.Punch.setRotate(ST2110.this.Punch.rx + 3.0f, ST2110.this.Punch.ry, ST2110.this.Punch.rz);
                }
                if (n == 105) break;
                ++n;
                System.sleep(1);
            }
        }

        void MMM() {
            int n = 0;
            ST2110.this.Eye.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196762);
                }
                if (n >= 0 && n < 6) {
                    ST2110.this.Eye.setRotate(ST2110.this.Eye.rx, ST2110.this.Eye.ry + 12.0f, ST2110.this.Eye.rz);
                }
                if (n >= 6 && n < 18) {
                    ST2110.this.Eye.setRotate(ST2110.this.Eye.rx, ST2110.this.Eye.ry - 12.0f, ST2110.this.Eye.rz);
                }
                if (n >= 18 && n < 24) {
                    ST2110.this.Eye.setRotate(ST2110.this.Eye.rx, ST2110.this.Eye.ry + 12.0f, ST2110.this.Eye.rz);
                }
                if (n == 36) {
                    ST2110.this.E12.disp(true);
                }
                if (n == 90) {
                    ST2110.this.EV_CameraS_01();
                }
                if (n == 95) {
                    ST2110.this.E13.disp(true);
                    ST2110.this.E14.disp(true);
                    Sound.effectPlay(196763);
                }
                if (n == 180) break;
                ++n;
                System.sleep(1);
            }
            ST2110.this.win = Window.create();
            ST2110.this.win.setSize(4, 45);
            ST2110.this.win.setLocation(15, 305);
            ST2110.this.win.print(ST2110.this.Robo4_3_01, 0);
            System.waitFor(ST2110.this.win);
            Runtime.setPlayerControl(true);
            ST2110.this.cam0.setMode(0);
        }

        void Mirumiru() {
            int n = 0;
            ST2110.this.R3.getTranslate();
            ST2110.this.R2.getTranslate();
            ST2110.this.R5.getTranslate();
            ST2110.this.R4.getTranslate();
            ST2110.this.R1.getTranslate();
            ST2110.this.Dai_1.getTranslate();
            ST2110.this.Dai_2.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196752);
                }
                if (n >= 0 && n < 90) {
                    ST2110.this.R3.setTranslate(ST2110.this.R3.px + 0.2777778f, ST2110.this.R3.py, ST2110.this.R3.pz);
                    ST2110.this.R2.setTranslate(ST2110.this.R2.px + 0.2777778f, ST2110.this.R2.py, ST2110.this.R2.pz);
                    ST2110.this.R5.setTranslate(ST2110.this.R5.px + 0.2777778f, ST2110.this.R5.py, ST2110.this.R5.pz);
                    ST2110.this.R4.setTranslate(ST2110.this.R4.px + 0.2777778f, ST2110.this.R4.py, ST2110.this.R4.pz);
                    ST2110.this.R1.setTranslate(ST2110.this.R1.px + 0.2777778f, ST2110.this.R1.py, ST2110.this.R1.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px + 0.2777778f, ST2110.this.Dai_1.py, ST2110.this.Dai_1.pz);
                    ST2110.this.Dai_2.setTranslate(ST2110.this.Dai_2.px + 0.2777778f, ST2110.this.Dai_2.py, ST2110.this.Dai_2.pz);
                }
                if (n == 90) {
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px - 25.0f, ST2110.this.Dai_1.py, ST2110.this.Dai_1.pz);
                    ST2110.this.Dai_2.setTranslate(ST2110.this.Dai_2.px - 25.0f, ST2110.this.Dai_2.py, ST2110.this.Dai_2.pz);
                    ++ST2110.this.Miru;
                    if (ST2110.this.Miru != 4) break;
                    ST2110.this.Limit = 1;
                    ST2110.this.Dai_2.setTranslate(ST2110.this.Dai_2.px + 50.0f, ST2110.this.Dai_2.py, ST2110.this.Dai_2.pz);
                    break;
                }
                ++n;
                System.sleep(1);
            }
        }

        void OOO() {
            int n = 0;
            ST2110.this.OArm_r.getTranslate();
            ST2110.this.OArm_l.getTranslate();
            ST2110.this.Otu.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196753);
                }
                if (n >= 0 && n < 30) {
                    ST2110.this.OArm_r.setRotate(ST2110.this.OArm_r.rx + 4.5f, ST2110.this.OArm_r.ry, ST2110.this.OArm_r.rz);
                    ST2110.this.OArm_l.setRotate(ST2110.this.OArm_l.rx - 4.5f, ST2110.this.OArm_l.ry, ST2110.this.OArm_l.rz);
                }
                if (n >= 30 && n < 90) {
                    ST2110.this.OArm_r.setRotate(ST2110.this.OArm_r.rx - 4.5f, ST2110.this.OArm_r.ry, ST2110.this.OArm_r.rz);
                    ST2110.this.OArm_l.setRotate(ST2110.this.OArm_l.rx + 4.5f, ST2110.this.OArm_l.ry, ST2110.this.OArm_l.rz);
                }
                if (n >= 90 && n < 120) {
                    ST2110.this.OArm_r.setRotate(ST2110.this.OArm_r.rx + 4.5f, ST2110.this.OArm_r.ry, ST2110.this.OArm_r.rz);
                    ST2110.this.OArm_l.setRotate(ST2110.this.OArm_l.rx - 4.5f, ST2110.this.OArm_l.ry, ST2110.this.OArm_l.rz);
                }
                if (n >= 150 && n < 180) {
                    ST2110.this.OArm_r.setRotate(ST2110.this.OArm_r.rx - 6.0f, ST2110.this.OArm_r.ry, ST2110.this.OArm_r.rz);
                    ST2110.this.OArm_l.setRotate(ST2110.this.OArm_l.rx - 6.0f, ST2110.this.OArm_l.ry, ST2110.this.OArm_l.rz);
                }
                if (n == 210) {
                    ST2110.this.E07a.disp(true);
                    Sound.effectPlay(196754);
                }
                if (n == 240) {
                    ST2110.this.E07a.disp(true);
                    Sound.effectPlay(196755);
                }
                if (n >= 240 && n < 300) {
                    ST2110.this.OArm_r.setTranslate(ST2110.this.OArm_r.px, ST2110.this.OArm_r.py + 0.25f, ST2110.this.OArm_r.pz);
                    ST2110.this.OArm_l.setTranslate(ST2110.this.OArm_l.px, ST2110.this.OArm_l.py + 0.25f, ST2110.this.OArm_l.pz);
                    ST2110.this.E07a.setTranslate(ST2110.this.E07a.px, ST2110.this.E07a.py + 0.25f, ST2110.this.E07a.pz);
                    ST2110.this.Otu.setTranslate(ST2110.this.Otu.px, ST2110.this.Otu.py + 0.25f, ST2110.this.Otu.pz);
                }
                if (n == 300) {
                    ST2110.this.E07a.disp(false);
                    Sound.effectPlay(196756);
                }
                if (n >= 300 && n < 390) {
                    ST2110.this.OArm_r.setTranslate(ST2110.this.OArm_r.px, ST2110.this.OArm_r.py - 0.16666667f, ST2110.this.OArm_r.pz);
                    ST2110.this.OArm_l.setTranslate(ST2110.this.OArm_l.px, ST2110.this.OArm_l.py - 0.16666667f, ST2110.this.OArm_l.pz);
                    ST2110.this.E07a.setTranslate(ST2110.this.E07a.px, ST2110.this.E07a.py - 0.16666667f, ST2110.this.E07a.pz);
                    ST2110.this.Otu.setTranslate(ST2110.this.Otu.px, ST2110.this.Otu.py - 0.16666667f, ST2110.this.Otu.pz);
                }
                if (n >= 390 && n < 420) {
                    ST2110.this.OArm_r.setRotate(ST2110.this.OArm_r.rx + 6.0f, ST2110.this.OArm_r.ry, ST2110.this.OArm_r.rz);
                    ST2110.this.OArm_l.setRotate(ST2110.this.OArm_l.rx + 6.0f, ST2110.this.OArm_l.ry, ST2110.this.OArm_l.rz);
                }
                if (n == 420) break;
                ++n;
                System.sleep(1);
            }
            Runtime.setPlayerControl(true);
            ST2110.this.cam0.setMode(0);
            ST2110.this.o8 = 1;
        }

        void Rumirumi() {
            int n = 0;
            ST2110.this.R3.getTranslate();
            ST2110.this.R2.getTranslate();
            ST2110.this.R5.getTranslate();
            ST2110.this.R4.getTranslate();
            ST2110.this.R1.getTranslate();
            ST2110.this.Dai_1.getTranslate();
            ST2110.this.Dai_2.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196752);
                }
                if (n >= 0 && n < 90) {
                    ST2110.this.R3.setTranslate(ST2110.this.R3.px - 0.2777778f, ST2110.this.R3.py, ST2110.this.R3.pz);
                    ST2110.this.R2.setTranslate(ST2110.this.R2.px - 0.2777778f, ST2110.this.R2.py, ST2110.this.R2.pz);
                    ST2110.this.R5.setTranslate(ST2110.this.R5.px - 0.2777778f, ST2110.this.R5.py, ST2110.this.R5.pz);
                    ST2110.this.R4.setTranslate(ST2110.this.R4.px - 0.2777778f, ST2110.this.R4.py, ST2110.this.R4.pz);
                    ST2110.this.R1.setTranslate(ST2110.this.R1.px - 0.2777778f, ST2110.this.R1.py, ST2110.this.R1.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px - 0.2777778f, ST2110.this.Dai_1.py, ST2110.this.Dai_1.pz);
                    ST2110.this.Dai_2.setTranslate(ST2110.this.Dai_2.px - 0.2777778f, ST2110.this.Dai_2.py, ST2110.this.Dai_2.pz);
                }
                if (n == 90) {
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px + 25.0f, ST2110.this.Dai_1.py, ST2110.this.Dai_1.pz);
                    ST2110.this.Dai_2.setTranslate(ST2110.this.Dai_2.px + 25.0f, ST2110.this.Dai_2.py, ST2110.this.Dai_2.pz);
                    --ST2110.this.Miru;
                    if (ST2110.this.Miru != 0) break;
                    ST2110.this.Limit = 0;
                    ST2110.this.Dai_2.setTranslate(ST2110.this.Dai_2.px - 50.0f, ST2110.this.Dai_2.py, ST2110.this.Dai_2.pz);
                    break;
                }
                ++n;
                System.sleep(1);
            }
        }

        void make_ashi() {
            int n = 0;
            ST2110.this.Dai_1.getTranslate();
            ST2110.this.Annyo_r.getTranslate();
            ST2110.this.Annyo_l.getTranslate();
            ST2110.this.Arm_1.getTranslate();
            ST2110.this.Arm_2.getTranslate();
            ST2110.this.Arm_3.getTranslate();
            ST2110.this.Arm_4.getTranslate();
            ST2110.this.Arm_5.getTranslate();
            ST2110.this.Arm_6.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196750);
                }
                if (n >= 0 && n < 45) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 45 && n < 90) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 90 && n < 135) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 135 && n < 180) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 180 && n < 240) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.25f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.25f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.25f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.25f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.25f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.25f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py - 0.25f, ST2110.this.Dai_1.pz);
                    ST2110.this.Annyo_r.setTranslate(ST2110.this.Annyo_r.px, ST2110.this.Annyo_r.py - 0.25f, ST2110.this.Annyo_r.pz);
                    ST2110.this.Annyo_l.setTranslate(ST2110.this.Annyo_l.px, ST2110.this.Annyo_l.py - 0.25f, ST2110.this.Annyo_l.pz);
                }
                if (n == 240) {
                    ST2110.this.fade1.call(0);
                    ST2110.this.E11.disp(true);
                    ST2110.this.Annyo_r.setTranslate(100.0f, 100.0f, 100.0f);
                    ST2110.this.Annyo_l.setTranslate(100.0f, 100.0f, 100.0f);
                }
                if (n == 300) {
                    ST2110.this.R2.setTranslate(0.0f, -15.0f, -22.0f);
                    ST2110.this.fade2.call(0);
                    ST2110.this.E11.disp(false);
                }
                if (n >= 300 && n < 420) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.125f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.125f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.125f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.125f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.125f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.125f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py + 0.125f, ST2110.this.Dai_1.pz);
                    ST2110.this.R2.setTranslate(ST2110.this.R2.px, ST2110.this.R2.py + 0.125f, ST2110.this.R2.pz);
                }
                if (n == 420) break;
                ++n;
                System.sleep(1);
            }
        }

        void make_atama() {
            int n = 0;
            ST2110.this.Dai_1.getTranslate();
            ST2110.this.Atama.getTranslate();
            ST2110.this.Arm_1.getTranslate();
            ST2110.this.Arm_2.getTranslate();
            ST2110.this.Arm_3.getTranslate();
            ST2110.this.Arm_4.getTranslate();
            ST2110.this.Arm_5.getTranslate();
            ST2110.this.Arm_6.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196750);
                }
                if (n >= 0 && n < 45) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 45 && n < 90) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 90 && n < 135) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 135 && n < 180) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 180 && n < 240) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.25f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.25f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.25f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.25f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.25f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.25f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py - 0.25f, ST2110.this.Dai_1.pz);
                    ST2110.this.Atama.setTranslate(ST2110.this.Atama.px, ST2110.this.Atama.py - 0.25f, ST2110.this.Atama.pz);
                }
                if (n == 240) {
                    ST2110.this.fade1.call(0);
                    ST2110.this.E11.disp(true);
                    ST2110.this.Atama.setTranslate(100.0f, 100.0f, 100.0f);
                }
                if (n == 300) {
                    ST2110.this.R5.setTranslate(0.0f, -15.0f, -22.0f);
                    ST2110.this.fade2.call(0);
                    ST2110.this.E11.disp(false);
                }
                if (n >= 300 && n < 420) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.125f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.125f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.125f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.125f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.125f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.125f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py + 0.125f, ST2110.this.Dai_1.pz);
                    ST2110.this.R5.setTranslate(ST2110.this.R5.px, ST2110.this.R5.py + 0.125f, ST2110.this.R5.pz);
                }
                if (n == 420) break;
                ++n;
                System.sleep(1);
            }
        }

        void make_kansei() {
            int n = 0;
            while (true) {
                if (n == 0) {
                    ST2110.this.R1.setTranslate(0.0f, -30.0f, -22.0f);
                    ST2110.this.E11.disp(false);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py - 15.0f, ST2110.this.Dai_1.pz);
                }
                if (n >= 0 && n < 120) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.125f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.125f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.125f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.125f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.125f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.125f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py + 0.125f, ST2110.this.Dai_1.pz);
                    ST2110.this.R1.setTranslate(ST2110.this.R1.px, ST2110.this.R1.py + 0.125f, ST2110.this.R1.pz);
                }
                if (n == 420) break;
                ++n;
                System.sleep(1);
            }
        }

        void make_karada() {
            int n = 0;
            ST2110.this.Dai_1.getTranslate();
            ST2110.this.Mune.getTranslate();
            ST2110.this.Arm_1.getTranslate();
            ST2110.this.Arm_2.getTranslate();
            ST2110.this.Arm_3.getTranslate();
            ST2110.this.Arm_4.getTranslate();
            ST2110.this.Arm_5.getTranslate();
            ST2110.this.Arm_6.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196750);
                }
                if (n >= 0 && n < 45) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 45 && n < 90) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 90 && n < 135) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 135 && n < 180) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 180 && n < 240) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.25f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.25f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.25f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.25f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.25f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.25f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py - 0.25f, ST2110.this.Dai_1.pz);
                    ST2110.this.Mune.setTranslate(ST2110.this.Mune.px, ST2110.this.Mune.py - 0.25f, ST2110.this.Mune.pz);
                }
                if (n == 240) {
                    ST2110.this.fade1.call(0);
                    ST2110.this.E11.disp(true);
                    ST2110.this.Mune.setTranslate(100.0f, 100.0f, 100.0f);
                }
                if (n == 300) {
                    ST2110.this.R4.setTranslate(0.0f, -15.0f, -22.0f);
                    ST2110.this.fade2.call(0);
                    ST2110.this.E11.disp(false);
                }
                if (n >= 300 && n < 420) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.125f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.125f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.125f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.125f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.125f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.125f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py + 0.125f, ST2110.this.Dai_1.pz);
                    ST2110.this.R4.setTranslate(ST2110.this.R4.px, ST2110.this.R4.py + 0.125f, ST2110.this.R4.pz);
                }
                if (n == 420) break;
                ++n;
                System.sleep(1);
            }
        }

        void make_ude() {
            int n = 0;
            ST2110.this.Dai_1.getTranslate();
            ST2110.this.Otete1.getTranslate();
            ST2110.this.Otete2.getTranslate();
            ST2110.this.Arm_1.getTranslate();
            ST2110.this.Arm_2.getTranslate();
            ST2110.this.Arm_3.getTranslate();
            ST2110.this.Arm_4.getTranslate();
            ST2110.this.Arm_5.getTranslate();
            ST2110.this.Arm_6.getTranslate();
            while (true) {
                if (n == 0) {
                    Sound.effectPlay(196750);
                }
                if (n >= 0 && n < 45) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 45 && n < 90) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 90 && n < 135) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.22222222f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.22222222f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.22222222f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.22222222f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.22222222f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.22222222f, ST2110.this.Arm_6.pz);
                }
                if (n >= 135 && n < 180) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.11111111f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.11111111f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.11111111f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.11111111f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.11111111f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.11111111f, ST2110.this.Arm_6.pz);
                }
                if (n >= 180 && n < 240) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py - 0.25f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py - 0.25f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py - 0.25f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py - 0.25f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py - 0.25f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py - 0.25f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py - 0.25f, ST2110.this.Dai_1.pz);
                    ST2110.this.Otete1.setTranslate(ST2110.this.Otete1.px, ST2110.this.Otete1.py - 0.25f, ST2110.this.Otete1.pz);
                    ST2110.this.Otete2.setTranslate(ST2110.this.Otete2.px, ST2110.this.Otete2.py - 0.25f, ST2110.this.Otete2.pz);
                }
                if (n == 240) {
                    ST2110.this.fade1.call(0);
                    ST2110.this.E11.disp(true);
                    ST2110.this.Otete1.setTranslate(100.0f, 100.0f, 100.0f);
                    ST2110.this.Otete2.setTranslate(100.0f, 100.0f, 100.0f);
                }
                if (n == 300) {
                    ST2110.this.R3.setTranslate(0.0f, -15.0f, -22.0f);
                    ST2110.this.fade2.call(0);
                    ST2110.this.E11.disp(false);
                }
                if (n >= 300 && n < 420) {
                    ST2110.this.Arm_1.setTranslate(ST2110.this.Arm_1.px, ST2110.this.Arm_1.py + 0.125f, ST2110.this.Arm_1.pz);
                    ST2110.this.Arm_2.setTranslate(ST2110.this.Arm_2.px, ST2110.this.Arm_2.py + 0.125f, ST2110.this.Arm_2.pz);
                    ST2110.this.Arm_3.setTranslate(ST2110.this.Arm_3.px, ST2110.this.Arm_3.py + 0.125f, ST2110.this.Arm_3.pz);
                    ST2110.this.Arm_4.setTranslate(ST2110.this.Arm_4.px, ST2110.this.Arm_4.py + 0.125f, ST2110.this.Arm_4.pz);
                    ST2110.this.Arm_5.setTranslate(ST2110.this.Arm_5.px, ST2110.this.Arm_5.py + 0.125f, ST2110.this.Arm_5.pz);
                    ST2110.this.Arm_6.setTranslate(ST2110.this.Arm_6.px, ST2110.this.Arm_6.py + 0.125f, ST2110.this.Arm_6.pz);
                    ST2110.this.Dai_1.setTranslate(ST2110.this.Dai_1.px, ST2110.this.Dai_1.py + 0.125f, ST2110.this.Dai_1.pz);
                    ST2110.this.R3.setTranslate(ST2110.this.R3.px, ST2110.this.R3.py + 0.125f, ST2110.this.R3.pz);
                }
                if (n == 420) break;
                ++n;
                System.sleep(1);
            }
        }

        void robo() {
            ST2110.this.ohanasi_sinkou = Runtime.getFlags(3400, 4);
            if (ST2110.this.ohanasi_sinkou == 0) {
                ++ST2110.this.ohanasi_sinkou;
                Runtime.setFlags(3400, 4, ST2110.this.ohanasi_sinkou);
                System.println("1話");
                ST2110.this.First();
                ST2110.this.ohanasi = 0;
                ST2110.this.tukuru = 10;
            } else if (Runtime.getFlags(3305, 1) == 1) {
                System.println("完成後のロボ研へ 完成後のロボ研へ 完成後のロボ研へ");
                ST2110.this.ohanasi = 2;
                ST2110.this.tukuru = 0;
            } else if (Runtime.checkItem(10, 57) != 0 && Runtime.checkItem(10, 58) != 0 && Runtime.getFlags(3302, 1) == 0) {
                System.println("手を造るロボ研へ 手を造るロボ研へ 手を造るロボ研へ");
                ST2110.this.ohanasi = 1;
                ST2110.this.tukuru = 1;
            } else if (Runtime.checkItem(10, 60) != 0 && Runtime.checkItem(10, 61) != 0 && Runtime.getFlags(3304, 1) == 0) {
                System.println("足を造るロボ研へ 足を造るロボ研へ 足を造るロボ研へ");
                ST2110.this.ohanasi = 1;
                ST2110.this.tukuru = 2;
            } else if (Runtime.checkItem(10, 56) != 0 && Runtime.getFlags(3301, 1) == 0) {
                System.println("頭を造るロボ研へ 頭を造るロボ研へ 頭を造るロボ研へ");
                ST2110.this.ohanasi = 1;
                ST2110.this.tukuru = 3;
            } else if (Runtime.checkItem(10, 59) != 0 && Runtime.getFlags(3303, 1) == 0) {
                System.println("胴を造るロボ研へ 胴を造るロボ研へ 胴を造るロボ研へ");
                ST2110.this.ohanasi = 1;
                ST2110.this.tukuru = 4;
            } else if (Runtime.getFlags(3156, 1) == 1) {
                System.println("ロボを造るロボ研へ ロボを造るロボ研へ ロボを造るロボ研へ");
                ST2110.this.ohanasi = 1;
                ST2110.this.tukuru = 5;
            } else {
                System.println("何も起こらないロボ研へ 何も起こらないロボ研へ 何も起こらないロボ研へ");
                ST2110.this.ohanasi = 0;
                ST2110.this.tukuru = 10;
            }
            if (ST2110.this.ohanasi == 1) {
                if (ST2110.this.ohanasi_sinkou == 1) {
                    ST2110.this.Second();
                    ++ST2110.this.ohanasi_sinkou;
                    Runtime.setFlags(3400, 4, ST2110.this.ohanasi_sinkou);
                    System.println("２話");
                } else if (ST2110.this.ohanasi_sinkou == 2) {
                    ++ST2110.this.ohanasi_sinkou;
                    Runtime.setFlags(3400, 4, ST2110.this.ohanasi_sinkou);
                    System.println("3話");
                    ST2110.this.Third();
                } else if (ST2110.this.ohanasi_sinkou == 3) {
                    ++ST2110.this.ohanasi_sinkou;
                    Runtime.setFlags(3400, 4, ST2110.this.ohanasi_sinkou);
                    System.println("4話");
                    ST2110.this.Forth();
                } else if (ST2110.this.ohanasi_sinkou == 4) {
                    ++ST2110.this.ohanasi_sinkou;
                    Runtime.setFlags(3400, 4, ST2110.this.ohanasi_sinkou);
                    System.println("5話");
                    ST2110.this.Fifth();
                } else if (ST2110.this.ohanasi_sinkou == 5) {
                    if (Runtime.getFlags(3305, 1) == 0 && Runtime.getFlags(3156, 1) == 1) {
                        ++ST2110.this.ohanasi_sinkou;
                        Runtime.setFlags(3400, 4, ST2110.this.ohanasi_sinkou);
                        System.println("6話");
                        ST2110.this.Final();
                    } else {
                        System.println("01話");
                    }
                }
            }
            if (ST2110.this.ohanasi_sinkou != 2) {
                if (ST2110.this.tukuru == 0) {
                    ST2110.this.Robo_Final();
                } else if (ST2110.this.tukuru == 1) {
                    ST2110.this.Robo_Ude();
                    Runtime.setFlags(3302, 1, 1);
                } else if (ST2110.this.tukuru == 2) {
                    ST2110.this.Robo_Ashi();
                    Runtime.setFlags(3304, 1, 1);
                } else if (ST2110.this.tukuru == 3) {
                    ST2110.this.Robo_Atama();
                    Runtime.setFlags(3301, 1, 1);
                } else if (ST2110.this.tukuru == 4) {
                    ST2110.this.Robo_Karada();
                    Runtime.setFlags(3303, 1, 1);
                } else if (ST2110.this.tukuru == 5) {
                    ST2110.this.Robo_Zenbu();
                    Runtime.setFlags(3305, 1, 1);
                } else {
                    ST2110.this.Robo_Nashi();
                }
            }
        }
    }
}

