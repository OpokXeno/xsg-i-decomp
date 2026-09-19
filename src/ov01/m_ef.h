/*
 * TU-local declarations of ov01/tu022 (src/ov01/m_ef.c).
 */

#ifndef SRC_OV01_M_EF_H
#define SRC_OV01_M_EF_H

#include "shared.h"

extern float MMathMakeRandom2PI(void);

/*
 * mefCamParams: main-owned camera/quad parameter block, still asm-owned
 * (config/symbols/main.txt "mefCamParams = 0x0043AF40; // size:0x50").
 * screen and matrix are opaque pointers forwarded verbatim to
 * MMathRotTransPersClip; basis[0..3] are the quadwords the vertex makers read
 * to build their transform. Bytes 0x08..0x0f are read by no function of this
 * allocation and are left an explicit unlabelled extent rather than a guess.
 */
typedef struct MEfCamParams {
    void *screen;
    void *matrix;
    unsigned char unread_08[8];
    Vector4 basis[4];
} MEfCamParams;

extern MEfCamParams mefCamParams;

/*
 * mefVertex2CornerTable: OV01 rodata still asm-owned by this TU (the packet's
 * data ownership window 0x00a51265..0x00a51380), referenced by the name the
 * scaffold witness already uses (docs/naming.md, "scaffold-owned data keeps
 * its splat name") -- not a claim of a recovered original name.
 */
typedef struct MEfCornerAngles {
    float angle[4];
} MEfCornerAngles;

extern const MEfCornerAngles mefVertex2CornerTable;

extern int MMathRotTransPersClip(Vector4 *destination, const void *screen, const void *matrix, const Vector4 *point);

/*
 * MMathApplyMatrix / MMathRotateMatrixYX / MMathMakeRandom: main math-library
 * helpers, still asm-owned in main (config/units/math-correction13-9.json
 * "references"). Declared TU-locally until main's own TU claims them.
 */
extern void *MMathApplyMatrix(Vector4 *destination, const Vector4 *matrix, const Vector4 *point);
extern void MMathRotateMatrixYX(Vector4 *destination, const Vector4 *source, const Vector4 *angles);
extern float MMathMakeRandom(void);

/*
 * MEfCalcAngle: sibling function of this same TU, still INCLUDE_ASM
 * scaffolding and untouched by this allocation; declared only to call it.
 */
extern void *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to);

/*
 * MOutputDebugStringWarn: main-owned diagnostic logger (main:0x002eef50,
 * config/tu/main/tu218.json), declared only to call it. D_00A512B0 is this
 * TU's own rodata (data ownership window 0x00a51265..0x00a51380), the
 * "%s has overflowed" format string MEfCheckWorkSize passes.
 */
extern void MOutputDebugStringWarn(const char *format, ...);
extern const char D_00A512B0[];

/*
 * nmlModelSetTexture (main:0x0022fe60, src/main/nml_model_set.c),
 * nmlModelSetPlace (main:0x0022fed8) and nmlModelEntry (main:0x00232b00):
 * main-owned model system entry points, declared only to call them.
 */
extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetPlace(const Vector4 *place);
extern void nmlModelEntry(int entry);

#endif /* SRC_OV01_M_EF_H */
