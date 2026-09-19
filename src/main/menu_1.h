/*
 * TU-local declarations of main/tu169 (src/main/menu_1.c).
 */

#ifndef SRC_MAIN_MENU_1_H
#define SRC_MAIN_MENU_1_H

/*
 * MenuTextGet returns a pointer to a per-entry text record of three string
 * pointers. The leading word is the display name: the fallback record
 * dumm_msg_8 holds "None" there, MenuTextGet stores the event item name or
 * MenuCharNameGet's result there. subListMake00/subListMake01 copy it into
 * a MenuListEntry. The string at +0x8 is the one MenuSortSubType04/05 walk
 * byte by byte to order two entries; it is empty ("") in dumm_msg_8 while
 * the display name is not, so it is kept distinct as the sort name. The
 * word between them is never read by any function of this allocation.
 */
typedef struct MenuTextEntry {
    const char *name;
    unsigned char unmodeled_04[4];
    const unsigned char *sortName;
} MenuTextEntry;

/*
 * A menu list row: subListMake00 fills it with an item/weapon's owned
 * quantity, subListMake01 with a shop price, both in the same `amount`
 * slot; both clear `flag`. MenuListMake terminates a list with the row
 * { "No data." (msg_12), -1, 0 }. sizeof == 12 (pointer + int + char,
 * padded), matching every construction site.
 */
typedef struct MenuListEntry {
    const char *name;
    int amount;
    unsigned char flag;
} MenuListEntry;

MenuTextEntry *MenuTextGet(int index);
int MenuBoxChk(int index);
/*
 * priceMode 0 applies the discount rates stored in SaveData (clamped to at
 * least half price); any other value uses the full rate, and 1 additionally
 * halves the result (the sell price). subListMake01 passes 0.
 */
int MenuBoxMoneyGet(int index, int priceMode);

#endif /* SRC_MAIN_MENU_1_H */
