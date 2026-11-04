/*
 * OV12 original TU 34: 0x00a20040..0x00a20718 (18 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_error.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_font.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_singleton_id.h"

/* Glyphs in each font: digits and a few symbols, A-Z for the weapon font. */
#define BULLET_FONT_GLYPH_COUNT 13
#define TIME_FONT_GLYPH_COUNT 14
#define DISP_WPN_FONT_GLYPH_COUNT 39
#define ANNOUNCE_FONT_GLYPH_COUNT 11

extern const char battleCommonDataAssertEnvNotNil[];
extern const char battleCommonDataSourcePath[];
extern const char battleCommonDataNotLoadedDataError[];

/* Strings defined below. */
extern const char battleCommonDataAlreadyLoadedDataError[];
extern const char battleCommonDataAlreadyLoadedDispTexError[];
extern const char battleCommonDataAlreadyLoadedAnnounceAddTexError[];
extern const char battleCommonDataAlreadyLoadedAnnounceSubTexError[];
extern const char battleCommonDataWeaponArchiveFileName[];
extern const char battleCommonDataDirectory[];
extern const char battleCommonDataDispTexBxxFileName[];
extern const char battleCommonDataBulletFontFileName[];
extern const char battleCommonDataTimeFontFileName[];
extern const char battleCommonDataWeaponFontFileName[];
extern const char battleCommonDataAnnouncePlusBxxFileName[];
extern const char battleCommonDataAnnounceMinusBxxFileName[];
extern const char battleCommonDataTimePointFontFileName[];

static void _DestructEnv(RgBattleCommonDataEnv *pEnv);




const char battleCommonDataAssertEnvNotNil[16] = "pEnv != NIL";

const char battleCommonDataSourcePath[32] = "../rg_battle_common_data.euc.c";

const char battleCommonDataAlreadyLoadedDataError[40] = "already loaded battle common data";

const char battleCommonDataAlreadyLoadedDispTexError[40] = "already loaded battle common disp tex";

const char battleCommonDataAlreadyLoadedAnnounceAddTexError[32] = "already loaded announce add tex";

const char battleCommonDataAlreadyLoadedAnnounceSubTexError[32] = "already loaded announce sub tex";

const char battleCommonDataWeaponArchiveFileName[16] = "weponall.fpk";

const char battleCommonDataDirectory[16] = "data\\nisimori\\";

const char battleCommonDataDispTexBxxFileName[16] = "btl_all.bxx";

const char battleCommonDataBulletFontFileName[16] = "bullet.bmp";

const char battleCommonDataTimeFontFileName[16] = "time.bmp";

const char battleCommonDataWeaponFontFileName[16] = "wpn_font.bmp";

const char battleCommonDataAnnouncePlusBxxFileName[16] = "announce_p.bxx";

const char battleCommonDataAnnounceMinusBxxFileName[16] = "announce_n.bxx";

const char battleCommonDataTimePointFontFileName[16] = "time_pt.bmp";

const char battleCommonDataNotLoadedDataError[32] = "not loaded battle common data";

static RgFontGlyph s_aBulletFontTbl_0[BULLET_FONT_GLYPH_COUNT] = {
    {32, 0, 0, 0, {0, 0}, 0, 4},
    {48, 0, 0, 0, {11, 16}, 0, 8},
    {49, 0, 11, 0, {8, 16}, 0, 8},
    {50, 0, 19, 0, {11, 16}, 0, 8},
    {51, 0, 30, 0, {10, 16}, 0, 8},
    {52, 0, 40, 0, {11, 16}, 0, 8},
    {53, 0, 51, 0, {10, 16}, 0, 8},
    {54, 0, 61, 0, {11, 16}, 0, 8},
    {55, 0, 72, 0, {11, 16}, 0, 8},
    {56, 0, 83, 0, {11, 16}, 0, 8},
    {57, 0, 94, 0, {11, 16}, 0, 8},
    {47, 0, 105, 0, {9, 16}, 0, 7},
    {37, 0, 114, 0, {12, 16}, 0, 10}
};

