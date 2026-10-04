#include "common.h"

short EventID = 0;
signed char FlagExEvent = 0;

INCLUDE_ASM("asm/main/nonmatchings/pause_menu_page_enemy", PauseMenuPageEnemy);

INCLUDE_ASM("asm/main/nonmatchings/pause_menu_page_enemy", Debug_Mark);
