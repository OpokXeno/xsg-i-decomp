#include "common.h"
#include "shared.h"
#include "nml_model_set.h"

extern int s_nClip;

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _VectorLengthSQ_0022DE28);

/*
 * Loads the current model matrix's four rows into the resident VU0
 * macro-mode registers vf27-vf30 (ee-vu-cop2), for the _CurRotTransPersClip/
 * _CurRotTransPersFog/_CurApplyMatrix family that reads them back.
 */
static void _CurSetMatrix(float matrix[4][4])
{
    __asm__ __volatile__(
        "lqc2 $vf27,0(%0)\n\t"
        "lqc2 $vf28,16(%0)\n\t"
        "lqc2 $vf29,32(%0)\n\t"
        "lqc2 $vf30,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

/*
 * Loads the current view-scale and view-translation vectors into the
 * resident VU0 macro-mode registers vf25 and vf26 (ee-vu-cop2), for the
 * same _CurRotTransPersClip/_CurRotTransPersFog/_CurApplyMatrix family.
 */
static void _CurSetViewScaleTrans(const float viewScale[4], const float viewTrans[4])
{
    __asm__ __volatile__(
        "lqc2 $vf25,0(%0)\n\t"
        "lqc2 $vf26,0(%1)\n\t"
        "nop"
        :
        : "r"(viewScale), "r"(viewTrans)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _CurRotTransPersClip_0022DE80);

/*
 * Transforms one vector by the resident VU0 matrix (_CurSetMatrix above),
 * perspective-divides it and folds the result into a fog coordinate clamped
 * to the supplied [min, max] bounds, the same resident-register family as
 * _CurApplyMatrix below.
 */
static void _CurRotTransPersFog(Vector4 *destination, const Vector4 *vector, const Vector4 *fog) {
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf4, 0(%2)\n\t"
        "vmulax.xyzw ACC, vf27, vf31x\n\t"
        "vmadday.xyzw ACC, vf28, vf31y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf31z\n\t"
        "vmaddw.xyzw vf31, vf30, vf0w\n\t"
        "vdiv Q, vf0w, vf31w\n\t"
        "vmulz.w vf1, vf0, vf4z\n\t"
        "vwaitq\n\t"
        "vmula.w ACC, vf1, vf0\n\t"
        "vmaddq.w vf31, vf4, Q\n\t"
        "vmaxx.w vf31, vf31, vf4x\n\t"
        "vminiy.w vf31, vf31, vf4y\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        "nop"
        :
        : "r"(destination), "r"(vector), "r"(fog)
        : "memory"
    );
}

/*
 * Multiplies one vector by the resident VU0 matrix (_CurSetMatrix above)
 * and stores the transformed result, the same row-by-row accumulation as
 * src/main/face_point.c's _ApplyMatrix.
 */
static void _CurApplyMatrix(Vector4 *destination, const Vector4 *vector) {
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "vmulax.xyzw ACC, vf27, vf31x\n\t"
        "vmadday.xyzw ACC, vf28, vf31y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf31z\n\t"
        "vmaddw.xyzw vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%1)\n\t"
        "nop"
        :
        : "r"(vector), "r"(destination)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _WeightToGlobalVec);

/* Stores componentwise minima in the first vector and maxima in the second. */
static void _MinMaxSort(Vector4 *min, Vector4 *max) {
    __asm__ __volatile__(
        "lqc2 vf20, 0(%0)\n\t"
        "lqc2 vf21, 0(%1)\n\t"
        "vmini.xyz vf22, vf20, vf21\n\t"
        "vmax.xyz vf23, vf20, vf21\n\t"
        "sqc2 vf22, 0(%0)\n\t"
        "sqc2 vf23, 0(%1)\n\t"
        "nop"
        :
        : "r"(min), "r"(max)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CLEAR_LAYOUT_MODEL);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_CIRCLR_SHADOW);

static void CONSTRUCT_ALPHA_GROUP(void)
{
    s_nAlphaGroup = 0;
    s_nNonAlphaGroup = 0;
}

static void INIT_ALPHA_GROUP(void)
{
    CONSTRUCT_ALPHA_GROUP();
}

static void FLUSH_ALPHA_GROUP(void)
{
    s_nAlphaGroup = 0;
    s_nNonAlphaGroup = 0;
}

/*
 * The only word this TU's CLEAR_PROREAL evidences within its caller's
 * "proreal" render block (nmlModelClear passes &s_inProReal, main
 * 0x0095b940, further modeled in part by src/main/nml_packet_add.h's
 * NmlProRealParam at its own +0x11c field): nothing else in this
 * allocation reads or writes byte offset 0x1c0, so it stays a named
 * array index rather than a guessed struct member.
 */
static void CLEAR_PROREAL(int *proReal)
{
    proReal[0x1c0 / 4] = 0;
}

/*
 * s_inBackBuffer (main 0x0095bb20, see the header comment) is reset one
 * word at a time below; only +0x10 and +0x1c/+0x24 have evidenced roles
 * within this allocation:
 *   - +0x10 "owner index": set to -1 (no owner) by CONSTRUCT/INIT.
 *   - +0x1c: D_0095BB3C, the flag nmlModelIsBackBufferRequest reads and
 *     nmlModelSendPacketChangeSignal clears (both above, this TU).
 *   - +0x24: D_0095BB44, the value nmlModelSetMpeg2CrossFadeTime writes;
 *     INIT_BACK_BUFFER is the only one of these three that leaves it
 *     untouched, preserving the configured cross-fade time.
 */
static void CONSTRUCT_BACK_BUFFER(void)
{
    s_inBackBuffer[0x10 / 4] = -1;
    s_inBackBuffer[0x14 / 4] = 0;
    s_inBackBuffer[0x00 / 4] = 0;
    s_inBackBuffer[0x04 / 4] = 0;
    s_inBackBuffer[0x08 / 4] = 0;
    s_inBackBuffer[0x0c / 4] = 0;
    s_inBackBuffer[0x18 / 4] = 0;
    s_inBackBuffer[0x1c / 4] = 0;
    s_inBackBuffer[0x20 / 4] = 0;
    s_inBackBuffer[0x24 / 4] = 0;
}

static void INIT_BACK_BUFFER(void)
{
    s_inBackBuffer[0x10 / 4] = -1;
    s_inBackBuffer[0x14 / 4] = 0;
    s_inBackBuffer[0x00 / 4] = 0;
    s_inBackBuffer[0x04 / 4] = 0;
    s_inBackBuffer[0x08 / 4] = 0;
    s_inBackBuffer[0x0c / 4] = 0;
    s_inBackBuffer[0x18 / 4] = 0;
    s_inBackBuffer[0x1c / 4] = 0;
    s_inBackBuffer[0x20 / 4] = 0;
}

/*
 * The active-request word FLUSH_BACK_BUFFER's caller (nmlModelFlushClear)
 * clears once per frame; CONSTRUCT/INIT_BACK_BUFFER reset it the same way
 * among the other words above.
 */
static void FLUSH_BACK_BUFFER(void)
{
    s_inBackBuffer[0x00 / 4] = 0;
}

static void CONSTRUCT_PARENT_BUF(void)
{
    s_nParentBuf = 0;
}

static void INIT_PARENT_BUF(void)
{
    CONSTRUCT_PARENT_BUF();
}

static void FLUSH_PARENT_BUF(void)
{
    s_nParentBuf = 0;
}

/*
 * mapHandle[0] and mapHandle[1] are the two words this TU's
 * CONSTRUCT_MAP_HANDLE/CLEAR_MAP_HANDLE/FLUSH_MAP_HANDLE evidence within
 * the caller's map-handle record (nmlModelConstruct passes &s_inMapHandle,
 * main 0x0095db50): CLEAR_MAP_HANDLE resets only mapHandle[0] and
 * FLUSH_MAP_HANDLE resets only mapHandle[1], so the two words are kept
 * separate rather than folded into one guessed struct member.
 */
static void CONSTRUCT_MAP_HANDLE(int *mapHandle)
{
    /*
     * The original writes word 0 and puts the word-1 store in the return's
     * delay slot; the two stores are independent, and in declaration order
     * cc1 schedules them the other way round (form 01, build/form-01,
     * first difference at 0x6e8: sw zero,4(a0) for sw zero,0(a0)).
     */
    mapHandle[1] = 0;
    mapHandle[0] = 0;
}

static void INIT_MAP_HANDLE(int *mapHandle)
{
    CONSTRUCT_MAP_HANDLE(mapHandle);
}

static void CLEAR_MAP_HANDLE(int *mapHandle)
{
    mapHandle[0] = 0;
}

static void FLUSH_MAP_HANDLE(int *mapHandle)
{
    mapHandle[1] = 0;
}

static void CONSTRUCT_FADE_CONTROL(FadeControl *control)
{
    control->frame = -1;
    control->duration = 1;
    control->skipRender = 0;
    control->dispose = 0;
    control->cancelFrames = 0;
    control->startDelay = 0;
    control->locked = 0;
}

static void INIT_FADE_CONTROL(FadeControl *control)
{
    CONSTRUCT_FADE_CONTROL(control);
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CLEAR_MODEL_ENTRY);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_MODELSYSTEM);

/* CONSTRUCT_MODELSYSTEM (above) is still INCLUDE_ASM; declare it so its
 * caller does not see an implicit declaration. */
static void CONSTRUCT_MODELSYSTEM(void);

static void INIT_MODELSYSTEM(void)
{
    CONSTRUCT_MODELSYSTEM();
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", FLUSH_MODELSYSTEM);

void nmlModelSetBackBufferClear(void)
{
    INIT_BACK_BUFFER();
}

void nmlModelSetFaceModel(int enabled)
{
    if (enabled != 0) {
        s_inLayout.slots[0x250 / 4].i |= 0x100000;
    }
}

void nmlModelSetHumanModel(int enabled)
{
    if (enabled != 0) {
        s_inLayout.slots[0x250 / 4].i |= 0x40;
    }
}

void nmlModelSetMapClip(int enabled)
{
    if (enabled != 0) {
        s_nMapClip = 1;
    }
}

void nmlModelSendRenderCancel(void)
{
}

int nmlModelGetRenderCancel(void)
{
    return s_nRenderCancelOld;
}

void nmlModelSendBackBufferSignal(void)
{
    s_nUseBackBuffer = 1;
}

int nmlModelIsBackBufferRequest(void)
{
    return D_0095BB3C[0];
}

void nmlModelSendPauseSignal(int pause)
{
    s_nPause = pause;
}

void nmlModelSendMenuStart(void)
{
    s_nMenu = 1;
}

void nmlModelSendMenuEnd(void)
{
    s_nMenu = 0;
}

int nmlModelGetMenuStatus(void)
{
    return s_nMenu;
}

void nmlModelSendSignalStartBattle(void)
{
    s_nFrameLockOff = 1;
}

void nmlModelSendSignalEventFinish(void)
{
    s_nFrameLockOff = 1;
}

void nmlModelSendPacketChangeSignal(void)
{
    s_nPacketSignal = 0;
    D_0095BB3C[0] = 0;
}

/* INIT_BACK_BUFFER (above) is still INCLUDE_ASM; declare it so its caller
 * does not see an implicit declaration. */
static void INIT_BACK_BUFFER(void);

void nmlModelSendSignalMovieStart(void)
{
    INIT_BACK_BUFFER();
    INIT_FADE_CONTROL(&s_inFadeIn);
    INIT_FADE_CONTROL(&s_inFadeOut);
    INIT_FADE_CONTROL(&s_inActiveFadeIn);
    INIT_FADE_CONTROL(&s_inActiveFadeOut);
}

void nmlModelSetMpeg2CrossFadeTime(int time)
{
    D_0095BB44[0] = time;
}

/*
 * TU-local partial view of the shared render-state global sRender (main
 * 0x004A90E0, size 0x5C; see src/main/game_over.c's DrawImageRenderState,
 * src/main/map_1.c's MapRenderState, src/main/window_tex_load.c's
 * UmnRenderSize and src/main/game_defocus.c's DefocusRenderState for other
 * TUs' views of the same object). Only the halfword at +0x22 is evidenced
 * here: nmlModelSendSignalMovieFinish below forwards it unchanged as the
 * "ownerIndex" argument of nmlModelSetBackBuffer and
 * nmlModelSetBackBufferToBattle.
 */
typedef struct {
    unsigned char unmodeled_00[0x22];
    unsigned short ownerIndex;  /* +0x22 */
} NmlBackBufferOwnerView;

extern NmlBackBufferOwnerView sRender;

/*
 * nmlModelSetFadeInCancel/nmlModelSetFadeOutCancel/nmlModelSetBackBuffer
 * (below, this TU) and nmlModelSetBackBufferToBattle (still INCLUDE_ASM)
 * are used before their definitions; declare them so these calls do not
 * see an implicit declaration.
 */
void nmlModelSetFadeInCancel(int frames);
void nmlModelSetFadeOutCancel(int frames);
void nmlModelSetBackBuffer(int modelId, int duration, int ownerIndex, int value);
void nmlModelSetBackBufferToBattle(unsigned short ownerIndex);

/*
 * The caller (GameMpeg2Play, main 0x00250c18) never inspects the result:
 * the very next instruction after the call is an unrelated jal, so this is
 * void. The case labels are declared in the order the original jump
 * table's targets are laid out in .text (0, 1, 4, 2, 3), not in numeric
 * order, matching the compiled block layout.
 */
void nmlModelSendSignalMovieFinish(int mode)
{
    switch (mode - 1) {
    case 0:
        nmlModelSetBackBufferToBattle(sRender.ownerIndex);
        nmlModelSetFadeInCancel(30);
        nmlModelSetFadeOutCancel(30);
        break;
    case 1:
        nmlModelSetBackBuffer(0x26, 1, sRender.ownerIndex, 0);
        nmlModelSetFadeInCancel(30);
        break;
    case 4:
        nmlModelSetBackBuffer(0x26, 1, sRender.ownerIndex, 0);
        nmlModelSetFadeOutCancel(30);
        nmlModelSetFadeInCancel(30);
        break;
    case 2:
        nmlModelSetBackBuffer(3, 1, sRender.ownerIndex, 0);
        nmlModelSetFadeInCancel(30);
        break;
    case 3:
        /*
         * Unlike the other cases, this one's call is a genuine jal that
         * falls into the shared epilogue below rather than a tail jump
         * straight into the callee; the single-iteration loop reproduces
         * that call shape.
         */
        nmlModelSetBackBuffer(D_0095BB44[0], 1, sRender.ownerIndex, 1);
        do {
            nmlModelSetFadeInCancel(30);
        } while (0);
        break;
    }
}

/*
 * s_inLayout.slots[0xac] (byte offset 0x2b0) is a bitmask word this
 * function only ever ORs new bits into, the same raw-slot idiom
 * nmlModelSetFaceModel/nmlModelSetHumanModel use for the render-status word
 * at +0x250 above.
 */
int nmlModelSetRenderLevel(int level)
{
    int renderLevel = s_inLayout.slots[0x2b0 / 4].i | level;
    s_inLayout.slots[0x2b0 / 4].i = renderLevel;
    return renderLevel;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", fade_render);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlFadePacketWrite);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", fade_set);

/*
 * D_0095DB90 is main:0x0095DB90, the same byte the FadeControl comment above
 * ties to s_inFadeOut's "locked" field at +0x30 (0x0095DB60 + 0x30): cited
 * there only as evidence, not as claimed source. Addressing it through
 * &s_inFadeOut+0x30 rather than its own symbol changes the emitted %lo
 * immediate (sw v0,48(v1) for the original's sw v0,0(v1)), so this
 * function keeps the flat scaffold symbol, as an unsized array so -G8
 * small-data gp-relative addressing does not apply to it either (matching
 * D_0095BB3C/D_0095BB44 above).
 */
extern int D_0095DB90[];

void nmlModelSetFadeOutLock(void) {
    D_0095DB90[0] = 1;
}

void nmlModelSetFadeOutLockOff(void) {
    D_0095DB90[0] = 0;
}

/* D_0095DC48 is s_inActiveFadeIn's "cancelFrames" byte (+0x28); see the
 * D_0095DB90 comment above for why it stays a flat scaffold symbol. */
extern int D_0095DC48[];

void nmlModelSetActiveFadeInCancel(int frames) {
    D_0095DC48[0] = (frames < 0) ? 0 : frames;
}

/* D_0095DBC8 is s_inFadeIn's "cancelFrames" byte (+0x28). */
extern int D_0095DBC8[];

void nmlModelSetFadeInCancel(int frames) {
    D_0095DBC8[0] = (frames < 0) ? 0 : frames;
}

/* D_0095DC08 is s_inActiveFadeOut's "cancelFrames" byte (+0x28). */
extern int D_0095DC08[];

void nmlModelSetActiveFadeOutCancel(int frames) {
    D_0095DC08[0] = (frames < 0) ? 0 : frames;
}

/* D_0095DB88 is s_inFadeOut's "cancelFrames" byte (+0x28). */
extern int D_0095DB88[];

void nmlModelSetFadeOutCancel(int frames) {
    D_0095DB88[0] = (frames < 0) ? 0 : frames;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFadeDoit);

void nmlModelSetFadeDoit(void)
{
    s_nFadeDoit = 1;
}

/* D_0095DB84 is s_inFadeOut's "dispose" byte (+0x24). */
extern int D_0095DB84[];

void nmlModelSetFadeOutDispose(void) {
    D_0095DB84[0] = 1;
}

/* D_0095DBC4 is s_inFadeIn's "dispose" byte (+0x24). */
extern int D_0095DBC4[];

void nmlModelSetFadeInDispose(void) {
    D_0095DBC4[0] = 1;
}

/*
 * TU-local partial view of GameLoopState (main 0x00338680, size 0x2a030;
 * TU-local by canon, config/header-canon.json). Only the two fields this
 * setter reads are modeled: the fade level at +0x58 and the three fade
 * color components at +0x90, both forwarded to fade_set unchanged.
 */
typedef struct {
    unsigned char unmodeled_00[0x58];
    int level;                 /* +0x58 */
    unsigned char unmodeled_5c[0x90 - 0x5c];
    float color[3];            /* +0x90 */
} NmlGameLoopState;

extern NmlGameLoopState GameLoopState;

/* fade_set (near the top of this TU) is still INCLUDE_ASM; declare it so
 * this caller does not see an implicit declaration. */
static void fade_set(FadeControl *, float *, int, int, int, int, int);

void nmlModelSetFadeOut(int duration, int mode) {
    fade_set(&s_inFadeOut, GameLoopState.color, GameLoopState.level, duration + 2, 0, mode, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeIn);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeInInterrupt);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetActiveFadeOut);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetActiveFadeIn);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetFadeLevel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelAskNonLinearCamera);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetGblPosition);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetBackBufferToBattle);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetBackBufferToEventSkip);

