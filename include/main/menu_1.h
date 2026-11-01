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
 */
typedef struct MenuTextEntry {
    const char *name;
    const unsigned char *auxiliary;
    const unsigned char *sortName;
} MenuTextEntry;

int MenuMaryIdChange(int id);
MenuTextEntry *MenuTextGet(int index);
int MenuScenarioNoGet(void);
int MenuEquipStealMaskCheck(void);
const unsigned char *MenuSaveMapNameGet(int mapId);
int MenuMainCharCheck(int charId);
unsigned short MenuFaceEpidGet(int charId, int smallPortrait);
int MenuAccessoryEquipCheck(int charId, int accessoryId, int slotIndex);

#endif /* INCLUDE_MAIN_MENU_1_H */
