#ifndef INCLUDE_OV12_RG_BATTLE_COMMON_DATA_H
#define INCLUDE_OV12_RG_BATTLE_COMMON_DATA_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_font.h"

/*
 * The battle-common environment: the weapon-motion resource and the three
 * Bxx picture archives ("btl_all.bxx", "announce_p.bxx", "announce_n.bxx")
 * loaded by RgBattleCommonDataLoad, plus the four fonts built from them.
 */
typedef struct RgBattleCommonDataEnv {
    RgFileSysData *weaponMotList; /* 0x00: "weponall.fpk" */
    RgBxx *dispTexArchive;        /* 0x04: "btl_all.bxx", the battle display textures */
    RgFont *bulletFont;           /* 0x08: from dispTexArchive "bullet.bmp" */
    RgFont *timeFont;             /* 0x0C: from dispTexArchive "time.bmp" */
    RgFont *dispWeaponFont;       /* 0x10: from dispTexArchive "wpn_font.bmp" */
    RgBxx *announceAddTex;        /* 0x14: "announce_p.bxx", the additive announce textures */
    RgBxx *announceSubTex;        /* 0x18: "announce_n.bxx", the subtractive ones */
    RgFont *announceFont;         /* 0x1C: from announceAddTex "time_pt.bmp" */
} RgBattleCommonDataEnv;

/* Functions and data used by other translation units. */
RgBattleCommonDataEnv *InstanceOfRgBattleCommonData(void);
void RgBattleCommonDataLoad(RgBattleCommonDataEnv *pEnv);
void RgBattleCommonDataDispose(RgBattleCommonDataEnv *pEnv);
void *RgBattleCommonDataGetWeaponMot(RgBattleCommonDataEnv *pEnv);
RgBxx *RgBattleCommonDataGetDispTex(RgBattleCommonDataEnv *pEnv);
RgFont *RgBattleCommonDataBulletFont(RgBattleCommonDataEnv *pEnv);
RgFont *RgBattleCommonDataTimeFont(RgBattleCommonDataEnv *pEnv);
RgFont *RgBattleCommonDataDispWeaponFont(RgBattleCommonDataEnv *pEnv);
RgBxx *RgBattleCommonDataAnnounceAdd(RgBattleCommonDataEnv *pEnv);
RgBxx *RgBattleCommonDataAnnounceSub(RgBattleCommonDataEnv *pEnv);
RgFont *RgBattleCommonDataAnnounceFont(RgBattleCommonDataEnv *pEnv);

#endif /* INCLUDE_OV12_RG_BATTLE_COMMON_DATA_H */