void nmlModelSetBackBuffer(int modelId, int duration, int ownerIndex, int value)
{
    s_inBackBuffer[8 / 4] = modelId;
    s_inBackBuffer[0x10 / 4] = ownerIndex;
    s_inBackBuffer[0x20 / 4] = value;
    s_inBackBuffer[0xC / 4] = duration;
    s_inBackBuffer[0x14 / 4] = 0;
    s_inBackBuffer[0x00 / 4] = 0;
    s_inBackBuffer[0x04 / 4] = 0;
    if (duration <= 0) {
        s_inBackBuffer[0xC / 4] = 1;
    }
}

void nmlModelSetEffectWrite(int enabled)
{
    s_nEffectWrite = enabled;
}

extern int g_aSubWindow[4];

void nmlModelUseSubWindow(unsigned int index, int enabled) {
    if (index < 4U) {
        g_aSubWindow[index] = enabled;
    }
}

void nmlModelSetWindow(u32 window) {
    if (window < 4U) {
        s_inLayout.slots[0x27c / 4].i = window;
    }
}

void nmlModelSetSortOffset(float offset)
{
    s_fSortOffsetEntry = offset;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightPos);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightReset);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPointLight);

void nmlModelSetSpecularOff(int disableSpecular) {
    if (disableSpecular != 0) {
        s_inLayout.slots[0x2b0 / 4].i |= 2;
    }
}

