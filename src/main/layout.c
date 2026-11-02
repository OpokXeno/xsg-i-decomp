#include "common.h"
#include "layout.h"
#include "main/jni.h"

static int defaultLayout;

int layoutManagerId;

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
                             loadConstString("peer", -1), 0);
    peer = *(u8 **)(layout + field->offset);
    position = (float *)(peer + 16);
    rotation = (float *)(peer + 32);
    position[0] = header->locator.x;
    position[1] = header->locator.y;
    position[2] = header->locator.z;
    rotation[1] = header->locator.w;

    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString("px", -1), 0);
    *(float *)(layout + field->offset) = position[0];
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString("py", -1), 0);
    *(float *)(layout + field->offset) = position[1];
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString("pz", -1), 0);
    *(float *)(layout + field->offset) = position[2];
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString("ry", -1), 0);
    *(float *)(layout + field->offset) =
        rotation[1] / 3.1415927f * 180.0f;
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
                             loadConstString("peer", -1), 0);
    peer = *(u8 **)(layout + field->offset);
    position = (float *)(peer + 16);
    rotation = (float *)(peer + 80);
    position[0] = header->locator.x;
    *(u16 *)(peer + 1232) |= 0x1000;
    position[1] = header->locator.y;
    position[2] = header->locator.z;
    angle = header->locator.w;
    rotation[1] = angle;
    *(float *)(peer + 2532) = angle;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString("px", -1), 0);
    *(float *)(layout + field->offset) = position[0];
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString("py", -1), 0);
    *(float *)(layout + field->offset) = position[1];
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString("pz", -1), 0);
    *(float *)(layout + field->offset) = position[2];
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString("ry", -1), 0);
    *(float *)(layout + field->offset) =
        rotation[1] / 3.1415927f * 180.0f;
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
                             loadConstString("peer", -1), 0);
    peer = *(LayoutEffectPeer **)(layout + field->offset);
    if (peer != 0) {
        peer->position[0] = header->locator.x;
        peer->position[1] = header->locator.y;
        peer->position[2] = header->locator.z;
        peer->rotation_y = header->locator.w;
    }

    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString("px", -1), 0);
    *(float *)(layout + field->offset) = header->locator.x;
    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString("py", -1), 0);
    *(float *)(layout + field->offset) = header->locator.y;
    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString("pz", -1), 0);
    *(float *)(layout + field->offset) = header->locator.z;
    field = lookupClassField(classJava_xeno_Effect,
                             loadConstString("ry", -1), 0);
    *(float *)(layout + field->offset) =
        header->locator.w / 3.1415927f * 180.0f;
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

    *manager = (int)classJava_xeno_util_Layout->classPointer;
    layoutManagerId = *id;
    *result = manager;
}
