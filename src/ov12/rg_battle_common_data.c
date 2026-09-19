/*
 * OV12 original TU 34: 0x00a20040..0x00a20718 (18 functions)
 */
#include "common.h"
#include "shared.h"

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_common_data", _DestructEnv_00A20160);

static void _WrapperDestruct(RgBattleCommonDataEnv *pEnv)
{
    _DestructEnv(pEnv);
    RgHeapFree(InstanceOfRgHeap(), pEnv, D_00A54168, 0xE6);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_common_data", InstanceOfRgBattleCommonData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_common_data", RgBattleCommonDataLoad);

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