static RgFontGlyph s_aFontTbl_1[TIME_FONT_GLYPH_COUNT] = {
    {32, 0, 0, 0, {0, 0}, 0, 6},
    {48, 0, 0, 0, {13, 19}, 0, 11},
    {49, 0, 13, 0, {13, 19}, 0, 11},
    {50, 0, 26, 0, {13, 19}, 0, 11},
    {51, 0, 39, 0, {13, 19}, 0, 11},
    {52, 0, 52, 0, {13, 19}, 0, 11},
    {53, 0, 66, 0, {13, 19}, 0, 11},
    {54, 0, 79, 0, {13, 19}, 0, 11},
    {55, 0, 93, 0, {13, 19}, 0, 11},
    {56, 0, 106, 0, {13, 19}, 0, 11},
    {57, 0, 121, 0, {13, 19}, 0, 11},
    {39, 0, 133, 0, {7, 19}, 0, 7},
    {34, 0, 140, 0, {10, 19}, 0, 7},
    {107, 0, 151, 0, {31, 20}, 0, 13}
};

static RgFontGlyph s_aFontTbl_2[DISP_WPN_FONT_GLYPH_COUNT] = {
    {65, 0, 0, 0, {9, 14}, 0, -1},
    {66, 0, 10, 0, {9, 14}, 0, -1},
    {67, 0, 20, 0, {9, 14}, 0, -1},
    {68, 0, 30, 0, {8, 14}, 0, -1},
    {69, 0, 39, 0, {8, 14}, 0, -1},
    {70, 0, 48, 0, {8, 14}, 0, -1},
    {71, 0, 57, 0, {9, 14}, 0, -1},
    {72, 0, 67, 0, {8, 14}, 0, -1},
    {73, 0, 76, 0, {4, 14}, 0, -1},
    {74, 0, 81, 0, {8, 14}, 0, -1},
    {75, 0, 90, 0, {8, 14}, 0, -1},
    {76, 0, 99, 0, {7, 14}, 0, -1},
    {77, 0, 107, 0, {10, 14}, 0, -1},
    {78, 0, 118, 0, {9, 14}, 0, -1},
    {79, 0, 128, 0, {10, 14}, 0, -1},
    {80, 0, 139, 0, {9, 14}, 0, -1},
    {81, 0, 149, 0, {10, 14}, 0, -1},
    {82, 0, 160, 0, {9, 14}, 0, -1},
    {83, 0, 170, 0, {8, 14}, 0, -1},
    {84, 0, 179, 0, {9, 14}, 0, -1},
    {85, 0, 189, 0, {9, 14}, 0, -1},
    {86, 0, 199, 0, {10, 14}, 0, -1},
    {87, 0, 210, 0, {11, 14}, 0, -1},
    {88, 0, 222, 0, {9, 14}, 0, -1},
    {89, 0, 232, 0, {9, 14}, 0, -1},
    {90, 0, 242, 0, {8, 14}, 0, -1},
    {48, 0, 0, 16, {8, 14}, 0, 0},
    {49, 0, 9, 16, {6, 14}, 0, 0},
    {50, 0, 16, 16, {8, 14}, 0, 0},
    {51, 0, 25, 16, {7, 14}, 0, 0},
    {52, 0, 33, 16, {7, 14}, 0, 0},
    {53, 0, 41, 16, {7, 14}, 0, 0},
    {54, 0, 49, 16, {7, 14}, 0, 0},
    {55, 0, 57, 16, {7, 14}, 0, 0},
    {56, 0, 65, 16, {8, 14}, 0, 0},
    {57, 0, 74, 16, {8, 14}, 0, 0},
    {45, 0, 83, 16, {8, 14}, 0, -2},
    {58, 0, 92, 16, {5, 14}, 0, 0},
    {47, 0, 98, 16, {7, 14}, 0, 0}
};

