/*
 * TU-local declarations of main/tu106 (src/main/nml_model_set.c).
 */

#ifndef SRC_MAIN_NML_MODEL_SET_H
#define SRC_MAIN_NML_MODEL_SET_H

#include "shared.h"

/*
 * Bounded evidenced partial view of the 800-byte .data object at 0x004a91e0.
 * The extent (200 four-byte slots = 800 bytes) matches the original symbol;
 * no field names, struct layout or padding members are claimed. Each slot
 * is explicitly a float-or-int untagged word: the body witnesses float
 * members (shadow inputs at 0x40/0x60, plane value at 0xb4, matrix rows at
 * 0x180/0x190/0x194/0x198/0x1b0/0x1b4/0x1b8, height value at 0x2ac) and one
 * integer flag (height enable at 0x2a8). The flag is read through the .i
 * member, a documented GNU C union access, never a pun through a float
 * array. Uses beyond this body's witnessed offsets (for example the
 * pointer-width slot evidenced only by other callers) are unknown here and
 * receive no member. All accesses below name explicit original byte offsets.
 */
typedef union {
    float f;
    int i;
} LayoutSlot;

typedef struct {
    LayoutSlot slots[200];
} LayoutStore;

/*
 * s_inLayout is the 800-byte layout store at main:0x004A91E0, modelled as 200
 * four-byte slots.  LayoutSlot is four bytes wide, so slots[0x23c / 4] is byte
 * offset 0x23c exactly: the pointer-width slot this function writes.
 */
void nmlModelSetMatrix(void *matrix);

/* canon: config/header-canon.json chose src/math/main/spark-cont01-00231d50/nmlModelCalcDropShadow.c over 0 other accepted spellings */
extern LayoutStore s_inLayout;

extern unsigned int s_nShadowVec;

extern Vector4 s_inShadowVec;

#endif /* SRC_MAIN_NML_MODEL_SET_H */
