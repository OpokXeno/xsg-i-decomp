/*
 * OV12 original TU 43: 0x00a25d30..0x00a26a00 (12 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_disp_life.h"
#include "ov12/rg_gauge.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_robot_spec.h"
#include "ov12/rg_font.h"
#include "ov12/rg_bxx.h"

#define RG_DISP_LIFE_PLAYERS 2
#define RG_DISP_LIFE_MAX_WINS 2
#define RG_DISP_LIFE_AGWS_NAMES 6

/* Life-bar picture slots: three win-count pictures per player. */
#define RG_DISP_LIFE_WIN_PICS_PER_PLAYER 3
/* lifePic[6] and [7] (12 pointers past secondaryBoardPic) are the players' boards. */

/* The win picture of the player who just won blinks with this period. */
#define RG_DISP_LIFE_BLINK_PERIOD 1.0f
#define RG_DISP_LIFE_BLINK_HALF 0.5f

/* The timer text counts centiseconds: m'ss"cc. */
#define RG_DISP_LIFE_CENTIS_PER_MINUTE 6000U


/*
 * The AGWS name texture's UV rectangle _GetAgwsNameUVWH fills: u, v feed
 * XrgPaint2DSetUVOffset and w, h feed both XrgPaint2DSetUVSize and the draw
 * call's own width/height.
 */
typedef struct AgwsNameUvwh {
    int u;
    int v;
    int w;
    int h;
} AgwsNameUvwh;

/* One entry of the character-id to name-texture table (32 bytes). */
typedef struct AgwsNameRecord {
    int characterId;
    unsigned char unmodeled_04[12];
    int texture_u;
    int texture_v;
    int texture_width;
    int texture_height;
} AgwsNameRecord;

static void _InitDisp(RgDispLife *pDisp, RgBxx *pBxx, RgFont *timeFont);
static void _DestructDisp(RgDispLife *pDisp);
static int _GetAgwsNameUVWH(int nameIndex, AgwsNameUvwh *uvwh);

/* Scaffold-owned strings (string table at 0x00a54848..0x00a54940). */
extern const char dispLifeAssertBxxNotNil[];
extern const char dispLifeAssertTimeFontNotNil[];
extern const char dispLifeAgwsNamePictureName[];
extern const char dispLifeLeftWindow0PictureName[];
extern const char dispLifeLeftWindow1PictureName[];
extern const char dispLifeLeftWindow2PictureName[];
extern const char dispLifeRightWindow0PictureName[];
extern const char dispLifeRightWindow1PictureName[];
extern const char dispLifeRightWindow2PictureName[];
extern const char dispLifeLeftBarPictureName[];
extern const char dispLifeRightBarPictureName[];
extern const char dispLifeBarTop1PictureName[];
extern const char dispLifeBarTopDeadPictureName[];
extern const char dispLifeBarTop2PictureName[];
static int s_anPos_1[2][2];

static AgwsNameRecord s_aCharIDToUVWH_0[RG_DISP_LIFE_AGWS_NAMES] = {
    {0, {0}, 0, 0, 88, 16},
    {2, {0}, 168, 0, 80, 16},
    {4, {0}, 88, 0, 80, 16},
    {5, {0}, 0, 32, 88, 16},
    {3, {0}, 88, 32, 80, 16},
    {1, {0}, 168, 32, 80, 16}
};

const char dispLifeAssertPictureNotNil[] = "pPic != NIL";

const char dispLifeSourcePath[] = "../rg_disp_life.euc.c";

const char dispLifeAssertDispNotNil[] = "pDisp != NIL";

const char dispLifePrimaryBoardPictureName[] = "board_na.bmp";

const char dispLifeSecondaryBoardPictureName[] = "board_nb.bmp";

const char dispLifeVsPrimaryBoardPictureName[] = "board_nc.bmp";

const char dispLifeVsSecondaryBoardPictureName[16] = "board_nd.bmp";

static int _GetAgwsNameUVWH(int nameIndex, AgwsNameUvwh *uvwh)
{
    unsigned int index;

    for (index = 0; index < RG_DISP_LIFE_AGWS_NAMES; index++) {
        if (s_aCharIDToUVWH_0[index].characterId == nameIndex) {
            /*
             * Each record is 32 bytes, with an aligned 16-byte UVWH vector
             * at +0x10. VF31 carries those four words into the output object;
             * it is scratch VU state for this transfer.
             */
            __asm__ __volatile__(
                "lqc2 vf31, 0(%0)\n\t"
                "sqc2 vf31, 0(%1)\n\t"
                :
                : "r"(&s_aCharIDToUVWH_0[index].texture_u), "r"(uvwh)
                : "memory");
            return 1;
        }
    }
    return 0;
}

