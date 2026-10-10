#ifndef SRC_MAIN_ANM_H
#define SRC_MAIN_ANM_H

typedef struct FpkFcvPack FpkFcvPack;
typedef struct FpkAnimationEntry FpkAnimationEntry;
typedef struct AnmClock AnmClock;
typedef struct AnmDefaultState AnmDefaultState;

FpkAnimationEntry *ANM_getEntry(AnmClock *clock, FpkFcvPack *pack);
void *PACK_getEntry(FpkFcvPack *pack, int index);
void *PACK_getAttr(FpkFcvPack *pack, int index);
void *PACK_unlink(FpkFcvPack *pack, int base_address);
u32 PACK_getSize(FpkFcvPack *pack);
FpkAnimationEntry *ANM_resetDefault(AnmDefaultState *state, FpkFcvPack *pack);
FpkAnimationEntry *ANM_resetTime(AnmClock *clock);
FpkAnimationEntry *ANM_resetPack(AnmClock *clock);
FpkAnimationEntry *ANM_resetTimeInterp(AnmClock *clock);
FpkAnimationEntry *ANM_reset(AnmClock *clock, FpkFcvPack *pack);
void ANM_update(AnmClock *clock);
void ANM_updateTime(AnmClock *clock, float elapsed);

#endif /* SRC_MAIN_ANM_H */
