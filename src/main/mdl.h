/*
 * TU-local declarations of main/tu264 (src/main/mdl.c).
 */

#ifndef SRC_MAIN_MDL_H
#define SRC_MAIN_MDL_H

#include "shared.h"

typedef struct MdlPart MdlPart;

/*
 * Accessed prefix of the model draw record passed by ACT_modelDrawSub
 * (actor + 0x840, main:0x00307984/0x003079b0). The fields below are the
 * widths and offsets read or written by MDL_create, MDL_partsSetVisible and
 * MDL_draw. Gaps retain their original unknown bytes. This type describes
 * the evidenced prefix through +0x9f; it does not claim the complete record
 * size or describe bytes after that prefix.
 */
typedef struct MdlHandle {
    int entry;                         /* +0x00: MDL_draw loads and passes to nmlModelEntry */
    u32 visibleParts[4];               /* +0x04..+0x13: MDL_create writes four words */
    unsigned char unmodeled_14[0x2c];  /* +0x14..+0x3f */
    int partCount;                     /* +0x40: MDL_partsSetVisible reads one word */
    unsigned char unmodeled_44[0x10];  /* +0x44..+0x53 */
    const char *texture;               /* +0x54: MDL_draw passes to nmlModelSetTexture */
    MdlPart *parts;                    /* +0x58: MDL_create clears this part-table pointer */
    unsigned char unmodeled_5c[4];     /* +0x5c..+0x5f */
    Vector4 copiedData[4];             /* +0x60..+0x9f: four slots; MDL_create writes three */
} MdlHandle;

/* MDL_create and MDL_partsSetVisible access these same evidenced offsets
 * through the resource pointer stored in MdlHandle::entry. */
typedef MdlHandle MdlResource;

/*
 * One 0x40-byte part-table entry. The selected visibility functions read the
 * group word at +0x30 and the two name words at +0x38 and +0x3c.
 */
struct MdlPart {
    unsigned char unmodeled_00[0x30];
    int group;
    unsigned char unmodeled_34[4];
    int name[2];
};

/*
 * MDL_partsSetVisible: sibling of this TU, still INCLUDE_ASM scaffolding
 * and untouched by this allocation; declared only to call it.
 */
extern void MDL_partsSetVisible(MdlHandle *model);

extern void nmlModelSetPartsVisible(MdlResource *resource, int partIndex, int visible);

/*
 * nmlModelSetTexture, nmlModelSetPlace, nmlModelEntry (main:0x0022fe60,
 * main:0x0022fed8, main:0x00232b00, src/main/nml_model_set.c): main-owned
 * model system entry points, declared only to call them.
 */
extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetPlace(const Vector4 *place);
extern void nmlModelEntry(int entry);

#endif /* SRC_MAIN_MDL_H */
