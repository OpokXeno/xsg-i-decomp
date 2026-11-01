/* UMN code embedded in MAIN, original OV02 translation unit 6. */
/*
 * OV02 original TU 6: 0x00a02d68..0x00a03450 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_task.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "main/xgl_packet.h"
#include "main/xgl_render.h"

/* The original vector has component and doubleword-copy views. */
typedef union UmnBgCubeVec {
    Vector4 v;
    float f[4];
    u64 words[2];
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

typedef unsigned int UmnLightQuadword __attribute__((mode(TI)));

/* One contiguous 0xF0-byte light workspace: the light object proper (0x00-0x6F)
 * followed by the two matrices xglLightCalcMatrix fills in (0x70 and 0xB0),
 * which are what nmlModelSetLight receives. */
typedef struct UmnBgCubeLightWork {
    UmnLightQuadword light[7];
    Matrix4 model_set;
    Matrix4 model_light;
} UmnBgCubeLightWork;

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

typedef struct UmnIf2AdCommand {
    u64 value;
    u64 register_address;
} UmnIf2AdCommand;

typedef struct UmnIf2SetupPacket {
    u64 tag;
    u64 registers;
    UmnIf2AdCommand texture_flush;
    UmnIf2AdCommand alpha;
    UmnIf2AdCommand texture;
    UmnIf2AdCommand test;
} UmnIf2SetupPacket;

/* GIF PACKED mode expands each register into four 32-bit lanes. The tag's
 * 0x531 list repeats RGBAQ, UV and XYZ2 for each of the six vertices. */
typedef struct UmnIf2PackedRegister {
    u32 lane[4];
} UmnIf2PackedRegister;

typedef struct UmnIf2GeometryPacket {
    u32 tag_low;
    u32 tag_high;
    u32 registers_low;
    u32 registers_high;
    UmnIf2PackedRegister payload[18];
} UmnIf2GeometryPacket;

/* Both direct-data submissions use the same scratchpad address. The setup
 * packet is copied out before the geometry packet overwrites that storage. */
typedef union UmnIf2PacketWorkspace {
    UmnIf2SetupPacket setup;
    UmnIf2GeometryPacket geometry;
} UmnIf2PacketWorkspace;

typedef struct UmnIf2Context {
    XglPacket *packet;
    unsigned char unmodeled_04[0x2C];
    UmnIf2PacketWorkspace workspace;
} UmnIf2Context;

/*
 * State reached through the global UmnInterface2 pointer; only the two
 * frame counters UmnInterface2Main touches are evidenced.
 */
typedef struct UmnInterface2State {
    unsigned int base_color;
    unsigned char color_a[3];
    unsigned char intensity_a; /* decremented by 8 each frame until zero */
    unsigned char color_b[3];
    unsigned char intensity_b; /* decremented by 2 each frame until zero */
} UmnInterface2State;

extern UmnWorkData UmnWork;
extern UmnBgCubeVec D_00A11E60;
extern const UmnBgCubeLights D_00A11E70;
extern unsigned char *task_xmx;
extern void *UmnBgCubeXtx;
extern void *UmnBgCubeLex;
extern const UmnIf2Coordinates D_00A11EE0;
extern const UmnIf2Coordinates D_00A11EF8;
extern UmnInterface2State *UmnInterface2;

extern float xglFRand(void);
extern void xglLightIntensityParallel(void *light, int index, const Vector4 *intensity);
extern void xglLightIntensityAmbient(void *light, const Vector4 *intensity);
extern void xglLightDirection(void *light, unsigned int index, const Vector4 *direction);
extern void xglLightCalcMatrix(void *light);
extern void nmlModelSetLight(void *model, void *light);
extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetPlace(const Vector4 *place);
extern void nmlModelEntry(int entry);
extern void xglCameraInit(StudioCamera *camera);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

/* The font queue calls draw(context, arg) with the arg registered below;
 * the context record (read at +0x00 and +0x30) is not yet recovered. */
static void DrawUmnInterface2(void *context, void *arg);

extern void xglFontPrintExtFunc(unsigned int flags, void (*draw)(void *context, void *arg), void *arg);

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
    UmnBgCubeLightWork work;
    UmnBgCubeLights lights;
    StudioCamera *camera;
    int i;

    if (UmnWork.mode == 0xFF)
        task->state = 99;

    switch (task->state) {
    case 0:
        task->state = 1;
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
    case 1:
        for (i = 0; i < 3; i++)
            rotation->f[i] += 0.001f;
        break;
    case 99:
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
    xglLightIntensityAmbient(&work, &lights.v[0].v);
    xglLightIntensityParallel(&work, 0, &lights.v[1].v);
    xglLightDirection(&work, 0, &lights.v[4].v);
    xglLightIntensityParallel(&work, 1, &lights.v[2].v);
    xglLightDirection(&work, 1, &lights.v[5].v);
    xglLightIntensityParallel(&work, 2, &lights.v[3].v);
    xglLightDirection(&work, 2, &lights.v[6].v);
    xglLightCalcMatrix(&work);
    nmlModelSetLight(&work.model_light, &work.model_set);
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
    XglTaskScheduler *scheduler = (XglTaskScheduler *)task_xmx;
    UmnBgCubeMainTask *task;
    StudioCamera *camera;

    task = (UmnBgCubeMainTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))tskUmnBgCubeMain,
        scheduler != 0 ? scheduler->active_tail : 0);
    task->data = (UmnBgCubeData *)address;
    address += sizeof(UmnBgCubeData);
    task->state = 0;
    task->enabled = 0;
    camera = xglStudioGetCamera2(0);
    task->camera_id = 0;
    xglCameraInit(camera);
    camera->rotation.x = 0.0f;
    camera->rotation.y = 0.0f;
    camera->rotation.z = 0.0f;
    camera->position.x = 0.0f;
    camera->position.y = 0.0f;
    camera->position.z = 4.0f;
    camera->fov = 0.91593236f;
    camera->active = 1;
    camera->state = 1;
    task->texture = (int)UmnBgCubeXtx;
    task->model = (int)UmnBgCubeLex;
    return (void *)address;
}