void nmlModelSetShadowMapId(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowHeight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowVec);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexMap);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetProreal);

int nmlModelSetTexProreal(void) {
    int renderStatus = s_inLayout.fields.render_status | 0x800;
    s_inLayout.fields.render_status = renderStatus;
    return renderStatus;
}

int nmlModelSetAxisSymmetry(float symmetry) {
    int flags;

    s_inLayout.slots[0x230 / 4].f = symmetry * 0.0f;
    flags = s_inLayout.slots[0x230 / 4].i | 1;
    s_inLayout.slots[0x230 / 4].i = flags;
    return flags;
}

void nmlModelSetParent(int parent)
{
    s_nParent = parent;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetRenderStatus);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShapeId);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetToumeiParts);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFilter);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPixelAlpha);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPartsPixelAlpha);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetScale);

void nmlModelSetClip(int clip)
{
    s_nClip = clip;
}

void nmlModelSetAlpha(u64 alpha)
{
    s_inLayout.fields.alpha = alpha;
}

#define NML_RENDER_STENCIL 0x4u
#define NML_RENDER_ZWRITE 0x8u
#define NML_RENDER_TOUMEI 0x20u

void nmlModelSetStencil(int enabled)
{
    if (enabled) {
        s_inLayout.fields.render_status |= NML_RENDER_STENCIL;
    }
}

