/*
 * OV12 original TU 77: 0x00a434b8..0x00a44f08 (35 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_font.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_paint2d.h"
#include "rg_announce.h"

/*
 * The 16-byte fixed color the total line copies before drawing, as two
 * doublewords so the copy is an aligned ld/sd pair.
 */
typedef union RgAnnounceColorHalves {
    long long halves[2];
    XrgPaint2DRGBA rgba;
} RgAnnounceColorHalves;

/*
 * The stack copy _time_disp_title and _damage_disp_title hand to _paint_uvwh.
 * Both titles move their rectangle a word pair at a time: uvOffset is the
 * u/v pair _paint_uvwh feeds to XrgPaint2DSetUVOffset, uvSize the w/h pair it
 * feeds to XrgPaint2DSetUVSize and to the draw call's own width/height.
 */
typedef struct RgAnnounceUvRectHalves {
    long long uvOffset;
    long long uvSize;
} RgAnnounceUvRectHalves;

/* Scaffold-owned strings of _InitAnn: texture names, assert expressions and the
 * asset file name. Their bytes stay in the original .rodata. */
extern const char announceIdHeaderPath[];

extern const char announceRound1PicName[];
extern const char announceAssertRound1PicNotNil[];
extern const char announceRound2PicName[];
extern const char announceAssertRound2PicNotNil[];
extern const char announceRound3PicName[];
extern const char announceAssertRound3PicNotNil[];
extern const char announceReadyPicName[];
extern const char announceAssertReadyPicNotNil[];
extern const char announceFightPicName[];
extern const char announceAssertFightPicNotNil[];
extern const char announceYouWinPicName[];
extern const char announceAssertYouWinPicNotNil[];
extern const char announceYouLosePicName[];
extern const char announceAssertYouLosePicNotNil[];
extern const char announceDrawPicName[];
extern const char announceAssertDrawPicNotNil[];
extern const char announceTimeOverPicName[];
extern const char announceAssertTimeOverPicNotNil[];
extern const char announceGameOverPicName[];
extern const char announceAssertGameOverPicNotNil[];
extern const char announceStageClearPicName[];
extern const char announceAssertStageClearPicNotNil[];
extern const char announceGameClearPicName[];
extern const char announceAssertGameClearPicNotNil[];
extern const char announceContinuePicName[];
extern const char announceAssertContinuePicNotNil[];
extern const char announceCount0PicName[];
extern const char announceAssertCount0PicNotNil[];
extern const char announceCount1PicName[];
extern const char announceAssertCount1PicNotNil[];
extern const char announceCount2PicName[];
extern const char announceAssertCount2PicNotNil[];
extern const char announceCount3PicName[];
extern const char announceAssertCount3PicNotNil[];
extern const char announceCount4PicName[];
extern const char announceAssertCount4PicNotNil[];
extern const char announceCount5PicName[];
extern const char announceAssertCount5PicNotNil[];
extern const char announceCount6PicName[];
extern const char announceAssertCount6PicNotNil[];
extern const char announceCount7PicName[];
extern const char announceAssertCount7PicNotNil[];
extern const char announceCount8PicName[];
extern const char announceAssertCount8PicNotNil[];
extern const char announceCount9PicName[];
extern const char announceAssertCount9PicNotNil[];
extern const char announceTotalLabelPicName[];
extern const char announceAssertTotalLabelPicNotNil[];
extern const char announceTotalValueAPicName[];
extern const char announceAssertTotalValueAPicNotNil[];
extern const char announceTotalValueBPicName[];
extern const char announceAssertTotalValueBPicNotNil[];
extern const char announceTimePointPicName[];
extern const char announceAssertTimePointPicNotNil[];
extern const char announceYouWinSubPicName[];
extern const char announceAssertYouWinSubPicNotNil[];
extern const char announceCount0SubPicName[];
extern const char announceAssertCount0SubPicNotNil[];
extern const char announceCount1SubPicName[];
extern const char announceAssertCount1SubPicNotNil[];
extern const char announceCount2SubPicName[];
extern const char announceAssertCount2SubPicNotNil[];
extern const char announceCount3SubPicName[];
extern const char announceAssertCount3SubPicNotNil[];
extern const char announceCount4SubPicName[];
extern const char announceAssertCount4SubPicNotNil[];
extern const char announceCount5SubPicName[];
extern const char announceAssertCount5SubPicNotNil[];
extern const char announceCount6SubPicName[];
extern const char announceAssertCount6SubPicNotNil[];
extern const char announceCount7SubPicName[];
extern const char announceAssertCount7SubPicNotNil[];
extern const char announcePicNameCount82[];
extern const char announceAssertPicCount82NotNil[];
extern const char announcePicNameCount92[];
extern const char announceAssertPicCount92NotNil[];
extern const char announcePicNameDraw2[];
extern const char announceAssertPicDraw2NotNil[];
extern const char announcePicNameFight2[];
extern const char announceAssertPicFight2NotNil[];
extern const char announcePicNameGmcler2[];
extern const char announceAssertPicGmcler2NotNil[];
extern const char announcePicNameGmover2[];
extern const char announceAssertPicGmover2NotNil[];
extern const char announcePicNameReady2[];
extern const char announceAssertPicReady2NotNil[];
extern const char announcePicNameRound12[];
extern const char announceAssertPicRound12NotNil[];
extern const char announcePicNameRound22[];
extern const char announceAssertPicRound22NotNil[];
extern const char announcePicNameRound32[];
extern const char announceAssertPicRound32NotNil[];
extern const char announcePicNameStcler2[];
extern const char announceAssertPicStcler2NotNil[];
extern const char announcePicNameTmover2[];
extern const char announceAssertPicTmover2NotNil[];
extern const char announcePicNameYoulos2[];
extern const char announceAssertPicYoulos2NotNil[];
extern const char announcePicNameCntinu2[];
extern const char announceAssertPicCntinu2NotNil[];

