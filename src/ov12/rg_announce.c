/*
 * OV12 original TU 77: 0x00a434b8..0x00a44f08 (35 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_announce.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a57bc0 contains the source filename "../rg_announce.euc.c".
 * ov12:0x00a57bb0 contains the assertion expression "pAnn != NIL".
 */
extern const char D_00A57BB0[];
extern const char D_00A57BC0[];

extern void XrgPaint2DUseTexture(XrgPaint2D *paint, int texture);
extern void XrgPaint2DAlpha(XrgPaint2D *paint, int blend);
extern void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y,
                               int width, int height);
extern void XrgPaint2DSetUVOffset(XrgPaint2D *paint, int u, int v);
extern void XrgPaint2DSetUVSize(XrgPaint2D *paint, int width, int height);
extern void XrgPaint2DFlush(XrgPaint2D *paint);
extern void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *source_file,
                                  int line);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern RgHeap *InstanceOfRgHeap(void);

static void _InitAnn(RgAnnounce *pAnn);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _InitAnn);

static void _DestructAnn(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 102);
    }
    DisposeXrgPaint2D_sub(pAnn->paint, D_00A57BC0, 103);
}

RgAnnounce *CreateRgAnnounce(void)
{
    RgAnnounce *pAnn;

    pAnn = RgHeapAlloc(InstanceOfRgHeap(), RG_ANNOUNCE_SIZE, D_00A57BC0, 110);
    _InitAnn(pAnn);
    return pAnn;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", DisposeRgAnnounce);

void RgAnnouncePassTime(RgAnnounce *pAnn, float deltaTime)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 129);
    }
    if (pAnn->timer != RG_ANNOUNCE_TIMER_INFINITE) {
        pAnn->timer -= deltaTime;
        if (pAnn->timer < 0.0f) {
            pAnn->timer = 0.0f;
            pAnn->drawFunc = 0;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", RgAnnounceDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _time_disp_title);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _time_disp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _damage_disp_title);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _damage_disp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _round1);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _round2);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _round3);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _ready);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _fight);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _youwin);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _youlose);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _draw);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _gameover);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _timeover);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _stageclear);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _gameclear);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _you_win_you_lose);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _you_lose_you_win);

static void _paint(XrgPaint2D *paint, int texture, int blend, int x, int y)
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", RgAnnounceDispInit);

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
        assert_prog(D_00A57BB0, D_00A57BC0, 493);
    }
    pAnn->drawFunc = 0;
    pAnn->stageClearActive = 0;
}

void RgAnnounceSetStageClear(RgAnnounce *pAnn, int time, float damage)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 505);
    }
    pAnn->stageClearTime = time;
    pAnn->stageClearDamage = damage;
    pAnn->gameClearActive = 0;
    pAnn->stageClearActive = 1;
    pAnn->stageClearTick = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", RgAnnounceSetGameClear);
