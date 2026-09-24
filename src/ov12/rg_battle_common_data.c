/*
 * OV12 original TU 34: 0x00a20040..0x00a20718 (18 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"

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

extern unsigned char s_aBulletFontTbl_0[];
extern unsigned char s_aFontTbl_1[];
extern unsigned char s_aFontTbl_2[];
extern unsigned char s_aBulletFontTbl_3[];

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
