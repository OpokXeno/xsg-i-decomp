#include "common.h"
#include "map_2.h"
#include "main/xgl_2.h"
#include "main/map_2.h"
#include "main/mdl.h"
#include "main/nml_model_set.h"
#include "main/res_get_path.h"
#include "quadword.h"

#define NULL ((void *)0)

#define MAP_UNIT_RESOURCE_GROUP_MASK 0x7F00 /* fallback: the group's first resource */
#define MAP_UNIT_SERIAL_UNIT6003 0x6025     /* gets unit6003_update as typeUpdate */
#define MAP_UNIT_TEXMAP_SIZE_DEFAULT 16

/* MapUnitRecord.flags */
#define MAP_UNIT_FLAG_SUSPENDED 0x10 /* update is skipped */
#define MAP_UNIT_FLAG_SHADOW_MAP 0x20
#define MAP_UNIT_FLAG_CLIP 0x40

/* parentMatrixIndex: bit 15 marks a joint matrix, the low 15 bits index it. */
#define MAP_PARENT_MATRIX_JOINT 0x8000
#define MAP_PARENT_MATRIX_MASK 0x7FFF

struct MdlHandle;

extern MapUnitResource *RES_loadFile(int command, int callback, int resource_id, int flags);
extern void LOG(const char *format, ...);
extern int MDL_create(void *model_instance, void *resource_model);
extern void unit6003_update(MapUnitRecord *unit);
extern void nmlModelSetShadowMapEntry(void);
extern void nmlModelEntry(int entry);
/*
 * Loads the model for `resource_id`, falling back to the group's first resource
 * when that fails, and stores it in the unit. Returns MDL_create's result.
 */
int MAP_loadUnitResource(MapUnitRecord *unit, int resource_id)
{
    MapUnitResource *resource = RES_loadFile(RES_LOAD_ANY_BLOCK, RES_TYPE_MODEL_NO_JOINTS, resource_id, 0);
    void *model;
    const char *status;

    if (resource == 0) {
        resource = RES_loadFile(RES_LOAD_ANY_BLOCK, RES_TYPE_MODEL_NO_JOINTS, resource_id & MAP_UNIT_RESOURCE_GROUP_MASK, 0);
    }

    if (resource != 0) {
        model = resource->model;
        status = resource->status;
    } else {
        model = 0;
        status = 0;
    }

    if (model == 0) {
        /* Report the missing model resource. */
        /* JP: モデルが見つかりませんでした。\n
         * EN: Model not found.\n */
        LOG("\245\342\245\307\245\353\244\254\270\253\244\304\244\253\244\352\244\336\244\273\244\363\244\307\244\267\244\277\241\243\n");
    }

    unit->resourceStatus = status;
    unit->resourceModel = model;
    unit->model = model;
    return MDL_create(&unit->modelState, model);
}

/* Resets every record in MapUnit to its default state. */
void MAP_initUnit(void)
{
    MapUnitRecord *unit = MapUnit;
    int i;
    int j;

    for (i = 0; i < MAP_UNIT_COUNT; i++) {
        unit->flags = 0;
        unit->serial = -1;
        unit->update = 0;
        unit->draw = 0;
        unit->typeUpdate = 0;
        unit->resourceModel = 0;
        unit->sequence = 0;
        unit->scale.x = 1.0f;
        unit->scale.y = 1.0f;
        unit->scale.z = 1.0f;
        unit->scale.w = 1.0f;
        unit->position.x = 0;
        unit->position.y = 0;
        unit->position.z = 0;
        unit->position.w = 1.0f;
        unit->modelPosition[0] = 0;
        unit->modelPosition[1] = 0;
        unit->modelPosition[2] = 0;
        unit->modelPosition[3] = 1.0f;
        unit->render_command = 0;
        unit->filter_mode = 0;
        unit->texMapMode = 0;
        unit->texMapSize = MAP_UNIT_TEXMAP_SIZE_DEFAULT;
        unit->shadow_map_count = 0;
        unit->texMapWeight = 0.5f;
        unit->sort_offset = 0;
        unit->monitor_priority = 0;
        for (j = 0; j < 2; j++) {
            unit->cleared_d8[j] = 0;
        }
        for (j = 0; j < 2; j++) {
            unit->modelResourceWords[j] = 0;
        }
        for (j = 0; j < 3; j++) {
            unit->effectCf[j] = 0;
        }
        unit->parent = 0;
        unit->modelState = 0;
        unit++;
    }
}

