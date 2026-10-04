/*
 * OV12 original TU 34: 0x00a20040..0x00a20718 (18 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_font.h"

const char D_00A54158[16] = "pEnv != NIL";
const char D_00A54168[32] = "../rg_battle_common_data.euc.c";
const char D_00A54188[40] = "already loaded battle common data";
const char D_00A541B0[40] = "already loaded battle common disp tex";
const char D_00A541D8[32] = "already loaded announce add tex";
const char D_00A541F8[32] = "already loaded announce sub tex";
const char D_00A54218[16] = "weponall.fpk";
const char D_00A54228[16] = "data\\nisimori\\";
const char D_00A54238[16] = "btl_all.bxx";
const char D_00A54248[16] = "bullet.bmp";
const char D_00A54258[16] = "time.bmp";
const char D_00A54268[16] = "wpn_font.bmp";
const char D_00A54278[16] = "announce_p.bxx";
const char D_00A54288[16] = "announce_n.bxx";
const char D_00A54298[16] = "time_pt.bmp";
const char D_00A542A8[32] = "not loaded battle common data";

/*
 * The battle-common environment: the weapon-motion resource and the three
 * Bxx picture archives ("btl_all.bxx", "announce_p.bxx", "announce_n.bxx")
 * loaded by RgBattleCommonDataLoad, plus the four RgFont handles built from
 * them.  Field names/comments follow the "already loaded ..." RgError text
 * RgBattleCommonDataLoad reports for each archive, and the field 0x00 load
 * ("weponall.fpk" through RgFileSysRead).  Every accessor below asserts pEnv
 * is non-null before reading a field, matching the "pEnv != NIL" assertion
 * string shared by all of them.
 */
typedef struct RgBattleCommonDataEnv {
    int *weaponMotList;     /* 0x00: RgFileSysRead("weponall.fpk") result */
    int dispTexArchive;     /* 0x04: LoadRgBxx_sub("btl_all.bxx") -- "battle common disp tex" */
    int bulletFont;         /* 0x08: _CreateBulletFont(dispTexArchive, "bullet.bmp") */
    int timeFont;           /* 0x0C: _CreateTimeFont(dispTexArchive, "time.bmp") */
    int dispWeaponFont;     /* 0x10: _CreateDispWpnFont(dispTexArchive, "wpn_font.bmp") */
    int announceAddTex;     /* 0x14: LoadRgBxx_sub("announce_p.bxx") -- "announce add tex" */
    int announceSubTex;     /* 0x18: LoadRgBxx_sub("announce_n.bxx") -- "announce sub tex" */
    int announceFont;       /* 0x1C: _CreateAnnounceNumFont(announceAddTex, "time_pt.bmp") */
} RgBattleCommonDataEnv;

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgError(const char *message, const char *source_file, int line, ...);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

extern const char D_00A54158[];
extern const char D_00A54168[];
extern const char D_00A542A8[];

static RgFontGlyph s_aBulletFontTbl_0[13] = {
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

static RgFontGlyph s_aFontTbl_1[14] = {
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

static RgFontGlyph s_aFontTbl_2[39] = {
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

static RgFontGlyph s_aBulletFontTbl_3[11] = {
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

extern int RgBxxGetPic(int archive, const char *pictureName);
extern int CreateRgFont(void *table, int glyphCount, int pic);

static void _DestructEnv(RgBattleCommonDataEnv *pEnv);

static int _CreateBulletFont(int archive, const char *pictureName)
{
    int pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aBulletFontTbl_0, 13, pic);
}

static int _CreateTimeFont(int archive, const char *pictureName)
{
    int pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aFontTbl_1, 14, pic);
}

static int _CreateDispWpnFont(int archive, const char *pictureName)
{
    int pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aFontTbl_2, 0x27, pic);
}

static int _CreateAnnounceNumFont(int archive, const char *pictureName)
{
    int pic = RgBxxGetPic(archive, pictureName);
    return CreateRgFont(s_aBulletFontTbl_3, 11, pic);
}

static void _InitEnv(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0xB6);
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
 * RgFileSys and RgFileSysData are defined by ov12/tu065 (src/ov12/rg_filesys.c);
 * this allocation only stores and forwards the pointers InstanceOfRgFileSys/
 * RgFileSysRead return, so incomplete types are enough here (matching
 * rg_effect_env.c's own declaration of the same pair).
 */
extern RgFileSys *InstanceOfRgFileSys(void);
extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);
extern void DisposeRgFileSysData_sub(RgFileSysData *pFile,
                                     const char *source_file, int line);

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation only
 * stores and forwards the pointer LoadRgBxx_sub returns and DisposeRgBxx_sub
 * takes, so an incomplete type is enough here.
 */
typedef struct RgBxx RgBxx;
extern RgBxx *LoadRgBxx_sub(const char *pszName, const char *source_file,
                            int line);
extern void DisposeRgBxx_sub(RgBxx *pBxx, const char *source_file, int line);

/*
 * RgFont is defined by ov12/tu072 (src/ov12/rg_font.c); this allocation only
 * stores the int handle _CreateBulletFont/_CreateTimeFont/_CreateDispWpnFont/
 * _CreateAnnounceNumFont (above) return and DisposeRgFont takes.
 */
extern void DisposeRgFont(int font);

static void _DestructEnv(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 195);
    }
    if (pEnv->weaponMotList != 0) {
        DisposeRgFileSysData_sub((RgFileSysData *) pEnv->weaponMotList,
                                 D_00A54168, 197);
        pEnv->weaponMotList = 0;
    }
    if (pEnv->dispTexArchive != 0) {
        DisposeRgBxx_sub((RgBxx *) pEnv->dispTexArchive, D_00A54168, 201);
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
        DisposeRgBxx_sub((RgBxx *) pEnv->announceAddTex, D_00A54168, 217);
        pEnv->announceAddTex = 0;
    }
    if (pEnv->announceSubTex != 0) {
        DisposeRgBxx_sub((RgBxx *) pEnv->announceSubTex, D_00A54168, 221);
        pEnv->announceSubTex = 0;
    }
    if (pEnv->announceFont != 0) {
        DisposeRgFont(pEnv->announceFont);
        pEnv->announceFont = 0;
    }
}

