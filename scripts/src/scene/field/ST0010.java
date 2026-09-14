import xeno.Camera;
import xeno.Chr;
import xeno.Effect;
import xeno.Enepc;
import xeno.Light;
import xeno.Stage;
import xeno.Unit;
import xeno.Uwamono;
import xeno.XenoConstants;
import xeno.map.MC_VOK01_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0010
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK01_PRJ {
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
    Enepc npc20;
    Enepc chr1;
    Enepc chr2;
    Unit unit1;
    Unit unit2;
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
    int talkFlag11;
    int talkFlag12;
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
    int touchFlag4;
    int touchFlag5;
    int touchFlag6;
    int touchFlag7;
    int touchFlag8;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    int S07A1;
    int S07A2;
    int S07A3;
    int S07C;
    int S08;
    int S010;
    int S012;
    int S015B;
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
    Uwamono teiten7;
    Uwamono teiten8;
    int page;
    String[] msg0001 = new String[]{"/[label(Allen)]", "Uh-oh...A summons to the bridge is never a good sign. You better hurry or you'll never hear the end of it, if the report is late.", "/[waitkey(64)]/[close()]"};
    String[] msg00011 = new String[]{"/[label(Allen)]", "The Captain is an easygoing guy, but the Commander on the other hand...", "/[waitkey(1)]/[clear()]", "Cherenkov, was it? He's not exactly the nicest guy...", "/[waitkey(64)]/[close()]"};
    String[] msg0002 = new String[]{"/[label(Allen)]", "You should hurry to the bridge. You'll never hear the end of it, if the report is late.", "/[waitkey(64)]/[close()]"};
    String[] msg0003 = new String[]{"/[label(Allen)]", "But still, what do you suppose the purpose of this fleet is? The other day, it looked as though they were retrieving something.", "/[waitkey(1)]/[clear()]", "Of course, I doubt they'd tell me even if I asked...", "/[waitkey(64)]/[close()]"};
    String[] msg0004 = new String[]{"/[label(Togashi)]", "Enlighten those jarheads, will ya? Tell them that developmental research is a living, breathing thing.", "/[waitkey(64)]/[close()]"};
    String[] msg0005 = new String[]{"/[label(Togashi)]", "No matter how you slice it, those military guys are just a rough bunch.", "/[waitkey(64)]/[close()]"};
    String[] msg0006 = new String[]{"/[label(Togashi)]", "Chief, just what did they expect to gain by assigning us to this fleet?", "/[waitkey(1)]/[clear()]", "How are we supposed to function as escorts, anyway? Surely they're not counting on a weapon that's still in development.", "/[waitkey(64)]/[close()]"};
    String[] msg0007 = new String[]{"/[label(Basil)]", "Chief, I've been meaning to ask you, but why are you so devoted to KOS-MOS' aesthetic sensitivity system? I think we should be focusing our research on KOS-MOS' outlying systems in order to develop her weaponry.", "/[waitkey(64)]/[close()]"};
    String[] msg0008 = new String[]{"/[label(Basil)]", "Chief, it seemed as though you intentionally caused the last test to fail...I can't tell if you want KOS-MOS to be functional or not, but remember that KOS-MOS\ndoesn't exist for you alone.", "/[waitkey(64)]/[close()]"};
    String[] msg0009 = new String[]{"/[label(Basil)]", "I'd really like to see our research make more definitive and substantial progress.", "/[waitkey(64)]/[close()]"};
    String[] msg0010 = new String[]{"/[label(Karol Hadji)]", "Umm...Chief, there was a strange interference earlier. Did anything unusual happen during the dive?", "/[waitkey(1)]/[clear()]", "There's no record of it in our log. I don't know what to make of it.", "/[waitkey(64)]/[close()]"};
    String[] msg0011 = new String[]{"/[label(Karol Hadji)]", "I think developing psychological algorithms might be a job more suited for psychics...", "/[waitkey(64)]/[close()]"};
    String[] msg0012 = new String[]{"/[label(Karol Hadji)]", "Chief, do you think KOS-MOS will really be effective against them? I'm worried.", "/[waitkey(64)]/[close()]"};
    String[] msg0013 = new String[]{"/[label(John Bell)]", "Chief, I understand you have your reasons, but a machine is only as meaningful as the functional goals it fulfills. Perhaps KOS-MOS would be happiest if she was able to demonstrate her full capabilities?", "/[waitkey(64)]/[close()]"};
    String[] msg0014 = new String[]{"/[label(John Bell)]", "Chief, your actions make me feel as though you're running away from your responsibilities as an engineer.", "/[waitkey(64)]/[close()]"};
    String[] msg0015 = new String[]{"/[label(Evelyn)]", "I can't wait to see KOS-MOS actually get up and around.", "/[waitkey(64)]/[close()]"};
    String[] msg0016 = new String[]{"/[label(Evelyn)]", "KOS-MOS' motor control system was designed so that her motions are more graceful than those of humans. Simulations are useful, but I'd like to get her in action, and check the load on her actuator and frame.", "/[waitkey(64)]/[close()]"};
    String[] msg0017 = new String[]{"/[label(Evelyn)]", "A humanoid machine, more graceful than any human. Isn't that incredible?", "/[waitkey(64)]/[clear()]"};
    String[] msg0018 = new String[]{"/[label(Shion)]", "Evelyn, is it just me, or are you starting to sound like Miyuki?", "/[waitkey(64)]/[clear()]"};
    String[] msg0019 = new String[]{"/[label(Evelyn)]", "W-what?! No way! I am NOT like Miyuki at all!", "/[waitkey(64)]/[close()]"};
    String[] msg0580BCD0 = new String[]{"/[label(Allen)]", "He hasn't been with us that long, so he still has a lot to learn about KOS-MOS.", "/[waitkey(1)]/[clear()]", "I'll have a talk with him later to explain things.", "/[waitkey(64)]/[close()]"};
    String[] msg05811C7D = new String[]{"/[label(Allen)]", "Chief, don't let what happened earlier bother you. Good luck.", "/[waitkey(64)]/[close()]"};
    String[] msg05819FE3 = new String[]{"/[label(Allen)]", "But still, what do you suppose the purpose of this fleet is? The other day, it looked as though they were retrieving something.", "/[waitkey(1)]/[clear()]", "Of course, I doubt they'd tell me even if I asked...", "/[waitkey(64)]/[close()]"};
    String[] msg05819FE31 = new String[]{"/[label(Shion)]", "Uh, umm...which way was the bridge again?", "/[waitkey(64)]/[clear()]"};
    String[] msg05819FE32 = new String[]{"/[label(Allen)]", "What? Not again, Chief! You really need to do something about your bad sense of direction.", "/[waitkey(1)]/[clear()]", "Okay, there's a map posted on the information board just outside of the lab. You can check out how to get to the bridge from there.", "/[waitkey(64)]/[close()]"};
    String[] msg05819FE33 = new String[]{"/[label(Allen)]", "If you get lost, just use the information boards to reorient yourself. Okay, Chief?", "/[waitkey(64)]/[close()]"};
    String[] msg1001 = new String[]{"/[label(Togashi)]", "Oh, hey Chief. Allen just left to take care of some errands.", "/[waitkey(64)]/[clear()]"};
    String[] msg1002 = new String[]{"/[label(Shion)]", "Oh? I guess we just missed each other. Thanks, I'll go look for him.", "/[waitkey(64)]/[close()]"};
    String[] msg1003 = new String[]{"/[label(Shion)]", "Did Allen mention what his errands were?", "/[waitkey(64)]/[clear()]"};
    String[] msg1004 = new String[]{"/[label(Togashi)]", "Uh...n, no. Maybe you should ask him yourself. Yep, I think that's the ticket...", "/[waitkey(64)]/[clear()]"};
    String[] msg1005 = new String[]{"/[label(Shion)]", "Hmm...that's was weird...", "/[waitkey(64)]/[close()]"};
    String[] msg1006 = new String[]{"/[label(Shion)]", "...Hey, Togashi, are you hiding something from me?", "/[waitkey(64)]/[clear()]"};
    String[] msg1007 = new String[]{"/[label(Togashi)]", "Huh? N-no. No way! I would never!", "/[waitkey(64)]/[close()]"};
    String[] msg1008 = new String[]{"/[label(Basil)]", "Chief, you're still here?", "/[waitkey(64)]/[close()]"};
    String[] msg1030 = new String[]{"/[label(Evelyn)]", "Oh, Chief, did you run into Allen?", "/[waitkey(64)]/[clear()]"};
    String[] msg1031 = new String[]{"/[label(Shion)]", "Not yet. Why? Was there something important?", "/[waitkey(64)]/[clear()]"};
    String[] msg1032 = new String[]{"/[label(Evelyn)]", "Oh, never mind if you haven't seen him yet.", "/[waitkey(1)]/[clear()]", "Chief, I can see you're going to have your work cut out for you.", "/[waitkey(64)]/[close()]"};
    String[] msg1009 = new String[]{"/[label(Karol Hadji)]", "There's a reason behind everything that happens in this world. Take for example, that experiment we conducted earlier. The fact that the accident occurred at all, was because we needed to experience it.", "/[waitkey(64)]/[clear()]"};
    String[] msg1010 = new String[]{"/[label(Shion)]", "Wow, I've never thought of it that way. Who said that?", "/[waitkey(64)]/[clear()]"};
    String[] msg1011 = new String[]{"/[label(Karol Hadji)]", "I did.", "/[waitkey(64)]/[clear()]"};
    String[] msg1012 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg1013 = new String[]{"/[label(John Bell)]", "Hey Chief? Allen here would like to ask you out on a da...", "/[waitkey(64)]/[clear()]"};
    String[] msg1014 = new String[]{"/[label(Togashi)]", "Hey! John Bell!", "/[waitkey(64)]/[clear()]"};
    String[] msg10141 = new String[]{"/[label(Togashi)]", "Umm...Oh! The Hilbert FAPP...give me the one that takes the Turek function into account!", "/[waitkey(64)]/[clear()]"};
    String[] msg1015 = new String[]{"/[label(John Bell)]", "What?! Right now? Why?", "/[waitkey(64)]/[clear()]"};
    String[] msg1016 = new String[]{"/[label(Togashi)]", "Cause I said so! Just do it!", "/[waitkey(1)]/[clear()]", "Oh, hey Chief, sorry about that, he's gotta go.", "/[waitkey(64)]/[clear()]"};
    String[] msg1017 = new String[]{"/[label(Shion)]", "...", "(Is something going on here?)", "/[waitkey(64)]/[close()]"};
    String[] msg1018 = new String[]{"/[label(Togashi)]", "Chieeef! Chief! John Bell is really busy right now! You're busy, right?! RIGHT, Johnny?!", "/[waitkey(64)]/[clear()]"};
    String[] msg1019 = new String[]{"/[label(John Bell)]", "N-no, not really.", "/[waitkey(64)]/[clear()]"};
    String[] msg1020 = new String[]{"/[label(Togashi)]", "You're busy, right?! You must be busy! Or rather, you SHOULD be busy!", "/[waitkey(64)]/[clear()]"};
    String[] msg1021 = new String[]{"/[label(John Bell)]", "Yeah...", "/[waitkey(64)]/[close()]"};
    String[] msg1022 = new String[]{"/[label(John Bell)]", "Apparently, I'm supposed to be busy right now.", "/[waitkey(64)]/[close()]"};
    String[] msg2004 = new String[]{"/[label(Allen)]", "Chief, shouldn't you hurry and report to the bridge?", "/[waitkey(64)]/[close()]"};
    String[] msg2007 = new String[]{"/[label(Allen)]", "Chief, I think you'd better hurry.", "/[waitkey(64)]/[close()]"};
    String[] msg2008 = new String[]{"/[label(Togashi)]", "Still dawdling? Well, I suppose it's important to stop and smell the flowers once in a while.", "/[waitkey(64)]/[close()]"};
    String[] msg2009 = new String[]{"/[label(Togashi)]", "You really ought to go and report to the bridge.", "/[waitkey(1)]/[clear()]", "After all, it isn't just the Captain you'll be reporting to.", "/[waitkey(64)]/[close()]"};
    String[] msg20091 = new String[]{"/[label(Togashi)]", "A little piece of advice since you seem to get lost easily, Chief. You need to use the central elevator in order to get to the bridge.", "/[waitkey(64)]/[close()]"};
    String[] msg2010 = new String[]{"/[label(Basil)]", "Anyway, right now, I think you should hurry and present the report to the Captain. Good luck!", "/[waitkey(64)]/[close()]"};
    String[] msg2011 = new String[]{"/[label(Karol Hadji)]", "Oh, Chief! Perfect timing.", "/[waitkey(1)]/[clear()]", "I think I've tracked down the cause of that abnormality in the Encephalon...", "/[waitkey(64)]/[clear()]"};
    String[] msg20111 = new String[]{"/[label(Shion)]", "Let me see...A forcible external interference?", "/[waitkey(64)]/[clear()]"};
    String[] msg20112 = new String[]{"/[label(Karol Hadji)]", "Yes, it was very brief as the interference only lasted 0.002 nanoseconds. It doesn't seem to be from a local source, so it's probably from the U.M.N.", "/[waitkey(64)]/[clear()]"};
    String[] msg20113 = new String[]{"/[label(Shion)]", "Wait a minute.\n", "/[waitkey(1)]/[clear()]", "KOS-MOS' simulator is supposed to be an isolated system.", "/[waitkey(64)]/[clear()]"};
    String[] msg20114 = new String[]{"/[label(Karol Hadji)]", "I know. Theoretically, this should be impossible.", "/[waitkey(1)]/[clear()]", "I'm going to investigate further, but I may not be able to find anything.", "/[waitkey(64)]/[close()]"};
    String[] msg2012 = new String[]{"/[label(John Bell)]", "I can't wait to see KOS-MOS awaken! Then I'll be recognized and honored for my part in her development.", "/[waitkey(64)]/[close()]"};
    String[] msg2013 = new String[]{"/[label(Evelyn)]", "An alabaster angel with a strong nanotube frame, powerful and flexible tetracytose muscles, and kevlar tendons all wrapped up in a slender form!", "/[waitkey(1)]/[clear()]", "I can't wait to see her elegance in action!", "/[waitkey(64)]/[close()]"};
    String[] msg20131 = new String[]{"/[label(Evelyn)]", "By the way, what did you think of Miyuki's M.W.S.?", "/[waitkey(64)]/[clear()]"};
    String[] msg20132 = new String[]{"/[label(Shion)]", "What did I think? Well, it might be useful for self-defense, but ultimately, it's just data inside a simulator -- who knows what will happen in actual combat situations?", "/[waitkey(64)]/[clear()]"};
    String[] msg20133 = new String[]{"/[label(Evelyn)]", "I agree.", "/[waitkey(1)]/[clear()]", "Maybe she thought so too, because she just emailed me saying she'd send the real thing.", "/[waitkey(64)]/[clear()]"};
    String[] msg20134 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msg20135 = new String[]{"/[label(Evelyn)]", "What's wrong Chief...?", "/[waitkey(64)]/[clear()]"};
    String[] msg20136 = new String[]{"/[label(Shion)]", "I-it's nothing. Just a little headache.", "/[waitkey(64)]/[close()]"};
    String[] msgNO5 = new String[]{"/[label()]", "We don't have lines for the 5th time yet.", "/[waitkey(64)]/[close()]"};
    String[] msgNO2 = new String[]{"/[label()]", "We don't have lines for the 2nd time yet.", "/[waitkey(64)]/[close()]"};
    String[] msg4001 = new String[]{"/[label(Allen)]", "Oh, Chief. H-how was it?", "/[waitkey(64)]/[clear()]"};
    String[] msg4002 = new String[]{"/[label(Shion)]", "Oh, nothing to worry about, Allen. It went perfectly! The entire bridge was blown away!", "/[waitkey(64)]/[clear()]"};
    String[] msg4003 = new String[]{"/[label(Allen)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg4004 = new String[]{"/[label(Allen)]", "Okay, so what really happened?", "/[waitkey(64)]/[clear()]"};
    String[] msg4005 = new String[]{"/[label(Shion)]", "The complete antithesis. Maybe they're just not interested.", "/[waitkey(1)]/[clear()]", "On the other hand, I'd have mixed feelings about it too, if the military really valued her as a weapon.", "/[waitkey(64)]/[close()]"};
    String[] msg4006 = new String[]{"/[label(Togashi)]", "Chief, welcome back.", "/[waitkey(1)]/[clear()]", "I know how talking in front of others is a tough thing to do.", "/[waitkey(64)]/[close()]"};
    String[] msg4007 = new String[]{"/[label(Togashi)]", "And the Captain was his usual self, with that noncommital smile on his face, right?", "/[waitkey(1)]/[clear()]", "Geez, I'd wager our assignment here was on the whim of another bigwig, too.", "/[waitkey(64)]/[close()]"};
    String[] msg4008 = new String[]{"/[label(Basil)]", "What was the result of your report?", "/[waitkey(1)]/[clear()]", "I think the Commander is really restless because of the way this development is handled.", "/[waitkey(64)]/[close()]"};
    String[] msg4009 = new String[]{"/[label(Karol Hadji)]", "So the Captain was his usual self?", "/[waitkey(1)]/[clear()]", "He's an intelligent man...surely he knows that KOS-MOS currently can't be counted on for combat use yet.", "/[waitkey(64)]/[close()]"};
    String[] msg4017 = new String[]{"/[label(Karol Hadji)]", "I've got a feeling that there's something going on, if he put us onboard knowing that.", "/[waitkey(64)]/[close()]"};
    String[] msg4010 = new String[]{"/[label(John Bell)]", "Chief! Welcome back! So how did it go? What was the military's reaction? What did they think of my KOS-MOS?!", "/[waitkey(64)]/[clear()]"};
    String[] msg4011 = new String[]{"/[label(Shion)]", "W-well, i-it was good. I think it went well.", "/[waitkey(64)]/[close()]"};
    String[] msg4012 = new String[]{"/[label(John Bell)]", "It did?! Of course it did! After all, I developed KOS-MOS!", "/[waitkey(64)]/[close()]"};
    String[] msg4013 = new String[]{"/[label(Evelyn)]", "Chief, don't let it get you down.", "/[waitkey(64)]/[clear()]"};
    String[] msg4014 = new String[]{"/[label(Shion)]", "Huh? What?", "/[waitkey(64)]/[clear()]"};
    String[] msg4015 = new String[]{"/[label(Evelyn)]", "It'll go better next time...", "/[waitkey(64)]/[clear()]"};
    String[] msg4016 = new String[]{"/[label(Shion)]", "What? No, they didn't treat me THAT badly...", "/[waitkey(64)]/[close()]"};
    String[] msg3001 = new String[]{"/[label(Researcher)]", "Chief, shall we try the start-up experiment again?", "/[waitkey(1)]/[clear()]", "If there are any items you didn't get last time, I suggest you go get them now. I'm sure they'll be useful in the real world too!", "/[waitkey(64)]/[close()]"};
    String[] msg3002 = new String[]{"/[label(Researcher)]", "Roger. Please take your place on the dive seat. Opening interconnection. Assistant Chief, please verify the protocol.", "/[waitkey(64)]/[close()]"};
    String[] msg4020 = new String[]{"/[label(Researcher)]", "Chief, shall we try the start-up experiment again?", "/[waitkey(64)]/[close()]"};
    String[] msg4021 = new String[]{"/[label(Researcher)]", "Roger. Please take your place on the dive seat. Opening interconnection. Assistant Chief...whoops, he isn't here. Togashi, please verify the protocol.", "/[waitkey(64)]/[close()]"};
    String[] msg9001 = new String[]{"/[label(Shion)]", "Oh, hey Janice. Taking a break?", "/[waitkey(64)]/[clear()]"};
    String[] msg9002 = new String[]{"/[label(Janice)]", "Oh, hi Chief, how are you?", "/[waitkey(1)]/[clear()]", "Why don't you take a break too? You won't hold up if you keep pushing yourself so hard.", "/[waitkey(64)]/[clear()]"};
    String[] msg9003 = new String[]{"/[label(Shion)]", "Thanks. But we're almost done. I'll be okay. No need to worry about me.", "/[waitkey(64)]/[clear()]"};
    String[] msg9004 = new String[]{"/[label(Janice)]", "Are you sure? All right, if you say so.", "/[waitkey(1)]/[clear()]", "Oh, that's right! There's some valuable info that I wanted to share with you.", "/[waitkey(1)]/[clear()]", "I was actually keeping it quiet, hoping you'd notice on your own. But since it doesn't look like you will, I guess I'll have to tell you.", "/[waitkey(64)]/[clear()]"};
    String[] msg9005 = new String[]{"/[label(Shion)]", "Valuable info?", "/[waitkey(64)]/[clear()]"};
    String[] msg9006 = new String[]{"/[label(Janice)]", "Uh-huh. You know that there is a \"red Vector box\" inside the Encephalon, right?", "/[waitkey(64)]/[clear()]"};
    String[] msg9007 = new String[]{"/[label(Shion)]", "You mean the \"red Vector box\" where all the equipment is kept?", "/[waitkey(64)]/[clear()]"};
    String[] msg9008 = new String[]{"/[label(Janice)]", "Yes, that's the one. We put a little something in there for you, to show our appreciation for all your hard work.", "/[waitkey(1)]/[clear()]", "We were hoping you would pick it up during the last experiment. But considering how things went, you definitely didn't have the time to find it.", "/[waitkey(1)]/[clear()]", "Anyway, I think you'll like it.", "/[waitkey(64)]/[close()]"};
    String[] msg9009 = new String[]{"/[label(Janice)]", "Don't forget Chief, it's the \"red Vector box.\" Make sure you pick it up the next time you Encephalon dive.", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};
    String[] msgMWS1 = new String[]{"/[label(Allen)]", "By the way, Chief, what did you think of the M.W.S. and the A.G.W.S.?", "/[waitkey(1)]/[clear()]", "It's just dummy data created inside the Encephalon, but it was pretty exciting stuff, wasn't it? If neither had problems during this test, all that remains are the functionality tests using actual models.", "/[waitkey(64)]/[clear()]"};
    String[] msgMWS2 = new String[]{"/[label(Shion)]", "It worked pretty well.", "/[waitkey(1)]/[clear()]", "The M.W.S. in particular felt right to me. I sense something contrived in that, though.", "/[waitkey(64)]/[clear()]"};
    String[] msgMWS3 = new String[]{"/[label(Allen)]", "Well, I hear Miyuki is closely involved with the M.W.S. I'm sure she's up to something again.", "/[waitkey(64)]/[clear()]"};
    String[] msgMWS4 = new String[]{"/[label(Shion)]", "Oh, by the way, could you send the Connection Gear data to Headquarters for me?", "/[waitkey(1)]/[clear()]", "You know the Vaporizer Plug-in that we used to destroy obstacles? I wonder if they're also going to manufacture it based on our test data?", "/[waitkey(64)]/[clear()]"};
    String[] msgMWS41 = new String[]{"/[label(Shion)]", "I swear, they pile everything they can on us, under the pretext of testing KOS-MOS...", "/[waitkey(64)]/[clear()]"};
    String[] msgMWS5 = new String[]{"/[label(Allen)]", "Well it's inevitable, considering they have a lot of other concerns, like the budget.", "/[waitkey(64)]/[close()]"};

    ST0010() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(-1.646f, 3.0f, -7.86f);
        this.camEV.setRotate(-19.136f, -456.769f, 0.0f);
        this.camEV.setFov(39.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        block0:
        switch (n2) {
            case 0: {
                if (this.S07A3 == 0) {
                    this.fadeIn.call(0);
                    System.sleep(23);
                    Runtime.setFlags(12, 1, 1);
                    Runtime.jumpEvent(1074);
                }
            }
            case 1: {
                if (Runtime.getFlags(25, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(7082, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(23, 1) != 1 || Runtime.getFlags(7057, 2) != 2) break;
                if (Runtime.getFlags(7066, 4) == 0) {
                    Runtime.mailArriveSet(5);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.setFlags(7066, 4, 1);
                    Runtime.setFlags(7082, 1, 1);
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
                            break block0;
                        }
                    }
                    Runtime.setPlayerControl(true);
                    break;
                }
                Runtime.mailArriveSet(10);
                Runtime.setPlayerControl(false);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msgMAIL1, 0);
                Runtime.setFlags(7066, 1, 2);
                Runtime.setFlags(7082, 1, 1);
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
                        break block0;
                    }
                }
                Runtime.setPlayerControl(true);
                break;
            }
        }
    }

    public void Talk_chr1(Enepc enepc) {
        this.Talk_chr1_1();
    }

    void Talk_chr1_1() {
        this.menu = Menu.create();
        this.menu.addQuery("Commence Encephalon Dive [Imaginary Space]?\n");
        this.menu.addItem("Go for it\nNo");
        this.menu.setCursor(1);
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                Runtime.setFlags(1024, 1, 1);
                System.println("*********FUJI_24をセットしたつもり**************");
                Runtime.evsSetRetPoint();
                System.println("Goto 239-3");
                Runtime.jumpCF(239, 3);
                return;
            }
        }
        System.println("cancel");
    }

    void Talk_chr1_2(Window window) {
    }

    void Talk_chr1_3(Window window) {
    }

    void Talk_chr1_4(Window window) {
    }

    public void Talk_chr2() {
        this.Talk_chr2_1();
    }

    void Talk_chr2_1() {
        this.menu = Menu.create();
        this.menu.addQuery("Commence Encephalon Dive [Imaginary Space]?\n");
        this.menu.addItem("Go for it\nNo");
        System.waitFor(this.menu);
        this.menu.setCursor(1);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                Runtime.setFlags(1024, 1, 1);
                System.println("*********FUJI_24をセットしたつもり**************");
                Runtime.evsSetRetPoint();
                System.println("Goto 239-3");
                Runtime.jumpCF(239, 3);
                return;
            }
        }
        System.println("cancel");
    }

    void Talk_chr2_2(Window window) {
    }

    void Talk_chr2_3(Window window) {
    }

    void Talk_chr2_4(Window window) {
    }

    public void Talk_npc1(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc1_5(window);
        } else if (this.S012 == 1) {
            this.Talk_npc1_4(window);
        } else if (this.S07C == 1) {
            this.Talk_npc1_3(window);
        } else if (this.S07A3 == 1) {
            this.Talk_npc1_1(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg0001, 0);
                ST0010.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg00011, 0);
                ST0010.waitPage(window, 64);
                return;
            }
            case 3: {
                window.print(this.msg0002, 0);
                ST0010.waitPage(window, 64);
                return;
            }
            case 4: {
                window.print(this.msg05819FE31, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg05819FE32, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg05819FE33, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc1_3(Window window) {
    }

    void Talk_npc1_4(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                window.print(this.msg2004, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg2007, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc1_5(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg4001, 0);
                ST0010.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg4002, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg4003, 0);
                ST0010.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg4004, 0);
        ST0010.waitPage(window, 64);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg4005, 0);
        ST0010.waitPage(window, 64);
        Runtime.disable(65536);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc2_5(window);
        } else if (this.S012 == 1) {
            this.Talk_npc2_4(window);
        } else if (this.S07C == 1) {
            this.Talk_npc2_3(window);
        } else if (this.S07A3 == 1) {
            this.Talk_npc2_1(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                window.print(this.msg0007, 0);
                ST0010.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0008, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0009, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
        window.print(this.msg1008, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc2_4(Window window) {
        window.print(this.msg2010, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc2_5(Window window) {
        window.print(this.msg4008, 0);
        ST0010.waitPage(window, 64);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc3_5(window);
        } else if (this.S012 == 1) {
            this.Talk_npc3_4(window);
        } else if (this.S07C == 1) {
            this.Talk_npc3_3(window);
        } else if (this.S07A3 == 1) {
            this.Talk_npc3_1(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg0004, 0);
                ST0010.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0005, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0006, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg1001, 0);
                ST0010.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg1002, 0);
                ST0010.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg1003, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg1004, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg1005, 0);
                ST0010.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg1006, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg1007, 0);
        ST0010.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc3_4(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg2008, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg2009, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc3_5(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                window.print(this.msg4006, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg4007, 0);
        ST0010.waitPage(window, 64);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc4_5(window);
        } else if (this.S012 == 1) {
            this.Talk_npc4_4(window);
        } else if (this.S07C == 1) {
            this.Talk_npc4_3(window);
        } else if (this.S07A3 == 1) {
            this.Talk_npc4_1(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg0013, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0014, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc4_3(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                this.npc4.kickEnepc(1, 9);
                window.print(this.msg1013, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg1014, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg10141, 0);
                ST0010.waitPage(window, 64);
                this.npc4.disableDTKFlag(2);
                this.npc4.moveEnepc(17, 0.0f, 0.1f, 10);
                System.sleep(10);
                window.print(this.msg1015, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg1016, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg1017, 0);
                ST0010.waitPage(window, 64);
                this.npc4.moveEnepc(17, 60.0f, 0.1f, 10);
                this.npc4.enableDTKFlag(2);
                System.sleep(10);
                return;
            }
            case 2: {
                this.npc4.kickEnepc(0, 10);
                window.print(this.msg1018, 0);
                ST0010.waitPage(window, 64);
                this.npc4.kickEnepc(0, 9);
                this.npc4.disableDTKFlag(2);
                this.npc4.moveEnepc(17, 0.0f, 0.1f, 10);
                window.print(this.msg1019, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg1020, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg1021, 0);
                ST0010.waitPage(window, 64);
                this.npc4.moveEnepc(17, 60.0f, 0.1f, 10);
                this.npc4.disableDTKFlag(2);
                this.npc4.enableDTKFlag(4);
                System.sleep(10);
                return;
            }
        }
        this.npc4.disableDTKFlag(2);
        this.npc4.enableDTKFlag(4);
        window.print(this.msg1022, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc4_4(Window window) {
        window.print(this.msg2012, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc4_5(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg4010, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg4011, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg4012, 0);
        ST0010.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc5_5(window);
        } else if (this.S012 == 1) {
            this.Talk_npc5_4(window);
        } else if (this.S07C == 1) {
            this.Talk_npc5_3(window);
        } else if (this.S07A3 == 1) {
            this.Talk_npc5_1(window);
        } else {
            this.Talk_npc5_1(window);
        }
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg0015, 0);
                ST0010.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0016, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg0017, 0);
        ST0010.waitPage(window, 64);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg0018, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg0019, 0);
        ST0010.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc5_3(Window window) {
        Runtime.enable(65536);
        window.print(this.msg1030, 0);
        ST0010.waitPage(window, 64);
        this.player.mtn(9, 1, 1.0f, true);
        window.print(this.msg1031, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg1032, 0);
        ST0010.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc5_4(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                window.print(this.msg2013, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg20131, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20132, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20133, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20134, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20135, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20136, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc5_5(Window window) {
        Runtime.enable(65536);
        window.print(this.msg4013, 0);
        ST0010.waitPage(window, 64);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg4014, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg4015, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg4016, 0);
        ST0010.waitPage(window, 64);
        Runtime.disable(65536);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc6_5(window);
        } else if (this.S012 == 1) {
            this.Talk_npc6_4(window);
        } else if (this.S07C == 1) {
            this.Talk_npc6_3(window);
        } else if (this.S07A3 == 1) {
            this.Talk_npc6_1(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg0010, 0);
                ST0010.waitPage(window, 64);
                return;
            }
            case 2: {
                window.print(this.msg0011, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg0012, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc6_3(Window window) {
        Runtime.enable(65536);
        window.print(this.msg1009, 0);
        ST0010.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg1010, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg1011, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg1012, 0);
        ST0010.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc6_4(Window window) {
        window.print(this.msg2011, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20111, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20112, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20113, 0);
        ST0010.waitPage(window, 64);
        window.print(this.msg20114, 0);
        ST0010.waitPage(window, 64);
    }

    void Talk_npc6_5(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                window.print(this.msg4009, 0);
                ST0010.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg4017, 0);
        ST0010.waitPage(window, 64);
    }

    public void Talk_npc7(Enepc enepc) {
        if (this.S015B == 1) {
            this.Talk_npc7_1();
        } else if (this.S012 == 1) {
            this.Talk_npc7_1();
        } else if (this.S07C == 1) {
            this.Talk_npc7_3();
        } else if (this.S07A3 == 1) {
            this.Talk_npc7_1();
        } else {
            this.Talk_npc7_1();
        }
    }

    void Talk_npc7_1() {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg3001, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        this.menu.setCursor(1);
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg3002, 0);
                ST0010.waitPage(this.win, 64);
                Runtime.setFlags(1024, 1, 1);
                System.println("*********FUJI_24をセットしたつもり**************");
                Runtime.setPlayerControl(true);
                System.println("Goto 239-3");
                Runtime.evsSetRetPoint();
                Runtime.jumpCF(239, 3);
                return;
            }
        }
        Runtime.setPlayerControl(true);
    }

    void Talk_npc7_3() {
        Runtime.setPlayerControl(false);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg4020, 0);
        System.waitFor(this.win);
        this.menu = Menu.create();
        this.menu.addItem("Yes\nNo");
        this.menu.setCursor(1);
        System.waitFor(this.menu);
        this.selected = this.menu.getSelected();
        switch (this.selected) {
            case 0: {
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg4021, 0);
                ST0010.waitPage(this.win, 64);
                Runtime.setFlags(1024, 1, 1);
                System.println("*********FUJI_24をセットしたつもり**************");
                Runtime.setPlayerControl(true);
                System.println("Goto 239-3");
                Runtime.evsSetRetPoint();
                Runtime.jumpCF(239, 3);
                return;
            }
        }
        Runtime.setPlayerControl(true);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        switch (Runtime.getFlags(7041, 1)) {
            case 0: {
                window.print(this.msg9001, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg9002, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg9003, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg9004, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg9005, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg9006, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg9007, 0);
                ST0010.waitPage(window, 64);
                window.print(this.msg9008, 0);
                ST0010.waitPage(window, 64);
                Runtime.setFlags(7041, 1, 1);
                return;
            }
        }
        window.print(this.msg9009, 0);
        ST0010.waitPage(window, 64);
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(20, 1);
                break;
            }
            case 1: {
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.setTranslate(-2.13f, 5.0438f, -22.911f);
                this.player.setRotate(0.0f, 0.0f, 0.0f);
                System.println("２Ｆへ移動");
                System.sleep(15);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                break;
            }
            case 2: {
                Runtime.setPlayerControl(false);
                Runtime.enable(65536);
                this.player.setTranslate(-3.635f, 1.043f, -21.371f);
                this.player.setRotate(0.0f, 0.0f, 0.0f);
                System.println("１Ｆへ移動");
                System.sleep(15);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                break;
            }
        }
    }

    void init() {
        this.teiten2 = new Uwamono(28690, -5.0f, 0.0f, -4.0f, 0.0f);
        this.teiten2.SetBgm(196645);
        this.teiten3 = new Uwamono(28690, -5.0f, 0.0f, -9.0f, 0.0f);
        this.teiten3.SetBgm(196645);
        this.teiten4 = new Uwamono(28690, 6.0f, 0.0f, -9.0f, 0.0f);
        this.teiten4.SetBgm(196645);
        this.teiten6 = new Uwamono(28690, 6.0f, 0.0f, 1.0f, 0.0f);
        this.teiten6.SetBgm(196645);
        this.teiten7 = new Uwamono(28690, 0.5f, 0.0f, 1.0f, 0.0f);
        this.teiten7.SetBgm(196610);
        this.teiten8 = new Uwamono(28690, 0.5f, 0.0f, -8.0f, 0.0f);
        this.teiten8.SetBgm(196610);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setIdLightCol(1, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 1, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightCol(1, 2, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightCol(1, 3, 0.0f, 0.0f, 0.0f);
        Runtime.setIdLightVec(1, 1, 0.0f, 1.0f, 0.0f);
        Runtime.setIdLightVec(1, 2, 0.0f, 1.0f, 0.5f);
        Runtime.setIdLightVec(1, 3, 0.0f, 1.0f, -0.5f);
        this.S07A3 = Runtime.getFlags(12, 1);
        this.S07C = Runtime.getFlags(14, 1);
        this.S012 = Runtime.getFlags(19, 1);
        this.S015B = Runtime.getFlags(23, 1);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Stage.setVisible(5, false);
        Stage.setVisible(6, false);
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 5.0f, 40.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 10.5f, 40.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 4.0f, 40.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        Runtime.setRegister(1, 32896);
        Runtime.setRegister(2, 0x800000);
        Runtime.setRegister(3, 32768);
        Runtime.setRegister(4, 128);
        new Uwamono(28678, 0.49f, 1.1f, 5.43f);
        this.unit1 = new Obj();
        this.unit1.init(20555, 0.0f, 0.0f, 0.0f, 180.0f);
        this.unit1.setTranslate(0.46f, 0.19f, -8.04f);
        this.unit1.setRotate(0.0f, 0.0f, 0.0f);
        this.unit1.renderCommand(530);
        this.unit2 = new Obj();
        this.unit2.init(20583, 0.0f, 0.0f, 0.0f, 180.0f);
        this.unit2.setTranslate(0.5f, 0.0f, 0.7f);
        this.unit2.setRotate(0.0f, 180.0f, 0.0f);
        this.unit2.renderCommand(530);
        this.doorA = new Uwamono(8, 40, '\u0001');
        if (this.S07A3 == 0) {
            this.doorA.SetDoorType('\u0002');
        } else {
            this.doorA.SetDoorType('\u0004');
        }
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
        if (Runtime.getFlags(12, 1) == 0 && Runtime.getFlags(7050, 1) == 0) {
            System.println("OutFriend設定  完了");
            Runtime.setOutFriend(2);
            Runtime.setPartyData(0x1010000, 3);
            Runtime.setPartyData(65538, 1);
            Runtime.setPartyData(0x1010004, 0);
            Runtime.setPartyData(65542, 0);
            Runtime.setPartyData(0x1010008, 0);
            Runtime.setPartyData(65546, 0);
            Runtime.setPartyData(16777260, 1);
            Runtime.setFlags(7050, 1, 1);
        }
        if (this.S015B == 1) {
            this.npcset_5();
        } else if (this.S012 == 1) {
            this.npcset_1();
        } else if (this.S07C == 1) {
            this.npcset_3();
        } else if (this.S07A3 == 1) {
            this.npcset_1();
        } else {
            this.npcset_1();
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(263, 1, 0, 67, 3, 2.5f, 0.05f, -7.3f, 200.0f);
        this.npc2 = new NPC_NORMAL(515, 2, 0, 76, 25, -3.81f, 0.0f, -4.1f, -90.0f);
        this.npc3 = new NPC_NORMAL(301, 3, 0, 76, 10, 4.83f, 0.0f, 0.9f, 90.0f);
        this.npc4 = new NPC_NORMAL(514, 4, 0, 76, 25, 4.83f, 0.0f, -9.1f, 90.0f);
        this.npc5 = new NPC_NORMAL(517, 5, 0, 2, 14, -1.32f, 0.0f, -16.07f, 270.0f);
        this.npc6 = new NPC_NORMAL(518, 6, 0, 76, 13, -3.81f, 0.0f, -9.04f, -90.0f);
        this.npc7 = new NPC_NORMAL(516, 7, 0, 76, 9, 1.55f, 0.0f, 0.88f, 270.0f);
        this.npc8 = new NPC_NORMAL(518, 8, 0, 14, 14, 4.48f, 5.2f, -21.9f, 300.0f);
        this.npc1.enableDTKFlag(262144);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 1);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 3);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 2);
        this.npc6.setInvalidID(1);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc7.setInvalidID(1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 1);
        this.npc8.setMotion(0, 10);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        if (Runtime.getFlags(7129, 1) == 1) {
            return;
        }
        this.npc20 = new NPC_NORMAL(263, 20, 0, 67, 3, 100.0f, 0.0f, 100.0f, 180.0f);
        this.npc20.talkto("TalkNPC11");
        this.npc20.disableDTKFlag(131072);
        this.npc20.disableDTKFlag(8);
        this.npc20.setInvalidID(1);
        this.npc20.start(1, "MWS");
    }

    void npcset_2() {
    }

    void npcset_3() {
        this.npc2 = new NPC_NORMAL(515, 2, 0, 76, 25, -3.81f, 0.0f, -4.1f, -90.0f);
        this.npc3 = new NPC_NORMAL(301, 3, 0, 76, 10, 4.83f, 0.0f, 0.9f, 90.0f);
        this.npc4 = new NPC_NORMAL(514, 4, 0, 14, 5, 5.15f, 0.0f, -0.19f, 45.0f);
        this.npc5 = new NPC_NORMAL(517, 5, 0, 2, 14, -1.32f, 0.0f, -16.07f, 270.0f);
        this.npc6 = new NPC_NORMAL(518, 6, 0, 76, 13, -3.81f, 0.0f, -9.04f, -90.0f);
        this.npc7 = new NPC_NORMAL(516, 7, 0, 76, 9, 1.55f, 0.0f, 0.88f, 270.0f);
        this.npc8 = new NPC_NORMAL(518, 8, 0, 14, 14, 4.48f, 5.2f, -21.9f, 300.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 2);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 3);
        this.npc4.setMotion(0, 10);
        this.npc4.enableDTKFlag(262144);
        this.npc6.setInvalidID(1);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc7.setInvalidID(1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 1);
        this.npc8.setMotion(0, 12);
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
    }

    void npcset_4() {
    }

    void npcset_5() {
        this.npc1 = new NPC_NORMAL(263, 1, 0, 2, 3, 2.47f, 0.0f, -5.94f, 180.0f);
        this.npc2 = new NPC_NORMAL(515, 2, 0, 76, 25, -3.81f, 0.0f, -4.1f, -90.0f);
        this.npc3 = new NPC_NORMAL(301, 3, 0, 76, 10, 4.83f, 0.0f, 0.9f, 90.0f);
        this.npc4 = new NPC_NORMAL(514, 4, 0, 76, 25, 4.83f, 0.0f, -9.1f, 90.0f);
        this.npc5 = new NPC_NORMAL(517, 5, 0, 2, 14, -1.32f, 0.0f, -16.07f, 270.0f);
        this.npc6 = new NPC_NORMAL(518, 6, 0, 76, 13, -3.81f, 0.0f, -9.04f, -90.0f);
        this.npc7 = new NPC_NORMAL(516, 7, 0, 76, 9, 1.55f, 0.0f, 0.88f, 270.0f);
        this.npc8 = new NPC_NORMAL(518, 8, 0, 14, 14, 4.48f, 5.2f, -21.9f, 300.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 2);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 3);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 1);
        this.npc6.setInvalidID(1);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc7.setInvalidID(1);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 1);
        this.npc8.setMotion(0, 12);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
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
            this.renderCommand(530);
        }
    }

    class Obj
            extends Unit {
        Obj() {
        }
    }

    class NPC_NORMAL
            extends Enepc {
        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(3, 16);
            this.renderCommand(530);
        }

        NPC_NORMAL(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(3, 16);
            this.renderCommand(530);
        }

        void MWS() {
            Runtime.setPlayerControl(false);
            Runtime.setFlags(7129, 1, 1);
            Runtime.enable(65536);
            ST0010.this.player.look_char(ST0010.this.npc1);
            ST0010.this.npc1.kickEnepc(4, 1);
            ST0010.this.npc1.kickEnepc(1, 9);
            System.sleep(10);
            ST0010.this.win = Window.create();
            ST0010.this.win.setSize(4, 45);
            ST0010.this.win.setLocation(15, 305);
            ST0010.this.win.print(ST0010.this.msgMWS1, 0);
            ST0010.waitPage(ST0010.this.win, 64);
            ST0010.this.npc1.kickEnepc(1, 10);
            ST0010.this.player.mtn(11, 1, 1.0f, true);
            ST0010.this.win.print(ST0010.this.msgMWS2, 0);
            ST0010.waitPage(ST0010.this.win, 64);
            Runtime.disable(65536);
            ST0010.this.npc1.kickEnepc(1, 9);
            ST0010.this.win.print(ST0010.this.msgMWS3, 0);
            ST0010.waitPage(ST0010.this.win, 64);
            Runtime.enable(65536);
            ST0010.this.player.mtn(10, 1, 1.0f, true);
            ST0010.this.npc1.kickEnepc(1, 10);
            ST0010.this.win.print(ST0010.this.msgMWS4, 0);
            ST0010.waitPage(ST0010.this.win, 64);
            Runtime.disable(65536);
            Runtime.enable(65536);
            ST0010.this.player.mtn(11, 1, 1.0f, true);
            ST0010.this.npc1.kickEnepc(1, 10);
            ST0010.this.win.print(ST0010.this.msgMWS41, 0);
            ST0010.waitPage(ST0010.this.win, 64);
            ST0010.this.npc1.kickEnepc(0, 9);
            ST0010.this.win.print(ST0010.this.msgMWS5, 0);
            ST0010.waitPage(ST0010.this.win, 64);
            System.sleep(30);
            ST0010.this.player.look_default();
            ST0010.this.npc1.kickEnepc(4, 0);
            Runtime.disable(65536);
            Runtime.setPlayerControl(true);
        }
    }

    class Dummy
            extends Enepc {
        Dummy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(0, 0);
            this.renderCommand(530);
        }

        Dummy(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4, float[] fArray) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5, fArray);
            this.setShadow(0, 0);
            this.renderCommand(530);
        }
    }
}

