#include "common.h"

#include "map_2.h"

extern MapUnitResource *RES_loadFile(int command, int callback, int resource_id, int flags);

extern void LOG(const char *format, ...);

extern int MDL_create(void *model_instance, void *resource_model);

#define NULL ((void *)0)

#include "main/xgl_2.h"

extern void unit6003_update(MapUnitSlot *unit);

struct MdlHandle;

extern void MDL_partsSetVisible(struct MdlHandle *model);

extern void nmlModelSetTexture(const char *texture);

extern void nmlModelSetPlace(const Vector4 *place);

extern void nmlModelSetRenderLevel(int level);

extern void nmlModelSetSortOffset(float offset);

extern void nmlModelSetClip(int enabled);

extern void nmlModelSetMapShadowParts(unsigned short part);

extern void nmlModelSetTexMap(int mode, int size, float weight);

extern void nmlModelSetToumei(int enabled);

extern void nmlModelSetZwrite(int enabled);

extern void nmlModelSetStencil(int enabled);

extern void nmlModelSetFilter(int filter, float start, float end);

extern void nmlModelSetTransparency(float transparency);

extern void nmlModelSetReflTransparency(float transparency);

extern void nmlModelSetShadowMapEntry(void);

extern void nmlModelEntry(int entry);

extern void nmlModelGetGblPosition(Vector4 *position);

extern void xglMatrixStackLoad(float matrix[4][4]);

extern void xglMatrixStackMul(const Matrix4 matrix);

extern void xglMatrixStackPush(void);

extern void xglMatrixStackPop(int count);




int MAP_loadUnitResource(MapUnitSlot *unit, int resource_id)
{
    MapUnitResource *resource = RES_loadFile(-1, 4, resource_id, 0);
    void *model;
    int status;

    if (resource == 0) {
        resource = RES_loadFile(-1, 4, resource_id & 0x7F00, 0);
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
        LOG("\245\342\245\307\245\353\244\254\270\253\244\304\244\253\244\352\244\336\244\273\244\363\244\307\244\267\244\277\241\243\n");
    }

    unit->resource_status = status;
    unit->resource_model = model;
    unit->model = model;
    return MDL_create(&unit->model_state, model);
}

void MAP_initUnit(void)
{
    MapUnitSlot *unit = MapUnit;
    int i;
    int j;

    for (i = 0; i < 64; i++) {
        unit->flags = 0;
        unit->serial = -1;
        unit->update = 0;
        unit->draw = 0;
        unit->typeUpdate = 0;
        unit->resource_model = 0;
        unit->sequence = 0;
        unit->scale[0] = 1.0f;
        unit->scale[1] = 1.0f;
        unit->scale[2] = 1.0f;
        unit->scale[3] = 1.0f;
        unit->position[0] = 0;
        unit->position[1] = 0;
        unit->position[2] = 0;
        unit->position[3] = 1.0f;
        unit->modelPosition[0] = 0;
        unit->modelPosition[1] = 0;
        unit->modelPosition[2] = 0;
        unit->modelPosition[3] = 1.0f;
        unit->renderLevel = 0;
        unit->filter = 0;
        unit->texMapMode = 0;
        unit->texMapSize = 16;
        unit->shadowPartCount = 0;
        unit->texMapWeight = 0.5f;
        unit->sortOffset = 0;
        unit->cleared_a6 = 0;
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
        unit->model_state = 0;
        unit++;
    }
}

