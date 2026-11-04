/*
 * OV12 original TU 43: 0x00a25d30..0x00a26a00 (12 functions)
 */
#include "common.h"
#include "rg_disp_life.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);
static void _InitDisp(RgDispLife *pDisp, int dispTex, int timeFont);
static void _DestructDisp(RgDispLife *pDisp);
/* The original TU-local tables and strings used by the C accessors below. */
struct AgwsNameUvwh;

typedef struct AgwsNameRecord {
    int characterId;
    unsigned char unmodeled_04[12];
    int texture_u;
    int texture_v;
    int texture_width;
    int texture_height;
} AgwsNameRecord;

static AgwsNameRecord s_aCharIDToUVWH_0[6] = {
    {0, {0}, 0, 0, 88, 16},
    {2, {0}, 168, 0, 80, 16},
    {4, {0}, 88, 0, 80, 16},
    {5, {0}, 0, 32, 88, 16},
    {3, {0}, 88, 32, 80, 16},
    {1, {0}, 168, 32, 80, 16}
};

const char D_00A54810[] = "pPic != NIL";

const char D_00A54820[] = "../rg_disp_life.euc.c";

const char D_00A54838[] = "pDisp != NIL";

const char D_00A54870[] = "board_na.bmp";

const char D_00A54880[] = "board_nb.bmp";

const char D_00A54950[] = "board_nc.bmp";

const char D_00A54960[] = "board_nd.bmp";

