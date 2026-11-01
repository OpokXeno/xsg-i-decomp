/*
 * TU-local declarations of main/tu106 (src/main/nml_model_set.c).
 */

#ifndef SRC_MAIN_NML_MODEL_SET_H
#define SRC_MAIN_NML_MODEL_SET_H

#include "shared.h"

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

/* The same 16 bytes as two doublewords, for the plain ld/sd copies. */
typedef struct NmlQword {
    unsigned long long lo;
    unsigned long long hi;
} NmlQword;

/* A vector on a doubleword boundary: the original copies it with ld/sd. */
typedef union NmlVector4Overlay {
    Vector4 v;
    NmlQword q;
} NmlVector4Overlay;

typedef struct NmlAlphaGroup {
    short firstBlock;
    short blockCount;
    float sortDistance;
    u32 status;
    union {
        union LayoutStore *layout; /* layout record this group was built from */
        int address;
    } owner;                       /* +0x0c */
} NmlAlphaGroup;

extern NmlAlphaGroup s_aAlphaGroup[512];

extern float s_fSortOffset;

typedef union {
    float f;
    int i;
} LayoutSlot;

typedef struct NmlPartPixelAlpha {
    short alpha;
    short partId;
} NmlPartPixelAlpha;

typedef union LayoutStore LayoutStore;

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

/*
 * s_inLayout is the 800-byte layout store at main:0x004A91E0, modelled as 200
 * four-byte slots.  LayoutSlot is four bytes wide, so slots[0x23c / 4] is byte
 * offset 0x23c exactly: the pointer-width slot this function writes.
 */
void nmlModelSetMatrix(void *matrix);

/* Marks model fades for processing during the next render pass. */
void nmlModelSetFadeDoit(void);

/* canon: config/header-canon.json chose src/math/main/spark-cont01-00231d50/nmlModelCalcDropShadow.c over 0 other accepted spellings */
extern LayoutStore s_inLayout;

extern Vector4 s_inShadowVec;

extern Vector4 s_inScale;

extern Vector4 s_inGblPointC[];
void xglVectorScaleXYZ(Vector4 *destination, const Vector4 *source, float scale);
typedef unsigned int NmlPointQuadword __attribute__((mode(TI)));
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
 * Allocation counter CONSTRUCT_PARENT_BUF/FLUSH_PARENT_BUF reset to zero:
 * an index into the s_aParentBuf entries (main 0x0095bb50), also still
 * INCLUDE_ASM in this TU.
 */

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

/*
 * One named mesh block in a loaded model.  The name matcher below witnesses
 * the block name at +0x00, its visibility flags at +0x20, and the first
 * block's material flags at +0xc0.
 */
typedef struct NmlModelBlock {
    char name[0x20];
    u32 flags;
    int baseOffset;
    int textOffset;
    int partId; /* +0x2c */
    unsigned short matrixIndices[48]; /* +0x30; transform then weighted entries */
    float clipBounds[4]; /* +0x90; element 3 is the weighted clip radius */
    unsigned char unmodeled_0a0[0x20];
    u32 materialFlags;
    unsigned char unmodeled_0c4[0x3c];
    Vector4 multiplyColor; /* +0x100 */
    float translationX, translationY;
    float translationStepX, translationStepY;
    unsigned char unmodeled_120[0x4c];
    signed char shapeMathParameter;
    signed char shapeMode, shapeWeight;
    unsigned char shapeFlags;
    char materialName[32]; /* +0x170 */
} NmlModelBlock;

#define NML_BLOCK_HIDDEN 0x8u
#define NML_BLOCK_MATERIAL_NAMED_PARTS 0x10u

/*
 * A loaded model resource.  Its block count is +0x44 and the table at +0xb0
 * holds byte offsets from this record to individual NmlModelBlock records.
 */
typedef struct NmlModel {
    unsigned char unmodeled_000[0x3f];
    signed char unclassified_3f;
    int partCount; /* +0x40 */
    int blockCount;
    int sampleCount; /* +0x48 */
    unsigned char unmodeled_04c[4];
    int weightedMatrixOffset; /* +0x50: byte offset from the model to its weighted matrices */
    unsigned char unmodeled_054[0x18];
    int unclassified_6c;
    unsigned char unmodeled_070[0x20];
    Vector4 entryTranslation; /* +0x90; w at +0x9c */
    int unclassified_a0;
    unsigned char unmodeled_0a4[0x0c];
    int blockOffsets[1];
} NmlModel;

