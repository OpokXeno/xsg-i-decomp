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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_ALPHA_GROUP);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_ALPHA_GROUP);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", FLUSH_ALPHA_GROUP);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CLEAR_PROREAL);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_BACK_BUFFER);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_BACK_BUFFER);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", FLUSH_BACK_BUFFER);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_PARENT_BUF);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_PARENT_BUF);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", FLUSH_PARENT_BUF);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_MAP_HANDLE);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_MAP_HANDLE);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CLEAR_MAP_HANDLE);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", FLUSH_MAP_HANDLE);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_MODELSYSTEM);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", FLUSH_MODELSYSTEM);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetBackBufferClear);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendSignalMovieStart);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFadeDoit);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetEffectWrite);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelUseSubWindow);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetWindow);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetSortOffset);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightPos);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetGlobalPointLightReset);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetPointLight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetSpecularOff);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowMapId);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowHeight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetShadowVec);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexMap);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetProreal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexProreal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetAxisSymmetry);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetParent);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFogDist);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapLastInit);

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