static RgFontGlyph s_aBulletFontTbl_3[ANNOUNCE_FONT_GLYPH_COUNT] = {
    {32, 0, 0, 0, {0, 0}, 0, 8},
    {48, 0, 0, 0, {13, 15}, 0, 12},
    {49, 0, 14, 0, {13, 15}, 0, 12},
    {50, 0, 28, 0, {13, 15}, 0, 12},
    {51, 0, 42, 0, {13, 15}, 0, 12},
    {52, 0, 56, 0, {13, 15}, 0, 12},
    {53, 0, 70, 0, {13, 15}, 0, 12},
    {54, 0, 84, 0, {13, 15}, 0, 12},
    {55, 0, 98, 0, {13, 15}, 0, 12},
    {56, 0, 112, 0, {13, 15}, 0, 12},
    {57, 0, 126, 0, {13, 15}, 0, 12}
};

static RgFont *_CreateBulletFont(RgBxx *archive, const char *pictureName)
{
    struct RgBxxPic *pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aBulletFontTbl_0, BULLET_FONT_GLYPH_COUNT, pic);
}

static RgFont *_CreateTimeFont(RgBxx *archive, const char *pictureName)
{
    struct RgBxxPic *pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aFontTbl_1, TIME_FONT_GLYPH_COUNT, pic);
}

static RgFont *_CreateDispWpnFont(RgBxx *archive, const char *pictureName)
{
    struct RgBxxPic *pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aFontTbl_2, DISP_WPN_FONT_GLYPH_COUNT, pic);
}

static RgFont *_CreateAnnounceNumFont(RgBxx *archive, const char *pictureName)
{
    struct RgBxxPic *pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aBulletFontTbl_3, ANNOUNCE_FONT_GLYPH_COUNT, pic);
}

static void _InitEnv(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0xB6);
    }
    pEnv->weaponMotList = 0;
    pEnv->dispTexArchive = 0;
    pEnv->bulletFont = 0;
    pEnv->timeFont = 0;
    pEnv->dispWeaponFont = 0;
    pEnv->announceAddTex = 0;
    pEnv->announceSubTex = 0;
    pEnv->announceFont = 0;
}

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation only
 * stores and forwards the pointer LoadRgBxx_sub returns and DisposeRgBxx_sub
 * takes, so an incomplete type is enough here.
 */

static void _DestructEnv(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 195);
    }
    if (pEnv->weaponMotList != 0) {
        DisposeRgFileSysData_sub(pEnv->weaponMotList,
                                 battleCommonDataSourcePath, 197);
        pEnv->weaponMotList = 0;
    }
    if (pEnv->dispTexArchive != 0) {
        DisposeRgBxx_sub(pEnv->dispTexArchive, battleCommonDataSourcePath, 201);
        pEnv->dispTexArchive = 0;
    }
    if (pEnv->bulletFont != 0) {
        DisposeRgFont(pEnv->bulletFont);
        pEnv->bulletFont = 0;
    }
    if (pEnv->timeFont != 0) {
        DisposeRgFont(pEnv->timeFont);
        pEnv->timeFont = 0;
    }
    if (pEnv->dispWeaponFont != 0) {
        DisposeRgFont(pEnv->dispWeaponFont);
        pEnv->dispWeaponFont = 0;
    }
    if (pEnv->announceAddTex != 0) {
        DisposeRgBxx_sub(pEnv->announceAddTex, battleCommonDataSourcePath, 217);
        pEnv->announceAddTex = 0;
    }
    if (pEnv->announceSubTex != 0) {
        DisposeRgBxx_sub(pEnv->announceSubTex, battleCommonDataSourcePath, 221);
        pEnv->announceSubTex = 0;
    }
    if (pEnv->announceFont != 0) {
        DisposeRgFont(pEnv->announceFont);
        pEnv->announceFont = 0;
    }
}

static void _WrapperDestruct(void *pEnv)
{
    _DestructEnv(pEnv);
    RgHeapFree(InstanceOfRgHeap(), pEnv, battleCommonDataSourcePath, 0xE6);
}

