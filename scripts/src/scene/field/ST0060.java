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
import xeno.map.MC_VOK06_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST0060
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_VOK06_PRJ {
    int smd = 0;
    Player player;
    Camera cam0;
    Camera camEV;
    Menu menu;
    Window win;
    Camera cam1;
    int entrance;
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
    Enepc npc15;
    Unit[] unit;
    Unit Star;
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
    int touchFlag1;
    int touchFlag2;
    int touchFlag3;
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
    int S014A;
    int S014B;
    int S015B;
    int rea_talk = 0;
    int BUTTON_F = 0;
    Effect fadeIn;
    Effect fadeOut;
    Effect fade;
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
    Effect EF14;
    Effect EF15;
    Effect EF16;
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
    Uwamono teiten10;
    Unit box;
    int page;
    String[] msg0001 = new String[]{"/[label(Shion)]", "Let's see, I just need to check the Realians lying on the maintenance beds, right?", "/[waitkey(64)]/[clear()]"};
    String[] msg0002 = new String[]{"/[label(Caspase)]", "Yes, thank you for your help.", "/[waitkey(64)]/[close()]"};
    String[] msg5A07C5B6 = new String[]{"/[label(Shion)]", "They're definitely rejecting the data. It has to do with the motivator and the motivating body. The sector that discriminates between self and others needs a\ntune-up.", "/[waitkey(1)]/[clear()]", "You have to make their self-preservation clear to them, or they get confused about the correlation between themselves and their orders.", "/[waitkey(64)]/[clear()]"};
    String[] msg5A082564 = new String[]{"/[label(Caspase)]", "I see...", "/[waitkey(64)]/[close()]"};
    String[] msg4A07C5B6 = new String[]{"/[label()]", "Let's see, so basically,\n", "/[waitkey(1)]/[clear()]", "it means that if they cannot clearly distinguish themselves from others, they aren't able to tell where they exist within the world.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A082564 = new String[]{"/[label(Caspase)]", "If we could keep the Realians constantly activated, I suppose these types of things wouldn't happen. But during a battle tour, it's not exactly easy...", "/[waitkey(64)]/[close()]"};
    String[] msg3A08A8CA = new String[]{"/[label(Caspase)]", "I specialize in biochemistry, so I understand purely physical matters, but I have a hard time grasping psychological software.", "/[waitkey(64)]/[close()]"};
    String[] msg3A94221A = new String[]{"/[label()]", "We had a full-time counselor do the servicing before we left port. Unfortunately, when it's a long tour and we only have maintenance beds, problems always crop up.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A94221B = new String[]{"/[label(Shion)]", "Long missions are stressful for us, too.", "/[waitkey(1)]/[clear()]", "Besides, these guys were just born, so it's not surprising.", "/[waitkey(64)]/[close()]"};
    String[] msg3A9481C7 = new String[]{"/[label()]", "I placed a request for a counselor, but I was denied due to chronic shortages of manpower.", "/[waitkey(64)]/[close()]"};
    String[] msg387C5DA1 = new String[]{"/[label()]", "With some light counseling, they seem to have no problems at first glance. But errors show up when we try to install new software.", "/[waitkey(64)]/[clear()]"};
    String[] msg387C5DA2 = new String[]{"/[label(Shion)]", "That's because their compensator program makes it seem like you are getting through to them. But if you examine more closely, errors start showing up in their subconscious. In other words, the stress eventually gets to them.", "/[waitkey(1)]/[clear()]", "Basically, they're the same as infants.", "/[waitkey(64)]/[close()]"};
    String[] msg39F0DE1C = new String[]{"/[label()]", "I'm sure at Vector, you must have a much larger and more sophisticated tuning department.", "/[waitkey(1)]/[clear()]", "In the army, the Realians are becoming a more vital part of the force, but we are still way behind when it comes to maintenance and tuning.", "/[waitkey(64)]/[close()]"};
    String[] msg39F13DC8 = new String[]{"/[label()]", "Well, I guess it just means many of the army higher-ups only think of Realians as disposable tools.", "/[waitkey(64)]/[close()]"};
    String[] msg38F1A19E = new String[]{"/[label(Shion)]", "How is that? It's just a temporary fix, though.", "/[waitkey(64)]/[clear()]"};
    String[] msg38F1A19F = new String[]{"/[label()]", "Yes. I cannot express it appropriately, but I feel like some weight has been lifted. Everything feels a bit brighter. Thank you very much.", "/[waitkey(64)]/[close()]"};
    String[] msg38F2014B = new String[]{"/[label(Shion)]", "Good! If the world looks brighter to you, then you're fine!", "/[waitkey(64)]/[close()]"};
    String[] msg383176AD = new String[]{"/[label(Shion)]", "That should do it. Well? Do you have any dizziness or anything?", "/[waitkey(64)]/[clear()]"};
    String[] msg383176AE = new String[]{"/[label()]", "No, I am fine.", "/[waitkey(1)]/[clear()]", "We are combat models, so even our training causes heavy wear and tear. Regular tune-ups like this are very much appreciated.", "/[waitkey(64)]/[clear()]"};
    String[] msg383176AF = new String[]{"/[label(Shion)]", "No no no, I didn't really do much.", "/[waitkey(64)]/[close()]"};
    String[] msg3831D65B = new String[]{"/[label(Shion)]", "To be honest, I wish you guys didn't have to fight.", "/[waitkey(64)]/[clear()]"};
    String[] msg3831D65C = new String[]{"/[label()]", "You are very kind.", "/[waitkey(1)]/[clear()]", "But I was developed for combat. To deny combat is to deny my very existence.", "/[waitkey(64)]/[clear()]"};
    String[] msg3831D65D = new String[]{"/[label(Shion)]", "You're right, I'm sorry if I said anything that confused you.", "/[waitkey(64)]/[close()]"};
    String[] msg39E7A8A8 = new String[]{"/[label(Shion)]", "Okay, your values are stable. I don't see any particular problems.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E7A8A9 = new String[]{"/[label()]", "Understood. Then I will return to my duties.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E7A8AA = new String[]{"/[label(Shion)]", "Wait a minute, I think you could express a bit more joy. Maybe your emotional expression isn't working right...", "/[waitkey(64)]/[close()]"};
    String[] msg39E7A8AB = new String[]{"/[label()]", "I am a Realian developed for combat. I believe such fluctuations of emotions are unnecessary for my duties.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E7A8AC = new String[]{"/[label(Shion)]", "That's true, but...", "/[waitkey(64)]/[close()]"};
    String[] msg39E7A8AD = new String[]{"/[label(Shion)]", "But you know, the phrase, \"Thank you,\" is the first step in communicating. Do you understand?", "/[waitkey(64)]/[clear()]"};
    String[] msg39E7A8AE = new String[]{"/[label()]", "Understood. \"Thank you.\"", "/[waitkey(64)]/[clear()]"};
    String[] msg39E7A8AF = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg39E8085B = new String[]{"/[label()]", "Is there something else necessary?", "/[waitkey(64)]/[close()]"};
    String[] msg37959E29 = new String[]{"/[label(Shion)]", "Hmm, I wonder what's wrong? Your brain waves seem to be chaotic.", "/[waitkey(64)]/[clear()]"};
    String[] msg37959E2A = new String[]{"/[label()]", "Um, ever since I was assigned to the Woglinde, I haven't been the same. I've slowly lost mental composure, and my body has begun to manifest minute spasms.", "/[waitkey(64)]/[clear()]"};
    String[] msg37959E2B = new String[]{"/[label(Shion)]", "Hmm, the shape of these waves...could it be fear of battle?", "/[waitkey(64)]/[close()]"};
    String[] msg37959E2C = new String[]{"/[label()]", "Fear of fighting? I was developed for combat, how could that be?!", "/[waitkey(64)]/[clear()]"};
    String[] msg37959E2D = new String[]{"/[label(Shion)]", "It's okay, don't worry about it. There's nothing wrong with you. The emotion called fear is natural to all living things.", "/[waitkey(64)]/[close()]"};
    String[] msg3795FDDA = new String[]{"/[label()]", "A Realian designed for combat, feeling fear?", "/[waitkey(64)]/[clear()]"};
    String[] msg3795FDDB = new String[]{"/[label(Shion)]", "Even the greatest, strongest soldier feels fear and terror. To conquer your fears is a key to becoming stronger.", "/[waitkey(64)]/[clear()]"};
    String[] msg3795FDDC = new String[]{"/[label()]", "But I am a Realian.", "/[waitkey(64)]/[clear()]"};
    String[] msg3795FDDD = new String[]{"/[label(Shion)]", "That doesn't matter, you're the same as us humans.", "/[waitkey(1)]/[clear()]", "You just happened to be born differently, that's all.", "/[waitkey(64)]/[close()]"};
    String[] msg391D10B0 = new String[]{"/[label(Shion)]", "Is there anything that feels wrong?", "/[waitkey(64)]/[clear()]"};
    String[] msg391D10B1 = new String[]{"/[label()]", "...", "/[waitkey(64)]/[clear()]"};
    String[] msg391D10B2 = new String[]{"/[label(Shion)]", "Hey, are you okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg391D10B3 = new String[]{"/[label()]", "...zzz", "/[waitkey(64)]/[clear()]"};
    String[] msg391D10B4 = new String[]{"/[label(Shion)]", "Oh, dear, he's fast asleep. We'll give him some peace and quiet for now.", "/[waitkey(64)]/[close()]"};
    String[] msg391D7060 = new String[]{"/[label()]", "...zzz", "/[waitkey(64)]/[close()]"};
    String[] msg3AABEEB1 = new String[]{"/[label(Shion)]", "Hmm, nothing seems particularly unusual. Are you not feeling well?", "/[waitkey(64)]/[clear()]"};
    String[] msg3AABEEB2 = new String[]{"/[label()]", "I have been studying fables and proverbs. But I am unable to use them properly, and I am always being laughed at.", "/[waitkey(64)]/[clear()]"};
    String[] msg3AABEEB5 = new String[]{"/[label(Shion)]", "Hmm, but your linguistic center doesn't seem to have any abnormalities.", "/[waitkey(64)]/[close()]"};
    String[] msg3AABEEB6 = new String[]{"/[label()]", "I see, I must be just thinking too much. They do say \"better safe than curry,\" so I figured I had nothing to lose in being overly cautious.", "/[waitkey(64)]/[clear()]"};
    String[] msg3AABEEB7 = new String[]{"/[label(Shion)]", "...Say, do you know any other proverbs?", "/[waitkey(64)]/[clear()]"};
    String[] msg3AABEEB8 = new String[]{"/[label()]", "Others?", "/[waitkey(1)]/[clear()]", "\"Casting food before swine.\"\n", "\"Adding insult to perjury.\"\n", "\"Roast meat prey upon the meal.\"", "/[waitkey(64)]/[clear()]"};
    String[] msg3AABEEB9 = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg3AAC4E66 = new String[]{"/[label(Shion)]", "Where...did you learn those?", "/[waitkey(64)]/[clear()]"};
    String[] msg3AAC4E67 = new String[]{"/[label()]", "A soldier knowledgeable in proverbs taught me. Is there something wrong with me after all?", "/[waitkey(64)]/[clear()]"};
    String[] msg3AAC4E68 = new String[]{"/[label(Shion)]", "W-well...first, I think you should change teachers.", "/[waitkey(64)]/[close()]"};
    String[] msg3A4F6617 = new String[]{"/[label()]", "Umm...lately I find functional impediments occurring when communicating with a certain person. My pulse becomes rapid, perspiration increases, and I become unable to articulate well.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A4F6618 = new String[]{"/[label(Shion)]", "Hmm, that's odd. I wonder if it's an OS bug?", "/[waitkey(64)]/[close()]"};
    String[] msg3A4F66181 = new String[]{"/[label(Shion)]", "It doesn't hinder you in your normal duties?", "/[waitkey(64)]/[clear()]"};
    String[] msg3A4F6619 = new String[]{"/[label()]", "No, as long as I do not come in contact with that person, the abnormalities do not seem to appear.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A4F661A = new String[]{"/[label(Shion)]", "What could it be? Is that person spreading a virus or something?", "/[waitkey(64)]/[close()]"};
    String[] msg3A50BB3C = new String[]{"/[label(Shion)]", "So who is this person?", "/[waitkey(64)]/[clear()]"};
    String[] msg3A50BB3D = new String[]{"/[label()]", "Allen, a member of Vector First R&D Division.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A50BB3E = new String[]{"/[label(Shion)]", "What? Allen?", "/[waitkey(64)]/[clear()]"};
    String[] msg3A50BB3F = new String[]{"/[label()]", "Yes, I feel lightheaded whenever I see Allen, and I become confused and embarrassed. I become unable to converse normally.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A50BB40 = new String[]{"/[label(Shion)]", "Hmm, you have some interesting symptoms.", "/[waitkey(64)]/[close()]"};
    String[] msg3A50BB41 = new String[]{"/[label()]", "(I see, she has a crush on Allen...)", "/[waitkey(64)]/[close()]"};
    String[] msg3A513EA7 = new String[]{"/[label(Shion)]", "Hmm, it's no good. I don't know what the cause is!", "/[waitkey(64)]/[close()]"};
    String[] msg3A513EA8 = new String[]{"/[label()]", "(What? She can't tell?!)", "/[waitkey(64)]/[close()]"};
    String[] msg3A513EA9 = new String[]{"/[label(Shion)]", "Could it possibly be that Allen is emitting strange waves?", "/[waitkey(64)]/[close()]"};
    String[] msg3A513EAA = new String[]{"/[label()]", "(No, that's not it, she probably has a crush on Allen...)", "/[waitkey(64)]/[close()]"};
    String[] msg3A513EAB = new String[]{"/[label(Shion)]", "I guess we'll need to examine Allen!", "/[waitkey(64)]/[close()]"};
    String[] msg3A513EAC = new String[]{"/[label()]", "(She's off...she's WAY off base.)", "/[waitkey(64)]/[close()]"};
    String[] msg57AD5D50 = new String[]{"I'm sorry,\n", "I'll add it later!", "/[waitkey(64)]/[close()]"};
    String[] msg3A949678 = new String[]{"/[label(Caspase)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg3A949679 = new String[]{"/[label()]", "Really, that Lieutenant Virgil is just so slimy.", "/[waitkey(1)]/[clear()]", "Did you see? He's wearing a women's bracelet. I'm sure he thinks it's cool, but that just makes him smug and even less likeable.", "/[waitkey(1)]/[clear()]", "Shion, you feel the same way too, don't you?", "/[waitkey(64)]/[close()]"};
    String[] msg387CD200 = new String[]{"/[label()]", "Um, Shion, what is this \"Emergency Override Code\" for the Realians?", "/[waitkey(64)]/[clear()]"};
    String[] msg387CD201 = new String[]{"/[label(Shion)]", "I'm sorry.", "/[waitkey(1)]/[clear()]", "It's classified information that only a handful of people know, even at HQ. So I'm afraid I can't tell you.", "/[waitkey(64)]/[clear()]"};
    String[] msg387CD202 = new String[]{"/[label()]", "Really? Awww...", "/[waitkey(64)]/[close()]"};
    String[] msg387D31AD = new String[]{"/[label()]", "A secret code that only Vector's developers know. Hmmm...I want to know.", "/[waitkey(64)]/[close()]"};
    String[] msg39F1527B = new String[]{"/[label()]", "That Lieutenant Virgil is definitely a skilled soldier, but he doesn't take good care of Realians. He won't hesitate to use them as shields or to abandon them in battle.", "/[waitkey(1)]/[clear()]", "Apparently, something happened in the past, and ever since then, he's come to hate Realians, I hear.", "/[waitkey(1)]/[clear()]", "Even if that were the truth, I still can't forgive his actions.", "/[waitkey(64)]/[close()]"};
    String[] msg39F1B227 = new String[]{"/[label()]", "Lieutenant Virgil's actions are unforgivable!", "/[waitkey(64)]/[close()]"};
    String[] msg38F215FD = new String[]{"/[label(Shion)]", "Okay, it looks good now. But be careful, it's just a temporary fix.", "/[waitkey(64)]/[clear()]"};
    String[] msg38F215FE = new String[]{"/[label()]", "Okay, but umm...are you all right, Miss Uzuki?", "/[waitkey(64)]/[clear()]"};
    String[] msg38F215FF = new String[]{"/[label(Shion)]", "Huh? Why?", "/[waitkey(64)]/[clear()]"};
    String[] msg38F21600 = new String[]{"/[label()]", "Because Lieutenant Virgil just...", "/[waitkey(64)]/[clear()]"};
    String[] msg38F21601 = new String[]{"/[label(Shion)]", "Oh, don't worry about me, I'm fine! But I've caused you pain. I'm sorry.", "/[waitkey(64)]/[close()]"};
    String[] msg38F275AE = new String[]{"/[label(Shion)]", "But don't worry, I've never thought of you guys as tools.", "/[waitkey(64)]/[clear()]"};
    String[] msg38F275AF = new String[]{"/[label()]", "Yes, your feelings for us are very clear to us.", "/[waitkey(64)]/[close()]"};
    String[] msg3831EB0C = new String[]{"/[label(Shion)]", "I'm sorry. Don't let what the Lieutenant said bother you.", "/[waitkey(64)]/[clear()]"};
    String[] msg3831EB0D = new String[]{"/[label()]", "Thank you, Miss Uzuki. We know very well that you care about us.", "/[waitkey(1)]/[clear()]", "It's true, we follow any order to fight the enemy.", "/[waitkey(1)]/[clear()]", "But we do not think of ourselves as tools. We fight in order to protect everyone, and we are proud to do so.", "/[waitkey(64)]/[close()]"};
    String[] msg38324ABA = new String[]{"/[label()]", "We sacrifice ourselves to preserve that honor.", "/[waitkey(64)]/[clear()]"};
    String[] msg38324ABB = new String[]{"/[label(Shion)]", "Yes, but that kind of life is a bit sad...", "/[waitkey(64)]/[close()]"};
    String[] msg39E81D07 = new String[]{"/[label()]", "May I ask a question?", "/[waitkey(64)]/[clear()]"};
    String[] msg39E81D08 = new String[]{"/[label(Shion)]", "Please, go ahead.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E81D09 = new String[]{"/[label()]", "Do you hate Lieutenant Virgil?", "/[waitkey(64)]/[clear()]"};
    String[] msg39E81D0A = new String[]{"/[label(Shion)]", "What? Well, I just met him, but the way he thinks is a little...", "/[waitkey(64)]/[clear()]"};
    String[] msg39E81D0B = new String[]{"/[label()]", "Lieutenant Virgil is an excellent soldier. I believe he thinks of us as objects because of his excellence.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E81D0C = new String[]{"/[label(Shion)]", "That might be true, but...I don't like such a heartless way of thinking.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E81D0D = new String[]{"/[label()]", "I'm sorry. I am not yet able to comprehend that emotion.", "/[waitkey(64)]/[close()]"};
    String[] msg39E87CB9 = new String[]{"/[label()]", "Human emotions are very difficult. I still do not understand them well.", "/[waitkey(64)]/[close()]"};
    String[] msg37961288 = new String[]{"/[label(Shion)]", "What's wrong?  You're trembling.", "/[waitkey(64)]/[clear()]"};
    String[] msg37961289 = new String[]{"/[label()]", "Um, I...am feeling fear.", "/[waitkey(1)]/[clear()]", "When I heard what Lieutenant Virgil said, my body began to shake, and I cannot control it.", "/[waitkey(1)]/[clear()]", "I understand clearly what Lieutenant Virgil said. But my body seems to be rejecting it.", "/[waitkey(64)]/[close()]"};
    String[] msg37967236 = new String[]{"/[label(Shion)]", "It's okay, don't worry, you're not a disposable tool. I won't let anyone treat you that way!", "/[waitkey(64)]/[clear()]"};
    String[] msg37967237 = new String[]{"/[label()]", "T-thank you.", "/[waitkey(64)]/[close()]"};
    String[] msg391D850F = new String[]{"/[label()]", "......zzz", "/[waitkey(64)]/[clear()]"};
    String[] msg391D8510 = new String[]{"/[label(Shion)]", "He really seems to be sleeping comfortably.", "/[waitkey(64)]/[close()]"};
    String[] msg3AAC6310 = new String[]{"/[label()]", "I do not think I could get myself to like Lieutenant Virgil very much. But I hear he is quite influential among those in this unit.", "/[waitkey(1)]/[clear()]", "\"Ill weeds grow a mace,\" they say!", "/[waitkey(64)]/[clear()]"};
    String[] msg3AAC6311 = new String[]{"/[label(Shion)]", "Okay, that's it. Next time I come by, I'm bringing you a proper proverb dictionary.", "/[waitkey(64)]/[close()]"};
    String[] msg3A50CFEB = new String[]{"/[label()]", "My body seems to manifest a particular response to Lieutenant Virgil, too.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A50CFEC = new String[]{"/[label(Shion)]", "Do you get short of breath and become unable to talk?", "/[waitkey(64)]/[clear()]"};
    String[] msg3A50CFED = new String[]{"/[label()]", "No, my stomach feels nauseous and my spine feels tingly.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A50CFEE = new String[]{"/[label(Shion)]", "Oh, don't worry about that, I feel the same way.", "/[waitkey(64)]/[close()]"};
    String[] msg38346575 = new String[]{"/[label(Caspase)]", "Oh, Shion, did you go to the bridge already? If you don't hurry, Virgil's sarcasm will rear its ugly head again.", "/[waitkey(64)]/[clear()]"};
    String[] msg38346576 = new String[]{"/[label(Shion)]", "Ha ha ha,\n", "I'd like to avoid that if possible.", "/[waitkey(64)]/[close()]"};
    String[] msg3A950AD8 = new String[]{"/[label()]", "Oh, Miss Uzuki.\n", "Weren't you summoned to the bridge? I think it would be better if you hurried. After all, on the bridge is the infamous Commander Cherenkov. He won't let you off easily.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A950AD9 = new String[]{"/[label(Shion)]", "You're kidding...\n", "Is the Commander that scary?!", "/[waitkey(64)]/[clear()]"};
    String[] msg3A950ADA = new String[]{"/[label()]", "I wouldn't say scary, but there's just something peculiar about him.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A950ADB = new String[]{"/[label(Shion)]", "...", "/[waitkey(64)]/[close()]"};
    String[] msg3A956A88 = new String[]{"/[label()]", "The Commander was just transferred, so I haven't exactly talked to him much. But I'm uncomfortable around him.", "/[waitkey(64)]/[close()]"};
    String[] msg387D465F = new String[]{"/[label()]", "Oh, Miss Uzuki! Perfect timing. I was hoping you could take a look at this error here.", "/[waitkey(64)]/[clear()]"};
    String[] msg387D4660 = new String[]{"/[label(Shion)]", "Certainly.", "/[waitkey(1)]/[clear()]", "Let's see, when you run this code like this, it'll give you an error. Here's a semi-trick you can use -- if you interrupt the command...", "/[waitkey(64)]/[clear()]"};
    String[] msg387D4661 = new String[]{"/[label()]", "Oh, you're right! It's working now.", "/[waitkey(64)]/[clear()]"};
    String[] msg387D4662 = new String[]{"/[label(Shion)]", "Yes, depending on the OS version, these little tricks become necessary.", "/[waitkey(64)]/[close()]"};
    String[] msg39F1C6DA = new String[]{"/[label()]", "Oh, Miss Uzuki, you're still here? Wow, you must be quite a big shot to be able to ignore a summons from the bridge!", "/[waitkey(64)]/[clear()]"};
    String[] msg39F1C6DB = new String[]{"/[label(Shion)]", "What? No,\n", "I'm not really ignoring it.", "/[waitkey(64)]/[clear()]"};
    String[] msg39F1C6DC = new String[]{"/[label()]", "Oh, but you know, I hear that you do some reckless things.", "/[waitkey(1)]/[clear()]", "Allen was lamenting that he was going to get a bunch of ulcers.", "/[waitkey(64)]/[clear()]"};
    String[] msg39F1C6DD = new String[]{"/[label(Shion)]", "What? Really? Is that true? I'm not doing it on purpose, though.", "/[waitkey(64)]/[close()]"};
    String[] msg39F22689 = new String[]{"/[label()]", "A young, beautiful, talented scientist who won't compromise her work! That's so great, I really admire that.", "/[waitkey(64)]/[clear()]"};
    String[] msg39F2268A = new String[]{"/[label(Shion)]", "I-I don't know about beautiful and talented, but it's true I try not to compromise my work as much as possible.", "/[waitkey(64)]/[close()]"};
    String[] msg48F215FD = new String[]{"/[label(Shion)]", "Okay, it looks good now. But be careful, it's just a temporary fix.", "/[waitkey(64)]/[clear()]"};
    String[] msg48F215FE = new String[]{"/[label()]", "Okay, but umm...are you all right, Miss Uzuki?", "/[waitkey(64)]/[clear()]"};
    String[] msg48F215FF = new String[]{"/[label(Shion)]", "Huh? Why?", "/[waitkey(64)]/[clear()]"};
    String[] msg48F21600 = new String[]{"/[label()]", "Because Lieutenant Virgil just...", "/[waitkey(64)]/[clear()]"};
    String[] msg48F21601 = new String[]{"/[label(Shion)]", "Oh, don't worry about me, I'm fine! But I've caused you pain. I'm sorry.", "/[waitkey(64)]/[close()]"};
    String[] msg48F275AE = new String[]{"/[label(Shion)]", "But don't worry, I've never thought of you guys as tools.", "/[waitkey(64)]/[clear()]"};
    String[] msg48F275AF = new String[]{"/[label()]", "Yes, your feelings for us are very clear to us.", "/[waitkey(64)]/[close()]"};
    String[] msg4831EB0C = new String[]{"/[label(Shion)]", "I'm sorry. Don't let what the Lieutenant said bother you.", "/[waitkey(64)]/[clear()]"};
    String[] msg4831EB0D = new String[]{"/[label()]", "Thank you, Miss Uzuki. We know very well that you care about us.", "/[waitkey(1)]/[clear()]", "It's true, we follow any order to fight the enemy.", "/[waitkey(1)]/[clear()]", "But we do not think of ourselves as tools. We fight in order to protect everyone, and we are proud to do so.", "/[waitkey(64)]/[close()]"};
    String[] msg48324ABA = new String[]{"/[label()]", "We sacrifice ourselves to preserve that honor.", "/[waitkey(64)]/[clear()]"};
    String[] msg48324ABB = new String[]{"/[label(Shion)]", "Yes, but that kind of life is a bit sad...", "/[waitkey(64)]/[close()]"};
    String[] msg49E81D07 = new String[]{"/[label()]", "May I ask a question?", "/[waitkey(64)]/[clear()]"};
    String[] msg49E81D08 = new String[]{"/[label(Shion)]", "Please, go ahead.", "/[waitkey(64)]/[clear()]"};
    String[] msg49E81D09 = new String[]{"/[label()]", "Do you hate Lieutenant Virgil?", "/[waitkey(64)]/[clear()]"};
    String[] msg49E81D0A = new String[]{"/[label(Shion)]", "What? Well, I just met him, but the way he thinks is a little...", "/[waitkey(64)]/[clear()]"};
    String[] msg49E81D0B = new String[]{"/[label()]", "Lieutenant Virgil is an excellent soldier. I believe he thinks of us as objects because of his excellence.", "/[waitkey(64)]/[clear()]"};
    String[] msg49E81D0C = new String[]{"/[label(Shion)]", "That might be true, but...I don't like such a heartless way of thinking.", "/[waitkey(64)]/[clear()]"};
    String[] msg49E81D0D = new String[]{"/[label()]", "I'm sorry. I am not yet able to comprehend that emotion.", "/[waitkey(64)]/[close()]"};
    String[] msg49E87CB9 = new String[]{"/[label()]", "Human emotions are very difficult. I still do not understand them well.", "/[waitkey(64)]/[close()]"};
    String[] msg47961288 = new String[]{"/[label(Shion)]", "What's wrong?  You're trembling.", "/[waitkey(64)]/[clear()]"};
    String[] msg47961289 = new String[]{"/[label()]", "Um, I...am feeling fear.", "/[waitkey(1)]/[clear()]", "When I heard what Lieutenant Virgil said, my body began to shake, and I cannot control it.", "/[waitkey(1)]/[clear()]", "I understand what Lieutenant Virgil said well. But my body seems to be rejecting it.", "/[waitkey(64)]/[close()]"};
    String[] msg47967236 = new String[]{"/[label(Shion)]", "It's okay, don't worry, you're not a disposable tool. I won't let anyone treat you that way!", "/[waitkey(64)]/[clear()]"};
    String[] msg47967237 = new String[]{"/[label()]", "T-thank you.", "/[waitkey(64)]/[close()]"};
    String[] msg491D850F = new String[]{"/[label()]", "......zzz", "/[waitkey(64)]/[clear()]"};
    String[] msg491D8510 = new String[]{"/[label(Shion)]", "He really seems to be sleeping comfortably.", "/[waitkey(64)]/[close()]"};
    String[] msg4AAC6310 = new String[]{"/[label()]", "I do not think I could get myself to like Lieutenant Virgil very much. But I hear he is quite influential among those in this unit.", "/[waitkey(1)]/[clear()]", "\"Ill weeds grow a mace,\" they say!", "/[waitkey(64)]/[clear()]"};
    String[] msg4AAC6311 = new String[]{"/[label(Shion)]", "Okay, that's it, next time I come, I'm bringing you a proper proverb dictionary.", "/[waitkey(64)]/[close()]"};
    String[] msg4A50CFEB = new String[]{"/[label()]", "My body seems to manifest a particular response to Lieutenant Virgil, too.", "/[waitkey(64)]/[clear()]"};
    String[] msg4A50CFEC = new String[]{"/[label(Shion)]", "Do you get short of breath and become unable to talk?", "/[waitkey(64)]/[clear()]"};
    String[] msg4A50CFED = new String[]{"/[label()]", "No, my stomach feels nauseous and my spine feels tingly.", "/[waitkey(64)]/[clear()]"};
    String[] msg4A50CFEE = new String[]{"/[label(Shion)]", "Oh, don't worry about that, I feel the same way.", "/[waitkey(64)]/[clear()]"};
    String[] msg3834D9D4 = new String[]{"/[label()]", "Oh, Miss Uzuki, you've reported to the bridge already?", "/[waitkey(1)]/[clear()]", "From the look on your face, it seems you got chewed out pretty good by Commander Cherenkov.", "/[waitkey(64)]/[clear()]"};
    String[] msg3834D9D5 = new String[]{"/[label(Shion)]", "Oh, can you tell?", "/[waitkey(64)]/[clear()]"};
    String[] msg3834D9D6 = new String[]{"/[label(Caspase)]", "Yes, you've got \"I hate that stupid Commander!\" written all over your face.", "/[waitkey(64)]/[clear()]"};
    String[] msg3834D9D7 = new String[]{"/[label(Shion)]", "What? Really?!", "/[waitkey(64)]/[close()]"};
    String[] msg38353983 = new String[]{"/[label(Caspase)]", "Ha ha ha ha. Cherenkov can be a bit much sometimes.", "/[waitkey(64)]/[close()]"};
    String[] msg3A957F37 = new String[]{"/[label()]", "Oh, Miss Uzuki! How did it go?", "/[waitkey(64)]/[clear()]"};
    String[] msg3A957F38 = new String[]{"/[label(Shion)]", "Yeah, well, it went so-so, I guess.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A957F39 = new String[]{"/[label()]", "Oh, really? How boring. I was expecting more.", "/[waitkey(64)]/[clear()]"};
    String[] msg3A957F3A = new String[]{"/[label(Shion)]", "Just what were you expecting?!", "/[waitkey(64)]/[clear()]"};
    String[] msg3A957F3B = new String[]{"/[label()]", "Oh, uh, umm...well, back to work! Back to work!", "/[waitkey(64)]/[close()]"};
    String[] msg387DBABE = new String[]{"/[label()]", "Oh, how are you?", "/[waitkey(64)]/[clear()]"};
    String[] msg387DBABF = new String[]{"/[label(Shion)]", "How's the tuning progressing? Going okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg387DBAC0 = new String[]{"/[label()]", "Yes, thanks to you, Miss Uzuki, I think we'll manage to get it done. After this, we're all on standby until their mission starts.", "/[waitkey(64)]/[clear()]"};
    String[] msg387DBAC1 = new String[]{"/[label(Shion)]", "If I had it my way, this mission will end without anything happening.", "/[waitkey(64)]/[close()]"};
    String[] msg39F23B39 = new String[]{"/[label()]", "You're so amazing, Miss Uzuki. You designed KOS-MOS, and you're the next rising star of Vector.", "/[waitkey(1)]/[clear()]", "KOS-MOS, our Athena against the Gnosis! With the Hilbert Effect to boot, the Gnosis won't stand a chance!", "/[waitkey(64)]/[close()]"};
    String[] msg38F2FEBB = new String[]{"/[label()]", "Did something happen on the bridge?", "/[waitkey(64)]/[clear()]"};
    String[] msg38F2FEBC = new String[]{"/[label(Shion)]", "What?! Nothing really. Why?", "/[waitkey(64)]/[clear()]"};
    String[] msg38F2FEBD = new String[]{"/[label()]", "Well, you looked a little down.", "/[waitkey(64)]/[clear()]"};
    String[] msg38F2FEBE = new String[]{"/[label(Shion)]", "Not at all! I'm fine, really. I don't get discouraged that easily!", "/[waitkey(64)]/[clear()]"};
    String[] msg38F2FEBF = new String[]{"/[label()]", "Miss Uzuki, you're not good at hiding things, are you?", "/[waitkey(64)]/[close()]"};
    String[] msg3832D3CA = new String[]{"/[label(Shion)]", "Okay, everything checks out. You can go back to your duties.", "/[waitkey(64)]/[clear()]"};
    String[] msg3832D3CB = new String[]{"/[label()]", "Thank you, Miss Uzuki.\n", "/[waitkey(64)]/[clear()]"};
    String[] msg3832D3CC = new String[]{"/[label(Shion)]", "Call me Shion.", "/[waitkey(64)]/[clear()]"};
    String[] msg3832D3CD = new String[]{"/[label()]", "Thank you, Shion.", "/[waitkey(64)]/[clear()]"};
    String[] msg3832D3CE = new String[]{"/[label(Shion)]", "Listen, honor and pride are important, but it's not worth throwing your life away, okay?", "/[waitkey(64)]/[clear()]"};
    String[] msg3832D3CF = new String[]{"/[label()]", "Yes, of course. We do not want to die, either.", "/[waitkey(64)]/[clear()]"};
    String[] msg3832D3D0 = new String[]{"/[label(Shion)]", "Okay, I'm glad you feel that way.", "/[waitkey(64)]/[close()]"};
    String[] msg3833337C = new String[]{"/[label(Shion)]", "Do your best with your duties!", "/[waitkey(64)]/[clear()]"};
    String[] msg3833337D = new String[]{"/[label()]", "Yes, thank you very much.", "/[waitkey(64)]/[close()]"};
    String[] msg39E905C5 = new String[]{"/[label(Shion)]", "How's that? Can you express your emotions a bit better now?", "/[waitkey(1)]/[clear()]", "You guys aren't tools of warfare. You need to become more capable of communicating with a variety of people.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E905C6 = new String[]{"/[label()]", "Is that an order? If it is an order, I will work hard to express my emotions.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E905C7 = new String[]{"/[label(Shion)]", "What? No, it's not an order. Besides, I have no right to give you orders.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E905C8 = new String[]{"/[label()]", "Then, I cannot accept the request, as I do not recognize its necessity for my duties.", "/[waitkey(64)]/[clear()]"};
    String[] msg39E905C9 = new String[]{"/[label(Shion)]", "...I see you have no problem being stubborn.", "/[waitkey(64)]/[close()]"};
    String[] msg39E96575 = new String[]{"/[label()]", "Anything else?", "/[waitkey(64)]/[close()]"};
    String[] msg3796FB46 = new String[]{"/[label(Shion)]", "Oh, you're new, aren't you?", "/[waitkey(64)]/[clear()]"};
    String[] msg3796FB47 = new String[]{"/[label()]", "Hey, my sweet honeycake. To you, I could reveal everything.", "/[waitkey(64)]/[clear()]"};
    String[] msg3796FB48 = new String[]{"/[label(Shion)]", "Wha...?", "/[waitkey(64)]/[clear()]"};
    String[] msg3796FB49 = new String[]{"/[label()]", "Oh, how cute! Your puzzled look is sexy, baby!", "/[waitkey(64)]/[clear()]"};
    String[] msg3796FB4A = new String[]{"/[label(Shion)]", "Baby...? Um, what's going on?", "/[waitkey(64)]/[clear()]"};
    String[] msg3796FB4B = new String[]{"/[label()]", "Oh...it seems the soldiers were having some fun teaching him stuff and it's altered his personality.", "/[waitkey(64)]/[close()]"};
    String[] msg37975AF7 = new String[]{"/[label(Shion)]", "*Sigh* Soldiers do the stupidest things!", "/[waitkey(1)]/[clear()]", "But this is a problem, because we can't reset him here. I guess he'll have to stay this way until we get back.", "/[waitkey(64)]/[clear()]"};
    String[] msg37975AF8 = new String[]{"/[label()]", "Oh, are you dumping me? Kitten, don't say such sad things. I am so devoted to you, but you are so frigid. But then again, that's part of your charm, lover!", "/[waitkey(64)]/[clear()]"};
    String[] msg37975AF9 = new String[]{"/[label(Shion)]", "Okay, okay, honey or lover, call me what you want, but just be quieter, okay?", "/[waitkey(64)]/[close()]"};
    String[] msg391E6DCD = new String[]{"/[label()]", "......zzz", "/[waitkey(64)]/[clear()]"};
    String[] msg391E6DCE = new String[]{"/[label(Shion)]", "Oh, god, she is still asleep.", "/[waitkey(64)]/[clear()]"};
    String[] msg391E6DCF = new String[]{"/[label()]", "zzz...", "/[waitkey(64)]/[clear()]"};
    String[] msg391E6DD0 = new String[]{"/[label(Shion)]", "I wonder if Realians dream, too...?", "/[waitkey(64)]/[close()]"};
    String[] msg3AAD4BCE = new String[]{"/[label()]", "Miss Uzuki! They call beautiful people like you \"trash on a dunghill!\"", "/[waitkey(64)]/[clear()]"};
    String[] msg3AAD4BCF = new String[]{"/[label(Shion)]", "...It's \"jewel,\" for crying out loud!", "/[waitkey(64)]/[close()]"};
    String[] msg3A51B8A9 = new String[]{"/[label(Shion)]", "How is it? Still not feeling right?", "/[waitkey(64)]/[clear()]"};
    String[] msg3A51B8AA = new String[]{"/[label()]", "No, just thinking about Allen makes me feel tense and hot. I just can't stand it...", "/[waitkey(64)]/[clear()]"};
    String[] msg3A51B8AB = new String[]{"/[label(Shion)]", "I see...I don't know the cause, but this seems to be a pretty severe case.", "/[waitkey(64)]/[close()]"};
    String[] msg3A51B8AC = new String[]{"/[label()]", "(No, that's not it, she's got a crush on Allen.)", "/[waitkey(64)]/[close()]"};
    String[] msg3A51B8AD = new String[]{"/[label(Shion)]", "I wonder if it has something to do with Allen after all?", "/[waitkey(64)]/[close()]"};
    String[] msg3A51B8AE = new String[]{"/[label()]", "(Why doesn't she get it? Is she going senile?)", "/[waitkey(64)]/[close()]"};
    String[] msg3A52185A = new String[]{"/[label(Shion)]", "I know, next time I'll bring Allen here and run some tests. That should clarify the cause of your problem.", "/[waitkey(64)]/[close()]"};
    String[] msg3A52185B = new String[]{"/[label()]", "(Now I understand why Allen's so uncomfortable.)", "/[waitkey(64)]/[close()]"};
    String[] msgstop = new String[]{"/[label(Shion)]", "I still haven't talked to all the Realians. ", "/[waitkey(1)]/[clear()]", "Better hurry and finish up the tuning. Okay, back to work!", "/[waitkey(64)]/[close()]"};
    String[] msg00000001 = new String[]{"/[label()]", "Phew...this is my second all-nighter in a row, and my ninth cup of coffee.", "/[waitkey(1)]/[clear()]", "Irregular lifestyles like this are really bad for your complexion!", "/[waitkey(64)]/[close()]"};
    String[] msg00000002 = new String[]{"/[label()]", "I'm definitely sleeping in my room tonight. I'll eat a lot of good food and sleep well in a soft, comfortable bed!", "/[waitkey(64)]/[close()]"};
    String[] msg00000003 = new String[]{"/[label()]", "That Lieutenant Virgil really ticks me off. He doesn't do any work and just hangs around here, talking like a big shot.", "/[waitkey(64)]/[close()]"};
    String[] msgSTOP = new String[]{"/[label()]", "Oh, sorry. We're bringing in supplies right now.", "/[waitkey(1)]/[clear()]", "I'm sorry, but would you mind using the door over there?", "/[waitkey(64)]/[close()]"};
    String[] msgMAIL1 = new String[]{"/[label()]", "There's an email for Shion!!", "/[waitkey(64)]/[close()]"};

    ST0060() {
    }

    void EV_Camera01() {
        this.camEV = Camera.create(1);
        this.camEV.setTranslate(3.874f, 3.905f, 7.996f);
        this.camEV.setRotate(-28.162f, -53.919f, 0.0f);
        this.camEV.setFov(29.999f);
        this.camEV.change();
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
        if (n != 100) {
            return;
        }
        switch (n2) {
            case 0: {
                if (this.rea_talk == 6) {
                    return;
                }
                if (this.S014A == 0) {
                    if (this.BUTTON_F == 1) {
                        return;
                    }
                    this.BUTTON_F = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.print(this.msgstop, 0);
                    ST0060.waitPage(this.win, 64);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (this.S014B != 0) break;
                this.fadeIn.call(0);
                System.sleep(23);
                Runtime.setFlags(22, 1, 1);
                Runtime.jumpEvent(1141);
                break;
            }
            case 1: {
                if (this.rea_talk == 6) {
                    return;
                }
                if (this.S014A == 0) {
                    if (this.BUTTON_F == 1) {
                        return;
                    }
                    this.BUTTON_F = 1;
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.print(this.msgstop, 0);
                    ST0060.waitPage(this.win, 64);
                    Runtime.setPlayerControl(true);
                    this.BUTTON_F = 0;
                    break;
                }
                if (this.S014B != 0) break;
                this.fadeIn.call(0);
                Runtime.setPlayerControl(false);
                System.sleep(23);
                Runtime.setFlags(22, 1, 1);
                Runtime.setPlayerControl(true);
                Runtime.jumpEvent(1141);
                break;
            }
            case 2: {
                if (Runtime.getFlags(23, 1) == 1) {
                    return;
                }
                if (Runtime.getFlags(22, 1) == 1) {
                    if (Runtime.getFlags(7061, 1) != 0) break;
                    Runtime.mailArriveSet(6);
                    Runtime.setPlayerControl(false);
                    this.win = Window.create();
                    this.win.setSize(4, 45);
                    this.win.setLocation(15, 15);
                    this.win.print(this.msgMAIL1, 0);
                    Runtime.setFlags(7061, 1, 1);
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
                } else {
                    if (this.rea_talk != 6) break;
                    this.fade.call(0);
                    System.sleep(30);
                    Runtime.setFlags(21, 1, 1);
                    Runtime.jumpEvent(1140);
                    break;
                }
            }
            case 3: {
                if (this.rea_talk != 6) break;
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(21, 1, 1);
                Runtime.jumpEvent(1140);
                break;
            }
            case 4: {
                if (this.rea_talk != 6) break;
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(21, 1, 1);
                Runtime.jumpEvent(1140);
                break;
            }
            case 5: {
                if (this.rea_talk != 6) break;
                this.fade.call(0);
                System.sleep(30);
                Runtime.setFlags(21, 1, 1);
                Runtime.jumpEvent(1140);
                break;
            }
            case 6: {
                if (this.S014B == 1) {
                    return;
                }
                if (this.S014A != 1) break;
                if (this.BUTTON_F == 1) {
                    return;
                }
                this.BUTTON_F = 1;
                Runtime.setPlayerControl(false);
                this.cam0.setMode(-1);
                this.EV_Camera01();
                Runtime.enable(65536);
                this.player.setTranslate(8.71f, -0.62f, 4.29f);
                this.player.setRotate(0.0f, 90.0f, 0.0f);
                this.npc15.kickEnepc(9, 100);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msgSTOP, 0);
                ST0060.waitPage(this.win, 64);
                this.npc15.kickEnepc(9, -1);
                this.cam0.setMode(0);
                Runtime.setPlayerControl(true);
                Runtime.disable(65536);
                this.BUTTON_F = 0;
                break;
            }
            case 7: {
                if (this.smd == 1) {
                    Sound.sequenceStop(0, 1000);
                    Sound.sequencePlay(1, 127);
                    this.smd = 0;
                    break;
                }
                return;
            }
            case 8: {
                if (this.smd == 0) {
                    Sound.sequenceStop(1, 1000);
                    Sound.sequencePlay(0, 127);
                    this.smd = 1;
                    break;
                }
                return;
            }
            case 9: {
                if (this.smd == 1) {
                    Sound.sequenceStop(0, 1000);
                    Sound.sequencePlay(1, 127);
                    this.smd = 0;
                    break;
                }
                return;
            }
            case 10: {
                if (this.smd == 0) {
                    Sound.sequenceStop(1, 1000);
                    Sound.sequencePlay(0, 127);
                    this.smd = 1;
                    break;
                }
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
            this.Talk_npc1_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc1_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc1_2(window);
        } else {
            this.Talk_npc1_1(window);
        }
    }

    public void Talk_npc10(Enepc enepc) {
        if (this.S015B == 1) {
            this.Talk_npc10_4();
        } else if (this.S014B == 1) {
            this.Talk_npc10_3();
        } else if (this.S014A == 1) {
            this.Talk_npc10_2();
        } else {
            this.Talk_npc10_1();
        }
    }

    void Talk_npc10_1() {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                ++this.rea_talk;
                Runtime.enable(65536);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg3A4F6617, 0);
                ST0060.waitPage(this.win, 64);
                this.player.mtn(11, 1, 1.0f, true);
                this.win.print(this.msg3A4F6618, 0);
                ST0060.waitPage(this.win, 64);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg3A4F66181, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A4F6619, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A4F661A, 0);
                ST0060.waitPage(this.win, 64);
                Runtime.disable(65536);
                return;
            }
            case 3: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg3A50BB3C, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A50BB3D, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A50BB3E, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A50BB3F, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A50BB40, 0);
                ST0060.waitPage(this.win, 64);
                this.npc4.moveEnepc(17, 250.0f, 0.0f, 20);
                this.npc4.kickEnepc(0, 7);
                System.sleep(20);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msg3A50BB41, 0);
                ST0060.waitPage(this.win, 64);
                this.npc4.moveEnepc(17, 0.0f, 0.1f, 20);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(11, 1, 1.0f, true);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg3A513EA7, 0);
        ST0060.waitPage(this.win, 64);
        this.npc4.moveEnepc(17, 250.0f, 0.0f, 10);
        this.npc4.kickEnepc(0, 9);
        System.sleep(10);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msg3A513EA8, 0);
        ST0060.waitPage(this.win, 64);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg3A513EA9, 0);
        ST0060.waitPage(this.win, 64);
        this.npc4.kickEnepc(0, 8);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msg3A513EAA, 0);
        ST0060.waitPage(this.win, 64);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg3A513EAB, 0);
        ST0060.waitPage(this.win, 64);
        this.npc4.kickEnepc(0, 8);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msg3A513EAC, 0);
        ST0060.waitPage(this.win, 64);
        this.npc4.moveEnepc(17, 0.0f, 0.1f, 20);
        Runtime.disable(65536);
    }

    void Talk_npc10_2() {
        Runtime.enable(65536);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg3A50CFEB, 0);
        ST0060.waitPage(this.win, 64);
        this.player.mtn(11, 1, 1.0f, true);
        this.win.print(this.msg3A50CFEC, 0);
        ST0060.waitPage(this.win, 64);
        this.win.print(this.msg3A50CFED, 0);
        ST0060.waitPage(this.win, 64);
        this.win.print(this.msg3A50CFEE, 0);
        ST0060.waitPage(this.win, 64);
        Runtime.disable(65536);
    }

    void Talk_npc10_3() {
        Runtime.enable(65536);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg3A50CFEB, 0);
        ST0060.waitPage(this.win, 64);
        this.player.mtn(11, 1, 1.0f, true);
        this.win.print(this.msg3A50CFEC, 0);
        ST0060.waitPage(this.win, 64);
        this.win.print(this.msg3A50CFED, 0);
        ST0060.waitPage(this.win, 64);
        this.win.print(this.msg3A50CFEE, 0);
        ST0060.waitPage(this.win, 64);
        Runtime.disable(65536);
    }

    void Talk_npc10_4() {
        ++this.talkFlag10;
        switch (this.talkFlag10) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg3A51B8A9, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A51B8AA, 0);
                ST0060.waitPage(this.win, 64);
                this.win.print(this.msg3A51B8AB, 0);
                ST0060.waitPage(this.win, 64);
                this.npc4.moveEnepc(17, 250.0f, 0.0f, 10);
                System.sleep(10);
                this.npc4.kickEnepc(0, 8);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msg3A51B8AC, 0);
                ST0060.waitPage(this.win, 64);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 305);
                this.win.print(this.msg3A51B8AD, 0);
                ST0060.waitPage(this.win, 64);
                this.npc4.kickEnepc(0, 8);
                this.win = Window.create();
                this.win.setSize(4, 45);
                this.win.setLocation(15, 15);
                this.win.print(this.msg3A51B8AE, 0);
                ST0060.waitPage(this.win, 64);
                this.npc4.moveEnepc(17, 0.0f, 0.1f, 20);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(11, 1, 1.0f, true);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 305);
        this.win.print(this.msg3A52185A, 0);
        ST0060.waitPage(this.win, 64);
        this.npc4.moveEnepc(17, 250.0f, 0.0f, 10);
        System.sleep(10);
        this.npc4.kickEnepc(0, 7);
        this.win = Window.create();
        this.win.setSize(4, 45);
        this.win.setLocation(15, 15);
        this.win.print(this.msg3A52185B, 0);
        ST0060.waitPage(this.win, 64);
        this.npc4.moveEnepc(17, 0.0f, 0.1f, 20);
        Runtime.disable(65536);
    }

    public void Talk_npc11(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc11_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc11_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc11_2(window);
        } else {
            this.Talk_npc11_1(window);
        }
    }

    void Talk_npc11_1(Window window) {
        ++this.talkFlag11;
        switch (this.talkFlag11) {
            case 1: {
                ++this.rea_talk;
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg3AABEEB1, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3AABEEB2, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3AABEEB5, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                window.print(this.msg3AABEEB6, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3AABEEB7, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3AABEEB8, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3AABEEB9, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg3AAC4E66, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3AAC4E67, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3AAC4E68, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc11_2(Window window) {
        Runtime.enable(65536);
        window.print(this.msg3AAC6310, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(9, 1, 1.0f, true);
        window.print(this.msg3AAC6311, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc11_3(Window window) {
        Runtime.enable(65536);
        window.print(this.msg3AAC6310, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(9, 1, 1.0f, true);
        window.print(this.msg3AAC6311, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc11_4(Window window) {
        Runtime.enable(65536);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg3AAD4BCE, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3AAD4BCF, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    public void Talk_npc12(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc12_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc12_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc12_2(window);
        } else {
            this.Talk_npc12_1(window);
        }
    }

    void Talk_npc12_1(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                window.print(this.msg57AD5D50, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg57AD5D50, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc12_2(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                window.print(this.msg57AD5D50, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg57AD5D50, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc12_3(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                window.print(this.msg57AD5D50, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg57AD5D50, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc12_4(Window window) {
        ++this.talkFlag12;
        switch (this.talkFlag12) {
            case 1: {
                window.print(this.msg57AD5D50, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg57AD5D50, 0);
        ST0060.waitPage(window, 64);
    }

    public void Talk_npc13(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc13_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc13_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc13_2(window);
        } else {
            this.Talk_npc13_1(window);
        }
    }

    void Talk_npc13_1(Window window) {
        ++this.talkFlag13;
        switch (this.talkFlag13) {
            case 1: {
                window.print(this.msg00000001, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000002, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc13_2(Window window) {
        window.print(this.msg00000003, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc13_3(Window window) {
        window.print(this.msg00000003, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc13_4(Window window) {
        ++this.talkFlag13;
        switch (this.talkFlag13) {
            case 1: {
                window.print(this.msg00000001, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg00000002, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc1_1(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg0001, 0);
                ST0060.waitPage(window, 64);
                this.npc1.kickEnepc(1, 9);
                window.print(this.msg0002, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg5A07C5B6, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg5A082564, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 3: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg4A07C5B6, 0);
                ST0060.waitPage(window, 64);
                this.npc1.kickEnepc(1, 9);
                window.print(this.msg3A082564, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        this.npc1.kickEnepc(1, 9);
        window.print(this.msg3A08A8CA, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc1_2(Window window) {
        window.print(this.msg3A949678, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc1_3(Window window) {
        Runtime.enable(65536);
        window.print(this.msg38346575, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg38346576, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc1_4(Window window) {
        ++this.talkFlag1;
        switch (this.talkFlag1) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg3834D9D4, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg3834D9D5, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3834D9D6, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3834D9D7, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg38353983, 0);
        ST0060.waitPage(window, 64);
    }

    public void Talk_npc2(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc2_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc2_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc2_2(window);
        } else {
            this.Talk_npc2_1(window);
        }
    }

    void Talk_npc2_1(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg3A94221A, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg3A94221B, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg3A9481C7, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc2_2(Window window) {
        window.print(this.msg3A949679, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc2_3(Window window) {
        ++this.talkFlag2;
        switch (this.talkFlag2) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg3A950AD8, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg3A950AD9, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3A950ADA, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3A950ADB, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg3A956A88, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc2_4(Window window) {
        Runtime.enable(65536);
        window.print(this.msg3A957F37, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg3A957F38, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3A957F39, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3A957F3A, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3A957F3B, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    public void Talk_npc3(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc3_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc3_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc3_2(window);
        } else {
            this.Talk_npc3_1(window);
        }
    }

    void Talk_npc3_1(Window window) {
        Runtime.enable(65536);
        window.print(this.msg387C5DA1, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg387C5DA2, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc3_2(Window window) {
        ++this.talkFlag3;
        switch (this.talkFlag3) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg387CD200, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg387CD201, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg387CD202, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg387D31AD, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc3_3(Window window) {
        Runtime.enable(65536);
        window.print(this.msg387D465F, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg387D4660, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg387D4661, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg387D4662, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc3_4(Window window) {
        Runtime.enable(65536);
        window.print(this.msg387DBABE, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg387DBABF, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg387DBAC0, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg387DBAC1, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    public void Talk_npc4(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc4_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc4_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc4_2(window);
        } else {
            this.Talk_npc4_1(window);
        }
    }

    void Talk_npc4_1(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg39F0DE1C, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg39F13DC8, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc4_2(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                window.print(this.msg39F1527B, 0);
                ST0060.waitPage(window, 64);
                return;
            }
        }
        window.print(this.msg39F1B227, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc4_3(Window window) {
        ++this.talkFlag4;
        switch (this.talkFlag4) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg39F1C6DA, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg39F1C6DB, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39F1C6DC, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39F1C6DD, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg39F22689, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg39F2268A, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc4_4(Window window) {
        window.print(this.msg39F23B39, 0);
        ST0060.waitPage(window, 64);
    }

    public void Talk_npc5(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc5_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc5_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc5_2(window);
        } else {
            this.Talk_npc5_1(window);
        }
    }

    void Talk_npc5_1(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                ++this.rea_talk;
                Runtime.enable(65536);
                this.player.look_char(this.npc5);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg38F1A19E, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F1A19F, 0);
                ST0060.waitPage(window, 64);
                this.player.look_default();
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.look_char(this.npc5);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg38F2014B, 0);
        ST0060.waitPage(window, 64);
        this.player.look_default();
        Runtime.disable(65536);
    }

    void Talk_npc5_2(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                Runtime.enable(65536);
                this.player.look_char(this.npc5);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg38F215FD, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F215FE, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F215FF, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F21600, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F21601, 0);
                ST0060.waitPage(window, 64);
                this.player.look_default();
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.look_char(this.npc5);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg38F275AE, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg38F275AF, 0);
        ST0060.waitPage(window, 64);
        this.player.look_default();
        Runtime.disable(65536);
    }

    void Talk_npc5_3(Window window) {
        ++this.talkFlag5;
        switch (this.talkFlag5) {
            case 1: {
                Runtime.enable(65536);
                this.player.look_char(this.npc5);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg38F215FD, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F215FE, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F215FF, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F21600, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg38F21601, 0);
                ST0060.waitPage(window, 64);
                this.player.look_default();
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.look_char(this.npc5);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg38F275AE, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg38F275AF, 0);
        ST0060.waitPage(window, 64);
        this.player.look_default();
        Runtime.disable(65536);
    }

    void Talk_npc5_4(Window window) {
        Runtime.enable(65536);
        this.player.look_char(this.npc5);
        window.print(this.msg38F2FEBB, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg38F2FEBC, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg38F2FEBD, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg38F2FEBE, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg38F2FEBF, 0);
        ST0060.waitPage(window, 64);
        this.player.look_default();
        Runtime.disable(65536);
    }

    public void Talk_npc6(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc6_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc6_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc6_2(window);
        } else {
            this.Talk_npc6_1(window);
        }
    }

    void Talk_npc6_1(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                ++this.rea_talk;
                Runtime.enable(65536);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg383176AD, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg383176AE, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg383176AF, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg3831D65B, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3831D65C, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3831D65D, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc6_2(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg3831EB0C, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3831EB0D, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg38324ABA, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg38324ABB, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc6_3(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg3831EB0C, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3831EB0D, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg38324ABA, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg38324ABB, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc6_4(Window window) {
        ++this.talkFlag6;
        switch (this.talkFlag6) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg3832D3CA, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3832D3CB, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3832D3CC, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3832D3CD, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3832D3CE, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3832D3CF, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3832D3D0, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg3833337C, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3833337D, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    public void Talk_npc7(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc7_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc7_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc7_2(window);
        } else {
            this.Talk_npc7_1(window);
        }
    }

    void Talk_npc7_1(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                ++this.rea_talk;
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg39E7A8A8, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E7A8A9, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E7A8AA, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                window.print(this.msg39E7A8AB, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E7A8AC, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 3: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg39E7A8AD, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E7A8AE, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E7A8AF, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg39E8085B, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc7_2(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg39E81D07, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D08, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D09, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg39E81D0A, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D0B, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D0C, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D0D, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg39E87CB9, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc7_3(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                Runtime.enable(65536);
                window.print(this.msg39E81D07, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D08, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D09, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg39E81D0A, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D0B, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D0C, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E81D0D, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg39E87CB9, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc7_4(Window window) {
        ++this.talkFlag7;
        switch (this.talkFlag7) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg39E905C5, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E905C6, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E905C7, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E905C8, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg39E905C9, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg39E96575, 0);
        ST0060.waitPage(window, 64);
    }

    public void Talk_npc8(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc8_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc8_3(window);
        } else if (this.S014A == 1) {
            this.Talk_npc8_2(window);
        } else {
            this.Talk_npc8_1(window);
        }
    }

    void Talk_npc8_1(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                ++this.rea_talk;
                Runtime.enable(65536);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg37959E29, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg37959E2A, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg37959E2B, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
            case 2: {
                Runtime.enable(65536);
                window.print(this.msg37959E2C, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg37959E2D, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        window.print(this.msg3795FDDA, 0);
        ST0060.waitPage(window, 64);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg3795FDDB, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3795FDDC, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg3795FDDD, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc8_2(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg37961288, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg37961289, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg37967236, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg37967237, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc8_3(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg37961288, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg37961289, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(10, 1, 1.0f, true);
        window.print(this.msg37967236, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg37967237, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    void Talk_npc8_4(Window window) {
        ++this.talkFlag8;
        switch (this.talkFlag8) {
            case 1: {
                Runtime.enable(65536);
                this.player.mtn(10, 1, 1.0f, true);
                window.print(this.msg3796FB46, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3796FB47, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3796FB48, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3796FB49, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3796FB4A, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg3796FB4B, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        Runtime.enable(65536);
        this.player.mtn(11, 1, 1.0f, true);
        window.print(this.msg37975AF7, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg37975AF8, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg37975AF9, 0);
        ST0060.waitPage(window, 64);
        Runtime.disable(65536);
    }

    public void Talk_npc9(Enepc enepc, Window window) {
        if (this.S015B == 1) {
            this.Talk_npc9_4(window);
        } else if (this.S014B == 1) {
            this.Talk_npc9_3(window);
        } else if (this.S014A == 1) {
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
                window.print(this.msg391D10B0, 0);
                ST0060.waitPage(window, 64);
                this.player.mtn(11, 1, 1.0f, true);
                window.print(this.msg391D10B1, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg391D10B2, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg391D10B3, 0);
                ST0060.waitPage(window, 64);
                window.print(this.msg391D10B4, 0);
                ST0060.waitPage(window, 64);
                Runtime.disable(65536);
                return;
            }
        }
        window.print(this.msg391D7060, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc9_2(Window window) {
        window.print(this.msg391D850F, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg391D8510, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc9_3(Window window) {
        window.print(this.msg491D850F, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg491D8510, 0);
        ST0060.waitPage(window, 64);
    }

    void Talk_npc9_4(Window window) {
        window.print(this.msg391E6DCD, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg391E6DCE, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg391E6DCF, 0);
        ST0060.waitPage(window, 64);
        window.print(this.msg391E6DD0, 0);
        ST0060.waitPage(window, 64);
    }

    public void Touch_npc3(Enepc enepc, Window window) {
    }

    void doorset_1() {
        this.doorA = new Uwamono(121, 42, '\u0001');
        new Uwamono(122, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(123, 42, '\u0001');
        new Uwamono(124, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(12, 42, '\u0001');
        new Uwamono(9, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(11, 42, '\u0001');
        new Uwamono(10, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(8, 42, '\u0001');
        new Uwamono(6, 42, '\u0001', this.doorE);
        this.doorF = new Uwamono(7, 42, '\u0001');
        new Uwamono(5, 42, '\u0001', this.doorF);
        this.doorA.SetDoorType('\u0002');
        this.doorB.SetDoorType('\u0002');
        this.doorC.SetDoorType('\u0002');
        this.doorD.SetDoorType('\u0002');
        this.doorE.SetDoorType('\u0002');
        this.doorF.SetDoorType('\u0002');
    }

    void doorset_2() {
        this.doorA = new Uwamono(121, 42, '\u0001');
        new Uwamono(122, 42, '\u0001', this.doorA);
        this.doorB = new Uwamono(123, 42, '\u0001');
        new Uwamono(124, 42, '\u0001', this.doorB);
        this.doorC = new Uwamono(12, 42, '\u0001');
        new Uwamono(9, 42, '\u0001', this.doorC);
        this.doorD = new Uwamono(11, 42, '\u0001');
        new Uwamono(10, 42, '\u0001', this.doorD);
        this.doorE = new Uwamono(8, 42, '\u0001');
        new Uwamono(6, 42, '\u0001', this.doorE);
        this.doorF = new Uwamono(7, 42, '\u0001');
        new Uwamono(5, 42, '\u0001', this.doorF);
        this.doorA.SetDoorType('\u0004');
        this.doorB.SetDoorType('\u0004');
        this.doorC.SetDoorType('\u0004');
        this.doorD.SetDoorType('\u0004');
        this.doorE.SetDoorType('\u0004');
        this.doorF.SetDoorType('\u0004');
    }

    void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                this.doorA.DoorOpen();
                Runtime.jumpCF(140, 2);
                break;
            }
            case 1: {
                this.doorB.DoorOpen();
                Runtime.jumpCF(40, 3);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, -4.2f, 0.0f, -8.4f, 0.0f);
        this.teiten1.SetBgm(196621);
        this.teiten2 = new Uwamono(28690, 7.0f, 0.0f, -8.4f, 0.0f);
        this.teiten2.SetBgm(196621);
        this.teiten3 = new Uwamono(28690, 4.2f, 0.0f, -0.9f, 0.0f);
        this.teiten3.SetBgm(196621);
        this.teiten4 = new Uwamono(28690, -7.0f, 0.0f, -0.9f, 0.0f);
        this.teiten4.SetBgm(196621);
        this.teiten5 = new Uwamono(28690, 7.3f, 0.0f, -9.0f, 0.0f);
        this.teiten5.SetBgm(196622);
        this.teiten6 = new Uwamono(28690, 4.0f, 0.0f, -9.0f, 0.0f);
        this.teiten6.SetBgm(196646);
        this.teiten7 = new Uwamono(28690, 7.2f, 0.0f, -0.2f, 0.0f);
        this.teiten7.SetBgm(196622);
        this.teiten8 = new Uwamono(28690, -3.8f, 0.0f, -0.2f, 0.0f);
        this.teiten8.SetBgm(196622);
        this.teiten9 = new Uwamono(28690, 10.0f, 0.0f, 12.0f, 0.0f);
        this.teiten9.SetBgm(196623);
        this.teiten10 = new Uwamono(28690, 11.0f, 0.0f, 9.0f, 0.0f);
        this.teiten10.SetBgm(196623);
        this.light.setColor(0, 0.4f, 0.4f, 0.4f);
        this.light.setColor(1, 0.35f, 0.35f, 0.35f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        this.light.setColor(2, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(2, 0.0f, 1.0f, 5.0f);
        this.light.setColor(3, 0.6f, 0.6f, 0.6f);
        this.light.setDirection2(3, 0.0f, -1.0f, -5.0f);
        Runtime.setIdLightCol(1, 0, 0.5f, 0.5f, 0.5f);
        Runtime.setIdLightCol(1, 1, 1.0f, 0.45f, 0.0f);
        Runtime.setIdLightCol(1, 2, 1.0f, 0.6f, 0.0f);
        Runtime.setIdLightCol(1, 3, 0.6f, 0.6f, 0.0f);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
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
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 9.0f, 30.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFPedestal(2, 16.03f, 8.29f, 7.392f, 40.0f, -72.072f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(2, 1);
        this.cam0.setCFPedestal(3, 0.03f, 11.074f, -15.613f, 40.0f, -87.891f, 0.0f, 0.0f, 2.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFPedestalHokan(3, 1);
        this.cam0.setCFAngle(4, -28.0f, 25.0f, 0.0f, 11.0f, 30.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.Star = new Mapunits();
        this.Star.mapUnit(161);
        this.Star.start(4, null);
        this.Star.setTranslate(this.Star.px, this.Star.py + 300.0f, this.Star.pz);
        Stage.setVisible(0, false);
        Stage.setVisible(19, false);
        Stage.setVisible(18, false);
        Stage.setVisible(17, false);
        Stage.setVisible(16, false);
        Stage.setVisible(15, false);
        Stage.setVisible(14, false);
        Stage.setVisible(13, false);
        this.EF04 = new Effect(1007, 0);
        this.EF04.disp(true);
        this.EF04.setLocation(6, 3);
        this.EF05 = new Effect(1007, 0);
        this.EF05.disp(true);
        this.EF05.setLocation(6, 4);
        if (Runtime.getFlags(23, 1) == 1) {
            this.EF06 = new Effect(1007, 0);
            this.EF06.disp(true);
            this.EF06.setLocation(6, 5);
        }
        if (Runtime.getFlags(23, 1) == 0) {
            this.EF08 = new Effect(1007, 0);
            this.EF08.disp(true);
            this.EF08.setLocation(6, 7);
        }
        this.EF12 = new Effect(1007, 0);
        this.EF12.disp(true);
        this.EF12.setLocation(6, 11);
        if (Runtime.getFlags(23, 1) == 0) {
            this.EF13 = new Effect(1007, 0);
            this.EF13.disp(true);
            this.EF13.setLocation(6, 12);
        }
        this.EF16 = new Effect(1007, 0);
        this.EF16.disp(true);
        this.EF16.setLocation(6, 15);
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
        if (Runtime.getFlags(22, 1) == 1) {
            this.player.setID(1);
        } else if (Runtime.getFlags(21, 1) == 1) {
            this.player.setID(2);
        } else {
            this.player.setID(1);
        }
        this.S014B = Runtime.getFlags(22, 1);
        if (this.S014B == 1) {
            this.doorset_2();
        } else {
            this.doorset_1();
        }
        this.S014A = Runtime.getFlags(21, 1);
        this.S014B = Runtime.getFlags(22, 1);
        this.S015B = Runtime.getFlags(23, 1);
        if (this.S015B == 1) {
            this.npcset_4();
        } else if (this.S014B == 1) {
            this.npcset_3();
        } else if (this.S014A == 1) {
            this.npcset_2();
        } else {
            this.npcset_1();
        }
    }

    void npcset_1() {
        this.npc1 = new NPC_NORMAL(528, 1, 0, 14, 3, -4.5f, 0.0f, -2.11f, 0.0f);
        this.npc2 = new NPC_NORMAL(534, 2, 12, 76, 14, -7.26f, 0.0f, -8.5f, 180.0f);
        this.npc3 = new NPC_NORMAL(531, 3, 12, 76, 13, 3.92f, 0.0f, -8.5f, 180.0f);
        this.npc4 = new NPC_NORMAL(535, 4, 12, 14, 6, 6.82f, 0.22f, -1.32f, 0.0f);
        this.npc5 = new NPC_NORMAL2(537, 5, 12, 76, 11, -5.58f, 0.0f, -2.06f, 50.0f);
        this.npc6 = new NPC_NORMAL2(537, 6, 12, 76, 11, -4.1f, 0.0f, -6.4f, 0.0f);
        this.npc7 = new NPC_NORMAL2(542, 7, 12, 76, 11, 6.0f, 0.0f, -8.1f, -55.0f);
        this.npc8 = new NPC_NORMAL2(542, 8, 12, 76, 11, 4.82f, 0.0f, 0.51f, 0.0f);
        this.npc10 = new NPC_NORMAL2(783, 10, 12, 76, 11, 5.27f, 0.0f, -1.56f, 60.0f);
        this.npc11 = new NPC_NORMAL2(780, 11, 12, 76, 11, -2.52f, 0.0f, -8.71f, 37.0f);
        this.npc13 = new NPC_NORMAL(535, 13, 11, 7, 6, -1.04f, 0.0f, -3.31f, 180.0f);
        this.npc1.disableDTKFlag(1);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 2);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 16);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc5.disableDTKFlag(8);
        this.npc5.setInvalidID(1);
        this.npc5.setTranslate(-5.2f, 0.6f, -1.36f);
        this.npc5.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc5.moveEnepc(18, 98.0f, 0.0f, 0);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc6.disableDTKFlag(8);
        this.npc6.setInvalidID(1);
        this.npc6.setTranslate(-4.05f, 0.59f, -6.9f);
        this.npc6.moveEnepc(16, -32.0f, 0.0f, 0);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 1);
        this.npc7.disableDTKFlag(8);
        this.npc7.setInvalidID(1);
        this.npc7.setTranslate(5.9f, 0.8f, -8.1f);
        this.npc7.moveEnepc(16, -30.0f, 0.0f, 0);
        this.npc7.moveEnepc(18, -30.0f, 0.0f, 0);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 1);
        this.npc8.disableDTKFlag(8);
        this.npc8.setInvalidID(1);
        this.npc8.setTranslate(5.3f, 1.3f, 0.9f);
        this.npc8.moveEnepc(16, -50.0f, 0.0f, 0);
        this.npc8.moveEnepc(18, 25.0f, 0.0f, 0);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 1);
        this.npc10.disableDTKFlag(8);
        this.npc10.setInvalidID(1);
        this.npc10.setTranslate(5.6f, 0.75f, -1.7f);
        this.npc10.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc10.moveEnepc(18, 105.0f, 0.0f, 0);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 1);
        this.npc11.disableDTKFlag(8);
        this.npc11.setInvalidID(1);
        this.npc11.setTranslate(-2.15f, 1.1f, -8.66f);
        this.npc11.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc11.moveEnepc(18, 95.0f, 0.0f, 0);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc13.talkto("Talk_npc13");
    }

    void npcset_2() {
        this.npc1 = new NPC_NORMAL(528, 1, 0, 14, 3, -4.5f, 0.0f, -2.11f, 0.0f);
        this.npc2 = new NPC_NORMAL(534, 2, 12, 76, 14, -7.26f, 0.0f, -8.5f, 180.0f);
        this.npc3 = new NPC_NORMAL(531, 3, 12, 76, 13, 3.92f, 0.0f, -8.5f, 180.0f);
        this.npc4 = new NPC_NORMAL(535, 4, 12, 14, 6, 6.82f, 0.22f, -1.32f, 0.0f);
        this.npc5 = new NPC_NORMAL2(537, 5, 12, 76, 11, -5.58f, 0.0f, -2.06f, 50.0f);
        this.npc6 = new NPC_NORMAL2(537, 6, 12, 76, 11, -4.1f, 0.0f, -6.4f, 0.0f);
        this.npc7 = new NPC_NORMAL2(542, 7, 12, 76, 11, 6.0f, 0.0f, -8.1f, -55.0f);
        this.npc8 = new NPC_NORMAL2(542, 8, 12, 76, 11, 4.82f, 0.0f, 0.51f, 0.0f);
        this.npc10 = new NPC_NORMAL2(783, 10, 12, 76, 11, 5.27f, 0.0f, -1.56f, 60.0f);
        this.npc11 = new NPC_NORMAL2(780, 11, 12, 76, 11, -2.52f, 0.0f, -8.71f, 37.0f);
        this.npc13 = new NPC_NORMAL(535, 13, 11, 7, 6, -1.04f, 0.0f, -3.31f, 180.0f);
        this.npc1.disableDTKFlag(3);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 2);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 16);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc5.setInvalidID(1);
        this.npc5.setTranslate(-5.2f, 0.6f, -1.36f);
        this.npc5.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc5.moveEnepc(18, 98.0f, 0.0f, 0);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc6.setInvalidID(1);
        this.npc6.setTranslate(-4.05f, 0.59f, -6.9f);
        this.npc6.moveEnepc(16, -32.0f, 0.0f, 0);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 1);
        this.npc7.setInvalidID(1);
        this.npc7.setTranslate(5.9f, 0.8f, -8.1f);
        this.npc7.moveEnepc(16, -30.0f, 0.0f, 0);
        this.npc7.moveEnepc(18, -30.0f, 0.0f, 0);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 1);
        this.npc8.setInvalidID(1);
        this.npc8.setTranslate(5.3f, 1.3f, 0.9f);
        this.npc8.moveEnepc(16, -50.0f, 0.0f, 0);
        this.npc8.moveEnepc(18, 25.0f, 0.0f, 0);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 1);
        this.npc10.setInvalidID(1);
        this.npc10.setTranslate(5.6f, 0.75f, -1.7f);
        this.npc10.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc10.moveEnepc(18, 105.0f, 0.0f, 0);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 1);
        this.npc11.setInvalidID(1);
        this.npc11.setTranslate(-2.15f, 1.1f, -8.66f);
        this.npc11.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc11.moveEnepc(18, 95.0f, 0.0f, 0);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc13.talkto("Talk_npc13");
        this.npc15 = new NPC_NORMAL(535, 15, 11, 13, 16, 9.73f, 0.0f, 4.29f, 90.0f);
        this.npc15.setMotion(0, 2);
        this.box = new Unit();
        this.box.mapUnit(128);
        this.box.start(4, null);
        this.box.setTranslate(10.55f, -0.62f, 4.55f);
        this.box.setRotate(0.0f, 90.0f, 0.0f);
    }

    void npcset_3() {
        this.npc1 = new NPC_NORMAL(528, 1, 0, 14, 3, -4.5f, 0.0f, -2.11f, 0.0f);
        this.npc2 = new NPC_NORMAL(534, 2, 12, 76, 14, -7.26f, 0.0f, -8.5f, 180.0f);
        this.npc3 = new NPC_NORMAL(531, 3, 12, 76, 13, 3.92f, 0.0f, -8.5f, 180.0f);
        this.npc4 = new NPC_NORMAL(535, 4, 12, 14, 6, 6.82f, 0.22f, -1.32f, 0.0f);
        this.npc5 = new NPC_NORMAL2(537, 5, 12, 76, 11, -5.58f, 0.0f, -2.06f, 50.0f);
        this.npc6 = new NPC_NORMAL2(537, 6, 12, 76, 11, -4.1f, 0.0f, -6.4f, 0.0f);
        this.npc7 = new NPC_NORMAL2(542, 7, 12, 76, 11, 6.0f, 0.0f, -8.1f, -55.0f);
        this.npc8 = new NPC_NORMAL2(542, 8, 12, 76, 11, 4.82f, 0.0f, 0.51f, 0.0f);
        this.npc10 = new NPC_NORMAL2(783, 10, 12, 76, 11, 5.27f, 0.0f, -1.56f, 60.0f);
        this.npc11 = new NPC_NORMAL2(780, 11, 12, 76, 11, -2.52f, 0.0f, -8.71f, 37.0f);
        this.npc13 = new NPC_NORMAL(535, 13, 11, 7, 6, -1.04f, 0.0f, -3.31f, 180.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 2);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 16);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc5.setInvalidID(1);
        this.npc5.setTranslate(-5.2f, 0.6f, -1.36f);
        this.npc5.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc5.moveEnepc(18, 98.0f, 0.0f, 0);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc6.setInvalidID(1);
        this.npc6.setTranslate(-4.05f, 0.59f, -6.9f);
        this.npc6.moveEnepc(16, -32.0f, 0.0f, 0);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 1);
        this.npc7.setInvalidID(1);
        this.npc7.setTranslate(5.9f, 0.8f, -8.1f);
        this.npc7.moveEnepc(16, -30.0f, 0.0f, 0);
        this.npc7.moveEnepc(18, -30.0f, 0.0f, 0);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 1);
        this.npc8.setInvalidID(1);
        this.npc8.setTranslate(5.3f, 1.3f, 0.9f);
        this.npc8.moveEnepc(16, -50.0f, 0.0f, 0);
        this.npc8.moveEnepc(18, 25.0f, 0.0f, 0);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 1);
        this.npc10.setInvalidID(1);
        this.npc10.setTranslate(5.6f, 0.75f, -1.7f);
        this.npc10.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc10.moveEnepc(18, 105.0f, 0.0f, 0);
        this.npc11.disableDTKFlag(3);
        this.npc11.enableDTKFlag(4);
        this.npc11.setMotion(0, 1);
        this.npc11.setInvalidID(1);
        this.npc11.setTranslate(-2.15f, 1.1f, -8.66f);
        this.npc11.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc11.moveEnepc(18, 95.0f, 0.0f, 0);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc13.talkto("Talk_npc13");
    }

    void npcset_4() {
        this.npc1 = new NPC_NORMAL(528, 1, 0, 14, 3, -4.5f, 0.0f, -2.11f, 0.0f);
        this.npc2 = new NPC_NORMAL(534, 2, 12, 76, 14, -7.26f, 0.0f, -8.5f, 180.0f);
        this.npc3 = new NPC_NORMAL(531, 3, 12, 76, 13, 3.92f, 0.0f, -8.5f, 180.0f);
        this.npc4 = new NPC_NORMAL(535, 4, 12, 14, 6, 6.82f, 0.22f, -1.32f, 0.0f);
        this.npc5 = new NPC_NORMAL2(537, 5, 12, 76, 11, -5.58f, 0.0f, -2.06f, 50.0f);
        this.npc6 = new NPC_NORMAL2(537, 6, 12, 76, 11, -4.1f, 0.0f, -6.4f, 0.0f);
        this.npc7 = new NPC_NORMAL2(542, 7, 12, 76, 11, 6.0f, 0.0f, -8.1f, -55.0f);
        this.npc8 = new NPC_NORMAL2(537, 8, 12, 76, 11, -5.46f, 0.0f, -7.87f, -45.0f);
        this.npc10 = new NPC_NORMAL2(783, 10, 12, 76, 11, 5.27f, 0.0f, -1.56f, 60.0f);
        this.npc11 = new NPC_NORMAL2(780, 11, 11, 2, 15, -1.08f, 0.0f, -7.86f, 90.0f);
        this.npc13 = new NPC_NORMAL(535, 13, 11, 7, 6, -1.04f, 0.0f, -3.31f, 180.0f);
        this.npc2.disableDTKFlag(3);
        this.npc2.enableDTKFlag(4);
        this.npc2.setMotion(0, 2);
        this.npc3.disableDTKFlag(3);
        this.npc3.enableDTKFlag(4);
        this.npc3.setMotion(0, 1);
        this.npc4.disableDTKFlag(3);
        this.npc4.enableDTKFlag(4);
        this.npc4.setMotion(0, 16);
        this.npc4.setInvalidID(1);
        this.npc5.disableDTKFlag(3);
        this.npc5.enableDTKFlag(4);
        this.npc5.setMotion(0, 1);
        this.npc5.setInvalidID(1);
        this.npc5.setTranslate(-5.2f, 0.6f, -1.36f);
        this.npc5.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc5.moveEnepc(18, 98.0f, 0.0f, 0);
        this.npc6.disableDTKFlag(3);
        this.npc6.enableDTKFlag(4);
        this.npc6.setMotion(0, 1);
        this.npc6.setInvalidID(1);
        this.npc6.setTranslate(-4.05f, 0.59f, -6.9f);
        this.npc6.moveEnepc(16, -32.0f, 0.0f, 0);
        this.npc7.disableDTKFlag(3);
        this.npc7.enableDTKFlag(4);
        this.npc7.setMotion(0, 1);
        this.npc7.setInvalidID(1);
        this.npc7.setTranslate(5.9f, 0.8f, -8.1f);
        this.npc7.moveEnepc(16, -30.0f, 0.0f, 0);
        this.npc7.moveEnepc(18, -30.0f, 0.0f, 0);
        this.npc8.disableDTKFlag(3);
        this.npc8.enableDTKFlag(4);
        this.npc8.setMotion(0, 1);
        this.npc8.setInvalidID(1);
        this.npc8.setTranslate(-6.0f, 0.9f, -8.17f);
        this.npc8.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc8.moveEnepc(18, -90.0f, 0.0f, 0);
        this.npc10.disableDTKFlag(3);
        this.npc10.enableDTKFlag(4);
        this.npc10.setMotion(0, 1);
        this.npc10.setInvalidID(1);
        this.npc10.setTranslate(5.6f, 0.75f, -1.7f);
        this.npc10.moveEnepc(16, -90.0f, 0.0f, 0);
        this.npc10.moveEnepc(18, 105.0f, 0.0f, 0);
        this.npc1.talkto("Talk_npc1");
        this.npc2.talkto("Talk_npc2");
        this.npc3.talkto("Talk_npc3");
        this.npc4.talkto("Talk_npc4");
        this.npc5.talkto("Talk_npc5");
        this.npc6.talkto("Talk_npc6");
        this.npc7.talkto("Talk_npc7");
        this.npc8.talkto("Talk_npc8");
        this.npc10.talkto("Talk_npc10");
        this.npc11.talkto("Talk_npc11");
        this.npc13.talkto("Talk_npc13");
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

    class Mapunits
            extends MAPUnit {
        Mapunits() {
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

    class NPC_NORMAL2
            extends Enepc {
        NPC_NORMAL2(int n, int n2, int n3, int n4, int n5, float f, float f2, float f3, float f4) {
            this.init(n, n5, f, f2, f3, f4);
            this.id = n2;
            this.setParams(n3, n4, n2, n5);
            this.setShadow(0, 0);
        }
    }
}