void nmlModelSetZwrite(int enabled)
{
    if (enabled) {
        s_inLayout.fields.render_status |= NML_RENDER_ZWRITE;
    }
}

void nmlModelSetTransparency(float transparency)
{
    if (transparency < 0.0f) {
        transparency = 0.0f;
    }
    if (1.0f < transparency) {
        transparency = 1.0f;
    }
    s_inLayout.slots[0x220 / 4].f = transparency;
    s_inLayout.slots[0x2c0 / 4].f = transparency;
}

void nmlModelSetReflTransparency(float transparency)
{
    if (transparency < 0.0f) {
        transparency = 0.0f;
    }
    if (1.0f < transparency) {
        transparency = 1.0f;
    }
    s_inLayout.slots[0x224 / 4].f = transparency;
}

void nmlModelSetToumei(int enabled)
{
    if (enabled) {
        s_inLayout.fields.render_status |= NML_RENDER_TOUMEI;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetLight);

/*
 * Bounds and alignment a model texture pointer must satisfy: the pointer
 * lands inside main RAM at or above the executable's own load base
 * (config/tu/main/tu000.json "start": "0x00200000") and at or below the top
 * of the 32 MiB EE RAM, and it is qword (16-byte) aligned as GIF/DMA
 * texture transfers require.
 */
#define NML_TEXTURE_RAM_BASE 0x00200000u
#define NML_TEXTURE_RAM_MAX_OFFSET 0x01dfffffu
#define NML_TEXTURE_ALIGN_MASK 0xfu

void nmlModelSetTexture(const char *texture)
{
    /* Accept only a non-null, in-range, qword-aligned pointer whose header
     * carries the "XTX" texture-resource signature: texture[0..2] ==
     * 'X','T','X' (texture[2] is compared against texture[0], not a literal,
     * per the original disassembly). */
    if (texture == 0 ||
        (u32)texture - NML_TEXTURE_RAM_BASE > NML_TEXTURE_RAM_MAX_OFFSET ||
        ((u32)texture & NML_TEXTURE_ALIGN_MASK) != 0 ||
        texture[0] != 'X' || texture[1] != 'T' || texture[2] != texture[0]) {
        return;
    }
    s_inLayout.fields.texture = texture;
}

void nmlModelSetTexfunc(int texfunc)
{
    s_inLayout.fields.texfunc = texfunc;
}

/*
 * Stores the current model matrix pointer in the pointer-width s_inLayout
 * word at byte offset 0x23c (slot 143 of the shared LayoutStore view, which
 * gives that slot no pointer member).
 */
void nmlModelSetMatrix(void *matrix)
{
    *(void **)&s_inLayout.slots[0x23c / 4] = matrix;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPlace);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexOffset);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMatOffset);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMulColor);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFogCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFogPara);

