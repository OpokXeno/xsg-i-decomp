#include "common.h"
#include "layout.h"
#include "main/jni.h"

/* UnduDataGetHeader keys: the map id is offset into the Undu map index space and the unit id is flagged for a by-index lookup. */
#define LAYOUT_UNDU_MAP_BASE 0x200
#define LAYOUT_UNDU_UNIT_FLAG 0x8000

/* Byte offsets inside a unit / character peer object. */
#define LAYOUT_PEER_POSITION_OFFSET 16
#define LAYOUT_UNIT_PEER_ROTATION_OFFSET 32
#define LAYOUT_CHR_PEER_ROTATION_OFFSET 80

static int defaultLayout;

int layoutManagerId;

/*
 * Copies the layout entry for unit_id into a Unit object: the peer's position
 * and yaw, then the px/py/pz/ry fields (ry in degrees).
 */
void LAYOUT_mapID_setUnit(u8 *layout, int map_id, int unit_id)
{
    LayoutHeader *header;
    JavaField *field;
    u8 *peer;
    float *position;
    float *rotation;

    header = UnduDataGetHeader(map_id + LAYOUT_UNDU_MAP_BASE, unit_id + LAYOUT_UNDU_UNIT_FLAG);
    if (header == 0)
        return;

    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString("peer", -1), 0);
    peer = *(u8 **)(layout + field->offset);
    position = (float *)(peer + LAYOUT_PEER_POSITION_OFFSET);
    rotation = (float *)(peer + LAYOUT_UNIT_PEER_ROTATION_OFFSET);
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

/*
 * Copies the layout entry for unit_id into a Chr object: the peer's position
 * and yaw (also set at peer+2532), then the px/py/pz/ry fields (ry in degrees).
 * Sets flag 0x1000 in the peer at +1232.
 */
void LAYOUT_mapID_setChr(u8 *layout, int map_id, int unit_id)
{
    LayoutHeader *header;
    JavaField *field;
    u8 *peer;
    float *position;
    float *rotation;
    float angle;

    header = UnduDataGetHeader(map_id + LAYOUT_UNDU_MAP_BASE, unit_id + LAYOUT_UNDU_UNIT_FLAG);
    if (header == 0)
        return;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString("peer", -1), 0);
    peer = *(u8 **)(layout + field->offset);
    position = (float *)(peer + LAYOUT_PEER_POSITION_OFFSET);
    rotation = (float *)(peer + LAYOUT_CHR_PEER_ROTATION_OFFSET);
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

/*
 * Copies the layout entry for unit_id into an Effect object: the peer's position
 * and yaw when the peer exists, then the px/py/pz/ry fields (ry in degrees).
 */
void LAYOUT_mapID_setEffect(u8 *layout, int map_id, int unit_id)
{
    LayoutHeader *header;
    JavaField *field;
    LayoutEffectPeer *peer;

    header = UnduDataGetHeader(map_id + LAYOUT_UNDU_MAP_BASE, unit_id + LAYOUT_UNDU_UNIT_FLAG);
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

/*
 * Native for Layout.set(Object, int): applies the layout entry for unit_id to
 * the target when it is a Chr, Unit or Effect; other classes are ignored.
 */
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

/* Native for Layout.getManager(int): stores the id and returns the static defaultLayout object. */
void Java_xeno_util_Layout_getManager__I(JThread *thread, int *id,
                                         void **result)
{
    int *manager = &defaultLayout;

    *manager = (int)classJava_xeno_util_Layout->classPointer;
    layoutManagerId = *id;
    *result = manager;
}
