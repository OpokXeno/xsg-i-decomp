/*
 * TU-local declarations of main/tu212 (src/main/srs.c).
 */

#ifndef SRC_MAIN_SRS_H
#define SRC_MAIN_SRS_H
#include "main/srs.h"

int srsGetLoadMode(void);
char *srsGetEffectName(int effectNo);
static int fileLoad(void *buffer, const char *name, int mode);

static int srsLoadMode = 0;

static int _nRead;

#endif /* SRC_MAIN_SRS_H */