/* ov12:0x00a586a0: the fixed color the total line copies before drawing (scaffold .rodata). */
extern const RgAnnounceColorHalves announceClearPanelColor;

static void _InitAnn(RgAnnounce *pAnn);
static void _time_disp_title(RgAnnounce *pAnn, int x, int y);
static void _time_disp(RgAnnounce *pAnn, int x, int y, unsigned int time);
static void _damage_disp_title(RgAnnounce *pAnn, int x, int y);
static void _damage_disp(RgAnnounce *pAnn, int x, int y, float damage);
static void _paint_flush(RgAnnounce *pAnn);
static void _paint_uvwh(RgAnnounce *pAnn, int index, int x, int y,
                        const RgAnnounceUvRect *rect);
static void _paint_add(RgAnnounce *pAnn, int index, int x, int y);
static void _paint_sub(RgAnnounce *pAnn, int index, int x, int y);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a57bc0 contains the source filename "../rg_announce.euc.c".
 * ov12:0x00a57bb0 contains the assertion expression "pAnn != NIL".
 */
const char announceAssertAnnNotNil[] = "pAnn != NIL";

const char announceSourcePath[] = "../rg_announce.euc.c";

static void _InitAnn(RgAnnounce *pAnn)
{
    RgBattleCommonDataEnv *common;
    RgBxx *addedCommon;
    RgBxx *subCommon;

    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 76);
    }
    common = InstanceOfRgBattleCommonData();
    addedCommon = RgBattleCommonDataAnnounceAdd(common);
    subCommon = RgBattleCommonDataAnnounceSub(common);

    pAnn->textures[0] = RgBxxGetPic(addedCommon, announceRound1PicName);
    if (pAnn->textures[0] == 0) {
        assert_prog(announceAssertRound1PicNotNil, announceIdHeaderPath, 1);
    }
    pAnn->textures[1] = RgBxxGetPic(addedCommon, announceRound2PicName);
    if (pAnn->textures[1] == 0) {
        assert_prog(announceAssertRound2PicNotNil, announceIdHeaderPath, 2);
    }
    pAnn->textures[2] = RgBxxGetPic(addedCommon, announceRound3PicName);
    if (pAnn->textures[2] == 0) {
        assert_prog(announceAssertRound3PicNotNil, announceIdHeaderPath, 3);
    }
    pAnn->textures[3] = RgBxxGetPic(addedCommon, announceReadyPicName);
    if (pAnn->textures[3] == 0) {
        assert_prog(announceAssertReadyPicNotNil, announceIdHeaderPath, 4);
    }
    pAnn->textures[4] = RgBxxGetPic(addedCommon, announceFightPicName);
    if (pAnn->textures[4] == 0) {
        assert_prog(announceAssertFightPicNotNil, announceIdHeaderPath, 5);
    }
    pAnn->textures[5] = RgBxxGetPic(addedCommon, announceYouWinPicName);
    if (pAnn->textures[5] == 0) {
        assert_prog(announceAssertYouWinPicNotNil, announceIdHeaderPath, 6);
    }
    pAnn->textures[6] = RgBxxGetPic(addedCommon, announceYouLosePicName);
    if (pAnn->textures[6] == 0) {
        assert_prog(announceAssertYouLosePicNotNil, announceIdHeaderPath, 7);
    }
    pAnn->textures[7] = RgBxxGetPic(addedCommon, announceDrawPicName);
    if (pAnn->textures[7] == 0) {
        assert_prog(announceAssertDrawPicNotNil, announceIdHeaderPath, 8);
    }
    pAnn->textures[8] = RgBxxGetPic(addedCommon, announceTimeOverPicName);
    if (pAnn->textures[8] == 0) {
        assert_prog(announceAssertTimeOverPicNotNil, announceIdHeaderPath, 9);
    }
    pAnn->textures[9] = RgBxxGetPic(addedCommon, announceGameOverPicName);
    if (pAnn->textures[9] == 0) {
        assert_prog(announceAssertGameOverPicNotNil, announceIdHeaderPath, 10);
    }
    pAnn->textures[10] = RgBxxGetPic(addedCommon, announceStageClearPicName);
    if (pAnn->textures[10] == 0) {
        assert_prog(announceAssertStageClearPicNotNil, announceIdHeaderPath, 11);
    }
    pAnn->textures[11] = RgBxxGetPic(addedCommon, announceGameClearPicName);
    if (pAnn->textures[11] == 0) {
        assert_prog(announceAssertGameClearPicNotNil, announceIdHeaderPath, 12);
    }
    pAnn->textures[12] = RgBxxGetPic(addedCommon, announceContinuePicName);
    if (pAnn->textures[12] == 0) {
        assert_prog(announceAssertContinuePicNotNil, announceIdHeaderPath, 13);
    }
    pAnn->textures[13] = RgBxxGetPic(addedCommon, announceCount0PicName);
    if (pAnn->textures[13] == 0) {
        assert_prog(announceAssertCount0PicNotNil, announceIdHeaderPath, 14);
    }
    pAnn->textures[14] = RgBxxGetPic(addedCommon, announceCount1PicName);
    if (pAnn->textures[14] == 0) {
        assert_prog(announceAssertCount1PicNotNil, announceIdHeaderPath, 15);
    }
    pAnn->textures[15] = RgBxxGetPic(addedCommon, announceCount2PicName);
    if (pAnn->textures[15] == 0) {
        assert_prog(announceAssertCount2PicNotNil, announceIdHeaderPath, 16);
    }
    pAnn->textures[16] = RgBxxGetPic(addedCommon, announceCount3PicName);
    if (pAnn->textures[16] == 0) {
        assert_prog(announceAssertCount3PicNotNil, announceIdHeaderPath, 17);
    }
    pAnn->textures[17] = RgBxxGetPic(addedCommon, announceCount4PicName);
    if (pAnn->textures[17] == 0) {
        assert_prog(announceAssertCount4PicNotNil, announceIdHeaderPath, 18);
    }
    pAnn->textures[18] = RgBxxGetPic(addedCommon, announceCount5PicName);
    if (pAnn->textures[18] == 0) {
        assert_prog(announceAssertCount5PicNotNil, announceIdHeaderPath, 19);
    }
    pAnn->textures[19] = RgBxxGetPic(addedCommon, announceCount6PicName);
    if (pAnn->textures[19] == 0) {
        assert_prog(announceAssertCount6PicNotNil, announceIdHeaderPath, 20);
    }
    pAnn->textures[20] = RgBxxGetPic(addedCommon, announceCount7PicName);
    if (pAnn->textures[20] == 0) {
        assert_prog(announceAssertCount7PicNotNil, announceIdHeaderPath, 21);
    }
    pAnn->textures[21] = RgBxxGetPic(addedCommon, announceCount8PicName);
    if (pAnn->textures[21] == 0) {
        assert_prog(announceAssertCount8PicNotNil, announceIdHeaderPath, 22);
    }
    pAnn->textures[22] = RgBxxGetPic(addedCommon, announceCount9PicName);
    if (pAnn->textures[22] == 0) {
        assert_prog(announceAssertCount9PicNotNil, announceIdHeaderPath, 23);
    }
    pAnn->textures[23] = RgBxxGetPic(addedCommon, announceTotalLabelPicName);
    if (pAnn->textures[23] == 0) {
        assert_prog(announceAssertTotalLabelPicNotNil, announceIdHeaderPath, 24);
    }
    pAnn->textures[24] = RgBxxGetPic(addedCommon, announceTotalValueAPicName);
    if (pAnn->textures[24] == 0) {
        assert_prog(announceAssertTotalValueAPicNotNil, announceIdHeaderPath, 25);
    }
    pAnn->textures[25] = RgBxxGetPic(addedCommon, announceTotalValueBPicName);
    if (pAnn->textures[25] == 0) {
        assert_prog(announceAssertTotalValueBPicNotNil, announceIdHeaderPath, 26);
    }
    pAnn->textures[26] = RgBxxGetPic(addedCommon, announceTimePointPicName);
    if (pAnn->textures[26] == 0) {
        assert_prog(announceAssertTimePointPicNotNil, announceIdHeaderPath, 27);
    }

    pAnn->textures[27] = RgBxxGetPic(subCommon, announceYouWinSubPicName);
    if (pAnn->textures[27] == 0) {
        assert_prog(announceAssertYouWinSubPicNotNil, announceIdHeaderPath, 29);
    }
    pAnn->textures[28] = RgBxxGetPic(subCommon, announceCount0SubPicName);
    if (pAnn->textures[28] == 0) {
        assert_prog(announceAssertCount0SubPicNotNil, announceIdHeaderPath, 30);
    }
    pAnn->textures[29] = RgBxxGetPic(subCommon, announceCount1SubPicName);
    if (pAnn->textures[29] == 0) {
        assert_prog(announceAssertCount1SubPicNotNil, announceIdHeaderPath, 31);
    }
    pAnn->textures[30] = RgBxxGetPic(subCommon, announceCount2SubPicName);
    if (pAnn->textures[30] == 0) {
        assert_prog(announceAssertCount2SubPicNotNil, announceIdHeaderPath, 32);
    }
    pAnn->textures[31] = RgBxxGetPic(subCommon, announceCount3SubPicName);
    if (pAnn->textures[31] == 0) {
        assert_prog(announceAssertCount3SubPicNotNil, announceIdHeaderPath, 33);
    }
    pAnn->textures[32] = RgBxxGetPic(subCommon, announceCount4SubPicName);
    if (pAnn->textures[32] == 0) {
        assert_prog(announceAssertCount4SubPicNotNil, announceIdHeaderPath, 34);
    }
    pAnn->textures[33] = RgBxxGetPic(subCommon, announceCount5SubPicName);
    if (pAnn->textures[33] == 0) {
        assert_prog(announceAssertCount5SubPicNotNil, announceIdHeaderPath, 35);
    }
    pAnn->textures[34] = RgBxxGetPic(subCommon, announceCount6SubPicName);
    if (pAnn->textures[34] == 0) {
        assert_prog(announceAssertCount6SubPicNotNil, announceIdHeaderPath, 36);
    }
    pAnn->textures[35] = RgBxxGetPic(subCommon, announceCount7SubPicName);
    if (pAnn->textures[35] == 0) {
        assert_prog(announceAssertCount7SubPicNotNil, announceIdHeaderPath, 37);
    }
    pAnn->textures[36] = RgBxxGetPic(subCommon, announcePicNameCount82);
    if (pAnn->textures[36] == 0) {
        assert_prog(announceAssertPicCount82NotNil, announceIdHeaderPath, 38);
    }
    pAnn->textures[37] = RgBxxGetPic(subCommon, announcePicNameCount92);
    if (pAnn->textures[37] == 0) {
        assert_prog(announceAssertPicCount92NotNil, announceIdHeaderPath, 39);
    }
    pAnn->textures[38] = RgBxxGetPic(subCommon, announcePicNameDraw2);
    if (pAnn->textures[38] == 0) {
        assert_prog(announceAssertPicDraw2NotNil, announceIdHeaderPath, 40);
    }
    pAnn->textures[39] = RgBxxGetPic(subCommon, announcePicNameFight2);
    if (pAnn->textures[39] == 0) {
        assert_prog(announceAssertPicFight2NotNil, announceIdHeaderPath, 41);
    }
    pAnn->textures[40] = RgBxxGetPic(subCommon, announcePicNameGmcler2);
    if (pAnn->textures[40] == 0) {
        assert_prog(announceAssertPicGmcler2NotNil, announceIdHeaderPath, 42);
    }
    pAnn->textures[41] = RgBxxGetPic(subCommon, announcePicNameGmover2);
    if (pAnn->textures[41] == 0) {
        assert_prog(announceAssertPicGmover2NotNil, announceIdHeaderPath, 43);
    }
    pAnn->textures[42] = RgBxxGetPic(subCommon, announcePicNameReady2);
    if (pAnn->textures[42] == 0) {
        assert_prog(announceAssertPicReady2NotNil, announceIdHeaderPath, 44);
    }
    pAnn->textures[43] = RgBxxGetPic(subCommon, announcePicNameRound12);
    if (pAnn->textures[43] == 0) {
        assert_prog(announceAssertPicRound12NotNil, announceIdHeaderPath, 45);
    }
    pAnn->textures[44] = RgBxxGetPic(subCommon, announcePicNameRound22);
    if (pAnn->textures[44] == 0) {
        assert_prog(announceAssertPicRound22NotNil, announceIdHeaderPath, 46);
    }
    pAnn->textures[45] = RgBxxGetPic(subCommon, announcePicNameRound32);
    if (pAnn->textures[45] == 0) {
        assert_prog(announceAssertPicRound32NotNil, announceIdHeaderPath, 47);
    }
    pAnn->textures[46] = RgBxxGetPic(subCommon, announcePicNameStcler2);
    if (pAnn->textures[46] == 0) {
        assert_prog(announceAssertPicStcler2NotNil, announceIdHeaderPath, 48);
    }
    pAnn->textures[47] = RgBxxGetPic(subCommon, announcePicNameTmover2);
    if (pAnn->textures[47] == 0) {
        assert_prog(announceAssertPicTmover2NotNil, announceIdHeaderPath, 49);
    }
    pAnn->textures[48] = RgBxxGetPic(subCommon, announcePicNameYoulos2);
    if (pAnn->textures[48] == 0) {
        assert_prog(announceAssertPicYoulos2NotNil, announceIdHeaderPath, 50);
    }
    pAnn->textures[49] = RgBxxGetPic(subCommon, announcePicNameCntinu2);
    if (pAnn->textures[49] == 0) {
        assert_prog(announceAssertPicCntinu2NotNil, announceIdHeaderPath, 51);
    }

    pAnn->timer = RG_ANNOUNCE_TIMER_INFINITE;
    pAnn->paint = CreateXrgPaint2D_sub(announceSourcePath, 91);
    XrgPaint2DSetDrawPrio(pAnn->paint, 5);
    pAnn->drawFunc = 0;
    pAnn->pendingDispY = RG_ANNOUNCE_DISP_Y_DEFAULT;
    pAnn->dispY = RG_ANNOUNCE_DISP_Y_DEFAULT;
    pAnn->stageClearActive = 0;
    pAnn->gameClearActive = 0;
}

