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
import xeno.map.MC_VOK16_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.Math;
import xeno.vm.System;

class ST0160
        extends Stage
        implements XenoConstants,
        JNT_Accesories,
        JNT_Human,
        CfConstants,
        MC_VOK16_PRJ {
    int i = 0;
    int play = 0;
    float ang;
    float ang2 = 1.5f;
    float posT = 0.852f;
    float posT2;
    Player player;
    Camera cam1;
    Camera Ecam;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc6;
    Enepc npc7;
    Enepc npc8;
    Enepc ukun01;
    Enepc ukun02;
    Enepc ukun03;
    Enepc ukun04;
    Enepc ukun05;
    Enepc ukun06;
    Unit unit1;
    Menu menu;
    Window win;
    int mau;
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
    Uwamono doa01;
    Uwamono doa02;
    Uwamono doa03;
    Uwamono doa04;
    Uwamono doa05;
    Uwamono doa06;
    Uwamono doa07;
    Unit dai;
    Unit dai2;
    Unit dai3;
    MAPUnit tensi;
    MAPUnit ugau;
    MAPUnit drill_01;
    MAPUnit drill_02;
    MAPUnit drill_03;
    MAPUnit drill_04;
    MAPUnit drill_05;
    MAPUnit hako_01;
    MAPUnit hako_02;
    MAPUnit hako_03;
    MAPUnit hako_04;
    MAPUnit hako_05;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono drill;
    Uwamono kon0;
    Uwamono kon1;
    Uwamono kon2;
    Uwamono kon3;
    Uwamono kon4;
    Uwamono kon5;
    Uwamono kon6;
    Uwamono kon7;
    Uwamono kon8;
    Uwamono kon9;
    Uwamono kon10;
    Uwamono kon11;
    Uwamono kon12;
    Uwamono kon13;
    Uwamono kon14;
    Uwamono kon15;
    Uwamono kon16;
    Uwamono kon17;
    Uwamono kon18;
    Uwamono kon19;
    Uwamono kon20;
    Uwamono kon21;
    Uwamono kon22;
    Uwamono kon23;
    Uwamono kon24;
    Uwamono kon25;
    Uwamono kon26;
    Uwamono kon27;
    Uwamono kon28;
    Unit DRILLD;
    int breakflg;
    String moji;
    Effect fade;
    Effect EFtensi;
    Effect fade30;
    Effect fade60;
    Effect EF001;
    Effect EF002;
    Effect EF003;
    Effect EF004;
    Effect EF005;
    Effect EF00;
    Effect EF01;
    Effect EF02;
    Effect EF03;
    Effect EF04;
    Effect EF05;
    Effect EF06;
    Effect EF07;
    Effect EF08;
    Effect EF09;
    Effect EF10;
    Effect EF11;
    Effect EF12;
    Effect EF13;
    Light light = new Light(0);
    boolean tensibroken = false;
    boolean hutuu = false;
    boolean hutoi = false;
    boolean hosoi = false;
    boolean SUCCESS = false;
    boolean FAILED = false;
    boolean YET = false;
    boolean npc5TKF = true;
    boolean npc6TKF = true;
    boolean npc7TKF = true;
    boolean mission1 = false;
    boolean mission2 = false;
    boolean mission3 = false;
    boolean mission4 = false;
    boolean timeover = false;
    boolean cancel = false;
    boolean junbi = false;
    boolean M1_1 = false;
    boolean M1_2 = false;
    boolean M1_3 = false;
    boolean M1_4 = false;
    boolean M1_5 = false;
    boolean M1_6 = false;
    boolean M1_7 = false;
    boolean M1_8 = false;
    boolean M1_9 = false;
    boolean M1_10 = false;
    boolean M1_11 = false;
    boolean M1_12 = false;
    boolean M1_13 = false;
    boolean M1_14 = false;
    boolean M1_15 = false;
    boolean M1_16 = false;
    boolean M1_17 = false;
    boolean M1_18 = false;
    boolean M1_19 = false;
    boolean M1_20 = false;
    boolean M1_21 = false;
    boolean M1_22 = false;
    boolean M1_23 = false;
    boolean M1_24 = false;
    boolean M1_25 = false;
    boolean M1_26 = false;
    boolean M1_27 = false;
    boolean M1_28 = false;
    boolean M1_29 = false;
    boolean M1_30 = false;
    boolean M1_31 = false;
    boolean M1_32 = false;
    boolean M1_33 = false;
    boolean M1_34 = false;
    boolean M1_35 = false;
    boolean M1_36 = false;
    Unit dummy;
    boolean setumei = true;
    boolean ugauga = true;
    boolean ugauga2 = true;
    int b_flg;
    String[] EVS = new String[]{"Ending Driller Game.", "/[waitkey(64)]/[close()]"};
    int page;
    String[] msgFirst_M = new String[]{"/[label(Holgar)]", "Welcome to the Driller Game!! It's your first time, right? Let me check your /[color(0x329bbe)]Drill Passport/[color(0x808080)].", "/[waitkey(64)]/[clear()]"};
    String[] msgFirst_M1 = new String[]{"/[label(Holgar)]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msgFirst_M2 = new String[]{"/[label(Holgar)]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msgFirst_M3 = new String[]{"/[label(Holgar)]", "Okay! Everything checks out. Now, you're our customers. Go talk to Hollyanna over there for instructions on the Driller Game. Come talk to me whenever you want to\nplay the game.", "/[waitkey(64)]/[close()]"};
    String[] msgFsetumei = new String[]{"/[label(Holgar)]", "Want me to explain Free Mode?", "/[waitkey(64)]/[close()]"};
    String[] msgMsetumei = new String[]{"/[label(Holgar)]", "Want me to explain this mission?", "/[waitkey(64)]/[close()]"};
    String[] msgMsetuClear = new String[]{"/[label(Holgar)]", "You've already cleared this mission. You still want to retry it?\n", "/[waitkey(64)]/[close()]"};
    String[] msgYappari = new String[]{"/[label(Holgar)]", "What's this? Just wandering around, huh? You can't win prizes just by watching.", "/[waitkey(64)]/[close()]"};
    String[] msgFsetumei1 = new String[]{"/[label(Holgar)]", "There's no time limit. You can move the drill 5 times.", "/[waitkey(1)]/[clear()]", "An item will appear when you break a box. Pick them up as you wish after you finish the game.", "/[waitkey(1)]/[clear()]", "You won't always find an item inside, but once in a while, you'll find a /[color(0x329bbe)]Drill Key/[color(0x808080)].", "/[waitkey(64)]/[close()]"};
    String[] msgMsetumei1 = new String[]{"/[label(Holgar)]", "Time limit is 80 seconds, but you can move the drill as many times as you want. The mission is to destroy all the boxes in that time.", "/[waitkey(1)]/[clear()]", "Even if you destroy all the boxes, you won't clear the mission until the drill's strut is back at its starting position, so be careful. A true driller will make sure to completely finish the task.", "/[waitkey(1)]/[clear()]", "If you succeed, you'll get a special present! The mission's not too difficult if you use your noggin.", "/[waitkey(64)]/[close()]"};
    String[] msgMsetumei2 = new String[]{"/[label(Holgar)]", "Time limit is 120 seconds, but you can move the drill as many times as you want.", "/[waitkey(1)]/[clear()]", "The mission is to destroy all the boxes in that time. Even if you destroy all the boxes, you won't clear the mission until the drill's strut is back at its starting position,\nso be careful. A true driller will make sure to completely finish the task.", "/[waitkey(1)]/[clear()]", "If you succeed, you'll get a special present. Accuracy is essential -- misses are detrimental. Errors will cost you.", "/[waitkey(64)]/[close()]"};
    String[] msgMsetumei3 = new String[]{"/[label(Holgar)]", "Time limit is 90 seconds, but you can move the drill as many times as you want. During that time, get the moving white demons...", "/[waitkey(64)]/[clear()]"};
    String[] msgMsetumei3_1 = new String[]{"/[label(Hollyanna)]", "Its name is Bunnie!!", "/[waitkey(64)]/[clear()]"};
    String[] msgMsetumei3_2 = new String[]{"/[label(Holgar)]", "Uh...umm...yeah...I like to call them white demons, but anyway, destroy them all to clear the mission.", "/[waitkey(1)]/[clear()]", "Even if you destroy all the white demons, you won't clear the mission until the drill's strut is back at its starting position, so be careful. A true driller will make sure to completely finish the task.", "/[waitkey(1)]/[clear()]", "These white demons are our state-of-the-art mechanical dolls. I highly doubt anyone will be able to destroy all of them.", "/[waitkey(64)]/[close()]"};
    String[] msgMsetumei4 = new String[]{"/[label(Holgar)]", "Time limit is 50 seconds, but you can move the drill as many times as you want. During that time, get the moving white demon...", "/[waitkey(64)]/[clear()]"};
    String[] msgMsetumei4_1 = new String[]{"/[label(Hollyanna)]", "It's name is Bunnie!!", "/[waitkey(64)]/[clear()]"};
    String[] msgMsetumei4_2 = new String[]{"/[label(Holgar)]", "It is three times better than the ones in Mission 3, so only those with true passion for drillin' will be able to take it down.", "/[waitkey(1)]/[clear()]", "Oh, right, even if you destroy the white demon, you won't clear the mission until the drill's strut is back at its starting position, so be careful. Basically, the meal ain't over 'til you put down the fork.", "/[waitkey(64)]/[close()]"};
    String[] msgD01_1 = new String[]{"/[label(Holgar)]", "Welcome! One game costs 200G. How about it?", "/[waitkey(64)]/[close()]"};
    String[] msgD01_2 = new String[]{"/[label(Holgar)]", "Really?! You will?! What mode do you want to play?", "/[waitkey(64)]/[close()]"};
    String[] msgD01_3 = new String[]{"/[label(Holgar)]", "You're not playing?! And here I thought you had a passion for drillin'. Well, come back whenever you feel like playing!", "/[waitkey(64)]/[close()]"};
    String[] msgD01_4 = new String[]{"/[label(Holgar)]", "Uh-oh, looks like you don't have enough money. Man! If only you had 200G, you might have gotten incredibly rare items up the wazoo!", "/[waitkey(1)]/[clear()]", "Awww, it's a real shame. Come back again when you have some more money, will ya?", "/[waitkey(64)]/[close()]"};
    String[] msgFirst_F = new String[]{"/[label(Hollyanna)]", "Welcome to the Driller Game!! You're a first time customer, correct? Please have your /[color(0x329bbe)]Drill Passport/[color(0x808080)] verified at the counter to your left.", "/[waitkey(64)]/[close()]"};
    String[] msgOpeA = new String[]{"/[label(Hollyanna)]", "Welcome to the Driller Game!!", "/[waitkey(64)]/[clear()]"};
    String[] msgOpeB = new String[]{"/[label(Hollyanna)]", "What would you like me to explain?", "/[waitkey(64)]/[close()]"};
    String[] msgOpe0_1 = new String[]{"/[label(Hollyanna)]", "This is a sophisticated, intense, and exhilarating game. The object is to move a drill and destroy various objects.", "/[waitkey(1)]/[clear()]", "Of course, this game isn't simply about destroying things. We have various items and rare prizes waiting for you! We eagerly await people with a serious passion\nfor drilling to give this game a try! One game costs 200G.", "/[waitkey(1)]/[clear()]", "This concludes the explanation of the Driller Game. Would you like me to explain it again?", "/[waitkey(64)]/[close()]"};
    String[] msgOpe1_1 = new String[]{"/[label(Hollyanna)]", "When the Driller Game starts, use the □ Button to move the drill strut. It will continue to move while you are holding down the □ Button.", "/[waitkey(1)]/[clear()]", "Release the button when the drill strut reaches your desired location to make it stop.", "/[waitkey(1)]/[clear()]", "Next, you will move the drill itself. You will also move this with the □ Button. When you are done moving the drill strut and the drill, the drill bit will drop. This will\nhappen automatically.", "/[waitkey(1)]/[clear()]", "If there is a box below it, it will be destroyed. If nothing is there, you have failed. When you are not moving the drill, you can change your view freely with the Left Analog Stick.", "/[waitkey(1)]/[clear()]", "Furthermore, the R2 Button allows you to switch between three viewpoints quickly. You will be more successful if you learn how to utilize this feature.", "/[waitkey(1)]/[clear()]", "To discontinue the Driller Game, press the Ｘ Button. Please note that you cannot replay a Driller Game that you quit.", "/[waitkey(1)]/[clear()]", "This concludes the explanation of the controls. Would you like me to explain it again?", "/[waitkey(64)]/[close()]"};
    String[] msgOpe2_1 = new String[]{"/[label(Hollyanna)]", "There are two modes in the Driller Game.", "/[waitkey(1)]/[clear()]", "The first one is Free Mode. In this mode, you will aim for randomly placed boxes and destroy them. This is a very popular mode -- drillers constantly play this\nmode in order to get the items that come out of the broken boxes.", "/[waitkey(1)]/[clear()]", "This is where you can obtain the /[color(0x329bbe)]Drill Key/[color(0x808080)] that allows you access to rare prizes.", "/[waitkey(1)]/[clear()]", "The second one is Mission Mode. In this mode, you can get a special, one-of-a-kind prize if you fulfill the conditions within the time limit. These highly difficult Driller Game missions are very popular among our veteran patrons.", "/[waitkey(1)]/[clear()]", "Some missions may be added later on, so come back and play periodically to see if we added any more games!", "/[waitkey(1)]/[clear()]", "This concludes the explanation of the game modes. Would you like me to explain it again?", "/[waitkey(64)]/[close()]"};
    String[] msgOpe3_1 = new String[]{"/[label(Hollyanna)]", "Here at the Driller Game, we are proud to offer a large assortment of rare items!! ", "If you have a real passion for drilling, all of our prizes are yours for the taking!", "/[waitkey(1)]/[clear()]", "In Free Mode, there are some boxes with a /[color(0x329bbe)]Drill Key/[color(0x808080)] hidden inside. If you are lucky enough to get a /[color(0x329bbe)]Drill Key/[color(0x808080)], please use it at the prize exhibit under the\nstar-shaped, neon \"DRILL\" sign.", "/[waitkey(1)]/[clear()]", "You may exchange one /[color(0x329bbe)]Drill Key/[color(0x808080)] for one prize. We also have deluxe prizes when you clear a game in Mission Mode. Please give it a try!", "/[waitkey(1)]/[clear()]", "This concludes the explanation on prizes. Would you like me to explain it again?", "/[waitkey(64)]/[close()]"};
    String[] msgOpe4_1 = new String[]{"/[label(Hollyanna)]", "This will conclude the explanations. Will that be all?", "/[waitkey(64)]/[close()]"};
    String[] msgOpe5_1 = new String[]{"/[label(Hollyanna)]", "Then please enjoy the Driller Game!", "/[waitkey(64)]/[close()]"};
    String[] msgTensi = new String[]{"...There's a mysterious button. Press it?", "/[waitkey(64)]/[close()]"};
    String[] msgTensi1 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    String[] msgTensi2 = new String[]{"...", "/[waitkey(64)]/[close()]"};
    String[] msgTensi3 = new String[]{"...!!", "/[waitkey(64)]/[clear()]"};
    String[] msgTensi4 = new String[]{"...", "/[waitkey(64)]/[clear()]"};
    String[] msgTensi5 = new String[]{"...!!", "/[waitkey(64)]/[close()]"};
    String[] msgTensi6 = new String[]{"The Drill Angel has been destroyed.", "/[waitkey(64)]/[close()]"};
    String[] msgend = new String[]{"/[label(Hollyanna)]", "Would you like to quit the Driller Game?", "/[waitkey(64)]/[close()]"};
    String[] msgM1_1 = new String[]{"/[label(Holgar)]", "Mission 1, complete.", "/[waitkey(1)]/[clear()]", "Swift and precise. You guys have all the qualities of a good driller! Hollyanna! Give them their prize!", "/[waitkey(64)]/[clear()]"};
    String[] msgM1_2 = new String[]{"/[label()]", "Yes, Sir. Here's your prize. Please enjoy.", "/[waitkey(64)]/[clear()]"};
    String[] msgM2_1 = new String[]{"/[label(Holgar)]", "Mission 2, complete.", "/[waitkey(1)]/[clear()]", "That was amazing! You guys have a great passion for Drilling. Hollyanna! Give them their prize! And make it a special rare one at that!!", "/[waitkey(64)]/[clear()]"};
    String[] msgM2_2 = new String[]{"/[label()]", "Yes, Sir. Here's your special rare prize. Please enjoy.", "/[waitkey(64)]/[clear()]"};
    String[] msgM3_1 = new String[]{"/[label(Holgar)]", "Mission 3, complete!", "/[waitkey(1)]/[clear()]", "You destroyed all 5 Bunnies?! Hollyanna, give them their prize...", "/[waitkey(64)]/[clear()]"};
    String[] msgM3_2 = new String[]{"/[label()]", "That's i-impossible...m-my Bunnies...\n", "/[waitkey(1)]/[clear()]", "Oh! Pardon me! Congratulations. Here's your prize. Please enjoy.", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_1 = new String[]{"/[label(Holgar)]", "Mission 4, complete.", "/[waitkey(1)]/[clear()]", "An absolute defeat! Hollyanna, satisfied now? Give them their prize!", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_2 = new String[]{"/[label(Hollyanna)]", "How could my Bunnie lose...? I guess using those old circuits were...", "/[waitkey(1)]/[clear()]", "*mutter, mutter, mutter*", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_3 = new String[]{"/[label(Holgar)]", "Hollyanna!", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_4 = new String[]{"/[label(Hollyanna)]", "Oh! Pardon me! Congratulations. B-but what should I give them...?", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_5 = new String[]{"/[label(Holgar)]", "We have that thing, remember?", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_6 = new String[]{"/[label(Hollyanna)]", "B-by that, do you mean what we acquired just the other day at the Drilling Championship...?!", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_7 = new String[]{"/[label(Holgar)]", "That's right, these people have a passion for drilling worthy of it.", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_8 = new String[]{"/[label(Hollyanna)]", "But...that's...", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_9 = new String[]{"/[label(Holgar)]", "That's enough. The customers are waiting. Hurry up and give it to them!", "/[waitkey(64)]/[clear()]"};
    String[] msgM4_10 = new String[]{"/[label()]", "Yes, Sir, pardon me. Congratulations. Here's your prize. Please enjoy.", "/[waitkey(64)]/[clear()]"};
    String[] msgAgain1 = new String[]{"/[label(Holgar)]", "You cleared it again?! You guys are really something. Hollyanna, give them their pri...", "/[waitkey(1)]/[clear()]", "Oops, I'd love to give you one, but we're out of prizes for that mission. Hollyanna, give them something else instead!", "/[waitkey(64)]/[clear()]"};
    String[] msgAgain2 = new String[]{"/[label()]", "Yes, Sir. Here's your prize. It's a Card Game set that is popular right now. Please enjoy.", "/[waitkey(64)]/[clear()]"};
    String[] msgConp1 = new String[]{"/[label(Hollyanna)]", "These people have now cleared all of the missions.", "/[waitkey(64)]/[clear()]"};
    String[] msgConp2 = new String[]{"/[label(Holgar)]", "Really?! So they finally did it! I knew you guys were the real deal! This item is a bit worn out from use, but take it with you. This is the most powerful drill in the world!", "/[waitkey(1)]/[clear()]", "Back in my day, I used this to wreck anything I saw. From now on, you can choose from either \"Most powerful drill\" or \"Normal drill\" to play with.", "/[waitkey(64)]/[close()]"};
    String[] msgDrillSelect1 = new String[]{"/[label(Holgar)]", "Which drill are you going to play with?", "/[waitkey(64)]/[close()]"};
    String[] msgDrillSelectL = new String[]{"/[label(Hollyanna)]", "As you wish. Setting the most powerful drill.", "/[waitkey(64)]/[close()]"};
    String[] msgDrillSelectM = new String[]{"/[label(Hollyanna)]", "As you wish. Setting the normal drill.", "/[waitkey(64)]/[close()]"};
    String[] msgDrillSelectS = new String[]{"/[label(Hollyanna)]", "As you wish. Setting the most powerful drill.", "/[waitkey(1)]/[clear()]", "...\n", "/[waitkey(1)]/[clear()]", "What...?! Pardon me! I mistakenly attached the weakest drill!", "/[waitkey(64)]/[clear()]"};
    String[] msgDrillSelectS2 = new String[]{"/[label(Holgar)]", "What?! Urg, i-it...w-won't come off...", "/[waitkey(1)]/[clear()]", "Sorry, but you gotta play with the weakest drill this time.", "/[waitkey(64)]/[close()]"};
    String[] msgTimeOver = new String[]{"/[label(Holgar)]", "What's wrong? Did you lose your passion for the drill? If it were me, I would...uh, I'm sorry, got carried away.", "/[waitkey(1)]/[clear()]", "Here's your consolation prize, a Med Kit. Next time, show me that passion you had for the drill, okay?", "/[waitkey(64)]/[close()]"};
    String[] msgStop = new String[]{"/[label(Holgar)]", "You haven't used the /[color(0x329bbe)]Drill Key/[color(0x808080)] that you got! Press the ○ Button in front of any DRILL door!", "/[waitkey(64)]/[close()]"};
    String[] msgBox1 = new String[]{"The box with a ribbon on it is marked, \"Spectacular prize inside!\"", "/[waitkey(64)]/[close()]"};
    String[] msgBox2 = new String[]{"/[label(Holgar)]", "That door won't open without a /[color(0x329bbe)]Drill Key/[color(0x808080)]! The prize is yours if you have a /[color(0x329bbe)]Drill Key/[color(0x808080)]!", "/[waitkey(64)]/[close()]"};
    String[] msgGameend = new String[]{"/[label(Holgar)]", "Well? Did you hit your targets? Pick up any items that came out, because they'll get cleared away before the next game.", "/[waitkey(64)]/[close()]"};
    String[] msgGameend2 = new String[]{"/[label(Holgar)]", "Hey, hey, you quitting?! I guess being passionate about drilling isn't cool anymore, huh? Come play again...", "/[waitkey(64)]/[close()]"};
    String[] msgJunbi = new String[]{"/[label(Hollyanna)]", "Pardon me. That one is not ready yet. Please select a different mission.", "/[waitkey(64)]/[close()]"};
    String[] msgShopJ1 = new String[]{"/[label(Retired Old Man)]", "Ho ho ho, this child can't get items in the Driller Game, so she's asking me to buy them. If you people have any extra items, won't you sell them to me?", "/[waitkey(64)]/[clear()]"};
    String[] msgShopJ2 = new String[]{"/[label(Retired Old Man)]", "Ho ho ho, I'll buy your items. If you're going to sell me some items, sell me a lot. Tam will be happier that way.", "/[waitkey(64)]/[clear()]"};
    String[] msgShopJ3 = new String[]{"/[label(Retired Old Man)]", "Ho ho ho. I'll buy any extra items that you have.", "/[waitkey(64)]/[clear()]"};
    String[] msgShopM1 = new String[]{"/[label(Tam)]", "Grandpa, buy lots, okay?", "/[waitkey(64)]/[close()]"};
    String[] msgShopM2 = new String[]{"/[label(Tam)]", "We're buying!", "/[waitkey(64)]/[close()]"};
    String[] msgShopM3 = new String[]{"/[label(Tam)]", "I can't get any items!\n", "It's no fun!\n", "It's no fun!\n", "I want items!\n", "Gimme!\n", "Gimme!", "/[waitkey(64)]/[close()]"};
    String[] msgWoman1 = new String[]{"/[label(Driller Fan)]", "Looks like it's your first time here. Why don't you check in first? The check-in counter is near the entrance.", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA1 = new String[]{"/[label(Unlucky Man)]", "Damn, damn, damn! Where is the /[color(0x329bbe)]Drill Key/[color(0x808080)]?! ", "Hmm? You wanna know what a /[color(0x329bbe)]Drill Key/[color(0x808080)] is? You must be a first-timer. Go check in first.", "/[waitkey(64)]/[close()]"};
    String[] msgGoroB1 = new String[]{"/[label(Hardcore Driller)]", "Never seen you around here. If you want to use the drill, you should get checked in first.", "/[waitkey(64)]/[close()]"};
    String[] msgWoman2_1 = new String[]{"/[label(Driller Fan)]", "What mode do you like to play? I'm all about the Free Mode. It's so exciting to see what comes out. If you're lucky enough to get a /[color(0x329bbe)]Drill Key/[color(0x808080)], you can even get\nprizes. I really recommend the Free Mode.", "/[waitkey(64)]/[close()]"};
    String[] msgWoman2_2 = new String[]{"/[label(Driller Fan)]", "The box contents in Free Mode seem to have a pattern to them. There are all sorts of boxes, but for instance, there are some boxes that always have money in them. If you can figure out what box has what, it'll be easy to consistently get things.", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA2_1 = new String[]{"/[label(Unlucky Man)]", "Damn, damn, damn! I can't get any /[color(0x329bbe)]Drill Key/[color(0x808080)]! If only I had a /[color(0x329bbe)]Drill Key/[color(0x808080)], I could open this door! The prizes here are all one of a kind.", "/[waitkey(1)]/[clear()]", "Grr, I don't want someone else to get it! Hey, you better not swipe them!", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA2_2 = new String[]{"/[label(Unlucky Man)]", "Damn, damn, damn! I'm so worried. Someone else might make off with the prize while I'm looking for a /[color(0x329bbe)]Drill Key/[color(0x808080)]. Hey, you better not swipe them!", "/[waitkey(64)]/[close()]"};
    String[] msgGoroB2_1 = new String[]{"/[label(Hardcore Driller)]", "Humph. Missions are what the DG is about. Oops, sorry, sorry, it's the Driller Game, to be correct. Veterans like me call it DG.", "/[waitkey(1)]/[clear()]", "Well, you seem like a total newbie, so you should just stick to calling it the Driller Game.", "/[waitkey(64)]/[close()]"};
    String[] msgGoroB2_2 = new String[]{"/[label(Hardcore Driller)]", "Tell you the secret to clearing missions?! Humph! What a lame thing to ask. A true driller has his own method of conquest. I can't just tell you so easily.", "/[waitkey(64)]/[close()]"};
    String[] msgWoman3_1 = new String[]{"/[label(Driller Fan)]", "I'm beginning to understand what's in the boxes in Free Mode. You know those thin, green boxes?", "/[waitkey(1)]/[clear()]", "You always seem to get money when you break them. The amount varies, but I'm sure of it.", "/[waitkey(64)]/[close()]"};
    String[] msgWoman3_2 = new String[]{"/[label(Driller Fan)]", "It seems the /[color(0x329bbe)]Drill Key/[color(0x808080)] is always in the same type of box as well. It's just a rumor, though.", "/[waitkey(1)]/[clear()]", "I'm friends with Hollyanna, but she still refuses to tell me. No matter how many times I ask her, all she talks about is her Bunnie. She's got some serious\nattachment issues with her Bunnie.", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA3_0 = new String[]{"/[label(Unlucky Man)]", "I still can't find it. The /[color(0x329bbe)]Drill Key/[color(0x808080)]...", "/[waitkey(1)]/[clear()]", "It's okay for now, since none of the doors have been opened yet, but I have to find it soon. It'll get taken...it'll get taken!", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA3_1 = new String[]{"/[label(Unlucky Man)]", "Someone finally found a /[color(0x329bbe)]Drill Key/[color(0x808080)]...! I wonder what was in there. It bothers me so much, I can't sleep at night...", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA3_2 = new String[]{"/[label(Unlucky Man)]", "Two of the doors have been opened...There's only three left.", "/[waitkey(1)]/[clear()]", "...", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA3_3 = new String[]{"/[label(Unlucky Man)]", "What's with you?! Grinning so happily...\n", "/[waitkey(1)]/[clear()]", "Wait a minute, it's you, isn't it?! You're the one who got the /[color(0x329bbe)]Drill Key/[color(0x808080)], aren't you?!", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA3_4 = new String[]{"/[label(Unlucky Man)]", "Only one door left...I've been looking for a long time. I have to get at least one. The other four probably didn't have anything great in them. I'm certain of it.", "/[waitkey(64)]/[close()]"};
    String[] msgGoroA3_5 = new String[]{"/[label(Unlucky Man)]", "Ha ha...all the doors are open. You opened all of them, right? I was right. If only my drilling skills matched yours...", "/[waitkey(64)]/[close()]"};
    String[] msgGoroB3_1 = new String[]{"/[label(Hardcore Driller)]", "I hear a new mission was added to the Mission Mode. The targets are moving this time. Now that's too much, even for me.", "/[waitkey(64)]/[close()]"};
    String[] msgGoroB3_2 = new String[]{"/[label(Hardcore Driller)]", "I've heard that if you clear all the missions, Holgar plans to give that person an awesome reward. Hmmm...he has the most powerful drill. Could he possibly...", "/[waitkey(1)]/[clear()]", "Oh, forget what I was saying...after all, it has nothing to do with you.", "/[waitkey(64)]/[close()]"};
    String[] msgKanben1 = new String[]{"/[label(Holgar)]", "Welcome! One game costs 200G. How about it?", "/[waitkey(64)]/[clear()]"};
    String[] msgKanben2 = new String[]{"/[label(Hollyanna)]", "Um, Sir, it isn't ready yet...", "/[waitkey(64)]/[clear()]"};
    String[] msgKanben3 = new String[]{"/[label(Holgar)]", "What?! How long is it going to take?", "/[waitkey(64)]/[clear()]"};
    String[] msgKanben4 = new String[]{"/[label(Hollyanna)]", "A while, Sir. I feel terrible for our customers, but I hope that they will return later.", "/[waitkey(64)]/[clear()]"};
    String[] msgKanben5 = new String[]{"/[label(Holgar)]", "Hmm...sorry. You heard her. Come back later, okay?", "/[waitkey(64)]/[close()]"};
    String[] msgSize = new String[]{"/[label(Debug Chick)]", "What up, dogg?", "/[waitkey(64)]/[close()]"};
    String[] msgGold_1 = new String[]{"/[label(Debako)]", "I'll hook you up with some bling bling.", "/[waitkey(64)]/[clear()]"};
    String[] msgGold_2 = new String[]{"/[label(Debako)]", "You got 20,000G, G!\n", "(And I lost 20,000G!)", "/[waitkey(64)]/[close()]"};
    String[] msgGold_3 = new String[]{"/[label(Debako)]", "You still have tons of juice, yo!\n", "You don't need it!!", "/[waitkey(64)]/[close()]"};
    String[] msgFlag = new String[]{"/[label(Debako)]", "I hooked you up with the flag so that you met Bunnie in the Encephalon.", "/[waitkey(64)]/[close()]"};
    String[] msgFlag2 = new String[]{"/[label(Debako)]", "I hooked you up with the flags so that you've cleared all the missions.", "/[waitkey(64)]/[close()]"};
    String[] msgDummy = new String[]{"/[label(Hollyanna)]", "The balance and dialogue in the Driler Game are all temporary.", "/[waitkey(64)]/[close()]"};
    String[] msg_D = new String[]{"Obtained result of pattern D. This pattern was not anticipated. Please check the source.", "/[waitkey(64)]/[close()]"};

    ST0160() {
    }

    void After_Drill() {
        this.fade60.call(0);
        System.sleep(60);
        this.SUCCESS = false;
        this.FAILED = true;
        this.mission1 = false;
        this.mission2 = false;
        this.M1_1 = false;
        this.M1_2 = false;
        this.M1_3 = false;
        this.M1_4 = false;
        this.M1_5 = false;
        this.M1_6 = false;
        this.M1_7 = false;
        this.M1_8 = false;
        this.M1_9 = false;
        this.M1_10 = false;
        this.M1_11 = false;
        this.M1_12 = false;
        this.M1_13 = false;
        this.M1_14 = false;
        this.M1_15 = false;
        this.M1_16 = false;
        this.M1_17 = false;
        this.M1_18 = false;
        this.M1_19 = false;
        this.M1_20 = false;
        this.M1_21 = false;
        this.M1_22 = false;
        this.M1_23 = false;
        this.M1_24 = false;
        this.hutuu = false;
        this.hutoi = false;
        this.hosoi = false;
        this.count = 0;
        this.npc3.kickEnepc(4, 0);
        this.npc4.kickEnepc(4, 0);
        this.npc5.kickEnepc(4, 0);
        this.npc6.kickEnepc(4, 0);
        this.npc7.kickEnepc(4, 0);
        this.npc3.setVisible(true);
        this.npc4.setVisible(true);
        this.npc5.setVisible(true);
        this.npc6.setVisible(true);
        this.npc7.setVisible(true);
        this.drill.DrillCommand(8);
        this.drill.DrillCommand(3);
        if (this.mission3) {
            this.mission3 = false;
            this.ukun01.kickEnepc(4, 1);
            this.ukun02.kickEnepc(4, 1);
            this.ukun03.kickEnepc(4, 1);
            this.ukun04.kickEnepc(4, 1);
            this.ukun05.kickEnepc(4, 1);
            this.ukun01.setVisible(false);
            this.ukun02.setVisible(false);
            this.ukun03.setVisible(false);
            this.ukun04.setVisible(false);
            this.ukun05.setVisible(false);
            Stage.setVisible(41, true);
            Stage.setVisible(123, true);
            Stage.setVisible(126, true);
            Stage.setVisible(7, true);
            Stage.setVisible(3, true);
            this.EF01.disp(false);
            this.EF03.disp(false);
            this.EF04.disp(false);
            this.EF05.disp(false);
            this.player.setTranslate(10.454f, 0.0f, -22.403f);
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgGameend2, 0);
            ST0160.waitPage(this.win, 64);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        } else if (this.mission4) {
            this.mission4 = false;
            this.ukun06.kickEnepc(4, 1);
            this.ukun06.setVisible(false);
            Stage.setVisible(41, true);
            Stage.setVisible(123, true);
            Stage.setVisible(126, true);
            Stage.setVisible(7, true);
            Stage.setVisible(3, true);
            this.EF01.disp(false);
            this.EF03.disp(false);
            this.EF04.disp(false);
            this.EF05.disp(false);
            this.player.setTranslate(10.454f, 0.0f, -22.403f);
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgGameend2, 0);
            ST0160.waitPage(this.win, 64);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        } else {
            Stage.setVisible(41, true);
            Stage.setVisible(123, true);
            Stage.setVisible(126, true);
            Stage.setVisible(7, true);
            Stage.setVisible(3, true);
            this.EF01.disp(false);
            this.EF03.disp(false);
            this.EF04.disp(false);
            this.EF05.disp(false);
            this.player.setTranslate(10.454f, 0.0f, -22.403f);
            Runtime.setPlayerControl(false);
            this.win = Window.create();
            this.win.setSize(4, 45);
            this.win.setLocation(15, 305);
            this.win.print(this.msgGameend2, 0);
            ST0160.waitPage(this.win, 64);
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        }
    }

    void EcamD1() {
        this.Ecam = Camera.create(1);
        this.Ecam.setTranslate(-3.5f, 1.7f, -25.0f);
        this.Ecam.setRotate(2.0f, 20.0f, 0.0f);
        this.Ecam.setFov(45.0f);
        this.Ecam.change();
    }

    void EcamD2() {
        this.Ecam = Camera.create(1);
        this.Ecam.setTranslate(-0.7f, 1.7f, -25.0f);
        this.Ecam.setRotate(2.0f, 20.0f, 0.0f);
        this.Ecam.setFov(45.0f);
        this.Ecam.change();
    }

    void EcamD3() {
        this.Ecam = Camera.create(1);
        this.Ecam.setTranslate(0.138f, 1.7f, -25.0f);
        this.Ecam.setRotate(2.0f, 0.0f, 0.0f);
        this.Ecam.setFov(45.0f);
        this.Ecam.change();
    }

    void EcamD4() {
        this.Ecam = Camera.create(1);
        this.Ecam.setTranslate(0.7f, 1.7f, -25.0f);
        this.Ecam.setRotate(2.0f, -20.0f, 0.0f);
        this.Ecam.setFov(45.0f);
        this.Ecam.change();
    }

    void EcamD5() {
        this.Ecam = Camera.create(1);
        this.Ecam.setTranslate(3.5f, 1.7f, -25.0f);
        this.Ecam.setRotate(2.0f, -20.0f, 0.0f);
        this.Ecam.setFov(45.0f);
        this.Ecam.change();
    }

    void Ecam_box() {
        this.Ecam = Camera.create(1);
        this.Ecam.setTranslate(10.891395f, 2.4878771f, -21.426437f);
        this.Ecam.setRotate(-12.899862f, 36.679745f, 0.0f);
        this.Ecam.setFov(45.0f);
        this.Ecam.change();
    }

    void Ecam_tensi() {
        this.Ecam = Camera.create(1);
        this.Ecam.setTranslate(-0.456f, 6.679f, -19.42f);
        this.Ecam.setRotate(-20.311f, 90.0f, 0.0f);
        this.Ecam.setFov(45.0f);
        this.Ecam.change();
    }

    void Final_init(int n) {
    }

    void Freeplay() {
        System.println("フリープレイ(Freeplay)");
        this.count = 0;
        this.timeover = false;
        this.FAILED = false;
        this.SUCCESS = false;
        this.fade60.call(0);
        System.sleep(60);
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.setTranslate(-4.05f, 0.0f, -9.69758f);
        Stage.setVisible(41, false);
        Stage.setVisible(123, false);
        Stage.setVisible(126, false);
        Stage.setVisible(7, false);
        Stage.setVisible(3, false);
        this.npc3.kickEnepc(4, 1);
        this.npc4.kickEnepc(4, 1);
        this.npc5.kickEnepc(4, 1);
        this.npc6.kickEnepc(4, 1);
        this.npc7.kickEnepc(4, 1);
        this.npc3.setVisible(false);
        this.npc4.setVisible(false);
        this.npc5.setVisible(false);
        this.npc6.setVisible(false);
        this.npc7.setVisible(false);
        this.drill.DrillGameCount(5);
        this.drill.DrillSetContainer(18);
        this.drill.DrillCommand(9);
        this.drill.DrillCommand(12);
        this.drill.DrillCommand(11);
        this.drill.SendSignal();
        this.drill.DrillCommand(1);
        this.EF01.disp(true);
        if (this.hutuu) {
            this.EF03.disp(true);
        }
        if (this.hutoi) {
            this.EF04.disp(true);
        }
        if (this.hosoi) {
            this.EF05.disp(true);
        }
        this.EF08 = new Effect(1755, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF08.disp(true);
    }

    public void KickEvent(int var1_1, int var2_2) {
        if (var1_1 != 100) return;
        switch (var2_2) {
            case 0: {
                if (Runtime.getFlags(4061, 1) != 0) return;
                if (Runtime.checkItem(10, 65) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD1();
                    Runtime.setPlayerControl(false);
                    this.doa01.SetDoorType('\u0004');
                    Runtime.removeItem(10, 65);
                    Runtime.setFlags(4061, 1, 1);
                    Stage.setVisible(18, true);
                    Stage.setVisible(99, false);
                    System.sleep(1);
                    this.drill_01.start(1, "Dkaiten01");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 66) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD1();
                    Runtime.setPlayerControl(false);
                    this.doa01.SetDoorType('\u0004');
                    Runtime.removeItem(10, 66);
                    Runtime.setFlags(4061, 1, 1);
                    Stage.setVisible(18, true);
                    Stage.setVisible(99, false);
                    System.sleep(1);
                    this.drill_01.start(1, "Dkaiten01");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 67) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD1();
                    Runtime.setPlayerControl(false);
                    this.doa01.SetDoorType('\u0004');
                    Runtime.removeItem(10, 67);
                    Runtime.setFlags(4061, 1, 1);
                    Stage.setVisible(18, true);
                    Stage.setVisible(99, false);
                    System.sleep(1);
                    this.drill_01.start(1, "Dkaiten01");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 68) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD1();
                    Runtime.setPlayerControl(false);
                    this.doa01.SetDoorType('\u0004');
                    Runtime.removeItem(10, 68);
                    Runtime.setFlags(4061, 1, 1);
                    Stage.setVisible(18, true);
                    Stage.setVisible(99, false);
                    System.sleep(1);
                    this.drill_01.start(1, "Dkaiten01");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 69) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD1();
                    Runtime.setPlayerControl(false);
                    this.doa01.SetDoorType('\u0004');
                    Runtime.removeItem(10, 69);
                    Runtime.setFlags(4061, 1, 1);
                    Stage.setVisible(18, true);
                    Stage.setVisible(99, false);
                    System.sleep(1);
                    this.drill_01.start(1, "Dkaiten01");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 65) != 0) return;
                if (Runtime.checkItem(10, 66) != 0) return;
                if (Runtime.checkItem(10, 67) != 0) return;
                if (Runtime.checkItem(10, 68) != 0) return;
                if (Runtime.checkItem(10, 69) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox1, 0);
                ST0160.waitPage(this.win, 64);
                this.cam0.setMode(-1);
                this.Ecam_box();
                this.npc1.moveEnepc(17, 225.0f, -10.0f, 10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox2, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, 0.0f, 10);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                return;
            }
            case 1: {
                if (Runtime.getFlags(4062, 1) != 0) return;
                if (Runtime.checkItem(10, 65) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD2();
                    Runtime.setPlayerControl(false);
                    this.doa02.SetDoorType('\u0004');
                    Runtime.removeItem(10, 65);
                    Runtime.setFlags(4062, 1, 1);
                    Stage.setVisible(19, true);
                    Stage.setVisible(100, false);
                    System.sleep(1);
                    this.drill_02.start(1, "Dkaiten02");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 66) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD2();
                    Runtime.setPlayerControl(false);
                    this.doa02.SetDoorType('\u0004');
                    Runtime.removeItem(10, 66);
                    Runtime.setFlags(4062, 1, 1);
                    Stage.setVisible(19, true);
                    Stage.setVisible(100, false);
                    System.sleep(1);
                    this.drill_02.start(1, "Dkaiten02");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 67) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD2();
                    Runtime.setPlayerControl(false);
                    this.doa02.SetDoorType('\u0004');
                    Runtime.removeItem(10, 67);
                    Runtime.setFlags(4062, 1, 1);
                    Stage.setVisible(19, true);
                    Stage.setVisible(100, false);
                    System.sleep(1);
                    this.drill_02.start(1, "Dkaiten02");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 68) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD2();
                    Runtime.setPlayerControl(false);
                    this.doa02.SetDoorType('\u0004');
                    Runtime.removeItem(10, 68);
                    Runtime.setFlags(4062, 1, 1);
                    Stage.setVisible(19, true);
                    Stage.setVisible(100, false);
                    System.sleep(1);
                    this.drill_02.start(1, "Dkaiten02");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 69) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD2();
                    Runtime.setPlayerControl(false);
                    this.doa02.SetDoorType('\u0004');
                    Runtime.removeItem(10, 69);
                    Runtime.setFlags(4062, 1, 1);
                    Stage.setVisible(19, true);
                    Stage.setVisible(100, false);
                    System.sleep(1);
                    this.drill_02.start(1, "Dkaiten02");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 65) != 0) return;
                if (Runtime.checkItem(10, 66) != 0) return;
                if (Runtime.checkItem(10, 67) != 0) return;
                if (Runtime.checkItem(10, 68) != 0) return;
                if (Runtime.checkItem(10, 69) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox1, 0);
                ST0160.waitPage(this.win, 64);
                this.cam0.setMode(-1);
                this.Ecam_box();
                this.npc1.moveEnepc(17, 225.0f, -10.0f, 10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox2, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, 0.0f, 10);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                return;
            }
            case 2: {
                if (Runtime.getFlags(4063, 1) != 0) return;
                if (Runtime.checkItem(10, 65) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD3();
                    Runtime.setPlayerControl(false);
                    this.doa03.SetDoorType('\u0004');
                    Runtime.removeItem(10, 65);
                    Runtime.setFlags(4063, 1, 1);
                    Stage.setVisible(20, true);
                    Stage.setVisible(101, false);
                    System.sleep(1);
                    this.drill_03.start(1, "Dkaiten03");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 66) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD3();
                    Runtime.setPlayerControl(false);
                    this.doa03.SetDoorType('\u0004');
                    Runtime.removeItem(10, 66);
                    Runtime.setFlags(4063, 1, 1);
                    Stage.setVisible(20, true);
                    Stage.setVisible(101, false);
                    System.sleep(1);
                    this.drill_03.start(1, "Dkaiten03");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 67) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD3();
                    Runtime.setPlayerControl(false);
                    this.doa03.SetDoorType('\u0004');
                    Runtime.removeItem(10, 67);
                    Runtime.setFlags(4063, 1, 1);
                    Stage.setVisible(20, true);
                    Stage.setVisible(101, false);
                    System.sleep(1);
                    this.drill_01.start(1, "Dkaiten03");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 68) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD3();
                    Runtime.setPlayerControl(false);
                    this.doa03.SetDoorType('\u0004');
                    Runtime.removeItem(10, 68);
                    Runtime.setFlags(4063, 1, 1);
                    Stage.setVisible(20, true);
                    Stage.setVisible(101, false);
                    System.sleep(1);
                    this.drill_03.start(1, "Dkaiten03");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 69) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD3();
                    Runtime.setPlayerControl(false);
                    this.doa03.SetDoorType('\u0004');
                    Runtime.removeItem(10, 69);
                    Runtime.setFlags(4063, 1, 1);
                    Stage.setVisible(20, true);
                    Stage.setVisible(101, false);
                    System.sleep(1);
                    this.drill_03.start(1, "Dkaiten03");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 65) != 0) return;
                if (Runtime.checkItem(10, 66) != 0) return;
                if (Runtime.checkItem(10, 67) != 0) return;
                if (Runtime.checkItem(10, 68) != 0) return;
                if (Runtime.checkItem(10, 69) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox1, 0);
                ST0160.waitPage(this.win, 64);
                this.cam0.setMode(-1);
                this.Ecam_box();
                this.npc1.moveEnepc(17, 225.0f, -10.0f, 10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox2, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, 0.0f, 10);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                return;
            }
            case 3: {
                if (Runtime.getFlags(4064, 1) != 0) return;
                if (Runtime.checkItem(10, 65) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD4();
                    Runtime.setPlayerControl(false);
                    this.doa04.SetDoorType('\u0004');
                    Runtime.removeItem(10, 65);
                    Runtime.setFlags(4064, 1, 1);
                    Stage.setVisible(21, true);
                    Stage.setVisible(102, false);
                    System.sleep(1);
                    this.drill_04.start(1, "Dkaiten04");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 66) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD4();
                    Runtime.setPlayerControl(false);
                    this.doa04.SetDoorType('\u0004');
                    Runtime.removeItem(10, 66);
                    Runtime.setFlags(4064, 1, 1);
                    Stage.setVisible(21, true);
                    Stage.setVisible(102, false);
                    System.sleep(1);
                    this.drill_04.start(1, "Dkaiten04");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 67) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD4();
                    Runtime.setPlayerControl(false);
                    this.doa04.SetDoorType('\u0004');
                    Runtime.removeItem(10, 67);
                    Runtime.setFlags(4064, 1, 1);
                    Stage.setVisible(21, true);
                    Stage.setVisible(102, false);
                    System.sleep(1);
                    this.drill_04.start(1, "Dkaiten04");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 68) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD4();
                    Runtime.setPlayerControl(false);
                    this.doa04.SetDoorType('\u0004');
                    Runtime.removeItem(10, 68);
                    Runtime.setFlags(4064, 1, 1);
                    Stage.setVisible(21, true);
                    Stage.setVisible(102, false);
                    System.sleep(1);
                    this.drill_04.start(1, "Dkaiten04");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 69) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD4();
                    Runtime.setPlayerControl(false);
                    this.doa04.SetDoorType('\u0004');
                    Runtime.removeItem(10, 69);
                    Runtime.setFlags(4064, 1, 1);
                    Stage.setVisible(21, true);
                    Stage.setVisible(102, false);
                    System.sleep(1);
                    this.drill_04.start(1, "Dkaiten04");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 65) != 0) return;
                if (Runtime.checkItem(10, 66) != 0) return;
                if (Runtime.checkItem(10, 67) != 0) return;
                if (Runtime.checkItem(10, 68) != 0) return;
                if (Runtime.checkItem(10, 69) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox1, 0);
                ST0160.waitPage(this.win, 64);
                this.cam0.setMode(-1);
                this.Ecam_box();
                this.npc1.moveEnepc(17, 225.0f, -10.0f, 10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox2, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, 0.0f, 10);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                return;
            }
            case 4: {
                if (Runtime.getFlags(4065, 1) != 0) return;
                if (Runtime.checkItem(10, 65) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD5();
                    Runtime.setPlayerControl(false);
                    this.doa05.SetDoorType('\u0004');
                    Runtime.removeItem(10, 65);
                    Runtime.setFlags(4065, 1, 1);
                    Stage.setVisible(22, true);
                    Stage.setVisible(103, false);
                    System.sleep(1);
                    this.drill_05.start(1, "Dkaiten05");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 66) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD5();
                    Runtime.setPlayerControl(false);
                    this.doa05.SetDoorType('\u0004');
                    Runtime.removeItem(10, 66);
                    Runtime.setFlags(4065, 1, 1);
                    Stage.setVisible(22, true);
                    Stage.setVisible(103, false);
                    System.sleep(1);
                    this.drill_05.start(1, "Dkaiten05");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 67) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD5();
                    Runtime.setPlayerControl(false);
                    this.doa05.SetDoorType('\u0004');
                    Runtime.removeItem(10, 67);
                    Runtime.setFlags(4065, 1, 1);
                    Stage.setVisible(22, true);
                    Stage.setVisible(103, false);
                    System.sleep(1);
                    this.drill_05.start(1, "Dkaiten05");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 68) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD5();
                    Runtime.setPlayerControl(false);
                    this.doa05.SetDoorType('\u0004');
                    Runtime.removeItem(10, 68);
                    Runtime.setFlags(4065, 1, 1);
                    Stage.setVisible(22, true);
                    Stage.setVisible(103, false);
                    System.sleep(1);
                    this.drill_05.start(1, "Dkaiten05");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 69) != 0) {
                    this.cam0.setMode(-1);
                    this.EcamD5();
                    Runtime.setPlayerControl(false);
                    this.doa05.SetDoorType('\u0004');
                    Runtime.removeItem(10, 69);
                    Runtime.setFlags(4065, 1, 1);
                    Stage.setVisible(22, true);
                    Stage.setVisible(103, false);
                    System.sleep(1);
                    this.drill_05.start(1, "Dkaiten05");
                    System.sleep(60);
                    this.cam0.setMode(0);
                    Runtime.setPlayerControl(true);
                    return;
                }
                if (Runtime.checkItem(10, 65) != 0) return;
                if (Runtime.checkItem(10, 66) != 0) return;
                if (Runtime.checkItem(10, 67) != 0) return;
                if (Runtime.checkItem(10, 68) != 0) return;
                if (Runtime.checkItem(10, 69) != 0) return;
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox1, 0);
                ST0160.waitPage(this.win, 64);
                this.cam0.setMode(-1);
                this.Ecam_box();
                this.npc1.moveEnepc(17, 225.0f, -10.0f, 10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgBox2, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, 0.0f, 10);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                return;
            }
            case 5: {
                Runtime.setPlayerControl(false);
                this.cam0.setMode(-1);
                this.Ecam_tensi();
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                if (this.tensibroken) **GOTO lbl609
                this.win.print(this.msgTensi, 0);
                ST0160.waitPage(this.win, 64);
                System.waitFor(this.win);
                this.menu = Menu.create();
                this.menu.addItem("Slow rotation\nNormal rotation\nFast rotation\nSuper fast rotation");
                System.waitFor(this.menu);
                this.selected = this.menu.getSelected();
                switch (this.selected) {
                    case 0: {
                        this.ang2 = 0.5f;
                        this.tensi.start(1, "Kaiten1");
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgTensi1, 0);
                        ST0160.waitPage(this.win, 64);
                        break;
                    }
                    case 1: {
                        this.ang2 = 1.5f;
                        this.tensi.start(1, "Kaiten1");
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgTensi1, 0);
                        ST0160.waitPage(this.win, 64);
                        break;
                    }
                    case 2: {
                        this.ang2 = 15.0f;
                        this.tensi.start(1, "Kaiten1");
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgTensi1, 0);
                        ST0160.waitPage(this.win, 64);
                        break;
                    }
                    case 3: {
                        this.ang2 = 50.0f;
                        this.tensi.start(1, "Kaiten1");
                        this.win = Window.create();
                        this.win.setSize(4, 45);
                        this.win.setLocation(15, 305);
                        this.win.print(this.msgTensi3, 0);
                        ST0160.waitPage(this.win, 64);
                        this.win.print(this.msgTensi4, 0);
                        ST0160.waitPage(this.win, 64);
                        this.win.print(this.msgTensi5, 0);
                        ST0160.waitPage(this.win, 64);
                        this.posT2 = 0.05f;
                        this.tensi.start(1, "Kaiten1");
                        System.sleep(45);
                        Sound.effectPlay(65542);
                        this.EFtensi = new Effect(601, -10.0f, 2.0f, -19.5f, 0.0f);
                        this.EFtensi.disp(true);
                        Stage.setVisible(135, false);
                        this.tensibroken = true;
                        break;
                    }
                    lbl609:

                    if (!this.tensibroken) break;
                    this.win.print(this.msgTensi6, 0);
                    ST0160.waitPage(this.win, 64);
                    break;
                }
                this.cam0.setMode(0);
                Runtime.enable(65536);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(30, -5.906f, -18.985f, true);
                this.player.setRotate(0.0f, 270.0f, 0.0f);
                System.sleep(30);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                return;
            }
            case 6: {
                if (Runtime.getFlags(4081, 1) != 0) return;
                if (Runtime.checkItem(10, 65) == 0 && Runtime.checkItem(10, 66) == 0 && Runtime.checkItem(10, 67) == 0 && Runtime.checkItem(10, 68) == 0) {
                    if (Runtime.checkItem(10, 69) == 0) return;
                }
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.npc1.moveEnepc(17, 315.0f, 0.0f, 10);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgStop, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, 0.0f, 10);
                this.player.mtn(2, 9, 1.0f, true);
                this.player.move(60, 5.765f, -19.439f, true);
                this.player.setRotate(0.0f, 270.0f, 0.0f);
                System.sleep(60);
                Runtime.disable(65536);
                Runtime.setPlayerControl(true);
                Runtime.setFlags(4081, 1, 1);
                return;
            }
        }
    }

    void M1_result() {
        if (!this.YET) {
            this.YET = true;
            System.println("ミッション成功結果処理(SUCCESS == true)");
            this.fade60.call(0);
            System.sleep(60);
            this.mission1 = false;
            this.hutuu = false;
            this.hutoi = false;
            this.hosoi = false;
            this.M1_1 = false;
            this.M1_2 = false;
            this.M1_3 = false;
            this.M1_4 = false;
            this.M1_5 = false;
            this.M1_6 = false;
            this.M1_7 = false;
            this.M1_8 = false;
            this.M1_9 = false;
            this.M1_10 = false;
            this.M1_11 = false;
            this.M1_12 = false;
            this.M1_13 = false;
            this.M1_14 = false;
            this.M1_15 = false;
            this.npc3.kickEnepc(4, 0);
            this.npc4.kickEnepc(4, 0);
            this.npc5.kickEnepc(4, 0);
            this.npc6.kickEnepc(4, 0);
            this.npc7.kickEnepc(4, 0);
            this.npc3.setVisible(true);
            this.npc4.setVisible(true);
            this.npc5.setVisible(true);
            this.npc6.setVisible(true);
            this.npc7.setVisible(true);
            this.drill.DrillCommand(8);
            this.drill.DrillCommand(3);
            Stage.setVisible(41, true);
            Stage.setVisible(123, true);
            Stage.setVisible(126, true);
            Stage.setVisible(7, true);
            Stage.setVisible(3, true);
            this.EF01.disp(false);
            this.EF03.disp(false);
            this.EF04.disp(false);
            this.EF05.disp(false);
            this.player.setTranslate(10.454f, 0.0f, -22.403f);
            if (Runtime.getFlags(4075, 1) != 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgAgain1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgAgain2, 0);
                ST0160.waitPage(this.win, 64);
                if (Runtime.getFlags(115, 1) != 0 && Runtime.getFlags(301, 1) != 0) {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #822/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    ST0160.waitPage(this.win, 64);
                } else {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #821/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    ST0160.waitPage(this.win, 64);
                }
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgM1_1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgM1_2, 0);
                ST0160.waitPage(this.win, 64);
                Sound.effectPlay(6);
                this.win.print("Obtained Speed Stim DX!/[waitkey(64)]/[close()]");
                Runtime.addItem(0, 23);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4075, 1, 1);
            }
            if (Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) != 0 && Runtime.getFlags(4079, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgConp1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgConp2, 0);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4079, 1, 1);
            }
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        } else {
            System.println("二回目はなしだよ");
        }
    }

    void M2_result() {
        if (!this.YET) {
            this.YET = true;
            this.fade60.call(0);
            System.sleep(60);
            this.mission2 = false;
            this.hutuu = false;
            this.hutoi = false;
            this.hosoi = false;
            this.M1_16 = false;
            this.M1_17 = false;
            this.M1_18 = false;
            this.M1_19 = false;
            this.M1_20 = false;
            this.M1_21 = false;
            this.M1_22 = false;
            this.M1_23 = false;
            this.M1_24 = false;
            this.npc3.kickEnepc(4, 0);
            this.npc4.kickEnepc(4, 0);
            this.npc5.kickEnepc(4, 0);
            this.npc6.kickEnepc(4, 0);
            this.npc7.kickEnepc(4, 0);
            this.npc3.setVisible(true);
            this.npc4.setVisible(true);
            this.npc5.setVisible(true);
            this.npc6.setVisible(true);
            this.npc7.setVisible(true);
            this.drill.DrillCommand(8);
            this.drill.DrillCommand(3);
            Stage.setVisible(41, true);
            Stage.setVisible(123, true);
            Stage.setVisible(126, true);
            Stage.setVisible(7, true);
            Stage.setVisible(3, true);
            this.EF01.disp(false);
            this.EF03.disp(false);
            this.EF04.disp(false);
            this.EF05.disp(false);
            this.player.setTranslate(10.454f, 0.0f, -22.403f);
            if (Runtime.getFlags(4076, 1) != 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgAgain1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgAgain2, 0);
                ST0160.waitPage(this.win, 64);
                if (Runtime.getFlags(115, 1) != 0 && Runtime.getFlags(301, 1) != 0) {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #822/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    ST0160.waitPage(this.win, 64);
                } else {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #821/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    ST0160.waitPage(this.win, 64);
                }
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgM2_1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgM2_2, 0);
                ST0160.waitPage(this.win, 64);
                Sound.effectPlay(6);
                this.win.print("Obtained Yamato Belt!/[waitkey(64)]/[close()]");
                Runtime.addItem(3, 95);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4076, 1, 1);
            }
            if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) != 0 && Runtime.getFlags(4079, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgConp1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgConp2, 0);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4079, 1, 1);
            }
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        } else {
            System.println("二回目はなしだよ");
        }
    }

    void M3_result() {
        if (!this.YET) {
            this.YET = true;
            this.fade60.call(0);
            System.sleep(60);
            this.mission3 = false;
            this.hutuu = false;
            this.hutoi = false;
            this.hosoi = false;
            this.ukun01.kickEnepc(4, 1);
            this.ukun02.kickEnepc(4, 1);
            this.ukun03.kickEnepc(4, 1);
            this.ukun04.kickEnepc(4, 1);
            this.ukun05.kickEnepc(4, 1);
            this.ukun01.setVisible(false);
            this.ukun02.setVisible(false);
            this.ukun03.setVisible(false);
            this.ukun04.setVisible(false);
            this.ukun05.setVisible(false);
            this.npc3.kickEnepc(4, 0);
            this.npc4.kickEnepc(4, 0);
            this.npc5.kickEnepc(4, 0);
            this.npc6.kickEnepc(4, 0);
            this.npc7.kickEnepc(4, 0);
            this.npc3.setVisible(true);
            this.npc4.setVisible(true);
            this.npc5.setVisible(true);
            this.npc6.setVisible(true);
            this.npc7.setVisible(true);
            this.drill.DrillCommand(8);
            this.drill.DrillCommand(3);
            Stage.setVisible(41, true);
            Stage.setVisible(123, true);
            Stage.setVisible(126, true);
            Stage.setVisible(7, true);
            Stage.setVisible(3, true);
            this.EF01.disp(false);
            this.EF03.disp(false);
            this.EF04.disp(false);
            this.EF05.disp(false);
            this.player.setTranslate(10.454f, 0.0f, -22.403f);
            if (Runtime.getFlags(4077, 1) != 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgAgain1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgAgain2, 0);
                ST0160.waitPage(this.win, 64);
                if (Runtime.getFlags(115, 1) != 0 && Runtime.getFlags(301, 1) != 0) {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #822/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    ST0160.waitPage(this.win, 64);
                } else {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #821/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    ST0160.waitPage(this.win, 64);
                }
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgM3_1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgM3_2, 0);
                ST0160.waitPage(this.win, 64);
                Sound.effectPlay(6);
                this.win.print("Obtained Hunter Goggle!/[waitkey(64)]/[close()]");
                Runtime.addItem(3, 92);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4077, 1, 1);
            }
            if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4078, 1) != 0 && Runtime.getFlags(4079, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgConp1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgConp2, 0);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4079, 1, 1);
            }
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        } else {
            System.println("二回目はなしだよ");
        }
    }

    void M4_result() {
        if (!this.YET) {
            this.YET = true;
            this.fade60.call(0);
            System.sleep(60);
            this.mission4 = false;
            this.hutuu = false;
            this.hutoi = false;
            this.hosoi = false;
            this.ukun06.kickEnepc(4, 1);
            this.ukun06.setVisible(false);
            this.npc3.kickEnepc(4, 0);
            this.npc4.kickEnepc(4, 0);
            this.npc5.kickEnepc(4, 0);
            this.npc6.kickEnepc(4, 0);
            this.npc7.kickEnepc(4, 0);
            this.npc3.setVisible(true);
            this.npc4.setVisible(true);
            this.npc5.setVisible(true);
            this.npc6.setVisible(true);
            this.npc7.setVisible(true);
            this.drill.DrillCommand(8);
            this.drill.DrillCommand(3);
            Stage.setVisible(41, true);
            Stage.setVisible(123, true);
            Stage.setVisible(126, true);
            Stage.setVisible(7, true);
            Stage.setVisible(3, true);
            this.EF01.disp(false);
            this.EF03.disp(false);
            this.EF04.disp(false);
            this.EF05.disp(false);
            this.player.setTranslate(10.454f, 0.0f, -22.403f);
            if (Runtime.getFlags(4078, 1) != 0) {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgAgain1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgAgain2, 0);
                ST0160.waitPage(this.win, 64);
                if (Runtime.getFlags(115, 1) != 0 && Runtime.getFlags(301, 1) != 0) {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #822/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    Runtime.addItem(10, 55);
                    ST0160.waitPage(this.win, 64);
                } else {
                    Sound.effectPlay(6);
                    this.win.print("Obtained 5 /[color(0x329bbe)]Card Pack #821/[color(0x808080)]./[waitkey(64)]/[close()]");
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    Runtime.addItem(10, 54);
                    ST0160.waitPage(this.win, 64);
                }
            } else {
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgM4_1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgM4_2, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 45.0f, 10.0f, 10);
                this.win.print(this.msgM4_3, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                this.npc2.moveEnepc(17, 315.0f, -10.0f, 10);
                this.win.print(this.msgM4_4, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgM4_5, 0);
                ST0160.waitPage(this.win, 64);
                this.npc2.moveEnepc(17, 270.0f, -10.0f, 10);
                this.win.print(this.msgM4_6, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 45.0f, 10.0f, 10);
                this.win.print(this.msgM4_7, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgM4_8, 0);
                ST0160.waitPage(this.win, 64);
                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                this.win.print(this.msgM4_9, 0);
                ST0160.waitPage(this.win, 64);
                this.npc2.moveEnepc(17, 359.9f, -10.0f, 10);
                this.win.print(this.msgM4_10, 0);
                ST0160.waitPage(this.win, 64);
                Sound.effectPlay(6);
                this.win.print("Obtained Swimsuit!/[waitkey(64)]/[close()]");
                Runtime.addItem(3, 45);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4078, 1, 1);
            }
            if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4079, 1) == 0) {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgConp1, 0);
                ST0160.waitPage(this.win, 64);
                this.win.print(this.msgConp2, 0);
                ST0160.waitPage(this.win, 64);
                Runtime.setFlags(4079, 1, 1);
            }
            Runtime.setPlayerControl(true);
            Runtime.disable(65536);
        } else {
            System.println("二回目はなしだよ");
        }
    }

    void Mission1() {
        System.println("ミッション１(Mission1)");
        this.timeover = false;
        this.mission1 = true;
        this.FAILED = false;
        this.SUCCESS = false;
        this.YET = false;
        this.fade60.call(0);
        System.sleep(60);
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.setTranslate(-4.05f, 0.0f, -9.69758f);
        this.M1_1 = false;
        this.M1_2 = false;
        this.M1_3 = false;
        this.M1_4 = false;
        this.M1_5 = false;
        this.M1_6 = false;
        this.M1_7 = false;
        this.M1_8 = false;
        this.M1_9 = false;
        this.M1_10 = false;
        this.M1_11 = false;
        this.M1_12 = false;
        this.M1_13 = false;
        this.M1_14 = false;
        this.M1_15 = false;
        Stage.setVisible(41, false);
        Stage.setVisible(123, false);
        Stage.setVisible(126, false);
        Stage.setVisible(7, false);
        Stage.setVisible(3, false);
        this.npc3.kickEnepc(4, 1);
        this.npc4.kickEnepc(4, 1);
        this.npc5.kickEnepc(4, 1);
        this.npc6.kickEnepc(4, 1);
        this.npc7.kickEnepc(4, 1);
        this.npc3.setVisible(false);
        this.npc4.setVisible(false);
        this.npc5.setVisible(false);
        this.npc6.setVisible(false);
        this.npc7.setVisible(false);
        this.drill.DrillGameCount(-1);
        this.drill.DrillCommand(1);
        this.drill.DrillCommand(10);
        this.drill.DrillSetTimeLimit(2400);
        this.drill.DrillCommand(13);
        this.EF01.disp(true);
        if (this.hutuu) {
            this.EF03.disp(true);
        }
        if (this.hutoi) {
            this.EF04.disp(true);
        }
        if (this.hosoi) {
            this.EF05.disp(true);
        }
        this.EF08 = new Effect(1755, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF08.disp(true);
        this.kon1 = new Uwamono(28708, -1.0f, 0.0f, -22.0f, 0.0f);
        this.kon1.SetParticle(601);
        this.kon1.SetBroken(true);
        this.kon1.SetCallNo(1);
        this.kon2 = new Uwamono(28708, -1.5f, 0.0f, -20.0f, 0.0f);
        this.kon2.SetParticle(601);
        this.kon2.SetCallNo(2);
        this.kon3 = new Uwamono(28708, -1.0f, 0.0f, -18.0f, 0.0f);
        this.kon3.SetParticle(601);
        this.kon3.SetCallNo(3);
        this.kon4 = new Uwamono(28708, -1.5f, 0.0f, -16.0f, 0.0f);
        this.kon4.SetParticle(601);
        this.kon4.SetCallNo(4);
        this.kon5 = new Uwamono(28709, -1.0f, 0.0f, -14.0f, 0.0f);
        this.kon5.SetParticle(601);
        this.kon5.SetCallNo(5);
        this.kon6 = new Uwamono(28709, 1.5f, 0.0f, -22.0f, 0.0f);
        this.kon6.SetParticle(601);
        this.kon6.SetCallNo(6);
        this.kon7 = new Uwamono(28709, 1.0f, 0.0f, -20.0f, 0.0f);
        this.kon7.SetParticle(601);
        this.kon7.SetCallNo(7);
        this.kon8 = new Uwamono(28709, 1.5f, 0.0f, -18.0f, 0.0f);
        this.kon8.SetParticle(601);
        this.kon8.SetCallNo(8);
        this.kon9 = new Uwamono(28710, 1.0f, 0.0f, -16.0f, 0.0f);
        this.kon9.SetParticle(601);
        this.kon9.SetCallNo(9);
        this.kon10 = new Uwamono(28710, 1.5f, 0.0f, -14.0f, 0.0f);
        this.kon10.SetParticle(601);
        this.kon10.SetCallNo(10);
        this.kon11 = new Uwamono(28710, 3.5f, 0.0f, -22.0f, 0.0f);
        this.kon11.SetParticle(601);
        this.kon11.SetCallNo(11);
        this.kon12 = new Uwamono(28710, 3.0f, 0.0f, -20.0f, 0.0f);
        this.kon12.SetParticle(601);
        this.kon12.SetCallNo(12);
        this.kon13 = new Uwamono(28711, 3.5f, 0.0f, -18.0f, 0.0f);
        this.kon13.SetParticle(601);
        this.kon13.SetCallNo(13);
        this.kon14 = new Uwamono(28711, 3.0f, 0.0f, -16.0f, 0.0f);
        this.kon14.SetParticle(601);
        this.kon14.SetCallNo(14);
        this.kon15 = new Uwamono(28711, 3.5f, 0.0f, -14.0f, 0.0f);
        this.kon15.SetParticle(601);
        this.kon15.SetCallNo(15);
    }

    void Mission2() {
        System.println("ミッション２(Mission2)");
        this.timeover = false;
        this.mission2 = true;
        this.FAILED = false;
        this.SUCCESS = false;
        this.YET = false;
        this.fade60.call(0);
        System.sleep(60);
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.setTranslate(-4.05f, 0.0f, -9.69758f);
        this.M1_16 = false;
        this.M1_17 = false;
        this.M1_18 = false;
        this.M1_19 = false;
        this.M1_20 = false;
        this.M1_21 = false;
        this.M1_22 = false;
        this.M1_23 = false;
        this.M1_24 = false;
        Stage.setVisible(41, false);
        Stage.setVisible(123, false);
        Stage.setVisible(126, false);
        Stage.setVisible(7, false);
        Stage.setVisible(3, false);
        this.npc3.kickEnepc(4, 1);
        this.npc4.kickEnepc(4, 1);
        this.npc5.kickEnepc(4, 1);
        this.npc6.kickEnepc(4, 1);
        this.npc7.kickEnepc(4, 1);
        this.npc3.setVisible(false);
        this.npc4.setVisible(false);
        this.npc5.setVisible(false);
        this.npc6.setVisible(false);
        this.npc7.setVisible(false);
        this.drill.DrillGameCount(-1);
        this.drill.DrillCommand(1);
        this.drill.DrillCommand(10);
        this.drill.DrillSetTimeLimit(3600);
        this.drill.DrillCommand(13);
        this.EF01.disp(true);
        if (this.hutuu) {
            this.EF03.disp(true);
        }
        if (this.hutoi) {
            this.EF04.disp(true);
        }
        if (this.hosoi) {
            this.EF05.disp(true);
        }
        this.EF08 = new Effect(1755, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF08.disp(true);
        this.kon16 = new Uwamono(28692, 0.0f, 0.0f, -21.0f, 0.0f);
        this.kon16.SetParticle(601);
        this.kon16.SetBroken(true);
        this.kon16.SetCallNo(16);
        this.kon17 = new Uwamono(28693, 0.0f, 0.0f, -15.0f, 0.0f);
        this.kon17.SetParticle(601);
        this.kon17.SetCallNo(17);
        this.kon18 = new Uwamono(28694, 1.0f, 0.0f, -19.0f, 0.0f);
        this.kon18.SetParticle(601);
        this.kon18.SetCallNo(18);
        this.kon19 = new Uwamono(28695, 1.0f, 0.0f, -17.0f, 0.0f);
        this.kon19.SetParticle(601);
        this.kon19.SetCallNo(19);
        this.kon20 = new Uwamono(28692, 2.0f, 0.0f, -18.0f, 0.0f);
        this.kon20.SetParticle(601);
        this.kon20.SetCallNo(20);
        this.kon21 = new Uwamono(28693, 3.0f, 0.0f, -19.0f, 0.0f);
        this.kon21.SetParticle(601);
        this.kon21.SetCallNo(21);
        this.kon22 = new Uwamono(28694, 3.0f, 0.0f, -17.0f, 0.0f);
        this.kon22.SetParticle(601);
        this.kon22.SetCallNo(22);
        this.kon23 = new Uwamono(28695, 4.0f, 0.0f, -21.0f, 0.0f);
        this.kon23.SetParticle(601);
        this.kon23.SetCallNo(23);
        this.kon24 = new Uwamono(28692, 4.0f, 0.0f, -15.0f, 0.0f);
        this.kon24.SetParticle(601);
        this.kon24.SetCallNo(24);
    }

    void Mission3() {
        System.println("ミッション３(Mission3)");
        this.fade60.call(0);
        System.sleep(60);
        this.timeover = false;
        this.mission3 = true;
        this.FAILED = false;
        this.SUCCESS = false;
        this.YET = false;
        this.count = 0;
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.setTranslate(-4.05f, 0.0f, -9.69758f);
        Stage.setVisible(41, false);
        Stage.setVisible(123, false);
        Stage.setVisible(126, false);
        Stage.setVisible(7, false);
        Stage.setVisible(3, false);
        this.npc3.kickEnepc(4, 1);
        this.npc4.kickEnepc(4, 1);
        this.npc5.kickEnepc(4, 1);
        this.npc6.kickEnepc(4, 1);
        this.npc7.kickEnepc(4, 1);
        this.npc3.setVisible(false);
        this.npc4.setVisible(false);
        this.npc5.setVisible(false);
        this.npc6.setVisible(false);
        this.npc7.setVisible(false);
        this.EF01.disp(true);
        if (this.hutuu) {
            this.EF03.disp(true);
        }
        if (this.hutoi) {
            this.EF04.disp(true);
        }
        if (this.hosoi) {
            this.EF05.disp(true);
        }
        this.EF08 = new Effect(1755, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF08.disp(true);
        this.ukun01.kickEnepc(4, 0);
        this.ukun02.kickEnepc(4, 0);
        this.ukun03.kickEnepc(4, 0);
        this.ukun04.kickEnepc(4, 0);
        this.ukun05.kickEnepc(4, 0);
        this.ukun01.setVisible(true);
        this.ukun02.setVisible(true);
        this.ukun03.setVisible(true);
        this.ukun04.setVisible(true);
        this.ukun05.setVisible(true);
        this.drill.DrillGameCount(-1);
        this.drill.DrillCommand(1);
        this.drill.DrillCommand(10);
        this.drill.DrillSetTimeLimit(2700);
        this.drill.DrillCommand(13);
    }

    void Mission4() {
        System.println("ミッション３(Mission3)");
        this.fade60.call(0);
        System.sleep(60);
        this.timeover = false;
        this.mission4 = true;
        this.FAILED = false;
        this.SUCCESS = false;
        this.YET = false;
        this.count = 0;
        Runtime.setPlayerControl(false);
        Runtime.enable(65536);
        this.player.setTranslate(-4.05f, 0.0f, -9.69758f);
        Stage.setVisible(41, false);
        Stage.setVisible(123, false);
        Stage.setVisible(126, false);
        Stage.setVisible(7, false);
        Stage.setVisible(3, false);
        this.npc3.kickEnepc(4, 1);
        this.npc4.kickEnepc(4, 1);
        this.npc5.kickEnepc(4, 1);
        this.npc6.kickEnepc(4, 1);
        this.npc7.kickEnepc(4, 1);
        this.npc3.setVisible(false);
        this.npc4.setVisible(false);
        this.npc5.setVisible(false);
        this.npc6.setVisible(false);
        this.npc7.setVisible(false);
        this.EF01.disp(true);
        if (this.hutuu) {
            this.EF03.disp(true);
        }
        if (this.hutoi) {
            this.EF04.disp(true);
        }
        if (this.hosoi) {
            this.EF05.disp(true);
        }
        this.EF08 = new Effect(1755, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF08.disp(true);
        this.ukun06.kickEnepc(4, 0);
        this.ukun06.setVisible(true);
        this.drill.DrillGameCount(-1);
        this.drill.DrillCommand(1);
        this.drill.DrillCommand(10);
        this.drill.DrillSetTimeLimit(1500);
        this.drill.DrillCommand(13);
    }

    void Result_D() {
        this.player.setTranslate(10.564f, 0.0f, -16.992f);
        Runtime.disable(65536);
        this.win = Window.create();
        this.win.print(this.msg_D, 0);
        ST0160.waitPage(this.win, 64);
    }

    void Result_free() {
        System.println("結果処理(Result_free)");
        Runtime.setPlayerControl(false);
        this.player.setTranslate(10.564f, 0.0f, -16.992f);
        Runtime.disable(65536);
        Runtime.setPlayerControl(true);
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (Runtime.getFlags(4071, 1) == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    void Talk_npc1_1(Window window) {
        window.print(this.msgFirst_M, 0);
        ST0160.waitPage(window, 64);
        window.print(this.msgFirst_M1, 0);
        ST0160.waitPage(window, 64);
        window.print(this.msgFirst_M2, 0);
        ST0160.waitPage(window, 64);
        window.print(this.msgFirst_M3, 0);
        ST0160.waitPage(window, 64);
        Runtime.setFlags(4071, 1, 1);
        this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
    }

    void Talk_npc1_2(Window window) {
        if (this.play == 5) {
            window.print(this.msgKanben1, 0);
            ST0160.waitPage(window, 64);
            this.npc2.moveEnepc(17, 270.0f, -10.0f, 10);
            window.print(this.msgKanben2, 0);
            ST0160.waitPage(window, 64);
            window.print(this.msgKanben3, 0);
            ST0160.waitPage(window, 64);
            window.print(this.msgKanben4, 0);
            ST0160.waitPage(window, 64);
            this.npc2.moveEnepc(17, 359.9f, -10.0f, 10);
            window.print(this.msgKanben5, 0);
            ST0160.waitPage(window, 64);
        } else {
            window.print(this.msgD01_1, 0);
            ST0160.waitPage(window, 64);
            System.waitFor(window);
            this.menu = Menu.create();
            this.menu.addItem("Yes\nNo");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            block0:
            switch (this.selected) {
                case 0: {
                    if (Runtime.checkGold() < 200) {
                        window = Window.create();
                        window.setSize(4, 45);
                        window.setLocation(15, 305);
                        window.print(this.msgD01_4, 0);
                        ST0160.waitPage(window, 64);
                        this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                        break;
                    }
                    if (Runtime.getFlags(8059, 1) != 0) {
                        window = Window.create();
                        window.setSize(4, 45);
                        window.setLocation(15, 305);
                        window.print(this.msgD01_2, 0);
                        ST0160.waitPage(window, 64);
                        System.waitFor(window);
                        this.menu = Menu.create();
                        if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) == 0 && Runtime.getFlags(4077, 1) == 0 && Runtime.getFlags(4078, 1) == 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\nMISSION 2\nMISSION 3\nMISSION 4");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) == 0 && Runtime.getFlags(4078, 1) == 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\nMISSION 3\nMISSION 4");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) == 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) == 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\nMISSION 2\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\nMISSION 4");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) == 0 && Runtime.getFlags(4077, 1) == 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\nMISSION 2\nMISSION 3\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) == 0 && Runtime.getFlags(4078, 1) == 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\nMISSION 3\nMISSION 4");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) == 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) == 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\nMISSION 2\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\nMISSION 4");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) == 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\nMISSION 2\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) == 0 && Runtime.getFlags(4077, 1) == 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\nMISSION 2\nMISSION 3\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) == 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\nMISSION 4");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) == 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\nMISSION 3\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)]\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) == 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\nMISSION 4");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) == 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\nMISSION 3\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) != 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\n/[color(0xfa3232)]MISSION 2/[color(0x808080)]\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) == 0 && Runtime.getFlags(4077, 1) != 0 && Runtime.getFlags(4078, 1) != 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\nMISSION 2\n/[color(0xfa3232)]MISSION 3/[color(0x808080)]\n/[color(0xfa3232)]MISSION 4/[color(0x808080)]");
                        } else {
                            this.menu.addItem("Free Mode\nMISSION 1\nMISSION 2\nMISSION 3\nMISSION 4");
                        }
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        block4:
                        switch (this.selected) {
                            case 0: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                window.print(this.msgFsetumei, 0);
                                ST0160.waitPage(window, 64);
                                while (this.setumei) {
                                    block79:
                                    while (true) {
                                        System.waitFor(window);
                                        this.menu = Menu.create();
                                        this.menu.addItem("Start game\nHear instructions\nForget it");
                                        System.waitFor(this.menu);
                                        this.selected = this.menu.getSelected();
                                        switch (this.selected) {
                                            case 0: {
                                                if (Runtime.getFlags(4079, 1) != 0) {
                                                    window = Window.create();
                                                    window.setSize(4, 45);
                                                    window.setLocation(15, 305);
                                                    window.print(this.msgDrillSelect1, 0);
                                                    ST0160.waitPage(window, 64);
                                                    System.waitFor(window);
                                                    this.menu = Menu.create();
                                                    this.menu.addItem("Normal drill\nMost powerful drill");
                                                    System.waitFor(this.menu);
                                                    this.selected = this.menu.getSelected();
                                                    switch (this.selected) {
                                                        case 0: {
                                                            this.hutuu = true;
                                                            window = Window.create();
                                                            window.setSize(4, 45);
                                                            window.setLocation(15, 305);
                                                            window.print(this.msgDrillSelectM, 0);
                                                            ST0160.waitPage(window, 64);
                                                            this.drill.DrillSetDrillSize(0.15f);
                                                            Stage.setVisible(115, true);
                                                            Stage.setVisible(133, false);
                                                            Stage.setVisible(134, false);
                                                            Runtime.removeGold(200);
                                                            this.drill.DrillCommand(9);
                                                            this.Freeplay();
                                                            ++this.play;
                                                            return;
                                                        }
                                                        case 1: {
                                                            this.mau = Math.random();
                                                            this.mau = (this.mau & 0x7FFFFFF) % 4;
                                                            if (this.mau < 0) {
                                                                this.mau *= -1;
                                                            }
                                                            Runtime.setRegister(0, this.mau);
                                                            System.println("mauの値は: /[$0]");
                                                            if (this.mau == 0) {
                                                                this.hosoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectS, 0);
                                                                ST0160.waitPage(window, 64);
                                                                window.print(this.msgDrillSelectS2, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(0.05f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, false);
                                                                Stage.setVisible(134, true);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                this.Freeplay();
                                                                ++this.play;
                                                            } else {
                                                                this.hutoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectL, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(1.0f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, true);
                                                                Stage.setVisible(134, false);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                this.Freeplay();
                                                                ++this.play;
                                                            }
                                                            return;
                                                        }
                                                        default: {
                                                            if (this.setumei) continue block79;
                                                        }
                                                    }
                                                }
                                                Runtime.removeGold(200);
                                                this.drill.DrillCommand(9);
                                                this.Freeplay();
                                                ++this.play;
                                                return;
                                            }
                                            case 1: {
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgFsetumei1, 0);
                                                ST0160.waitPage(window, 64);
                                                break block79;
                                            }
                                            case 2: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block79;
                                            }
                                            default: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block79;
                                            }
                                        }
                                        break;
                                    }
                                    continue;
                                    break block4;
                                }
                                break block0;
                            }
                            case 1: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                if (Runtime.getFlags(4075, 1) != 0) {
                                    window.print(this.msgMsetuClear, 0);
                                    ST0160.waitPage(window, 64);
                                } else {
                                    window.print(this.msgMsetumei, 0);
                                    ST0160.waitPage(window, 64);
                                }
                                while (this.setumei) {
                                    block81:
                                    while (true) {
                                        System.waitFor(window);
                                        this.menu = Menu.create();
                                        this.menu.addItem("Start game\nHear instructions\nForget it");
                                        System.waitFor(this.menu);
                                        this.selected = this.menu.getSelected();
                                        switch (this.selected) {
                                            case 0: {
                                                if (Runtime.getFlags(4079, 1) != 0) {
                                                    window = Window.create();
                                                    window.setSize(4, 45);
                                                    window.setLocation(15, 305);
                                                    window.print(this.msgDrillSelect1, 0);
                                                    ST0160.waitPage(window, 64);
                                                    System.waitFor(window);
                                                    this.menu = Menu.create();
                                                    this.menu.addItem("Normal drill\nMost powerful drill");
                                                    System.waitFor(this.menu);
                                                    this.selected = this.menu.getSelected();
                                                    switch (this.selected) {
                                                        case 0: {
                                                            this.hutuu = true;
                                                            window = Window.create();
                                                            window.setSize(4, 45);
                                                            window.setLocation(15, 305);
                                                            window.print(this.msgDrillSelectM, 0);
                                                            ST0160.waitPage(window, 64);
                                                            this.drill.DrillSetDrillSize(0.15f);
                                                            Stage.setVisible(115, true);
                                                            Stage.setVisible(133, false);
                                                            Stage.setVisible(134, false);
                                                            Runtime.removeGold(200);
                                                            this.drill.DrillCommand(9);
                                                            System.sleep(1);
                                                            this.drill.DrillCommand(8);
                                                            this.Mission1();
                                                            ++this.play;
                                                            return;
                                                        }
                                                        case 1: {
                                                            this.mau = Math.random();
                                                            this.mau = (this.mau & 0x7FFFFFF) % 4;
                                                            if (this.mau < 0) {
                                                                this.mau *= -1;
                                                            }
                                                            Runtime.setRegister(0, this.mau);
                                                            System.println("mauの値は: /[$0]");
                                                            if (this.mau == 0) {
                                                                this.hosoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectS, 0);
                                                                ST0160.waitPage(window, 64);
                                                                window.print(this.msgDrillSelectS2, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(0.05f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, false);
                                                                Stage.setVisible(134, true);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission1();
                                                                ++this.play;
                                                            } else {
                                                                this.hutoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectL, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(1.0f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, true);
                                                                Stage.setVisible(134, false);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission1();
                                                                ++this.play;
                                                            }
                                                            return;
                                                        }
                                                        default: {
                                                            if (this.setumei) continue block81;
                                                        }
                                                    }
                                                }
                                                Runtime.removeGold(200);
                                                this.drill.DrillCommand(9);
                                                System.sleep(1);
                                                this.drill.DrillCommand(8);
                                                this.Mission1();
                                                ++this.play;
                                                return;
                                            }
                                            case 1: {
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgMsetumei1, 0);
                                                ST0160.waitPage(window, 64);
                                                break block81;
                                            }
                                            case 2: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block81;
                                            }
                                            default: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block81;
                                            }
                                        }
                                        break;
                                    }
                                    continue;
                                    break block4;
                                }
                                break block0;
                            }
                            case 2: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                if (Runtime.getFlags(4076, 1) != 0) {
                                    window.print(this.msgMsetuClear, 0);
                                    ST0160.waitPage(window, 64);
                                } else {
                                    window.print(this.msgMsetumei, 0);
                                    ST0160.waitPage(window, 64);
                                }
                                while (this.setumei) {
                                    block83:
                                    while (true) {
                                        System.waitFor(window);
                                        this.menu = Menu.create();
                                        this.menu.addItem("Start game\nHear instructions\nForget it");
                                        System.waitFor(this.menu);
                                        this.selected = this.menu.getSelected();
                                        switch (this.selected) {
                                            case 0: {
                                                if (Runtime.getFlags(4079, 1) != 0) {
                                                    window = Window.create();
                                                    window.setSize(4, 45);
                                                    window.setLocation(15, 305);
                                                    window.print(this.msgDrillSelect1, 0);
                                                    ST0160.waitPage(window, 64);
                                                    System.waitFor(window);
                                                    this.menu = Menu.create();
                                                    this.menu.addItem("Normal drill\nMost powerful drill");
                                                    System.waitFor(this.menu);
                                                    this.selected = this.menu.getSelected();
                                                    switch (this.selected) {
                                                        case 0: {
                                                            this.hutuu = true;
                                                            window = Window.create();
                                                            window.setSize(4, 45);
                                                            window.setLocation(15, 305);
                                                            window.print(this.msgDrillSelectM, 0);
                                                            ST0160.waitPage(window, 64);
                                                            this.drill.DrillSetDrillSize(0.15f);
                                                            Stage.setVisible(115, true);
                                                            Stage.setVisible(133, false);
                                                            Stage.setVisible(134, false);
                                                            Runtime.removeGold(200);
                                                            this.drill.DrillCommand(9);
                                                            System.sleep(1);
                                                            this.drill.DrillCommand(8);
                                                            this.Mission2();
                                                            ++this.play;
                                                            return;
                                                        }
                                                        case 1: {
                                                            this.mau = Math.random();
                                                            this.mau = (this.mau & 0x7FFFFFF) % 4;
                                                            if (this.mau < 0) {
                                                                this.mau *= -1;
                                                            }
                                                            Runtime.setRegister(0, this.mau);
                                                            System.println("mauの値は: /[$0]");
                                                            if (this.mau == 0) {
                                                                this.hosoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectS, 0);
                                                                ST0160.waitPage(window, 64);
                                                                window.print(this.msgDrillSelectS2, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(0.05f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, false);
                                                                Stage.setVisible(134, true);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission2();
                                                                ++this.play;
                                                            } else {
                                                                this.hutoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectL, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(1.0f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, true);
                                                                Stage.setVisible(134, false);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission2();
                                                                ++this.play;
                                                            }
                                                            return;
                                                        }
                                                        default: {
                                                            if (this.setumei) continue block83;
                                                        }
                                                    }
                                                }
                                                Runtime.removeGold(200);
                                                this.drill.DrillCommand(9);
                                                System.sleep(1);
                                                this.drill.DrillCommand(8);
                                                this.Mission2();
                                                ++this.play;
                                                return;
                                            }
                                            case 1: {
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgMsetumei2, 0);
                                                ST0160.waitPage(window, 64);
                                                break block83;
                                            }
                                            case 2: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block83;
                                            }
                                            default: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block83;
                                            }
                                        }
                                        break;
                                    }
                                    continue;
                                    break block4;
                                }
                                break block0;
                            }
                            case 3: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                if (Runtime.getFlags(4077, 1) != 0) {
                                    window.print(this.msgMsetuClear, 0);
                                    ST0160.waitPage(window, 64);
                                } else {
                                    window.print(this.msgMsetumei, 0);
                                    ST0160.waitPage(window, 64);
                                }
                                while (this.setumei) {
                                    block85:
                                    while (true) {
                                        System.waitFor(window);
                                        this.menu = Menu.create();
                                        this.menu.addItem("Start game\nHear instructions\nForget it");
                                        System.waitFor(this.menu);
                                        this.selected = this.menu.getSelected();
                                        switch (this.selected) {
                                            case 0: {
                                                if (Runtime.getFlags(4079, 1) != 0) {
                                                    window = Window.create();
                                                    window.setSize(4, 45);
                                                    window.setLocation(15, 305);
                                                    window.print(this.msgDrillSelect1, 0);
                                                    ST0160.waitPage(window, 64);
                                                    System.waitFor(window);
                                                    this.menu = Menu.create();
                                                    this.menu.addItem("Normal drill\nMost powerful drill");
                                                    System.waitFor(this.menu);
                                                    this.selected = this.menu.getSelected();
                                                    switch (this.selected) {
                                                        case 0: {
                                                            this.hutuu = true;
                                                            window = Window.create();
                                                            window.setSize(4, 45);
                                                            window.setLocation(15, 305);
                                                            window.print(this.msgDrillSelectM, 0);
                                                            ST0160.waitPage(window, 64);
                                                            this.drill.DrillSetDrillSize(0.15f);
                                                            Stage.setVisible(115, true);
                                                            Stage.setVisible(133, false);
                                                            Stage.setVisible(134, false);
                                                            Runtime.removeGold(200);
                                                            this.drill.DrillCommand(9);
                                                            System.sleep(1);
                                                            this.drill.DrillCommand(8);
                                                            this.Mission3();
                                                            ++this.play;
                                                            return;
                                                        }
                                                        case 1: {
                                                            this.mau = Math.random();
                                                            this.mau = (this.mau & 0x7FFFFFF) % 4;
                                                            if (this.mau < 0) {
                                                                this.mau *= -1;
                                                            }
                                                            Runtime.setRegister(0, this.mau);
                                                            System.println("mauの値は: /[$0]");
                                                            if (this.mau == 0) {
                                                                this.hosoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectS, 0);
                                                                ST0160.waitPage(window, 64);
                                                                window.print(this.msgDrillSelectS2, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(0.05f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, false);
                                                                Stage.setVisible(134, true);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission3();
                                                                ++this.play;
                                                            } else {
                                                                this.hutoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectL, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(1.0f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, true);
                                                                Stage.setVisible(134, false);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission3();
                                                                ++this.play;
                                                            }
                                                            return;
                                                        }
                                                        default: {
                                                            if (this.setumei) continue block85;
                                                        }
                                                    }
                                                }
                                                Runtime.removeGold(200);
                                                this.drill.DrillCommand(9);
                                                System.sleep(1);
                                                this.drill.DrillCommand(8);
                                                this.Mission3();
                                                ++this.play;
                                                return;
                                            }
                                            case 1: {
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgMsetumei3, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc2.moveEnepc(17, 270.0f, 0.0f, 10);
                                                window.print(this.msgMsetumei3_1, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc2.moveEnepc(17, 359.0f, 0.0f, 10);
                                                window.print(this.msgMsetumei3_2, 0);
                                                ST0160.waitPage(window, 64);
                                                break block85;
                                            }
                                            case 2: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block85;
                                            }
                                            default: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block85;
                                            }
                                        }
                                        break;
                                    }
                                    continue;
                                    break block4;
                                }
                                break block0;
                            }
                            case 4: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                if (Runtime.getFlags(4078, 1) != 0) {
                                    window.print(this.msgMsetuClear, 0);
                                    ST0160.waitPage(window, 64);
                                } else {
                                    window.print(this.msgMsetumei, 0);
                                    ST0160.waitPage(window, 64);
                                }
                                while (this.setumei) {
                                    block87:
                                    while (true) {
                                        System.waitFor(window);
                                        this.menu = Menu.create();
                                        this.menu.addItem("Start game\nHear instructions\nForget it");
                                        System.waitFor(this.menu);
                                        this.selected = this.menu.getSelected();
                                        switch (this.selected) {
                                            case 0: {
                                                if (Runtime.getFlags(4079, 1) != 0) {
                                                    window = Window.create();
                                                    window.setSize(4, 45);
                                                    window.setLocation(15, 305);
                                                    window.print(this.msgDrillSelect1, 0);
                                                    ST0160.waitPage(window, 64);
                                                    System.waitFor(window);
                                                    this.menu = Menu.create();
                                                    this.menu.addItem("Normal drill\nMost powerful drill");
                                                    System.waitFor(this.menu);
                                                    this.selected = this.menu.getSelected();
                                                    switch (this.selected) {
                                                        case 0: {
                                                            this.hutuu = true;
                                                            window = Window.create();
                                                            window.setSize(4, 45);
                                                            window.setLocation(15, 305);
                                                            window.print(this.msgDrillSelectM, 0);
                                                            ST0160.waitPage(window, 64);
                                                            this.drill.DrillSetDrillSize(0.15f);
                                                            Stage.setVisible(115, true);
                                                            Stage.setVisible(133, false);
                                                            Stage.setVisible(134, false);
                                                            Runtime.removeGold(200);
                                                            this.drill.DrillCommand(9);
                                                            System.sleep(1);
                                                            this.drill.DrillCommand(8);
                                                            this.Mission4();
                                                            ++this.play;
                                                            return;
                                                        }
                                                        case 1: {
                                                            this.mau = Math.random();
                                                            this.mau = (this.mau & 0x7FFFFFF) % 4;
                                                            if (this.mau < 0) {
                                                                this.mau *= -1;
                                                            }
                                                            Runtime.setRegister(0, this.mau);
                                                            System.println("mauの値は: /[$0]");
                                                            if (this.mau == 0) {
                                                                this.hosoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectS, 0);
                                                                ST0160.waitPage(window, 64);
                                                                window.print(this.msgDrillSelectS2, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(0.05f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, false);
                                                                Stage.setVisible(134, true);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission4();
                                                                ++this.play;
                                                            } else {
                                                                this.hutoi = true;
                                                                window = Window.create();
                                                                window.setSize(4, 45);
                                                                window.setLocation(15, 305);
                                                                window.print(this.msgDrillSelectL, 0);
                                                                ST0160.waitPage(window, 64);
                                                                this.drill.DrillSetDrillSize(1.0f);
                                                                Stage.setVisible(115, false);
                                                                Stage.setVisible(133, true);
                                                                Stage.setVisible(134, false);
                                                                Runtime.removeGold(200);
                                                                this.drill.DrillCommand(9);
                                                                System.sleep(1);
                                                                this.drill.DrillCommand(8);
                                                                this.Mission4();
                                                                ++this.play;
                                                            }
                                                            return;
                                                        }
                                                        default: {
                                                            if (this.setumei) continue block87;
                                                        }
                                                    }
                                                }
                                                Runtime.removeGold(200);
                                                this.drill.DrillCommand(9);
                                                System.sleep(1);
                                                this.drill.DrillCommand(8);
                                                this.Mission4();
                                                ++this.play;
                                                return;
                                            }
                                            case 1: {
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgMsetumei4, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc2.moveEnepc(17, 270.0f, 0.0f, 10);
                                                window.print(this.msgMsetumei4_1, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc2.moveEnepc(17, 359.0f, 0.0f, 10);
                                                window.print(this.msgMsetumei4_2, 0);
                                                ST0160.waitPage(window, 64);
                                                break block87;
                                            }
                                            case 2: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block87;
                                            }
                                            default: {
                                                this.setumei = false;
                                                window = Window.create();
                                                window.setSize(4, 45);
                                                window.setLocation(15, 305);
                                                window.print(this.msgYappari, 0);
                                                ST0160.waitPage(window, 64);
                                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                                break block87;
                                            }
                                        }
                                        break;
                                    }
                                    continue;
                                    break block4;
                                }
                                break block0;
                            }
                            default: {
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                window.print(this.msgYappari, 0);
                                ST0160.waitPage(window, 64);
                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                break;
                            }
                        }
                        break;
                    }
                    this.junbi = true;
                    window = Window.create();
                    window.setSize(4, 45);
                    window.setLocation(15, 305);
                    window.print(this.msgD01_2, 0);
                    ST0160.waitPage(window, 64);
                    block88:
                    while (this.junbi) {
                        System.waitFor(window);
                        this.menu = Menu.create();
                        if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) == 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)] \nMISSION 2\nUnder Construction\nUnder Construction");
                        } else if (Runtime.getFlags(4075, 1) == 0 && Runtime.getFlags(4076, 1) != 0) {
                            this.menu.addItem("Free Mode\nMISSION 1\n/[color(0xfa3232)]MISSION 2/[color(0x808080)] \nUnder Construction\nUnder Construction");
                        } else if (Runtime.getFlags(4075, 1) != 0 && Runtime.getFlags(4076, 1) != 0) {
                            this.menu.addItem("Free Mode\n/[color(0xfa3232)]MISSION 1/[color(0x808080)] \n/[color(0xfa3232)]MISSION 2/[color(0x808080)] \nUnder Construction\nUnder Construction");
                        } else {
                            this.menu.addItem("Free Mode\nMISSION 1\nMISSION 2\nUnder Construction\nUnder Construction");
                        }
                        System.waitFor(this.menu);
                        this.selected = this.menu.getSelected();
                        switch (this.selected) {
                            case 0: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                window.print(this.msgFsetumei, 0);
                                ST0160.waitPage(window, 64);
                                while (this.setumei) {
                                    System.waitFor(window);
                                    this.menu = Menu.create();
                                    this.menu.addItem("Start game\nHear instructions\nForget it");
                                    System.waitFor(this.menu);
                                    this.selected = this.menu.getSelected();
                                    switch (this.selected) {
                                        case 0: {
                                            Runtime.removeGold(200);
                                            this.drill.DrillCommand(9);
                                            this.Freeplay();
                                            ++this.play;
                                            this.junbi = false;
                                            return;
                                        }
                                        case 1: {
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgFsetumei1, 0);
                                            ST0160.waitPage(window, 64);
                                            this.junbi = false;
                                            break;
                                        }
                                        case 2: {
                                            this.setumei = false;
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgYappari, 0);
                                            this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                            ST0160.waitPage(window, 64);
                                            this.junbi = false;
                                            break;
                                        }
                                        default: {
                                            this.setumei = false;
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgYappari, 0);
                                            ST0160.waitPage(window, 64);
                                            this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                            this.junbi = false;
                                            break;
                                        }
                                    }
                                }
                                continue block88;
                            }
                            case 1: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                if (Runtime.getFlags(4075, 1) != 0) {
                                    window.print(this.msgMsetuClear, 0);
                                    ST0160.waitPage(window, 64);
                                } else {
                                    window.print(this.msgMsetumei, 0);
                                    ST0160.waitPage(window, 64);
                                }
                                while (this.setumei) {
                                    System.waitFor(window);
                                    this.menu = Menu.create();
                                    this.menu.addItem("Start game\nHear explanation\nForget it");
                                    System.waitFor(this.menu);
                                    this.selected = this.menu.getSelected();
                                    switch (this.selected) {
                                        case 0: {
                                            Runtime.removeGold(200);
                                            this.drill.DrillCommand(9);
                                            System.sleep(1);
                                            this.drill.DrillCommand(8);
                                            this.junbi = false;
                                            this.Mission1();
                                            ++this.play;
                                            return;
                                        }
                                        case 1: {
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgMsetumei1, 0);
                                            ST0160.waitPage(window, 64);
                                            this.junbi = false;
                                            break;
                                        }
                                        case 2: {
                                            this.setumei = false;
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgYappari, 0);
                                            ST0160.waitPage(window, 64);
                                            this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                            this.junbi = false;
                                            break;
                                        }
                                        default: {
                                            this.setumei = false;
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgYappari, 0);
                                            ST0160.waitPage(window, 64);
                                            this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                            this.junbi = false;
                                            break;
                                        }
                                    }
                                }
                                continue block88;
                            }
                            case 2: {
                                this.setumei = true;
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                if (Runtime.getFlags(4076, 1) != 0) {
                                    window.print(this.msgMsetuClear, 0);
                                    ST0160.waitPage(window, 64);
                                } else {
                                    window.print(this.msgMsetumei, 0);
                                    ST0160.waitPage(window, 64);
                                }
                                while (this.setumei) {
                                    System.waitFor(window);
                                    this.menu = Menu.create();
                                    this.menu.addItem("Start game\nHear instructions\nForget it");
                                    System.waitFor(this.menu);
                                    this.selected = this.menu.getSelected();
                                    switch (this.selected) {
                                        case 0: {
                                            Runtime.removeGold(200);
                                            this.drill.DrillCommand(9);
                                            System.sleep(1);
                                            this.drill.DrillCommand(8);
                                            this.junbi = false;
                                            this.Mission2();
                                            ++this.play;
                                            return;
                                        }
                                        case 1: {
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgMsetumei2, 0);
                                            ST0160.waitPage(window, 64);
                                            this.junbi = false;
                                            break;
                                        }
                                        case 2: {
                                            this.setumei = false;
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgYappari, 0);
                                            this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                            ST0160.waitPage(window, 64);
                                            this.junbi = false;
                                            break;
                                        }
                                        default: {
                                            this.setumei = false;
                                            window = Window.create();
                                            window.setSize(4, 45);
                                            window.setLocation(15, 305);
                                            window.print(this.msgYappari, 0);
                                            ST0160.waitPage(window, 64);
                                            this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                            this.junbi = false;
                                            break;
                                        }
                                    }
                                }
                                continue block88;
                            }
                            case 3: {
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                window.print(this.msgJunbi, 0);
                                ST0160.waitPage(window, 64);
                                this.junbi = true;
                                break;
                            }
                            case 4: {
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                window.print(this.msgJunbi, 0);
                                ST0160.waitPage(window, 64);
                                this.junbi = true;
                                break;
                            }
                            default: {
                                window = Window.create();
                                window.setSize(4, 45);
                                window.setLocation(15, 305);
                                window.print(this.msgYappari, 0);
                                ST0160.waitPage(window, 64);
                                this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                                this.junbi = false;
                                break;
                            }
                        }
                    }
                    break;
                }
                case 1: {
                    window = Window.create();
                    window.setSize(4, 45);
                    window.setLocation(15, 305);
                    window.print(this.msgD01_3, 0);
                    ST0160.waitPage(window, 64);
                    this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                    break;
                }
                default: {
                    window = Window.create();
                    window.setSize(4, 45);
                    window.setLocation(15, 305);
                    window.print(this.msgD01_3, 0);
                    ST0160.waitPage(window, 64);
                    this.npc1.moveEnepc(17, 359.9f, -10.0f, 10);
                    break;
                }
            }
        }
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (Runtime.getFlags(4071, 1) == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        window.print(this.msgFirst_F, 0);
        ST0160.waitPage(window, 64);
        this.npc2.moveEnepc(17, 359.9f, -10.0f, 10);
    }

    void Talk_npc2_2(Window var1_1) {

        throw new IllegalStateException("Decompilation failed");
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        this.Talk_npc3_1(window);
    }

    void Talk_npc3_1(Window window) {
        this.npc3.kickEnepc(1, 28);
        window.print(this.msgShopM3, 0);
        ST0160.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (Runtime.getFlags(4080, 1) == 1) {
            this.Talk_npc4_2(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        window.print(this.msgShopJ1, 0);
        ST0160.waitPage(window, 64);
        window.print(this.msgShopM1, 0);
        ST0160.waitPage(window, 64);
        System.sleep(15);
        Runtime.enterShop(20);
        window = Window.create();
        window.setSize(4, 45);
        window.setLocation(15, 305);
        window.print(this.msgShopJ3, 0);
        ST0160.waitPage(window, 64);
        window.print(this.msgShopM2, 0);
        ST0160.waitPage(window, 64);
        Runtime.setFlags(4080, 1, 1);
    }

    void Talk_npc4_2(Window window) {
        window.print(this.msgShopJ2, 0);
        ST0160.waitPage(window, 64);
        window.print(this.msgShopM1, 0);
        ST0160.waitPage(window, 64);
        System.sleep(15);
        Runtime.enterShop(20);
        window = Window.create();
        window.setSize(4, 45);
        window.setLocation(15, 305);
        window.print(this.msgShopJ3, 0);
        ST0160.waitPage(window, 64);
        window.print(this.msgShopM2, 0);
        ST0160.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (Runtime.getFlags(4071, 1) == 0) {
            this.Talk_npc5_1(window);
        } else if (Runtime.getFlags(8059, 1) != 0) {
            this.Talk_npc5_3(window);
        } else if (Runtime.getFlags(8059, 1) == 0) {
            this.Talk_npc5_2(window);
        }
    }

    void Talk_npc5_1(Window window) {
        window.print(this.msgWoman1, 0);
        ST0160.waitPage(window, 64);
    }

    void Talk_npc5_2(Window window) {
        if (this.npc5TKF) {
            window.print(this.msgWoman2_1, 0);
            ST0160.waitPage(window, 64);
            this.npc5TKF = false;
        } else if (!this.npc5TKF) {
            window.print(this.msgWoman2_2, 0);
            ST0160.waitPage(window, 64);
            this.npc5TKF = true;
        }
    }

    void Talk_npc5_3(Window window) {
        if (this.npc5TKF) {
            window.print(this.msgWoman3_1, 0);
            ST0160.waitPage(window, 64);
            this.npc5TKF = false;
        } else if (!this.npc5TKF) {
            window.print(this.msgWoman3_2, 0);
            ST0160.waitPage(window, 64);
            this.npc5TKF = true;
        }
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (Runtime.getFlags(4071, 1) == 0) {
            this.Talk_npc6_1(window);
        } else {
            this.Talk_npc6_2(window);
        }
    }

    void Talk_npc6_1(Window window) {
        window.print(this.msgGoroA1, 0);
        ST0160.waitPage(window, 64);
    }

    void Talk_npc6_2(Window window) {
        int n = 0;
        if (Runtime.getFlags(4061, 1) != 0) {
            ++n;
        }
        if (Runtime.getFlags(4062, 1) != 0) {
            ++n;
        }
        if (Runtime.getFlags(4063, 1) != 0) {
            ++n;
        }
        if (Runtime.getFlags(4064, 1) != 0) {
            ++n;
        }
        if (Runtime.getFlags(4065, 1) != 0) {
            ++n;
        }
        if (n == 0) {
            if (this.npc6TKF) {
                window.print(this.msgGoroA2_1, 0);
                ST0160.waitPage(window, 64);
                this.npc6TKF = false;
            } else if (!this.npc6TKF) {
                window.print(this.msgGoroA2_2, 0);
                ST0160.waitPage(window, 64);
                this.npc6TKF = true;
            }
        } else if (n == 1) {
            window.print(this.msgGoroA3_1, 0);
            ST0160.waitPage(window, 64);
        } else if (n == 2) {
            window.print(this.msgGoroA3_2, 0);
            ST0160.waitPage(window, 64);
        } else if (n == 3) {
            window.print(this.msgGoroA3_3, 0);
            ST0160.waitPage(window, 64);
        } else if (n == 4) {
            window.print(this.msgGoroA3_4, 0);
            ST0160.waitPage(window, 64);
        } else if (n == 5) {
            window.print(this.msgGoroA3_5, 0);
            ST0160.waitPage(window, 64);
        }
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (Runtime.getFlags(4071, 1) == 0) {
            this.Talk_npc7_1(window);
        } else if (Runtime.getFlags(8059, 1) != 0) {
            this.Talk_npc7_3(window);
        } else if (Runtime.getFlags(8059, 1) == 0) {
            this.Talk_npc7_2(window);
        }
    }

    void Talk_npc7_1(Window window) {
        window.print(this.msgGoroB1, 0);
        ST0160.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        if (this.npc7TKF) {
            window.print(this.msgGoroB2_1, 0);
            ST0160.waitPage(window, 64);
            this.npc7TKF = false;
        } else if (!this.npc7TKF) {
            window.print(this.msgGoroB2_2, 0);
            ST0160.waitPage(window, 64);
            this.npc7TKF = true;
        }
    }

    void Talk_npc7_3(Window window) {
        if (this.npc7TKF) {
            window.print(this.msgGoroB3_1, 0);
            ST0160.waitPage(window, 64);
            this.npc7TKF = false;
        } else if (!this.npc7TKF) {
            window.print(this.msgGoroB3_2, 0);
            ST0160.waitPage(window, 64);
            this.npc7TKF = true;
        }
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    int YNMenu() {
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    int YNMenu2() {
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        return this.menu.getSelected();
    }

    void broken(int n) {
        System.println("壊れ物破壊(broken)");
        ++this.count;
        switch (n) {
            case 0: {
                System.println("０番目破壊");
                break;
            }
            case 1: {
                System.println("Ｍ１＿１番目破壊");
                this.M1_1 = true;
                break;
            }
            case 2: {
                System.println("Ｍ１＿２番目破壊");
                this.M1_2 = true;
                break;
            }
            case 3: {
                System.println("Ｍ１＿３番目破壊");
                this.M1_3 = true;
                break;
            }
            case 4: {
                System.println("Ｍ１＿４番目破壊");
                this.M1_4 = true;
                break;
            }
            case 5: {
                System.println("Ｍ１＿５番目破壊");
                this.M1_5 = true;
                break;
            }
            case 6: {
                System.println("Ｍ１＿６番目破壊");
                this.M1_6 = true;
                break;
            }
            case 7: {
                System.println("Ｍ１＿７番目破壊");
                this.M1_7 = true;
                break;
            }
            case 8: {
                System.println("Ｍ１＿８番目破壊");
                this.M1_8 = true;
                break;
            }
            case 9: {
                System.println("Ｍ１＿９番目破壊");
                this.M1_9 = true;
                break;
            }
            case 10: {
                System.println("Ｍ１＿１０番目破壊");
                this.M1_10 = true;
                break;
            }
            case 11: {
                System.println("Ｍ１＿１１番目破壊");
                this.M1_11 = true;
                break;
            }
            case 12: {
                System.println("Ｍ１＿１２番目破壊");
                this.M1_12 = true;
                break;
            }
            case 13: {
                System.println("Ｍ１＿１３番目破壊");
                this.M1_13 = true;
                break;
            }
            case 14: {
                System.println("Ｍ１＿１４番目破壊");
                this.M1_14 = true;
                break;
            }
            case 15: {
                System.println("Ｍ１＿１５番目破壊");
                this.M1_15 = true;
                break;
            }
            case 16: {
                System.println("Ｍ２＿１番目破壊");
                this.M1_16 = true;
                break;
            }
            case 17: {
                System.println("Ｍ２＿２番目破壊");
                this.M1_17 = true;
                break;
            }
            case 18: {
                System.println("Ｍ２＿３番目破壊");
                this.M1_18 = true;
                break;
            }
            case 19: {
                System.println("Ｍ２＿４番目破壊");
                this.M1_19 = true;
                break;
            }
            case 20: {
                System.println("Ｍ２＿５番目破壊");
                this.M1_20 = true;
                break;
            }
            case 21: {
                System.println("Ｍ２＿６番目破壊");
                this.M1_21 = true;
                break;
            }
            case 22: {
                System.println("Ｍ２＿７番目破壊");
                this.M1_22 = true;
                break;
            }
            case 23: {
                System.println("Ｍ２＿８番目破壊");
                this.M1_23 = true;
                break;
            }
            case 24: {
                System.println("Ｍ２＿９番目破壊");
                this.M1_24 = true;
                break;
            }
            default: {
                System.println("未登録破壊");
            }
        }
    }

    void drill_end() {
        System.println("ドリゲー終了(drill_end)");
        this.FAILED = true;
        this.hutuu = false;
        this.hutoi = false;
        this.hosoi = false;
        this.count = 0;
        this.drill.DrillCommand(8);
        Stage.setVisible(41, true);
        Stage.setVisible(123, true);
        Stage.setVisible(126, true);
        Stage.setVisible(7, true);
        Stage.setVisible(3, true);
        this.npc3.kickEnepc(4, 0);
        this.npc4.kickEnepc(4, 0);
        this.npc5.kickEnepc(4, 0);
        this.npc6.kickEnepc(4, 0);
        this.npc7.kickEnepc(4, 0);
        this.npc3.setVisible(true);
        this.npc4.setVisible(true);
        this.npc5.setVisible(true);
        this.npc6.setVisible(true);
        this.npc7.setVisible(true);
        this.EF01.disp(false);
        this.EF03.disp(false);
        this.EF04.disp(false);
        this.EF05.disp(false);
        this.player.setTranslate(10.454f, 0.0f, -22.403f);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msgGameend, 0);
        ST0160.waitPage(this.win, 64);
        Runtime.setPlayerControl(true);
        Runtime.disable(65536);
    }

    void drill_stanby() {
        System.println("１ターン終了(drill_stanby)");
        if (this.mission1 && !this.FAILED) {
            if (this.M1_1 && this.M1_2 && this.M1_3 && this.M1_4 && this.M1_5 && this.M1_6 && this.M1_7 && this.M1_8 && this.M1_9 && this.M1_10 && this.M1_11 && this.M1_12 && this.M1_13 && this.M1_14 && this.M1_15) {
                System.println("ミッション成功(SUCCESS == true)");
                this.SUCCESS = true;
                this.M1_result();
            }
        } else if (this.mission2 && !this.FAILED) {
            if (this.M1_16 && this.M1_17 && this.M1_18 && this.M1_19 && this.M1_20 && this.M1_21 && this.M1_22 && this.M1_23 && this.M1_24) {
                this.SUCCESS = true;
                this.M2_result();
            }
        } else if (this.mission3 && !this.FAILED) {
            if (this.count == 5) {
                this.SUCCESS = true;
                this.M3_result();
            }
        } else if (this.mission4 && !this.FAILED) {
            if (this.count == 1) {
                this.SUCCESS = true;
                this.M4_result();
            }
        } else {
            return;
        }
    }

    void drill_stop() {
        if (!this.SUCCESS && !this.FAILED) {
            System.println("キャンセル希望(drill_stop)");
            this.win = Window.create();
            this.win.print(this.msgend, 0);
            ST0160.waitPage(this.win, 64);
            System.waitFor(this.win);
            this.menu = Menu.create();
            this.menu.addItem("Yes\nNo");
            System.waitFor(this.menu);
            this.selected = this.menu.getSelected();
            switch (this.selected) {
                case 0: {
                    this.After_Drill();
                }
                case 1: {
                    this.drill.DrillCommand(5);
                    return;
                }
            }
            this.drill.DrillCommand(5);
            return;
        }
    }

    void drill_timeover() {
        System.println("時間切れスレッド開始(drill_timeover)");
        if (!this.SUCCESS) {
            System.println("ミッション未成功(SUCCESS == false)");
            this.FAILED = true;
            this.mission1 = false;
            this.mission2 = false;
            this.M1_1 = false;
            this.M1_2 = false;
            this.M1_3 = false;
            this.M1_4 = false;
            this.M1_5 = false;
            this.M1_6 = false;
            this.M1_7 = false;
            this.M1_8 = false;
            this.M1_9 = false;
            this.M1_10 = false;
            this.M1_11 = false;
            this.M1_12 = false;
            this.M1_13 = false;
            this.M1_14 = false;
            this.M1_15 = false;
            this.M1_16 = false;
            this.M1_17 = false;
            this.M1_18 = false;
            this.M1_19 = false;
            this.M1_20 = false;
            this.M1_21 = false;
            this.M1_22 = false;
            this.M1_23 = false;
            this.M1_24 = false;
            this.hutuu = false;
            this.hutoi = false;
            this.hosoi = false;
            this.fade60.call(0);
            System.sleep(60);
            this.count = 0;
            this.npc3.kickEnepc(4, 0);
            this.npc4.kickEnepc(4, 0);
            this.npc5.kickEnepc(4, 0);
            this.npc6.kickEnepc(4, 0);
            this.npc7.kickEnepc(4, 0);
            this.npc3.setVisible(true);
            this.npc4.setVisible(true);
            this.npc5.setVisible(true);
            this.npc6.setVisible(true);
            this.npc7.setVisible(true);
            this.drill.DrillCommand(8);
            this.drill.DrillCommand(3);
            if (this.mission3) {
                this.mission3 = false;
                this.ukun01.kickEnepc(4, 1);
                this.ukun02.kickEnepc(4, 1);
                this.ukun03.kickEnepc(4, 1);
                this.ukun04.kickEnepc(4, 1);
                this.ukun05.kickEnepc(4, 1);
                this.ukun01.setVisible(false);
                this.ukun02.setVisible(false);
                this.ukun03.setVisible(false);
                this.ukun04.setVisible(false);
                this.ukun05.setVisible(false);
                Stage.setVisible(41, true);
                Stage.setVisible(123, true);
                Stage.setVisible(126, true);
                Stage.setVisible(7, true);
                Stage.setVisible(3, true);
                this.EF01.disp(false);
                this.EF03.disp(false);
                this.EF04.disp(false);
                this.EF05.disp(false);
                Runtime.setPlayerControl(false);
                this.player.setTranslate(10.454f, 0.0f, -22.403f);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTimeOver, 0);
                ST0160.waitPage(this.win, 64);
                Runtime.addItemWin(0, 1);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
            } else if (this.mission4) {
                this.mission4 = false;
                this.ukun06.kickEnepc(4, 1);
                this.ukun06.setVisible(false);
                Stage.setVisible(41, true);
                Stage.setVisible(123, true);
                Stage.setVisible(126, true);
                Stage.setVisible(7, true);
                Stage.setVisible(3, true);
                this.EF01.disp(false);
                this.EF03.disp(false);
                this.EF04.disp(false);
                this.EF05.disp(false);
                Runtime.setPlayerControl(false);
                this.player.setTranslate(10.454f, 0.0f, -22.403f);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTimeOver, 0);
                ST0160.waitPage(this.win, 64);
                Runtime.addItemWin(0, 1);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
            } else {
                Stage.setVisible(41, true);
                Stage.setVisible(123, true);
                Stage.setVisible(126, true);
                Stage.setVisible(7, true);
                Stage.setVisible(3, true);
                this.EF01.disp(false);
                this.EF03.disp(false);
                this.EF04.disp(false);
                this.EF05.disp(false);
                Runtime.setPlayerControl(false);
                this.player.setTranslate(10.454f, 0.0f, -22.403f);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgTimeOver, 0);
                ST0160.waitPage(this.win, 64);
                Runtime.addItemWin(0, 1);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
            }
        } else {
            System.println("ミッション成功済み(SUCCESS == true)");
        }
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(70, 4);
                break;
            }
            case 1: {
                Runtime.jumpCF(60, 1);
                break;
            }
        }
    }

    void evsExit() {
        System.println("evsExitをコールしました");
        if (this.b_flg == 1) {
            return;
        }
        this.b_flg = 1;
        Runtime.enable(262144);
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.print(this.EVS, 0);
        ST0160.waitPage(this.win, 64);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                Runtime.setFlags(4070, 1, 0);
                this.fade.call(0);
                System.sleep(30);
                Runtime.setPlayerControl(true);
                Runtime.disable(262144);
                Runtime.evsExit();
                return;
            }
        }
        Runtime.setPlayerControl(true);
        Runtime.disable(262144);
        this.b_flg = 0;
    }

    void init() {
        Runtime.setFlags(4070, 1, 1);
        new Uwamono(28734, 13.5f, 0.0f, -23.5f);
        this.ukun01 = new Enepc();
        this.ukun01.init(8705, 9, -1.0f, 0.0f, -22.0f, 0.0f);
        this.ukun01.id = 7;
        float[] fArray = new float[4];
        fArray[0] = -1.0f;
        fArray[2] = -22.0f;
        fArray[3] = -1.0f;
        float[] fArray2 = fArray;
        this.ukun01.setParams(11, 3, 7, 9, fArray2);
        float[] fArray3 = new float[9];
        fArray3[0] = -1.0f;
        fArray3[2] = -22.0f;
        fArray3[3] = -1.0f;
        fArray3[5] = -20.5f;
        fArray3[6] = -1.0f;
        fArray3[8] = -18.5f;
        float[] fArray4 = fArray3;
        this.ukun01.setParams(fArray4);
        this.ukun01.enableDTKFlag(262144);
        this.ukun01.disableDTKFlag(65536);
        this.ukun02 = new Enepc();
        this.ukun02.init(8705, 9, 1.5f, 0.0f, -22.0f, 0.0f);
        this.ukun02.id = 8;
        float[] fArray5 = new float[4];
        fArray5[0] = 1.5f;
        fArray5[2] = -22.0f;
        fArray5[3] = -1.0f;
        float[] fArray6 = fArray5;
        this.ukun02.setParams(11, 4, 8, 9, fArray6);
        float[] fArray7 = new float[9];
        fArray7[0] = 1.5f;
        fArray7[2] = -22.0f;
        fArray7[3] = 1.5f;
        fArray7[5] = -20.5f;
        fArray7[6] = 1.5f;
        fArray7[8] = -18.5f;
        float[] fArray8 = fArray7;
        this.ukun02.setParams(fArray8);
        this.ukun02.enableDTKFlag(262144);
        this.ukun02.disableDTKFlag(65536);
        this.ukun03 = new Enepc();
        this.ukun03.init(8705, 9, 4.0f, 0.0f, -22.0f, 0.0f);
        this.ukun03.id = 9;
        float[] fArray9 = new float[4];
        fArray9[0] = 4.0f;
        fArray9[2] = -22.0f;
        fArray9[3] = -1.0f;
        float[] fArray10 = fArray9;
        this.ukun03.setParams(11, 5, 9, 9, fArray10);
        float[] fArray11 = new float[9];
        fArray11[0] = 4.0f;
        fArray11[2] = -22.0f;
        fArray11[3] = 4.0f;
        fArray11[5] = -20.5f;
        fArray11[6] = 4.0f;
        fArray11[8] = -18.5f;
        float[] fArray12 = fArray11;
        this.ukun03.setParams(fArray12);
        this.ukun03.enableDTKFlag(262144);
        this.ukun03.disableDTKFlag(65536);
        this.ukun04 = new Enepc();
        this.ukun04.init(8705, 9, 0.0f, 0.0f, -14.0f, 180.0f);
        this.ukun04.id = 10;
        float[] fArray13 = new float[4];
        fArray13[2] = -14.0f;
        fArray13[3] = -1.0f;
        float[] fArray14 = fArray13;
        this.ukun04.setParams(11, 6, 10, 9, fArray14);
        float[] fArray15 = new float[9];
        fArray15[2] = -14.0f;
        fArray15[5] = -15.5f;
        fArray15[8] = -17.5f;
        float[] fArray16 = fArray15;
        this.ukun04.setParams(fArray16);
        this.ukun04.enableDTKFlag(262144);
        this.ukun04.disableDTKFlag(65536);
        this.ukun05 = new Enepc();
        this.ukun05.init(8705, 9, 3.0f, 0.0f, -14.0f, 180.0f);
        this.ukun05.id = 11;
        float[] fArray17 = new float[4];
        fArray17[0] = 3.0f;
        fArray17[2] = -14.0f;
        fArray17[3] = -1.0f;
        float[] fArray18 = fArray17;
        this.ukun05.setParams(11, 7, 11, 9, fArray18);
        float[] fArray19 = new float[9];
        fArray19[0] = 3.0f;
        fArray19[2] = -14.0f;
        fArray19[3] = 3.0f;
        fArray19[5] = -15.5f;
        fArray19[6] = 3.0f;
        fArray19[8] = -17.5f;
        float[] fArray20 = fArray19;
        this.ukun05.setParams(fArray20);
        this.ukun05.enableDTKFlag(262144);
        this.ukun05.disableDTKFlag(65536);
        this.ukun06 = new Enepc();
        this.ukun06.init(8705, 9, 0.0f, 0.0f, -19.0f, 0.0f);
        this.ukun06.id = 12;
        float[] fArray21 = new float[4];
        fArray21[0] = 2.5f;
        fArray21[2] = -23.5f;
        fArray21[3] = -1.0f;
        float[] fArray22 = fArray21;
        this.ukun06.setParams(11, 8, 12, 9, fArray22);
        float[] fArray23 = new float[9];
        fArray23[0] = 2.5f;
        fArray23[2] = -23.5f;
        fArray23[3] = 2.5f;
        fArray23[5] = -21.5f;
        fArray23[6] = 2.5f;
        fArray23[8] = -19.5f;
        float[] fArray24 = fArray23;
        this.ukun06.setParams(fArray24);
        this.ukun06.enableDTKFlag(262144);
        this.ukun06.disableDTKFlag(65536);
        this.ukun01.kickEnepc(4, 1);
        this.ukun02.kickEnepc(4, 1);
        this.ukun03.kickEnepc(4, 1);
        this.ukun04.kickEnepc(4, 1);
        this.ukun05.kickEnepc(4, 1);
        this.ukun06.kickEnepc(4, 1);
        this.ukun01.setVisible(false);
        this.ukun02.setVisible(false);
        this.ukun03.setVisible(false);
        this.ukun04.setVisible(false);
        this.ukun05.setVisible(false);
        this.ukun06.setVisible(false);
        this.item01 = new Uwamono(28677, -5.167f, 0.0f, -31.1f, 180.0f, 551);
        this.item01.SetSymbol(28684);
        this.item02 = new Uwamono(28677, -2.597f, 0.0f, -31.1f, 180.0f, 552);
        this.item02.SetSymbol(28684);
        this.item03 = new Uwamono(28677, 0.032f, 0.0f, -31.1f, 180.0f, 553);
        this.item03.SetSymbol(28684);
        this.item04 = new Uwamono(28677, 2.661f, 0.0f, -31.1f, 180.0f, 554);
        this.item04.SetSymbol(28685);
        this.item05 = new Uwamono(28677, 5.214f, 0.0f, -31.1f, 180.0f, 555);
        this.item05.SetSymbol(28685);
        Runtime.disable(524288);
        this.EF00 = new Effect(1742, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF00.disp(true);
        this.EF01 = new Effect(1748, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF01.disp(false);
        this.EF03 = new Effect(1750, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF03.disp(false);
        this.EF04 = new Effect(1749, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF04.disp(false);
        this.EF05 = new Effect(1751, 0.0f, 0.0f, 0.0f, 0.0f);
        this.EF05.disp(false);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Stage.setColor(2.0f, 2.0f, 2.0f);
        Stage.setVisible(-1, true);
        this.drill = new Uwamono(28691, 116, 132, 115);
        this.drill.DrillSetExParts(133, 134);
        this.drill.DrillSetSpeed(0.15f, 0.1f, 0.15f);
        this.drill.DrillSetReturnSpeed(0.3f, 0.5f, 0.3f);
        this.drill.DrillSetContainerRandom(true);
        this.drill.DrillGameCount(-1);
        this.drill.DrillSetContainer(18);
        this.drill.DrillCommand(8);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        Runtime.setDefocusQuick(0, 1, 8880, 1);
        Runtime.setDefocusQuick(1, 1, 7880, 1);
        Runtime.setDefocusQuick(2, 1, 6880, 1);
        Runtime.setDefocusQuick(3, 1, 5880, 1);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 8.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFPedestal(2, 14.58315f, 2.17586f, -17.39835f, 45.0f, 0.482f, 29.5f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(2, 1);
        this.cam0.setCFPedestal(3, 0.0f, 8.34388f, -14.390718f, 45.0f, -13.199867f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(3, 1);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 3.5f, 45.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.doa01 = new Uwamono(110, 40, '\u0003');
        this.doa02 = new Uwamono(111, 40, '\u0003');
        this.doa03 = new Uwamono(112, 40, '\u0003');
        this.doa04 = new Uwamono(113, 40, '\u0003');
        this.doa05 = new Uwamono(114, 40, '\u0003');
        if (Runtime.getFlags(4061, 1) == 0) {
            this.doa01.SetDoorType('\u0002');
        } else {
            this.doa01.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(4062, 1) == 0) {
            this.doa02.SetDoorType('\u0002');
        } else {
            this.doa02.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(4063, 1) == 0) {
            this.doa03.SetDoorType('\u0002');
        } else {
            this.doa03.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(4064, 1) == 0) {
            this.doa04.SetDoorType('\u0002');
        } else {
            this.doa04.SetDoorType('\u0004');
        }
        if (Runtime.getFlags(4065, 1) == 0) {
            this.doa05.SetDoorType('\u0002');
        } else {
            this.doa05.SetDoorType('\u0004');
        }
        Stage.setVisible(40, false);
        Stage.setVisible(133, false);
        Stage.setVisible(134, false);
        if (Runtime.getFlags(4061, 1) == 1) {
            Stage.setVisible(18, true);
            Stage.setVisible(99, false);
            Stage.setVisible(105, false);
        } else {
            Stage.setVisible(18, false);
            Stage.setVisible(99, true);
        }
        if (Runtime.getFlags(4062, 1) == 1) {
            Stage.setVisible(19, true);
            Stage.setVisible(100, false);
            Stage.setVisible(106, false);
        } else {
            Stage.setVisible(19, false);
            Stage.setVisible(100, true);
        }
        if (Runtime.getFlags(4063, 1) == 1) {
            Stage.setVisible(20, true);
            Stage.setVisible(101, false);
            Stage.setVisible(107, false);
        } else {
            Stage.setVisible(20, false);
            Stage.setVisible(101, true);
        }
        if (Runtime.getFlags(4064, 1) == 1) {
            Stage.setVisible(21, true);
            Stage.setVisible(102, false);
            Stage.setVisible(108, false);
        } else {
            Stage.setVisible(21, false);
            Stage.setVisible(102, true);
        }
        if (Runtime.getFlags(4065, 1) == 1) {
            Stage.setVisible(22, true);
            Stage.setVisible(103, false);
            Stage.setVisible(109, false);
        } else {
            Stage.setVisible(22, false);
            Stage.setVisible(103, true);
        }
        class Mapunits
                extends MAPUnit {
            private final  ST0160 this$0;

            Mapunits(ST0160 sT0160) {
                this.this$0 = sT0160;
            }

            void Dkaiten01() {
                int n = 0;
                float f = 0.0f;
                float f2 = 2.7f;
                float f3 = 0.84f;
                this.this$0.drill_01.setTranslate(-5.167f, 2.7f, -31.1f);
                while (n < 30) {
                    this.this$0.drill_01.setRotateY(f -= 6.0f);
                    this.this$0.drill_01.setTranslate(-5.167f, f2 -= 0.06f, -31.1f);
                    System.sleep(1);
                    if (++n != 30) continue;
                    Stage.setVisible(105, false);
                    Sound.effectPlay(65542);
                    this.this$0.EF001 = new Effect(601, -5.167f, 0.0f, -31.1f, 0.0f);
                    this.this$0.EF001.disp(true);
                    System.sleep(1);
                }
                while (n > 0) {
                    --n;
                    this.this$0.drill_01.setRotateY(f += 6.0f);
                    this.this$0.drill_01.setTranslate(-5.167f, f3 += 0.06f, -31.1f);
                    System.sleep(1);
                }
            }

            void Dkaiten02() {
                int n = 0;
                float f = 0.0f;
                float f2 = 2.7f;
                float f3 = 0.84f;
                this.this$0.drill_02.setTranslate(-2.597f, 2.7f, -31.1f);
                while (n < 30) {
                    this.this$0.drill_02.setRotateY(f -= 6.0f);
                    this.this$0.drill_02.setTranslate(-2.597f, f2 -= 0.06f, -31.1f);
                    System.sleep(1);
                    if (++n != 30) continue;
                    Stage.setVisible(106, false);
                    Sound.effectPlay(65542);
                    this.this$0.EF002 = new Effect(601, -2.597f, 0.0f, -31.1f, 0.0f);
                    this.this$0.EF002.disp(true);
                    System.sleep(1);
                }
                while (n > 0) {
                    --n;
                    this.this$0.drill_02.setRotateY(f += 6.0f);
                    this.this$0.drill_02.setTranslate(-2.597f, f3 += 0.06f, -31.1f);
                    System.sleep(1);
                }
            }

            void Dkaiten03() {
                int n = 0;
                float f = 0.0f;
                float f2 = 2.7f;
                float f3 = 0.84f;
                this.this$0.drill_03.setTranslate(0.032f, 2.7f, -31.1f);
                while (n < 30) {
                    this.this$0.drill_03.setRotateY(f -= 6.0f);
                    this.this$0.drill_03.setTranslate(0.032f, f2 -= 0.06f, -31.1f);
                    System.sleep(1);
                    if (++n != 30) continue;
                    Stage.setVisible(107, false);
                    Sound.effectPlay(65542);
                    this.this$0.EF003 = new Effect(601, 0.032f, 0.0f, -31.1f, 0.0f);
                    this.this$0.EF003.disp(true);
                    System.sleep(1);
                }
                while (n > 0) {
                    --n;
                    this.this$0.drill_03.setRotateY(f += 6.0f);
                    this.this$0.drill_03.setTranslate(0.032f, f3 += 0.06f, -31.1f);
                    System.sleep(1);
                }
            }

            void Dkaiten04() {
                int n = 0;
                float f = 0.0f;
                float f2 = 2.7f;
                float f3 = 0.84f;
                this.this$0.drill_04.setTranslate(2.661f, 2.7f, -31.1f);
                while (n < 30) {
                    this.this$0.drill_04.setRotateY(f -= 6.0f);
                    this.this$0.drill_04.setTranslate(2.661f, f2 -= 0.06f, -31.1f);
                    System.sleep(1);
                    if (++n != 30) continue;
                    Stage.setVisible(108, false);
                    Sound.effectPlay(65542);
                    this.this$0.EF004 = new Effect(601, 2.661f, 0.0f, -31.1f, 0.0f);
                    this.this$0.EF004.disp(true);
                    System.sleep(1);
                }
                while (n > 0) {
                    --n;
                    this.this$0.drill_04.setRotateY(f += 6.0f);
                    this.this$0.drill_04.setTranslate(2.661f, f3 += 0.06f, -31.1f);
                    System.sleep(1);
                }
            }

            void Dkaiten05() {
                int n = 0;
                float f = 0.0f;
                float f2 = 2.7f;
                float f3 = 0.84f;
                this.this$0.drill_05.setTranslate(5.214f, 2.7f, -31.1f);
                while (n < 30) {
                    this.this$0.drill_05.setRotateY(f -= 6.0f);
                    this.this$0.drill_05.setTranslate(5.214f, f2 -= 0.06f, -31.1f);
                    System.sleep(1);
                    if (++n != 30) continue;
                    Stage.setVisible(109, false);
                    Sound.effectPlay(65542);
                    this.this$0.EF005 = new Effect(601, 5.214f, 0.0f, -31.1f, 0.0f);
                    this.this$0.EF005.disp(true);
                    System.sleep(1);
                }
                while (n > 0) {
                    --n;
                    this.this$0.drill_05.setRotateY(f += 6.0f);
                    this.this$0.drill_05.setTranslate(5.214f, f3 += 0.06f, -31.1f);
                    System.sleep(1);
                }
            }

            void Kaiten1() {
                while (true) {
                    this.this$0.ang += this.this$0.ang2;
                    this.this$0.posT -= this.this$0.posT2;
                    this.this$0.tensi.setRotateY(this.this$0.ang);
                    this.this$0.tensi.setTranslate(-9.997f, this.this$0.posT, -19.435f);
                    System.sleep(1);
                }
            }

            void Tenmetu() {
                boolean bl = true;
                boolean bl2 = false;
                int n = 3;
                while (true) {
                    Stage.setVisible(74, bl);
                    Stage.setVisible(94, bl2);
                    System.sleep(n);
                    Stage.setVisible(85, bl);
                    Stage.setVisible(95, bl2);
                    System.sleep(n);
                    Stage.setVisible(75, bl);
                    Stage.setVisible(96, bl2);
                    System.sleep(n);
                    Stage.setVisible(76, bl);
                    Stage.setVisible(86, bl2);
                    System.sleep(n);
                    Stage.setVisible(77, bl);
                    Stage.setVisible(89, bl2);
                    System.sleep(n);
                    Stage.setVisible(78, bl);
                    Stage.setVisible(87, bl2);
                    System.sleep(n);
                    Stage.setVisible(78, bl);
                    Stage.setVisible(87, bl2);
                    System.sleep(n);
                    Stage.setVisible(79, bl);
                    Stage.setVisible(88, bl2);
                    System.sleep(n);
                    Stage.setVisible(80, bl);
                    Stage.setVisible(90, bl2);
                    System.sleep(n);
                    Stage.setVisible(81, bl);
                    Stage.setVisible(91, bl2);
                    System.sleep(n);
                    Stage.setVisible(82, bl);
                    Stage.setVisible(97, bl2);
                    System.sleep(n);
                    Stage.setVisible(83, bl);
                    Stage.setVisible(92, bl2);
                    System.sleep(n);
                    Stage.setVisible(84, bl);
                    Stage.setVisible(93, bl2);
                    System.sleep(n);
                    bl = !bl;
                    if (bl2) {
                        bl2 = false;
                        continue;
                    }
                    bl2 = true;
                }
            }
        }
        this.ugau = new Mapunits(this);
        this.ugau.mapUnit(38);
        this.ugau.start(1, "Tenmetu");
        this.dai = new Obj();
        this.dai.init(20641, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dai.setScale(0.7f, 1.2f, 0.7f);
        this.dai.start(4, null);
        this.dai.setTranslate(-0.1f, -0.02f, 0.0f);
        this.dai.setRotate(0.0f, 0.0f, 272.0f);
        this.dai2 = new Obj();
        this.dai2.init(20641, 0.0f, 0.0f, 0.0f, 0.0f);
        this.dai2.setScale(0.7f, 1.2f, 0.7f);
        this.dai2.start(4, null);
        this.dai2.setTranslate(-0.1f, -0.02f, 0.0f);
        this.dai2.setRotate(0.0f, 0.0f, 272.0f);
        this.tensi = new Mapunits(this);
        this.tensi.mapUnit(135);
        this.tensi.start(4, null);
        this.tensi.start(1, "Kaiten1");
        this.drill_01 = new Mapunits(this);
        this.drill_01.mapUnit(127);
        this.drill_01.start(4, null);
        this.drill_02 = new Mapunits(this);
        this.drill_02.mapUnit(128);
        this.drill_02.start(4, null);
        this.drill_03 = new Mapunits(this);
        this.drill_03.mapUnit(129);
        this.drill_03.start(4, null);
        this.drill_04 = new Mapunits(this);
        this.drill_04.mapUnit(130);
        this.drill_04.start(4, null);
        this.drill_05 = new Mapunits(this);
        this.drill_05.mapUnit(131);
        this.drill_05.start(4, null);
        this.npcset_1();
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        this.fade60 = new Effect(0);
        this.fade60.args[0] = -268435456;
        this.fade60.args[1] = 60;
        this.fade60.args[2] = 0;
        this.fade30 = new Effect(0);
        this.fade30.args[0] = -268435456;
        this.fade30.args[1] = 30;
        this.fade30.args[2] = 1;
        if (Runtime.checkItem(10, 7) != 0) {
            Runtime.setShootFlag(true);
            System.println("shoot on");
        } else {
            Runtime.setShootFlag(false);
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(1616, 1, 0, 0, 5, 10.45f, 0.55f, -23.5f, 0.0f);
        this.npc2 = new NPC_NORMAL(1571, 2, 0, 0, 5, 12.0f, 0.55f, -23.5f, 0.0f);
        this.npc3 = new Enepc();
        this.npc3.init(1585, 10, 10.5f, 0.0f, -17.5f, 180.0f);
        this.npc3.id = 3;
        float[] fArray = new float[4];
        fArray[0] = 10.5f;
        fArray[2] = -17.5f;
        fArray[3] = -1.0f;
        float[] fArray2 = fArray;
        this.npc3.setParams(0, 10, 3, 10, fArray2);
        float[] fArray3 = new float[24];
        fArray3[0] = 10.5f;
        fArray3[2] = -17.5f;
        fArray3[3] = 10.25f;
        fArray3[5] = -18.25f;
        fArray3[6] = 9.5f;
        fArray3[8] = -18.5f;
        fArray3[9] = 8.75f;
        fArray3[11] = -18.25f;
        fArray3[12] = 8.5f;
        fArray3[14] = -17.5f;
        fArray3[15] = 8.75f;
        fArray3[17] = -16.75f;
        fArray3[18] = 9.5f;
        fArray3[20] = -16.5f;
        fArray3[21] = 10.25f;
        fArray3[23] = -16.75f;
        float[] fArray4 = fArray3;
        this.npc3.setParams(fArray4);
        this.npc4 = new NPC_NORMAL(1552, 4, 0, 1, 6, 9.5f, 0.0f, -17.5f, 180.0f);
        this.npc5 = new NPC_NORMAL(1567, 5, 12, 9, 7, 4.0f, 0.0f, -7.0f, 0.0f);
        this.npc6 = new NPC_NORMAL(1609, 6, 0, 1, 11, -6.5f, 0.0f, -29.4f, 180.0f);
        this.npc7 = new NPC_NORMAL(1541, 6, 12, 9, 8, -6.25f, 0.0f, -15.5f, 270.0f);
        this.npc6.disableDTKFlag(3);
        this.npc6.setMotion(0, 4);
        this.npc6.setInvalidID(1);
        this.npc1.disableDTKFlag(8);
        this.npc2.disableDTKFlag(8);
        this.npc3.disableDTKFlag(8);
        this.npc4.disableDTKFlag(8);
        this.npc5.disableDTKFlag(8);
        this.npc6.disableDTKFlag(8);
        this.npc7.disableDTKFlag(8);
        this.npc1.setMotion(0, 9);
        this.npc2.setMotion(0, 27);
        this.dai.setParent(this.npc1, 36);
        this.dai2.setParent(this.npc2, 36);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
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

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(4, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(4, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(4, 16);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float[] fArray) {
            this.init(n, 0.0f, 0.0f, 0.0f, 0.0f);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(4, 16);
        }
    }
}

