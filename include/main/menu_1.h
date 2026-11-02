#ifndef INCLUDE_MAIN_MENU_1_H
#define INCLUDE_MAIN_MENU_1_H

/*
 * MenuTextGet returns a pointer to a per-entry text record of three string
 * pointers. The leading word is the display name: the fallback record
 * dumm_msg_8 holds "None" there, MenuTextGet stores the event item name or
 * MenuCharNameGet's result there. subListMake00/subListMake01 copy it into
 * a MenuListEntry. The string at +0x8 is the one MenuSortSubType04/05 walk
 * byte by byte to order two entries; it is empty ("") in dumm_msg_8 while
 * the display name is not, so it is kept distinct as the sort name. The
 * word between them is never read by any function of this allocation.
 * The help window of main/menu_para_pt_rate_get.c prints that middle string
 * (auxiliary, +0x04) as its description line. The index passed to
 * MenuTextGet carries the category in its high halfword and the item or
 * character id in its low one.
 */
typedef struct MenuTextEntry {
    const char *name;
    const char *auxiliary;
    const unsigned char *sortName;
} MenuTextEntry;

/*
 * A menu list row: subListMake00 fills it with an item/weapon's owned
 * quantity, subListMake01 with a shop price, both in the same `amount`
 * slot; both clear `flag`. MenuListMake terminates a list with the row
 * { "No data." (msg_12), -1, 0 }. sizeof == 12 (pointer + int + char,
 * padded), matching every construction site.
 *
 * The other list views read the same row: the character list reads only
 * `flag`; the skill list stores a skill's value in `amount`, its locked
 * state in `flag` and its level or equipped mark in `status`; the AGWS
 * and ether lists keep their equipped/usable state in `status`.
 *
 * It is the item record of every WindowSPCursor list (main/e_battle_win_open.c):
 * the window prints `name`, shows `amount` as its number and greys a row
 * whose `flag` is nonzero; eBattleWinOpen builds the same rows for the
 * battle command window. `flag` is read with lb there (a signed char).
 */
typedef struct MenuListEntry {
    const char *name;               /* +0x00 */
    int amount;                     /* +0x04 */
    signed char flag;               /* +0x08 */
    signed char status;             /* +0x09 */
    unsigned char unmodeled_0a[2];
} MenuListEntry;

/*
 * Character ids, as indexed by MenuCharNameGet's name table. Ids 1-12 are the
 * playable characters and crew, 13-16 the BIC units, 17 upward the AGWS and
 * the other mechs listed after them. MenuCharHpCheck/MenuCharEpCheck take
 * MENU_CHAR_WHOLE_PARTY to test every party member.
 */
enum MenuCharId {
    MENU_CHAR_WHOLE_PARTY = -1,
    MENU_CHAR_NONE = 0,
    MENU_CHAR_CHAOS = 1,
    MENU_CHAR_KOSMOS = 2,
    MENU_CHAR_SHION = 3,
    MENU_CHAR_ZIGGY = 5,
    MENU_CHAR_MOMO = 6,
    MENU_CHAR_JR = 7,
    MENU_CHAR_CASSIRIA = 8,
    MENU_CHAR_CATALINA = 9,
    MENU_CHAR_VIRGIL = 10,
    MENU_CHAR_MARY = 11,
    MENU_CHAR_SOLDIER = 12,
    MENU_CHAR_BIC_13 = 13,
    MENU_CHAR_BIC_14 = 14,
    MENU_CHAR_BIC_15 = 15,
    MENU_CHAR_BIC_16 = 16,
    MENU_CHAR_VX_10000 = 17,
    MENU_CHAR_VX_7000 = 18,
    MENU_CHAR_VX_9000 = 19,
    MENU_CHAR_VX_20000 = 20,
    MENU_CHAR_VX_05 = 21,
    MENU_CHAR_VX_4000 = 22,
    MENU_CHAR_AG_01 = 23,
    MENU_CHAR_AG_02 = 24,
    MENU_CHAR_AG_03 = 25,
    MENU_CHAR_AG_04 = 26,
    MENU_CHAR_AG_05 = 27,
    MENU_CHAR_AG_06 = 28,
    MENU_CHAR_BUNNIE = 29,
    MENU_CHAR_AGWS_FIRST = MENU_CHAR_VX_10000
};