static void _DestructAnn(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 102);
    }
    DisposeXrgPaint2D_sub(pAnn->paint, announceSourcePath, 103);
}

RgAnnounce *CreateRgAnnounce(void)
{
    RgAnnounce *pAnn;

    pAnn = RgHeapAlloc(InstanceOfRgHeap(), RG_ANNOUNCE_SIZE, announceSourcePath, 110);
    _InitAnn(pAnn);
    return pAnn;
}

void DisposeRgAnnounce(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 118);
    }
    _DestructAnn(pAnn);
    RgHeapFree(InstanceOfRgHeap(), pAnn, announceSourcePath, 120);
}

void RgAnnouncePassTime(RgAnnounce *pAnn, float deltaTime)
{
    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 129);
    }
    if (pAnn->timer != RG_ANNOUNCE_TIMER_INFINITE) {
        pAnn->timer -= deltaTime;
        if (pAnn->timer < 0.0f) {
            pAnn->timer = 0.0f;
            pAnn->drawFunc = 0;
        }
    }
}

void RgAnnounceDisp(RgAnnounce *pAnn)
{
    unsigned int totalTime;
    float totalDamage;
    unsigned int i;
    int y;

    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 154);
    }
    if (pAnn->drawFunc != 0) {
        pAnn->drawFunc(pAnn);
    }
    if (pAnn->stageClearActive != 0) {
        XrgPaint2D *paint;
        XrgPaint2DRGBA color;
        int alpha;
        int fadeAlpha;
        int stageTick;

        paint = pAnn->paint;
        memset(&color, 0, sizeof(color));
        color.a = RG_ANNOUNCE_CLEAR_PANEL_ALPHA;
        alpha = 0;
        stageTick = pAnn->stageClearTick + 1;
        fadeAlpha = stageTick * RG_ANNOUNCE_CLEAR_FADE_STEP;
        pAnn->stageClearTick = stageTick;
        if (fadeAlpha >= 0) {
            if (fadeAlpha <= color.a) {
                alpha = fadeAlpha;
            } else {
                alpha = color.a;
            }
        }
        color.a = alpha;
        XrgPaint2DColor(paint, &color);
        XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
        XrgPaint2DDrawXYWH(paint, 0, 0, 218, RG_ANNOUNCE_SCREEN_WIDTH, 128);
        if (color.a >= RG_ANNOUNCE_CLEAR_PANEL_ALPHA) {
            _time_disp_title(pAnn, 0, 232);
            _time_disp(pAnn, 0, 263, pAnn->stageClearTime);
            _damage_disp_title(pAnn, 0, 289);
            _damage_disp(pAnn, 0, 313, pAnn->stageClearDamage);
        }
    }
    if (pAnn->gameClearActive != 0) {
        XrgPaint2D *paint;
        XrgPaint2DRGBA color;
        RgAnnounceColorHalves fixedColor;
        int alpha;
        int fadeAlpha;
        int stageTick;

        paint = pAnn->paint;
        memset(&color, 0, sizeof(color));
        color.a = RG_ANNOUNCE_CLEAR_PANEL_ALPHA;
        totalTime = 0;
        totalDamage = 0.0f;
        stageTick = pAnn->stageClearTick + 1;
        pAnn->stageClearTick = stageTick;
        fadeAlpha = stageTick * RG_ANNOUNCE_CLEAR_FADE_STEP;
        alpha = 0;
        if (fadeAlpha >= 0) {
            if (fadeAlpha <= color.a) {
                alpha = fadeAlpha;
            } else {
                alpha = color.a;
            }
        }
        color.a = alpha;
        XrgPaint2DColor(paint, &color);
        XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
        XrgPaint2DDrawXYWH(paint, 0, 0, 200, RG_ANNOUNCE_SCREEN_WIDTH, 150);
        if (color.a >= RG_ANNOUNCE_CLEAR_PANEL_ALPHA) {
            fixedColor = announceClearPanelColor;
            i = 0;
            _time_disp_title(pAnn, -128, 202);
            _damage_disp_title(pAnn, 128, 206);
            if (pAnn->m_uStage != 0) {
                do {
                    y = 232 + i * 20;
                    _time_disp(pAnn, -128, y, pAnn->m_auTotalTime[i]);
                    _damage_disp(pAnn, 128, y, pAnn->m_afTotalDamage[i]);
                    totalTime += pAnn->m_auTotalTime[i];
                    totalDamage += pAnn->m_afTotalDamage[i];
                    i++;
                } while (i < pAnn->m_uStage);
            }
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DColor(paint, &fixedColor.rgba);
            XrgPaint2DDrawXYWH(paint, 0, 0, 316, RG_ANNOUNCE_SCREEN_WIDTH, 1);
            _time_disp(pAnn, -128, 322, totalTime);
            _damage_disp(pAnn, 128, 322, totalDamage);
        }
    }
    _paint_flush(pAnn);
}

