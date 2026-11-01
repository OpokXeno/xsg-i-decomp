#ifndef INCLUDE_OV01_DATA_UNIT_ORG_GET_H
#define INCLUDE_OV01_DATA_UNIT_ORG_GET_H

#include "ov01/calc.h"

/* dataUnitInitGet selects 0x34-byte records. MCamGetAtkRange and
 * MCamGetDefRange read signed halfwords at +0x10 and +0x12, respectively;
 * both ranges are stored in hundredths. Other fields remain unmodeled. */
typedef struct UnitInitData {
    unsigned char unmodeled_00[0x10];
    short atkRange;
    short defRange;
    unsigned char unmodeled_14[0x34 - 0x14];
} UnitInitData;

UnitInitData *dataUnitInitGet(int entry);

/* Callers pass the unit whose default weapon they want; the function
 * always returns 0 and ignores it. */
int dataDefWpnGet(void *unit);

/*
 * One 12-byte entry of the special-weapon table, terminated by a zero
 * tecId. dataTid2WepSp only reads tecId; dataFileLoadWepSp* (still asm)
 * read the words at +0x04 and +0x08.
 */
typedef struct WepSpEntry {
    int tecId;                       /* +0x00 */
    unsigned char unmodeled_04[8];
} WepSpEntry;

#endif /* INCLUDE_OV01_DATA_UNIT_ORG_GET_H */
