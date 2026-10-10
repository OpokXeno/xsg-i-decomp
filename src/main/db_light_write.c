#include "common.h"

static int mode_004DC5A8;

#include "shared.h"

#include "db_light_write.h"

/*
 * The retail ELF has a GLOBAL OBJECT named cursor at 0x0099C7F0 with size
 * 0x60. The shared declaration and this TU's uses establish a 16-byte
 * HomogeneousVector slot at +0x10; the callback pair is accessed as raw
 * words at +0x50/+0x54. Six slots of the existing vector type express the
 * recorded object extent without assigning meanings to the other words.
 */

typedef struct CursorState {
    int mode;
    unsigned char unmodeled_04[0x0C];
    HomogeneousVector position;
    unsigned char unmodeled_20[0x30];
    CursorCallback callback;
    void *callback_argument;
    unsigned char unmodeled_58[8];
} CursorState;

HomogeneousVector cursor[6];

#include "main/xgl_studio.h"

/* Owner declarations used by these debug views; storage remains original scaffolding. */

typedef struct {
    unsigned char unmodeled_00[0x50];
    unsigned int matrix_data_offset;
} MapPartListView;

typedef struct {
    unsigned char unmodeled_00[4];
    MapPartListView *parts;
} MapStageModelView;

typedef struct {
    unsigned char unmodeled_00[0x54];
    MapStageModelView *model;
} MapGameLoopStateView;

typedef struct {
    unsigned char unmodeled_00[0x28];
    unsigned short half_28;
    unsigned short half_2a;
    unsigned short half_2c;
    unsigned char unmodeled_2e[0x66 - 0x2e];
    signed char stick_x;
    signed char stick_y;
} DbLightWritePadSecondPrefix;

typedef struct {
    unsigned char unmodeled_00[0x28];
    unsigned short half_28;
    unsigned short half_2a;
    unsigned char unmodeled_2c[0x68 - 0x2c];
    DbLightWritePadSecondPrefix second;
} DbLightWritePadData;

extern MapGameLoopStateView GameLoopState;

extern DbLightWritePadData PadData;

extern int visible_0;

extern int partsIndex;

extern const char D_004C2158[];

extern const char D_004DA520[];

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern void nmlModelSetPartsVisible(MapPartListView *parts,
                                    int part_index, int visible);

extern Vector4 lightDir[6];

extern Vector4 lightCol[6];

extern void xglLightIntensityParallel(void *light, int index,
                                      const Vector4 *intensity);

extern void xglLightIntensityAmbient(void *light,
                                     const Vector4 *intensity);

extern void xglLightDirection(void *light, unsigned int index,
                              const Vector4 *direction);

extern float atan2f(float y, float x);

extern float xglSin(float angle);

extern float xglCos(float angle);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", DB_lightWrite);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawRect2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawBox2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawCircle2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawVector2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawVector4S);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawGrid);

static StudioCamera *getCurrentCamera(void)
{
    int camera_index;
    StudioCamera *camera;

    camera_index = 0;
    for (;;) {
        xglStudioGetCamera(&camera, camera_index);
        camera_index++;
        if (camera->active != 0)
            return camera;
        if (camera_index >= 8)
            return 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawAxis);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawCircle3);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawTags);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawVector4);

float ball2point(const Vector4 *cursor_position,
                 const Vector4 *actor_position, float radius)
{
    Vector4 difference;
    float distance;

    difference.x = cursor_position->x - actor_position->x;
    difference.y = cursor_position->y - actor_position->y;
    difference.z = cursor_position->z - actor_position->z;
    xglVectorLength(&distance, &difference);
    if (distance < radius)
        return distance;
    return -1.0f;
}

void *prevActor(int startIndex)
{
    int i;

    for (i = startIndex; i >= 0; i--) {
        if (actor[i].inUseId != 0 && !(actor[i].flags & 8)) {
            return &actor[i];
        }
    }
    return 0;
}

ActorHead *nextActor(register int startIndex)
{
    register ActorHead *entry;
    ActorHead *end;

    if (startIndex < ACTOR_COUNT) {
        startIndex *= sizeof(ActorHead);
        entry = (ActorHead *)((char *)actor + startIndex);
        end = &actor[ACTOR_COUNT];
        for (;;) {
            if (entry->inUseId != 0 && !(entry->flags & 8)) {
                return entry;
            }
            entry++;
            if ((int)entry >= (int)end) {
                break;
            }
        }
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode1);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorModePlane);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode0);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", changeCameraMode);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursor);

