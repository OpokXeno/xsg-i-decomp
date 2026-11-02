#include "common.h"
#include "shared.h"
#include "sdv.h"
#include "main/xgl_cd.h"
#include "main/menu_1.h"
#include "menu_1.h"
#include "main/party.h"
#include "main/data.h"
#include "main/res_get_path.h"
#include "main/xgl_mc.h"

/* Event flags tested through xglFlagsGet. */
#define MENU_FLAG_SHION_MWS 0x3FF
#define MENU_FLAG_EQUIP_STEAL_MASK 0x3F9

/* Scenario numbers that gate menu behaviour (MenuScenarioNoGet). */
#define MENU_SCENARIO_RANGE_COUNT 5
#define MENU_SCENARIO_MWS_GATE 10
#define MENU_SCENARIO_ZIGGY_NAMED 108

/* The MWS key event item and Shion's MWS weapon (weapon range index 2). */
#define MENU_EVENT_ITEM_SHION_MWS 10
#define MENU_WEAPON_SHION_MWS 0x45

/* SaveData: option byte read by MenuCursorKeepCheck (menu_system sets it) and
 * the five shop discount levels, one per price rate class. */
#define SAVE_MENU_CURSOR_KEEP 0x44
#define SAVE_MENU_DISCOUNT_LEVELS 0x102DC

/* mapex.bin: info texts, then item names at +0x3C0, then map names at +0x540. */
#define MAPEX_ITEM_NAME_OFFSET 0x3C0
#define MAPEX_MAP_NAME_OFFSET 0x540
#define MAPEX_BUFFER_SIZE 0x2000
#define SAVEMAP_BUFFER_SIZE 0x1000
#define SAVEMAP_ENTRY_COUNT 0x25
#define SAVEMAP_NAME_SIZE 0x21

/* MenuNumberTextGet output: at most ten digits plus the terminator and fill. */
#define MENU_NUMBER_TEXT_SIZE 0x18
#define MENU_NUMBER_MAX_DIGITS 10

/* Face tables: the first 12 characters have both sizes, AGWS faces start at
 * MENU_CHAR_AGWS_FIRST; the small table has 16 character entries. */
#define MENU_FACE_CHAR_COUNT 12
#define MENU_FACE_SMALL_COUNT 16
#define MENU_FACE_KOSMOS_SMALL 0x401
#define MENU_FACE_KOSMOS_LARGE 0xC01

/* Name tables. */
#define MENU_CHAR_NAME_COUNT 30
#define MENU_TAG_COUNT 9
#define MENU_PARA_COUNT 8

/* Equipment slots per character record. */
/* WeaponStatus.flags: the weapon is a gun that consumes bullets. */
#define WEAPON_FLAG_NEEDS_BULLETS 0x100

#define MENU_WEAPON_SLOT_COUNT 3
#define MENU_ACCESSORY_SLOT_COUNT 3
#define MENU_SKILL_SLOT_COUNT 3
#define MENU_ETHER_SLOT_COUNT 12

/* Total equipWeight the equipped weapons may add up to. */
#define MENU_EQUIP_WEIGHT_LIMIT 3
#define MENU_EQUIP_WEIGHT_LIMIT_AG_05 2

/*
 * Mount codes of opt_mnt_tbl: the code of equip-mask bit N of a weapon record
 * (bits 0-3 are 0x20-0x23, bits 4-7 are 0x18-0x1B).
 */
#define MENU_MOUNT_BIT_COUNT 8
/* Equip-mask (mountMask) bits; the bit pairs 0/1 and 4/5 are alternatives. */
enum MenuMountMask {
    MENU_MOUNT_MASK_BIT0 = 0x1,
    MENU_MOUNT_MASK_BIT1 = 0x2,
    MENU_MOUNT_MASK_BIT3 = 0x8,
    MENU_MOUNT_MASK_BIT4 = 0x10,
    MENU_MOUNT_MASK_BIT5 = 0x20,
    MENU_MOUNT_MASK_BIT7 = 0x80,
    MENU_MOUNT_MASK_LOW = 0x0F,
    MENU_MOUNT_MASK_HIGH = 0xF0,
    MENU_MOUNT_MASK_PAIR_LOW = MENU_MOUNT_MASK_BIT0 | MENU_MOUNT_MASK_BIT1,
    MENU_MOUNT_MASK_PAIR_HIGH = MENU_MOUNT_MASK_BIT4 | MENU_MOUNT_MASK_BIT5
};

enum MenuMountCode {
    MENU_MOUNT_CODE_BIT0 = 0x20,
    MENU_MOUNT_CODE_BIT1 = 0x21,
    MENU_MOUNT_CODE_BIT2 = 0x22,
    MENU_MOUNT_CODE_BIT3 = 0x23,
    MENU_MOUNT_CODE_BIT4 = 0x18,
    MENU_MOUNT_CODE_BIT5 = 0x19,
    MENU_MOUNT_CODE_BIT6 = 0x1A,
    MENU_MOUNT_CODE_BIT7 = 0x1B
};

/* Price rate classes: rate[] holds one multiplier per class. */
#define MENU_PRICE_RATE_COUNT 5
#define MENU_RATE_ITEM 0
#define MENU_RATE_AGWS 1
#define MENU_RATE_AGWS_WEAPON 2
#define MENU_RATE_AGWS_ACCESSORY 3
#define MENU_RATE_CHARACTER_GEAR 4

/* Id ranges. Event items 53-55 are the purchasable ones (boxPriceTable). */
#define MENU_ITEM_ID_END 73
#define MENU_EVENT_ITEM_LAST 254
#define MENU_EVENT_ITEM_SHOP_FIRST 53
#define MENU_EVENT_ITEM_SHOP_LAST 55
#define MENU_EVENT_ITEM_SHOP_COUNT 3
#define MENU_WEAPON_ID_COUNT 115
#define MENU_BULLET_ID_CHARACTER_LAST 63
#define MENU_BULLET_ID_AGWS_FIRST 64
#define MENU_BULLET_ID_LAST 178
#define MENU_ACCESSORY_AGWS_FIRST 129
#define MENU_ACCESSORY_ID_END 181
#define MENU_SKILL_ID_END 91
#define MENU_ETHER_ID_END 80

/* MenuSortSet buffers: two lists of up to 255 entries plus the terminator. */
#define MENU_LIST_COUNT 2
#define MENU_LIST_CAPACITY 0x100
#define MENU_LIST_NO_DATA_AMOUNT (-1)
#define MENU_LIST_MODE_SHOP (-0xA)

/* weaponRanges rows: one per character id - 1, then the AGWS and all-character
 * ranges that MenuSortSet picks for the AGWS / character filter. */
#define MENU_WEAPON_RANGE_COUNT 9
#define MENU_WEAPON_RANGE_AGWS 7
#define MENU_WEAPON_RANGE_ALL_CHARACTERS 8

/* MenuFileNameGet: resource kinds, extension, base path and motion name
 * indices, and model id layout (kind in the top nibble). */
#define MENU_FILE_NAME_SIZE 0x80
#define MENU_FILE_MODEL_LEX 0
#define MENU_FILE_MODEL_XTX 1
#define MENU_FILE_MODEL_JNT 2
#define MENU_FILE_MOTION 3
#define MENU_FILE_MODEL2_LEX 4
#define MENU_FILE_MODEL2_JNT 5
#define MENU_EXT_LEX 0
#define MENU_EXT_JNT 2
#define MENU_EXT_FPK 3
#define MENU_EXT_BIN 4
#define MENU_PATH_MOTION 0
#define MENU_PATH_YAMAMOTO_MOT 1
#define MENU_PATH_ROBO_ETC 2
#define MENU_CHARMOT_MEQP 0
#define MENU_CHARMOT_UKN_G 1
#define MENU_CHARMOT_AGWS 2
#define MENU_MODEL_KIND_CHARACTER 0
#define MENU_MODEL_KIND_AGWS 2
#define MENU_MODEL_KIND_MONSTER 4
#define MENU_MODEL_ID_UKN_AGWS 0x2201
#define MENU_MODEL_ID_MONSTER_BASE 0x4000
#define MENU_MODEL_ID_MONSTER_ALIAS 0x4037
#define MENU_MONSTER_ALIAS_MOTION_ID 114
#define MENU_MODEL2_ID_OFFSET 0x10000

typedef struct MenuNumberDigits {
    const char *text[10];
} MenuNumberDigits;

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

