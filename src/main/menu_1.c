#include "common.h"
#include "main/xgl_cd.h"
#include "menu_1.h"

/* OptMntToPosEquip2 maps the mount code through this asm sibling first. */
int OptMntToPosEquip(int mountCode);

extern unsigned char SaveData[];

unsigned char *MenuSaveDataGet(void)
{
    return SaveData + 0x10254;
}

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

int MenuCursorKeepCheck(void)
{
    return SaveData[0x44] == 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSegmentInfoTextGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSegmentMapNameGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSegmentItemNameGet);

extern char f_name_0_0036D6C8[];
extern int map_ex_text;

/*
 * MenuLoadFile is defined in src/main/window_tex_load.c; not yet published
 * in its own header, so forward-declared here like MenuScenarioNoGet above
 * (and again below for MenuSaveMapTextLoad, which needs it after this
 * point in the file).
 */
extern int MenuLoadFile(const char *name, void *buffer);

int MenuMapExTextLoad(int work, int mode)
{
    int aligned = (work + 0xF) & ~0xF;
    int next = aligned + 0x2000;

    map_ex_text = aligned;
    if (mode == 0) {
        xglCdReadFile(f_name_0_0036D6C8, (void *)aligned, 0, 1);
    } else {
        MenuLoadFile(f_name_0_0036D6C8, (void *)aligned);
    }
    return next;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSaveMapNameGet);

/*
 * MenuLoadFile is defined in src/main/window_tex_load.c; not yet published
 * in its own header, so forward-declared here like MenuScenarioNoGet above.
 */
extern int MenuLoadFile(const char *name, void *buffer);

extern char f_name_1[];
extern int save_map_text;

int MenuSaveMapTextLoad(int work, int mode)
{
    int aligned = (work + 0xF) & ~0xF;
    int next = aligned + 0x1000;

    save_map_text = aligned;
    if (mode == 0) {
        xglCdReadFile(f_name_1, (void *)aligned, 0, 1);
    } else {
        MenuLoadFile(f_name_1, (void *)aligned);
    }
    return next;
}

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

extern char *tag_name_3[];

char *MenuTagTextGet(int index)
{
    return tag_name_3[index];
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuFaceEpidGet);

extern char *para_name_4[];

char *MenuParaNameGet(int index)
{
    return para_name_4[index];
}

extern char *para_name2_5[];

char *MenuParaNameGet2(int index)
{
    return para_name2_5[index];
}

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

extern unsigned char opt_mnt_tbl[8][2];

unsigned char PosEquipToOptMnt(int posEquip)
{
    int bit;

    for (bit = 0; bit < 8; bit++) {
        if (((posEquip & 0xFF) >> bit) & 1) {
            return opt_mnt_tbl[bit][0];
        }
    }
    return 0;
}

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

int OptMntToWpnPos(int mountCode)
{
    int weaponPos = 0;

    switch (mountCode) {
    case 32:
    case 33:
        weaponPos = 1;
        break;
    case 24:
    case 25:
        weaponPos = 2;
        break;
    case 34:
        weaponPos = 3;
        break;
    case 26:
        weaponPos = 4;
        break;
    case 35:
        weaponPos = 5;
        break;
    case 27:
        weaponPos = 6;
        break;
    }
    return weaponPos;
}

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
 * dataWpnGet (ov01/data_unit_org_get.c, still asm) returns weapon weaponId's
 * status record (it rejects ids <= 0); MenuBulletCheck only reads the flag
 * halfword at +0x6, whose bit 0x100 marks a weapon that needs bullets.
 */
struct WeaponStatus {
    unsigned char unmodeled_00[6];
    unsigned short flags; /* +0x6: bit 0x100 - weapon needs bullets */
};

extern WeaponStatus *dataWpnGet(int weaponId);

typedef struct BulletData BulletData;

/*
 * dataAttGet (same TU, still asm) indexes the bullet/attachment table by
 * ammoId; only the owning weapon id short at +0xC is read here.
 */
struct BulletData {
    unsigned char unmodeled_00[0xC];
    short weaponId; /* +0xC: the weapon this ammunition belongs to */
};

