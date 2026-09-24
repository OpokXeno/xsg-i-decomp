/*
 * TU-local declarations of ov12/tu077 (src/ov12/rg_announce.c).
 */

#ifndef SRC_OV12_RG_ANNOUNCE_H
#define SRC_OV12_RG_ANNOUNCE_H

/*
 * The 2D paint context (defined by ov12/tu086 xrg_paint2d); this TU only
 * stores the pointer and hands it to the XrgPaint2D* calls.
 */
typedef struct XrgPaint2D XrgPaint2D;

typedef struct RgAnnounce RgAnnounce;

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
 * RgAnnounceSetGameClear (this allocation) resolves the remaining
 * +0xF0..+0x173 the comment above still describes as unclaimed: m_auTotalTime
 * at +0xF0, m_afTotalDamage at +0x130 and m_uStage at +0x170, ending exactly
 * at +0x174.
 */

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
 * D_00A586D8 string below); m_auTotalTime and m_afTotalDamage each span
 * 0x40 bytes, 16 elements of 4 bytes.
 */
#define RG_ANNOUNCE_GAMECLEAR_MAX 16

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
    int textures[RG_ANNOUNCE_TEXTURE_COUNT];  /* 0x0C */
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
     * D_00A586D8). m_afTotalDamage is the adjacent float array the same loop
     * and index fill from its afDamage argument (only its position and
     * per-index pairing with m_auTotalTime are evidenced, not its original
     * name). m_uStage is the count RgAnnounceSetGameClear stores once the
     * copy finishes.
     */
    unsigned int m_auTotalTime[RG_ANNOUNCE_GAMECLEAR_MAX]; /* 0xF0 */
    float m_afTotalDamage[RG_ANNOUNCE_GAMECLEAR_MAX];      /* 0x130 */
    unsigned int m_uStage;                    /* 0x170 */
};

extern void RgAnnounceDispInit(RgAnnounce *pAnn, int kind);

RgAnnounce *CreateRgAnnounce(void);
void RgAnnouncePassTime(RgAnnounce *pAnn, float deltaTime);
void RgAnnounceDispInitTime(RgAnnounce *pAnn, int kind, float duration);
void RgAnnounceDispOff(RgAnnounce *pAnn);
void RgAnnounceSetStageClear(RgAnnounce *pAnn, int time, float damage);
void DisposeRgAnnounce(RgAnnounce *pAnn);
void RgAnnounceSetGameClear(RgAnnounce *pAnn, const unsigned int *auTime,
                            const float *afDamage, unsigned int uStage);

#endif /* SRC_OV12_RG_ANNOUNCE_H */