typedef struct BulletData BulletData;

/*
 * dataAttGet (same TU, still asm) indexes the bullet/attachment table by
 * ammoId; only the owning weapon id short at +0xC is read here.
 */
struct BulletData {
    unsigned char unmodeled_00[0xC];
    short weaponId; /* +0xC: the weapon this ammunition belongs to */
};

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
    unsigned char unmodeled_056[0x5a - 0x56];
    signed char weaponMount[MENU_WEAPON_SLOT_COUNT]; /* +0x5a */
    unsigned char unmodeled_05d[0x5e - 0x5d];
    short weapon[MENU_WEAPON_SLOT_COUNT]; /* +0x5E */
    short accessoryId[MENU_ACCESSORY_SLOT_COUNT]; /* +0x64 */
    unsigned char unmodeled_06a[0x8e - 0x6a];
    short etherId[MENU_ETHER_SLOT_COUNT]; /* +0x8e */
    short skillId[MENU_SKILL_SLOT_COUNT]; /* +0xa6 */
} UnitOrgWeapons;

/*
 * dataWpnGet's own record (ov01 VA 0x00a1a3d8). MenuBulletCheck above models
 * its +0x6 flags halfword as WeaponStatus, already accepted and unable to
 * grow, so this call site keeps its own scaffold label func_A1A3D8 (matching
 * src/main/menu_shop.c) for the battle weapon price record it returns.
 */

struct MenuScenarioRange {
    int scenario;
    int flag;
    int minimumFlag;
};

struct MenuScenarioRangeSet {
    struct MenuScenarioRange ranges[MENU_SCENARIO_RANGE_COUNT];
};

struct MenuScenarioTable {
    struct MenuScenarioRangeSet rangeSet;
    unsigned char unmodeled_3c[4];
};

extern const MenuNumberDigits numberDigitTexts;
extern const char numberEmptyText[];
extern const unsigned short largeFaceTable[MENU_FACE_CHAR_COUNT];
extern const unsigned short largeAgwsFaceTable[MENU_FACE_CHAR_COUNT];
extern const unsigned short smallFaceTable[MENU_FACE_SMALL_COUNT];
extern const unsigned short smallAgwsFaceTable[MENU_FACE_CHAR_COUNT];
static void (*f_11[])();
static const char *f_base_14[];
static const char *f_exten_15[];
static const char *f_ukn_name_16[];
static const char *f_char_mot_17[];
static char *f_mons_mot_18[];
extern char noneName[5];
extern const unsigned char blankText[1];

extern const int wpnMountIdTable[8];

extern const int boxPriceTable[MENU_EVENT_ITEM_SHOP_COUNT];
extern const char MenuDigit0Text[8];
extern const char MenuDigit1Text[8];
extern const char MenuDigit2Text[8];
extern const char MenuDigit3Text[8];
extern const char MenuDigit4Text[8];
extern const char MenuDigit5Text[8];
extern const char MenuDigit6Text[8];
extern const char MenuDigit7Text[8];
extern const char MenuDigit8Text[8];
extern const char MenuDigit9Text[8];
extern const char registeredReserveText[19];
extern const char registeredCombatantText[21];
extern const char combatantText[10];
extern const char dexterityText[10];
extern const char etherDefenseText[14];
extern const char etherAttackText[13];
extern const char vitalityText[9];
extern const char strengthText[9];
extern const char vx20000Name[9];
extern const char vx10000Name[9];
extern const char catalinaText[9];
extern const char cassiriaText[11];
extern const char blankNameText[11];
extern const char ziggurat8Name[11];
extern char toText[8];
extern char targetText[8];
extern char fromText[8];
extern char userText[8];
extern char menu1ReserveText[8];
extern char evasionText[8];
extern char epText[8];
extern char menu1HpText[8];
extern char evaText[8];
extern char dexText[8];
extern char edefText[8];
extern char eatkText[8];
extern char vitText[8];
extern char strText[8];
extern char bunnieName[7];
extern char ag06Name[6];
extern char ag05Name[6];
extern char ag04Name[6];
extern char ag03Name[6];
extern char ag02Name[6];
extern char ag01Name[6];
extern char vx4000Name[8];
extern char vx05Name[6];
extern char vx9000Name[8];
extern char vx7000Name[8];
extern char bic16Name[7];
extern char bic15Name[7];
extern char bic14Name[7];
extern char bic13Name[7];
extern char soldierName[8];
extern char maryName[5];
extern char virgilName[7];
extern char jrName[4];
extern char momoName[5];
extern char menu1ZiggyName[6];
extern char shionName[6];
extern char kosMosName[8];
extern char chaosName[6];
extern const char binExtension[5];
extern const char menu1FpkExtension[5];
extern const char jntExtension[5];
extern const char xtxExtension[5];
extern const char lexExtension[5];
extern const char ukunBName[6];
extern const char ukunSName[6];
extern const char uknGName[6];
typedef struct SaveMapNameEntry {
    unsigned char names[2][SAVEMAP_NAME_SIZE];
    short map_id;
    unsigned char unknown[2];
} SaveMapNameEntry;

extern unsigned char *map_ex_text;
extern SaveMapNameEntry *save_map_text;
extern const char roboEtcPath[15];
extern const char yamamotoMotPath[19];
extern const char motionPath[13];
extern const char menuAgwsName[9];
extern const char mtnMeqpName[9];
extern const char gunoMotionName[9];

/* OptMntToPosEquip2 maps the mount code through this asm sibling first. */
int OptMntToPosEquip(int mountCode);

int xglFlagsGet(int bitOffset, int bitCount);

/* MenuLoadFile is defined in src/main/window_tex_load.c. */
extern void MenuLoadFile(const char *name, void *buffer);

extern WeaponStatus *dataWpnGet(int weaponId);
extern BulletData *dataAttGet(int ammoId);
extern BattleFramePriceRecord *dataFrmGet(int frameId);
extern BattleEnginePriceRecord *dataEngGet(int engineId);
extern UnitOrgWeapons *func_A191C0(int chrNo);
extern BattleWeaponPriceRecord *func_A1A3D8(int weaponId);
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
const char *MenuCharNameGet(unsigned int charId);
/* dataUnitOrgGet's per-character record fields used by this TU are at
 * +0x5A, +0x5E, +0x64, and +0xA6. */
extern BattleAccessoryPriceRecord *func_A1A548(int accessoryId);

extern BattleItemPriceRecord *func_A1A4E8(int itemId);
extern BattleBulletPriceRecord *func_A1A428(int bulletId);
extern BattleFramePriceRecord *func_A1A598(int frameId);
extern BattleEnginePriceRecord *func_A1A5E8(int engineId);
extern void *memset(void *destination, int value, unsigned int bytes);
static void MenuSortSubType00(unsigned int *first, unsigned int *second);
static void MenuSortSubType01(unsigned int *first, unsigned int *second);
static void MenuSortSubType02(int *first, int *second);
static void MenuSortSubType03(int *first, int *second);
static void MenuSortSubType04(int *first, int *second);
static void MenuSortSubType05(int *first, int *second);
extern struct MenuCharParameters *func_00A11108(int charId, int *attack, int *defense);

const struct MenuScenarioTable scenarioRangeTable[1] = {
    {{{{500, 500, 500}, {400, 400, 399}, {399, 399, 300},
       {197, 197, 100}, {56, 56, 1}}}, 0}
};

const MenuNumberDigits numberDigitTexts = {{ MenuDigit0Text, MenuDigit1Text, MenuDigit2Text, MenuDigit3Text, MenuDigit4Text, MenuDigit5Text, MenuDigit6Text, MenuDigit7Text, MenuDigit8Text, MenuDigit9Text }};

const char registeredReserveText[19] = "Registered Reserve";

const char registeredCombatantText[21] = "Registered Combatant";

const char combatantText[10] = "Combatant";

static char f_name_0_0036D6C8[21] = "data\\endou\\mapex.bin";

static char f_name_1[23] = "data\\endou\\savemap.bin";

static const char *tag_name_3[MENU_TAG_COUNT] = {
    0, combatantText, menu1ReserveText, registeredCombatantText, registeredReserveText,
    userText, fromText, targetText, toText
};

static const char *para_name_4[MENU_PARA_COUNT] = {
    menu1HpText, epText, strengthText, vitalityText,
    etherAttackText, etherDefenseText, dexterityText, evasionText
};

