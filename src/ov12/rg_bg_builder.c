/*
 * OV12 original TU 64: 0x00a32c48..0x00a33bf8 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_bg_builder.h"
#include "ov12/rg_bgobj.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_geom_group.h"
#include "ov12/rg_geom_tray.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_simple_db.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_light.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_error.h"
#include "ov12/xrg_v_log.h"

/* The builder's layout: CreateRgBgBuilder allocates sizeof(RgBgBuilder), 0x90
 * bytes. */
struct RgBgBuilder {
    RgGeomGroup *group;              /* +0x00 */
    RgFileSysData *loadData;         /* +0x04: the loaded BG data file */
    unsigned char unmodeled_08[8];   /* +0x08 */
    RgLight light;                   /* +0x10 */
    RgSimpleDB *database;            /* +0x80: collision boxes by name */
    unsigned char unmodeled_84[0xc]; /* +0x84 */
};

/*
 * A collision-box entry _EntryColi allocates (RgHeapAlloc size 0x14) and
 * registers by name into a RgSimpleDB. halfWidth/halfHeight are half of the
 * width/height arguments; value1/value2 are the box's x/z position for the
 * "posbox" keyword.
 */
typedef struct RgColiEntry {
    int kind;         /* +0x00: RG_COLI_KIND_* */
    float halfWidth;  /* +0x04 */
    float halfHeight; /* +0x08 */
    float value1;     /* +0x0C */
    float value2;     /* +0x10 */
} RgColiEntry;

/* The geometry a collision box creates: "tray" gives a tray, "box" and
 * "posbox" a pillar. */
#define RG_COLI_KIND_TRAY 0
#define RG_COLI_KIND_PILLAR 1

/* Capacity and entry size of the collision-box database. */
#define RG_BG_BUILDER_DB_CAPACITY 0x40
#define RG_BG_BUILDER_DB_ENTRY_SIZE 0x40

/* Size of the text-token buffers of the parser. */
#define RG_BG_BUILDER_TOKEN_SIZE 0x100

/* Directional lights the "light0".."light2" keywords set. */
#define RG_BG_BUILDER_LIGHT_COUNT 3

/* Distance of one "grid" or "rand" cell. */
#define RG_BG_BUILDER_GRID_SCALE 10.0f

/* Hardness used when a keyword's "*" suffix is empty. */
#define RG_BG_BUILDER_DEFAULT_HARDNESS 10


const char bgBuilderAssertDbNotNil[16] = "pDB != NIL";
const char bgBuilderSourcePath[24] = "../rg_bg_builder.euc.c";
const char bgBuilderAssertNameNotNil[16] = "pszName != NIL";
const char bgBuilderAssertDatNotNil[16] = "pDat != NIL";
const char bgBuilderAlreadyEntriedFormat[24] = "already entried '%s'";
const char bgBuilderAssertBuilderNotNil[16] = "pBuilder != NIL";
const char bgBuilderAssertGroupNotNil[16] = "pGroup != NIL";
const char bgBuilderKeywordFile[8] = "file";
const char bgBuilderDuplicateFileFormat[32] = "duplicate specified 'file %s'";
const char bgBuilderDataDirectory[16] = "data\\nisimori\\";
const char bgBuilderCannotLoadFileFormat[32] = "cannot load BG data file '%s'\n";
const char bgBuilderKeywordTray[8] = "tray";
const char bgBuilderFileNotSpecifiedMessage[24] = "not specified 'file'";
const char bgBuilderKeywordBox[8] = "box";
const char bgBuilderKeywordPosBox[8] = "posbox";
const char bgBuilderUnknownKeywordFormat[32] = "unknown BG keyword '%s'\n";
const char bgBuilderAssertObjAndKeywordNotNil[40] = "pObj != NIL && pszKeyWord != NIL";
const char bgBuilderAssertReaderNotNil[16] = "pReader != NIL";
const char bgBuilderParagraphBg[8] = "BG";
const char bgBuilderStageNotExistFormat[24] = "not exist BG '%s'\n";
const char bgBuilderParagraphData[8] = "DATA";
const char bgBuilderAssertFalse[8] = "False";
const char bgBuilderOptionNoFog[8] = "nofog";
const char bgBuilderOptionNoHide[8] = "nohide";
const char bgBuilderOptionHitBody[8] = "hitbody";
const char bgBuilderUnknownOptionFormat[24] = "unknown option '%s'";
const char bgBuilderKeywordPut[8] = "put";
const char bgBuilderKeywordPos[8] = "pos";
const char bgBuilderKeywordGrid[8] = "grid";
const char bgBuilderKeywordRand[8] = "rand";
const char bgBuilderKeywordLight[8] = "light";
const char bgBuilderUnknownLightCodeFormat[24] = "unknown light-code '%c'";
const char bgBuilderKeywordAmbient[8] = "amb";
const char bgBuilderKeywordFog[8] = "fog";
const char bgBuilderKeywordFogColor[8] = "fogcol";
const char bgBuilderUnknownKeyWordFormat[24] = "unknown key word '%s'";
const char bgBuilderCompleteFormat[16] = "complete BG %s\n";
const char bgBuilderAssertGetBufNotNil[16] = "pGetBuf != NIL";


