#ifndef INCLUDE_OV01_CALC_H
#define INCLUDE_OV01_CALC_H

#include "shared.h"

/*
 * The per-unit parameter block calcUPGet (this TU, still asm) returns for a
 * battle unit's ObjectTask. Only the members other TUs' code reads are
 * modelled; everything between them stays an unmodeled span.
 *
 * - flags (+0x1C): a halfword bit set; sndConvSe (ov01/tu013) and
 *   unitCmdMove (ov01/tu002) both read it with `lhu $3,0x1C($2)` right after
 *   `jal calcUPGet` (0x00a2dfc0, 0x00a02634); sndConvSe tests bit 0x40.
 * - charaId (+0x38): the character/type id. unitCidGet feeds it to
 *   dataCidGet, and unitLoad range-tests it (0xBB..0xC2) to decide whether
 *   the unit still needs its sound-effect setup call.
 * - sefSetupParams (+0x5E/+0x60/+0x62): the three shorts unitLoad hands to
 *   sefSetupPlayer as its second through fourth arguments (lh 0x5e/0x60/0x62
 *   at 0x00a018d8..0x00a018e8). sefSetupPlayer stores them, next to the id,
 *   in its _battlePrm slot (sh at +0x10/+0x12/+0x14,
 *   0x002e6d84..0x002e6d8c); nothing says more about what they select.
 * - linkedUnit (+0x50): another unit's parameter block. menuStatChrIdGet
 *   (ov01/tu009) loads it with `lw $2,0x50($18)` from its saved calcUPGet
 *   result and returns that block's charaId (`lh $17,0x38($2)`, 0x00a29cac)
 *   for an enemy unit or one in the 0xBB..0xC2 range.
 * - moveKindId (+0x0F): a signed byte unitCmdMove (ov01/tu002) loads with
 *   `lb $3,0xF($2)` right after `jal calcUPGet` (0x00a02640..0x00a02648) and
 *   compares with 4 to choose the warp move over the jump move.
 * - statEffUnits (+0x15C): eight object pointers. statEffOn/statEffOff
 *   (ov01/tu003, 0x00a0f620/0x00a0f6a8) walk them with `lw $3,0xC($2)` from
 *   calcUPGet(unit) + 0x150 + 4 * i, i = 0..7, skip null entries and set or
 *   clear bit 0x100 of the word at +0xA8C of each pointee. The pointee type
 *   is named only by its tag here; its first user completes it.
 */
typedef struct CalcUnitParam CalcUnitParam;

struct CalcUnitParam {
    unsigned char unmodeled_00[0x0F];
    signed char moveKindId;    /* +0x0F: unitCmdMove compares it with 4 */
    unsigned char unmodeled_10[0x1C - 0x10];
    u16 flags;               /* +0x1C */
    unsigned char unmodeled_1e[0x38 - 0x1E];
    short charaId;           /* +0x38 */
    unsigned char unmodeled_3a[0x50 - 0x3A];
    CalcUnitParam *linkedUnit;   /* +0x50: menuStatChrIdGet reads its charaId */
    unsigned char unmodeled_54[0x5E - 0x54];
    short sefSetupParams[3]; /* +0x5E: sefSetupPlayer's extra arguments */
    unsigned char unmodeled_64[0x15C - 0x64];
    struct UnitWork *statEffUnits[8]; /* +0x15C: statEffOn/statEffOff */
};

#endif /* INCLUDE_OV01_CALC_H */
