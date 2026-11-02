/*
 * TU-local declarations of main/tu169 (src/main/menu_1.c).
 */

#ifndef SRC_MAIN_MENU_1_H
#define SRC_MAIN_MENU_1_H
#include "main/menu_1.h"

/* The menu parameter result exposes HP/EP maxima and current values. */
struct MenuCharParameters {
    short maxHp;
    short maxEp;
    unsigned char unmodeled_04[0x34 - 0x04];
    short hp;
    short ep;
};

#endif /* SRC_MAIN_MENU_1_H */