/* nmlModelFogPara (above) is still INCLUDE_ASM; declare it so its caller
 * does not see an implicit declaration. */
static void nmlModelFogPara(LayoutSlot *fog, float fogNear, float fogFar, float fogMin, float fogMax);

#define NML_RENDER_FOG 0x2u

/* The four fog distances of RgFog.dist (defaults 50, 250, 0, 1). */
void nmlModelSetFogDist(float fogNear, float fogFar, float fogMin, float fogMax)
{
    nmlModelFogPara(&s_inLayout.slots[0x1d0 / 4], fogNear, fogFar, fogMin, fogMax);
    s_inLayout.fields.render_status |= NML_RENDER_FOG;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalFogCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalFogDist);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetFogPara);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetFogParaStudio);

int nmlModelSetGlobalFogReset(void) {
    int renderStatus = s_inLayout.fields.render_status & 0xfeffffff;
    s_inLayout.fields.render_status = renderStatus;
    return renderStatus;
}

int nmlModelSetFogCancel(void) {
    int renderStatus = s_inLayout.fields.render_status | 0x2000;
    s_inLayout.fields.render_status = renderStatus;
    return renderStatus;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelInitPartsVisible);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapEntry);

void nmlModelSetShadowMapEntry(void) {
    s_inLayout.fields.render_status |= 0x10000;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapShadowParts);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapLastEntry);