/*
 * Claims the record at `slot` (the first free slot when `slot` is negative) for
 * `serial` and returns it, or returns 0 when no slot is free. Serial
 * MAP_UNIT_SERIAL_UNIT6003 also gets unit6003_update as its type update.
 */
MapUnitRecord *MAP_createUnit(int slot, int serial)
{
    MapUnitRecord *unit;
    int i;
    void (*typeUpdate)(MapUnitRecord *);

    if (slot < 0) {
        for (i = 0; i < MAP_UNIT_COUNT; i++) {
            if (MapUnit[i].serial < 0) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            return 0;
        }
    }
    unit = &MapUnit[slot];
    unit->slot = slot;
    unit->serial = serial;
    unit->scale.x = 1.0f;
    unit->scale.y = 1.0f;
    unit->scale.z = 1.0f;
    unit->scale.w = 1.0f;
    unit->modelPosition[0] = 0;
    unit->modelPosition[1] = 0;
    unit->modelPosition[2] = 0;
    unit->modelPosition[3] = 1.0f;
    unit->sequence = 0;
    if (serial == MAP_UNIT_SERIAL_UNIT6003) {
        typeUpdate = unit6003_update;
    } else {
        typeUpdate = 0;
    }
    unit->update = 0;
    unit->draw = 0;
        unit->actionSub = 0;
    unit->typeUpdate = typeUpdate;
    for (i = 2; i >= 0; i--) {
        unit->effectCf[i] = 0;
    }
    return unit;
}

/* Runs the update and type-update hooks of each live, unsuspended unit. */
void MAP_updateUnit(void)
{
    MapUnitRecord *unit = MapUnit;
    int i;

    for (i = 0; i < MAP_UNIT_COUNT; i++) {
        if (unit->serial >= 0 && !(unit->flags & MAP_UNIT_FLAG_SUSPENDED)) {
            if (unit->update != NULL) {
                unit->update(unit);
            }
            if (unit->typeUpdate != NULL) {
                unit->typeUpdate(unit);
            }
        }
        unit++;
    }
}

/*
 * Draws one map unit: builds its transform from position, rotation and scale,
 * combined with a joint or root matrix of its parent when it has one, sets the
 * model's render state, then draws the model and calls the unit's draw hook.
 */
