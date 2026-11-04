/*
 * TU-local declarations of ov12/tu077 (src/ov12/rg_announce.c).
 */

#ifndef SRC_OV12_RG_ANNOUNCE_H
#define SRC_OV12_RG_ANNOUNCE_H

#include "ov12/rg_announce.h"
#include "ov12/xrg_paint2d.h"

typedef void (*RgAnnounceDrawFunc)(RgAnnounce *pAnn);

/*
 * The value _InitAnn and RgAnnounceDispInit (for an out-of-range kind) leave
 * on the timer: RgAnnouncePassTime compares against it to know the countdown
 * has not started and skips it.
 */
#define RG_ANNOUNCE_TIMER_INFINITE 1.0e8f

/*
 * _paint_lin/_paint_add/_paint_sub each hand XrgPaint2DAlpha one of exactly
 * these three values (0x00a44c10, 0x00a44c40, 0x00a44c70): 0 pastes the
 * source over the destination, 1 adds it, 2 subtracts it.
 */
#define XRG_PAINT2D_BLEND_LINEAR 0
#define XRG_PAINT2D_BLEND_ADD 1
#define XRG_PAINT2D_BLEND_SUB 2

/*
 * XrgPaint2DDrawXYWH's mode word: bit 0x20 takes the drawn extent from the
 * bound picture instead of the width/height arguments (_paint, 0x00a44b04).
 */
#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20

/*
 * Per-glyph texture ids for the announcement font, indexed by
 * _paint_lin/_paint_add/_paint_sub/_paint_uvwh. _InitAnn fills exactly these
 * 50 slots, 0x0C through 0xD0, with one RgBxxGetPic lookup each.
 */
#define RG_ANNOUNCE_TEXTURE_COUNT 50

/*
 * The size RgHeapAlloc is called with (CreateRgAnnounce, ov12:0x00a43f54).
 * The struct below claims only the members this TU's own functions read or
 * write, through gameClearActive at +0xEC; nothing in this allocation's
 * evidence resolves the remaining +0xF0..+0x173.
 */
#define RG_ANNOUNCE_SIZE 0x174

/*
 * RgAnnounceDispInit resets pAnn->dispY from pendingDispY to this constant
 * each time a new kind is dispatched; _InitAnn stores the same 0x99 in both
 * words when it first builds the announce (see the dispY/pendingDispY
 * comment on struct RgAnnounce below).
 */
#define RG_ANNOUNCE_DISP_Y_DEFAULT 0x99

/*
 * s_apFuncs_0 (ov12:0x00a50860, size 0x38) holds one RgAnnounceDrawFunc per
 * kind; RgAnnounceDispInit's own bound check (kind < 0xE) and the 14
 * _round1.._you_lose_you_win callbacks this TU defines both size it at
 * 0x38 / sizeof(RgAnnounceDrawFunc) = 14 entries.
 */
#define RG_ANNOUNCE_DRAW_KIND_COUNT 14

/*
 * RgAnnounceSetGameClear's own assert bounds uStage against
 * sizeof(pAnn->m_auTotalTime) / sizeof(pAnn->m_auTotalTime[0]) (the
 * announceAssertStageInTotalTimeRange string below); m_auTotalTime and m_afTotalDamage each span
 * 0x40 bytes, 16 elements of 4 bytes.
 */
#define RG_ANNOUNCE_GAMECLEAR_MAX 16

/*
 * Texture slots of RgAnnounce.textures used by name: the ADD layer of kind K
 * is slot K (kinds 0..11 in order, except TIME_OVER/GAME_OVER swapped), the
 * SUB layer follows the table below; the three clear-screen slots are the
 * time/damage title bar and their unit labels. The slot roles come from the
 * draw callbacks; the artwork names are not recovered.
 */
#define RG_ANNOUNCE_TEX_ROUND1_ADD 0
#define RG_ANNOUNCE_TEX_ROUND2_ADD 1
#define RG_ANNOUNCE_TEX_ROUND3_ADD 2
#define RG_ANNOUNCE_TEX_READY_ADD 3
#define RG_ANNOUNCE_TEX_FIGHT_ADD 4
#define RG_ANNOUNCE_TEX_YOU_WIN_ADD 5
#define RG_ANNOUNCE_TEX_YOU_LOSE_ADD 6
#define RG_ANNOUNCE_TEX_DRAW_ADD 7
#define RG_ANNOUNCE_TEX_TIME_OVER_ADD 8
#define RG_ANNOUNCE_TEX_GAME_OVER_ADD 9
#define RG_ANNOUNCE_TEX_STAGE_CLEAR_ADD 10
#define RG_ANNOUNCE_TEX_GAME_CLEAR_ADD 11
#define RG_ANNOUNCE_TEX_TITLE_BAR 0x17
#define RG_ANNOUNCE_TEX_TIME_LABEL 0x18
#define RG_ANNOUNCE_TEX_DAMAGE_LABEL 0x19
#define RG_ANNOUNCE_TEX_YOU_WIN_SUB 0x1B
#define RG_ANNOUNCE_TEX_DRAW_SUB 0x26
#define RG_ANNOUNCE_TEX_FIGHT_SUB 0x27
#define RG_ANNOUNCE_TEX_GAME_CLEAR_SUB 0x28
#define RG_ANNOUNCE_TEX_GAME_OVER_SUB 0x29
#define RG_ANNOUNCE_TEX_READY_SUB 0x2A
#define RG_ANNOUNCE_TEX_ROUND1_SUB 0x2B
#define RG_ANNOUNCE_TEX_ROUND2_SUB 0x2C
#define RG_ANNOUNCE_TEX_ROUND3_SUB 0x2D
#define RG_ANNOUNCE_TEX_STAGE_CLEAR_SUB 0x2E
#define RG_ANNOUNCE_TEX_TIME_OVER_SUB 0x2F
#define RG_ANNOUNCE_TEX_YOU_LOSE_SUB 0x30

