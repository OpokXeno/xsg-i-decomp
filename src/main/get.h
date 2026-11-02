/*
 * TU-local declarations of main/tu201 (src/main/get.c).
 */

#ifndef SRC_MAIN_GET_H
#define SRC_MAIN_GET_H

#include "shared.h"
#include "main/Undulate.h"
#include "main/get.h"

/*
 * UnduDataGetHeader's result type: src/main/layout.h, src/main/call_java_method.h
 * and src/main/chr.h already declare this tag the same way (canon:
 * config/header-canon.json, src/math/main/review09-002f6c50/private.h);
 * this TU only carries a pointer to it, so it stays opaque here.
 */
typedef struct LayoutHeader LayoutHeader;

/*
 * The player's own Actor pointer at GameLoopState[1], documented the same
 * way by src/main/chr.h and src/main/enemy_2.h (main VA 0x00338680, +0x4
 * word). src/main/chr.h and src/main/near_dir.h already own the "Actor" tag
 * with their own full struct definitions, so this TU reads the one field it
 * needs through its own minimal, differently-named view instead of a second
 * definition of that tag: src/main/chr.h documents data_header at the same
 * +0x4e0 offset.
 */
typedef unsigned int GameLoopStateWords[];

/* MARK: negative-pi comparison bound (provisional name). */
extern volatile const float k_negative_pi;

/* Role name: full-circle addend applied when below negative pi. */
extern volatile const float kFullCircleAdd;

/* MARK: positive-pi comparison bound (provisional name). */
extern volatile const float k_positive_pi;

/* Role name: full-circle subtrahend applied when above positive pi.
   MARK: deliberately NON-volatile like the prior k_full_circle_again;
   making it volatile adds a reload (100B vs 96B). Qualifier asymmetry
   is load-bearing, not cosmetic. */
extern const float kFullCircleSub;

extern volatile const float radius_neg_pi;
extern const float radius_two_pi_a;
extern volatile const float radius_two_pi_b;
extern volatile const float radius_pi;
extern volatile const float kAbsBoundPi;

/* MARK: |delta| bound (pi) */
extern volatile const float kNegPi;

/* Role name for TwoPiA: added to a
   below-pi delta, i.e. cursor = current + (delta + 2pi) / speed. */
extern volatile const float kTwoPiAddend;

/* Role name for TwoPiB:
   delta is subtracted from it, i.e. cursor = current - (2pi - delta)/speed. */
extern volatile const float kTwoPiSubtrahend;

/* MARK: negative-pi comparison bound */
extern volatile const float kNegPiLimit;

/* MARK: low wrap clamp (neg pi) */
extern volatile const float kTwoPiWrapLo;

/* Role name for TwoPiC: added to
   wrap the cursor up when it sits at/below the low clamp. */
extern volatile const float kPiLimit;

/* MARK: high wrap clamp (pos pi) */
extern volatile const float kTwoPiWrapHi;

extern GameLoopStateWords GameLoopState;

/* Original symbol UnduTest, main VA 0x003b2610, size 0x40
   (config/symbols/main.txt); Undulate.c has no header of its own yet. */
extern UnduParam UnduTest;

extern const float getRandomFloat;

extern float cosf(float angle);

extern float atan2f(float y, float x);

void Get_MiddlePoint_Parabora(const ParabolaVec *first, const ParabolaVec *second,
                              int current, int total, float height,
                              ParabolaVec *destination);

/*
 * UnduParamInit and UnduCheck are still original bytes in src/main/Undulate.c
 * (main); this is their query parameter block, TU-local until that TU has
 * its own header. Only the fields Get_HeightAttr/Get_Height touch are
 * modeled.
 */
extern void UnduParamInit(UnduParam *param);

float Get_HeightAttr(const Vector4 *position, int mapIndex, int attrMask,
                    UnduParam *param);

#endif /* SRC_MAIN_GET_H */
