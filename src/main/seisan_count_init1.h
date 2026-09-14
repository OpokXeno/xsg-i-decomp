/*
 * TU-local declarations of main/tu172 (src/main/seisan_count_init1.c).
 */

#ifndef SRC_MAIN_SEISAN_COUNT_INIT1_H
#define SRC_MAIN_SEISAN_COUNT_INIT1_H

#include "shared.h"

void tskTskMain2(TskObject *task);

/* Only byte 1 of the pointed-to work area is evidenced by this allocation. */
extern unsigned char *SeisanWork;

#endif /* SRC_MAIN_SEISAN_COUNT_INIT1_H */
