/*
 * TU-local declarations of main/tu264 (src/main/mdl.c).
 */

#ifndef SRC_MAIN_MDL_H
#define SRC_MAIN_MDL_H

#include "shared.h"

/*
 * The model-draw handle MDL_draw and MDL_partsSetVisible take: a sub-block
 * of an actor record (ACT_modelDrawSub passes actor + 0x840, addiu
 * $4,$17,0x840 at main 0x00307984/0x003079b0).
 *
 *   +0x00 entry    the nmlModelEntry index MDL_draw tail-calls with
 *                  (lw $18,0($16) at main 0x00318108, j nmlModelEntry at
 *                  0x0031814c).
 *   +0x54 texture  the nmlModelSetTexture pointer MDL_draw passes first
 *                  (lw $4,84($16) at main 0x0031810c).
 */
typedef struct MdlHandle {
    int entry;
    unsigned char unmodeled_04[0x50];
    const char *texture;
} MdlHandle;

/*
 * MDL_partsSetVisible: sibling of this TU, still INCLUDE_ASM scaffolding
 * and untouched by this allocation; declared only to call it.
 */
extern void MDL_partsSetVisible(MdlHandle *model);

/*
 * nmlModelSetTexture, nmlModelSetPlace, nmlModelEntry (main:0x0022fe60,
 * main:0x0022fed8, main:0x00232b00, src/main/nml_model_set.c): main-owned
 * model system entry points, declared only to call them.
 */
extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetPlace(const Vector4 *place);
extern void nmlModelEntry(int entry);

#endif /* SRC_MAIN_MDL_H */
