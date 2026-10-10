#include "common.h"

#include "main/xgl_cd.h"

#include "menu_1.h"

static char f_name_0_0036D6C8[21] = "data\\endou\\mapex.bin";

static char f_name_1[23] = "data\\endou\\savemap.bin";

int map_ex_text = 0;

int save_map_text = 0;

/* MenuNumberTextGet's same-TU assembly writes and returns this buffer. */

static char msg_2_00532A78[0x18];

/* MenuFileNameGet builds names into this same-TU filename buffer. */

static char fname_13[0x80];

static int sort[2][0x100];

static MenuListEntry mw_list[2][256];

char D_004C51D8[16] = "Combatant";

char D_004DABB0[8] = "Reserve";

char D_004C51C0[24] = "Registered Combatant";

char D_004C51A8[24] = "Registered Reserve";

char D_004DABA8[8] = "User";

char D_004DABA0[8] = "From";

char D_004DAB98[8] = "Target";

char D_004DAB90[8] = "To";

char D_004DABC8[8] = "HP";

char D_004DABC0[8] = "EP";

char D_004C5290[16] = "Strength";

char D_004C5280[16] = "Vitality";

char D_004C5270[16] = "Ether Attack";

char D_004C5260[16] = "Ether Defense";

char D_004C5250[16] = "Dexterity";

char D_004DABB8[8] = "Evasion";

char D_004DABF8[8] = "STR";

char D_004DABF0[8] = "VIT";

char D_004DABE8[8] = "EATK";

char D_004DABE0[8] = "EDEF";

char D_004DABD8[8] = "DEX";

char D_004DABD0[8] = "EVA";

static char *tag_name_3[9] = {
    0, D_004C51D8, D_004DABB0, D_004C51C0, D_004C51A8,
    D_004DABA8, D_004DABA0, D_004DAB98, D_004DAB90
};

static char *para_name_4[8] = {
    D_004DABC8, D_004DABC0, D_004C5290, D_004C5280,
    D_004C5270, D_004C5260, D_004C5250, D_004DABB8
};

static char *para_name2_5[8] = {
    D_004DABC8, D_004DABC0, D_004DABF8, D_004DABF0,
    D_004DABE8, D_004DABE0, D_004DABD8, D_004DABD0
};

static unsigned char opt_mnt_tbl[8][2] = {
    {0x20, 0x00}, {0x21, 0x00}, {0x22, 0x00}, {0x23, 0x00},
    {0x18, 0x00}, {0x19, 0x00}, {0x1A, 0x00}, {0x1B, 0x00}
};

static char msg_12[9] = "No data.";

unsigned char ParaDataChangeTbl[8] __attribute__((section(".data"))) = {
    6, 7, 0, 1, 2, 3, 4, 5
};

/* OptMntToPosEquip2 maps the mount code through this asm sibling first. */

int OptMntToPosEquip(int mountCode);

extern unsigned char SaveData[];

int MenuScenarioNoGet(void);

/*
 * dataEvtBoxChk is defined in src/main/data.c and xglFlagsGet in
 * src/main/xgl_flags.c; neither is published in include/main/ yet (only
 * xglFlagsInitial is), so both are forward-declared here like
 * MenuScenarioNoGet above and OptMntToPosEquip at the top of this file.
 */

int dataEvtBoxChk(int eventId);

int xglFlagsGet(int bitOffset, int bitCount);

/* MenuLoadFile is defined in src/main/window_tex_load.c. */

extern void MenuLoadFile(const char *name, void *buffer);

/* MenuLoadFile is defined in src/main/window_tex_load.c. */

typedef struct WeaponStatus WeaponStatus;

/*
 * dataWpnGet (ov01/data_unit_org_get.c, still asm) returns weapon weaponId's
 * status record (it rejects ids <= 0); MenuBulletCheck only reads the flag
 * halfword at +0x6, whose bit 0x100 marks a weapon that needs bullets.
 */

struct WeaponStatus {
    unsigned char unmodeled_00[6];
    unsigned short flags; /* +0x6: bit 0x100 - weapon needs bullets */
    unsigned short characterEquipMask; /* +0x8 */
    unsigned char unmodeled_0a[2];
    short weaponClassMask; /* +0xc */
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
    unsigned char unmodeled_08[2];
    unsigned short category; /* +0xa */
};

