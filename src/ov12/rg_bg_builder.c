/*
 * OV12 original TU 64: 0x00a32c48..0x00a33bf8 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_simple_db.h"
#include "rg_bg_builder.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern void InitRgLight(void *light);
extern void RgLightCopy(void *dest, void *src);
extern void RgBgObjSetHardness(RgBgObj *pObj, float hardness);

/* Defined later in this TU. */
static int _GetTailParameter(const char *pszStr, int cDelim, int nBase);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a55748 contains the source filename "../rg_bg_builder.euc.c".
 * ov12:0x00a55798 contains the assertion expression "pBuilder != NIL".
 * ov12:0x00a557a8 contains the assertion expression "pGroup != NIL".
 * ov12:0x00a55860 contains the assertion expression
 * "pObj != NIL && pszKeyWord != NIL".
 * ov12:0x00a55978 contains the assertion expression "pGetBuf != NIL".
 */
extern const char D_00A55748[];
extern const char D_00A55798[];
extern const char D_00A557A8[];
extern const char D_00A55860[];
extern const char D_00A55978[];

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bg_builder", _CreateColi);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bg_builder", _EntryColi);

RgBgBuilder *CreateRgBgBuilder(void *pGroup)
{
    RgBgBuilder *pBuilder;

    pBuilder = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBgBuilder), D_00A55748,
                           129);
    if (pBuilder == 0) {
        assert_prog(D_00A55798, D_00A55748, 130);
    }
    if (pGroup == 0) {
        assert_prog(D_00A557A8, D_00A55748, 131);
    }
    pBuilder->group = pGroup;
    pBuilder->loadData = 0;
    pBuilder->database = CreateRgSimpleDB(0x40, 0x40);
    InitRgLight(&pBuilder->light);
    return pBuilder;
}

void DisposeRgBgBuilder(RgBgBuilder *pBuilder)
{
    if (pBuilder == 0) {
        assert_prog(D_00A55798, D_00A55748, 146);
    }
    DisposeRgSimpleDB(pBuilder->database);
    RgHeapFree(InstanceOfRgHeap(), pBuilder, D_00A55748, 148);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bg_builder", _ReadBgData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bg_builder", _NewBgObj);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bg_builder", _GetTailParameter);

static void _SetBgObjAttr(RgBgObj *pObj, const char *pszKeyWord)
{
    if (pObj == 0 || pszKeyWord == 0) {
        assert_prog(D_00A55860, D_00A55748, 382);
    }
    RgBgObjSetHardness(pObj, (float)_GetTailParameter(pszKeyWord, '*', 10));
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bg_builder", RgBgBuilderBuildFromText);

void *RgBgBuilderGetLoadData(RgBgBuilder *pBuilder)
{
    if (pBuilder == 0) {
        assert_prog(D_00A55798, D_00A55748, 609);
    }
    return pBuilder->loadData;
}

void RgBgBuilderGetLight(RgBgBuilder *pBuilder, void *pGetBuf)
{
    if (pBuilder == 0) {
        assert_prog(D_00A55798, D_00A55748, 617);
    }
    if (pGetBuf == 0) {
        assert_prog(D_00A55978, D_00A55748, 618);
    }
    RgLightCopy(pGetBuf, &pBuilder->light);
}
