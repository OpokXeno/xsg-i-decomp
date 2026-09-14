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
import xeno.map.MC_DYU05_PRJ;
import xeno.plan.CfConstants;
import xeno.util.Menu;
import xeno.util.Runtime;
import xeno.util.Window;
import xeno.vm.System;

class ST1752
        extends Stage
        implements XenoConstants,
        CfConstants,
        MC_DYU05_PRJ {
    static final int MTN_TEST1 = 257;
    static final int MTN_TEST2 = 258;
    static final int MTN_TEST3 = 259;
    static final int MTN_TEST4 = 260;
    static final int MTN_TEST5 = 261;
    static final int MTN_TEST6 = 262;
    static final int MTN_TEST7 = 263;
    static final int MTN_TEST8 = 264;
    static final int MTN_TEST9 = 265;
    static final int MTN_TEST10 = 266;
    Player player;
    Camera cam1;
    Camera camEV;
    Enepc npc1;
    Enepc npc2;
    Enepc npc3;
    Enepc npc4;
    Enepc npc5;
    Enepc npc06;
    Enepc npc07;
    Enepc npc08;
    Enepc npc09;
    Enepc npc10;
    Enepc npc11;
    Enepc npc12;
    Enepc npc13;
    Enepc npc14;
    Enepc enemy1;
    Enepc enemy2;
    Enepc enemy3;
    Enepc enemy4;
    Enepc enemy5;
    Unit unit1;
    Effect light01;
    Effect light02;
    Effect light03;
    Effect fade;
    Menu menu;
    Window win;
    int count = 0;
    int selected = 0;
    int npc1talked = 0;
    int npc2talked = 0;
    int npc3talked = 0;
    int npc4talked = 0;
    int npc5talked = 0;
    int npc06talked = 0;
    int npc07talked = 0;
    int npc08talked = 0;
    int npc09talked = 0;
    int npc10talked = 0;
    int npc11talked = 0;
    int npc12talked = 0;
    int npc13talked = 0;
    int npc14talked = 0;
    boolean npc1flg = false;
    boolean npc2flg = false;
    Uwamono doorA;
    Uwamono doorB;
    Uwamono doorC;
    Uwamono doorD;
    Uwamono doorE;
    Uwamono item01;
    Uwamono item02;
    Uwamono item03;
    Uwamono item04;
    Uwamono item05;
    Uwamono BASE01;
    Uwamono BASE02;
    Uwamono BASE03;
    int test1;
    int test2;
    int test3;
    int test4;
    int test5;
    Light light = new Light(0);
    Uwamono teiten1;
    Uwamono teiten2;
    Uwamono teiten3;
    Uwamono teiten4;
    Uwamono teiten5;
    int page;
    String[] K01_00CN = new String[]{"This is no joking matter!", "/[waitkey(1)]/[clear()]", "How many people do you think are living on the Foundation?", "/[waitkey(1)]/[clear()]", "Check rooms and corridors and evacuate as many people as you can!", "/[waitkey(64)]/[close()]"};
    String[] K01_00CJ = new String[]{"Oh, Little Master, hurry! Get the Gnosis above! We won't be able to take all the people above with just this ship!!", "/[waitkey(64)]/[close()]"};
    String[] K02_00CN = new String[]{"People evacuating from the Foundation are going to be here soon!", "/[waitkey(1)]/[clear()]", "This place is going to be full of people, so you'll get lost if you wander around here!", "/[waitkey(64)]/[close()]"};
    String[] K02_00CJ = new String[]{"Little Master, please leave the Residential Areas to us!!", "/[waitkey(1)]/[clear()]", "Little Master, please go rescue the people left on the Foundation!!", "/[waitkey(64)]/[close()]"};
    String[] K03_00CN = new String[]{"Things are pretty chaotic over on the Foundation.", "/[waitkey(1)]/[clear()]", "We have to evacuate them quickly, or our losses will only increase!", "/[waitkey(64)]/[close()]"};
    String[] K03_00CJ = new String[]{"Things are pretty chaotic over on the Foundation.", "/[waitkey(1)]/[clear()]", "We have to evacuate them quickly, or our losses will only increase!", "/[waitkey(64)]/[close()]"};
    String[] K04_00CN = new String[]{"Damn, why do the Gnosis have to appear at a time like this?!", "/[waitkey(1)]/[clear()]", "If we don't do something quickly, the Foundation will...!", "/[waitkey(64)]/[close()]"};
    String[] K04_00CJ = new String[]{"Oh, Little Master! How are things in the City Area?!", "/[waitkey(1)]/[clear()]", "We'll be fine here by ourselves! Little Master, please hurry and rescue the civilians!", "/[waitkey(64)]/[close()]"};
    String[] K05_00CN = new String[]{"Oh, um, excuse me! Please hang on if you want to sign up for the fan club.", "/[waitkey(1)]/[clear()]", "Right now, rescuing the people on the Foundation takes precedence!", "/[waitkey(64)]/[close()]"};
    String[] K05_00CJ = new String[]{"Little Master! We have opened all the doors in the Residential Areas.", "/[waitkey(1)]/[clear()]", "Please make us the top priority when dispatching medical teams!", "/[waitkey(64)]/[close()]"};
    String[] U01_00CN = new String[]{"We're done rescuing the refugees, but now, we have to prepare to restore the Foundation.", "/[waitkey(1)]/[clear()]", "Looks like we'll be busy for a long time to come.", "/[waitkey(64)]/[close()]"};
    String[] U01_00CJ = new String[]{"The people of the Foundation have begun to regain a good deal of their composure.", "/[waitkey(1)]/[clear()]", "Please leave the Durandal to us, Little Master, and you go put everything you need to into what it is you have to do!!", "/[waitkey(64)]/[close()]"};
    String[] U02_00CN = new String[]{"The Captain of the Elsa was looking for all of you a little while ago!!", "/[waitkey(1)]/[clear()]", "He said, \"We're departing, so hurry up!!\"", "/[waitkey(1)]/[clear()]", "He can be a little rude, but I like him.", "/[waitkey(64)]/[close()]"};
    String[] U02_00CJ = new String[]{"!!\n", "Little Master?", "/[waitkey(1)]/[clear()]", "Did you come to help us out?", "/[waitkey(1)]/[clear()]", "That can't be, right? Little Master, you must have other things to do, right?!", "/[waitkey(64)]/[close()]"};
    String[] U03_00CN = new String[]{"The people of the Foundation are quite tenacious.", "/[waitkey(1)]/[clear()]", "They've already gone back to the Foundation and have begun restorative work.", "/[waitkey(64)]/[close()]"};
    String[] U03_00CJ = new String[]{"Little Master! Due to the incident the other day, the Durandal has a shortage of medical staff!!", "/[waitkey(1)]/[clear()]", "Could you and Director Gaignun do something about this?", "/[waitkey(64)]/[close()]"};
    String[] U04_00CN = new String[]{"Just when I thought the monster commotion finally settled down, now there's a weird fortress?", "/[waitkey(1)]/[clear()]", "Sheesh, there's excitement every day when you're involved with this ship.", "/[waitkey(64)]/[close()]"};
    String[] U04_00CJ = new String[]{"Yo!! Little Master! You're always at the center of these incidents, aren't you?!", "/[waitkey(1)]/[clear()]", "But you know, I'm always cheering you on!!", "/[waitkey(64)]/[close()]"};
    String[] U05_00CN = new String[]{"Oh, um, excuse me! Please hang on if you want to sign up for the fan club.", "/[waitkey(1)]/[clear()]", "Right now...um, that is...we're in a terrible situation!!", "/[waitkey(64)]/[close()]"};
    String[] U05_00CJ = new String[]{"Little Master? I'm glad we rescued the Residential Area, but I am worried that it will lead to a decline in safety.", "/[waitkey(1)]/[clear()]", "The people of the Foundation are too gung ho!", "/[waitkey(1)]/[clear()]", "Especially the old ladies!", "/[waitkey(64)]/[close()]"};
    String[] KARI_00 = new String[]{"/[label(Temp Person)]", "You are in front of Foundation. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] KARI_01 = new String[]{"/[label(Temp Person)]", "You are in front of the Song of Nephilim. 【temp】", "/[waitkey(64)]/[close()]"};
    String[] T06_1 = new String[]{"That Realian boy is so cute.", "/[waitkey(1)]/[clear()]", "He'll do anything that I ask him to do!", "/[waitkey(64)]/[close()]"};
    String[] T07_1 = new String[]{"I figured it out! No matter how strong you get, you're powerless against monsters!!", "/[waitkey(1)]/[clear()]", "Kids are so naive in that sense. They're so enthusiastic and believe everything will work out as long as they are strong!!", "/[waitkey(64)]/[close()]"};
    String[] T08_1 = new String[]{"The people on this ship are so kind.", "/[waitkey(1)]/[clear()]", "This ship is too big for an old person like me.", "/[waitkey(1)]/[clear()]", "Oh, I want to go back to the Foundation.", "/[waitkey(64)]/[close()]"};
    String[] T09_1 = new String[]{"What am I doing? Working out, of course, working out!!", "/[waitkey(1)]/[clear()]", "When you're in a ship this big, even a stroll can amount to a bit of a workout!!", "/[waitkey(64)]/[close()]"};
    String[] T10_1 = new String[]{"I've been making all kinds of new discoveries ever since I got on this ship!!", "/[waitkey(1)]/[clear()]", "Like the things being sold in this vending machine are all things I've never seen before!!", "/[waitkey(1)]/[clear()]", "I hear there's someone that's been destroying vending machines lately. I just can't believe that!!", "/[waitkey(64)]/[close()]"};
    String[] T11_1 = new String[]{"If monsters show up again, I'm going to defeat them!!", "/[waitkey(1)]/[clear()]", "In order to do that, I have to keep myself in shape!!", "/[waitkey(64)]/[close()]"};
    String[] T12_1 = new String[]{"I wonder if this \"S. Carrot Juice\" is tasty?", "/[waitkey(1)]/[clear()]", "Well, I don't like carrots, so...", "/[waitkey(64)]/[close()]"};
    String[] T13_1 = new String[]{"A lot of people get lost lately, and it's a major problem.", "/[waitkey(1)]/[clear()]", "You won't find a ship this big anywhere else.", "/[waitkey(64)]/[close()]"};
    String[] T14_1 = new String[]{"The Durandal has been bustling lately. And to be honest, I didn't like it. But when I think of the Foundation being restored and all these people going back, I feel a little lonely.", "/[waitkey(64)]/[close()]"};

    ST1752() {
    }

    void Final_init(int n) {
    }

    public void KickEvent(int n, int n2) {
    }

    public void TalkNPC06(Enepc enepc, Window window) {
        window.print(this.T06_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC07(Enepc enepc, Window window) {
        window.print(this.T07_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC08(Enepc enepc, Window window) {
        window.print(this.T08_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC09(Enepc enepc, Window window) {
        window.print(this.T09_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC1(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC10(Enepc enepc, Window window) {
        window.print(this.T10_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC11(Enepc enepc, Window window) {
        window.print(this.T11_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC12(Enepc enepc, Window window) {
        window.print(this.T12_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC13(Enepc enepc, Window window) {
        window.print(this.T13_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC14(Enepc enepc, Window window) {
        window.print(this.T14_1, 0);
        System.waitFor(window);
    }

    public void TalkNPC1a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U01_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U01_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K02_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K02_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC2a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U02_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U02_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC3(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K03_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K03_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC3a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U03_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U03_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC4(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K04_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC4a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U04_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U04_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.K05_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.K05_00CN, 0);
            System.waitFor(window);
        }
    }

    public void TalkNPC5a(Enepc enepc, Window window) {
        if (Runtime.getLeader() == 5) {
            window.print(this.U05_00CJ, 0);
            System.waitFor(window);
        } else {
            window.print(this.U05_00CN, 0);
            System.waitFor(window);
        }
    }

    public void entered(int n) {
        Runtime.setRegister(0, n);
        System.println("enterd : /[$0]");
        this.fade.call(0);
        System.sleep(30);
        switch (n) {
            case 0: {
                Runtime.jumpCF(1882, 1);
                break;
            }
            case 1: {
                Runtime.jumpCF(1732, 1);
                break;
            }
            case 2: {
                Runtime.jumpCF(1742, 1);
                break;
            }
            case 3: {
                Runtime.jumpCF(1872, 1);
                break;
            }
            case 4: {
                Runtime.jumpCF(1842, 1);
                break;
            }
        }
    }

    void init() {
        this.teiten1 = new Uwamono(28690, 3.5f, 0.0f, -2.0f, 0.0f);
        this.teiten1.SetBgm(196615);
        this.teiten2 = new Uwamono(28690, 3.5f, 0.0f, 2.0f, 0.0f);
        this.teiten2.SetBgm(196615);
        this.teiten3 = new Uwamono(28690, 0.0f, 0.0f, -13.0f, 0.0f);
        this.teiten3.SetBgm(196616);
        this.teiten4 = new Uwamono(28690, -12.5f, 0.0f, -17.5f, 0.0f);
        this.teiten4.SetBgm(196617);
        this.teiten5 = new Uwamono(28690, 13.5f, 0.0f, 5.2f, 0.0f);
        this.teiten5.SetBgm(196616);
        Stage.setVisible(-1, true);
        int n = Runtime.getEntrance();
        if (n >= 0) {
            Runtime.setRegister(0, n);
            System.println("entrance: /[$0]");
            this.player.setLocation(1, n);
        }
        Runtime.setPlayerMoveParam(32.0f, 96.0f, 9.895E-4f);
        this.light.setColor(0, 0.3f, 0.3f, 0.3f);
        this.light.setColor(1, 0.3f, 0.3f, 0.3f);
        this.light.setDirection2(1, 0.0f, 1.0f, 0.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(2, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(2, 0.0f, 1.0f, 2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.light.setColor(3, 0.55f, 0.55f, 0.55f);
        this.light.setDirection2(3, 0.0f, -1.0f, -2.0f);
        Stage.setColor(1.0f, 1.0f, 1.0f);
        this.cam0.setCFAngle(1, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(1, 0.01f, 0.01f);
        this.cam0.setCFAngle(2, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(2, 0.01f, 0.01f);
        this.cam0.setCFLockX(2, 13.5f);
        this.cam0.setCFAngle(3, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(3, 0.01f, 0.01f);
        this.cam0.setCFAngle(4, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(4, 0.01f, 0.01f);
        this.cam0.setCFAngle(5, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(5, 100.0f, 100.0f);
        this.cam0.setCFAngle(6, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(6, 100.0f, 100.0f);
        this.cam0.setCFAngle(7, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(7, 0.01f, 0.01f);
        this.cam0.setCFAngle(8, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(8, 0.01f, 0.01f);
        this.cam0.setCFLockX(8, 0.0f);
        this.cam0.setCFAngle(9, -28.0f, 0.0f, 0.0f, 10.0f, 45.0f);
        this.cam0.setCFHokan(9, 0.01f, 0.01f);
        this.cam0.setCFAngle(10, -28.0f, 0.0f, 0.0f, 7.0f, 45.0f);
        this.cam0.setCFHokan(10, 0.01f, 0.01f);
        this.fade = new Effect(0);
        this.fade.args[0] = -268435456;
        this.fade.args[1] = 30;
        this.fade.args[2] = 0;
        if (Runtime.getFlags(373, 1) == 0) {
            this.npc3 = new NPC_NORMAL(1543, 13, 0, 0, 7, -2.106f, 0.0f, 3.053f, 90.0f);
            this.npc4 = new NPC_NORMAL(1540, 14, 0, 0, 27, 2.186f, 0.0f, -11.413f, 0.0f);
            this.npc3.talkto("TalkNPC3");
            this.npc4.talkto("TalkNPC4");
            this.npc3.disableDTKFlag(131074);
            this.npc4.disableDTKFlag(131074);
            this.npc4.setMotion(0, 27);
            this.npc3.enableDTKFlag(12);
            this.npc4.enableDTKFlag(12);
            this.npc4.setInvalidID(1);
            this.npc1 = new Enepc();
            this.npc1.init(1289, 3, -1.0f, 0.0f, 25.0f, 90.0f);
            this.npc1.id = 12;
            this.npc1.setParams(0, 11, 11, 3);
            float[] fArray = new float[114];
            fArray[0] = -1.0f;
            fArray[2] = 25.0f;
            fArray[5] = 25.0f;
            fArray[6] = 1.0f;
            fArray[8] = 25.0f;
            fArray[9] = 2.0f;
            fArray[11] = 25.0f;
            fArray[12] = 3.0f;
            fArray[14] = 25.0f;
            fArray[15] = 4.0f;
            fArray[17] = 25.0f;
            fArray[18] = 5.0f;
            fArray[20] = 25.0f;
            fArray[21] = 6.0f;
            fArray[23] = 25.0f;
            fArray[24] = 7.0f;
            fArray[26] = 25.0f;
            fArray[27] = 8.0f;
            fArray[29] = 25.0f;
            fArray[30] = 9.0f;
            fArray[32] = 25.0f;
            fArray[33] = 10.0f;
            fArray[35] = 25.0f;
            fArray[36] = 11.0f;
            fArray[38] = 25.0f;
            fArray[39] = 12.0f;
            fArray[41] = 25.0f;
            fArray[42] = 13.0f;
            fArray[44] = 25.0f;
            fArray[45] = 14.0f;
            fArray[47] = 25.0f;
            fArray[48] = 14.0f;
            fArray[50] = 25.0f;
            fArray[51] = 14.0f;
            fArray[53] = 24.0f;
            fArray[54] = 14.0f;
            fArray[56] = 23.0f;
            fArray[57] = 14.0f;
            fArray[59] = 22.0f;
            fArray[60] = 14.0f;
            fArray[62] = 21.0f;
            fArray[63] = 14.0f;
            fArray[65] = 20.0f;
            fArray[66] = 14.0f;
            fArray[68] = 19.0f;
            fArray[69] = 14.0f;
            fArray[71] = 18.0f;
            fArray[72] = 14.0f;
            fArray[74] = 17.0f;
            fArray[75] = 14.0f;
            fArray[77] = 16.0f;
            fArray[78] = 14.0f;
            fArray[80] = 15.0f;
            fArray[81] = 14.0f;
            fArray[83] = 14.0f;
            fArray[84] = 14.0f;
            fArray[86] = 13.0f;
            fArray[87] = 14.0f;
            fArray[89] = 12.0f;
            fArray[90] = 14.0f;
            fArray[92] = 11.0f;
            fArray[93] = 14.0f;
            fArray[95] = 10.0f;
            fArray[96] = 14.0f;
            fArray[98] = 9.0f;
            fArray[99] = 15.0f;
            fArray[101] = 9.0f;
            fArray[102] = 16.0f;
            fArray[104] = 9.0f;
            fArray[105] = 17.0f;
            fArray[107] = 9.0f;
            fArray[108] = 18.0f;
            fArray[110] = 9.0f;
            fArray[111] = 19.0f;
            fArray[113] = 9.0f;
            float[] fArray2 = fArray;
            this.npc1.setShadow(3, 16);
            this.npc1.setParams(fArray2);
            this.npc1.talkto("TalkNPC1");
            this.npc1.disableDTKFlag(131072);
            this.npc1.enableDTKFlag(12);
            this.npc1.setInvalidID(1);
            this.npc2 = new Enepc();
            this.npc2.init(1025, 5, 12.5f, 0.0f, 23.5f, 180.0f);
            this.npc2.id = 12;
            this.npc2.setParams(0, 9, 12, 5);
            float[] fArray3 = new float[75];
            fArray3[0] = 12.5f;
            fArray3[2] = 23.5f;
            fArray3[3] = 12.5f;
            fArray3[5] = 22.5f;
            fArray3[6] = 12.5f;
            fArray3[8] = 21.5f;
            fArray3[9] = 12.5f;
            fArray3[11] = 20.5f;
            fArray3[12] = 12.5f;
            fArray3[14] = 19.5f;
            fArray3[15] = 12.5f;
            fArray3[17] = 18.5f;
            fArray3[18] = 12.5f;
            fArray3[20] = 17.5f;
            fArray3[21] = 12.5f;
            fArray3[23] = 16.5f;
            fArray3[24] = 12.5f;
            fArray3[26] = 15.5f;
            fArray3[27] = 12.5f;
            fArray3[29] = 14.5f;
            fArray3[30] = 12.5f;
            fArray3[32] = 13.5f;
            fArray3[33] = 12.5f;
            fArray3[35] = 12.5f;
            fArray3[36] = 12.5f;
            fArray3[38] = 11.5f;
            fArray3[39] = 12.5f;
            fArray3[41] = 10.5f;
            fArray3[42] = 12.5f;
            fArray3[44] = 9.5f;
            fArray3[45] = 12.5f;
            fArray3[47] = 8.5f;
            fArray3[48] = 12.5f;
            fArray3[50] = 7.5f;
            fArray3[51] = 11.5f;
            fArray3[53] = 7.5f;
            fArray3[54] = 10.5f;
            fArray3[56] = 7.5f;
            fArray3[57] = 9.5f;
            fArray3[59] = 7.5f;
            fArray3[60] = 8.5f;
            fArray3[62] = 7.5f;
            fArray3[63] = 7.5f;
            fArray3[65] = 7.5f;
            fArray3[66] = 6.5f;
            fArray3[68] = 7.5f;
            fArray3[69] = 5.5f;
            fArray3[71] = 7.5f;
            fArray3[72] = 4.5f;
            fArray3[74] = 7.5f;
            float[] fArray4 = fArray3;
            this.npc2.setShadow(3, 16);
            this.npc2.setParams(fArray4);
            this.npc2.talkto("TalkNPC2");
            this.npc2.disableDTKFlag(131072);
            this.npc2.enableDTKFlag(12);
            this.npc2.setInvalidID(1);
            this.npc5 = new Enepc();
            this.npc5.init(780, 28, -17.0f, 0.0f, -10.0f, 90.0f);
            this.npc5.id = 15;
            this.npc5.setParams(0, 11, 15, 28);
            float[] fArray5 = new float[33];
            fArray5[0] = -17.0f;
            fArray5[2] = -10.0f;
            fArray5[3] = -16.0f;
            fArray5[5] = -10.0f;
            fArray5[6] = -15.0f;
            fArray5[8] = -10.0f;
            fArray5[9] = -14.0f;
            fArray5[11] = -10.0f;
            fArray5[12] = -13.0f;
            fArray5[14] = -10.0f;
            fArray5[15] = -12.0f;
            fArray5[17] = -10.0f;
            fArray5[18] = -11.0f;
            fArray5[20] = -10.0f;
            fArray5[21] = -10.0f;
            fArray5[23] = -10.0f;
            fArray5[24] = -9.0f;
            fArray5[26] = -10.0f;
            fArray5[27] = -8.0f;
            fArray5[29] = -10.0f;
            fArray5[30] = -7.0f;
            fArray5[32] = -10.0f;
            float[] fArray6 = fArray5;
            this.npc5.setShadow(3, 16);
            this.npc5.setParams(fArray6);
            this.npc5.talkto("TalkNPC5");
            this.npc5.disableDTKFlag(131072);
            this.npc5.enableDTKFlag(12);
            this.npc5.setInvalidID(1);
        } else {
            this.npc3 = new NPC_NORMAL(1543, 13, 0, 0, 7, -2.106f, 0.0f, 3.053f, 90.0f);
            this.npc4 = new NPC_NORMAL(1540, 14, 0, 0, 27, 2.186f, 0.0f, -11.413f, 0.0f);
            this.npc3.talkto("TalkNPC3a");
            this.npc4.talkto("TalkNPC4a");
            this.npc3.disableDTKFlag(131074);
            this.npc4.disableDTKFlag(131074);
            this.npc4.setMotion(0, 27);
            this.npc3.enableDTKFlag(12);
            this.npc4.enableDTKFlag(12);
            this.npc4.setInvalidID(1);
            this.npc1 = new Enepc();
            this.npc1.init(1289, 3, -1.0f, 0.0f, 25.0f, 90.0f);
            this.npc1.id = 12;
            this.npc1.setParams(0, 11, 11, 3);
            float[] fArray = new float[114];
            fArray[0] = -1.0f;
            fArray[2] = 25.0f;
            fArray[5] = 25.0f;
            fArray[6] = 1.0f;
            fArray[8] = 25.0f;
            fArray[9] = 2.0f;
            fArray[11] = 25.0f;
            fArray[12] = 3.0f;
            fArray[14] = 25.0f;
            fArray[15] = 4.0f;
            fArray[17] = 25.0f;
            fArray[18] = 5.0f;
            fArray[20] = 25.0f;
            fArray[21] = 6.0f;
            fArray[23] = 25.0f;
            fArray[24] = 7.0f;
            fArray[26] = 25.0f;
            fArray[27] = 8.0f;
            fArray[29] = 25.0f;
            fArray[30] = 9.0f;
            fArray[32] = 25.0f;
            fArray[33] = 10.0f;
            fArray[35] = 25.0f;
            fArray[36] = 11.0f;
            fArray[38] = 25.0f;
            fArray[39] = 12.0f;
            fArray[41] = 25.0f;
            fArray[42] = 13.0f;
            fArray[44] = 25.0f;
            fArray[45] = 14.0f;
            fArray[47] = 25.0f;
            fArray[48] = 14.0f;
            fArray[50] = 25.0f;
            fArray[51] = 14.0f;
            fArray[53] = 24.0f;
            fArray[54] = 14.0f;
            fArray[56] = 23.0f;
            fArray[57] = 14.0f;
            fArray[59] = 22.0f;
            fArray[60] = 14.0f;
            fArray[62] = 21.0f;
            fArray[63] = 14.0f;
            fArray[65] = 20.0f;
            fArray[66] = 14.0f;
            fArray[68] = 19.0f;
            fArray[69] = 14.0f;
            fArray[71] = 18.0f;
            fArray[72] = 14.0f;
            fArray[74] = 17.0f;
            fArray[75] = 14.0f;
            fArray[77] = 16.0f;
            fArray[78] = 14.0f;
            fArray[80] = 15.0f;
            fArray[81] = 14.0f;
            fArray[83] = 14.0f;
            fArray[84] = 14.0f;
            fArray[86] = 13.0f;
            fArray[87] = 14.0f;
            fArray[89] = 12.0f;
            fArray[90] = 14.0f;
            fArray[92] = 11.0f;
            fArray[93] = 14.0f;
            fArray[95] = 10.0f;
            fArray[96] = 14.0f;
            fArray[98] = 9.0f;
            fArray[99] = 15.0f;
            fArray[101] = 9.0f;
            fArray[102] = 16.0f;
            fArray[104] = 9.0f;
            fArray[105] = 17.0f;
            fArray[107] = 9.0f;
            fArray[108] = 18.0f;
            fArray[110] = 9.0f;
            fArray[111] = 19.0f;
            fArray[113] = 9.0f;
            float[] fArray7 = fArray;
            this.npc1.setShadow(3, 16);
            this.npc1.setParams(fArray7);
            this.npc1.talkto("TalkNPC1a");
            this.npc1.disableDTKFlag(131072);
            this.npc1.enableDTKFlag(12);
            this.npc1.setInvalidID(1);
            this.npc2 = new Enepc();
            this.npc2.init(1025, 5, 12.5f, 0.0f, 23.5f, 180.0f);
            this.npc2.id = 12;
            this.npc2.setParams(0, 9, 12, 5);
            float[] fArray8 = new float[75];
            fArray8[0] = 12.5f;
            fArray8[2] = 23.5f;
            fArray8[3] = 12.5f;
            fArray8[5] = 22.5f;
            fArray8[6] = 12.5f;
            fArray8[8] = 21.5f;
            fArray8[9] = 12.5f;
            fArray8[11] = 20.5f;
            fArray8[12] = 12.5f;
            fArray8[14] = 19.5f;
            fArray8[15] = 12.5f;
            fArray8[17] = 18.5f;
            fArray8[18] = 12.5f;
            fArray8[20] = 17.5f;
            fArray8[21] = 12.5f;
            fArray8[23] = 16.5f;
            fArray8[24] = 12.5f;
            fArray8[26] = 15.5f;
            fArray8[27] = 12.5f;
            fArray8[29] = 14.5f;
            fArray8[30] = 12.5f;
            fArray8[32] = 13.5f;
            fArray8[33] = 12.5f;
            fArray8[35] = 12.5f;
            fArray8[36] = 12.5f;
            fArray8[38] = 11.5f;
            fArray8[39] = 12.5f;
            fArray8[41] = 10.5f;
            fArray8[42] = 12.5f;
            fArray8[44] = 9.5f;
            fArray8[45] = 12.5f;
            fArray8[47] = 8.5f;
            fArray8[48] = 12.5f;
            fArray8[50] = 7.5f;
            fArray8[51] = 11.5f;
            fArray8[53] = 7.5f;
            fArray8[54] = 10.5f;
            fArray8[56] = 7.5f;
            fArray8[57] = 9.5f;
            fArray8[59] = 7.5f;
            fArray8[60] = 8.5f;
            fArray8[62] = 7.5f;
            fArray8[63] = 7.5f;
            fArray8[65] = 7.5f;
            fArray8[66] = 6.5f;
            fArray8[68] = 7.5f;
            fArray8[69] = 5.5f;
            fArray8[71] = 7.5f;
            fArray8[72] = 4.5f;
            fArray8[74] = 7.5f;
            float[] fArray9 = fArray8;
            this.npc2.setShadow(3, 16);
            this.npc2.setParams(fArray9);
            this.npc2.talkto("TalkNPC2a");
            this.npc2.disableDTKFlag(131072);
            this.npc2.enableDTKFlag(12);
            this.npc2.setInvalidID(1);
            this.npc5 = new Enepc();
            this.npc5.init(780, 28, -17.0f, 0.0f, -10.0f, 90.0f);
            this.npc5.id = 15;
            this.npc5.setParams(0, 11, 15, 28);
            float[] fArray10 = new float[33];
            fArray10[0] = -17.0f;
            fArray10[2] = -10.0f;
            fArray10[3] = -16.0f;
            fArray10[5] = -10.0f;
            fArray10[6] = -15.0f;
            fArray10[8] = -10.0f;
            fArray10[9] = -14.0f;
            fArray10[11] = -10.0f;
            fArray10[12] = -13.0f;
            fArray10[14] = -10.0f;
            fArray10[15] = -12.0f;
            fArray10[17] = -10.0f;
            fArray10[18] = -11.0f;
            fArray10[20] = -10.0f;
            fArray10[21] = -10.0f;
            fArray10[23] = -10.0f;
            fArray10[24] = -9.0f;
            fArray10[26] = -10.0f;
            fArray10[27] = -8.0f;
            fArray10[29] = -10.0f;
            fArray10[30] = -7.0f;
            fArray10[32] = -10.0f;
            float[] fArray11 = fArray10;
            this.npc5.setShadow(3, 16);
            this.npc5.setParams(fArray11);
            this.npc5.talkto("TalkNPC5a");
            this.npc5.disableDTKFlag(131072);
            this.npc5.enableDTKFlag(12);
            this.npc5.setInvalidID(1);
            this.npc06 = new Enepc();
            this.npc07 = new Enepc();
            this.npc08 = new Enepc();
            this.npc09 = new Enepc();
            this.npc10 = new Enepc();
            this.npc11 = new Enepc();
            this.npc12 = new Enepc();
            this.npc13 = new Enepc();
            this.npc14 = new Enepc();
            this.npc06.init(1576, 31, -14.3f, 0.0f, -17.25f, 15.0f);
            this.npc07.init(1561, 22, 2.75f, 0.55f, -3.0f, 270.0f);
            this.npc08.init(1579, 35, -6.858f, 0.0f, 6.89755f, 15.0f);
            this.npc09.init(1552, 32, -4.0f, 0.0f, 27.0f, 45.0f);
            this.npc10.init(1567, 34, 15.112f, 0.0f, 16.65f, 30.0f);
            this.npc11.init(1555, 36, 1.538f, 0.0f, -4.0536f, 30.0f);
            this.npc12.init(1588, 33, 15.396f, 0.0f, 17.358f, 90.0f);
            this.npc13.init(1288, 7, -6.385f, 0.0f, 7.848f, 195.0f);
            this.npc14.init(1025, 5, -11.736f, 0.0f, -13.978f, 9.0f);
            this.npc06.id = 16;
            this.npc07.id = 17;
            this.npc08.id = 18;
            this.npc09.id = 19;
            this.npc10.id = 20;
            this.npc11.id = 21;
            this.npc12.id = 22;
            this.npc13.id = 23;
            this.npc14.id = 24;
            this.npc06.setParams(0, 0, 16, 31);
            this.npc07.setParams(0, 0, 17, 22);
            this.npc08.setParams(0, 0, 18, 35);
            this.npc09.setParams(0, 9, 19, 32);
            this.npc10.setParams(0, 0, 20, 34);
            this.npc11.setParams(0, 0, 21, 36);
            this.npc12.setParams(0, 0, 22, 33);
            this.npc13.setParams(0, 0, 23, 7);
            this.npc14.setParams(0, 0, 24, 5);
            float[] fArray12 = new float[21];
            fArray12[0] = -4.0f;
            fArray12[2] = 27.0f;
            fArray12[3] = -3.0f;
            fArray12[5] = 27.0f;
            fArray12[6] = -2.0f;
            fArray12[8] = 27.0f;
            fArray12[9] = -1.0f;
            fArray12[11] = 27.0f;
            fArray12[14] = 27.0f;
            fArray12[15] = 1.0f;
            fArray12[17] = 27.0f;
            fArray12[18] = 2.0f;
            fArray12[20] = 27.0f;
            float[] fArray13 = fArray12;
            this.npc06.setShadow(3, 16);
            this.npc07.setShadow(3, 16);
            this.npc08.setShadow(3, 16);
            this.npc09.setShadow(3, 16);
            this.npc10.setShadow(3, 16);
            this.npc11.setShadow(3, 16);
            this.npc12.setShadow(3, 16);
            this.npc13.setShadow(3, 16);
            this.npc14.setShadow(3, 16);
            this.npc09.setParams(fArray13);
            this.npc06.setMotion(0, 8);
            this.npc07.setMotion(0, 5);
            this.npc08.setMotion(0, 10);
            this.npc10.setMotion(0, 9);
            this.npc11.setMotion(0, 30);
            this.npc13.setMotion(0, 9);
            this.npc14.setMotion(0, 9);
            this.npc06.talkto("TalkNPC06");
            this.npc07.talkto("TalkNPC07");
            this.npc08.talkto("TalkNPC08");
            this.npc09.talkto("TalkNPC09");
            this.npc10.talkto("TalkNPC10");
            this.npc11.talkto("TalkNPC11");
            this.npc12.talkto("TalkNPC12");
            this.npc13.talkto("TalkNPC13");
            this.npc14.talkto("TalkNPC14");
            this.npc06.disableDTKFlag(131075);
            this.npc07.disableDTKFlag(131075);
            this.npc08.disableDTKFlag(131074);
            this.npc09.disableDTKFlag(131072);
            this.npc10.disableDTKFlag(131074);
            this.npc11.disableDTKFlag(131075);
            this.npc12.disableDTKFlag(131074);
            this.npc13.disableDTKFlag(131074);
            this.npc14.disableDTKFlag(131074);
            this.npc06.enableDTKFlag(12);
            this.npc07.enableDTKFlag(12);
            this.npc08.enableDTKFlag(12);
            this.npc09.enableDTKFlag(12);
            this.npc10.enableDTKFlag(12);
            this.npc11.enableDTKFlag(12);
            this.npc12.enableDTKFlag(12);
            this.npc13.enableDTKFlag(12);
            this.npc14.enableDTKFlag(12);
            this.npc06.setInvalidID(1);
            this.npc07.setInvalidID(1);
            this.npc12.setInvalidID(1);
        }
        this.BASE01 = new Uwamono(28672, -12.5f, -1.0f, -14.5f);
        this.BASE02 = new Uwamono(28672, -2.5f, -1.0f, 0.0f);
        this.BASE03 = new Uwamono(28672, 15.5f, -1.0f, 18.0f);
        this.BASE01.SetSize(15.0f, 1.0f, 8.0f);
        this.BASE02.SetSize(5.0f, 1.0f, 10.0f);
        this.BASE03.SetSize(5.0f, 1.0f, 10.0f);
        this.item01 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 209);
        this.item02 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 210);
        this.item03 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 211);
        this.item04 = new Uwamono(28683, 0.0f, 0.0f, 0.0f, 0.0f, 212);
        this.item05 = new Uwamono(28672, 0.0f, 0.0f, 0.0f, 0.0f, 213);
        new Uwamono(0, 20, this.item03);
        new Uwamono(1, 19);
        new Uwamono(2, 20);
        new Uwamono(43, 20, this.item01);
        new Uwamono(44, 20);
        new Uwamono(45, 19, this.item02);
        new Uwamono(17, 106);
        new Uwamono(16, 106, this.item04);
        new Uwamono(4, 32, this.item05);
        this.doorA = new Uwamono(21, 40, '\u0001');
        this.doorA.SetDoorType('\u0004');
        this.doorB = new Uwamono(22, 40, '\u0001');
        this.doorB.SetDoorType('\u0004');
        this.doorC = new Uwamono(23, 40, '\u0001');
        this.doorC.SetDoorType('\u0004');
        this.doorD = new Uwamono(24, 40, '\u0001');
        this.doorD.SetDoorType('\u0004');
        this.doorE = new Uwamono(25, 40, '\u0001');
        this.doorE.SetDoorType('\u0004');
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
    }
}

