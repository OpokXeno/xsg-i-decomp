/*
 * TU-local declarations of ov01/tu005 (src/ov01/data_unit_org_get.c).
 */

#ifndef SRC_OV01_DATA_UNIT_ORG_GET_H
#define SRC_OV01_DATA_UNIT_ORG_GET_H

#include "shared.h"
#include "ov01/calc.h"

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
 * The player character record dataPlChaGet (this TU, still asm) returns for
 * a character id in 0..0x10. dataSpecLearnSet below is the only claimed
 * accessor and reaches only the special-technique table at +0x48; the
 * record's own layout before that offset is not established by any function
 * this TU claims. The table is indexed by specialId - dataSpecBaseGet(cid)
 * with no bound this TU claims, so it keeps a one-slot declared length.
 */
typedef struct PlCharacter {
    unsigned char unmodeled_0[0x48];
    PlSpecialSlot special[1]; /* +0x48, indexed dynamically */
} PlCharacter;

extern PlCharacter *dataPlChaGet(int cid);
extern int dataSpecBaseGet(int cid);

/* "** dataSpecLearnSet: err %d\n" at 0x00a457b0 (ov01 .rodata), the format
 * string dataSpecLearnSet (0x00a19750) hands to printf when its character id
 * is out of the zero-to-0x10 range. */
extern int printf(const char *format, ...);
extern const char D_00A457B0[];

/* The six unit-file records are selected by dataUnitFileGet. */
typedef struct UnitFileInfo {
    int charaId;
    unsigned char unmodeled_4[0x12c];
} UnitFileInfo;

extern UnitFileInfo unitFileInfo[6];

extern CalcUnitParam *calcUPGet(ObjectTask *unit);

/* Partial motion and actor records used by the allocated load helpers. */
typedef struct MotionAdrTable {
    unsigned char unmodeled_0[0x8dc];
    int slot[1];
} MotionAdrTable;

typedef struct EquipActor {
    unsigned char unmodeled_0[0x8e0];
    int motionAdr;
} EquipActor;

typedef struct UnitEquipInfo {
    unsigned char unmodeled_00[0x14];
    MotionAdrTable *motionTable;
    unsigned char unmodeled_18[4];
    EquipActor *equipActor[4];
} UnitEquipInfo;

extern void dataMotAdrSet(MotionAdrTable *table, int motionId);

#endif /* SRC_OV01_DATA_UNIT_ORG_GET_H */
