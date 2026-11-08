#include "common.h"
#include "main/mdl.h"
#include "mdl.h"

#define SWAP32(x) \
    ((((u32)(x) >> 24) | (((x) >> 8) & 0xff00)) | \
     ((((u32)(x) << 8) & 0xff0000) | ((u32)(x) << 24)))

/* A model tracks the visibility of up to 128 parts, one bit each, in four 32-bit words. */
#define MDL_MAX_PARTS 128
#define MDL_VISIBLE_WORDS 4

/* Sets or clears the visibility of every part in `group` (byte-swapped) and returns how many parts matched. */
int MDL_setGroupVisible(MdlHandle *model, int group, int visible)
{
    MdlResource *resource = (MdlResource *)model->entry;
    u32 bit;
    int count;
    int partCount;
    int wordIndex;
    int partIndex;

    if (resource == 0) {
        return 0;
    }

    group = (((u32)group >> 24) | ((group >> 8) & 0xff00)) | ((((u32)group << 8) & 0xff0000) | ((u32)group << 24));
    count = 0;
    partCount = resource->partCount;
    for (partIndex = 0; partIndex < partCount; partIndex++) {
        if (model->parts[partIndex].group != group) {
            continue;
        }
        bit = 1u << (partIndex & 31);
        wordIndex = partIndex >> 5;
        if (visible) {
            model->visibleParts[wordIndex] |= bit;
        } else {
            model->visibleParts[wordIndex] &= ~bit;
        }
        count++;
    }
    return count;
}

/*
 * Sets or clears the visibility of every part whose name words match `name` under the
 * masks in name[2] and name[3] (byte-swapped).  Returns how many parts matched.
 */
int MDL_setNameVisible(MdlHandle *model, const int *name, int visible)
{
    MdlResource *resource = (MdlResource *)model->entry;
    u32 bit;
    int key0;
    int key1;
    int mask0;
    int mask1;
    int count;
    int partCount;
    int wordIndex;
    int partIndex;

    if (resource == 0 || model->parts == 0) {
        return 0;
    }

    key0 = name[0];
    key1 = name[1];
    mask0 = name[2];
    mask1 = name[3];
    key0 = SWAP32(key0);
    key1 = SWAP32(key1);
    mask0 = SWAP32(mask0);
    mask1 = SWAP32(mask1);
    key0 &= mask0;
    key1 &= mask1;
    count = 0;
    partCount = resource->partCount;
    for (partIndex = 0; partIndex < partCount; partIndex++) {
        if (((model->parts[partIndex].name[0] & mask0) != key0) || ((model->parts[partIndex].name[1] & mask1) != key1)) {
            continue;
        }
        bit = 1u << (partIndex & 31);
        wordIndex = partIndex >> 5;
        count++;
        if (visible) {
            model->visibleParts[wordIndex] |= bit;
        } else {
            model->visibleParts[wordIndex] &= ~bit;
        }
    }
    return count;
}

void MDL_setVisible(MdlHandle *model, int partIndex, int visible)
{
    int bitIndex = partIndex & 31;
    int wordIndex = partIndex >> 5;
    u32 bit = 1u << bitIndex;

    if (visible != 0) {
        model->visibleParts[wordIndex] |= bit;
    } else {
        model->visibleParts[wordIndex] &= ~bit;
    }
}

/*
 * Initializes `model` for `resource`: copies three of its data words (in permuted
 * order), points the model at it and makes all parts visible.
 */
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
    for (wordIndex = MDL_VISIBLE_WORDS - 1; wordIndex >= 0; wordIndex--) {
        model->visibleParts[wordIndex] = 0xffffffffu;
    }
}

/* Passes each part's visibility flag, for up to MDL_MAX_PARTS parts, to nmlModelSetPartsVisible. */
void MDL_partsSetVisible(MdlHandle *model)
{
    MdlResource *resource = (MdlResource *)model->entry;
    u32 *visibleWords;
    int partCount;
    int bitIndexMask;
    int partIndex;

    if (resource == 0) {
        return;
    }

    partCount = resource->partCount;
    if (partCount > MDL_MAX_PARTS) {
        partCount = MDL_MAX_PARTS;
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

/* Draws the model: sets its texture and placement, applies the part visibility, then draws the entry. */
void MDL_draw(MdlHandle *model, Matrix4 *place)
{
    int entry = model->entry;

    nmlModelSetTexture(model->texture);
    nmlModelSetPlace(place);
    MDL_partsSetVisible(model);
    nmlModelEntry(entry);
}
