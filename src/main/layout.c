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

void LAYOUT_mapID_setEffect(u8 *layout, int map_id, int unit_id)
{
    LayoutHeader *header;
    JavaField *field;
    LayoutEffectPeer *peer;

    header = UnduDataGetHeader(map_id + 0x200, unit_id + 0x8000);
    if (header == 0)
        return;

    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString(layout_peer, -1), 0);
    peer = *(LayoutEffectPeer **)(layout + field->offset);
    if (peer != 0) {
        peer->position[0] = header->components[0];
        peer->position[1] = header->components[1];
        peer->position[2] = header->components[2];
        peer->rotation_y = header->components[3];
    }

    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString(layout_px, -1), 0);
    *(float *)(layout + field->offset) = header->components[0];
    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString(layout_py, -1), 0);
    *(float *)(layout + field->offset) = header->components[1];
    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString(layout_pz, -1), 0);
    *(float *)(layout + field->offset) = header->components[2];
    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString(layout_ry, -1), 0);
    *(float *)(layout + field->offset) =
        header->components[3] / D_004D83A4 * 180.0f;
}

void Java_xeno_util_Layout_set__Ljava_lang_Object_I(JThread *thread,
                                                     LayoutSetArgs *arguments)
{
    int unit_id;
    LayoutMapIdObject *map;
    u8 *target;

    unit_id = arguments->unit_id;
    map = arguments->map;
    target = arguments->target;

    if (JNI_isInstanceOf(target, classJava_xeno_Chr) == 1) {
        LAYOUT_mapID_setChr(target, map->map_id, unit_id);
        return;
    }
    if (JNI_isInstanceOf(target, classJava_xeno_Unit) == 1) {
        LAYOUT_mapID_setUnit(target, map->map_id, unit_id);
        return;
    }
    if (JNI_isInstanceOf(target, classJava_xeno_Effect) == 1) {
        LAYOUT_mapID_setEffect(target, map->map_id, unit_id);
        return;
    }
}

void Java_xeno_util_Layout_getManager__I(JThread *thread, int *id,
                                         void **result)
{
    int *manager = &defaultLayout;

    *manager = *(int *)((u8 *)classJava_xeno_util_Layout + 0x18);
    D_004DC6B4 = *id;
    *result = manager;
}