extern BulletData *dataAttGet(int ammoId);

int MenuBulletCheck(int weaponId, int ammoId)
{
    if (weaponId == 0) {
        return 0;
    }
    if ((dataWpnGet(weaponId)->flags & 0x100) == 0) {
        return 0;
    }
    if (ammoId < 0) {
        return 1;
    }
    if (ammoId == 0) {
        return 0;
    }
    return (dataAttGet(ammoId)->weaponId ^ weaponId) == 0 ? ammoId : 0;
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

/*
 * dataUnitOrgGet's own 0x180-byte per-character record (ov01 VA 0x00a191c0,
 * cross-overlay call under the scaffold label func_A191C0, matching
 * src/main/window_tex_load.c and src/main/menu_para_pt_rate_get.c; not in
 * this image). The three equipped-weapon id halfwords at +0x5E immediately
 * precede the accessory id halfwords src/main/menu_para_pt_rate_get.c models
 * at +0x64 as CharParaData's accessory[3]; only that weapon array is read
 * here.
 */
typedef struct {
    unsigned char unmodeled_00[0x5E];
    short weapon[3]; /* +0x5E */
} UnitOrgWeapons;

extern UnitOrgWeapons *func_A191C0(int chrNo);

/*
 * dataWpnGet's own record (ov01 VA 0x00a1a3d8). MenuBulletCheck above models
 * its +0x6 flags halfword as WeaponStatus, already accepted and unable to
 * grow, so this call site keeps its own scaffold label func_A1A3D8 (matching
 * src/main/menu_shop.c) for the different +0x12 halfword summed here.
 */
typedef struct {
    unsigned char unmodeled_00[0x12];
    short wagl; /* +0x12 */
} WeaponWaglData;

extern WeaponWaglData *func_A1A3D8(int weaponId);

int MenuAgwsWaglGet(int chrNo)
{
    UnitOrgWeapons *org;
    short weaponId;
    int result;
    int total;
    int slot;

    result = 0;
    total = 0;
    if (chrNo != 0) {
        org = func_A191C0(chrNo);
        for (slot = 0; slot < 3; slot++) {
            weaponId = org->weapon[slot];
            if (weaponId != 0) {
                total += func_A1A3D8(weaponId)->wagl;
            }
        }
        result = total;
    }
    return result;
}

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

extern int sort[2][0x100];

int MenuSortCheck(int listIndex)
{
    int *row = sort[listIndex];
    int *entry = row + 1;
    int value;
    int count = 0;

    if (*row != 0) {
        do {
            value = *entry;
            entry++;
            count++;
        } while (value != 0);
    }
    return count;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuSortSet);

void *MenuSortAddrGet(int listIndex)
{
    return sort[listIndex];
}

int MenuSortGet(int listIndex, int entryIndex)
{
    return sort[listIndex][entryIndex];
}

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

extern const char msg_12[];

/*
 * mw_list is defined below, right before MenuListGet, which is the only
 * other function reading it directly; forward-declared again here since
 * MenuListMake needs it earlier in the file.
 */
extern MenuListEntry mw_list[2][256];

/*
 * mode == -10 selects the shop price fill (subListMake01, which fills
 * amount from MenuBoxMoneyGet); every other value uses the plain owned-
 * count fill (subListMake00). Callers include MenuShopListChange00/01
 * (still asm).
 */
MenuListEntry *MenuListMake(int listIndex, int mode)
{
    MenuListEntry *list = mw_list[listIndex];
    MenuListEntry *entry = list;
    int *row = sort[listIndex];
    void (*fill)(MenuListEntry *, int);
    int index;
    int next;

    fill = (mode == -0xA) ? subListMake01 : subListMake00;

    index = *row;
    if (index != 0) {
        do {
            fill(entry, index);
            entry++;
            row++;
            next = *row;
            index = next;
        } while (next != 0);
    }
    entry->amount = -1;
    entry->name = msg_12;
    entry->flag = 0;
    entry[1].name = 0;
    return list;
}

extern MenuListEntry mw_list[2][256];

MenuListEntry *MenuListGet(int listIndex)
{
    return mw_list[listIndex];
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuFileNameGet);
