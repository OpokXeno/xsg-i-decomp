#include "common.h"
#include "layout.h"

void LAYOUT_mapID_setUnit(u8 *layout, int map_id, int unit_id)
{
    LayoutHeader *header;
    JavaField *field;
    u8 *peer;
    float *position;
    float *rotation;

    header = UnduDataGetHeader(map_id + 0x200, unit_id + 0x8000);
    if (header == 0)
        return;

    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(layout_peer, -1), 0);
    peer = *(u8 **)(layout + field->offset);
    position = (float *)(peer + 16);
    rotation = (float *)(peer + 32);
    position[0] = header->components[0];
    position[1] = header->components[1];
    position[2] = header->components[2];
    rotation[1] = header->components[3];

    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(layout_px, -1), 0);
    *(float *)(layout + field->offset) = position[0];
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(layout_py, -1), 0);
    *(float *)(layout + field->offset) = position[1];
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(layout_pz, -1), 0);
    *(float *)(layout + field->offset) = position[2];
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(layout_ry, -1), 0);
    *(float *)(layout + field->offset) =
        rotation[1] / layout_unit_pi * 180.0f;
}

void LAYOUT_mapID_setChr(u8 *layout, int map_id, int unit_id)
{
    LayoutHeader *header;
    JavaField *field;
    u8 *peer;
    float *position;
    float *rotation;
    float angle;

    header = UnduDataGetHeader(map_id + 0x200, unit_id + 0x8000);
    if (header == 0)
        return;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(layout_peer, -1), 0);
    peer = *(u8 **)(layout + field->offset);
    position = (float *)(peer + 16);
    rotation = (float *)(peer + 80);
    position[0] = header->components[0];
    *(u16 *)(peer + 1232) |= 0x1000;
    position[1] = header->components[1];
    position[2] = header->components[2];
    angle = header->components[3];
    rotation[1] = angle;
    *(float *)(peer + 2532) = angle;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(layout_px, -1), 0);
    *(float *)(layout + field->offset) = position[0];
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(layout_py, -1), 0);
    *(float *)(layout + field->offset) = position[1];
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(layout_pz, -1), 0);
    *(float *)(layout + field->offset) = position[2];
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(layout_ry, -1), 0);
    *(float *)(layout + field->offset) =
        rotation[1] / layout_chr_pi * 180.0f;
}

INCLUDE_ASM("asm/main/nonmatchings/layout", LAYOUT_mapID_setEffect);

INCLUDE_ASM("asm/main/nonmatchings/layout", Java_xeno_util_Layout_set__Ljava_lang_Object_I);

INCLUDE_ASM("asm/main/nonmatchings/layout", Java_xeno_util_Layout_getManager__I);