/*
 * The two fixed title rectangles, asm-owned scaffold data (splat names, no
 * config/symbols/ov12.txt entry): ov12:0x00a586b0 belongs to the time title
 * and ov12:0x00a586c0 to the damage title.
 */
const RgAnnounceUvRectHalves announceTimeTitleUvRect = {
    0, 0x0000001800000200LL
};

const RgAnnounceUvRectHalves announceDamageTitleUvRect = {
    0x0000003800000000LL, 0x0000001200000200LL
};

static void _time_disp_title(RgAnnounce *pAnn, int x, int y)
{
    RgAnnounceUvRectHalves rect;

    rect.uvSize = announceTimeTitleUvRect.uvSize;
    rect.uvOffset = announceTimeTitleUvRect.uvOffset;
    XrgPaint2DAlpha(pAnn->paint, XRG_PAINT2D_BLEND_ADD);
    _paint_uvwh(pAnn, RG_ANNOUNCE_TEX_TITLE_BAR, x, y, (const RgAnnounceUvRect *) &rect);
}

static void _time_disp(RgAnnounce *pAnn, int x, int y, unsigned int time)
{
    XrgPaint2D *paint;
    RgFont *font;
    unsigned int minutes;
    unsigned int rest;
    unsigned int centiseconds;
    char text[16];

    paint = pAnn->paint;
    font = RgBattleCommonDataAnnounceFont(InstanceOfRgBattleCommonData());
    minutes = (time / 60000) % 10;
    text[0] = '0' + minutes;
    text[1] = '0' + (time / 6000) % 10;
    text[2] = '\0';
    RgFontStr(paint, font, text, x + 227, y, 0);
    rest = time % 6000;
    text[0] = '0' + (rest / 1000u) % 10u;
    text[1] = '0' + (rest / 100u) % 10u;
    RgFontStr(paint, font, text, x + 256, y, 0);
    centiseconds = rest % 100;
    text[0] = '0' + (centiseconds / 10) % 10;
    text[1] = '0' + centiseconds % 10;
    RgFontStr(paint, font, text, x + 290, y, 0);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_TIME_LABEL, x, y - 8);
}

