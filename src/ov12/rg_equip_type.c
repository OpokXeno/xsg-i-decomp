/*
 * OV12 original TU 30: 0x00a1eea0..0x00a1f7e0 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_equip_type.h"
#include "ov12/assert_prog.h"


/* Defined here: mount position -> joint id, then the assert and name strings. */
static int s_aePosToJntID[8] = {0, 2, 4, 1, 3, 5, 6, 7};

const char equipTypeAssertEquipPosInRange[] = "(RG_EQUIP_POS_LEFT_HAND <= (ePos) && (ePos) <= RG_EQUIP_POS_BACK_RIGHT) || (ePos) == RG_EQUIP_POS_INVALID";

const char equipTypeSourcePath[] = "../rg_equip_type.euc.c";

const char equipTypeAssertEquipNotNil[] = "pEquip != NIL";

const char equipTypeAssertTypeInRange[] = "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM";

const char equipTypeLeftText[] = "left";

const char equipTypeRightText[] = "right";

const char equipTypeBackText[] = "back";

const char equipTypeUnknownText[] = "?\?\?";

/* Declared here, defined after the functions below: the assert, weapon and mount name strings. */
extern const char equipTypeAssertCopyPointersNotNil[];  /* CopyRgEquip assert text */
extern const char equipTypeAssertMountCountBelowMax[];  /* mount-count assert text */
extern const char equipTypeAssertMountCountNotNegative[];  /* "nMount >= 0" */
extern const char equipTypeAssertEquipArrayNotNil[];  /* "apEquip != NIL" */
extern const char equipTypeAssertTypeInRangeLoose[];  /* "RG_EQUIP_TYPE_MIN <= eType && eType < RG_EQUIP_TYPE_NUM" */
extern const char equipTypeWeaponNameBmp45vx[];  /* weapon name "BMP45VX" */
extern const char equipTypeWeaponNameSmp53ag[];  /* weapon name "SMP53AG" */
extern const char equipTypeWeaponNameCb85vx[];  /* weapon name "CB85VX" */
/* The eight mount position names, indexed by the values RgEquipMountPos returns. */
extern const char equipMountLeftHandText[];
extern const char equipMountLeftElbowText[];
extern const char equipMountLeftShoulderText[];
extern const char equipMountRightHandText[];
extern const char equipMountRightElbowText[];
extern const char equipMountRightShoulderText[];
extern const char equipMountBackLeftText[];
extern const char equipMountBackRightText[];
/* Maps each mount position to its equip type. */
static int s_aePosToType[RG_EQUIP_POS_NUM];

/* Returns the joint id of a mount position, or -1 for RG_EQUIP_POS_INVALID. */
int RgEquipPosToJntID(int ePos)
{
    if ((unsigned int)ePos + 1u >= 9u) {
        assert_prog(equipTypeAssertEquipPosInRange, equipTypeSourcePath, 50);
    }
    if (ePos == RG_EQUIP_POS_INVALID) {
        return -1;
    }
    return s_aePosToJntID[ePos];
}

void InitRgEquip(RgEquip *pEquip)
{
    if (pEquip == 0) {
        assert_prog(equipTypeAssertEquipNotNil, equipTypeSourcePath, 0x3D);
    }
    pEquip->mountCount = 0;
    pEquip->activeMount = 0;
}

/* Copies the mount list of pSrc into pDst. */
void CopyRgEquip(RgEquip *pDst, RgEquip *pSrc)
{
    int i;

    if (pDst == 0 || pSrc == 0) {
        assert_prog(equipTypeAssertCopyPointersNotNil, equipTypeSourcePath, 69);
    }
    pDst->mountCount = pSrc->mountCount;
    for (i = 0; i < pSrc->mountCount; i++) {
        pDst->mounts[i].type = pSrc->mounts[i].type;
        pDst->mounts[i].pos = pSrc->mounts[i].pos;
        pDst->mounts[i].posMask = pSrc->mounts[i].posMask;
    }
}