static void _DispAgwsName(XrgPaint2D *paint, RgBxxPic *pPic, int nameIndex,
                          int x, int y)
{
    AgwsNameUvwh uvwh;

    if (pPic == 0) {
        assert_prog(dispLifeAssertPictureNotNil, dispLifeSourcePath, 100);
    }
    if (_GetAgwsNameUVWH(nameIndex, &uvwh) && (pPic != 0)) {
        XrgPaint2DUseTexture(paint, pPic);
        XrgPaint2DSetUVOffset(paint, uvwh.u, uvwh.v);
        XrgPaint2DSetUVSize(paint, uvwh.w, uvwh.h);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, 0, x, y, uvwh.w, uvwh.h);
    }
}

static void _InitDisp(RgDispLife *pDisp, RgBxx *pBxx, RgFont *timeFont)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 117);
    }
    if (pBxx == 0) {
        assert_prog(dispLifeAssertBxxNotNil, dispLifeSourcePath, 118);
    }
    if (timeFont == 0) {
        assert_prog(dispLifeAssertTimeFontNotNil, dispLifeSourcePath, 119);
    }
    pDisp->timeFont = timeFont;
    pDisp->timer = 0.0f;
    pDisp->vsMode = 0;
    pDisp->dispBxx = pBxx;
    pDisp->primaryBoardPic = RgBxxGetPic(pBxx, dispLifePrimaryBoardPictureName);
    pDisp->secondaryBoardPic = RgBxxGetPic(pBxx, dispLifeSecondaryBoardPictureName);
    pDisp->agwsNamePic = RgBxxGetPic(pBxx, dispLifeAgwsNamePictureName);
    pDisp->lifePic[0] = RgBxxGetPic(pBxx, dispLifeLeftWindow0PictureName);
    pDisp->lifePic[1] = RgBxxGetPic(pBxx, dispLifeLeftWindow1PictureName);
    pDisp->lifePic[2] = RgBxxGetPic(pBxx, dispLifeLeftWindow2PictureName);
    pDisp->lifePic[3] = RgBxxGetPic(pBxx, dispLifeRightWindow0PictureName);
    pDisp->lifePic[4] = RgBxxGetPic(pBxx, dispLifeRightWindow1PictureName);
    pDisp->lifePic[5] = RgBxxGetPic(pBxx, dispLifeRightWindow2PictureName);
    pDisp->lifePic[6] = RgBxxGetPic(pBxx, dispLifeLeftBarPictureName);
    pDisp->lifePic[7] = RgBxxGetPic(pBxx, dispLifeRightBarPictureName);
    pDisp->winnerMask = 0;
    pDisp->elapsedTime = 0.0f;
    for (i = 0; i < RG_DISP_LIFE_PLAYERS; ++i) {
        RgGaugeDisp *gaugeDisp;
        RgBxxPic *barPic;
        RgBxxPic *restPic;

        pDisp->robots[i] = 0;
        pDisp->gauge[i] = CreateRgGauge();
        pDisp->winCount[i] = 0;
        gaugeDisp = RgGaugeGetDisp(pDisp->gauge[i]);
        if (i == 0) {
            barPic = RgBxxGetPic(pBxx, dispLifeBarTop1PictureName);
            restPic = RgBxxGetPic(pBxx, dispLifeBarTopDeadPictureName);
            RgGaugeDispSetTex(gaugeDisp, barPic, restPic);
            RgGaugeDispSetBarUVWH(gaugeDisp, 14, 8, 183, 12);
            RgGaugeDispSetRestUVWH(gaugeDisp, 0, 0, 0, 12);
            RgGaugeDispSetAlpha(gaugeDisp, 1);
            RgGaugeDispSetPos(gaugeDisp, 142, 24);
        } else {
            barPic = RgBxxGetPic(pBxx, dispLifeBarTop2PictureName);
            restPic = RgBxxGetPic(pBxx, dispLifeBarTopDeadPictureName);
            RgGaugeDispSetTex(gaugeDisp, barPic, restPic);
            RgGaugeDispSetBarUVWH(gaugeDisp, 11, 15, 183, 12);
            RgGaugeDispSetRestUVWH(gaugeDisp, 0, 0, 0, 12);
            RgGaugeDispSetAlpha(gaugeDisp, 1);
            RgGaugeDispSetPos(gaugeDisp, 187, 47);
        }
    }
    pDisp->paint = CreateXrgPaint2D_sub(dispLifeSourcePath, 181);
    XrgPaint2DSetDrawPrio(pDisp->paint, 4);
}