/* MenuTagTextGet indices: the tag texts shown on window headers. */
enum MenuTagId {
    MENU_TAG_COMBATANT = 1,
    MENU_TAG_RESERVE = 2,
    MENU_TAG_REGISTERED_COMBATANT = 3,
    MENU_TAG_REGISTERED_RESERVE = 4,
    MENU_TAG_USER = 5,
    MENU_TAG_FROM = 6,
    MENU_TAG_TARGET = 7,
    MENU_TAG_TO = 8
};

/*
 * A menu list entry (the value in the MenuSortSet buffers, MenuTextGet,
 * MenuBoxChk ...) packs the category into its high halfword and the id into
 * its low one.
 */
#define MENU_ENTRY_ID_MASK 0xFFFF
#define MENU_ENTRY_CATEGORY_SHIFT 16

enum MenuEntryCategory {
    MENU_CATEGORY_ITEM = 1,
    MENU_CATEGORY_EVENT_ITEM = 2,
    MENU_CATEGORY_WEAPON = 3,
    MENU_CATEGORY_BULLET = 4,
    MENU_CATEGORY_ACCESSORY = 5,
    MENU_CATEGORY_CHARACTER = 6,
    MENU_CATEGORY_AGWS = 7,
    MENU_CATEGORY_SKILL = 8,
    MENU_CATEGORY_ETHER = 9,
    MENU_CATEGORY_ABILITY = 10,
    MENU_CATEGORY_FRAME = 11,
    MENU_CATEGORY_ENGINE = 12
};

/* Pre-shifted category halfwords of the entry value, for ids added directly. */
#define MENU_ENTRY_EVENT_ITEM_BASE 0x20000
#define MENU_ENTRY_SKILL_BASE 0x80000
#define MENU_ENTRY_ETHER_BASE 0x90000
#define MENU_ENTRY_ABILITY_BASE 0xA0000

/* MenuSortSet categoryMask bits: which lists to put into the sort buffer. */
enum MenuSortCategoryMask {
    MENU_SORT_ITEM = 0x1,
    MENU_SORT_EVENT_ITEM = 0x2,
    MENU_SORT_WEAPON = 0x4,
    MENU_SORT_BULLET = 0x8,
    MENU_SORT_ACCESSORY = 0x10,
    MENU_SORT_CHARACTER = 0x20,
    MENU_SORT_SKILL = 0x40,
    MENU_SORT_ETHER = 0x80
};

/*
 * MenuSortSet filter values of the weapon, bullet and accessory lists: the
 * character set (weapon class MENU_EQUIP_CLASS_CHARACTER, bullet ids up to 63,
 * accessory ids up to 128) or the AGWS set (MENU_EQUIP_CLASS_AGWS, bullet ids
 * 64-178, accessory ids 129-180). The item list uses MENU_SORT_FILTER_SHOP to
 * list every item, owned or not.
 */
#define MENU_SORT_FILTER_CHARACTER (-1)
#define MENU_SORT_FILTER_AGWS (-2)
#define MENU_SORT_FILTER_BOTH (-3)
#define MENU_SORT_FILTER_SHOP (-10)
/* The event item list with this filter holds only the purchasable event items. */
#define MENU_SORT_FILTER_EVENT_SHOP (-1)

/* categoryMask of a weapon record: whether characters or AGWS can use it. */
#define MENU_EQUIP_CLASS_CHARACTER 0x4000
#define MENU_EQUIP_CLASS_AGWS 0x2000

int MenuMaryIdChange(int id);
MenuTextEntry *MenuTextGet(int index);
int MenuScenarioNoGet(void);
int MenuEquipStealMaskCheck(void);
const unsigned char *MenuSaveMapNameGet(int mapId);
/* Whether the shown character may use the Equip command. */
int MenuMainCharCheck(int charId);
unsigned short MenuFaceEpidGet(int charId, int smallPortrait);
int MenuAccessoryEquipCheck(int charId, int accessoryId, int slotIndex);

/* Functions and data used by other translation units. */
int MenuIdChange(short id, int highPart);
int MenuBoxChk(int index);
/*
 * priceMode 0 applies the discount rates stored in SaveData (clamped to at
 * least half price); any other value uses the full rate, and 1 additionally
 * halves the result (the sell price). subListMake01 passes 0.
 */
int MenuBoxMoneyGet(int index, int priceMode);
void MenuSortSet(int bufferIndex, int categoryMask, int filter);

#endif /* INCLUDE_MAIN_MENU_1_H */