extern PartEquipData *dataFrmGet(int frameId);

extern PartEquipData *dataEngGet(int engineId);

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
    unsigned char unmodeled_00[0x54];
    short agwsId; /* +0x54 */
    unsigned char unmodeled_56[0x5e - 0x56];
    short weapon[3]; /* +0x5E */
    short accessoryId[3]; /* +0x64 */
    unsigned char unmodeled_6a[0xa6 - 0x6a];
    short skillId[3]; /* +0xa6 */
} UnitOrgWeapons;

extern UnitOrgWeapons *func_A191C0(int chrNo);

/*
 * dataWpnGet's own record (ov01 VA 0x00a1a3d8). MenuBulletCheck above models
 * its +0x6 flags halfword as WeaponStatus, already accepted and unable to
 * grow, so this call site keeps its own scaffold label func_A1A3D8 (matching
 * src/main/menu_shop.c) for the different +0x12 halfword summed here.
 */

typedef struct {
    unsigned char unmodeled_00[0x0e];
    unsigned char mountMask; /* +0xe */
    unsigned char unmodeled_0f[0x12 - 0x0f];
    short wagl; /* +0x12 */
} WeaponWaglData;

extern WeaponWaglData *func_A1A3D8(int weaponId);


#include "main/party.h"

struct MenuScenarioRange {
    int scenario;
    int flag;
    int minimumFlag;
};

struct MenuScenarioRangeSet {
    struct MenuScenarioRange ranges[5];
};

struct MenuScenarioTable {
    struct MenuScenarioRangeSet rangeSet;
    unsigned char unmodeled_3c[4];
};

const struct MenuScenarioTable D_004C5140[1] = {
    {{{{500, 500, 500}, {400, 400, 399}, {399, 399, 300},
       {197, 197, 100}, {56, 56, 1}}}, 0}
};

extern const char D_004DACC0[16];

extern short func_A197E8(int characterId, int specialId);

extern MenuTextEntry *func_A2C5A8(unsigned int itemId);

extern MenuTextEntry *func_A2C5F8(unsigned int itemId);

extern MenuTextEntry *func_A2C648(unsigned int itemId);

extern MenuTextEntry *func_A2C698(unsigned int itemId);

extern MenuTextEntry *func_A2C6E8(unsigned int itemId);

extern MenuTextEntry *func_A2C738(unsigned int itemId);

extern MenuTextEntry *func_A2C7D8(unsigned int itemId);

extern MenuTextEntry *func_A2C918(unsigned int itemId);

extern MenuTextEntry *func_A2C968(unsigned int itemId);

extern char *dataEvtItmNameGet(unsigned int itemId);

extern char *MenuCharNameGet(unsigned int charId);

int dataItmBoxChk(int id);

int dataWpnBoxChk(int id);

int dataBltBoxChk(int id);

int dataAccBoxChk(int id);

int dataItmBoxInc(int id);

int dataEvtBoxInc(int id);

int dataWpnBoxInc(int id);

int dataBltBoxInc(int id);

int dataAccBoxInc(int id);

void dataItmBoxDec(int id);

void dataEvtBoxDec(int id);

void dataWpnBoxDec(int id);

void dataBltBoxDec(int id);

void dataAccBoxDec(int id);

void PartyTakeAgwsOn(int id);

/* dataUnitOrgGet's per-character record fields used by this TU are at
 * +0x5A, +0x5E, +0x64, and +0xA6. */

extern PartEquipData *func_A1A548(int accessoryId);

/* MenuNumberTextGet's same-TU assembly writes and returns this buffer. */

/* MenuFileNameGet builds names into this same-TU filename buffer. */

static MenuTextEntry dumm_msg_8 = {
    D_004DACC0,
    (const unsigned char *)(D_004DACC0 + 8),
    (const unsigned char *)(D_004DACC0 + 8)
};

static MenuTextEntry dumm_9 = {
    0,
    (const unsigned char *)(D_004DACC0 + 8),
    (const unsigned char *)(D_004DACC0 + 8)
};

static MenuTextEntry dumm2_10 = {0, 0, 0};

extern const int D_004C5418[8];

unsigned char *MenuSaveDataGet(void)
{
    return SaveData + 0x10254;
}

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

