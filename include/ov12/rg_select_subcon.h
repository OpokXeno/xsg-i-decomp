#ifndef INCLUDE_OV12_RG_SELECT_SUBCON_H
#define INCLUDE_OV12_RG_SELECT_SUBCON_H

#include "ov12/rg_equip_type.h"
#include "ov12/rg_weapon_db.h"

/* Number of selectable characters. */
#define RG_SELECT_CHAR_NUM 6

/* Number of weapons a selection holds, one per equip type. */
#define RG_SELECT_WEAPON_SLOTS RG_EQUIP_TYPE_NUM

/* Most weapon records one character and equip type can offer. */
#define RG_SELECT_WEAPON_MAX 128

/* Functions and data used by other translation units. */
extern void RgSelectIgnoreEventFlag(void);
extern int RgSelectGetPlayerChars(void);
extern int RgSelectGetEnemyChars(void);
extern int RgSelectGetEnemyChar(void);
extern unsigned int RgSelectWeapons(RgWeaponEssence **list,
                                    int characterID, int weaponType);
extern int RgSelectGetEnemyWeapons(RgWeaponEssence **weapons, int characterID);

#endif /* INCLUDE_OV12_RG_SELECT_SUBCON_H */
