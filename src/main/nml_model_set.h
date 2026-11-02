/*
 * TU-local declarations of main/tu106 (src/main/nml_model_set.c).
 */

#ifndef SRC_MAIN_NML_MODEL_SET_H
#define SRC_MAIN_NML_MODEL_SET_H

#include "shared.h"
#include "main/nml_model_set.h"
#include "main/xgl_2.h"

/*
 * Per-frame capacities.  s_apBlock / s_aBlockLayout hold the opaque blocks
 * from the bottom and the alpha blocks from the top (NML_BLOCK_POOL_LAST
 * downwards); both group lists keep their last slot free.
 */
#define NML_MODEL_MAX 512
#define NML_GROUP_MAX 512
#define NML_GROUP_LIMIT (NML_GROUP_MAX - 1)
#define NML_BLOCK_POOL_SIZE 4096
#define NML_BLOCK_POOL_LAST (NML_BLOCK_POOL_SIZE - 1)
#define NML_PARENT_BUF_MAX 512
#define NML_DIRECT_MAX 5500
#define NML_SHAPE_MAX 32
#define NML_TOUMEI_MAX 16
#define NML_POINT_LIGHT_MAX 3U
#define NML_WINDOW_COUNT 4
#define NML_CIRCLE_SHADOW_SIZE 64  /* side of the circle shadow texture */
#define NML_FADE_LEVEL_MAX 128     /* fade level 0..128 */
#define NML_MOVIE_FADE_CANCEL_FRAMES 30
#define NML_BACK_BUFFER_TIME 0x28  /* frames of the battle / event-skip back buffer */

#define NML_BLOCK_HIDDEN 0x8u

/*
 * One render group: a run of s_apBlock entries drawn together.  The alpha
 * groups (s_aAlphaGroup) and the non-alpha groups (s_aNonAlphaGroup) share
 * this record and its render-kind flags (1 texture, 0x80 drop, 0x100 circle,
 * 0x200 drop circle), so the render helpers take either.  Only alpha groups
 * are depth sorted.  The block range is reached through a union (alias set
 * 0): in nmlModelEntryEffect the original keeps the blockCount store ahead
 * of the following s_apBlock store, which a plain short member does not
 * order.
 */
typedef struct NmlBlockGroup {
    union {
        struct {
            short firstBlock;
            short blockCount;
        };
    };
    float sortDistance; /* +0x04; alpha groups only */
    u32 flags;
    union {
        union LayoutStore *layout; /* layout record this group was built from */
        int address;     /* read back as a word by the circle renderers */
    } owner;                       /* +0x0c */
} NmlBlockGroup;

/*
 * Partial view of one fade controller (the four global instances are
 * s_inFadeIn/s_inFadeOut/s_inActiveFadeIn/s_inActiveFadeOut, all still
 * INCLUDE_ASM in this TU). Field names below are witnessed by callers/
 * readers outside this file's own two functions -- fade_set (0x0022eca8),
 * fade_render (0x0022e988), nmlModelFadeDoit (0x0022eda0) and the
 * nmlModelSetFade{In,Out}{Dispose,Cancel} / nmlModelSetFadeOutLock{,Off}
 * families -- which this file cites only as evidence, not as claimed
 * source:
 *   - +0x00..+0x08: RGB color scaled by 255 (fade_set, nmlModelSetFadeInInterrupt)
 *   - +0x0c: computed fade level, frame/duration * 128 (fade_render)
 *   - +0x10 "frame": countdown frame counter; -1 while inactive
 *   - +0x14 "duration": fade length in frames; also read as a float divisor
 *   - +0x18 "skipRender": fade_render skips building its DMA packet when set
 *   - +0x24 "dispose": set to 1 by nmlModelSetFadeOutDispose/FadeInDispose
 *     (fadeOutDisposeFlag/fadeInDisposeFlag, a plain `sw 1`); fade_render tests it, and when
 *     set resets frame to 0 instead of running the countdown, i.e. a dispose
 *     request rather than an "active" flag.
 *   - +0x28 "cancelFrames": written by nmlModelSetFadeOutCancel/FadeInCancel
 *     and nmlModelSetActiveFadeOutCancel/FadeInCancel (fadeOutCancelFrames/fadeInCancelFrames,
 *     the argument clamped to >= 0 with slti/movn); nmlModelFadeDoit
 *     decrements it once per tick while nonzero, and fade_render skips its
 *     DMA packet build while it is nonzero, i.e. a cancel countdown rather
 *     than a "lockFrames" count.
 *   - +0x2c "startDelay": fade_set's last argument; fade_render tests it
 *     first and, while nonzero, only decrements it and returns, i.e. a
 *     start-delay frame count rather than a "repeatCount".
 *   - +0x30 "locked": set to 1 by nmlModelSetFadeOutLock (fadeOutLockedFlag, a
 *     plain `sw 1`) and cleared by nmlModelSetFadeOutLockOff (`sw 0`);
 *     fade_render reads it once frame has counted down to 0 and, while set,
 *     holds frame at 0 instead of letting skipRender clear and the fade
 *     finish, i.e. a lock flag rather than "done".
 * CONSTRUCT_FADE_CONTROL/INIT_FADE_CONTROL never touch +0x1c or +0x20 (only
 * fade_set, outside this file, writes them), so those two stay an unclaimed
 * byte gap rather than a guessed int member.
 */
typedef struct {
    int color[3];
    int level;
    int frame;
    int duration;
    int skipRender;
    int unclassified_1c;
    int unclassified_20;
    int dispose;
    int cancelFrames;
    int startDelay;
    int locked;
} FadeControl;

typedef unsigned int NmlPointQuadword __attribute__((mode(TI)));

