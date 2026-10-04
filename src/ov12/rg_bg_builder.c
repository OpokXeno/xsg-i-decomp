/*
 * OV12 original TU 64: 0x00a32c48..0x00a33bf8 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_simple_db.h"
#include "rg_bg_builder.h"

const char D_00A55738[16] = "pDB != NIL";
const char D_00A55748[24] = "../rg_bg_builder.euc.c";
const char D_00A55760[16] = "pszName != NIL";
const char D_00A55770[16] = "pDat != NIL";
const char D_00A55780[24] = "already entried '%s'";
const char D_00A55798[16] = "pBuilder != NIL";
const char D_00A557A8[16] = "pGroup != NIL";
const char D_00A557B8[8] = "file";
const char D_00A557C0[32] = "duplicate specified 'file %s'";
const char D_00A557E0[16] = "data\\nisimori\\";
const char D_00A557F0[32] = "cannot load BG data file '%s'\n";
const char D_00A55810[8] = "tray";
const char D_00A55818[24] = "not specified 'file'";
const char D_00A55830[8] = "box";
const char D_00A55838[8] = "posbox";
const char D_00A55840[32] = "unknown BG keyword '%s'\n";
const char D_00A55860[40] = "pObj != NIL && pszKeyWord != NIL";
const char D_00A55888[16] = "pReader != NIL";
const char D_00A55898[8] = "BG";
const char D_00A558A0[24] = "not exist BG '%s'\n";
const char D_00A558B8[8] = "DATA";
const char D_00A558C0[8] = "False";
const char D_00A558C8[8] = "nofog";
const char D_00A558D0[8] = "nohide";
const char D_00A558D8[8] = "hitbody";
const char D_00A558E0[24] = "unknown option '%s'";
const char D_00A558F8[8] = "put";
const char D_00A55900[8] = "pos";
const char D_00A55908[8] = "grid";
const char D_00A55910[8] = "rand";
const char D_00A55918[8] = "light";
const char D_00A55920[24] = "unknown light-code '%c'";
const char D_00A55938[8] = "amb";
const char D_00A55940[8] = "fog";
const char D_00A55948[8] = "fogcol";
const char D_00A55950[24] = "unknown key word '%s'";
const char D_00A55968[16] = "complete BG %s\n";
const char D_00A55978[16] = "pGetBuf != NIL";

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
extern int atoi(const char *nptr);

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
extern const char D_00A55738[];
extern const char D_00A55760[];
extern const char D_00A55770[];

struct RgGeomGroup;
extern int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName);
extern void *RgSimpleDBGet(RgSimpleDB *pDB, int index);
extern void XrgUnitMatrix(RgMatrix destination);
extern RgGeom *RgGeomGroupCreatePiller(struct RgGeomGroup *pGroup,
                                       RgGeom *parent);
extern RgGeom *RgGeomGroupCreateTray(struct RgGeomGroup *pGroup,
                                     RgGeom *parent);
extern void RgGeomTraySetSize(void *tray, float width, float height);
extern void RgGeomTraySetLocal(void *tray, Matrix4 source);

static RgGeom *_CreateColi(void *pDB, const char *pszVariant, int id)
{
    RgColiEntry *pEntry;
    RgGeom *pGeom;
    Matrix4 local;
    int index;

    if (pDB == 0) {
        assert_prog(D_00A55738, D_00A55748, 45);
    }
    if (pszVariant == 0) {
        assert_prog(D_00A55760, D_00A55748, 46);
    }
    index = RgSimpleDBFind(pDB, pszVariant);
    if (index < 0) {
        return 0;
    }
    pEntry = RgSimpleDBGet(pDB, index);
    if (pEntry == 0) {
        assert_prog(D_00A55770, D_00A55748, 55);
    }
    XrgUnitMatrix(&local[0][0]);
    if (pEntry->id == 1) {
        pGeom = RgGeomGroupCreatePiller(
            (struct RgGeomGroup *)(unsigned int)id, 0);
    } else {
        pGeom = RgGeomGroupCreateTray(
            (struct RgGeomGroup *)(unsigned int)id, 0);
    }
    RgGeomTraySetSize(pGeom, pEntry->halfWidth, pEntry->halfHeight);
    local[3][0] = pEntry->value1;
    local[3][2] = pEntry->value2;
    RgGeomTraySetLocal(pGeom, local);
    return pGeom;
}

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

typedef struct RgReadText RgReadText;
extern int strcmp(const char *left, const char *right);
extern int RgReadTextIsEOF(RgReadText *pReader);
extern void RgReadTextGetString(RgReadText *pReader, char *pszOut);
extern float RgReadTextGetFloat(RgReadText *pReader);
extern RgFileSys *InstanceOfRgFileSys(void);
extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);
extern const char D_00A557B8[];
extern const char D_00A557C0[];
extern const char D_00A557E0[];
extern const char D_00A557F0[];
extern const char D_00A55810[];
extern const char D_00A55818[];
extern const char D_00A55830[];
extern const char D_00A55838[];
extern const char D_00A55840[];

static RgFileSysData *_ReadBgData(RgReadText *pReader, RgSimpleDB *pDB)
{
    char token[0x100];
    RgFileSysData *pDataFile;
    float width;
    float height;
    float positionX;
    float positionZ;

    pDataFile = 0;
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, token);
        if (strcmp(token, D_00A557B8) == 0) {
            if (pDataFile != 0) {
                RgError(D_00A557C0, D_00A55748, 168, token);
            }
            RgReadTextGetString(pReader, token);
            pDataFile = RgFileSysRead(InstanceOfRgFileSys(), token,
                                      D_00A557E0);
            if (pDataFile == 0) {
                RgError(D_00A557F0, D_00A55748, 174, token);
            }
        } else if (strcmp(token, D_00A55810) == 0) {
            if (pDataFile == 0) {
                RgError(D_00A55818, D_00A55748, 179);
            }
            width = RgReadTextGetFloat(pReader);
            height = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, token);
            _EntryColi(pDB, token, 0, width, height, 0.0f, 0.0f);
        } else if (strcmp(token, D_00A55830) == 0) {
            if (pDataFile == 0) {
                RgError(D_00A55818, D_00A55748, 188);
            }
            width = RgReadTextGetFloat(pReader);
            height = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, token);
            _EntryColi(pDB, token, 1, width, height, 0.0f, 0.0f);
        } else if (strcmp(token, D_00A55838) == 0) {
            positionX = RgReadTextGetFloat(pReader);
            positionZ = RgReadTextGetFloat(pReader);
            width = RgReadTextGetFloat(pReader);
            height = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, token);
            _EntryColi(pDB, token, 1, width, height, positionX, positionZ);
        } else {
            RgError(D_00A55840, D_00A55748, 204, token);
        }
    }
    return pDataFile;
}

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

static int _GetTailParameter(const char *pszStr, int cDelim, int nBase)
{
    char delimiter;

    delimiter = cDelim;
    while (*pszStr != delimiter && *pszStr != '\0') {
        pszStr++;
    }
    if (*pszStr != delimiter) {
        return -1;
    }
    if (pszStr[1] != '\0') {
        return atoi(pszStr + 1);
    }
    return nBase;
}

static void _SetBgObjAttr(RgBgObj *pObj, const char *pszKeyWord)
{
    if (pObj == 0 || pszKeyWord == 0) {
        assert_prog(D_00A55860, D_00A55748, 382);
    }
    RgBgObjSetHardness(pObj, (float)_GetTailParameter(pszKeyWord, '*', 10));
}

#include "ov12/rg_draw.h"
typedef struct RgReadText RgReadText;
static RgFileSysData *_ReadBgData(RgReadText *pReader, RgSimpleDB *pDB);
extern int RgReadTextFindParagraph(RgReadText *pReader, const char *pszTag,
                                   const char *pszSubTag);
extern int RgReadTextIsEOF(RgReadText *pReader);
extern void RgReadTextRewind(RgReadText *pReader);
extern void RgReadTextGetString(RgReadText *pReader, char *pszOut);
extern float RgReadTextGetFloat(RgReadText *pReader);
extern int RgReadTextGetInt(RgReadText *pReader);
extern int strcasecmp(const char *pszLeft, const char *pszRight);
extern int strncmp(const char *pszLeft, const char *pszRight,
                   unsigned int count);
extern void RgWarn(const char *format, const char *sourceFile, int line, ...);
extern void XrgLog(const char *format, const char *sourceFile, int line, ...);
extern int XrgRandIntRange(int lower, int upper);
extern void XrgClearVector(RgVector destination);
extern RgDraw *InstanceOfRgDraw(void);
extern void RgDrawGetGlobalFog(RgDraw *pDraw, RgFog *pFog);
extern void RgDrawSetGlobalFog(RgDraw *pDraw, RgFog *pFog);
extern void RgBgObjNotUseGeomLocal(RgBgObj *pObj);
extern void *RgBgObjGetDispModel(RgBgObj *pObj);
extern void RgBgObjIgnoreHide(RgBgObj *pObj, int ignoreHide);
extern void RgBgObjEnableBodyAttack(RgBgObj *pObj, int enable);
extern void RgDispModelResetMode(RgDispModel *pDispModel, int mode);
extern const char D_00A55888[];
extern const char D_00A55898[];
extern const char D_00A558A0[];
extern const char D_00A558B8[];
extern const char D_00A558C0[];
extern const char D_00A558C8[];
extern const char D_00A558D0[];
extern const char D_00A558D8[];
extern const char D_00A558E0[];
extern const char D_00A558F8[];
extern const char D_00A55900[];
extern const char D_00A55908[];
extern const char D_00A55910[];
extern const char D_00A55918[];
extern const char D_00A55920[];
extern const char D_00A55938[];
extern const char D_00A55940[];
extern const char D_00A55948[];
extern const char D_00A55950[];
extern const char D_00A55968[];

void RgBgBuilderBuildFromText(RgBgBuilder *pBuilder, RgReadText *pReader,
                              const char *pszStageName)
{
    char bgDataName[0x100];
    RgVector position;
    char keyword[0x100];
    char variant[0x100];
    int modeFlags;
    int ignoreHide;
    int enableBodyAttack;
    int firstX;
    int secondX;
    int firstZ;
    int secondZ;
    int x;
    int z;
    int lightIndex;
    char lightCode;
    unsigned char errorCode;
    float gridScale;
    RgFog fog;
    RgFog fogColor;
    RgDraw *pDraw;
    RgBgObj *pObj;
    RgDispModel *pDispModel;
    void *pGroup;

    if (pBuilder == 0) {
        assert_prog(D_00A55798, D_00A55748, 396);
    }
    if (pReader == 0) {
        assert_prog(D_00A55888, D_00A55748, 397);
    }

    RgReadTextRewind(pReader);
    if (RgReadTextFindParagraph(pReader, D_00A55898, pszStageName) == 0) {
        RgWarn(D_00A558A0, D_00A55748, 405, pszStageName);
        return;
    }

    RgReadTextRewind(pReader);
    RgReadTextFindParagraph(pReader, D_00A55898, pszStageName);
    RgReadTextGetString(pReader, bgDataName);
    if (bgDataName[0] != '*') {
        RgReadTextRewind(pReader);
        RgReadTextFindParagraph(pReader, D_00A558B8, bgDataName);
        pBuilder->loadData = _ReadBgData(pReader, pBuilder->database);
    } else {
        assert_prog(D_00A558C0, D_00A55748, 432);
    }

    gridScale = 10.0f;
    RgReadTextRewind(pReader);
    RgReadTextFindParagraph(pReader, D_00A55898, pszStageName);
    RgReadTextGetString(pReader, bgDataName);

    while (RgReadTextIsEOF(pReader) == 0) {
        pGroup = pBuilder->group;
        RgReadTextGetString(pReader, keyword);
        pObj = 0;
        modeFlags = 0;
        ignoreHide = 0;
        enableBodyAttack = 0;
        pDispModel = 0;

        if (keyword[0] == '-') {
            do {
                if (strcasecmp(keyword + 1, D_00A558C8) == 0) {
                    modeFlags |= 1;
                } else if (strcasecmp(keyword + 1, D_00A558D0) == 0) {
                    ignoreHide = 1;
                } else if (strcasecmp(keyword + 1, D_00A558D8) == 0) {
                    enableBodyAttack = 1;
                } else {
                    RgError(D_00A558E0, D_00A55748, 462, keyword);
                }
                RgReadTextGetString(pReader, keyword);
            } while (keyword[0] == '-');
        }

        XrgClearVector(position);
        if (strncmp(keyword, D_00A558F8, 3) == 0) {
            RgReadTextGetString(pReader, variant);
            pObj = _NewBgObj(pBuilder->database, pBuilder->loadData,
                             (int)pGroup, position, variant);
            RgBgObjNotUseGeomLocal(pObj);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, D_00A55900, 3) == 0) {
            position[0] = RgReadTextGetFloat(pReader);
            position[2] = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, variant);
            pObj = _NewBgObj(pBuilder->database, pBuilder->loadData,
                             (int)pGroup, position, variant);
            _SetBgObjAttr(pObj, keyword);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, D_00A55908, 4) == 0) {
            x = RgReadTextGetInt(pReader);
            z = RgReadTextGetInt(pReader);
            RgReadTextGetString(pReader, variant);
            position[0] = (float)x * gridScale;
            position[2] = (float)z * gridScale;
            pObj = _NewBgObj(pBuilder->database, pBuilder->loadData,
                             (int)pGroup, position, variant);
            _SetBgObjAttr(pObj, keyword);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, D_00A55910, 4) == 0) {
            firstX = RgReadTextGetInt(pReader);
            secondX = RgReadTextGetInt(pReader);
            firstZ = RgReadTextGetInt(pReader);
            secondZ = RgReadTextGetInt(pReader);
            x = XrgRandIntRange(firstX < secondX ? firstX : secondX,
                                firstX < secondX ? secondX : firstX);
            z = XrgRandIntRange(firstZ < secondZ ? firstZ : secondZ,
                                firstZ < secondZ ? secondZ : firstZ);
            RgReadTextGetString(pReader, variant);
            position[0] = (float)x * gridScale;
            position[2] = (float)z * gridScale;
            pObj = _NewBgObj(pBuilder->database, pBuilder->loadData,
                             (int)pGroup, position, variant);
            _SetBgObjAttr(pObj, keyword);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, D_00A55918, 5) == 0) {
            lightCode = keyword[5];
            errorCode = (unsigned char) keyword[5];
            if (lightCode < '3') {
                if (lightCode >= '0') {
                    lightIndex = lightCode - '0';
                    pBuilder->light[1 + lightIndex][0] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light[1 + lightIndex][1] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light[1 + lightIndex][2] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light[4 + lightIndex][0] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light[4 + lightIndex][1] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light[4 + lightIndex][2] =
                        RgReadTextGetFloat(pReader);
                } else {
                    RgError(D_00A55920, D_00A55748, 555,
                            (char) errorCode);
                }
            } else {
                RgError(D_00A55920, D_00A55748, 555,
                        (char) errorCode);
            }
        } else if (strcasecmp(keyword, D_00A55938) == 0) {
            pBuilder->light[0][0] = RgReadTextGetFloat(pReader);
            pBuilder->light[0][1] = RgReadTextGetFloat(pReader);
            pBuilder->light[0][2] = RgReadTextGetFloat(pReader);
        } else if (strcasecmp(keyword, D_00A55940) == 0) {
            pDraw = InstanceOfRgDraw();
            RgDrawGetGlobalFog(pDraw, &fog);
            fog.dist[0] = RgReadTextGetFloat(pReader);
            fog.dist[1] = RgReadTextGetFloat(pReader);
            fog.dist[2] = RgReadTextGetFloat(pReader);
            fog.dist[3] = RgReadTextGetFloat(pReader);
            pDraw = InstanceOfRgDraw();
            RgDrawSetGlobalFog(pDraw, &fog);
        } else if (strcasecmp(keyword, D_00A55948) == 0) {
            pDraw = InstanceOfRgDraw();
            RgDrawGetGlobalFog(pDraw, &fogColor);
            fogColor.color[0] = RgReadTextGetFloat(pReader);
            fogColor.color[1] = RgReadTextGetFloat(pReader);
            fogColor.color[2] = RgReadTextGetFloat(pReader);
            pDraw = InstanceOfRgDraw();
            RgDrawSetGlobalFog(pDraw, &fogColor);
        } else {
            RgError(D_00A55950, D_00A55748, 582, keyword);
        }

        if (pObj != 0) {
            RgBgObjIgnoreHide(pObj, ignoreHide);
            RgBgObjEnableBodyAttack(pObj, enableBodyAttack);
        }
        if (pDispModel != 0) {
            RgDispModelSetMode(pDispModel, 0);
            RgDispModelResetMode(pDispModel, modeFlags);
        }
    }

    XrgLog(D_00A55968, D_00A55748, 598, pszStageName);
}

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