static const char *para_name2_5[MENU_PARA_COUNT] = {
    menu1HpText, epText, strText, vitText,
    eatkText, edefText, dexText, evaText
};

static const char *char_name_6[MENU_CHAR_NAME_COUNT] = { noneName, chaosName, kosMosName, shionName, blankNameText, menu1ZiggyName, momoName, jrName, cassiriaText, catalinaText, virgilName, maryName, soldierName, bic13Name, bic14Name, bic15Name, bic16Name, vx10000Name, vx7000Name, vx9000Name, vx20000Name, vx05Name, vx4000Name, ag01Name, ag02Name, ag03Name, ag04Name, ag05Name, ag06Name, bunnieName };

static const char *char_name2_7[] = { ziggurat8Name };

static MenuTextEntry dumm_msg_8 = {
    noneName,
    (const char *)blankText,
    blankText
};

static MenuTextEntry dumm_9 = {
    0,
    (const char *)blankText,
    blankText
};

static MenuTextEntry dumm2_10 = {0, 0, 0};

static unsigned short opt_mnt_tbl[8] = {
    0x20, 0x21, 0x22, 0x23, 0x18, 0x19, 0x1A, 0x1B
};

/* Partial callback type: parameter signedness belongs to each accepted comparator. */
static void (*f_11[6])()  = { MenuSortSubType00, MenuSortSubType01, MenuSortSubType02, MenuSortSubType03, MenuSortSubType04, MenuSortSubType05 };

static char msg_12[9] = "No data.";

static const char * f_base_14[3] = { motionPath, yamamotoMotPath, roboEtcPath };

static const char * f_exten_15[5] = { lexExtension, xtxExtension, jntExtension, menu1FpkExtension, binExtension };

static const char * f_ukn_name_16[] = { ukunSName, ukunBName };

static const char * f_char_mot_17[3] = { mtnMeqpName, uknGName, menuAgwsName };

/* The monster number digits are rewritten in place through this pointer
 * (case 4 of the motion-name builder), so the view is writable. */
static char *f_mons_mot_18[] = { (char *)gunoMotionName };

unsigned char ParaDataChangeTbl[] = {
    6, 7, 0, 1, 2, 3, 4, 5
};

/* MenuNumberTextGet's same-TU assembly writes and returns this buffer. */
static char msg_2_00532A78[MENU_NUMBER_TEXT_SIZE];

/* MenuFileNameGet builds names into this same-TU filename buffer. */
static char fname_13[MENU_FILE_NAME_SIZE];

static int sort[MENU_LIST_COUNT][MENU_LIST_CAPACITY];

static MenuListEntry mw_list[MENU_LIST_COUNT][MENU_LIST_CAPACITY];

unsigned char *MenuSaveDataGet(void)
{
    return SaveData + SAVE_MENU_SAVE_DATA;
}

int MenuShionMwsCheck(void)
{
    if (MenuScenarioNoGet() < MENU_SCENARIO_MWS_GATE) {
        return 1;
    }
    if (xglFlagsGet(MENU_FLAG_SHION_MWS, 1) != 0) {
        return 1;
    }
    return dataEvtBoxChk(MENU_EVENT_ITEM_SHION_MWS) != 0;
}

int MenuEquipStealMaskCheck(void)
{
    return xglFlagsGet(MENU_FLAG_EQUIP_STEAL_MASK, 1) != 0;
}

int MenuScenarioNoGet(void)
{
    struct MenuScenarioRangeSet ranges = scenarioRangeTable[0].rangeSet;
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
    } while (group < MENU_SCENARIO_RANGE_COUNT);
    return scenario;
}

int MenuCursorKeepCheck(void)
{
    return SaveData[SAVE_MENU_CURSOR_KEEP] == 0;
}

char *MenuSegmentInfoTextGet(int segment)
{
    unsigned char *text = map_ex_text;

    while (segment >= 2) {
        while (*text++ != 0) {
        }
        segment--;
    }
    return (char *)text;
}

char *MenuSegmentMapNameGet(int segment)
{
    unsigned char *text = map_ex_text + MAPEX_MAP_NAME_OFFSET;

    while (segment >= 2) {
        while (*text++ != 0) {
        }
        segment--;
    }

    return (char *)text;
}

char *MenuSegmentItemNameGet(int segment)
{
    unsigned char *text = map_ex_text + MAPEX_ITEM_NAME_OFFSET;

    while (segment >= 2) {
        while (*text++ != 0) {
        }
        segment--;
    }

    return (char *)text;
}

unsigned char *MenuMapExTextLoad(unsigned char *work, int mode)
{
    unsigned char *aligned = (unsigned char *)(((u32)work + 0xF) & ~0xF);
    unsigned char *next = aligned + MAPEX_BUFFER_SIZE;

    map_ex_text = aligned;
    if (mode == 0) {
        xglCdReadFile(f_name_0_0036D6C8, aligned, 0, 1);
    } else {
        MenuLoadFile(f_name_0_0036D6C8, aligned);
    }
    return next;
}

const unsigned char *MenuSaveMapNameGet(int mapId)
{
    SaveMapNameEntry *entry = save_map_text;
    int count = 0;
    SaveMapNameEntry *first = entry;

    do {
        if (entry->map_id == mapId) {
            return entry->names[0];
        }
        entry++;
        count++;
    } while (count < SAVEMAP_ENTRY_COUNT);

    return first->names[0];
}

void *MenuSaveMapTextLoad(void *work, int mode)
{
    unsigned char *aligned = (unsigned char *)(((u32)work + 0xF) & ~0xF);
    unsigned char *next = aligned + SAVEMAP_BUFFER_SIZE;

    save_map_text = (SaveMapNameEntry *)aligned;
    if (mode == 0) {
        xglCdReadFile(f_name_1, aligned, 0, 1);
    } else {
        MenuLoadFile(f_name_1, aligned);
    }
    return next;
}

int MenuCharHpCheck(int charId)
{
    int attack[4];
    int defense[4];
    int party[16];
    int index;
    int count;
    struct MenuCharParameters *parameters;

    if (charId == MENU_CHAR_NONE) {
        return 0;
    }
    if (charId < 0) {
        count = PartyAllPartyGet(party, 0);
        for (index = 0; index < count; index++) {
            parameters = func_00A11108(party[index], attack, defense);
            if (parameters->hp != parameters->maxHp) {
                break;
            }
        }
        if (index == count) {
            return 1;
        }
    } else {
        func_A191C0(charId);
        parameters = func_00A11108(charId, attack, defense);
        if (parameters->hp == parameters->maxHp) {
            return 1;
        }
    }
    return 0;
}

int MenuCharEpCheck(int charId)
{
    int attack[4];
    int defense[4];
    int party[16];
    int index;
    int count;
    struct MenuCharParameters *parameters;

    if (charId == MENU_CHAR_NONE) {
        return 0;
    }
    if (charId < 0) {
        count = PartyAllPartyGet(party, 0);
        for (index = 0; index < count; index++) {
            parameters = func_00A11108(party[index], attack, defense);
            if (parameters->ep != parameters->maxEp) {
                break;
            }
        }
        if (index == count) {
            return 1;
        }
    } else {
        parameters = func_00A11108(charId, attack, defense);
        if (parameters->ep == parameters->maxEp) {
            return 1;
        }
    }
    return 0;
}

int MenuMaryIdChange(int characterId)
{
    if (characterId == MENU_CHAR_VX_20000) {
        return MENU_CHAR_MARY;
    }
    if (characterId == MENU_CHAR_VX_9000) {
        return MENU_CHAR_SOLDIER;
    }
    if (characterId < MENU_CHAR_AGWS_FIRST) {
        return characterId;
    }
    return MENU_CHAR_SHION;
}

int MenuMainCharCheck(int charId)
{
    return (unsigned int)(charId - MENU_CHAR_CHAOS) < 7U;
}

int MenuMainAgwsCheck(int charId)
{
    unsigned int isMainAgws;

    isMainAgws = (unsigned int)(charId - MENU_CHAR_VX_9000) < 2U;
    if (isMainAgws) {
        return 0;
    }
    return 1;
}