static void DrawUmnInterface2(void *context_arg, void *arg)
{
    UmnIf2Context *context = (UmnIf2Context *)context_arg;
    UmnIf2Parameters *parameters = (UmnIf2Parameters *)arg;
    UmnIf2SetupPacket *setup = &context->workspace.setup;
    UmnIf2GeometryPacket *geometry = &context->workspace.geometry;
    UmnIf2PackedRegister *out;
    UmnIf2Coordinates x_coordinates;
    UmnIf2Coordinates y_coordinates;
    int i;

    setup->tag = 0x1000000000000004ULL;
    setup->registers = 0xE;
    setup->texture_flush.value = 0;
    setup->texture_flush.register_address = 0x3F;
    setup->alpha.value = 0x44;
    setup->alpha.register_address = 0x42;
    setup->texture.value = (0x24120000 | (sRender.flip_base << 5)) | 0x2000000640000000ULL;
    setup->texture.register_address = 6;
    setup->test.value = 0x5000D;
    setup->test.register_address = 0x47;
    sceVif1PkAddDirectDataN(context->packet, setup, 5);

    x_coordinates = D_00A11EE0;
    y_coordinates = D_00A11EF8;

    geometry->tag_low = 0x8006;
    geometry->tag_high = 0x30AEC000;
    geometry->registers_low = 0x531;
    geometry->registers_high = 0;
    out = geometry->payload;

    for (i = 0; i < 6; i++) {
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
        out->lane[0] = (x_coordinates.values[i] << 4) + 0x6FF8;
        out->lane[1] = (y_coordinates.values[i] << 4) + 0x71F8;
        out->lane[2] = parameters->depth;
        out->lane[3] = 0;
        out++;
    }
    sceVif1PkAddDirectDataN(context->packet, geometry, 0x13);
}

void UmnInterface2Main(void) {
    if (UmnInterface2->intensity_a != 0) {
        UmnInterface2->intensity_a -= 8;
    }
    if (UmnInterface2->intensity_b != 0) {
        UmnInterface2->intensity_b -= 2;
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
    color_a_state->color_a[0] = 0x80;
    color_a_state->intensity_a = 0x80;
    color_a_state->color_a[2] = 0x80;
    color_a_state->color_a[1] = 0x80;
    color_a_state->base_color = 0x00fffffe;
    color_b_state = UmnInterface2;
    color_b_state->intensity_b = 0x80;
    color_b_state->color_b[2] = 0x80;
    color_b_state->color_b[1] = 0x80;
    color_b_state->color_b[0] = 0x80;
    return result;
}