static int _GetAgwsNameUVWH(int nameIndex, struct AgwsNameUvwh *uvwh)
{
    unsigned int index;

    for (index = 0; index < 6; index++) {
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

/*
 * The AGWS name texture's UV rectangle _GetAgwsNameUVWH (still INCLUDE_ASM)
 * fills: u, v feed XrgPaint2DSetUVOffset and w, h feed both
 * XrgPaint2DSetUVSize and the draw call's own width/height.
 */
typedef struct AgwsNameUvwh {
    int u;
    int v;
    int w;
    int h;
} AgwsNameUvwh;

static int _GetAgwsNameUVWH(int nameIndex, AgwsNameUvwh *uvwh);
extern void XrgPaint2DUseTexture(void *paint, void *pic);
extern void XrgPaint2DSetUVOffset(void *paint, int u, int v);
extern void XrgPaint2DSetUVSize(void *paint, int width, int height);
extern void XrgPaint2DAlpha(void *paint, int blendMode);
extern void XrgPaint2DDrawXYWH(void *paint, int mode, int x, int y,
                               int width, int height);

static void _DispAgwsName(void *paint, void *pPic, int nameIndex, int x,
                          int y)
{
    AgwsNameUvwh uvwh;

    if (pPic == 0) {
        assert_prog(D_00A54810, D_00A54820, 100);
    }
    if (_GetAgwsNameUVWH(nameIndex, &uvwh) && (pPic != 0)) {
        XrgPaint2DUseTexture(paint, pPic);
        XrgPaint2DSetUVOffset(paint, uvwh.u, uvwh.v);
        XrgPaint2DSetUVSize(paint, uvwh.w, uvwh.h);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, 0, x, y, uvwh.w, uvwh.h);
    }
}

extern void *RgBxxGetPic(struct RgBxx *bxx, const char *picture_name);
extern RgGauge *CreateRgGauge(void);
extern void *RgGaugeGetDisp(RgGauge *gauge);
extern void RgGaugeDispSetTex(void *gaugeDisp, void *barPic, void *restPic);
extern void RgGaugeDispSetBarUVWH(void *gaugeDisp, int u, int v, int w,
                                  int h);
extern void RgGaugeDispSetRestUVWH(void *gaugeDisp, int u, int v, int w,
                                   int h);
extern void RgGaugeDispSetAlpha(void *gaugeDisp, int alpha);
extern void RgGaugeDispSetPos(void *gaugeDisp, int x, int y);
extern void *CreateXrgPaint2D_sub(const char *source_file, int line);
extern void XrgPaint2DSetDrawPrio(void *paint, int prio);
extern const char D_00A54848[];
extern const char D_00A54858[];
extern const char D_00A54890[];
extern const char D_00A548A0[];
extern const char D_00A548B0[];
extern const char D_00A548C0[];
extern const char D_00A548D0[];
extern const char D_00A548E0[];
extern const char D_00A548F0[];
extern const char D_00A54900[];
extern const char D_00A54910[];
extern const char D_00A54920[];
extern const char D_00A54930[];
extern const char D_00A54940[];

static void _InitDisp(RgDispLife *pDisp, int dispTex, int timeFont)
{
    struct RgBxx *pBxx = (struct RgBxx *)dispTex;
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 117);
    }
    if (pBxx == 0) {
        assert_prog(D_00A54848, D_00A54820, 118);
    }
    if (timeFont == 0) {
        assert_prog(D_00A54858, D_00A54820, 119);
    }
    pDisp->timeFont = timeFont;
    pDisp->timer = 0.0f;
    pDisp->vsMode = 0;
    pDisp->dispBxx = pBxx;
    pDisp->primaryBoardPic = RgBxxGetPic(pBxx, D_00A54870);
    pDisp->secondaryBoardPic = RgBxxGetPic(pBxx, D_00A54880);
    pDisp->agwsNamePic = RgBxxGetPic(pBxx, D_00A54890);
    pDisp->lifePic[0] = RgBxxGetPic(pBxx, D_00A548A0);
    pDisp->lifePic[1] = RgBxxGetPic(pBxx, D_00A548B0);
    pDisp->lifePic[2] = RgBxxGetPic(pBxx, D_00A548C0);
    pDisp->lifePic[3] = RgBxxGetPic(pBxx, D_00A548D0);
    pDisp->lifePic[4] = RgBxxGetPic(pBxx, D_00A548E0);
    pDisp->lifePic[5] = RgBxxGetPic(pBxx, D_00A548F0);
    pDisp->lifePic[6] = RgBxxGetPic(pBxx, D_00A54900);
    pDisp->lifePic[7] = RgBxxGetPic(pBxx, D_00A54910);
    pDisp->winnerMask = 0;
    pDisp->elapsedTime = 0.0f;
    for (i = 0; i < 2; ++i) {
        void *gaugeDisp;
        void *barPic;
        void *restPic;

        pDisp->robots[i] = 0;
        pDisp->gauge[i] = CreateRgGauge();
        pDisp->winCount[i] = 0;
        gaugeDisp = RgGaugeGetDisp(pDisp->gauge[i]);
        if (i == 0) {
            barPic = RgBxxGetPic(pBxx, D_00A54920);
            restPic = RgBxxGetPic(pBxx, D_00A54930);
            RgGaugeDispSetTex(gaugeDisp, barPic, restPic);
            RgGaugeDispSetBarUVWH(gaugeDisp, 14, 8, 183, 12);
            RgGaugeDispSetRestUVWH(gaugeDisp, 0, 0, 0, 12);
            RgGaugeDispSetAlpha(gaugeDisp, 1);
            RgGaugeDispSetPos(gaugeDisp, 142, 24);
        } else {
            barPic = RgBxxGetPic(pBxx, D_00A54940);
            restPic = RgBxxGetPic(pBxx, D_00A54930);
            RgGaugeDispSetTex(gaugeDisp, barPic, restPic);
            RgGaugeDispSetBarUVWH(gaugeDisp, 11, 15, 183, 12);
            RgGaugeDispSetRestUVWH(gaugeDisp, 0, 0, 0, 12);
            RgGaugeDispSetAlpha(gaugeDisp, 1);
            RgGaugeDispSetPos(gaugeDisp, 187, 47);
        }
    }
    pDisp->paint = CreateXrgPaint2D_sub(D_00A54820, 181);
    XrgPaint2DSetDrawPrio(pDisp->paint, 4);
}

extern void DisposeRgGauge(RgGauge *gauge);
extern void DisposeXrgPaint2D_sub(void *paint, const char *source_file,
                                  int line);

static void _DestructDisp(RgDispLife *pDisp)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 188);
    }
    for (i = 0; i < 2; i++) {
        DisposeRgGauge(pDisp->gauge[i]);
    }
    DisposeXrgPaint2D_sub(pDisp->paint, D_00A54820, 192);
}

RgDispLife *CreateRgDispLife(int dispTex, int timeFont)
{
    RgDispLife *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), 0x60, D_00A54820, 0xC7);
    _InitDisp(pDisp, dispTex, timeFont);
    return pDisp;
}

void DisposeRgDispLife(RgDispLife *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 0xCF);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, D_00A54820, 0xD1);
}

void RgDispLifeSetRobot(RgDispLife *pDisp, RgStatus *pRobot1, RgStatus *pRobot2)
{
    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 0xDB);
    }
    pDisp->robot1P = pRobot1;
    pDisp->robot2P = pRobot2;
}

