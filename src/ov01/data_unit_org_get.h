/*
 * TU-local declarations of ov01/tu005 (src/ov01/data_unit_org_get.c).
 */

#ifndef SRC_OV01_DATA_UNIT_ORG_GET_H
#define SRC_OV01_DATA_UNIT_ORG_GET_H

#include "shared.h"
#include "ov01/calc.h"
#include "ov01/data_unit_org_get.h"

#define BATTLE_ACTOR_RESOURCE_MODEL 0
#define BATTLE_ACTOR_RESOURCE_MATRIX 1
#define BATTLE_ACTOR_RESOURCE_MOVE 2
#define BATTLE_ACTOR_RESOURCE_ANIM_PACK_TABLE 3 /* + table slot */
/* Table slot 1 (+0x8E0): dataUnitFileLoadMotSp2's weapon motion. */
#define BATTLE_ACTOR_RESOURCE_WEAPON_MOTION (BATTLE_ACTOR_RESOURCE_ANIM_PACK_TABLE + 1)

/* unitLoad uses this same range to skip the ordinary sound setup. */
#define SPECIAL_MODEL_CHARACTER_ID_BEGIN 0xBB
#define SPECIAL_MODEL_CHARACTER_ID_END 0xC3


#define PL_CHARACTER_ID_END 0x21
#define PL_LEARNING_CHARACTER_ID_END 0x11
#define PL_SPECIAL_SLOT_COUNT 8

int dataFileLoadNB(const char *filename, void *destination);

void dataCdSyncClear(void);

/* The ELF symbol table names these padMode (0x00a597bc) and padData
 * (0x00a597c0); dataVPadModeSet stores the virtual-pad mode and clears the
 * accumulated virtual-pad data. */

void dataNBreadCB(int result);

/* The party leader's character id: dataLeaderCidGet returns it and
   dataLeaderCidReset clears it. */

int dataLeaderCidGet(void);
void dataLeaderCidReset(void);

int dataUnitFileLoadMotSp(BattleUnit *unit, int motionId, int slot);

/*
 * A player character's learned-special-technique slot: dataSpecLearnSet
 * (0x00a19750) is the only claimed writer and touches only the leading
 * special id at the front of the 0xC-byte stride (sh $18,72($3) at
 * 0x00a197c8); the remaining ten bytes of each slot are untouched by any
 * function this TU claims.
 */
typedef struct PlSpecialSlot {
    short id;                      /* +0x00 */
    unsigned char unmodeled_2[10]; /* +0x02 */
} PlSpecialSlot;

/*
 * dataPlChaGet indexes plChaData in 0xA8-byte strides using cid - 1.
 * It rejects cid >= 0x21; the learning helpers separately reject cid >=
 * 0x11. The original functions do not check a lower bound.
 *
 * dataEtherLearnGet/Set access the signed-byte bitmap at +0x28, and
 * dataSkillLearnGet/Set access the one at +0x38. Each bitmap occupies the
 * 0x10-byte span before the next field; technique ids select byte and bit
 * with (id - 1) / 8 and (id - 1) % 8.
 *
 * dataSpecDataGet searches eight 0xC-byte special-technique slots from
 * +0x48, establishing the rest of the 0xA8-byte record. dataSpecLearnSet
 * indexes these slots by specialId - dataSpecBaseGet(cid).
 */
typedef struct PlCharacter {
    unsigned char unmodeled_0[0x28];
    signed char learnedEther[0x10]; /* +0x28 */
    signed char learnedSkill[0x10]; /* +0x38 */
    PlSpecialSlot special[PL_SPECIAL_SLOT_COUNT]; /* +0x48 */
} PlCharacter;

extern PlCharacter *dataPlChaGet(int cid);
extern int dataSpecBaseGet(int cid);

/* "** dataSpecLearnSet: err %d\n" at 0x00a457b0 (ov01 .rodata), the format
 * string dataSpecLearnSet (0x00a19750) hands to printf when its character id
 * is out of the zero-to-0x10 range. */
extern int printf(const char *format, ...);
extern const char unitOrgDataSpecLearnSetErrText[];

/*
 * The six 0x130-byte unit-file records are selected by dataUnitFileGet.
 * dataUnitFileLoadMdl reads the cached character id, model/animation/texture
 * addresses and CD-sector-rounded sizes, and clears the face cache at +0x10C.
 */
/* dataUnitFileLoadMot's per-slot motion pack record (24-byte stride from
 * UnitFileInfo +0x28): cached motion id, load address and CD-sector size.
 * Slot 0 is the base motion, slots 1-3 weapon motions, slot 4 a pack
 * loaded without caching its id. */
typedef struct UnitMotionFile {
    int motionId;
    void *adr; /* +0x04 */
    int size; /* +0x08 */
    unsigned char unmodeled_0c[12];
} UnitMotionFile;

typedef struct UnitFileEnt {
    int id;
    void *adr; /* +0x04 */
    int size; /* +0x08 */
} UnitFileEnt;

typedef struct UnitWpnFile {
    UnitFileEnt part[3]; /* model, animation and texture of one weapon slot */
} UnitWpnFile;

typedef struct UnitFileInfo {
    int charaId;
    int modelCharaId; /* +0x04 */
    void *modelAdr; /* +0x08 */
    int modelSize; /* +0x0C */
    unsigned char unmodeled_10[4];
    void *animationAdr; /* +0x14 */
    int animationSize; /* +0x18 */
    unsigned char unmodeled_1c[4];
    void *textureAdr; /* +0x20 */
    int textureSize; /* +0x24 */
    UnitMotionFile motion[5]; /* +0x28 */
    UnitWpnFile wpn[3]; /* +0xA0, 0x24-byte records selected by weapon slot */
    int faceCharaId; /* +0x10C */
    void *faceModelAdr; /* +0x110: dataUnitFileLoadFaceMdl's cached face model buffer */
    unsigned char unmodeled_114[0x128 - 0x114];
    void *faceTextureAdr; /* +0x128: dataUnitFileLoadFaceMdl's cached face texture buffer (stored to actor->textureAdr) */
    unsigned char unmodeled_12c[0x130 - 0x12C];
} UnitFileInfo;



/* The same task layout with the actor slots read as 32-bit words, as
 * dataUnitFileLoadWep stores and reloads them. */
typedef struct UnitWeaponActors {
    XglTaskPrefix scheduler;
    Actor *work;
    int motionActor;
    unsigned char unmodeled_18[4];
    int equipActor[4];
} UnitWeaponActors;

extern void dataMotAdrSet(Actor *actor, int motionId);

int dataWpnLRChk(BattleUnit *unit, int weaponId, int slot, UnitFileInfo *fileInfo);

#endif /* SRC_OV01_DATA_UNIT_ORG_GET_H */