static void _DestructDisp(RgDispLife *pDisp)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 188);
    }
    for (i = 0; i < RG_DISP_LIFE_PLAYERS; i++) {
        DisposeRgGauge(pDisp->gauge[i]);
    }
    DisposeXrgPaint2D_sub(pDisp->paint, dispLifeSourcePath, 192);
}

RgDispLife *CreateRgDispLife(RgBxx *dispTex, RgFont *timeFont)
{
    RgDispLife *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDispLife), dispLifeSourcePath, 0xC7);
    _InitDisp(pDisp, dispTex, timeFont);
    return pDisp;
}

void DisposeRgDispLife(RgDispLife *pDisp)
{
    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 0xCF);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, dispLifeSourcePath, 0xD1);
}

void RgDispLifeSetRobot(RgDispLife *pDisp, RgStatus *pRobot1, RgStatus *pRobot2)
{
    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 0xDB);
    }
    pDisp->robots[0] = pRobot1;
    pDisp->robots[1] = pRobot2;
}

void RgDispLifeSetTimer(RgDispLife *pDisp, float timer)
{
    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 0xE5);
    }
    if (timer >= 0.0f) {
        pDisp->timer = timer;
    }
}

static inline int _ClampWin(int count, int lo, int hi)
{
    if (count < lo) {
        return lo;
    }
    return count > hi ? hi : count;
}

void RgDispLifeSetWin(RgDispLife *pDisp, int win1P, int win2P, int winnerMask)
{
    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 0xEE);
    }
    pDisp->winCount[0] = _ClampWin(win1P, 0, RG_DISP_LIFE_MAX_WINS);
    pDisp->winCount[1] = _ClampWin(win2P, 0, RG_DISP_LIFE_MAX_WINS);
    pDisp->winnerMask = winnerMask;
}

void RgDispLifeSetVsMode(RgDispLife *pDisp, int vsMode)
{
    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 250);
    }

    pDisp->vsMode = vsMode;
    if (vsMode != 0) {
        pDisp->primaryBoardPic = RgBxxGetPic(pDisp->dispBxx, dispLifeVsPrimaryBoardPictureName);
        pDisp->secondaryBoardPic = RgBxxGetPic(pDisp->dispBxx, dispLifeVsSecondaryBoardPictureName);
    } else {
        pDisp->primaryBoardPic = RgBxxGetPic(pDisp->dispBxx, dispLifePrimaryBoardPictureName);
        pDisp->secondaryBoardPic = RgBxxGetPic(pDisp->dispBxx, dispLifeSecondaryBoardPictureName);
    }
}

void RgDispLifePassTime(RgDispLife *pDisp, float deltaTime)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 272);
    }
    for (i = 0; i < RG_DISP_LIFE_PLAYERS; i++) {
        if (pDisp->robots[i] != 0) {
            RgGaugeSetValue(pDisp->gauge[i], RgRobotGetLife(pDisp->robots[i]));
        }
        RgGaugePassTime(pDisp->gauge[i], deltaTime);
    }
    pDisp->elapsedTime += deltaTime;
    if (pDisp->elapsedTime > RG_DISP_LIFE_BLINK_PERIOD) {
        pDisp->elapsedTime = 0.0f;
    }
}