/* The stage/game clear overlays fade a dark panel in to this alpha, FADE_STEP per frame. */
#define RG_ANNOUNCE_CLEAR_PANEL_ALPHA 80
#define RG_ANNOUNCE_CLEAR_FADE_STEP 8
#define RG_ANNOUNCE_SCREEN_WIDTH 512

/*
 * _you_win_you_lose/_you_lose_you_win draw their paired banners this many
 * pixels left and right of the shared x origin.
 */
#define RG_ANNOUNCE_COMBO_X_OFFSET 0x80

/*
 * The rectangle _time_disp_title/_damage_disp_title build on their stack and
 * hand to _paint_uvwh: u, v feed XrgPaint2DSetUVOffset and w, h feed both
 * XrgPaint2DSetUVSize and the draw call's own width/height.
 */
typedef struct RgAnnounceUvRect {
    int u;
    int v;
    int w;
    int h;
} RgAnnounceUvRect;

/*
 * The observed access view. _InitAnn stores 0x99 in both words at 0xD4/0xD8;
 * RgAnnounceDispInit moves the word at 0xD8 into 0xD4 and resets 0xD8 to
 * 0x99. Their role is not established, so they stay unmodeled.
 * stageClearActive, stageClearTime, stageClearDamage and stageClearTick belong
 * to the secondary "stage clear" overlay RgAnnounceDisp draws while
 * stageClearActive is set: it feeds stageClearTime to _time_disp and
 * stageClearDamage to _damage_disp. gameClearActive is
 * RgAnnounceSetGameClear's equivalent flag; RgAnnounceSetStageClear clears it
 * so the two overlays stay mutually exclusive.
 */
/*
 * This allocation names the two words the comment above left unmodeled:
 * dispY (0xD4) is the y argument every _round1.._you_lose_you_win draw
 * callback hands to _paint_sub/_paint_add; RgAnnounceDispInit promotes
 * pendingDispY (0xD8) into dispY each time a new kind starts, then resets
 * pendingDispY to RG_ANNOUNCE_DISP_Y_DEFAULT. Nothing in this TU's evidence
 * names whatever else may set pendingDispY beforehand.
 */
struct RgAnnounce {
    float timer;                             /* 0x00 */
    XrgPaint2D *paint;                       /* 0x04 */
    RgAnnounceDrawFunc drawFunc;              /* 0x08 */
    struct RgBxxPic *textures[RG_ANNOUNCE_TEXTURE_COUNT];  /* 0x0C */
    int dispY;                                /* 0xD4 */
    int pendingDispY;                         /* 0xD8 */
    int stageClearActive;                     /* 0xDC */
    int stageClearTime;                       /* 0xE0 */
    float stageClearDamage;                   /* 0xE4 */
    int stageClearTick;                       /* 0xE8 */
    int gameClearActive;                      /* 0xEC */
    /*
     * RgAnnounceSetGameClear's assert names m_auTotalTime directly
     * ("uStage < sizeof(pAnn->m_auTotalTime) / sizeof(pAnn->m_auTotalTime[0])",
     * announceAssertStageInTotalTimeRange). m_afTotalDamage is the adjacent float array the same loop
     * and index fill from its afDamage argument (only its position and
     * per-index pairing with m_auTotalTime are evidenced, not its original
     * name). m_uStage is the count RgAnnounceSetGameClear stores once the
     * copy finishes.
     */
    unsigned int m_auTotalTime[RG_ANNOUNCE_GAMECLEAR_MAX]; /* 0xF0 */
    float m_afTotalDamage[RG_ANNOUNCE_GAMECLEAR_MAX];      /* 0x130 */
    unsigned int m_uStage;                    /* 0x170 */
};

#endif /* SRC_OV12_RG_ANNOUNCE_H */
