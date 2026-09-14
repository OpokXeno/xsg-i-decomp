/*
 * TU-local declarations of main/tu180 (src/main/data.c).
 */

#ifndef SRC_MAIN_DATA_H
#define SRC_MAIN_DATA_H

#include "shared.h"

int dataItmBoxInc(int id);

extern int dataBoxInc(int category, int id);

int dataBoxDec(int category, int id);

extern u16 *dataBoxPtrGet(int category);

#endif /* SRC_MAIN_DATA_H */