void RgDispLifeSetTimer(RgDispLife *pDisp, float timer)
{
    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 0xE5);
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
        assert_prog(D_00A54838, D_00A54820, 0xEE);
    }
    pDisp->winCount[0] = _ClampWin(win1P, 0, 2);
    pDisp->winCount[1] = _ClampWin(win2P, 0, 2);
    pDisp->winnerMask = winnerMask;
}

extern const char D_00A54950[];
extern const char D_00A54960[];
extern const char D_00A54870[];
extern const char D_00A54880[];

void RgDispLifeSetVsMode(RgDispLife *pDisp, int vsMode)
{
    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 250);
    }

    pDisp->vsMode = vsMode;
    if (vsMode != 0) {
        pDisp->primaryBoardPic = RgBxxGetPic(pDisp->dispBxx, D_00A54950);
        pDisp->secondaryBoardPic = RgBxxGetPic(pDisp->dispBxx, D_00A54960);
    } else {
        pDisp->primaryBoardPic = RgBxxGetPic(pDisp->dispBxx, D_00A54870);
        pDisp->secondaryBoardPic = RgBxxGetPic(pDisp->dispBxx, D_00A54880);
    }
}

extern float RgRobotGetLife(RgStatus *robot);
extern void RgGaugeSetValue(RgGauge *gauge, float value);
extern void RgGaugePassTime(RgGauge *gauge, float deltaTime);

void RgDispLifePassTime(RgDispLife *pDisp, float deltaTime)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 272);
    }
    for (i = 0; i < 2; i++) {
        if (pDisp->robots[i] != 0) {
            RgGaugeSetValue(pDisp->gauge[i], RgRobotGetLife(pDisp->robots[i]));
        }
        RgGaugePassTime(pDisp->gauge[i], deltaTime);
    }
    pDisp->elapsedTime += deltaTime;
    if (pDisp->elapsedTime > 1.0f) {
        pDisp->elapsedTime = 0.0f;
    }
}

extern float floorf(float value);
extern int RgFontStr(void *paint, int fontId, const char *text, int x, int y,
                      int color);
extern void RgGaugeDraw(RgGauge *gauge, void *paint);
extern int *RgRobotGetSpec(RgStatus *pRobot);
extern void XrgPaint2DFlush(void *paint);
extern const int s_anPos_1[2][2];

void RgDispLifeDisp(RgDispLife *pDisp)
{
    void *paint;
    void **playerPics;
    void **pBoard;
    char timeText[32];
    unsigned int centis;
    unsigned int i;
    int clampBase;
    int countBase;

    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 293);
    }
    paint = pDisp->paint;
    if (pDisp->secondaryBoardPic != 0) {
        XrgPaint2DUseTexture(paint, pDisp->secondaryBoardPic);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, 0x20, 0, 0x10, 0, 0);
    }
    if (pDisp->primaryBoardPic != 0) {
        XrgPaint2DUseTexture(paint, pDisp->primaryBoardPic);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, 0x20, 0, 0, 0, 0);
    }

    centis = (unsigned int) floorf(pDisp->timer * 100.0f);
    timeText[0] = (char) ('0' + (centis / 60000U) % 10U);
    timeText[1] = (char) ('0' + (centis / 6000U) % 10U);
    timeText[2] = '\'';
    centis %= 6000U;
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
                int nameIndex = *RgRobotGetSpec(robot);
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
    pBoard = (void **) &pDisp->secondaryBoardPic;
    for (; i < 2; i++) {
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
        void *countPic;
        void *clampPic;

        countPic = pDisp->lifePic[countBase + winCount];
        countBase += 3;
        clamped = _ClampWin(lastWin, 0, 2);
        clampPic = pDisp->lifePic[clampBase + clamped];
        clampBase += 3;

        XrgPaint2DUseTexture(paint, playerPics[i]);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, 0x20, s_anPos_1[i][0], s_anPos_1[i][1], 0, 0);

        winBit = 1 << i;
        if ((pDisp->winnerMask & winBit) && pDisp->elapsedTime > 0.5f) {
            XrgPaint2DUseTexture(paint, clampPic);
        } else {
            XrgPaint2DUseTexture(paint, countPic);
        }
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, 0x20, s_anPos_1[i][0], s_anPos_1[i][1], 0, 0);
    }
    XrgPaint2DFlush(paint);
}
