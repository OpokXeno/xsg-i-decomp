#ifndef INCLUDE_MAIN_SDV_H
#define INCLUDE_MAIN_SDV_H

#include "shared.h"

typedef struct GamePlayerRecord {
    u8 unmodeled_00[0xa8];
} GamePlayerRecord;

/* Functions and data used by other translation units. */
/* moneyBox: 8-byte .sdata counter at 0x004dbb68 (sd -16392(gp)). */
extern unsigned long long moneyBox;
extern const Vector4 McMathUnitVector;
void sdvSetAmbState(int state, int effect_no);
void sdvSetAmbState2(int state, int effect_no);
void sdvClearSpecialWork(void);
void sdvInitAlters(void);
void sdvDestroyAlters(void);
void sdvExecAlters(void);

#endif /* INCLUDE_MAIN_SDV_H */