int MenuScenarioNoGet(void)
{
    struct MenuScenarioRangeSet ranges = D_004C5140[0].rangeSet;
    int scenario = 0;
    int group = 0;

    do {
        int flag = ranges.ranges[group].flag;

        scenario = ranges.ranges[group].scenario;
        if (flag >= ranges.ranges[group].minimumFlag) {
            do {
                if (xglFlagsGet(flag, 1) != 0) {
                    return scenario;
                }
                flag--;
                scenario--;
            } while (flag >= ranges.ranges[group].minimumFlag);
        }
        group++;
    } while (group < 5);
    return scenario;
}

int MenuCursorKeepCheck(void)
{
    return SaveData[0x44] == 0;
}

char *MenuSegmentInfoTextGet(int segment)
{
    unsigned char *text = (unsigned char *)map_ex_text;

    while (segment >= 2) {
        while (*text++ != 0) {
        }
        segment--;
    }
    return (char *)text;
}

char *MenuSegmentMapNameGet(int segment)
{
    unsigned char *text = (unsigned char *)map_ex_text + 0x540;

    while (segment >= 2) {
        while (*text++ != 0) {
        }
        segment--;
    }

    return (char *)text;
}

char *MenuSegmentItemNameGet(int segment)
{
    unsigned char *text = (unsigned char *)map_ex_text + 0x3C0;

    while (segment >= 2) {
        while (*text++ != 0) {
        }
        segment--;
    }

    return (char *)text;
}

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

const unsigned char *MenuSaveMapNameGet(int mapId)
{
    typedef struct {
        unsigned char names[2][0x21];
        short map_id;
        unsigned char unknown[2];
    } SaveMapNameEntry;

    SaveMapNameEntry *entry = (SaveMapNameEntry *)save_map_text;
    int count = 0;
    SaveMapNameEntry *first = entry;

    do {
        if (entry->map_id == mapId) {
            return entry->names[0];
        }
        entry++;
        count++;
    } while (count < 0x25);

    return first->names[0];
}

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

int MenuMaryIdChange(int characterId)
{
    if (characterId == 20) {
        return 11;
    }
    if (characterId == 19) {
        return 12;
    }
    if (characterId < 17) {
        return characterId;
    }
    return 3;
}

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

char *MenuTagTextGet(int index)
{
    return tag_name_3[index];
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuFaceEpidGet);

char *MenuParaNameGet(int index)
{
    return para_name_4[index];
}

char *MenuParaNameGet2(int index)
{
    return para_name2_5[index];
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuCharNameGet);

MenuTextEntry *MenuTextGet(int index)
{
    unsigned int encodedIndex = (unsigned int)index;
    unsigned int itemId = encodedIndex & 0xFFFF;
    unsigned int highIndex = encodedIndex >> 16;
    MenuTextEntry *result = 0;

    if (itemId == 0) {
        return &dumm_msg_8;
    } else {
        unsigned int textType = highIndex - 1;

        switch (textType) {
        case 0:
            result = func_A2C5F8(itemId);
            break;
        case 1: {
            const char *itemName = dataEvtItmNameGet(itemId);
            const unsigned char *end = (const unsigned char *)itemName;

            dumm2_10.name = itemName;
            dumm2_10.sortName = (const unsigned char *)itemName;
            while (*end++ != '\0') {
            }
            dumm2_10.auxiliary = end;
            result = &dumm2_10;
            break;
        }
        case 2:
            result = &dumm_msg_8;
            if (MenuEquipStealMaskCheck() == 0) {
                if (itemId == 0x45) {
                    if (MenuShionMwsCheck() != 0) {
                        result = func_A2C6E8(0x45);
                    }
                } else {
                    result = func_A2C6E8(itemId);
                }
            }
            break;
        case 3:
            if (MenuEquipStealMaskCheck() != 0) {
                result = &dumm_msg_8;
            } else {
                result = func_A2C738(itemId);
            }
            break;
        case 4:
            if (MenuEquipStealMaskCheck() == 0) {
                result = func_A2C698(itemId);
            } else {
                result = &dumm_msg_8;
            }
            break;
        case 5:
        case 6:
            dumm_9.name = MenuCharNameGet(itemId);
            result = &dumm_9;
            break;
        case 7:
            result = func_A2C648(itemId);
            break;
        case 8:
            result = func_A2C5A8(itemId);
            break;
        case 9:
            result = func_A2C7D8(itemId);
            break;
        case 10:
            result = func_A2C968(itemId);
            break;
        case 11:
            result = func_A2C918(itemId);
            break;
        default:
            break;
        }
    }
    return result;
}

