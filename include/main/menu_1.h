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
