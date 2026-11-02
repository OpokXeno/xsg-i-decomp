/*
 * OV02 original TU 6: 0x00a02d68..0x00a03450 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/xgl_packet.h"
#include "main/xgl_render.h"
#include "main/xgl_task.h"
#include "ov02/ov02.h"
#include "ov02/title_hdd_install.h"
#include "ov02/umn_interface2.h"

/* tskUmnBgCubeMain states. */
#define UMN_BG_CUBE_INIT 0
#define UMN_BG_CUBE_RUN 1
#define UMN_BG_CUBE_END 99

/* Both intensity counters start at UMN_IF2_INTENSITY_MAX and drop by their own step each frame. */
#define UMN_IF2_INTENSITY_MAX 0x80
#define UMN_IF2_FADE_STEP_A 8
#define UMN_IF2_FADE_STEP_B 2

/* Vertices of the strip DrawUmnInterface2 builds. */
#define UMN_IF2_VERTEX_COUNT 6

extern float xglFRand(void);
extern void nmlModelSetLight(void *model, void *light);
extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetPlace(const Vector4 *place);
extern void nmlModelEntry(int entry);
extern void xglCameraInit(StudioCamera *camera);

/* The original vector has component and doubleword-copy views. */
typedef union UmnBgCubeVec {
    Vector4 v;
    float f[4];
    u64 words[2];
    XglLightQuadword quadword;
} UmnBgCubeVec;

typedef struct UmnBgCubeLights {
    UmnBgCubeVec v[7];
} UmnBgCubeLights;

typedef struct UmnBgCubeData {
    UmnBgCubeVec position;
    UmnBgCubeVec rotation;
    UmnBgCubeVec spare;
    UmnBgCubeVec scale;
    Matrix4 matrix;
} UmnBgCubeData;

typedef struct UmnBgCubeMainTask {
    XglTaskPrefix base;
    unsigned char state;
    unsigned char enabled;
    unsigned char camera_id;
    unsigned char unmodeled_13;
    int texture;
    int model;
    UmnBgCubeData *data;
} UmnBgCubeMainTask;

typedef struct UmnIf2Coordinates {
    int values[6];
} UmnIf2Coordinates;

typedef struct UmnIf2Rgba {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} UmnIf2Rgba;

/* UmnInterface2Init allocates these twelve bytes. UmnInterface2Main fades
 * the two alpha bytes, then queues this record as the callback argument. */
typedef struct UmnIf2Parameters {
    unsigned int depth;
    UmnIf2Rgba first_vertex;
    UmnIf2Rgba remaining_vertices;
} UmnIf2Parameters;

extern UmnBgCubeVec D_00A11E60;
extern const UmnBgCubeLights D_00A11E70;
extern const UmnIf2Coordinates D_00A11EE0;
extern const UmnIf2Coordinates D_00A11EF8;