void drawCursor(void)
{
    Matrix4 matrix;
    HomogeneousVector *position;

    position = &cursor[1];
    position->w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&position->x);
    xglMatrixStackSave(matrix);
    drawAxis(matrix, 1.0f);
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initCursor);

static void printM(int x, int y, const float *matrix)
{
    char buffer[256];
    const float *rowEnd;
    int rowIndex;

    rowEnd = matrix + 3;
    rowIndex = 0;
    do {
        rowIndex++;
        sprintf(buffer, D_004C2158,
                rowEnd[-3], rowEnd[-2], rowEnd[-1], rowEnd[0]);
        xglFontDebugPrintf(x, y, buffer);
        if (rowIndex > 3)
            break;
        rowEnd += 4;
        y += 8;
    } while (1);
}

void MAP_serach(void)
{
    MapStageModelView *model;
    MapPartListView *parts;
    DbLightWritePadSecondPrefix *mapPad;
    unsigned char *matrix_base;
    void *matrix_data;
    char buffer[256];

    model = GameLoopState.model;
    mapPad = &PadData.second;
    visible_0++;
    if (model != 0) {
        parts = model->parts;
        if (parts != 0) {
            matrix_base = (unsigned char *)parts +
                          parts->matrix_data_offset;
            if (mapPad->half_2c & 0x4000) {
                nmlModelSetPartsVisible(parts, partsIndex, 1);
                visible_0 = 0;
                partsIndex--;
            }

            if (mapPad->half_2c & 0x1000) {
                nmlModelSetPartsVisible(model->parts,
                                        partsIndex, 1);
                visible_0 = 0;
                partsIndex++;
            }

            if (partsIndex < 0)
                partsIndex = 0;
            matrix_data = matrix_base + partsIndex * sizeof(Matrix4);
            printM(0x20, 0x40, matrix_data);
            sprintf(buffer, D_004DA520, partsIndex);
            xglFontDebugPrintf(8, 0x20, buffer);
            nmlModelSetPartsVisible(model->parts, partsIndex,
                                    (visible_0 >> 4) & 1);
        }
    }
}

void EvtTools(void)
{
    if ((PadData.half_2a & 0x10) && (PadData.half_28 & 0x100)) {
        mode_004DC5A8 = (mode_004DC5A8 + 1) & 1;
    }
    MAP_serach();
    if (mode_004DC5A8 == 1) {
        JTHREAD_cntl();
        PLAY_ctrl();
    }
    TCAMERA_update();
    if (mode_004DC5A8 == 1) {
        ACT_update();
        MAP_updateUnit();
    }
    updateCursor(0);
    drawCursor();
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight3);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight2);

void initLight(void)
{
    StudioLight *light;
    Vector4 direction;
    int lightIndex;

    xglStudioGetLight(&light);
    lightIndex = 0;
    do {
        Vector4 *source = &lightDir[lightIndex];

        if (__builtin_sqrtf((source->x * source->x) +
                            (source->y * source->y)) > 1.0f) {
            float angle = atan2f(source->x, source->y);
            source->x = xglSin(angle);
            source->y = xglCos(angle);
        }

        direction.x = source->x;
        direction.y = source->y;
        direction.z = __builtin_sqrtf(
            1.0f - ((direction.x * direction.x) +
                    (direction.y * direction.y)));
        if (source->z < 0.0f)
            direction.z = -direction.z;

        xglLightIntensityParallel(light, lightIndex, &lightCol[lightIndex]);
        xglLightDirection(light, lightIndex, &direction);
    } while (++lightIndex < 3);

    xglLightIntensityAmbient(light, &lightCol[3]);
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateLight);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateWind);

void VW_setCursorMode(int mode)
{
    CursorState *state = (void *)cursor;

    state->mode = mode;
    changeCameraMode();
}

void VW_setCursorFunc(CursorCallback callback, void *argument)
{
    CursorState *state = (void *)cursor;

    state->callback = callback;
    state->callback_argument = argument;
}

void VW_setCursor(const Vector4 *position)
{
    cursor[1].x = position->x;
    cursor[1].y = position->y;
    cursor[1].z = position->z;
}

void VW_getCursor(HomogeneousVector *destination)
{
    destination->x = cursor[1].x;
    destination->y = cursor[1].y;
    destination->z = cursor[1].z;
    destination->w = 1.0f;
}
