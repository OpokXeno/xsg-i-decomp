#include "common.h"

#include "mdl.h"

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_setGroupVisible);

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_setNameVisible);

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_setVisible);

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_create);

INCLUDE_ASM("asm/main/nonmatchings/mdl", MDL_partsSetVisible);

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