static void _damage_disp_title(RgAnnounce *pAnn, int x, int y)
{
    RgAnnounceUvRectHalves rect;

    rect.uvSize = announceDamageTitleUvRect.uvSize;
    rect.uvOffset = announceDamageTitleUvRect.uvOffset;
    XrgPaint2DAlpha(pAnn->paint, XRG_PAINT2D_BLEND_ADD);
    _paint_uvwh(pAnn, RG_ANNOUNCE_TEX_TITLE_BAR, x, y, (const RgAnnounceUvRect *) &rect);
}

/* ov12:0x00a586d0 is the scaffold-owned "%d" format string. */
const char announceDamageFormat[] = "%d";

static void _damage_disp(RgAnnounce *pAnn, int x, int y, float damage)
{
    XrgPaint2D *paint;
    RgFont *font;
    unsigned int damageValue;
    char damageText[32];

    paint = pAnn->paint;
    font = RgBattleCommonDataAnnounceFont(InstanceOfRgBattleCommonData());
    damageValue = (unsigned int) floorf(damage);
    sprintf(damageText, announceDamageFormat, damageValue);
    RgFontStr(paint, font, damageText, x + 0x11F, y, 1);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_DAMAGE_LABEL, x, y - 0x10);
}

static void _round1(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_ROUND1_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_ROUND1_ADD, 0, pAnn->dispY);
}