static void _WrapperDestruct(RgBattleCommonDataEnv *pEnv)
{
    _DestructEnv(pEnv);
    RgHeapFree(InstanceOfRgHeap(), pEnv, D_00A54168, 0xE6);
}

RgBattleCommonDataEnv *InstanceOfRgBattleCommonData(void)
{
    RgBattleCommonDataEnv *pEnv;

    pEnv = RgSingletonIDGet(11);
    if (pEnv == 0) {
        pEnv = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBattleCommonDataEnv),
                           D_00A54168, 240);
        _InitEnv(pEnv);
        RgSingletonIDEntry(11, (RgSimpleDB *) pEnv,
                           (void (*)(RgSimpleDB *)) _WrapperDestruct);
    }
    return pEnv;
}

/*
 * ov12:0x00a54188 "already loaded battle common data", 0x00a541b0 "already
 * loaded battle common disp tex", 0x00a541d8 "already loaded announce add
 * tex", 0x00a541f8 "already loaded announce sub tex": the RgError text
 * RgBattleCommonDataLoad reports when the matching field (struct comment
 * above) is already non-null.
 */
extern const char D_00A54188[];
extern const char D_00A541B0[];
extern const char D_00A541D8[];
extern const char D_00A541F8[];

/* ov12:0x00a54218 "weponall.fpk", 0x00a54228 "data\nisimori\" (RgFileSysRead's root path). */
extern const char D_00A54218[];
extern const char D_00A54228[];

/* ov12:0x00a54238 "btl_all.bxx" */
extern const char D_00A54238[];
/* ov12:0x00a54248 "bullet.bmp", 0x00a54258 "time.bmp", 0x00a54268 "wpn_font.bmp" */
extern const char D_00A54248[];
extern const char D_00A54258[];
extern const char D_00A54268[];
/* ov12:0x00a54278 "announce_p.bxx", 0x00a54288 "announce_n.bxx" */
extern const char D_00A54278[];
extern const char D_00A54288[];
/* ov12:0x00a54298 "time_pt.bmp" */
extern const char D_00A54298[];

void RgBattleCommonDataLoad(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 253);
    }
    if (pEnv->weaponMotList != 0) {
        RgError(D_00A54188, D_00A54168, 255);
    }
    if (pEnv->dispTexArchive != 0) {
        RgError(D_00A541B0, D_00A54168, 258);
    }
    if (pEnv->announceAddTex != 0) {
        RgError(D_00A541D8, D_00A54168, 261);
    }
    if (pEnv->announceSubTex != 0) {
        RgError(D_00A541F8, D_00A54168, 264);
    }
    pEnv->weaponMotList = (int *) RgFileSysRead(InstanceOfRgFileSys(),
                                                D_00A54218, D_00A54228);
    pEnv->dispTexArchive = (int) LoadRgBxx_sub(D_00A54238, D_00A54168, 268);
    if (pEnv->dispTexArchive != 0) {
        pEnv->bulletFont = _CreateBulletFont(pEnv->dispTexArchive, D_00A54248);
        pEnv->timeFont = _CreateTimeFont(pEnv->dispTexArchive, D_00A54258);
        pEnv->dispWeaponFont = _CreateDispWpnFont(pEnv->dispTexArchive,
                                                  D_00A54268);
    }
    pEnv->announceAddTex = (int) LoadRgBxx_sub(D_00A54278, D_00A54168, 274);
    pEnv->announceSubTex = (int) LoadRgBxx_sub(D_00A54288, D_00A54168, 275);
    pEnv->announceFont = _CreateAnnounceNumFont(pEnv->announceAddTex,
                                                D_00A54298);
}

void RgBattleCommonDataDispose(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x11D);
        RgError(D_00A542A8, D_00A54168, 0x11F);
    }
    _DestructEnv(pEnv);
}

int RgBattleCommonDataGetWeaponMot(RgBattleCommonDataEnv *pEnv)
{
    int *weaponMotList;
    int weaponMot;

    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x12B);
    }
    weaponMotList = pEnv->weaponMotList;
    weaponMot = 0;
    if (weaponMotList != 0) {
        weaponMot = *weaponMotList;
    }
    return weaponMot;
}

int RgBattleCommonDataGetDispTex(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x138);
    }
    return pEnv->dispTexArchive;
}

int RgBattleCommonDataBulletFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x145);
    }
    return pEnv->bulletFont;
}

int RgBattleCommonDataTimeFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x14E);
    }
    return pEnv->timeFont;
}

int RgBattleCommonDataDispWeaponFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x157);
    }
    return pEnv->dispWeaponFont;
}

int RgBattleCommonDataAnnounceAdd(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x160);
    }
    return pEnv->announceAddTex;
}

int RgBattleCommonDataAnnounceSub(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x169);
    }
    return pEnv->announceSubTex;
}

int RgBattleCommonDataAnnounceFont(RgBattleCommonDataEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A54158, D_00A54168, 0x172);
    }
    return pEnv->announceFont;
}