/* Parent records reference render layouts; +8/+0xc are the two entry counters. */
typedef struct NmlParentBuffer {
    LayoutStore *layout;
    unsigned char unmodeled_04[4];
    int nonAlphaCount;
    int alphaCount;
} NmlParentBuffer;
typedef NmlParentBuffer NmlParentBuf;
extern NmlParentBuffer s_aParentBuf[];

/* Partial prefix of packets emitted by nmlFilterStealthMake. */
typedef struct NmlStealthPacket {
    unsigned char unmodeled_00[0x0c];
    float distanceSquared;
    unsigned char unmodeled_10[0x10];
    float unclassified_20;
} NmlStealthPacket;

/*
 * Partial view of the render volume a layout points at through +0x260: a
 * plane normal pair at +0x160/+0x168, an origin vector at +0x190, a min and a
 * max bounds vector at +0x1a0/+0x1b0, and the u64 alpha word at +0x1c8.
 * Only these witnessed offsets are named.
 */
typedef struct NmlRenderVolume {
    unsigned char unmodeled_000[0x160];
    float planeNormalX; /* +0x160 */
    unsigned char unmodeled_164[4];
    float planeNormalZ; /* +0x168 */
    unsigned char unmodeled_16c[0x190 - 0x16c];
    Vector4 origin; /* +0x190 */
    Vector4 boundsMin; /* +0x1a0 */
    Vector4 boundsMax; /* +0x1b0 */
    unsigned char unmodeled_1c0[0x1c8 - 0x1c0];
    u64 alpha; /* +0x1c8 */
} NmlRenderVolume;

union LayoutStore {
    LayoutSlot slots[200];
    unsigned long long qwords[100];
    struct {
        Matrix4 lightDirection; /* +0x0 */
        Matrix4 lightColor; /* +0x40 */
        Matrix4 place; /* +0x80 */
        Matrix4 inversePlace; /* +0xc0 */
        Vector4 pointPosition[4]; /* +0x100 */
        Vector4 pointColor[4]; /* +0x140 */
        Matrix4 projectionMatrix; /* +0x180; saved by the circle and map projection paths */
        Vector4 fogColor; /* +0x1c0 */
        Vector4 fogPara; /* +0x1d0 */
        unsigned char unmodeled_1e0[0x1f0 - 0x1e0];
        NmlVector4Overlay unmodeled_1f0; /* +0x1f0; copied as a projection input and passed to AlphaGroupSortEntry */
        Vector4 multiplyColor; /* +0x200 */
        unsigned char unmodeled_210[0x234 - 0x210];
        unsigned int packetFilterFlags;
        int pixelAlpha;                     /* +0x238, full-width pixel-alpha word */
        unsigned char unmodeled_23c[4];
        const char *texture;                /* +0x240 */
        int texfunc;                        /* +0x244 */
        u64 alpha;                          /* +0x248 */
        u32 render_status;                  /* +0x250 */
        int parent; /* +0x254 */
        Matrix4 *weightedMatrices; /* +0x258 */
        unsigned char unmodeled_25c[4];
        NmlRenderVolume *volume; /* +0x260 */
        NmlRenderVolume *renderVolume; /* +0x264; set while the layout renders */
        int texMapMode; /* +0x268; supplied by map and actor shadow setup */
        float textureTransparency;
        int texMapSize; /* +0x270 */
        float texMapWeight; /* +0x274 */
        int mapShadowPartCount; /* +0x278 */
        int renderLevel; /* +0x27c */
        NmlStealthPacket *stealthPackets[10]; /* +0x280 */
        unsigned char unmodeled_2a8[0x2b0 - 0x2a8];
        unsigned int featureFlags;
        int partPixelAlphaCount; /* +0x2b4 */
        unsigned char *clipMasks; /* +0x2b8 */
        unsigned char unmodeled_2bc[4];
        float circleShadowScale; /* +0x2c0 */
        unsigned char unmodeled_2c4[4];
        int stealthEntryCount; /* +0x2c8 */
        unsigned char unmodeled_2cc[4];
        NmlPartPixelAlpha partPixelAlpha[4]; /* +0x2d0 */
        unsigned short mapShadowParts[8];    /* +0x2e0 */
        unsigned char unmodeled_2f0[0x300 - 0x2f0];
        float viewportAlpha[4];
        int viewportFlags[4];
    } fields;
};

void nmlPacketAddGsScissor1(u64 scissor);
void nmlPacketAddScreenClear(void);
void nmlPacketClrTextureCache(void);
int nmlPacketDirectData(const void *data, int size);

#endif /* SRC_MAIN_NML_MODEL_SET_H */
