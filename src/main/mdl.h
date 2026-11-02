/*
 * TU-local declarations of main/tu264 (src/main/mdl.c).
 */

#ifndef SRC_MAIN_MDL_H
#define SRC_MAIN_MDL_H

#include "shared.h"
#include "main/mdl.h"
#include "main/nml_model_set.h"

typedef struct MdlPart MdlPart;

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

extern void nmlModelSetPartsVisible(MdlResource *resource, int partIndex, int visible);

extern void nmlModelEntry(int entry);

#endif /* SRC_MAIN_MDL_H */
