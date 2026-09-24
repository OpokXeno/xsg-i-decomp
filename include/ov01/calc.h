#ifndef INCLUDE_OV01_CALC_H
#define INCLUDE_OV01_CALC_H

#include "shared.h"

typedef struct CalcUnitParam CalcUnitParam;

struct CalcUnitParam {
    unsigned char unmodeled_00[0x02];
    short maxEp;                /* +0x02: calcEp's clamp ceiling */
    unsigned char unmodeled_04[0x0C - 0x04];
    unsigned char hitStat;      /* +0x0C: calcHitParaGet's own signed view */
    unsigned char unmodeled_0d[0x0E - 0x0D];
    signed char agility;       /* +0x0E: calcWct's wait-count formula */
    signed char moveKindId;    /* +0x0F: unitCmdMove compares it with 4 */
    /*
    * +0x10/+0x12: the two halfwords unitColiGet (ov01/tu002, 0x00a07488)
    * adds into the gap unitCmdDstSet keeps between a moving unit and the
    * unit it walks up to. It reads `lh $3,16($2)` (0x00a074a0) from the
    * moving unit's own block and `lh $3,18($2)` (0x00a074b8) from the
    * approached unit's block, adds them and divides the sum by 100.0f;
    * unitCmdDstSet subtracts that distance from the target position.
    */
    short coliMove;             /* +0x10: term used when this unit moves */
    short coliTarget;           /* +0x12: term used when it is approached */
    unsigned char unmodeled_14[0x1C - 0x14];
    u16 flags;               /* +0x1C */
    unsigned char unmodeled_1e[0x24 - 0x1E];
    u16 statProtect[1];      /* +0x24: calcStatProtectGet/Set's group word */
    unsigned char unmodeled_26[0x34 - 0x26];
    u16 currentHp;           /* +0x34: calcPara2OrgSub's copy source */
    u16 currentEp;           /* +0x36: calcPara2OrgSub's copy source */
    short charaId;           /* +0x38 */
    unsigned char unmodeled_3a[0x3E - 0x3A];
    short turnBias;             /* +0x3E: calcWct/calcTurnEnd */
    short waitCount;            /* +0x40: calcTurnEnd/calcWctDec */
    short waitCountBase;        /* +0x42: calcTurnEnd */
    unsigned char unmodeled_44[0x50 - 0x44];
    CalcUnitParam *linkedUnit;   /* +0x50: menuStatChrIdGet reads its charaId */
    unsigned char unmodeled_54[0x5E - 0x54];
    short sefSetupParams[3]; /* +0x5E: sefSetupPlayer's extra arguments */
    short accessoryId[3];    /* +0x64: calcMagDefGet's per-slot item ids */
    unsigned char unmodeled_6a[0xAC - 0x6A];
    u16 statActiveMask[8];   /* +0xAC: calcStatTurn's per-category bit set */
    unsigned char unmodeled_bc[0xCC - 0xBC];
    unsigned char statTurnCount[8][16]; /* +0xCC: calcStatTurn's countdown grid */
    u16 etherDisableFlags;   /* +0x14C: calcEtherDisableGet/Set's bit set */
    unsigned char unmodeled_14e[0x15C - 0x14E];
    struct UnitWork *statEffUnits[8]; /* +0x15C: statEffOn/statEffOff */
};

#endif /* INCLUDE_OV01_CALC_H */
