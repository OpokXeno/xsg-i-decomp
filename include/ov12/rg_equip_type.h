#ifndef INCLUDE_OV12_RG_EQUIP_TYPE_H
#define INCLUDE_OV12_RG_EQUIP_TYPE_H

/* Number of equip types (left, right, back): "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM". */
#define RG_EQUIP_TYPE_NUM 3

/* The back equip type: its weapon is the only one RgRobot reports ready for eType 2. */
#define RG_EQUIP_TYPE_BACK 2

/* Number of mount positions, RG_EQUIP_POS_LEFT_HAND..RG_EQUIP_POS_BACK_RIGHT. */
#define RG_EQUIP_POS_NUM 8

/* Mounts one equip record holds. */
#define RG_EQUIP_MOUNT_MAX 4

/*
 * The equip type and the mount position are enumerations in the original
 * (asserts "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM" and
 * "(RG_EQUIP_POS_LEFT_HAND <= (ePos) && (ePos) <= RG_EQUIP_POS_BACK_RIGHT) ||
 * (ePos) == RG_EQUIP_POS_INVALID").
 */
typedef enum RgEquipType {
    RG_EQUIP_TYPE_MIN = 0,
    RG_EQUIP_TYPE_LEFT = 0,
    RG_EQUIP_TYPE_RIGHT = 1
} RgEquipType;

typedef enum RgEquipPos {
    RG_EQUIP_POS_INVALID = -1,
    RG_EQUIP_POS_LEFT_HAND = 0,
    RG_EQUIP_POS_BACK_RIGHT = RG_EQUIP_POS_NUM - 1
} RgEquipPos;

/*
 * One mount, as RgEquipAddMount appends it: the equip type, the mount
 * position and the mask of positions allowed for that type.
 */
typedef struct RgEquipMount {
    RgEquipType type;
    RgEquipPos pos;
    unsigned int posMask;
} RgEquipMount;

/*
 * A weapon's equip record (0x38 bytes), embedded by value in RgWeapon and in
 * the weapon essences at +0x70.
 */
typedef struct RgEquip {
    RgEquipMount mounts[RG_EQUIP_MOUNT_MAX]; /* +0x00 */
    int mountCount;                          /* +0x30 */
    int activeMount;                         /* +0x34 */
} RgEquip;

/* Functions and data used by other translation units. */
void InitRgEquip(RgEquip *pEquip);
void CopyRgEquip(RgEquip *pDst, RgEquip *pSrc);
void RgEquipAddMount(RgEquip *pEquip, unsigned int eType, int ePos,
                     unsigned int posMask);
void RgEquipSetDefault(RgEquip *pEquip, unsigned int posMask);
int RgEquipIsEquipable(RgEquip *pEquip, int eType);
int RgEquipCheckConfrict(RgEquip **ppEquip, unsigned int eType);
const char *RgEquipTypeString(int eType);
int RgEquipMountPos(RgEquip *pEquip, unsigned int eType, int charID,
                    const char *pszWepName);
const char *RgEquipMountPosString(RgEquip *pEquip, unsigned int eType,
                                  int charID, const char *pszWepName);
int RgEquipPosToJntID(int ePos);

#endif /* INCLUDE_OV12_RG_EQUIP_TYPE_H */
