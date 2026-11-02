#ifndef INCLUDE_MAIN_SRS_H
#define INCLUDE_MAIN_SRS_H

/* Battle slots of the resource state. */
#define SRS_CHARACTER_SLOTS 3        /* party members that own weapon data */
#define SRS_WEAPONS_PER_CHARACTER 3  /* weapon effects loaded per party member */
#define SRS_COMBO_SLOTS 3            /* enemy combo data slots */
#define SRS_EFFECT_SLOTS 24          /* loaded cf effect slots */

/* Image-mapper resource types: every slot of the resource state is indexed by
 * the type it registers with svAddImageMapper. srsAnalyzeEftNo reports the
 * same numbers as the effect category. */
typedef enum {
    SRS_RES_IMAGE = 0,
    SRS_RES_EFFECT_IMAGE = 1,
    SRS_RES_WEAPON = 2,         /* 3 characters x 3 weapon effects */
    SRS_RES_COMBO = 11,         /* 3 enemy combo slots */
    SRS_RES_BATTLE_IMAGE = 14,
    SRS_RES_CF_IMAGE = 15,
    SRS_RES_EFFECT = 16,        /* 24 loaded effect slots */
    SRS_RES_COUNT = 40
} SrsResourceType;

/* The complete 0x1A0-byte resource state shared with sef.c. The pending
 * mapper arguments and resource numbers are tables indexed by the resource
 * type each data pointer registers with svAddImageMapper. */
typedef struct SrsMemRes {
    void *image;
    void *effectImage;
    void *weaponData[SRS_CHARACTER_SLOTS][SRS_WEAPONS_PER_CHARACTER];
    void *comboData[SRS_COMBO_SLOTS];
    void *battleImage;
    void *cfImage;
    void *effectData[SRS_EFFECT_SLOTS];
    short pending[SRS_RES_COUNT];
    short no[SRS_RES_COUNT];
    void *effectExtra[SRS_EFFECT_SLOTS];
} SrsMemRes;

extern SrsMemRes _srsMemRes;

int srsLoadEffectData(void *buffer, int effectNo);
int srsFileLoadCf(int cfId, int effectNo);

/* Functions and data used by other translation units. */
char *srsAnalyzeEftNo(int effectNo, int *charNo, int *effectType);
extern int srsGetEffect2Idx(int effectNo);
extern void sresFreeReloaderMemory(int reload_bgm);
void srsSetLoadMode(int load_mode);
char *srsGetEsdData(int index);

#endif /* INCLUDE_MAIN_SRS_H */
