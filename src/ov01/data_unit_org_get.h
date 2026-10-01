/*
 * TU-local declarations of ov01/tu005 (src/ov01/data_unit_org_get.c).
 */

#ifndef SRC_OV01_DATA_UNIT_ORG_GET_H
#define SRC_OV01_DATA_UNIT_ORG_GET_H

#include "shared.h"
#include "ov01/calc.h"
#include "ov01/battle_actor.h"
#include "ov01/data_file.h"

#define PL_CHARACTER_ID_END 0x21
#define PL_LEARNING_CHARACTER_ID_END 0x11
#define PL_SPECIAL_SLOT_COUNT 8

void dataCdSyncClear(void);

extern int cdReqNum;

/* The ELF symbol table names these padMode (0x00a597bc) and padData
 * (0x00a597c0); dataVPadModeSet stores the virtual-pad mode and clears the
 * accumulated virtual-pad data. */
extern int padMode;
extern short padData;

void dataNBreadCB(int result);

/* The party leader's character id: dataLeaderCidGet returns it and
   dataLeaderCidReset clears it. */
extern int leaderCid;

int dataLeaderCidGet(void);
void dataLeaderCidReset(void);

int dataUnitFileLoadMotSp(ObjectTask *unit, int motionId, int slot);

/* Callers pass the unit whose default weapon they want; the function
 * always returns 0 and ignores it. */
int dataDefWpnGet(void *unit);

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
extern const char D_00A457B0[];

/*
 * The six 0x130-byte unit-file records are selected by dataUnitFileGet.
 * dataUnitFileLoadMdl reads the cached character id, model/animation/texture
 * addresses and CD-sector-rounded sizes, and clears the face cache at +0x10C.
 */
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
    unsigned char unmodeled_28[0x10C - 0x28];
    int faceCharaId; /* +0x10C */
    unsigned char unmodeled_110[0x20];
} UnitFileInfo;

extern UnitFileInfo unitFileInfo[6];

extern CalcUnitParam *calcUPGet(ObjectTask *unit);

/* UnitRecord in unit_cmd.h evidences the same task prefix, separate
 * motion actor (+0x14) and four equipment actors (+0x1C). */
typedef struct UnitEquipInfo {
    /* Generic scheduler helpers use ObjectTask; battle code knows the
     * work allocation is a BattleActor. Both views have the same prefix. */
    union {
        ObjectTask object;
        struct {
            XglTaskPrefix scheduler;
            BattleActor *work;
        } battle;
    } task;
    BattleModelActor *motionActor;
    unsigned char unmodeled_18[4];
    BattleModelActor *equipActor[4];
} UnitEquipInfo;

extern void dataMotAdrSet(BattleModelActor *actor, int motionId);

typedef struct WpnFileInfo {
    unsigned char unmodeled_00[0xA0];
    int weaponId; /* +0xA0: dataWpnLRChk */
} WpnFileInfo;

int dataWpnLRChk(ObjectTask *unit, int weaponId, int slot, WpnFileInfo *fileInfo);

#endif /* SRC_OV01_DATA_UNIT_ORG_GET_H */