void nmlModelSetMapLastInit(void)
{
    s_nMapLast = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPartsVisible);

extern unsigned int strlen(const char *string);
extern int memcmp(const void *left, const void *right, unsigned int count);
extern int strcmp(const char *left, const char *right);
extern char *strstr(const char *string, const char *pattern);

/* nmlModelLexDataCheck remains assembly-owned below. */
int nmlModelLexDataCheck(NmlModel *model);

#define NML_NAME_MATCH_PREFIX 1
#define NML_NAME_MATCH_EXACT 2
#define NML_NAME_MATCH_SUBSTRING 3

void nmlModelSetNameVisible(NmlModel *model, const char *name, int visible, int match)
{
    int *block_offsets;
    NmlModelBlock *first_block;
    NmlModelBlock *block;
    u32 set_flags;
    u32 keep_flags;
    int block_index;

    if (nmlModelLexDataCheck(model) != 0) {
        return;
    }

    block_offsets = model->blockOffsets;
    first_block = (NmlModelBlock *)((char *)model + block_offsets[0]);
    if (!(first_block->materialFlags & NML_BLOCK_MATERIAL_NAMED_PARTS)) {
        return;
    }

    set_flags = NML_BLOCK_HIDDEN;
    keep_flags = ~0u;
    if (visible) {
        set_flags = 0;
        keep_flags = ~NML_BLOCK_HIDDEN;
    }

    switch (match) {
    case NML_NAME_MATCH_PREFIX:
        for (block_index = 0; block_index < model->blockCount; block_index++) {
            block = (NmlModelBlock *)((char *)model + block_offsets[block_index]);
            if (memcmp(block->name, name, strlen(name)) == 0) {
                block->flags = (block->flags | set_flags) & keep_flags;
            }
        }
        break;
    case NML_NAME_MATCH_EXACT:
        for (block_index = 0; block_index < model->blockCount; block_index++) {
            block = (NmlModelBlock *)((char *)model + block_offsets[block_index]);
            if (strcmp(block->name, name) == 0) {
                block->flags = (block->flags | set_flags) & keep_flags;
            }
        }
        break;
    case NML_NAME_MATCH_SUBSTRING:
        for (block_index = 0; block_index < model->blockCount; block_index++) {
            block = (NmlModelBlock *)((char *)model + block_offsets[block_index]);
            if (strstr(block->name, name) != 0) {
                block->flags = (block->flags | set_flags) & keep_flags;
            }
        }
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelDirectSend);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelDirectXtxSub);