/* Parent records reference render layouts; +8/+0xc are the two entry counters. */
typedef struct NmlParentBuffer {
    LayoutStore *layout;
    unsigned char unmodeled_04[4];
    int nonAlphaCount;
    int alphaCount;
} NmlParentBuffer;

typedef NmlParentBuffer NmlParentBuf;

extern Vector4 s_inMainCameraPos;
extern Vector4 s_inMainCameraAng;
extern int s_nModel;
extern int s_nBlocks;
extern int s_nDirect;
extern int s_nBlocksAlpha;
extern int s_nBlocksAlphaLast;
extern int s_nAnotherStudio;
extern int s_nUseStealth;
extern int s_nUseGnosys;
extern int s_nMapAlphaEntry;
extern int s_nMainCameraWarp;
extern NmlBlockGroup s_aAlphaGroup[NML_GROUP_MAX];
extern float s_fSortOffset;

/*
 * Bounded evidenced partial view of the 800-byte .data object at 0x004a91e0.
 * The extent (200 four-byte slots = 800 bytes) matches the original symbol;
 * no field names, struct layout or padding members are claimed. Each slot
 * is explicitly a float-or-int untagged word: the body witnesses float
 * members (shadow inputs at 0x40/0x60, plane value at 0xb4, matrix rows at
 * 0x180/0x190/0x194/0x198/0x1b0/0x1b4/0x1b8, height value at 0x2ac) and one
 * integer flag (height enable at 0x2a8). The flag is read through the .i
 * member, a documented GNU C union access, never a pun through a float
 * array. Uses beyond this body's witnessed offsets (for example the
 * pointer-width slot evidenced only by other callers) are unknown here and
 * receive no member. All accesses below name explicit original byte offsets.
 */
/* canon: config/header-canon.json chose src/math/main/spark-cont01-00231d50/nmlModelCalcDropShadow.c over 0 other accepted spellings */
extern LayoutStore s_inLayout;
extern Vector4 s_inShadowVec;
extern Vector4 s_inScale;
extern Vector4 s_inGblPointC[];
extern NmlPointQuadword s_inGblPointP[];
extern NmlPointQuadword s_inGblFogCol[];
extern Vector4 s_inGblFogPara;
extern float s_aTexOffset[2];

/* The four fade-control instances nmlModelSendSignalMovieStart initializes
 * through INIT_FADE_CONTROL; see the FadeControl comment above. */
extern FadeControl s_inFadeIn;
extern FadeControl s_inFadeOut;
extern FadeControl s_inActiveFadeIn;
extern FadeControl s_inActiveFadeOut;

/* TU-local scalar state referenced by the recovered model-system setters. */
extern int s_nEffectWrite;

/*
 * Render-group counters CONSTRUCT_ALPHA_GROUP/FLUSH_ALPHA_GROUP reset to
 * zero; s_aAlphaGroup/s_aNonAlphaGroup (main 0x00957940/0x00959940) are the
 * arrays they index into elsewhere in this TU.
 */
extern int s_nAlphaGroup;
extern int s_nNonAlphaGroup;

/*
 * These two status words have no published C owner in this TU. They are
 * declared as unsized arrays, not scalars, on purpose: under this TU's -G8
 * contract, GCC 2.96 only routes an extern *scalar* through gp-relative
 * (small-data) addressing when it can see the referenced object is within
 * the -G threshold; an incomplete array type has no known size, so the
 * compiler cannot classify it as small and always emits the original's
 * standard lui %hi / lw|sw %lo pair instead. Both readers below (
 * nmlModelIsBackBufferRequest, nmlModelSendPacketChangeSignal for
 * nmlModelBackBufferRequestFlags; nmlModelSetMpeg2CrossFadeTime for nmlModelMpeg2CrossFadeTime) are witnessed
 * doing exactly that in the original object -- never the gp-relative
 * single-symbol form the small-data statics of this TU use (compare
 * s_nPacketSignal/s_nFadeDoit) -- so declaring a plain `extern int` here
 * would let the compiler choose the wrong addressing and miss the gate.
 */
extern int nmlModelBackBufferRequestFlags[];
extern int nmlModelMpeg2CrossFadeTime[];

/*
 * The 40-byte back-buffer request record (main 0x0095bb20). CONSTRUCT_BACK_
 * BUFFER/INIT_BACK_BUFFER/FLUSH_BACK_BUFFER address every word of it through
 * one base register (lui %hi(s_inBackBuffer)/addiu %lo), including the two
 * words nmlModelBackBufferRequestFlags (+0x1c) and nmlModelMpeg2CrossFadeTime (+0x24) already declared above
 * under those names for nmlModelIsBackBufferRequest/
 * nmlModelSendPacketChangeSignal/nmlModelSetMpeg2CrossFadeTime; this array
 * is a second declaration of the same asm-owned bytes for the three
 * functions that reach them from the s_inBackBuffer symbol instead, for the
 * same reason nmlModelBackBufferRequestFlags/nmlModelMpeg2CrossFadeTime stay unsized arrays: an incomplete type
 * keeps GCC from routing it through gp-relative small-data addressing,
 * which the original object never uses for these words.
 */
extern int s_inBackBuffer[];

extern NmlParentBuffer s_aParentBuf[];

/*
 * s_inLayout is the 800-byte layout store at main:0x004A91E0, modelled as 200
 * four-byte slots.  LayoutSlot is four bytes wide, so slots[0x23c / 4] is byte
 * offset 0x23c exactly: the pointer-width slot this function writes.
 */
void nmlModelSetMatrix(void *matrix);

void nmlPacketAddGsScissor1(u64 scissor);
void nmlPacketAddScreenClear(void);
void nmlPacketClrTextureCache(void);
int nmlPacketDirectData(const void *data, int size);

#endif /* SRC_MAIN_NML_MODEL_SET_H */
