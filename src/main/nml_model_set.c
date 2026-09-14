#include "common.h"
#include "shared.h"
#include "nml_model_set.h"

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_FADE_CONTROL);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_FADE_CONTROL);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CLEAR_MODEL_ENTRY);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", CONSTRUCT_MODELSYSTEM);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", INIT_MODELSYSTEM);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", FLUSH_MODELSYSTEM);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetBackBufferClear);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetFaceModel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetHumanModel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMapClip);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendRenderCancel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetRenderCancel);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendBackBufferSignal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelIsBackBufferRequest);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendPauseSignal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendMenuStart);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendMenuEnd);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelGetMenuStatus);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendSignalStartBattle);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendSignalEventFinish);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendPacketChangeSignal);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSendSignalMovieStart);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetMpeg2CrossFadeTime);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetClip);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetAlpha);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetStencil);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetZwrite);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTransparency);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetReflTransparency);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetToumei);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetLight);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_model_set", nmlModelSetTexfunc);

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
