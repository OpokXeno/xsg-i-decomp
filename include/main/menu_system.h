#ifndef INCLUDE_MAIN_MENU_SYSTEM_H
#define INCLUDE_MAIN_MENU_SYSTEM_H

#include "shared.h"

/* SaveData bytes holding the system options. */
enum {
    SAVE_DATA_SOUND_OUTPUT = 0x28,
    SAVE_DATA_VIBRATION = 0x3c,
    SAVE_DATA_HDD = 0x40,
    SAVE_DATA_CURSOR_MEMORY = 0x44
};

/* Functions and data used by other translation units. */
extern const char MenuSkillSetSuffixText[8];
extern const char MenuSkillTitleText[8];
extern XglClock _CountTime;

#endif /* INCLUDE_MAIN_MENU_SYSTEM_H */
