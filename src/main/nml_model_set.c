#include "common.h"
#include "shared.h"
#include "nml_model_set.h"

extern int s_nClip;

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _VectorLengthSQ_0022DE28);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _CurSetMatrix);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _CurSetViewScaleTrans_0022DE70);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _CurRotTransPersClip_0022DE80);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _CurRotTransPersFog);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _CurApplyMatrix);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _WeightToGlobalVec);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", _MinMaxSort);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_MAP_HANDLE);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendSignalMovieFinish);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetRenderLevel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", fade_render);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlFadePacketWrite);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", fade_set);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeOutLock);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeOutLockOff);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetActiveFadeInCancel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeInCancel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetActiveFadeOutCancel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeOutCancel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFadeDoit);

void nmlModelSetFadeDoit(void)
{
    s_nFadeDoit = 1;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeOutDispose);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeInDispose);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeOut);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeIn);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeInInterrupt);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetActiveFadeOut);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetActiveFadeIn);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetFadeLevel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelAskNonLinearCamera);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetGblPosition);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetBackBufferToBattle);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetBackBufferToEventSkip);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetBackBuffer);

void nmlModelSetEffectWrite(int enabled)
{
    s_nEffectWrite = enabled;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelUseSubWindow);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetWindow);

void nmlModelSetSortOffset(float offset)
{
    s_fSortOffsetEntry = offset;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightPos);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightReset);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPointLight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetSpecularOff);

void nmlModelSetShadowMapId(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowHeight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowVec);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexMap);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetProreal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexProreal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetAxisSymmetry);

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
static void nmlModelFogPara(LayoutSlot *fog);

#define NML_RENDER_FOG 0x2u

void nmlModelSetFogDist(void)
{
    nmlModelFogPara(&s_inLayout.slots[0x1d0 / 4]);
    s_inLayout.fields.render_status |= NML_RENDER_FOG;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalFogCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalFogDist);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetFogPara);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetFogParaStudio);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalFogReset);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFogCancel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelInitPartsVisible);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapEntry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowMapEntry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapShadowParts);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapLastEntry);

void nmlModelSetMapLastInit(void)
{
    s_nMapLast = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPartsVisible);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetNameVisible);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelDirectSend);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelDirectXtxSub);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelDirectSendXtx);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", set_group_status);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", AlphaGroupSortEntry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", AlphaGroupLastEntry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", AlphaGroupSort);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", is_shadow_map_parts);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", is_parts_transparency);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", is_block_last_entry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", parent_buf_entry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", parent_buf_search);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", set_circle_shadow_ratio);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelConstruct);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelInit);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelClear);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFlush);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelFlushClear);
