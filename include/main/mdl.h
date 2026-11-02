#ifndef INCLUDE_MAIN_MDL_H
#define INCLUDE_MAIN_MDL_H

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
    u32 entry;                         /* +0x00: resource address; MDL_draw passes it to nmlModelEntry */
    u32 visibleParts[4];               /* +0x04..+0x13: MDL_create writes four words */
    unsigned char unmodeled_14[0x2c];  /* +0x14..+0x3f */
    int partCount;                     /* +0x40: MDL_partsSetVisible reads one word */
    unsigned char unmodeled_44[0x10];  /* +0x44..+0x53 */
    const char *texture;               /* +0x54: MDL_draw passes to nmlModelSetTexture */
    MdlPart *parts;                    /* +0x58: MDL_create clears this part-table pointer */
    unsigned char unmodeled_5c[4];     /* +0x5c..+0x5f */
    Vector4 copiedData[4];             /* +0x60..+0x9f: four slots; MDL_create writes three */
} MdlHandle;

/* Functions and data used by other translation units. */
void MDL_setVisible(MdlHandle *model, int partIndex, int visible);
/*
 * MDL_partsSetVisible: defined in this TU (src/main/mdl.c) and called by
 * MDL_draw.
 */
extern void MDL_partsSetVisible(MdlHandle *model);

#endif /* INCLUDE_MAIN_MDL_H */
