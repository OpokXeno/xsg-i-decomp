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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", _InitDisp_00A25E58);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", RgDispLifeSetWin);

extern void *RgBxxGetPic(struct RgBxx *bxx, const char *picture_name);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", RgDispLifeDisp);