void RgDispLifeDisp(RgDispLife *pDisp)
{
    XrgPaint2D *paint;
    RgBxxPic **playerPics;
    RgBxxPic **pBoard;
    char timeText[32];
    unsigned int centis;
    unsigned int i;
    int clampBase;
    int countBase;

    if (pDisp == 0) {
        assert_prog(dispLifeAssertDispNotNil, dispLifeSourcePath, 293);
    }
    paint = pDisp->paint;
    if (pDisp->secondaryBoardPic != 0) {
        XrgPaint2DUseTexture(paint, pDisp->secondaryBoardPic);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, 0, 0x10, 0, 0);
    }
    if (pDisp->primaryBoardPic != 0) {
        XrgPaint2DUseTexture(paint, pDisp->primaryBoardPic);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, 0, 0, 0, 0);
    }

    centis = (unsigned int) floorf(pDisp->timer * 100.0f);
    timeText[0] = (char) ('0' + (centis / (RG_DISP_LIFE_CENTIS_PER_MINUTE * 10U)) % 10U);
    timeText[1] = (char) ('0' + (centis / RG_DISP_LIFE_CENTIS_PER_MINUTE) % 10U);
    timeText[2] = '\'';
    centis %= RG_DISP_LIFE_CENTIS_PER_MINUTE;
    timeText[3] = (char) ('0' + (centis / 1000U) % 10U);
    timeText[4] = (char) ('0' + (centis / 100U) % 10U);
    timeText[5] = '"';
    timeText[6] = (char) ('0' + (centis / 10U) % 10U);
    timeText[7] = (char) ('0' + centis % 10U);
    timeText[8] = '\0';
    RgFontStr(paint, pDisp->timeFont, timeText, 62, 61, 0);

    if (pDisp->agwsNamePic != 0) {
        for (i = 0; i < 2; i++) {
            RgStatus *robot = pDisp->robots[i];

            if (robot != 0) {
                int nameIndex = RgRobotGetSpec(robot)->charID;
                int nameX;
                int nameY;

                if (i == 0) {
                    nameX = 26;
                    nameY = 20;
                } else {
                    nameX = 406;
                    nameY = 46;
                }
                _DispAgwsName(pDisp->paint, pDisp->agwsNamePic, nameIndex, nameX, nameY);
            }
        }
    }

    i = 0;
    pBoard = &pDisp->secondaryBoardPic;
    for (; i < RG_DISP_LIFE_PLAYERS; i++) {
        RgGaugeDraw(pDisp->gauge[i], paint);
    }
    playerPics = pBoard + 12;

    clampBase = 0;
    countBase = 0;
    for (i = 0; i < 2; i++) {
        int winCount = pDisp->winCount[i];
        int lastWin = winCount - 1;
        int clamped;
        int winBit;
        RgBxxPic *countPic;
        RgBxxPic *clampPic;

        countPic = pDisp->lifePic[countBase + winCount];
        countBase += RG_DISP_LIFE_WIN_PICS_PER_PLAYER;
        clamped = _ClampWin(lastWin, 0, RG_DISP_LIFE_MAX_WINS);
        clampPic = pDisp->lifePic[clampBase + clamped];
        clampBase += RG_DISP_LIFE_WIN_PICS_PER_PLAYER;

        XrgPaint2DUseTexture(paint, playerPics[i]);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, s_anPos_1[i][0], s_anPos_1[i][1], 0, 0);

        winBit = 1 << i;
        if ((pDisp->winnerMask & winBit) && pDisp->elapsedTime > RG_DISP_LIFE_BLINK_HALF) {
            XrgPaint2DUseTexture(paint, clampPic);
        } else {
            XrgPaint2DUseTexture(paint, countPic);
        }
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, s_anPos_1[i][0], s_anPos_1[i][1], 0, 0);
    }
    XrgPaint2DFlush(paint);
}



static int s_anPos_1[2][2] = { { 0, 64 }, { 464, 64 } };

const char dispLifeAssertBxxNotNil[16] = "pBxx != NIL";

const char dispLifeAssertTimeFontNotNil[24] = "pTimeFont != NIL";

const char dispLifeAgwsNamePictureName[16] = "agwsname.bmp";

const char dispLifeLeftWindow0PictureName[16] = "l_a_0win.bmp";

const char dispLifeLeftWindow1PictureName[16] = "l_a_1win.bmp";

const char dispLifeLeftWindow2PictureName[16] = "l_a_2win.bmp";

const char dispLifeRightWindow0PictureName[16] = "r_a_0win.bmp";

const char dispLifeRightWindow1PictureName[16] = "r_a_1win.bmp";

const char dispLifeRightWindow2PictureName[16] = "r_a_2win.bmp";

const char dispLifeLeftBarPictureName[16] = "left_b.bmp";

const char dispLifeRightBarPictureName[16] = "right_b.bmp";

const char dispLifeBarTop1PictureName[16] = "bar_top1.bmp";

const char dispLifeBarTopDeadPictureName[16] = "bar_topd.bmp";

const char dispLifeBarTop2PictureName[16] = "bar_top2.bmp";