/* Appends a mount of type eType at position ePos with the given position mask. */
void RgEquipAddMount(RgEquip *pEquip, unsigned int eType, int ePos,
                     unsigned int posMask)
{
    int mountIndex;

    if (pEquip == 0) {
        assert_prog(equipTypeAssertEquipNotNil, equipTypeSourcePath, 85);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(equipTypeAssertTypeInRange, equipTypeSourcePath, 86);
    }
    if ((unsigned int)ePos + 1u >= RG_EQUIP_POS_NUM + 1) {
        assert_prog(equipTypeAssertEquipPosInRange, equipTypeSourcePath, 88);
    }
    if (pEquip->mountCount >= RG_EQUIP_MOUNT_MAX) {
        assert_prog(equipTypeAssertMountCountBelowMax, equipTypeSourcePath, 89);
    }
    mountIndex = pEquip->mountCount++;
    pEquip->mounts[mountIndex].type = eType;
    pEquip->mounts[mountIndex].pos = ePos;
    pEquip->mounts[mountIndex].posMask = posMask;
}

/*
 * Rebuilds the mount list for the positions in posMask. Each equip type that has
 * a position in the mask gets one mount, at its highest matching position.
 */
void RgEquipSetDefault(RgEquip *pEquip, unsigned int posMask)
{
    int eType;
    int ePos;
    int i;
    int nMount;
    unsigned int typeMask;
    unsigned int mountMask;

    if (pEquip == 0) {
        assert_prog(equipTypeAssertEquipNotNil, equipTypeSourcePath, 100);
    }
    pEquip->mountCount = 0;
    for (eType = 0; eType < RG_EQUIP_TYPE_NUM; eType++) {
        typeMask = 0;
        for (ePos = 0; ePos < RG_EQUIP_POS_NUM; ePos++) {
            if (s_aePosToType[ePos] == eType) {
                typeMask |= 1 << ePos;
            }
        }
        mountMask = typeMask & posMask;
        if (mountMask != 0) {
            nMount = -1;
            for (i = 0; i < RG_EQUIP_POS_NUM; i++) {
                if (mountMask & (1 << i)) {
                    nMount = i;
                }
            }
            if (nMount < 0) {
                assert_prog(equipTypeAssertMountCountNotNegative, equipTypeSourcePath, 118);
            }
            RgEquipAddMount(pEquip, eType, nMount, mountMask);
        }
    }
}

/* Returns 1 when the equip has a mount of type eType, else 0. */
int RgEquipIsEquipable(RgEquip *pEquip, int eType)
{
    int mountIndex;

    if (pEquip == 0) {
        assert_prog(equipTypeAssertEquipNotNil, equipTypeSourcePath, 131);
    }
    for (mountIndex = 0; mountIndex < pEquip->mountCount; mountIndex++) {
        if (pEquip->mounts[mountIndex].type == eType) {
            return 1;
        }
    }
    return 0;
}

/*
 * Returns a bitmask of the equip types whose mount positions overlap those of
 * eType. Bit eType is set when its equip is active.
 */
int RgEquipCheckConfrict(RgEquip **ppEquip, unsigned int eType)
{
    unsigned int mask[RG_EQUIP_TYPE_NUM];
    unsigned int i;
    unsigned int j;
    unsigned int result;
    RgEquipMount *pMount;

    if (ppEquip == 0) {
        assert_prog(equipTypeAssertEquipArrayNotNil, equipTypeSourcePath, 149);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(equipTypeAssertTypeInRangeLoose, equipTypeSourcePath, 150);
    }
    if (ppEquip[eType] == 0) {
        return 0;
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        mask[i] = 0;
        if (ppEquip[i] != 0) {
            for (j = 0; j < ppEquip[i]->mountCount; j++) {
                pMount = &ppEquip[i]->mounts[j];
                if (pMount->type == i) {
                    mask[i] |= pMount->posMask;
                }
            }
        }
    }
    result = 0;
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        if (i != eType && (mask[i] & mask[eType]) != 0) {
            result = result | (1u << i);
        }
    }
    if (ppEquip[eType] != 0 && ppEquip[eType]->activeMount != 0) {
        result |= 1 << eType;
    }
    return result;
}

/* Returns the name of an equip type (left, right, back), or a placeholder. */
const char *RgEquipTypeString(int eType)
{
    if ((unsigned int)eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(equipTypeAssertTypeInRange, equipTypeSourcePath, 276);
    }
    switch (eType) {
    case RG_EQUIP_TYPE_LEFT:
        return equipTypeLeftText;
    case RG_EQUIP_TYPE_RIGHT:
        return equipTypeRightText;
    case RG_EQUIP_TYPE_BACK:
        return equipTypeBackText;
    default:
        return equipTypeUnknownText;
    }
}