static int _GetTailParameter(const char *pszStr, int cDelim, int nBase);



/*
 * Creates the tray or pillar geometry for the collision box named pszVariant
 * and adds it to pGroup, placed at the box's x/z position. Returns 0 when no
 * such box is registered.
 */
static RgGeom *_CreateColi(RgSimpleDB *pDB, const char *pszVariant,
                           RgGeomGroup *pGroup)
{
    RgColiEntry *pEntry;
    RgGeom *pGeom;
    RgMatrix local;
    int index;

    if (pDB == 0) {
        assert_prog(bgBuilderAssertDbNotNil, bgBuilderSourcePath, 45);
    }
    if (pszVariant == 0) {
        assert_prog(bgBuilderAssertNameNotNil, bgBuilderSourcePath, 46);
    }
    index = RgSimpleDBFind(pDB, pszVariant);
    if (index < 0) {
        return 0;
    }
    pEntry = RgSimpleDBGet(pDB, index);
    if (pEntry == 0) {
        assert_prog(bgBuilderAssertDatNotNil, bgBuilderSourcePath, 55);
    }
    XrgUnitMatrix(local);
    if (pEntry->kind == RG_COLI_KIND_PILLAR) {
        pGeom = RgGeomGroupCreatePiller(pGroup, 0);
    } else {
        pGeom = RgGeomGroupCreateTray(pGroup, 0);
    }
    RgGeomTraySetSize(pGeom, pEntry->halfWidth, pEntry->halfHeight);
    local[12] = pEntry->value1;
    local[14] = pEntry->value2;
    RgGeomTraySetLocal(pGeom, local);
    return pGeom;
}






/*
 * Allocates a collision-box entry with the given kind and size and registers
 * it under pszName. Returns 0 and reports an error when the name is taken.
 */
static RgColiEntry *_EntryColi(RgSimpleDB *pDB, const char *pszName, int kind,
                               float width, float height, float value1,
                               float value2)
{
    RgColiEntry *pEntry;

    pEntry = 0;
    if (pDB == 0) {
        assert_prog(bgBuilderAssertDbNotNil, bgBuilderSourcePath, 0x4F);
    }
    if (pszName == 0) {
        assert_prog(bgBuilderAssertNameNotNil, bgBuilderSourcePath, 0x50);
    }
    if (RgSimpleDBFind(pDB, pszName) < 0) {
        pEntry = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgColiEntry),
                             bgBuilderSourcePath, 0x55);
        pEntry->kind = kind;
        pEntry->halfWidth = width * 0.5f;
        pEntry->halfHeight = height * 0.5f;
        pEntry->value1 = value1;
        pEntry->value2 = value2;
        RgSimpleDBEntry(pDB, pEntry, pszName);
    } else {
        RgError(bgBuilderAlreadyEntriedFormat, bgBuilderSourcePath, 0x64, pszName);
    }
    return pEntry;
}

/* Allocates a builder that adds its objects to pGroup, with an empty collision-box database. */
RgBgBuilder *CreateRgBgBuilder(RgGeomGroup *pGroup)
{
    RgBgBuilder *pBuilder;

    pBuilder = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBgBuilder), bgBuilderSourcePath,
                           129);
    if (pBuilder == 0) {
        assert_prog(bgBuilderAssertBuilderNotNil, bgBuilderSourcePath, 130);
    }
    if (pGroup == 0) {
        assert_prog(bgBuilderAssertGroupNotNil, bgBuilderSourcePath, 131);
    }
    pBuilder->group = pGroup;
    pBuilder->loadData = 0;
    pBuilder->database = CreateRgSimpleDB(RG_BG_BUILDER_DB_CAPACITY,
                                         RG_BG_BUILDER_DB_ENTRY_SIZE);
    InitRgLight(&pBuilder->light);
    return pBuilder;
}

