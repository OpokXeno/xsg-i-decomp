/*
 * TU-local declarations of main/tu212 (src/main/srs.c).
 */

#ifndef SRC_MAIN_SRS_H
#define SRC_MAIN_SRS_H

/* The complete 0x1A0-byte resource state shared with sef.c. Pointer,
 * pending, resource-number and extra-pointer blocks retain their retail offsets. */
typedef struct SrsMemRes {
    void *image;
    void *effectImage;
    void *weaponData[3][3];
    void *comboData[3];
    void *battleImage;
    void *cfImage;
    void *effectData[24];
    short imagePending;
    short effectImagePending;
    short weaponPending[3][3];
    short comboPending[3];
    short battleImagePending;
    short cfImagePending;
    short effectPending[24];
    short imageNo;
    short effectImageNo;
    short weaponNo[3][3];
    short comboNo[3];
    short battleImageNo;
    short cfImageNo;
    short effectNo[24];
    void *effectExtra[24];
} SrsMemRes;

extern SrsMemRes _srsMemRes;

void srsSetLoadMode(int load_mode);

static int srsLoadMode = 0;

int srsGetLoadMode(void);

char *srsGetEsdData(int index);

static int _nRead;

char *srsGetEffectName(int effectNo);

static int fileLoad(void *buffer, const char *name, int mode);

#endif /* SRC_MAIN_SRS_H */