/* nmlModelDirectXtxSub (above) is still INCLUDE_ASM; declare it so this
 * caller does not see an implicit declaration. */
void nmlModelDirectXtxSub(void *data, int modelId, int nextModelId);

/*
 * Callers (GameRadarDraw main 0x00252fc8, unit6003_draw main 0x003211a0;
 * both still INCLUDE_ASM) never inspect the result -- the tail call below
 * matches the compiled form, so this is void.
 */
void nmlModelDirectSendXtx(int modelId, void *data)
{
    const signed char *bytes = data;

    if (s_nPacketSignal == 0 &&
        (u32)((u32)data - NML_TEXTURE_RAM_BASE) <= NML_TEXTURE_RAM_MAX_OFFSET &&
        ((u32)data & 0xF) == 0 &&
        bytes[0] == 'X' && bytes[1] == 'T' && bytes[2] == bytes[0]) {
        nmlModelDirectXtxSub(data, modelId, modelId + 1);
    }
}

extern int s_nUseZwrite;

static void set_group_status(LayoutStore *layout, u32 *status)
{
    *status = 0;
    if (layout->fields.render_status & NML_RENDER_FOG) {
        *status = 0x4;
    }
    if (layout->fields.render_status & NML_RENDER_STENCIL) {
        *status |= 0x2;
    }
    if (layout->fields.render_status & 0x4000) {
        *status |= 0x1;
    }
    if (layout->fields.render_status & 0x800) {
        *status |= 0x8;
    }
    if (layout->fields.render_status & 0x10000) {
        *status |= 0x10;
    }
    if (layout->fields.render_status & NML_RENDER_TOUMEI) {
        *status |= 0x20;
    }
    if (layout->fields.render_status & 0x20000) {
        *status |= 0x40;
    }
    if (layout->fields.render_status & 0x40000) {
        *status |= 0x80;
    }
    if (layout->fields.render_status & 0x200000) {
        *status |= 0x100;
    }
    if (layout->fields.render_status & 0x400000) {
        *status |= 0x200;
    }
    if (layout->fields.render_status & 0x800000) {
        *status |= 0x800;
    }
    if (layout->fields.render_status & NML_RENDER_ZWRITE) {
        *status |= 0x400;
        s_nUseZwrite = 1;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", AlphaGroupSortEntry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", AlphaGroupLastEntry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", AlphaGroupSort);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", is_shadow_map_parts);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", is_parts_transparency);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", is_block_last_entry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", parent_buf_entry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", parent_buf_search);

/*
 * D_00956940 is the .bss circle-shadow ratio table: set_circle_shadow_ratio
 * below only ever writes indices 0-7 and 16-23 (two 8-entry blocks 0x40
 * bytes apart); indices 8-15 are written elsewhere (CONSTRUCT_CIRCLR_SHADOW/
 * nmlModelRenderCircle/nmlModelRenderDropCircle, all still INCLUDE_ASM in
 * this TU), so it stays an unsized array rather than a guessed full extent.
 */
extern int D_00956940[];

static void set_circle_shadow_ratio(int scale)
{
    float ratio = (float)scale * 0.0625f;
    int i;

    for (i = 0; i < 16; i++) {
        int index = (i < 8) ? i : i + 8;

        D_00956940[index] = (int)(128.0f - (float)i * ratio) << 24;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelLexDataCheck);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelLexBlockDoit);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelStealthEntry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelCalcSample);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelCalcEntryClip);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", deapth_for_studio);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelCalcSpecularClip);