void DisposeRgBgBuilder(RgBgBuilder *pBuilder)
{
    if (pBuilder == 0) {
        assert_prog(bgBuilderAssertBuilderNotNil, bgBuilderSourcePath, 146);
    }
    DisposeRgSimpleDB(pBuilder->database);
    RgHeapFree(InstanceOfRgHeap(), pBuilder, bgBuilderSourcePath, 148);
}


/*
 * Reads the DATA paragraph until EOF. The "file" keyword loads a data file,
 * and tray, box and posbox register collision boxes in pDB. Returns the
 * loaded data file.
 */
static RgFileSysData *_ReadBgData(RgReadText *pReader, RgSimpleDB *pDB)
{
    char token[RG_BG_BUILDER_TOKEN_SIZE];
    RgFileSysData *pDataFile;
    float width;
    float height;
    float positionX;
    float positionZ;

    pDataFile = 0;
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, token);
        if (strcmp(token, bgBuilderKeywordFile) == 0) {
            if (pDataFile != 0) {
                RgError(bgBuilderDuplicateFileFormat, bgBuilderSourcePath, 168, token);
            }
            RgReadTextGetString(pReader, token);
            pDataFile = RgFileSysRead(InstanceOfRgFileSys(), token,
                                      bgBuilderDataDirectory);
            if (pDataFile == 0) {
                RgError(bgBuilderCannotLoadFileFormat, bgBuilderSourcePath, 174, token);
            }
        } else if (strcmp(token, bgBuilderKeywordTray) == 0) {
            if (pDataFile == 0) {
                RgError(bgBuilderFileNotSpecifiedMessage, bgBuilderSourcePath, 179);
            }
            width = RgReadTextGetFloat(pReader);
            height = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, token);
            _EntryColi(pDB, token, RG_COLI_KIND_TRAY, width, height, 0.0f, 0.0f);
        } else if (strcmp(token, bgBuilderKeywordBox) == 0) {
            if (pDataFile == 0) {
                RgError(bgBuilderFileNotSpecifiedMessage, bgBuilderSourcePath, 188);
            }
            width = RgReadTextGetFloat(pReader);
            height = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, token);
            _EntryColi(pDB, token, RG_COLI_KIND_PILLAR, width, height, 0.0f, 0.0f);
        } else if (strcmp(token, bgBuilderKeywordPosBox) == 0) {
            positionX = RgReadTextGetFloat(pReader);
            positionZ = RgReadTextGetFloat(pReader);
            width = RgReadTextGetFloat(pReader);
            height = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, token);
            _EntryColi(pDB, token, RG_COLI_KIND_PILLAR, width, height, positionX,
                   positionZ);
        } else {
            RgError(bgBuilderUnknownKeywordFormat, bgBuilderSourcePath, 204, token);
        }
    }
    return pDataFile;
}



/*
 * Creates a background object from the model named variant. Its collision
 * geometry, if the database has one, is moved by the x and z of offset.
 */
static RgBgObj *_NewBgObj(RgSimpleDB *pDB, RgFileSysData *pData,
                          RgGeomGroup *pGroup, RgVector offset,
                          const char *variant)
{
    RgDispModel *dispModel;
    RgGeom *geomTray;
    RgMatrix local;
    RgBgObj *pObj;

    dispModel = CreateXrgDispModelImpl(pData->data, variant);
    RgDispModelSetMode(dispModel, RG_DISP_MODEL_MODE_FOG);
    geomTray = _CreateColi(pDB, variant, pGroup);
    if (geomTray != 0) {
        RgGeomTrayGetLocal(geomTray, local);
        local[12] += offset[0];
        local[14] += offset[2];
        RgGeomTraySetLocal(geomTray, local);
    }
    pObj = CreateRgBgObj(dispModel, geomTray);
    if (geomTray != 0) {
        RgGeomSetParent(geomTray, pObj);
    }
    return pObj;
}