static void _round2(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_ROUND2_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_ROUND2_ADD, 0, pAnn->dispY);
}

static void _round3(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_ROUND3_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_ROUND3_ADD, 0, pAnn->dispY);
}

static void _ready(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_READY_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_READY_ADD, 0, pAnn->dispY);
}

static void _fight(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_FIGHT_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_FIGHT_ADD, 0, pAnn->dispY);
}

static void _youwin(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_YOU_WIN_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_YOU_WIN_ADD, 0, pAnn->dispY);
}

static void _youlose(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_YOU_LOSE_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_YOU_LOSE_ADD, 0, pAnn->dispY);
}

static void _draw(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_DRAW_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_DRAW_ADD, 0, pAnn->dispY);
}

static void _gameover(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_GAME_OVER_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_GAME_OVER_ADD, 0, pAnn->dispY);
}

static void _timeover(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_TIME_OVER_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_TIME_OVER_ADD, 0, pAnn->dispY);
}

static void _stageclear(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_STAGE_CLEAR_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_STAGE_CLEAR_ADD, 0, pAnn->dispY);
}

static void _gameclear(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_GAME_CLEAR_SUB, 0, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_GAME_CLEAR_ADD, 0, pAnn->dispY);
}

static void _you_win_you_lose(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_YOU_WIN_SUB, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_YOU_WIN_ADD, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_YOU_LOSE_SUB, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_YOU_LOSE_ADD, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
}