RgBattleCommonDataEnv *InstanceOfRgBattleCommonData(void)
{
    RgBattleCommonDataEnv *pEnv;

    pEnv = RgSingletonIDGet(RG_SINGLETON_BATTLE_COMMON_DATA);
    if (pEnv == 0) {
        pEnv = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBattleCommonDataEnv),
                           battleCommonDataSourcePath, 240);
        _InitEnv(pEnv);
        RgSingletonIDEntry(RG_SINGLETON_BATTLE_COMMON_DATA, pEnv, _WrapperDestruct);
    }
    return pEnv;
}

void RgBattleCommonDataLoad(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 253);
    }
    if (pEnv->weaponMotList != 0) {
        RgError(battleCommonDataAlreadyLoadedDataError, battleCommonDataSourcePath, 255);
    }
    if (pEnv->dispTexArchive != 0) {
        RgError(battleCommonDataAlreadyLoadedDispTexError, battleCommonDataSourcePath, 258);
    }
    if (pEnv->announceAddTex != 0) {
        RgError(battleCommonDataAlreadyLoadedAnnounceAddTexError, battleCommonDataSourcePath, 261);
    }
    if (pEnv->announceSubTex != 0) {
        RgError(battleCommonDataAlreadyLoadedAnnounceSubTexError, battleCommonDataSourcePath, 264);
    }
    pEnv->weaponMotList = RgFileSysRead(InstanceOfRgFileSys(),
                                                battleCommonDataWeaponArchiveFileName, battleCommonDataDirectory);
    pEnv->dispTexArchive = LoadRgBxx_sub(battleCommonDataDispTexBxxFileName, battleCommonDataSourcePath, 268);
    if (pEnv->dispTexArchive != 0) {
        pEnv->bulletFont = _CreateBulletFont(pEnv->dispTexArchive, battleCommonDataBulletFontFileName);
        pEnv->timeFont = _CreateTimeFont(pEnv->dispTexArchive, battleCommonDataTimeFontFileName);
        pEnv->dispWeaponFont = _CreateDispWpnFont(pEnv->dispTexArchive,
                                                  battleCommonDataWeaponFontFileName);
    }
    pEnv->announceAddTex = LoadRgBxx_sub(battleCommonDataAnnouncePlusBxxFileName, battleCommonDataSourcePath, 274);
    pEnv->announceSubTex = LoadRgBxx_sub(battleCommonDataAnnounceMinusBxxFileName, battleCommonDataSourcePath, 275);
    pEnv->announceFont = _CreateAnnounceNumFont(pEnv->announceAddTex,
                                                battleCommonDataTimePointFontFileName);
}

void RgBattleCommonDataDispose(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x11D);
        RgError(battleCommonDataNotLoadedDataError, battleCommonDataSourcePath, 0x11F);
    }
    _DestructEnv(pEnv);
}

void *RgBattleCommonDataGetWeaponMot(RgBattleCommonDataEnv *pEnv)
{
    RgFileSysData *weaponMotList;
    void *weaponMot;

    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x12B);
    }
    weaponMotList = pEnv->weaponMotList;
    weaponMot = 0;
    if (weaponMotList != 0) {
        weaponMot = weaponMotList->data;
    }
    return weaponMot;
}

RgBxx *RgBattleCommonDataGetDispTex(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x138);
    }
    return pEnv->dispTexArchive;
}

RgFont *RgBattleCommonDataBulletFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x145);
    }
    return pEnv->bulletFont;
}

RgFont *RgBattleCommonDataTimeFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x14E);
    }
    return pEnv->timeFont;
}

RgFont *RgBattleCommonDataDispWeaponFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x157);
    }
    return pEnv->dispWeaponFont;
}

RgBxx *RgBattleCommonDataAnnounceAdd(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x160);
    }
    return pEnv->announceAddTex;
}

RgBxx *RgBattleCommonDataAnnounceSub(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x169);
    }
    return pEnv->announceSubTex;
}

RgFont *RgBattleCommonDataAnnounceFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(battleCommonDataAssertEnvNotNil, battleCommonDataSourcePath, 0x172);
    }
    return pEnv->announceFont;
}
