#ifndef INCLUDE_OV01_DATA_UNIT_ORG_GET_H
#define INCLUDE_OV01_DATA_UNIT_ORG_GET_H

#include "ov01/calc.h"

/* dataUnitInitGet selects 0x34-byte records. dataUnitInitSet copies maxHp and
 * maxEp to the unit's current HP and EP. MCamGetAtkRange and MCamGetDefRange
 * read signed halfwords at +0x10 and +0x12, respectively; both ranges are
 * stored in hundredths. Other fields remain unmodeled. */
typedef struct UnitInitData {
    unsigned short maxHp; /* +0x00 */
    unsigned short maxEp; /* +0x02 */
    unsigned char unmodeled_04[0x10 - 0x04];
    short atkRange;
    short defRange;
    unsigned char unmodeled_14[0x1C - 0x14];
    unsigned short flags;  /* +0x1C: bit 0x2000 is tested by dataSndStEdNumSub (ov01/tu013) */
    unsigned char unmodeled_1e[0x34 - 0x1E];
} UnitInitData;

UnitInitData *dataUnitInitGet(int entry);

/* Callers pass the unit whose default weapon they want; the function
 * always returns 0 and ignores it. */
int dataDefWpnGet(BattleUnit *unit);

/*
 * One 12-byte entry of the special-weapon table, terminated by a zero
 * tecId. dataTid2WepSp only reads tecId; dataFileLoadWepSp* read the words
 * at +0x04 and +0x08.
 */
typedef struct WepSpEntry {
    int tecId;                       /* +0x00 */
    int weaponId;                    /* +0x04: stored to the unit's weaponId[2] */
    int unk8;                        /* +0x08: its low byte goes to CalcUnitParam +0x5C */
} WepSpEntry;

#endif /* INCLUDE_OV01_DATA_UNIT_ORG_GET_H */
