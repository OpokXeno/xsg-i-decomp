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

extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

void DisposeRgAnnounce(RgAnnounce *pAnn)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 118);
    }
    _DestructAnn(pAnn);
    RgHeapFree(InstanceOfRgHeap(), pAnn, D_00A57BC0, 120);
}

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

/*
 * The two fixed title rectangles, asm-owned scaffold data (splat names, no
 * config/symbols/ov12.txt entry): ov12:0x00a586b0 belongs to the time title
 * and ov12:0x00a586c0 to the damage title.
 */
extern const RgAnnounceUvRectHalves D_00A586B0;
extern const RgAnnounceUvRectHalves D_00A586C0;

static void _paint_uvwh(RgAnnounce *pAnn, int index, int x, int y,
                        const RgAnnounceUvRect *rect);

static void _time_disp_title(RgAnnounce *pAnn, int x, int y)
{
    RgAnnounceUvRectHalves rect;

    rect.uvSize = D_00A586B0.uvSize;
    rect.uvOffset = D_00A586B0.uvOffset;
    XrgPaint2DAlpha(pAnn->paint, XRG_PAINT2D_BLEND_ADD);
    _paint_uvwh(pAnn, 0x17, x, y, (const RgAnnounceUvRect *) &rect);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _time_disp);

static void _damage_disp_title(RgAnnounce *pAnn, int x, int y)
{
    RgAnnounceUvRectHalves rect;

    rect.uvSize = D_00A586C0.uvSize;
    rect.uvOffset = D_00A586C0.uvOffset;
    XrgPaint2DAlpha(pAnn->paint, XRG_PAINT2D_BLEND_ADD);
    _paint_uvwh(pAnn, 0x17, x, y, (const RgAnnounceUvRect *) &rect);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_announce", _damage_disp);

static void _paint_add(RgAnnounce *pAnn, int index, int x, int y);
static void _paint_sub(RgAnnounce *pAnn, int index, int x, int y);

static void _round1(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2B, 0, pAnn->dispY);
    _paint_add(pAnn, 0, 0, pAnn->dispY);
}

static void _round2(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2C, 0, pAnn->dispY);
    _paint_add(pAnn, 1, 0, pAnn->dispY);
}

static void _round3(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2D, 0, pAnn->dispY);
    _paint_add(pAnn, 2, 0, pAnn->dispY);
}

static void _ready(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2A, 0, pAnn->dispY);
    _paint_add(pAnn, 3, 0, pAnn->dispY);
}

static void _fight(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x27, 0, pAnn->dispY);
    _paint_add(pAnn, 4, 0, pAnn->dispY);
}

static void _youwin(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x1B, 0, pAnn->dispY);
    _paint_add(pAnn, 5, 0, pAnn->dispY);
}

static void _youlose(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x30, 0, pAnn->dispY);
    _paint_add(pAnn, 6, 0, pAnn->dispY);
}

static void _draw(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x26, 0, pAnn->dispY);
    _paint_add(pAnn, 7, 0, pAnn->dispY);
}

static void _gameover(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x29, 0, pAnn->dispY);
    _paint_add(pAnn, 9, 0, pAnn->dispY);
}

static void _timeover(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2F, 0, pAnn->dispY);
    _paint_add(pAnn, 8, 0, pAnn->dispY);
}

static void _stageclear(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x2E, 0, pAnn->dispY);
    _paint_add(pAnn, 0xA, 0, pAnn->dispY);
}

static void _gameclear(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x28, 0, pAnn->dispY);
    _paint_add(pAnn, 0xB, 0, pAnn->dispY);
}

static void _you_win_you_lose(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x1B, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 5, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_sub(pAnn, 0x30, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 6, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
}

static void _you_lose_you_win(RgAnnounce *pAnn) {
    _paint_sub(pAnn, 0x1B, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 5, RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_sub(pAnn, 0x30, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
    _paint_add(pAnn, 6, -RG_ANNOUNCE_COMBO_X_OFFSET, pAnn->dispY);
}

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

extern RgAnnounceDrawFunc s_apFuncs_0[RG_ANNOUNCE_DRAW_KIND_COUNT];

void RgAnnounceDispInit(RgAnnounce *pAnn, int kind)
{
    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 466);
    }
    if ((unsigned int) kind < RG_ANNOUNCE_DRAW_KIND_COUNT) {
        pAnn->timer = RG_ANNOUNCE_TIMER_INFINITE;
        pAnn->drawFunc = s_apFuncs_0[kind];
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

/* "uStage < sizeof(pAnn->m_auTotalTime) / sizeof(pAnn->m_auTotalTime[0])" */
extern const char D_00A586D8[];

void RgAnnounceSetGameClear(RgAnnounce *pAnn, const unsigned int *auTime,
                            const float *afDamage, unsigned int uStage)
{
    unsigned int i;

    if (pAnn == 0) {
        assert_prog(D_00A57BB0, D_00A57BC0, 521);
    }
    if (uStage >= RG_ANNOUNCE_GAMECLEAR_MAX) {
        assert_prog(D_00A586D8, D_00A57BC0, 522);
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