/* Returns the first mount of type eType, or null. */
static RgEquipMount *_GetMountData(RgEquip *pEquip, int eType)
{
    unsigned int mountIndex;

    if (pEquip == 0) {
        assert_prog(equipTypeAssertEquipNotNil, equipTypeSourcePath, 296);
    }
    for (mountIndex = 0; mountIndex < pEquip->mountCount; mountIndex++) {
        if (pEquip->mounts[mountIndex].type == eType) {
            return &pEquip->mounts[mountIndex];
        }
    }
    return 0;
}

/*
 * Returns the mount position of eType (RG_EQUIP_POS_INVALID when not mounted).
 * For character 4, the three weapons named in pszWepName mount at back-right.
 */
int RgEquipMountPos(RgEquip *pEquip, unsigned int eType, int charID,
                    const char *pszWepName)
{
    RgEquipMount *pMount;
    int ePos;

    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(equipTypeAssertTypeInRange, equipTypeSourcePath, 311);
    }
    ePos = RG_EQUIP_POS_INVALID;
    pMount = _GetMountData(pEquip, eType);
    if (pMount != 0) {
        ePos = pMount->pos;
    }
    if (charID == 4 && pszWepName != 0) {
        if (strcmp(pszWepName, equipTypeWeaponNameBmp45vx) == 0
            || strcmp(pszWepName, equipTypeWeaponNameSmp53ag) == 0) {
            ePos = RG_EQUIP_POS_BACK_RIGHT;
        } else if (strcmp(pszWepName, equipTypeWeaponNameCb85vx) == 0) {
            ePos = RG_EQUIP_POS_BACK_RIGHT;
        }
    }
    return ePos;
}

/*
 * Returns the name of the mount position of eType. Back equips, and equips with
 * no known position, get the type name instead.
 */
const char *RgEquipMountPosString(RgEquip *pEquip, unsigned int eType,
                                  int charID, const char *pszWepName)
{
    int ePos;

    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(equipTypeAssertTypeInRange, equipTypeSourcePath, 340);
    }
    if (eType == RG_EQUIP_TYPE_BACK) {
        return equipTypeBackText;
    }
    ePos = RgEquipMountPos(pEquip, eType, charID, pszWepName);
    switch (ePos) {
    case 0:
        return equipMountLeftHandText;
    case 1:
        return equipMountLeftElbowText;
    case 2:
        return equipMountLeftShoulderText;
    case 3:
        return equipMountRightHandText;
    case 4:
        return equipMountRightElbowText;
    case 5:
        return equipMountRightShoulderText;
    case 6:
        return equipMountBackLeftText;
    case 7:
        return equipMountBackRightText;
    default:
        if (eType == RG_EQUIP_TYPE_LEFT) {
            return equipTypeLeftText;
        }
        if (eType == RG_EQUIP_TYPE_RIGHT) {
            return equipTypeRightText;
        }
        return RgEquipTypeString(eType);
    }
}



static int s_aePosToType[RG_EQUIP_POS_NUM] = { 0, 0, 0, 1, 1, 1, 2, 2 };

const char equipTypeAssertCopyPointersNotNil[32] = "pDst != NIL && pSrc != NIL";

const char equipTypeAssertMountCountBelowMax[48] = "pEquip->m_nMountNum < RG_EQUIP_MOUNT_MAX";

const char equipTypeAssertMountCountNotNegative[16] = "nMount >= 0";

const char equipTypeAssertEquipArrayNotNil[16] = "apEquip != NIL";

const char equipTypeAssertTypeInRangeLoose[56] = "RG_EQUIP_TYPE_MIN <= eType && eType < RG_EQUIP_TYPE_NUM";

const char equipTypeWeaponNameBmp45vx[8] = "BMP45VX";

const char equipTypeWeaponNameSmp53ag[8] = "SMP53AG";

const char equipTypeWeaponNameCb85vx[8] = "CB85VX";

const char equipMountLeftHandText[16] = "left hand";

const char equipMountLeftElbowText[16] = "left elbow";

const char equipMountLeftShoulderText[16] = "left shoulder";

const char equipMountRightHandText[16] = "right hand";

const char equipMountRightElbowText[16] = "right elbow";

const char equipMountRightShoulderText[16] = "right shoulder";

const char equipMountBackLeftText[16] = "back left";

const char equipMountBackRightText[24] = "back right";