MapUnitSlot *MAP_createUnit(int slot, int serial)
{
    MapUnitSlot *unit;
    int i;
    void (*typeUpdate)(MapUnitSlot *);

    if (slot < 0) {
        for (i = 0; i < 64; i++) {
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
    unit->scale[0] = 1.0f;
    unit->scale[1] = 1.0f;
    unit->scale[2] = 1.0f;
    unit->scale[3] = 1.0f;
    unit->modelPosition[0] = 0;
    unit->modelPosition[1] = 0;
    unit->modelPosition[2] = 0;
    unit->modelPosition[3] = 1.0f;
    unit->sequence = 0;
    if (serial == 0x6025) {
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

void MAP_updateUnit(void)
{
    MapUnitSlot *unit = MapUnit;
    int i;

    for (i = 0; i < 64; i++) {
        if (unit->serial >= 0 && !(unit->flags & 0x10)) {
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

void MAP_drawUnitAt(MapUnitSlot *unit)
{
    Matrix4 place;
    Matrix4 joint;
    Matrix4 root;
    Matrix4 *matrices;
    int matrixIndex;
    int model;
    int i;
    unsigned short *shadowPart;
    void (*draw)(MapUnitSlot *);
    void *modelState;
    float homogeneousZero;

    xglMatrixStackUnit();
    xglMatrixStackTrans(unit->position);
    xglMatrixStackRotX(unit->rotation[0]);
    xglMatrixStackRotY(unit->rotation[1]);
    xglMatrixStackRotZ(unit->rotation[2]);
    xglMatrixStackScale(unit->scale);
    xglMatrixStackSave(place);
    if (unit->parent != 0) {
        matrixIndex = unit->parentMatrixIndex;
        matrices = unit->parent->matrices;
        if (matrixIndex >= 0) {
            /* Aligned matrix copies use the SDK's scratch-register quartet. */
            if (matrixIndex & 0x8000) {
                __asm__ __volatile__(
                    "lq $2, 0(%1)\n"
                    "sq $2, 0(%0)\n"
                    "lq $2, 16(%1)\n"
                    "sq $2, 16(%0)\n"
                    "lq $2, 32(%1)\n"
                    "sq $2, 32(%0)\n"
                    "lq $2, 48(%1)\n"
                    "sq $2, 48(%0)\n"
                    : : "r" (joint), "r" ((matrixIndex & 0x7FFF) * sizeof(Matrix4) + (unsigned int)matrices)
                    : "$2", "memory");
                homogeneousZero = 0.0f;
                joint[2][3] = homogeneousZero;
                joint[1][3] = homogeneousZero;
                joint[0][3] = homogeneousZero;
                xglMatrixStackLoad(joint);
            } else {
                __asm__ __volatile__(
                    "lq $2, 0(%1)\n"
                    "sq $2, 0(%0)\n"
                    "lq $2, 16(%1)\n"
                    "sq $2, 16(%0)\n"
                    "lq $2, 32(%1)\n"
                    "sq $2, 32(%0)\n"
                    "lq $2, 48(%1)\n"
                    "sq $2, 48(%0)\n"
                    : : "r" (root), "r" (matrices[0])
                    : "$2", "memory");
                __asm__ __volatile__(
                    "lq $2, 0(%1)\n"
                    "sq $2, 0(%0)\n"
                    "lq $2, 16(%1)\n"
                    "sq $2, 16(%0)\n"
                    "lq $2, 32(%1)\n"
                    "sq $2, 32(%0)\n"
                    "lq $2, 48(%1)\n"
                    "sq $2, 48(%0)\n"
                    : : "r" (joint), "r" ((unit->parentMatrixIndex & 0x7FFF) * sizeof(Matrix4) + (unsigned int)matrices)
                    : "$2", "memory");
                homogeneousZero = 0.0f;
                joint[2][3] = homogeneousZero;
                joint[1][3] = homogeneousZero;
                joint[0][3] = homogeneousZero;
                root[2][3] = homogeneousZero;
                root[1][3] = homogeneousZero;
                root[0][3] = homogeneousZero;
                xglMatrixStackLoad(root);
                xglMatrixStackMul((const float (*)[4])joint);
            }
            xglMatrixStackMul((const float (*)[4])place);
            xglMatrixStackSave(place);
        }
    }
    model = (int)unit->model;
    if (model != 0) {
        if (!(unit->flags & 4)) {
            if (unit->resource_status != 0) {
                nmlModelSetTexture((const char *)unit->resource_status);
            }
            modelState = &unit->model_state;
            MDL_partsSetVisible(modelState);
            nmlModelSetPlace((const Vector4 *)place);
            nmlModelSetRenderLevel(unit->renderLevel);
            nmlModelSetSortOffset((float)unit->sortOffset);
            if (unit->flags & 0x40) {
                nmlModelSetClip(1);
            }
            i = 0;
            if (unit->shadowPartCount > 0) {
                shadowPart = unit->shadowParts;
                do {
                    nmlModelSetMapShadowParts(*shadowPart++);
                    i++;
                } while (i < unit->shadowPartCount);
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
            if (unit->filter != 0) {
                nmlModelSetToumei(1);
                nmlModelSetZwrite(1);
                nmlModelSetStencil(1);
                nmlModelSetFilter(unit->filter, unit->filterStart, unit->filterEnd);
                nmlModelSetTransparency(unit->transparency);
                nmlModelSetReflTransparency(unit->reflTransparency);
            }
            if (unit->flags & 0x20) {
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

void MAP_drawUnit(void)
{
    MapUnitSlot *unit = MapUnit;
    int i;

    for (i = 0; i < 64; i++) {
        if (unit->serial >= 0) {
            MAP_drawUnitAt(unit);
        }
        unit++;
    }
}

void MAP_getHeight(MapPosition *position)
{
    float height = UnduGet(position->x, position->z);
    int found = 1;

    if (height == -1000.0f) {
        found = 0;
    }
    if (found) {
        position->y = height;
    }
}