static void _you_lose_you_win(RgAnnounce *pAnn) {
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_YOU_WIN_SUB, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_YOU_WIN_ADD, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_sub(pAnn, RG_ANNOUNCE_TEX_YOU_LOSE_SUB, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, RG_ANNOUNCE_TEX_YOU_LOSE_ADD, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
}

static void _paint(XrgPaint2D *paint, struct RgBxxPic *texture, int blend, int x, int y)
{
    if (texture != 0) {
        XrgPaint2DUseTexture(paint, texture);
        XrgPaint2DAlpha(paint, blend);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
    }
}

static void _paint_uvwh(RgAnnounce *pAnn, int index, int x, int y,
                        const RgAnnounceUvRect *rect)
{
    XrgPaint2D *paint = pAnn->paint;

    XrgPaint2DUseTexture(paint, pAnn->textures[index]);
    XrgPaint2DSetUVOffset(paint, rect->u, rect->v);
    XrgPaint2DSetUVSize(paint, rect->w, rect->h);
    XrgPaint2DDrawXYWH(paint, 0, x, y, rect->w, rect->h);
}

static void _paint_lin(RgAnnounce *pAnn, int index, int x, int y)
{
    _paint(pAnn->paint, pAnn->textures[index], XRG_PAINT2D_BLEND_LINEAR, x, y);
}

static void _paint_add(RgAnnounce *pAnn, int index, int x, int y)
{
    _paint(pAnn->paint, pAnn->textures[index], XRG_PAINT2D_BLEND_ADD, x, y);
}

static void _paint_sub(RgAnnounce *pAnn, int index, int x, int y)
{
    _paint(pAnn->paint, pAnn->textures[index], XRG_PAINT2D_BLEND_SUB, x, y);
}

static void _paint_flush(RgAnnounce *pAnn)
{
    XrgPaint2DFlush(pAnn->paint);
}

void RgAnnounceDispInit(RgAnnounce *pAnn, int kind)
{
    static RgAnnounceDrawFunc s_apFuncs[RG_ANNOUNCE_DRAW_KIND_COUNT] = {
        _round1, _round2, _round3, _ready, _fight, _youwin, _youlose,
        _draw, _gameover, _timeover, _stageclear, _gameclear,
        _you_win_you_lose, _you_lose_you_win
    };
    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 466);
    }
    if ((unsigned int) kind < RG_ANNOUNCE_DRAW_KIND_COUNT) {
        pAnn->timer = RG_ANNOUNCE_TIMER_INFINITE;
        pAnn->drawFunc = s_apFuncs[kind];
    } else {
        pAnn->drawFunc = 0;
    }
    pAnn->dispY = pAnn->pendingDispY;
    pAnn->pendingDispY = RG_ANNOUNCE_DISP_Y_DEFAULT;
}