void tskUmnBgCubeMain(XglTaskPrefix *task_opaque)
{
    UmnBgCubeMainTask *task = (UmnBgCubeMainTask *)task_opaque;
    UmnBgCubeVec *position = &task->data->position;
    UmnBgCubeVec *rotation = &task->data->rotation;
    UmnBgCubeVec *spare = &task->data->spare;
    UmnBgCubeVec *scale = &task->data->scale;
    float (*matrix)[4] = task->data->matrix;
    UmnBgCubeVec zero;
    UmnBgCubeVec scale_init;
    StudioLight work;
    UmnBgCubeLights lights;
    StudioCamera *camera;
    int i;

    if (UmnWork.mode == UMN_MODE_NONE)
        task->state = UMN_BG_CUBE_END;

    switch (task->state) {
    case UMN_BG_CUBE_INIT:
        task->state = UMN_BG_CUBE_RUN;
        task->enabled = 1;
        memset(&zero, 0, 16);
        zero.v.w = 1.0f;
        scale_init = D_00A11E60;
        *position = zero;
        *rotation = zero;
        *spare = zero;
        *scale = scale_init;
        rotation->v.x = xglFRand();
        rotation->v.y = xglFRand();
        rotation->v.z = xglFRand();
        spare->v.x = xglFRand();
        spare->v.y = xglFRand();
        spare->v.z = xglFRand();
        scale->v.z = 7.5f;
        scale->v.y = 7.5f;
        scale->v.x = 7.5f;
    case UMN_BG_CUBE_RUN:
        for (i = 0; i < 3; i++)
            rotation->f[i] += 0.001f;
        break;
    case UMN_BG_CUBE_END:
        xglTaskWaitRemove(task_opaque);
        return;
    }
    if (task->enabled == 0)
        return;
    camera = xglStudioGetCamera2(task->camera_id);
    camera->active = 1;
    xglMatrixStackUnit();
    xglMatrixStackScale(scale->f);
    xglMatrixStackTrans(position->f);
    xglMatrixStackRotZ(rotation->v.z);
    xglMatrixStackRotY(rotation->v.y);
    xglMatrixStackRotX(rotation->v.x);
    xglMatrixStackSave(matrix);
    lights = D_00A11E70;
    xglLightIntensityAmbient(&work, &lights.v[0].quadword);
    xglLightIntensityParallel(&work, 0, &lights.v[1].quadword);
    xglLightDirection(&work, 0, &lights.v[4].v);
    xglLightIntensityParallel(&work, 1, &lights.v[2].quadword);
    xglLightDirection(&work, 1, &lights.v[5].v);
    xglLightIntensityParallel(&work, 2, &lights.v[3].quadword);
    xglLightDirection(&work, 2, &lights.v[6].v);
    xglLightCalcMatrix(&work);
    nmlModelSetLight(&work.color_modifier_matrix, &work.normal_matrix);
    nmlModelSetTexture((const char *)task->texture);
    nmlModelSetPlace((const Vector4 *)matrix);
    nmlModelEntry(task->model);
}

/*
 * Creates the background-cube task on task_xmx, gives it the 0x80-byte
 * UmnBgCubeData that starts at the next 16-byte boundary of `memory`, and
 * sets up studio camera 0 for it. Returns the first byte after that data.
 */
void *UmnBgCubeInit(void *memory)
{
    unsigned int address = ((unsigned int)memory + 15) & ~15u;
    XglTaskScheduler *scheduler = task_xmx;
    UmnBgCubeMainTask *task;
    StudioCamera *camera;

    task = (UmnBgCubeMainTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))tskUmnBgCubeMain,
        scheduler != 0 ? scheduler->active_tail : 0);
    task->data = (UmnBgCubeData *)address;
    address += sizeof(UmnBgCubeData);
    task->state = UMN_BG_CUBE_INIT;
    task->enabled = 0;
    camera = xglStudioGetCamera2(0);
    task->camera_id = 0;
    xglCameraInit(camera);
    (&camera->rotation.vector)->x = 0.0f;
    (&camera->rotation.vector)->y = 0.0f;
    (&camera->rotation.vector)->z = 0.0f;
    (&camera->position.vector)->x = 0.0f;
    (&camera->position.vector)->y = 0.0f;
    (&camera->position.vector)->z = 4.0f;
    camera->fov = 0.91593236f;
    camera->active = 1;
    camera->state = 1;
    task->texture = UmnBgCubeXtx;
    task->model = UmnBgCubeLex;
    return (void *)address;
}