char *MenuNumberTextGet(long long number, int digitsBeforeUnits, int leadingZeroes)
{
    MenuNumberDigits digits = numberDigitTexts;
    long long divisor;
    long long remaining;
    int digitCount;
    int places;
    int started;

    msg_2_00532A78[0] = numberEmptyText[0];
    if (digitsBeforeUnits < 0) {
        divisor = 10;
        digitCount = 1;
        while (number / divisor != 0) {
            divisor *= 10;
            digitCount++;
        }
        digitsBeforeUnits = digitCount;
    }
    if (digitsBeforeUnits < MENU_NUMBER_MAX_DIGITS + 1) {
        places = digitsBeforeUnits;
    } else {
        places = MENU_NUMBER_MAX_DIGITS;
    }
    started = leadingZeroes == 1;
    remaining = number;
    while (places > 0) {
        int exponent;
        divisor = 10;
        for (exponent = 1; exponent < places; exponent++) {
            divisor *= 10;
        }
        if (started || remaining / divisor != 0) {
            started = 1;
            strcat(msg_2_00532A78, digits.text[(int)(remaining / divisor)]);
        }
        places--;
        remaining %= divisor;
    }
    strcat(msg_2_00532A78, digits.text[(int)(remaining % 10)]);
    return msg_2_00532A78;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char numberEmptyText[1] = "";

char toText[8] = "To";

char targetText[8] = "Target";

char fromText[8] = "From";

char userText[8] = "User";

char menu1ReserveText[8] = "Reserve";

char evasionText[8] = "Evasion";

char epText[8] = "EP";

char menu1HpText[8] = "HP";

char evaText[8] = "EVA";

char dexText[8] = "DEX";

char edefText[8] = "EDEF";

char eatkText[8] = "EATK";

char vitText[8] = "VIT";

char strText[8] = "STR";

char bunnieName[7] = "Bunnie";

char ag06Name[6] = "AG-06";

char ag05Name[6] = "AG-05";

char ag04Name[6] = "AG-04";

char ag03Name[6] = "AG-03";

char ag02Name[6] = "AG-02";

char ag01Name[6] = "AG-01";

char vx4000Name[8] = "VX-4000";

char vx05Name[6] = "VX-05";

char vx9000Name[8] = "VX-9000";

char vx7000Name[8] = "VX-7000";

char bic16Name[7] = "BIC-16";

char bic15Name[7] = "BIC-15";

char bic14Name[7] = "BIC-14";

char bic13Name[7] = "BIC-13";

char soldierName[8] = "Soldier";

char maryName[5] = "Mary";

char virgilName[7] = "Virgil";

char jrName[4] = "Jr.";

char momoName[5] = "MOMO";

char menu1ZiggyName[6] = "Ziggy";

char shionName[6] = "Shion";

char kosMosName[8] = "KOS-MOS";

char chaosName[6] = "chaos";

char noneName[5] = "None";

const unsigned char blankText[1] = "";

const char binExtension[5] = ".bin";

const char menu1FpkExtension[5] = ".fpk";

const char jntExtension[5] = ".jnt";

const char xtxExtension[5] = ".xtx";

const char lexExtension[5] = ".lex";

const char ukunBName[6] = "ukunB";

const char ukunSName[6] = "ukunS";

const char uknGName[6] = "ukn_g";

unsigned char *map_ex_text = 0;

SaveMapNameEntry *save_map_text = 0;

const char *MenuTagTextGet(int index)
{
    return tag_name_3[index];
}

unsigned short MenuFaceEpidGet(int charId, int smallPortrait)
{
    unsigned short largeFaces[MENU_FACE_CHAR_COUNT];
    unsigned short largeAgwsFaces[MENU_FACE_CHAR_COUNT];
    unsigned short smallFaces[MENU_FACE_SMALL_COUNT];
    unsigned short smallAgwsFaces[MENU_FACE_CHAR_COUNT];

    if (charId == MENU_CHAR_KOSMOS) {
        unsigned short epid = MENU_FACE_KOSMOS_SMALL;
        if (smallPortrait == 0) {
            epid = MENU_FACE_KOSMOS_LARGE;
        }
        return epid;
    }
    if (smallPortrait == 0) {
        if ((unsigned int)(charId - MENU_CHAR_CHAOS) < MENU_FACE_CHAR_COUNT) {
            __builtin_memcpy(largeFaces, largeFaceTable, sizeof(largeFaces));
            return largeFaces[charId - MENU_CHAR_CHAOS];
        }
        if ((unsigned int)(charId - MENU_CHAR_AGWS_FIRST) < MENU_FACE_CHAR_COUNT) {
            __builtin_memcpy(largeAgwsFaces, largeAgwsFaceTable, sizeof(largeAgwsFaces));
            return largeAgwsFaces[charId - MENU_CHAR_AGWS_FIRST];
        }
    } else {
        if ((unsigned int)(charId - MENU_CHAR_CHAOS) < MENU_FACE_CHAR_COUNT) {
            __builtin_memcpy(smallFaces, smallFaceTable, sizeof(smallFaces));
            return smallFaces[charId - MENU_CHAR_CHAOS];
        }
        __builtin_memcpy(smallAgwsFaces, smallAgwsFaceTable, sizeof(smallAgwsFaces));
        return smallAgwsFaces[charId - MENU_CHAR_AGWS_FIRST];
    }
    return 0;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const unsigned short largeFaceTable[12] = { 3328, 3073, 3072, 3840, 3585, 3329, 3584, 3328, 3328, 3841, 4096, 3840 };

const unsigned short largeAgwsFaceTable[12] = { 2306, 2305, 2310, 2311, 2306, 2304, 2307, 2307, 2307, 2308, 2309, 2307 };

const unsigned short smallFaceTable[16] = { 1026, 1025, 1024, 1030, 1029, 1027, 1028, 1026, 1026, 1031, 1032, 1030, 1033, 1033, 1033, 1033 };

const unsigned short smallAgwsFaceTable[12] = { 2314, 2313, 2310, 2311, 2314, 2312, 2315, 2315, 2315, 2316, 2317, 2315 };

const char dexterityText[10] = "Dexterity";

const char etherDefenseText[14] = "Ether Defense";

const char etherAttackText[13] = "Ether Attack";

const char vitalityText[9] = "Vitality";

const char strengthText[9] = "Strength";

const char vx20000Name[9] = "VX-20000";

const char vx10000Name[9] = "VX-10000";

/* JP: カタリナ
 * EN: Catalina */
const char catalinaText[9] = "\245\253\245\277\245\352\245\312";

/* JP: キャシリア
 * EN: Cassiria */
const char cassiriaText[11] = "\245\255\245\343\245\267\245\352\245\242";

/* JP: ーーーーー
 * EN: ----- */
const char blankNameText[11] = "\241\274\241\274\241\274\241\274\241\274";

const char ziggurat8Name[11] = "Ziggurat 8";

const char *MenuParaNameGet(int index)
{
    return para_name_4[index];
}

const char *MenuParaNameGet2(int index)
{
    return para_name2_5[index];
}

const char *MenuCharNameGet(unsigned int charId)
{
    const char *name = char_name_6[charId];

    if (charId == MENU_CHAR_ZIGGY) {
        if (MenuScenarioNoGet() < MENU_SCENARIO_ZIGGY_NAMED) {
            name = char_name2_7[0];
        }
    }
    return name;
}

MenuTextEntry *MenuTextGet(int index)
{
    unsigned int encodedIndex = (unsigned int)index;
    unsigned int itemId = encodedIndex & MENU_ENTRY_ID_MASK;
    unsigned int highIndex = encodedIndex >> MENU_ENTRY_CATEGORY_SHIFT;
    MenuTextEntry *result = 0;

    if (itemId == 0) {
        return &dumm_msg_8;
    } else {
        unsigned int textType = highIndex - 1;

        switch (textType) {
        case MENU_CATEGORY_ITEM - 1:
            result = func_A2C5F8(itemId);
            break;
        case MENU_CATEGORY_EVENT_ITEM - 1: {
            const char *itemName = dataEvtItmNameGet(itemId);
            const char *end = itemName;

            dumm2_10.name = itemName;
            dumm2_10.sortName = (const unsigned char *)itemName;
            while (*end++ != '\0') {
            }
            dumm2_10.auxiliary = end;
            result = &dumm2_10;
            break;
        }
        case MENU_CATEGORY_WEAPON - 1:
            result = &dumm_msg_8;
            if (MenuEquipStealMaskCheck() == 0) {
                if (itemId == MENU_WEAPON_SHION_MWS) {
                    if (MenuShionMwsCheck() != 0) {
                        result = func_A2C6E8(MENU_WEAPON_SHION_MWS);
                    }
                } else {
                    result = func_A2C6E8(itemId);
                }
            }
            break;
        case MENU_CATEGORY_BULLET - 1:
            if (MenuEquipStealMaskCheck() != 0) {
                result = &dumm_msg_8;
            } else {
                result = func_A2C738(itemId);
            }
            break;
        case MENU_CATEGORY_ACCESSORY - 1:
            if (MenuEquipStealMaskCheck() == 0) {
                result = func_A2C698(itemId);
            } else {
                result = &dumm_msg_8;
            }
            break;
        case MENU_CATEGORY_CHARACTER - 1:
        case MENU_CATEGORY_AGWS - 1:
            dumm_9.name = MenuCharNameGet(itemId);
            result = &dumm_9;
            break;
        case MENU_CATEGORY_SKILL - 1:
            result = func_A2C648(itemId);
            break;
        case MENU_CATEGORY_ETHER - 1:
            result = func_A2C5A8(itemId);
            break;
        case MENU_CATEGORY_ABILITY - 1:
            result = func_A2C7D8(itemId);
            break;
        case MENU_CATEGORY_FRAME - 1:
            result = func_A2C968(itemId);
            break;
        case MENU_CATEGORY_ENGINE - 1:
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
    unsigned int category = (unsigned int)index >> MENU_ENTRY_CATEGORY_SHIFT;
    int id = index & MENU_ENTRY_ID_MASK;
    int result = 0;

    switch (category) {
    case MENU_CATEGORY_ITEM:
        result = dataItmBoxChk(id);
        break;
    case MENU_CATEGORY_EVENT_ITEM:
        result = dataEvtBoxChk(id);
        break;
    case MENU_CATEGORY_WEAPON:
        result = dataWpnBoxChk(id);
        break;
    case MENU_CATEGORY_BULLET:
        result = dataBltBoxChk(id);
        break;
    case MENU_CATEGORY_ACCESSORY:
        result = dataAccBoxChk(id);
        break;
    case MENU_CATEGORY_CHARACTER:
        result = 0;
        break;
    }
    return result;
}

int MenuBoxInc(unsigned int index)
{
    unsigned int category = index >> MENU_ENTRY_CATEGORY_SHIFT;
    int id = index & MENU_ENTRY_ID_MASK;
    int result = 0;

    switch (category) {
    case MENU_CATEGORY_ITEM:
        dataItmBoxInc(id);
        break;
    case MENU_CATEGORY_EVENT_ITEM:
        dataEvtBoxInc(id);
        break;
    case MENU_CATEGORY_WEAPON:
        dataWpnBoxInc(id);
        break;
    case MENU_CATEGORY_BULLET:
        dataBltBoxInc(id);
        break;
    case MENU_CATEGORY_ACCESSORY:
        dataAccBoxInc(id);
        break;
    case MENU_CATEGORY_CHARACTER:
        result = -1;
        break;
    case MENU_CATEGORY_AGWS:
        PartyTakeAgwsOn(id);
        break;
    }
    return result;
}

int MenuBoxDec(unsigned int index)
{
    unsigned int category = index >> MENU_ENTRY_CATEGORY_SHIFT;
    int id = index & MENU_ENTRY_ID_MASK;
    int result = 0;

    switch (category) {
    case MENU_CATEGORY_ITEM:
        dataItmBoxDec(id);
        break;
    case MENU_CATEGORY_EVENT_ITEM:
        dataEvtBoxDec(id);
        break;
    case MENU_CATEGORY_WEAPON:
        dataWpnBoxDec(id);
        break;
    case MENU_CATEGORY_BULLET:
        dataBltBoxDec(id);
        break;
    case MENU_CATEGORY_ACCESSORY:
        dataAccBoxDec(id);
        break;
    case MENU_CATEGORY_CHARACTER:
        result = -1;
        break;
    }
    return result;
}

int MenuBoxMoneyGet(int index, int priceMode)
{
    float rate[MENU_PRICE_RATE_COUNT];
    int priceValues[MENU_EVENT_ITEM_SHOP_COUNT];
    int agwsPrices[12];
    int itemId = index & MENU_ENTRY_ID_MASK;
    int price = 0;
    int category = (unsigned int)index >> MENU_ENTRY_CATEGORY_SHIFT;
    int rateIndex;
    short weaponId;

    if (itemId == 0) {
        return 0;
    }
    if (priceMode == 0) {
        const unsigned char *discountLevel = &SaveData[SAVE_MENU_DISCOUNT_LEVELS];
        for (rateIndex = 0; rateIndex < MENU_PRICE_RATE_COUNT; rateIndex++) {
            rate[rateIndex] = 1.0f - discountLevel[rateIndex] * 0.1f;
            if (rate[rateIndex] < 0.5f) {
                rate[rateIndex] = 0.5f;
            }
        }
    } else {
        for (rateIndex = MENU_PRICE_RATE_COUNT - 1; rateIndex >= 0; rateIndex--) {
            rate[rateIndex] = 1.0f;
        }
    }
    if ((unsigned int)(category - 1) < 12U) {
    switch (category) {
    case MENU_CATEGORY_ITEM:
        {
            int rawPrice = func_A1A4E8(itemId)->price.value;
            price = (int)(rawPrice * rate[MENU_RATE_ITEM]);
        }
        break;
    case MENU_CATEGORY_EVENT_ITEM:
        if ((unsigned int)(itemId - MENU_EVENT_ITEM_SHOP_FIRST) < MENU_EVENT_ITEM_SHOP_COUNT) {
            __builtin_memcpy(priceValues, boxPriceTable, sizeof(priceValues));
            price = priceValues[itemId - MENU_EVENT_ITEM_SHOP_FIRST];
        }
        break;
    case MENU_CATEGORY_WEAPON:
        price = func_A1A3D8(itemId)->price.value;
        if (func_A1A3D8(itemId)->categoryMask & MENU_EQUIP_CLASS_CHARACTER) {
            price = (int)(price * rate[MENU_RATE_CHARACTER_GEAR]);
        } else {
            price = (int)(price * rate[MENU_RATE_AGWS_WEAPON]);
        }
        break;
    case MENU_CATEGORY_BULLET:
        price = func_A1A428(itemId)->price.value;
        weaponId = func_A1A428(itemId)->weaponId;
        if (weaponId != 0) {
            if (func_A1A3D8(weaponId)->categoryMask & MENU_EQUIP_CLASS_CHARACTER) {
                price = (int)(price * rate[MENU_RATE_CHARACTER_GEAR]);
            } else {
                price = (int)(price * rate[MENU_RATE_AGWS_WEAPON]);
            }
        }
        break;
    case MENU_CATEGORY_ACCESSORY:
        price = func_A1A548(itemId)->price.value;
        if (itemId < MENU_ACCESSORY_AGWS_FIRST) {
            price = (int)(price * rate[MENU_RATE_CHARACTER_GEAR]);
        } else {
            price = (int)(price * rate[MENU_RATE_AGWS_ACCESSORY]);
        }
        break;
    case MENU_CATEGORY_FRAME:
        price = func_A1A598(itemId)->price.value;
        break;
    case MENU_CATEGORY_ENGINE:
        price = func_A1A5E8(itemId)->price.value;
        break;
    case MENU_CATEGORY_AGWS:
        memset(agwsPrices, 0, sizeof(agwsPrices));
        agwsPrices[5] = 50000;
        agwsPrices[9] = 80000;
        agwsPrices[10] = 300000;
        price = (int)(agwsPrices[itemId - MENU_CHAR_AGWS_FIRST] * rate[MENU_RATE_AGWS]);
        break;
    }
    }
    if (priceMode == 1) {
        price = (int)(price * 0.5f);
    }
    return price;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const int boxPriceTable[MENU_EVENT_ITEM_SHOP_COUNT] = { 400, 100, 100 };

int MenuSkillEquipCheck(short charId, short skillId)
{
    UnitOrgWeapons *unitWeapons = func_A191C0(charId);
    int slot;

    for (slot = 0; slot < MENU_SKILL_SLOT_COUNT; slot++) {
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

        for (index = 0; index < MENU_SKILL_SLOT_COUNT; index++) {
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

    for (bit = 0; bit < MENU_MOUNT_BIT_COUNT; bit++) {
        if (((posEquip & 0xFF) >> bit) & 1) {
            return opt_mnt_tbl[bit];
        }
    }
    return 0;
}

int OptMntToPosEquip(int mountCode)
{
    int position = 0;
    int equipmentPosition;

    mountCode &= 0xff;
    if (opt_mnt_tbl[0] == mountCode)
        return 1;
    goto advance_position;
    do {
advance_position:
        position++;
        if (position >= MENU_MOUNT_BIT_COUNT)
            break;
    } while (opt_mnt_tbl[position] != mountCode);
    if (position < MENU_MOUNT_BIT_COUNT)
        equipmentPosition = (1 << position) & 0xff;
    else
        equipmentPosition = 0;
    return equipmentPosition;
}

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
    case MENU_MOUNT_CODE_BIT0:
    case MENU_MOUNT_CODE_BIT1:
        weaponPos = 1;
        break;
    case MENU_MOUNT_CODE_BIT4:
    case MENU_MOUNT_CODE_BIT5:
        weaponPos = 2;
        break;
    case MENU_MOUNT_CODE_BIT2:
        weaponPos = 3;
        break;
    case MENU_MOUNT_CODE_BIT6:
        weaponPos = 4;
        break;
    case MENU_MOUNT_CODE_BIT3:
        weaponPos = 5;
        break;
    case MENU_MOUNT_CODE_BIT7:
        weaponPos = 6;
        break;
    }
    return weaponPos;
}

int WpnPosToOptMnt(int mountId, int weaponId)
{
    BattleWeaponPriceRecord *weapon = 0;
    int result = 0;

    if (weaponId != 0) {
        weapon = func_A1A3D8(weaponId);
    }

    switch (mountId) {
    case 1:
        if (weapon != 0) {
            result = (weapon->mountMask & MENU_MOUNT_MASK_BIT0) != 0 ? MENU_MOUNT_CODE_BIT0 : MENU_MOUNT_CODE_BIT1;
        }
        break;
    case 2:
        if (weapon != 0) {
            result = (weapon->mountMask & MENU_MOUNT_MASK_BIT4) != 0 ? MENU_MOUNT_CODE_BIT4 : MENU_MOUNT_CODE_BIT5;
        }
        break;
    case 3:
        result = MENU_MOUNT_CODE_BIT2;
        break;
    case 4:
        result = MENU_MOUNT_CODE_BIT6;
        break;
    case 5:
        result = MENU_MOUNT_CODE_BIT3;
        break;
    case 6:
        result = MENU_MOUNT_CODE_BIT7;
        break;
    }

    return result;
}

int MenuWpnMountIdChange(int unused, int mountIndex)
{
    int mountIds[MENU_MOUNT_BIT_COUNT];

    __builtin_memcpy(mountIds, wpnMountIdTable, sizeof(mountIds));
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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const int wpnMountIdTable[8] = {32, 24, 34, 26, 35, 27, 33, 25};

static unsigned short chrEquipGet(int charId)
{
    if (charId < MENU_CHAR_AGWS_FIRST) {
        return 0x8000 >> (charId - 1);
    }
    return 0x8000 >> (charId - MENU_CHAR_AGWS_FIRST);
}

static int wpnInfoGet(int weaponType)
{
    return (weaponType < MENU_CHAR_AGWS_FIRST) ? MENU_EQUIP_CLASS_CHARACTER : MENU_EQUIP_CLASS_AGWS;
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

int MenuWeaponEquipPosCheck(int charId, int weaponId, int weaponPosition, int replacementSlot)
{
    UnitOrgWeapons *character;
    BattleWeaponPriceRecord *weapon;
    unsigned char occupiedPositions = 0;
    unsigned short blockedPositions = 0;
    int weight;
    int slot;
    int equipmentIndex;
    int requestedPosition;
    int supportedPositions;
    int result = 0;
    int capacity;
    int mountCode;
    int bitIndex;
    int bit;
    unsigned char alternatives;
    unsigned char mountMask;

    character = func_A191C0(charId);
    if ((unsigned int)(weaponId - 1) >= MENU_WEAPON_ID_COUNT) {
        return 0;
    }
    weapon = func_A1A3D8(weaponId);
    supportedPositions = weapon->mountMask;
    if (!(weapon->characterEquipMask & chrEquipGet(charId))) {
        return 0;
    }
    capacity = MENU_EQUIP_WEIGHT_LIMIT;
    if (charId == MENU_CHAR_AG_05) {
        capacity = MENU_EQUIP_WEIGHT_LIMIT_AG_05;
    }
    weight = 0;
    for (equipmentIndex = 0; equipmentIndex < MENU_WEAPON_SLOT_COUNT; equipmentIndex++) {
        if (character->weapon[equipmentIndex] != 0 && equipmentIndex != replacementSlot) {
            weight += func_A1A3D8(character->weapon[equipmentIndex])->equipWeight;
        }
    }
    if (capacity < weight + func_A1A3D8(weaponId)->equipWeight) {
        return 0;
    }
    requestedPosition = OptMntToPosEquip(WpnPosToOptMnt(weaponPosition, weaponId) & 0xff);
    OptMntToPosEquip2(WpnPosToOptMnt(weaponPosition, weaponId) & 0xff);
    for (equipmentIndex = 0; equipmentIndex < MENU_WEAPON_SLOT_COUNT; equipmentIndex++) {
        if (equipmentIndex != replacementSlot) {
            occupiedPositions = (occupiedPositions | OptMntToPosEquip(character->weaponMount[equipmentIndex] & 0xff)) & 0xff;
        }
    }
    for (equipmentIndex = 0; equipmentIndex < MENU_MOUNT_BIT_COUNT; equipmentIndex++) {
        if ((occupiedPositions >> equipmentIndex) & 1) {
            mountCode = PosEquipToOptMnt((1 << equipmentIndex) & 0xff);
            for (slot = 0; slot < MENU_WEAPON_SLOT_COUNT; slot++) {
                if (character->weaponMount[slot] == mountCode && character->weapon[slot] != 0) {
                    int existingBit;

                    weapon = func_A1A3D8(character->weapon[slot]);
                    mountMask = weapon->mountMask;
                    for (bitIndex = 0; bitIndex < 2; bitIndex++) {
                        unsigned short alternatives;
                        existingBit = MENU_MOUNT_MASK_BIT7 >> bitIndex;
                        alternatives = mountMask & ~existingBit & MENU_MOUNT_MASK_HIGH;
                        if (weapon->mountMask & existingBit) {
                            if (alternatives & MENU_MOUNT_MASK_BIT4) {
                                alternatives |= MENU_MOUNT_MASK_BIT5;
                            }
                            if (alternatives & MENU_MOUNT_MASK_BIT5) {
                                alternatives |= MENU_MOUNT_MASK_BIT4;
                            }
                            blockedPositions |= alternatives;
                        }
                    }
                    for (bitIndex = 0; bitIndex < 2; bitIndex++) {
                        unsigned char lowMountMask = weapon->mountMask;
                        existingBit = MENU_MOUNT_MASK_BIT3 >> bitIndex;
                        alternatives = mountMask & ~existingBit & MENU_MOUNT_MASK_LOW;
                        if (lowMountMask & existingBit) {
                            if (alternatives & MENU_MOUNT_MASK_BIT0) {
                                alternatives |= MENU_MOUNT_MASK_BIT1;
                            }
                            if (alternatives & MENU_MOUNT_MASK_BIT1) {
                                alternatives |= MENU_MOUNT_MASK_BIT0;
                            }
                            blockedPositions |= alternatives;
                        }
                    }
                }
            }
            for (slot = 0; slot < MENU_WEAPON_SLOT_COUNT; slot++) {
                if (character->weapon[slot] == 0x4f || character->weapon[slot] == 0x54) {
                    blockedPositions |= MENU_MOUNT_MASK_PAIR_HIGH;
                }
            }
        }
    }
    weapon = func_A1A3D8(weaponId);
    mountMask = weapon->mountMask;
    for (bitIndex = 0; bitIndex < 2; bitIndex++) {
        unsigned short alternatives;
        bit = MENU_MOUNT_MASK_BIT7 >> bitIndex;
        alternatives = mountMask & ~bit & MENU_MOUNT_MASK_HIGH;
        if (weapon->mountMask & bit) {
            if (alternatives & MENU_MOUNT_MASK_BIT4) {
                alternatives |= MENU_MOUNT_MASK_BIT5;
            }
            if (alternatives & MENU_MOUNT_MASK_BIT5) {
                alternatives |= MENU_MOUNT_MASK_BIT4;
            }
            if (occupiedPositions & alternatives) {
                blockedPositions = (blockedPositions | bit) & 0xff;
            }
        }
    }
    for (bitIndex = 0; bitIndex < 2; bitIndex++) {
        unsigned char lowMountMask = weapon->mountMask;
        bit = MENU_MOUNT_MASK_BIT3 >> bitIndex;
        alternatives = mountMask & ~bit & MENU_MOUNT_MASK_LOW;
        if (lowMountMask & bit) {
            if (alternatives & MENU_MOUNT_MASK_BIT0) {
                alternatives |= MENU_MOUNT_MASK_BIT1;
            }
            if (alternatives & MENU_MOUNT_MASK_BIT1) {
                alternatives |= MENU_MOUNT_MASK_BIT0;
            }
            if (occupiedPositions & alternatives) {
                blockedPositions = (blockedPositions | bit) & 0xff;
            }
        }
    }
    if ((weaponId == 0x4f || weaponId == 0x54) && (occupiedPositions & MENU_MOUNT_MASK_PAIR_HIGH)) {
        blockedPositions |= MENU_MOUNT_MASK_PAIR_LOW;
    }
    if ((requestedPosition & supportedPositions) && (requestedPosition & ~blockedPositions)) {
        result = WpnPosToOptMnt(weaponPosition, weaponId) & 0xff;
    }
    return result;
}

int MenuAccessoryEquipCheck(int charId, int accessoryId, int slotIndex)
{
    unsigned short characterMask;
    UnitOrgWeapons *characterWeapons;
    BattleAccessoryPriceRecord *accessory;
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
        BattleAccessoryPriceRecord *equipped =
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
        charId < MENU_CHAR_AGWS_FIRST ? accessory->category : 2;

    if (categoryToScan == 2) {
        for (index = 0; index < MENU_ACCESSORY_SLOT_COUNT; index++) {
            if (characterWeapons->accessoryId[index] == accessoryId) {
                return 0;
            }
        }
    } else if (accessory->category != 2) {
        for (index = 0; index < MENU_ACCESSORY_SLOT_COUNT; index++) {
            short equippedId = characterWeapons->accessoryId[index];

            if (equippedId != 0) {
                BattleAccessoryPriceRecord *equipped = func_A1A548(equippedId);

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
    if ((dataWpnGet(weaponId)->flags & WEAPON_FLAG_NEEDS_BULLETS) == 0) {
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
    case MENU_CHAR_CHAOS:
    case MENU_CHAR_MOMO:
        return 0;
    case MENU_CHAR_SHION:
        return func_A197E8(MENU_CHAR_SHION, 0x3E) != 0;
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
    } while (charId < MENU_CHAR_BIC_13);
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
        for (slot = 0; slot < MENU_WEAPON_SLOT_COUNT; slot++) {
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

void MenuSortChange(int bufferIndex, int sortMode)
{
    int *entries = sort[bufferIndex];
    int count = 0;
    void (*compare)(int *, int *) = f_11[sortMode];
    int *next = &entries[1];
    int entry;
    int first;
    int second;

    if (entries[0] != 0) {
        do {
            entry = *next++;
            count++;
        } while (entry != 0);
    }
    if (count != 0) {
        for (first = 0; first < count - 1; first++) {
            for (second = first + 1; second < count; second++) {
                compare(&entries[first], &entries[second]);
            }
        }
    }
}

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

void MenuSortSet(int bufferIndex, int categoryMask, int filter)
{
    struct MenuWeaponRangePair {
        unsigned char first;
        unsigned char last;
    } weaponRanges[MENU_WEAPON_RANGE_COUNT];
    int party[8];
    int *entry = sort[bufferIndex];
    int id;
    int first;
    int last;
    int range;
    int weaponCategory;
    int weaponPosition;
    int bitIndex;
    unsigned int mountBits;
    int availableMounts;
    int accessoryCategory;
    int requiresAssociatedId;
    int partyCount;
    int *partyMember;
    int currentMember;
    BattleWeaponPriceRecord *weapon;
    BattleAccessoryPriceRecord *accessory;
    UnitOrgWeapons *character;
    short selectedCharId;
    short *etherId;
    int currentEtherId;

    memset(entry, 0, sizeof(sort[bufferIndex]));
    if (categoryMask & MENU_SORT_ITEM) {
        if (filter == MENU_SORT_FILTER_SHOP) {
            for (id = 1; id < MENU_ITEM_ID_END; id++) {
                *entry++ = MenuIdChange((short)id, MENU_CATEGORY_ITEM);
            }
        } else {
            id = 1;
            goto inventory_bound;
            do {
                if (dataItmBoxChk(id) > 0) {
                    *entry++ = MenuIdChange((short)id, MENU_CATEGORY_ITEM);
                }
                id++;
inventory_bound:
                ;
            } while (id < MENU_ITEM_ID_END);
        }
    }
    if (categoryMask & MENU_SORT_EVENT_ITEM) {
        first = 1;
        last = MENU_EVENT_ITEM_LAST;
        if (filter == MENU_SORT_FILTER_EVENT_SHOP) {
            first = MENU_EVENT_ITEM_SHOP_FIRST;
            last = MENU_EVENT_ITEM_SHOP_LAST;
        }
        for (id = first; id <= last; id++) {
            if (dataEvtBoxChk(id) > 0) {
                if (id != MENU_EVENT_ITEM_SHION_MWS || MenuEquipStealMaskCheck() != 1) {
                    *entry++ = (id & MENU_ENTRY_ID_MASK) + MENU_ENTRY_EVENT_ITEM_BASE;
                }
            }
        }
    }
    if ((categoryMask & MENU_SORT_WEAPON) && MenuEquipStealMaskCheck() == 0) {
        __builtin_memcpy(weaponRanges, (const struct MenuWeaponRangePair[9]) {
            { 1, 11 }, { 12, 23 }, { 69, 69 }, { 0, 0 }, { 41, 45 },
            { 24, 40 }, { 46, 56 }, { 70, 114 }, { 1, 69 }
        }, sizeof(weaponRanges));
        weaponPosition = 0;
        switch (filter) {
        case MENU_SORT_FILTER_AGWS:
            range = MENU_WEAPON_RANGE_AGWS;
            weaponCategory = MENU_EQUIP_CLASS_AGWS;
            break;
        case MENU_SORT_FILTER_CHARACTER:
            weaponCategory = MENU_EQUIP_CLASS_CHARACTER;
            range = MENU_WEAPON_RANGE_ALL_CHARACTERS;
            break;
        default:
            if (filter & MENU_EQUIP_CLASS_AGWS) {
                weaponCategory = MENU_EQUIP_CLASS_AGWS;
                weaponPosition = filter & ~MENU_EQUIP_CLASS_AGWS;
                range = MENU_WEAPON_RANGE_AGWS;
            } else {
                weaponCategory = MENU_EQUIP_CLASS_CHARACTER;
                range = filter - 1;
            }
            break;
        }
        for (id = weaponRanges[range].first; id <= weaponRanges[range].last; id++) {
            if (dataWpnBoxChk(id) > 0) {
                weapon = func_A1A3D8(id);
                if (!(weapon->categoryMask & weaponCategory)) {
                    continue;
                }
                if (weaponPosition != 0) {
                    availableMounts = weapon->mountMask;
                    mountBits = 0x88;
                    for (bitIndex = 0; bitIndex < 2; bitIndex++) {
                        if (availableMounts & mountBits) {
                            availableMounts &= mountBits;
                            break;
                        }
                        mountBits >>= 1;
                    }
                    if (!(availableMounts & OptMntToPosEquip2(WpnPosToOptMnt(weaponPosition, id) & 0xff))) {
                        continue;
                    }
                }
                *entry++ = MenuIdChange((short)id, MENU_CATEGORY_WEAPON);
            }
        }
    }
    if (categoryMask & MENU_SORT_BULLET) {
        id = 0;
        last = 0;
        if (MenuEquipStealMaskCheck() == 0) {
            switch (filter) {
            case MENU_CHAR_SHION: id = 1; last = 15; break;
            case MENU_CHAR_KOSMOS: id = 16; last = 30; break;
            case MENU_CHAR_ZIGGY: id = 31; last = 45; break;
            case MENU_CHAR_JR: id = 46; last = MENU_BULLET_ID_CHARACTER_LAST; break;
            case MENU_SORT_FILTER_CHARACTER: id = 1; last = MENU_BULLET_ID_CHARACTER_LAST; break;
            case MENU_SORT_FILTER_AGWS: id = MENU_BULLET_ID_AGWS_FIRST; last = MENU_BULLET_ID_LAST; break;
            case MENU_SORT_FILTER_BOTH: id = 1; last = MENU_BULLET_ID_LAST; break;
            }
            for (; id <= last; id++) {
                if (dataBltBoxChk(id) > 0) {
                    *entry++ = MenuIdChange((short)id, MENU_CATEGORY_BULLET);
                }
            }
        }
    }
    if ((categoryMask & MENU_SORT_ACCESSORY) && MenuEquipStealMaskCheck() == 0) {
        accessoryCategory = filter;
        requiresAssociatedId = 0;
        if (filter > 0) {
            accessoryCategory = filter & 0xff;
            requiresAssociatedId = filter >> 8;
        }
        switch (accessoryCategory) {
        case MENU_SORT_FILTER_CHARACTER:
            for (id = 1; id < MENU_ACCESSORY_AGWS_FIRST; id++) {
                if (dataAccBoxChk(id) > 0) {
                    *entry++ = MenuIdChange((short)id, MENU_CATEGORY_ACCESSORY);
                }
            }
            break;
        case MENU_SORT_FILTER_AGWS:
            for (id = MENU_ACCESSORY_AGWS_FIRST; id < MENU_ACCESSORY_ID_END; id++) {
                if (dataAccBoxChk(id) > 0) {
                    *entry++ = MenuIdChange((short)id, MENU_CATEGORY_ACCESSORY);
                }
            }
            break;
        case 0:
        case 1:
        case 2:
            for (id = 1; id < MENU_ACCESSORY_AGWS_FIRST; id++) {
                if (dataAccBoxChk(id) <= 0) {
                    continue;
                }
                accessory = func_A1A548(id);
                if (requiresAssociatedId != 0 && accessory->associatedId == 0) {
                    continue;
                }
                switch (accessoryCategory) {
                case 0:
                    if (accessory->category == 1) {
                        *entry++ = MenuIdChange((short)id, MENU_CATEGORY_ACCESSORY);
                    }
                    break;
                case 1:
                    if (accessory->category == 0) {
                        *entry++ = MenuIdChange((short)id, MENU_CATEGORY_ACCESSORY);
                    }
                    break;
                case 2:
                    if (accessory->category == 2) {
                        *entry++ = MenuIdChange((short)id, MENU_CATEGORY_ACCESSORY);
                    }
                    break;
                }
            }
            break;
        }
    }
    if ((categoryMask & MENU_SORT_CHARACTER) && filter == 1) {
        partyMember = party;
        partyCount = PartyAllPartyGet(partyMember, 1);
        id = 0;
        if (partyCount > 0) {
            do {
                currentMember = *partyMember;
                if (currentMember == 0) {
                    break;
                }
                if (currentMember != MENU_CHAR_KOSMOS && currentMember != MENU_CHAR_ZIGGY) {
                    *entry++ = MenuIdChange((short)*partyMember, MENU_CATEGORY_CHARACTER);
                }
                id++;
                partyMember++;
            } while (id < partyCount);
        }
    }
    if (categoryMask & MENU_SORT_SKILL) {
        selectedCharId = filter;
        for (id = 1; id < MENU_SKILL_ID_END; id++) {
            if (func_A19698(selectedCharId, id) != 0) {
                *entry++ = (id & MENU_ENTRY_ID_MASK) + MENU_ENTRY_SKILL_BASE;
            }
        }
    }
    if (categoryMask & MENU_SORT_ETHER) {
        selectedCharId = filter;
        if (((unsigned int)filter >> 16) == 0) {
            for (id = 1; id < MENU_ETHER_ID_END; id++) {
                if (func_A19578(selectedCharId, id) != 0) {
                    *entry++ = (id & MENU_ENTRY_ID_MASK) + MENU_ENTRY_ETHER_BASE;
                }
            }
        } else {
            id = 0;
            character = func_A191C0(selectedCharId);
            etherId = character->etherId;
            currentEtherId = *etherId;
            if (currentEtherId != 0) {
                currentEtherId = (currentEtherId & MENU_ENTRY_ID_MASK) + MENU_ENTRY_ETHER_BASE;
                do {
                    do {
                        *entry++ = currentEtherId;
                        ++id;
                        ++etherId;
                    } while (0);
                    if (id < MENU_ETHER_SLOT_COUNT) {
                        currentEtherId = *etherId;
                        if (currentEtherId == 0) {
                            break;
                        }
                        currentEtherId = (currentEtherId & MENU_ENTRY_ID_MASK) + MENU_ENTRY_ETHER_BASE;
                        continue;
                    }
                    break;
                } while (1);
            }
        }
    }
    *entry = 0;
}

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
    return id + (highPart << MENU_ENTRY_CATEGORY_SHIFT);
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

    fill = (mode == MENU_LIST_MODE_SHOP) ? subListMake01 : subListMake00;

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
    entry->amount = MENU_LIST_NO_DATA_AMOUNT;
    entry->name = msg_12;
    entry->flag = 0;
    entry[1].name = 0;
    return list;
}

MenuListEntry *MenuListGet(int listIndex)
{
    return mw_list[listIndex];
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char roboEtcPath[15] = "data\\robo\\etc\\";

const char yamamotoMotPath[19] = "data\\yamamoto\\mot\\";

const char motionPath[13] = "data\\motion\\";

const char menuAgwsName[9] = "menuagws";

const char mtnMeqpName[9] = "mtn_meqp";

const char gunoMotionName[9] = "guno_000";

char *MenuFileNameGet(int id, unsigned int resourceKind)
{
    short modelId = id;

    switch (resourceKind) {
    case MENU_FILE_MODEL_LEX:
    case MENU_FILE_MODEL_XTX:
    case MENU_FILE_MODEL_JNT:
        if (modelId == MENU_MODEL_ID_UKN_AGWS) {
            strcpy(fname_13, f_base_14[MENU_PATH_ROBO_ETC]);
            strcat(fname_13, f_ukn_name_16[0]);
        } else {
            RES_GetMdlFileName(fname_13, modelId);
        }
        strcat(fname_13, f_exten_15[resourceKind]);
        break;
    case MENU_FILE_MOTION:
        switch ((modelId >> 12) & 15) {
        case MENU_MODEL_KIND_CHARACTER:
            strcpy(fname_13, f_base_14[MENU_PATH_MOTION]);
            strcat(fname_13, f_char_mot_17[MENU_CHARMOT_MEQP]);
            strcat(fname_13, f_exten_15[MENU_EXT_FPK]);
            break;
        case MENU_MODEL_KIND_AGWS:
            if (modelId == MENU_MODEL_ID_UKN_AGWS) {
                strcpy(fname_13, f_base_14[MENU_PATH_MOTION]);
                strcat(fname_13, f_char_mot_17[MENU_CHARMOT_UKN_G]);
                strcat(fname_13, f_exten_15[MENU_EXT_FPK]);
            } else {
                strcpy(fname_13, f_base_14[MENU_PATH_MOTION]);
                strcat(fname_13, f_char_mot_17[MENU_CHARMOT_AGWS]);
                strcat(fname_13, f_exten_15[MENU_EXT_FPK]);
            }
            break;
        case MENU_MODEL_KIND_MONSTER:
            {
                int monsterId = modelId == MENU_MODEL_ID_MONSTER_ALIAS ? MENU_MONSTER_ALIAS_MOTION_ID : modelId - MENU_MODEL_ID_MONSTER_BASE;
                char *motionName = f_mons_mot_18[0];

                strcpy(fname_13, f_base_14[MENU_PATH_YAMAMOTO_MOT]);
                motionName[7] = monsterId % 10 + '0';
                monsterId /= 10;
                motionName[6] = monsterId % 10 + '0';
                monsterId /= 10;
                motionName[5] = monsterId % 10 + '0';
                strcat(fname_13, motionName);
                strcat(fname_13, f_exten_15[MENU_EXT_BIN]);
            }
            break;
        }
        break;
    case MENU_FILE_MODEL2_LEX:
    case MENU_FILE_MODEL2_JNT:
        RES_GetMdlFileName(fname_13, modelId + MENU_MODEL2_ID_OFFSET);
        if (resourceKind == MENU_FILE_MODEL2_LEX) {
            strcat(fname_13, f_exten_15[MENU_EXT_LEX]);
        } else {
            strcat(fname_13, f_exten_15[MENU_EXT_JNT]);
        }
        break;
    }
    return fname_13;
}
