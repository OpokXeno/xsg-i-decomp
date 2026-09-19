/*
 * TU-local declarations of main/tu212 (src/main/srs.c).
 */

#ifndef SRC_MAIN_SRS_H
#define SRC_MAIN_SRS_H

void srsSetLoadMode(int load_mode);

extern int srsLoadMode;

int srsGetLoadMode(void);

char *srsGetEsdData(int index);

extern int _nRead;

char *srsGetEffectName(int effectNo);

static int fileLoad(void *buffer, const char *name, int mode);

#endif /* SRC_MAIN_SRS_H */
