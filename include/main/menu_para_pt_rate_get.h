#ifndef INCLUDE_MAIN_MENU_PARA_PT_RATE_GET_H
#define INCLUDE_MAIN_MENU_PARA_PT_RATE_GET_H

#include "shared.h"

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