void RgAnnounceDispInitTime(RgAnnounce *pAnn, int kind, float duration)
{
    RgAnnounceDispInit(pAnn, kind);
    if (pAnn->drawFunc != 0) {
        pAnn->timer = duration;
    }
}

void RgAnnounceDispOff(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 493);
    }
    pAnn->drawFunc = 0;
    pAnn->stageClearActive = 0;
}

void RgAnnounceSetStageClear(RgAnnounce *pAnn, int time, float damage)
{
    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 505);
    }
    pAnn->stageClearTime = time;
    pAnn->stageClearDamage = damage;
    pAnn->gameClearActive = 0;
    pAnn->stageClearActive = 1;
    pAnn->stageClearTick = 0;
}

/* "uStage < sizeof(pAnn->m_auTotalTime) / sizeof(pAnn->m_auTotalTime[0])" */
const char announceAssertStageInTotalTimeRange[] =
    "uStage < sizeof(pAnn->m_auTotalTime) / sizeof(pAnn->m_auTotalTime[0])";

void RgAnnounceSetGameClear(RgAnnounce *pAnn, const unsigned int *auTime,
                            const float *afDamage, unsigned int uStage)
{
    unsigned int i;

    if (pAnn == 0) {
        assert_prog(announceAssertAnnNotNil, announceSourcePath, 521);
    }
    if (uStage >= RG_ANNOUNCE_GAMECLEAR_MAX) {
        assert_prog(announceAssertStageInTotalTimeRange, announceSourcePath, 522);
    }
    for (i = 0; i < uStage; i++) {
        pAnn->m_auTotalTime[i] = auTime[i];
        pAnn->m_afTotalDamage[i] = afDamage[i];
    }
    pAnn->m_uStage = uStage;
    pAnn->gameClearActive = 1;
    pAnn->stageClearActive = 0;
    pAnn->stageClearTick = 0;
}
