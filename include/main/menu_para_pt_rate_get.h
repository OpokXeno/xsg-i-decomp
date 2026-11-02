#ifndef INCLUDE_MAIN_MENU_PARA_PT_RATE_GET_H
#define INCLUDE_MAIN_MENU_PARA_PT_RATE_GET_H

#include "shared.h"

/* MenuTextGet ids: the text kind in the high halfword, the item id below it. */
#define MENU_TEXT_WEAPON    0x30000
#define MENU_TEXT_BULLET    0x40000
#define MENU_TEXT_ACCESSORY 0x50000
#define MENU_TEXT_CHARACTER 0x60000

/* Functions and data used by other translation units. */
extern const char MenuAgwsUnequipCommandLabel[];
extern const char MenuAgwsNameLabel[];
extern const char MenuAgwsRightShoulderCommandLabel[];
extern const char MenuAgwsLeftShoulderCommandLabel[];
extern const char MenuAgwsAccessoryCommandLabel[];
extern const char MenuAgwsEngineUnitCommandLabel[];
/* Menu-local work storage and the original slot/point lookup tables. */
/* Font-script caption pointers; the strings keep their established owners. */
/* Tables consumed through the existing partial views. */
extern const char MenuAgwsAssignPilotCommandLabel[];
extern const char MenuAgwsEquipCommandLabel[];
extern const char MenuAgwsPilotCommandLabel[];
extern const char MenuAgwsListCommandLabel[];
extern const char MenuAgwsRightArmCommandLabel[];
extern const char MenuAgwsLeftArmCommandLabel[];
extern const char MenuAgwsRightAltCommandLabel[];
extern const char MenuAgwsLeftAltCommandLabel[];
extern const char MenuAgwsAmmoCommandLabel[];
extern const char MenuAgwsAssignCommandLabel[];
extern const char MenuAgwsRemoveCommandLabel[];
/* MenuPasLengthGet (this TU, still assembler): the caption's pixel width. */
int MenuPasLengthGet(const char *text);

#endif /* INCLUDE_MAIN_MENU_PARA_PT_RATE_GET_H */
