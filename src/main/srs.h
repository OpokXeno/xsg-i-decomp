/*
 * TU-local declarations of main/tu212 (src/main/srs.c).
 */

#ifndef SRC_MAIN_SRS_H
#define SRC_MAIN_SRS_H

/* Image-mapper resource types: every slot of the resource state is indexed by
 * the type it registers with svAddImageMapper. */
enum {
    SRS_RES_IMAGE = 0,
    SRS_RES_EFFECT_IMAGE = 1,
    SRS_RES_WEAPON = 2,         /* 3 characters x 3 weapon effects */
    SRS_RES_COMBO = 11,         /* 3 enemy combo slots */
    SRS_RES_BATTLE_IMAGE = 14,
    SRS_RES_CF_IMAGE = 15,
    SRS_RES_EFFECT = 16,        /* 24 loaded effect slots */
    SRS_RES_COUNT = 40
};

/* The complete 0x1A0-byte resource state shared with sef.c. The pending
 * mapper arguments and resource numbers are tables indexed by the resource
 * type each data pointer registers with svAddImageMapper. */
typedef struct SrsMemRes {
    void *image;
    void *effectImage;
    void *weaponData[3][3];
    void *comboData[3];
    void *battleImage;
    void *cfImage;
    void *effectData[24];
    short pending[SRS_RES_COUNT];
    short no[SRS_RES_COUNT];
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