int MenuBoxChk(int index)
{
    unsigned int category = (unsigned int)index >> 16;
    int id = index & 0xFFFF;
    int result = 0;

    switch (category) {
    case 1:
        result = dataItmBoxChk(id);
        break;
    case 2:
        result = dataEvtBoxChk(id);
        break;
    case 3:
        result = dataWpnBoxChk(id);
        break;
    case 4:
        result = dataBltBoxChk(id);
        break;
    case 5:
        result = dataAccBoxChk(id);
        break;
    case 6:
        result = 0;
        break;
    }
    return result;
}

int MenuBoxInc(unsigned int index)
{
    unsigned int category = index >> 16;
    int id = index & 0xFFFF;
    int result = 0;

    switch (category) {
    case 1:
        dataItmBoxInc(id);
        break;
    case 2:
        dataEvtBoxInc(id);
        break;
    case 3:
        dataWpnBoxInc(id);
        break;
    case 4:
        dataBltBoxInc(id);
        break;
    case 5:
        dataAccBoxInc(id);
        break;
    case 6:
        result = -1;
        break;
    case 7:
        PartyTakeAgwsOn(id);
        break;
    }
    return result;
}

int MenuBoxDec(unsigned int index)
{
    unsigned int category = index >> 16;
    int id = index & 0xFFFF;
    int result = 0;

    switch (category) {
    case 1:
        dataItmBoxDec(id);
        break;
    case 2:
        dataEvtBoxDec(id);
        break;
    case 3:
        dataWpnBoxDec(id);
        break;
    case 4:
        dataBltBoxDec(id);
        break;
    case 5:
        dataAccBoxDec(id);
        break;
    case 6:
        result = -1;
        break;
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuBoxMoneyGet);

int MenuSkillEquipCheck(short charId, short skillId)
{
    UnitOrgWeapons *unitWeapons = func_A191C0(charId);
    int slot;

    for (slot = 0; slot < 3; slot++) {
        if (unitWeapons->skillId[slot] == skillId) {
            return slot + 1;
        }
    }
    return 0;
}

void MenuSkillEquip(short charId, short skillId, int slot)
{
    UnitOrgWeapons *unitWeapons = func_A191C0(charId);

    if (slot >= 0) {
        unitWeapons->skillId[slot] = skillId;
    } else {
        int index;

        for (index = 0; index < 3; index++) {
            if (unitWeapons->skillId[index] != 0) {
                continue;
            }
            unitWeapons->skillId[index] = skillId;
            break;
        }
    }
}

int MenuRWeaponCheck(int weaponId, long equipType)
{
    if (equipType < 0x24) {
        if (0x1F < equipType) {
            switch (weaponId) {
            case 0x5B:
                weaponId = 0x70;
                break;
            case 0x5C:
                weaponId = 0x71;
                break;
            case 0x5E:
                weaponId = 0x72;
                break;
            }
        }
    }
    return weaponId;
}

int MenuRWeaponCheck2(int weaponId)
{
    if (weaponId != 0x71) {
        if (weaponId < 0x72) {
            if (weaponId == 0x70) {
                weaponId = 0x5B;
            }
        } else if (weaponId == 0x72) {
            weaponId = 0x5E;
        }
    } else {
        weaponId = 0x5C;
    }
    return weaponId;
}

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

int WpnPosToOptMnt(int mountId, int weaponId)
{
    WeaponWaglData *weapon = 0;
    int result = 0;

    if (weaponId != 0) {
        weapon = func_A1A3D8(weaponId);
    }

    switch (mountId) {
    case 1:
        if (weapon != 0) {
            result = (weapon->mountMask & 1) != 0 ? 0x20 : 0x21;
        }
        break;
    case 2:
        if (weapon != 0) {
            result = (weapon->mountMask & 0x10) != 0 ? 0x18 : 0x19;
        }
        break;
    case 3:
        result = 0x22;
        break;
    case 4:
        result = 0x1A;
        break;
    case 5:
        result = 0x23;
        break;
    case 6:
        result = 0x1B;
        break;
    }

    return result;
}

int MenuWpnMountIdChange(int unused, int mountIndex)
{
    int mountIds[8];

    __builtin_memcpy(mountIds, D_004C5418, sizeof(mountIds));
    (void)unused;
    if (mountIndex < 2) {
        return mountIds[mountIndex + 5];
    }
    return mountIds[mountIndex - 1];
}

int PosEquipToMntPos(void)
{
    return 0;
}

const int D_004C5418[8] = {32, 24, 34, 26, 35, 27, 33, 25};

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

int MenuWeaponEquipCheck(int charId, int weaponId)
{
    unsigned short characterMask;
    int weaponClassMask;
    WeaponStatus *weapon;

    if (charId == 0) {
        return 0;
    }
    characterMask = chrEquipGet(charId);
    weaponClassMask = wpnInfoGet(charId);
    if (weaponId == 0) {
        return 0;
    }

    weapon = dataWpnGet(weaponId);
    if ((weapon->weaponClassMask & weaponClassMask) == 0) {
        return 0;
    }
    return (weapon->characterEquipMask & characterMask) != 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuWeaponEquipPosCheck);

int MenuAccessoryEquipCheck(int charId, int accessoryId, int slotIndex)
{
    unsigned short characterMask;
    UnitOrgWeapons *characterWeapons;
    PartEquipData *accessory;
    unsigned short categoryToScan;
    int index;

    if (charId == 0) {
        return 0;
    }

    characterMask = chrEquipGet(charId);
    characterWeapons = func_A191C0(charId);

    if (accessoryId == 0) {
        return 0;
    }

    accessory = func_A1A548(accessoryId);
    if ((accessory->equipMask & characterMask) == 0) {
        return 0;
    }

    if (slotIndex < 0) {
        return 1;
    }

    if (characterWeapons->accessoryId[slotIndex] != 0) {
        PartEquipData *equipped =
            func_A1A548(characterWeapons->accessoryId[slotIndex]);

        if (equipped->category == accessory->category) {
            if (equipped->category != 2) {
                return 1;
            }
            if (characterWeapons->accessoryId[slotIndex] == accessoryId) {
                return 1;
            }
        }
    }

    categoryToScan =
        charId < 0x11 ? accessory->category : 2;

    if (categoryToScan == 2) {
        for (index = 0; index < 3; index++) {
            if (characterWeapons->accessoryId[index] == accessoryId) {
                return 0;
            }
        }
    } else if (accessory->category != 2) {
        for (index = 0; index < 3; index++) {
            short equippedId = characterWeapons->accessoryId[index];

            if (equippedId != 0) {
                PartEquipData *equipped = func_A1A548(equippedId);

                if (equipped->category == accessory->category) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

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

int MenuBulletCheck2(int weaponId, int charNo)
{
    switch (charNo) {
    case 1:
    case 6:
        return 0;
    case 3:
        return func_A197E8(3, 0x3E) != 0;
    default:
        return MenuBulletCheck(weaponId, -1);
    }
}

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

int MenuAgwsPilotCheck(int agwsId)
{
    int charId = 1;

    do {
        if (func_A191C0(charId)->agwsId == agwsId) {
            return charId;
        }
        charId++;
    } while (charId < 13);
    return 0;
}

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

static void MenuSortSubType05(int *first, int *second)
{
    const unsigned char *firstName = MenuTextGet(*first)->sortName;
    const unsigned char *secondName = MenuTextGet(*second)->sortName;
    unsigned char firstChar = *firstName++;

    for (;;) {
        unsigned char secondChar;

        secondChar = *secondName++;
        if (firstChar < secondChar) {
            int savedIndex = *first;
            *first = *second;
            *second = savedIndex;
            return;
        }
        if (firstChar != secondChar || firstChar == 0) {
            return;
        }
        firstChar = *firstName++;
    }
}

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

MenuListEntry *MenuListGet(int listIndex)
{
    return mw_list[listIndex];
}

INCLUDE_ASM("asm/main/nonmatchings/menu_1", MenuFileNameGet);
