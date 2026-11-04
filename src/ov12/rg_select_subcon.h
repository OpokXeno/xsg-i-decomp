/*
 * TU-local declarations of ov12/tu070 (src/ov12/rg_select_subcon.c).
 */

#ifndef SRC_OV12_RG_SELECT_SUBCON_H
#define SRC_OV12_RG_SELECT_SUBCON_H

/*
 * The weapon database record _get_all_weapons filters: the record RgWeaponDB
 * hands out by index. Only the members this allocation reads are modeled;
 * the record's database name sits at +0x330 (rg_weapon_db.c, RgWeaponDBRecord).
 */
struct RgSelectWeaponRecord {
    unsigned char unmodeled_00[0x70];
    unsigned char equip[0x38];     /* +0x70, address passed as the equip record */
    int characterMask;             /* +0xa8 */
    unsigned char unmodeled_ac[0x2f4];
    int eventLevel;                /* +0x3a0 */
    int eventUsable;               /* +0x3a4 */
};

extern unsigned int RgSelectWeapons(int *list, int characterID,
                                    int weaponType);

#endif /* SRC_OV12_RG_SELECT_SUBCON_H */
