#ifndef INCLUDE_OV12_XRG_EVENT_H
#define INCLUDE_OV12_XRG_EVENT_H

#include "ov12/rg_weapon_db.h"

/* The event level steps the unlock state of the selectable units: 0, 1 or 2. */
#define XRG_EVENT_LEVEL_NONE (-1) /* XrgSetEventLevel: go back to the flags */

/* Functions and data used by other translation units. */
void XrgSetEventLevel(int level);
int XrgEventGetLevel(void);
int XrgEventIsUsableEnemy(unsigned int index);
int XrgEventIsUsablePlayer(unsigned int character_index);
int XrgEventIsUsableWeapon(RgWeaponEssence *weapon);

#endif /* INCLUDE_OV12_XRG_EVENT_H */
