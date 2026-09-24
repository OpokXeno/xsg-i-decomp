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

/*
 * Scaffold-owned (splat names, no config/symbols/ov12.txt entry).
 * ov12:0x00a55738 holds the assertion expression "pDB != NIL".
 * ov12:0x00a55760 holds the assertion expression "pszName != NIL".
 * ov12:0x00a55780 holds the format string "already entried '%s'".
 */
extern const char D_00A55738[];
extern const char D_00A55760[];
extern const char D_00A55780[];

extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName);
extern void RgSimpleDBEntry(RgSimpleDB *pDB, void *pDat, const char *pszName);

static RgColiEntry *_EntryColi(RgSimpleDB *pDB, const char *pszName, int id,
                               float width, float height, float value1,
                               float value2)
{
    RgColiEntry *pEntry;

    pEntry = 0;
    if (pDB == 0) {
        assert_prog(D_00A55738, D_00A55748, 0x4F);
    }
    if (pszName == 0) {
        assert_prog(D_00A55760, D_00A55748, 0x50);
    }
    if (RgSimpleDBFind(pDB, pszName) < 0) {
        pEntry = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgColiEntry),
                             D_00A55748, 0x55);
        pEntry->id = id;
        pEntry->halfWidth = width * 0.5f;
        pEntry->halfHeight = height * 0.5f;
        pEntry->value1 = value1;
        pEntry->value2 = value2;
        RgSimpleDBEntry(pDB, pEntry, pszName);
    } else {
        RgError(D_00A55780, D_00A55748, 0x64, pszName);
    }
    return pEntry;
}

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

extern RgBgObj *CreateRgBgObj(void *dispModel, RgGeom *geomTray);
extern RgDispModel *CreateXrgDispModelImpl(const char *name,
                                           const char *variant);
extern void RgDispModelSetMode(RgDispModel *dispModel, int mode);
extern void RgGeomSetParent(RgGeom *pGeom, void *parent);
extern void RgGeomTrayGetLocal(void *tray, Matrix4 destination);
extern void RgGeomTraySetLocal(void *tray, Matrix4 source);

/* Defined later in this TU (LOCAL sibling still in asm). */
static RgGeom *_CreateColi(void *pGroup, const char *variant, int id);

static RgBgObj *_NewBgObj(void *pGroup, RgBgObjDef *pDef, int id,
                          RgVector offset, const char *variant)
{
    RgDispModel *dispModel;
    RgGeom *geomTray;
    Matrix4 local;
    RgBgObj *pObj;

    dispModel = CreateXrgDispModelImpl(pDef->name, variant);
    RgDispModelSetMode(dispModel, 1);
    geomTray = _CreateColi(pGroup, variant, id);
    if (geomTray != 0) {
        RgGeomTrayGetLocal(geomTray, local);
        local[3][0] += offset[0];
        local[3][2] += offset[2];
        RgGeomTraySetLocal(geomTray, local);
    }
    pObj = CreateRgBgObj(dispModel, geomTray);
    if (geomTray != 0) {
        RgGeomSetParent(geomTray, pObj);
    }
    return pObj;
}

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