#define LAYOUT_W(off) (s_inLayout.slots[(off) / 4].f)
#define LAYOUT_S32(off) (s_inLayout.slots[(off) / 4].i)

static void nmlModelCalcDropShadow(void)
{
    xglMatrixUnit((float (*)[4])&s_inLayout.slots[0x180 / 4].f);

    {
        Vector4 shadow;

        shadow.x = LAYOUT_W(0x40);
        shadow.y = 1.5f;
        shadow.z = LAYOUT_W(0x60);

        if (s_nShadowVec != 0) {
            shadow.x = s_inShadowVec.x;
            shadow.z = s_inShadowVec.z;
        }

        if (shadow.y != 0.0f) {
            LAYOUT_W(0x190) = -shadow.x / shadow.y;
            LAYOUT_W(0x198) = -shadow.z / shadow.y;
        } else {
            LAYOUT_W(0x190) = 0.0f;
            LAYOUT_W(0x198) = 0.0f;
        }

        LAYOUT_W(0x194) = 0.0f;
        LAYOUT_W(0x1b4) = LAYOUT_W(0xb4) + 0.005f;
        LAYOUT_W(0x1b0) = -LAYOUT_W(0xb4) * LAYOUT_W(0x190);
        LAYOUT_W(0x1b8) = -LAYOUT_W(0xb4) * LAYOUT_W(0x198);

        if (LAYOUT_S32(0x2a8) != 0) {
            LAYOUT_W(0x1b4) = LAYOUT_W(0x2ac) + 0.005f;
            LAYOUT_W(0x1b0) = -LAYOUT_W(0x2ac) * LAYOUT_W(0x190);
            LAYOUT_W(0x1b8) = -LAYOUT_W(0x2ac) * LAYOUT_W(0x198);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelCalcEntryProjectCircle);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelCalcEntryProjectMap);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelCalcEntryPartsClip);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelEntry);

ACCEPTED_ASM("src/main/nml_model_set", nmlModelEntryEffect);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelEntryCard);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelRenderProreal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelRenderTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelRenderCircle);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelRenderDropCircle);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelRenderDrop);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFlushSubNonAlpha);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFlushSubAlpha);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFlushSub);

/*
 * CONSTRUCT_CIRCLR_SHADOW (near the top of this TU) is still INCLUDE_ASM
 * and nmlModelClear is defined below (this TU); declare them, and the
 * s_inMapHandle array nmlModelClear also declares nearer its own use, so
 * this caller does not see an implicit declaration.
 */
static void CONSTRUCT_CIRCLR_SHADOW(void);
void nmlModelClear(void);
extern unsigned char s_inMapHandle[];

void nmlModelConstruct(void)
{
    CONSTRUCT_MODELSYSTEM();
    CONSTRUCT_CIRCLR_SHADOW();
    CONSTRUCT_BACK_BUFFER();
    CONSTRUCT_ALPHA_GROUP();
    CONSTRUCT_PARENT_BUF();
    CONSTRUCT_MAP_HANDLE((int *)s_inMapHandle);
    CONSTRUCT_FADE_CONTROL(&s_inFadeIn);
    CONSTRUCT_FADE_CONTROL(&s_inFadeOut);
    CONSTRUCT_FADE_CONTROL(&s_inActiveFadeIn);
    CONSTRUCT_FADE_CONTROL(&s_inActiveFadeOut);
    nmlModelClear();
    s_inLayout.fields.render_status = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelInit);

/* CLEAR_LAYOUT_MODEL and CLEAR_MODEL_ENTRY (near the top of this TU) are
 * still INCLUDE_ASM; declare them so this caller does not see an implicit
 * declaration. */
static void CLEAR_LAYOUT_MODEL(LayoutStore *);
static void CLEAR_MODEL_ENTRY(void);
extern unsigned char s_inMapHandle[];
extern unsigned char s_inProReal[];

void nmlModelClear(void) {
    CLEAR_LAYOUT_MODEL(&s_inLayout);
    CLEAR_PROREAL((int *) s_inProReal);
    CLEAR_MAP_HANDLE((int *) s_inMapHandle);
    CLEAR_MODEL_ENTRY();
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFlush);

/* FLUSH_MODELSYSTEM (above) is still INCLUDE_ASM; declare it so this
 * caller does not see an implicit declaration. */
static void FLUSH_MODELSYSTEM(void);

void nmlModelFlushClear(void) {
    FLUSH_MODELSYSTEM();
    FLUSH_ALPHA_GROUP();
    FLUSH_MAP_HANDLE((int *) s_inMapHandle);
    FLUSH_PARENT_BUF();
    FLUSH_BACK_BUFFER();
    nmlModelClear();
}