static void DrawUmnInterface2(FontDrawCursor *cursor, void *argument)
{
    UmnIf2Parameters *parameters = argument;
    UmnIf2SetupPacket *setup = &cursor->work.umn_setup;
    UmnIf2GeometryPacket *geometry = &cursor->work.umn_geometry;
    UmnIf2PackedRegister *out;
    UmnIf2Coordinates x_coordinates;
    UmnIf2Coordinates y_coordinates;
    int i;

    setup->tag = 0x1000000000000004ULL;
    setup->registers = GIF_REG_AD;
    setup->texture_flush.value = 0;
    setup->texture_flush.register_address = SCE_GS_TEXFLUSH;
    setup->alpha.value = 0x44;
    setup->alpha.register_address = SCE_GS_ALPHA_1;
    setup->texture.value = (0x24120000 | (sRender.flip_base << 5)) | 0x2000000640000000ULL;
    setup->texture.register_address = SCE_GS_TEX0_1;
    setup->test.value = 0x5000D;
    setup->test.register_address = SCE_GS_TEST_1;
    sceVif1PkAddDirectDataN(cursor->packet, setup, 5);

    x_coordinates = D_00A11EE0;
    y_coordinates = D_00A11EF8;

    geometry->tag_low = 0x8006;
    geometry->tag_high = 0x30AEC000;
    geometry->registers_low = 0x531;
    geometry->registers_high = 0;
    out = geometry->payload;

    for (i = 0; i < UMN_IF2_VERTEX_COUNT; i++) {
        if (i == 0) {
            out->lane[0] = parameters->first_vertex.red;
            out->lane[1] = parameters->first_vertex.green;
            out->lane[2] = parameters->first_vertex.blue;
            out->lane[3] = parameters->first_vertex.alpha;
        } else {
            out->lane[0] = parameters->remaining_vertices.red;
            out->lane[1] = parameters->remaining_vertices.green;
            out->lane[2] = parameters->remaining_vertices.blue;
            out->lane[3] = parameters->remaining_vertices.alpha;
        }
        out++;
        /* UV: U, V, and the two unused upper lanes. */
        out->lane[0] = x_coordinates.values[i] << 4;
        out->lane[1] = y_coordinates.values[i] << 4;
        out->lane[3] = 0;
        out->lane[2] = 0;
        out++;
        /* XYZ2: X, Y, depth, and the ADC lane. */
        out->lane[0] = (x_coordinates.values[i] << 4) + OV02_GS_VERTEX_X;
        out->lane[1] = (y_coordinates.values[i] << 4) + OV02_GS_VERTEX_Y;
        out->lane[2] = parameters->depth;
        out->lane[3] = 0;
        out++;
    }
    sceVif1PkAddDirectDataN(cursor->packet, geometry, 0x13);
}

void UmnInterface2Main(void) {
    if (UmnInterface2->intensity_a != 0) {
        UmnInterface2->intensity_a -= UMN_IF2_FADE_STEP_A;
    }
    if (UmnInterface2->intensity_b != 0) {
        UmnInterface2->intensity_b -= UMN_IF2_FADE_STEP_B;
    }
    if (UmnInterface2->intensity_a != 0 || UmnInterface2->intensity_b != 0) {
        xglFontPrintExtFunc(0x03FFFFF0, DrawUmnInterface2, UmnInterface2);
    }
}

void *UmnInterface2Init(void *memory) {
    unsigned int aligned_address;
    UmnInterface2State *color_a_state;
    void *result;
    UmnInterface2State *color_b_state;

    result = 0;
    if (memory != 0) {
        aligned_address = ((unsigned int)memory + 15) & ~15u;
        UmnInterface2 = (UmnInterface2State *)aligned_address;
        result = (void *)(aligned_address + 12);
    }

    color_a_state = UmnInterface2;
    color_a_state->color_a[0] = UMN_IF2_INTENSITY_MAX;
    color_a_state->intensity_a = UMN_IF2_INTENSITY_MAX;
    color_a_state->color_a[2] = UMN_IF2_INTENSITY_MAX;
    color_a_state->color_a[1] = UMN_IF2_INTENSITY_MAX;
    color_a_state->base_color = 0x00fffffe;
    color_b_state = UmnInterface2;
    color_b_state->intensity_b = UMN_IF2_INTENSITY_MAX;
    color_b_state->color_b[2] = UMN_IF2_INTENSITY_MAX;
    color_b_state->color_b[1] = UMN_IF2_INTENSITY_MAX;
    color_b_state->color_b[0] = UMN_IF2_INTENSITY_MAX;
    return result;
}
