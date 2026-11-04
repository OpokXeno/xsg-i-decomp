/*
 * OV12 original TU 79: 0x00a45120..0x00a454f8 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/rg_draw.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_linkdata.h"
#include "main/nml_model_set.h"

#define XRG_DISPMODEL_IMPL_SIZE 112

/* Below this transparency the model is drawn translucent. */
#define XRG_DISPMODEL_IMPL_OPAQUE_LIMIT 0.95f
/* nmlModelSetAlpha blend selector used for MODE_ALPHA models. */
#define XRG_DISPMODEL_IMPL_ALPHA_BLEND 0x48
#define XRG_DISPMODEL_IMPL_RENDER_LEVEL 2

extern void nmlModelEntry(const char *pLexData);

/* The draw callback RgDrawReq queues: the studio being drawn gives the fog. */
static void _Disp(void *pObject, RgDrawStudio *pStudio);

const char dispModelImplAssertDispNotNil[] = "pDisp != NIL";
const char dispModelImplSourcePath[] = "../xrg_dispmodel_impl.euc.c";
const char dispModelImplAssertXtxDataNotNil[] = "pXtxData != NIL";
const char dispModelImplAssertLexDataNotNil[] = "pLexData != NIL";

/*
 * The object CreateXrgDispModelImpl (ov12:0x00a453cc) allocates: 112 bytes
 * from the RgHeap, initialised as a RgDispModel by _InitRgDispModelImpl and
 * then handed to RgDispModelDisplay through the base object's dispMethod.
 * Past the base object it keeps the two link-data blocks
 * _InitRgDispModelImpl receives and asserts on, named by the assertion
 * expressions "pXtxData != NIL" and "pLexData != NIL": the texture block
 * _Disp passes to nmlModelSetTexture, which accepts it only when its first
 * three bytes spell "XTX" (main:0x0022fe60), and the model block _Disp
 * passes to nmlModelEntry. Offsets 0x54..0x5f stay an explicit unmodeled
 * span; no claimed function reads or writes the eight bytes past pLexData,
 * so the type ends with the two link-data pointers.
 */
typedef struct XrgDispModelImpl {
    RgDispModel model;
    unsigned char unmodeled_54[0xc];
    const char *pXtxData;
    const char *pLexData;
} XrgDispModelImpl;

static void _Disp(void *pObject, RgDrawStudio *pStudio)
{
    RgDispModel *pDisp = pObject;
    XrgDispModelImpl *pImpl;
    RgFog *pFog;
    float transparent;
    Vector4 color;

    if (pDisp == 0) {
        assert_prog(dispModelImplAssertDispNotNil, dispModelImplSourcePath, 47);
    }

    pImpl = (XrgDispModelImpl *)pDisp;
    if (pImpl->pXtxData == 0 || pImpl->pLexData == 0) {
        return;
    }

    transparent = pDisp->transparent;
    if (transparent <= 0.0f) {
        return;
    }

    if (transparent >= 0.0f && transparent < XRG_DISPMODEL_IMPL_OPAQUE_LIMIT) {
        /*
         * color is opaque white with the model's own transparency as its
         * fourth component here; the only call that reads the array back is
         * nmlModelSetFogCol below, after the fog pass has replaced the first
         * three components with the studio's fog color.
         */
        color.w = transparent;
        color.x = color.y = color.z = 1.0f;
        nmlModelSetTransparency(transparent);
        nmlModelSetToumei(1);
        nmlModelSetZwrite(1);
        if (pDisp->mode & RG_DISP_MODEL_MODE_ALPHA) {
            nmlModelSetAlpha(XRG_DISPMODEL_IMPL_ALPHA_BLEND);
        }
    }

    if (pStudio != 0 && (pDisp->mode & RG_DISP_MODEL_MODE_FOG)) {
        pFog = pStudio->m_pFog;
        color.x = pFog->color[0];
        color.y = pFog->color[1];
        color.z = pFog->color[2];
        nmlModelSetFogCol(&color);
        nmlModelSetFogDist(pFog->dist[0], pFog->dist[1], pFog->dist[2],
                           pFog->dist[3]);
    }

    if (pDisp->mode & RG_DISP_MODEL_MODE_MAPCLIP) {
        nmlModelSetMapClip(1);
    }
    nmlModelSetPlace((Matrix4 *)pDisp->local);
    nmlModelSetTexture(pImpl->pXtxData);
    nmlModelSetRenderLevel(XRG_DISPMODEL_IMPL_RENDER_LEVEL);
    nmlModelEntry(pImpl->pLexData);
}

static void _DispRgDispModelImpl(RgDispModel *pDisp)
{
    RgDrawReq(InstanceOfRgDraw(), pDisp, &_Disp, 0, 0, -1);
}

static void _DestructRgDispModelImpl(RgDispModel *pDisp)
{
    if (pDisp == 0) {
        assert_prog(dispModelImplAssertDispNotNil, dispModelImplSourcePath, 119);
    }
}

static void _InitRgDispModelImpl(XrgDispModelImpl *pDisp, const char *pXtxData,
                                 const char *pLexData)
{
    if (pDisp == 0) {
        assert_prog(dispModelImplAssertDispNotNil, dispModelImplSourcePath, 126);
    }
    if (pXtxData == 0) {
        assert_prog(dispModelImplAssertXtxDataNotNil, dispModelImplSourcePath, 127);
    }
    if (pLexData == 0) {
        assert_prog(dispModelImplAssertLexDataNotNil, dispModelImplSourcePath, 128);
    }

    InitRgDispModel(&pDisp->model);
    pDisp->model.dispMethod = _DispRgDispModelImpl;
    pDisp->model.destructMethod = _DestructRgDispModelImpl;
    pDisp->pXtxData = pXtxData;
    pDisp->pLexData = pLexData;
}

RgDispModel *CreateXrgDispModelImpl(void *archive, const char *variant)
{
    char szLexName[128];
    char szXtxName[128];
    RgLinkData linkData;
    const char *pLexData;
    const char *pXtxData;
    XrgDispModelImpl *pDisp;

    strcat(strcpy(szLexName, variant), ".lex");
    strcat(strcpy(szXtxName, variant), ".xtx");
    InitRgLinkData(&linkData, archive);
    pLexData = RgLinkDataGet(&linkData, szLexName);
    pXtxData = RgLinkDataGet(&linkData, szXtxName);
    if (pXtxData == 0) {
        pXtxData = RgLinkDataFindExt(&linkData, ".xtx");
    }

    if (pLexData != 0 && pXtxData != 0) {
        pDisp = RgHeapAlloc(InstanceOfRgHeap(), XRG_DISPMODEL_IMPL_SIZE, dispModelImplSourcePath, 168);
        if (pDisp == 0) {
            assert_prog(dispModelImplAssertDispNotNil, dispModelImplSourcePath, 169);
        }
        _InitRgDispModelImpl(pDisp, pXtxData, pLexData);
    } else {
        pDisp = 0;
    }
    return (RgDispModel *)pDisp;
}
