/*
 * TU-local declarations of ov12/tu030 (src/ov12/rg_equip_type.c).
 */

#ifndef SRC_OV12_RG_EQUIP_TYPE_H
#define SRC_OV12_RG_EQUIP_TYPE_H

#include "shared.h"

/*
 * A weapon's equip record: this translation unit implements its own methods
 * (InitRgEquip and its still-asm siblings CopyRgEquip, RgEquipAddMount,
 * RgEquipSetDefault, RgEquipMountPos, ...). A different translation unit,
 * rg_weapon.c (ov12/tu026), embeds the same object by value inside RgWeapon
 * and only knows its bound from adjacency (src/ov12/rg_weapon.h's own
 * placeholder "RgEquip", unmodeled past +0x3c); that TU-local declaration is
 * unrelated to this one and this allocation names its own type to avoid
 * restating it. Only the two fields InitRgEquip clears are modelled here.
 */
typedef struct RgEquipRecord RgEquipRecord;

/* The accessor compares each mount's first word; the rest stays opaque. */
typedef struct RgEquipMount {
    int type;
    unsigned char unmodeled_004[8];
} RgEquipMount;

#define RG_EQUIP_MOUNT_MAX 4

struct RgEquipRecord {
    RgEquipMount mounts[RG_EQUIP_MOUNT_MAX]; /* +0x00..+0x2f */
    int mountCount;                    /* +0x30: cleared by InitRgEquip;
                                         * RgEquipAddMount's counterpart in
                                         * this allocation's function group */
    int activeMount;                   /* +0x34: cleared by InitRgEquip;
                                         * role beyond this allocation is not
                                         * evidenced here */
};

#endif /* SRC_OV12_RG_EQUIP_TYPE_H */
