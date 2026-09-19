#include "common.h"
#include "menu_1.h"

/* OptMntToPosEquip2 maps the mount code through this asm sibling first. */
int OptMntToPosEquip(int mountCode);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSaveDataGet);

int MenuScenarioNoGet(void);

/*
 * dataEvtBoxChk is defined in src/main/data.c and xglFlagsGet in
 * src/main/xgl_flags.c; neither is published in include/main/ yet (only
 * xglFlagsInitial is), so both are forward-declared here like
 * MenuScenarioNoGet above and OptMntToPosEquip at the top of this file.
 */
int dataEvtBoxChk(int eventId);
int xglFlagsGet(int bitOffset, int bitCount);

int MenuShionMwsCheck(void)
{
    if (MenuScenarioNoGet() < 0xA) {
        return 1;
    }
    if (xglFlagsGet(0x3FF, 1) != 0) {
        return 1;
    }
    return dataEvtBoxChk(0xA) != 0;
}

int MenuEquipStealMaskCheck(void)
{
    return xglFlagsGet(0x3F9, 1) != 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuScenarioNoGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuCursorKeepCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSegmentInfoTextGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSegmentMapNameGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSegmentItemNameGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuMapExTextLoad);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSaveMapNameGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSaveMapTextLoad);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuCharHpCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuCharEpCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuMaryIdChange);

int MenuMainCharCheck(int charId)
{
    return (unsigned int)(charId - 1) < 7U;
}

int MenuMainAgwsCheck(int charId)
{
    unsigned int isMainAgws;

    isMainAgws = (unsigned int)(charId - 0x13) < 2U;
    if (isMainAgws) {
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuNumberTextGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuTagTextGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuFaceEpidGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuParaNameGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuParaNameGet2);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuCharNameGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuTextGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuBoxChk);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuBoxInc);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuBoxDec);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuBoxMoneyGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSkillEquipCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSkillEquip);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuRWeaponCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuRWeaponCheck2);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", PosEquipToOptMnt);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", OptMntToPosEquip);

int OptMntToPosEquip2(int mountCode)
{
    int posEquip = OptMntToPosEquip(mountCode & 0xFF);

    switch (posEquip) {
    case 1:
        posEquip = 3;
        break;
    case 2:
        posEquip = 3;
        break;
    case 16:
    case 32:
        posEquip = 0x30;
        break;
    }

    return posEquip;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", OptMntToWpnPos);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", WpnPosToOptMnt);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuWpnMountIdChange);

int PosEquipToMntPos(void)
{
    return 0;
}

static unsigned short chrEquipGet(int charId)
{
    if (charId < 0x11) {
        return 0x8000 >> (charId - 1);
    }
    return 0x8000 >> (charId - 0x11);
}

static int wpnInfoGet(int weaponType)
{
    return (weaponType < 0x11) ? 0x4000 : 0x2000;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuWeaponEquipCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuWeaponEquipPosCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuAccessoryEquipCheck);

typedef struct WeaponStatus WeaponStatus;

/*
 * dataWpnGet (ov01/data_unit_org_get.c, still asm) returns the currently
 * equipped weapon's status record; MenuBulletCheck only reads the flag
 * halfword at +0x6, whose bit 0x100 marks a weapon that needs bullets.
 */
struct WeaponStatus {
    unsigned char unmodeled_00[6];
    unsigned short flags; /* +0x6: bit 0x100 - weapon needs bullets */
};

extern WeaponStatus *dataWpnGet(void);

typedef struct BulletData BulletData;

/*
 * dataAttGet (same TU, still asm) indexes the bullet/attachment table by
 * ammoId; only the bullet-type short at +0xC is read here.
 */
struct BulletData {
    unsigned char unmodeled_00[0xC];
    short bulletType; /* +0xC */
};

extern BulletData *dataAttGet(int ammoId);

int MenuBulletCheck(int bulletType, int ammoId)
{
    if (bulletType == 0) {
        return 0;
    }
    if ((dataWpnGet()->flags & 0x100) == 0) {
        return 0;
    }
    if (ammoId < 0) {
        return 1;
    }
    if (ammoId == 0) {
        return 0;
    }
    return (dataAttGet(ammoId)->bulletType ^ bulletType) == 0 ? ammoId : 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuBulletCheck2);

typedef struct PartEquipData PartEquipData;

/*
 * dataFrmGet and dataEngGet (ov01/data_unit_org_get.c, still asm) return
 * per-entry AGWS part records that share this layout as far as
 * MenuFrameEquipCheck/MenuEngineEquipCheck read it: a halfword at +0x6
 * bitmask of the characters that can equip the part, tested against
 * chrEquipGet's per-character bit.
 */
struct PartEquipData {
    unsigned char unmodeled_00[6];
    unsigned short equipMask; /* +0x6 */
};

extern PartEquipData *dataFrmGet(int frameId);
extern PartEquipData *dataEngGet(int engineId);

int MenuFrameEquipCheck(int charId, int frameId)
{
    if (frameId == 0) {
        return 0;
    }
    return (dataFrmGet(frameId)->equipMask & chrEquipGet(charId)) != 0;
}

int MenuEngineEquipCheck(int charId, int engineId)
{
    if (engineId == 0) {
        return 0;
    }
    return (dataEngGet(engineId)->equipMask & chrEquipGet(charId)) != 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuAgwsPilotCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuAgwsWaglGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSortSubType05);

static void MenuSortSubType04(int *first, int *second)
{
    const unsigned char *firstName = MenuTextGet(*first)->sortName;
    const unsigned char *secondName = MenuTextGet(*second)->sortName;
    unsigned char firstChar;
    unsigned char secondChar;

    for (;;) {
        secondChar = *secondName++;
        firstChar = *firstName++;
        if (secondChar < firstChar) {
            int savedIndex = *first;
            *first = *second;
            *second = savedIndex;
            return;
        }
        if (firstChar != secondChar || firstChar == 0) {
            return;
        }
    }
}

static void MenuSortSubType03(int *first, int *second)
{
    int firstCount = MenuBoxChk(*first);

    if (MenuBoxChk(*second) < firstCount) {
        int savedIndex = *first;
        *first = *second;
        *second = savedIndex;
    }
}

static void MenuSortSubType02(int *first, int *second)
{
    int firstCount = MenuBoxChk(*first);

    if (firstCount < MenuBoxChk(*second)) {
        int savedIndex = *first;
        *first = *second;
        *second = savedIndex;
    }
}

static void MenuSortSubType01(unsigned int *first, unsigned int *second)
{
    unsigned int firstValue = *first;
    unsigned int secondValue = *second;

    if (firstValue < secondValue) {
        *first = secondValue;
        *second = firstValue;
    }
}

static void MenuSortSubType00(unsigned int *first, unsigned int *second)
{
    unsigned int firstValue = *first;
    unsigned int secondValue = *second;

    if (secondValue < firstValue) {
        *first = secondValue;
        *second = firstValue;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSortChange);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSortCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSortSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSortAddrGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSortGet);

int MenuIdChange(short id, int highPart)
{
    return id + (highPart << 16);
}

void subListMake00(MenuListEntry *entry, int index)
{
    entry->name = MenuTextGet(index)->name;
    entry->amount = MenuBoxChk(index);
    entry->flag = 0;
}

void subListMake01(MenuListEntry *entry, int index)
{
    entry->name = MenuTextGet(index)->name;
    entry->amount = MenuBoxMoneyGet(index, 0);
    entry->flag = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuListMake);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuListGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuFileNameGet);
