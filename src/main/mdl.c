#include "common.h"

#include "mdl.h"

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_setGroupVisible);

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_setNameVisible);

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_setVisible);

void MDL_create(MdlHandle *model, const MdlResource *resource)
{
    int wordIndex;

    if (resource != 0) {
        model->copiedData[1] = resource->copiedData[2];
        model->copiedData[2] = resource->copiedData[1];
        model->copiedData[0] = resource->copiedData[3];
    }
    model->entry = (int)resource;
    model->parts = 0;
    for (wordIndex = 3; wordIndex >= 0; wordIndex--) {
        model->visibleParts[wordIndex] = 0xffffffffu;
    }
}

void MDL_partsSetVisible(MdlHandle *model)
{
    MdlResource *resource = (MdlResource *)(u32)model->entry;
    u32 *visibleWords;
    int partCount;
    int bitIndexMask;
    int partIndex;

    if (resource == 0) {
        return;
    }

    partCount = resource->partCount;
    if (partCount > 128) {
        partCount = 128;
    }

    /* The low five index bits select a flag within each 32-part word. */
    visibleWords = model->visibleParts;
    bitIndexMask = 31;

    for (partIndex = 0; partIndex < partCount; partIndex++) {
        u32 bit = 1u << (partIndex & bitIndexMask);
        u32 visibilityMask = visibleWords[partIndex >> 5] & bit;

        nmlModelSetPartsVisible(resource, partIndex, visibilityMask);
    }
}

/*
 * Applies the model's texture and placement, marks its parts visible, then
 * tail-calls the model system to enter it for rendering.
 */
void MDL_draw(MdlHandle *model, const Vector4 *place)
{
    int entry = model->entry;

    nmlModelSetTexture(model->texture);
    nmlModelSetPlace(place);
    MDL_partsSetVisible(model);
    nmlModelEntry(entry);
}