void MAP_drawUnitAt(MapUnitRecord *unit)
{
    Matrix4 place;
    Matrix4 joint;
    Matrix4 root;
    Matrix4 *matrices;
    int matrixIndex;
    int model;
    int i;
    unsigned short *shadowPart;
    void (*draw)(MapUnitRecord *);
    void *modelState;
    float homogeneousZero;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackScale(&unit->scale.x);
    xglMatrixStackSave(place);
    if (unit->parent != 0) {
        matrixIndex = unit->parentMatrixIndex;
        matrices = unit->parent->matrices;
        if (matrixIndex >= 0) {
            /* The matrices are copied with quadword moves (QuadwordCopyMatrix). */
            if (matrixIndex & MAP_PARENT_MATRIX_JOINT) {
                QuadwordCopyMatrix(joint, (matrixIndex & MAP_PARENT_MATRIX_MASK) * sizeof(Matrix4) + (unsigned int)matrices);
                homogeneousZero = 0.0f;
                joint[2][3] = homogeneousZero;
                joint[1][3] = homogeneousZero;
                joint[0][3] = homogeneousZero;
                xglMatrixStackLoad(joint);
            } else {
                QuadwordCopyMatrix(root, matrices[0]);
                QuadwordCopyMatrix(joint, (unit->parentMatrixIndex & MAP_PARENT_MATRIX_MASK) * sizeof(Matrix4) + (unsigned int)matrices);
                homogeneousZero = 0.0f;
                joint[2][3] = homogeneousZero;
                joint[1][3] = homogeneousZero;
                joint[0][3] = homogeneousZero;
                root[2][3] = homogeneousZero;
                root[1][3] = homogeneousZero;
                root[0][3] = homogeneousZero;
                xglMatrixStackLoad(root);
                xglMatrixStackMul(joint);
            }
            xglMatrixStackMul(place);
            xglMatrixStackSave(place);
        }
    }
    model = unit->modelEntry;
    if (model != 0) {
        /* Model state and render settings, then the model entry itself. */
        if (!(unit->flags & 4)) {
            if (unit->resourceStatus != 0) {
                nmlModelSetTexture(unit->resourceStatus);
            }
            modelState = &unit->modelState;
            MDL_partsSetVisible(modelState);
            nmlModelSetPlace(&place);
            nmlModelSetRenderLevel(unit->render_command);
            nmlModelSetSortOffset((float)unit->sort_offset);
            if (unit->flags & MAP_UNIT_FLAG_CLIP) {
                nmlModelSetClip(1);
            }
            i = 0;
            if (unit->shadow_map_count > 0) {
                shadowPart = unit->shadow_map_ids;
                do {
                    nmlModelSetMapShadowParts(*shadowPart++);
                    i++;
                } while (i < unit->shadow_map_count);
            }
            switch (unit->texMapMode) {
            case 5:
                nmlModelSetTexMap(1, unit->texMapSize, unit->texMapWeight);
                break;
            case 10:
                nmlModelSetTexMap(0x40001, unit->texMapSize, unit->texMapWeight);
                break;
            case 6:
                nmlModelSetTexMap(0x40003, unit->texMapSize, unit->texMapWeight);
                break;
            case 7:
                nmlModelSetTexMap(0x10001, unit->texMapSize, unit->texMapWeight);
                break;
            case 8:
                nmlModelSetTexMap(0x20001, unit->texMapSize, unit->texMapWeight);
                break;
            case 9:
                nmlModelSetTexMap(0x30000, unit->texMapSize, unit->texMapWeight);
                break;
            }
            if (unit->filter_mode != 0) {
                nmlModelSetToumei(1);
                nmlModelSetZwrite(1);
                nmlModelSetStencil(1);
                nmlModelSetFilter(unit->filter_mode, unit->filterStart, unit->filterEnd);
                nmlModelSetTransparency(unit->transparency);
                nmlModelSetReflTransparency(unit->reflTransparency);
            }
            if (unit->flags & MAP_UNIT_FLAG_SHADOW_MAP) {
                nmlModelSetShadowMapEntry();
            }
            nmlModelEntry(model);
            nmlModelGetGblPosition(&unit->globalPosition);
        }
        draw = unit->draw;
        if (draw != 0) {
            xglMatrixStackPush();
            draw(unit);
            xglMatrixStackPop(1);
        }
    }
}

/* Draws every unit with a nonnegative serial. */
void MAP_drawUnit(void)
{
    MapUnitRecord *unit = MapUnit;
    int i;

    for (i = 0; i < MAP_UNIT_COUNT; i++) {
        if (unit->serial >= 0) {
            MAP_drawUnitAt(unit);
        }
        unit++;
    }
}

/* Sets position->y to the ground height at x and z when the lookup finds ground; otherwise leaves y unchanged. */
void MAP_getHeight(MapPosition *position)
{
    float height = UnduGet(position->x, position->z);
    int found = 1;

    if (height == GROUND_HEIGHT_NONE) {
        found = 0;
    }
    if (found) {
        position->y = height;
    }
}