/*
 * Returns the integer after the first cDelim in pszStr, or nBase when nothing
 * follows the delimiter. Returns -1 when the delimiter is absent.
 */
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
        assert_prog(bgBuilderAssertObjAndKeywordNotNil, bgBuilderSourcePath, 382);
    }
    RgBgObjSetHardness(pObj, (float)_GetTailParameter(pszKeyWord, '*',
                                                   RG_BG_BUILDER_DEFAULT_HARDNESS));
}


/*
 * Builds the objects of stage pszStageName from its BG paragraph: loads the
 * stage's data file, then handles one keyword at a time until the reader
 * reaches EOF.
 */
void RgBgBuilderBuildFromText(RgBgBuilder *pBuilder, RgReadText *pReader,
                              const char *pszStageName)
{
    char bgDataName[RG_BG_BUILDER_TOKEN_SIZE];
    RgVector position;
    char keyword[RG_BG_BUILDER_TOKEN_SIZE];
    char variant[RG_BG_BUILDER_TOKEN_SIZE];
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
    RgGeomGroup *pGroup;

    if (pBuilder == 0) {
        assert_prog(bgBuilderAssertBuilderNotNil, bgBuilderSourcePath, 396);
    }
    if (pReader == 0) {
        assert_prog(bgBuilderAssertReaderNotNil, bgBuilderSourcePath, 397);
    }

    /* Warns and returns when the stage has no BG paragraph. */
    RgReadTextRewind(pReader);
    if (RgReadTextFindParagraph(pReader, bgBuilderParagraphBg, pszStageName) == 0) {
        RgWarn(bgBuilderStageNotExistFormat, bgBuilderSourcePath, 405, pszStageName);
        return;
    }

    RgReadTextRewind(pReader);
    RgReadTextFindParagraph(pReader, bgBuilderParagraphBg, pszStageName);
    RgReadTextGetString(pReader, bgDataName);
    /* Loads the DATA paragraph that the BG entry names. */
    if (bgDataName[0] != '*') {
        RgReadTextRewind(pReader);
        RgReadTextFindParagraph(pReader, bgBuilderParagraphData, bgDataName);
        pBuilder->loadData = _ReadBgData(pReader, pBuilder->database);
    } else {
        assert_prog(bgBuilderAssertFalse, bgBuilderSourcePath, 432);
    }

    gridScale = RG_BG_BUILDER_GRID_SCALE;
    RgReadTextRewind(pReader);
    RgReadTextFindParagraph(pReader, bgBuilderParagraphBg, pszStageName);
    RgReadTextGetString(pReader, bgDataName);

    /* Each pass reads one keyword with its leading options and arguments. */
    while (RgReadTextIsEOF(pReader) == 0) {
        pGroup = pBuilder->group;
        RgReadTextGetString(pReader, keyword);
        pObj = 0;
        modeFlags = 0;
        ignoreHide = 0;
        enableBodyAttack = 0;
        pDispModel = 0;

        /* Leading "-" options apply to the object that the keyword creates. */
        if (keyword[0] == '-') {
            do {
                if (strcasecmp(keyword + 1, bgBuilderOptionNoFog) == 0) {
                    modeFlags |= RG_DISP_MODEL_MODE_FOG;
                } else if (strcasecmp(keyword + 1, bgBuilderOptionNoHide) == 0) {
                    ignoreHide = 1;
                } else if (strcasecmp(keyword + 1, bgBuilderOptionHitBody) == 0) {
                    enableBodyAttack = 1;
                } else {
                    RgError(bgBuilderUnknownOptionFormat, bgBuilderSourcePath, 462, keyword);
                }
                RgReadTextGetString(pReader, keyword);
            } while (keyword[0] == '-');
        }

        XrgClearVector(position);
        if (strncmp(keyword, bgBuilderKeywordPut, 3) == 0) {
            RgReadTextGetString(pReader, variant);
            pObj = _NewBgObj(pBuilder->database, pBuilder->loadData,
                             pGroup, position, variant);
            RgBgObjNotUseGeomLocal(pObj);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, bgBuilderKeywordPos, 3) == 0) {
            position[0] = RgReadTextGetFloat(pReader);
            position[2] = RgReadTextGetFloat(pReader);
            RgReadTextGetString(pReader, variant);
            pObj = _NewBgObj(pBuilder->database, pBuilder->loadData,
                             pGroup, position, variant);
            _SetBgObjAttr(pObj, keyword);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, bgBuilderKeywordGrid, 4) == 0) {
            x = RgReadTextGetInt(pReader);
            z = RgReadTextGetInt(pReader);
            RgReadTextGetString(pReader, variant);
            position[0] = (float)x * gridScale;
            position[2] = (float)z * gridScale;
            pObj = _NewBgObj(pBuilder->database, pBuilder->loadData,
                             pGroup, position, variant);
            _SetBgObjAttr(pObj, keyword);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, bgBuilderKeywordRand, 4) == 0) {
            /* Each axis takes a random cell between its two bounds, in either order. */
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
                             pGroup, position, variant);
            _SetBgObjAttr(pObj, keyword);
            pDispModel = RgBgObjGetDispModel(pObj);
        } else if (strncmp(keyword, bgBuilderKeywordLight, 5) == 0) {
            /* lightN reads three direction values, then three color values. */
            lightCode = keyword[5];
            errorCode = (unsigned char) keyword[5];
            if (lightCode < '0' + RG_BG_BUILDER_LIGHT_COUNT) {
                if (lightCode >= '0') {
                    lightIndex = lightCode - '0';
                    pBuilder->light.direction[lightIndex][0] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light.direction[lightIndex][1] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light.direction[lightIndex][2] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light.color[lightIndex][0] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light.color[lightIndex][1] =
                        RgReadTextGetFloat(pReader);
                    pBuilder->light.color[lightIndex][2] =
                        RgReadTextGetFloat(pReader);
                } else {
                    RgError(bgBuilderUnknownLightCodeFormat, bgBuilderSourcePath, 555,
                            (char) errorCode);
                }
            } else {
                RgError(bgBuilderUnknownLightCodeFormat, bgBuilderSourcePath, 555,
                        (char) errorCode);
            }
        } else if (strcasecmp(keyword, bgBuilderKeywordAmbient) == 0) {
            pBuilder->light.ambient_color[0] = RgReadTextGetFloat(pReader);
            pBuilder->light.ambient_color[1] = RgReadTextGetFloat(pReader);
            pBuilder->light.ambient_color[2] = RgReadTextGetFloat(pReader);
        } else if (strcasecmp(keyword, bgBuilderKeywordFog) == 0) {
            pDraw = InstanceOfRgDraw();
            RgDrawGetGlobalFog(pDraw, &fog);
            fog.dist[0] = RgReadTextGetFloat(pReader);
            fog.dist[1] = RgReadTextGetFloat(pReader);
            fog.dist[2] = RgReadTextGetFloat(pReader);
            fog.dist[3] = RgReadTextGetFloat(pReader);
            pDraw = InstanceOfRgDraw();
            RgDrawSetGlobalFog(pDraw, &fog);
        } else if (strcasecmp(keyword, bgBuilderKeywordFogColor) == 0) {
            pDraw = InstanceOfRgDraw();
            RgDrawGetGlobalFog(pDraw, &fogColor);
            fogColor.color[0] = RgReadTextGetFloat(pReader);
            fogColor.color[1] = RgReadTextGetFloat(pReader);
            fogColor.color[2] = RgReadTextGetFloat(pReader);
            pDraw = InstanceOfRgDraw();
            RgDrawSetGlobalFog(pDraw, &fogColor);
        } else {
            RgError(bgBuilderUnknownKeyWordFormat, bgBuilderSourcePath, 582, keyword);
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

    XrgLog(bgBuilderCompleteFormat, bgBuilderSourcePath, 598, pszStageName);
}

RgFileSysData *RgBgBuilderGetLoadData(RgBgBuilder *pBuilder)
{
    if (pBuilder == 0) {
        assert_prog(bgBuilderAssertBuilderNotNil, bgBuilderSourcePath, 609);
    }
    return pBuilder->loadData;
}

void RgBgBuilderGetLight(RgBgBuilder *pBuilder, RgLight *pGetBuf)
{
    if (pBuilder == 0) {
        assert_prog(bgBuilderAssertBuilderNotNil, bgBuilderSourcePath, 617);
    }
    if (pGetBuf == 0) {
        assert_prog(bgBuilderAssertGetBufNotNil, bgBuilderSourcePath, 618);
    }
    RgLightCopy(pGetBuf, &pBuilder->light);
}
