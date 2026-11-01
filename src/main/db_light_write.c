#include "common.h"

static int mode_004DC5A8;

#include "shared.h"
#include "main/get.h"

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
    union { int flag_04; int submode; };
    int actor_index;
    int step;
    HomogeneousVector position;
    unsigned char unmodeled_20[0x10];
    int attach_state;
    union { int flag_34; ActorHead *attached_actor; };
    union {
        struct {
            unsigned char unmodeled_38[0x0c];
            float offset_44;
            unsigned char unmodeled_48[8];
        };
        struct {
            unsigned char unmodeled_38_to_40[8];
            HomogeneousVector attach_offset;
        };
    };
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
    unsigned char unmodeled_58[0x28];
    Vector4 stage_color;
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

static int visible_0;

static int partsIndex;

extern const char matrixRowFormat[];

extern const char partsIndexFormat[];

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern void nmlModelSetPartsVisible(MapPartListView *parts,
                                    int part_index, int visible);

static Vector4 lightDir[6];

static RgVector lightCol[6];

extern void xglLightIntensityParallel(void *light, int index,
                                      const Vector4 *intensity);

extern void xglLightIntensityAmbient(void *light,
                                     const Vector4 *intensity);

extern void xglLightDirection(void *light, unsigned int index,
                              const Vector4 *direction);

extern float atan2f(float y, float x);

extern float xglSin(float angle);

extern float xglCos(float angle);

extern int sceWrite(int descriptor, const void *buffer, int size);

extern double fptodp(float value);

extern const char lightSetColorFormat[];

extern const char lightSetDirection2Format[];

extern const char stageSetColorFormat[];

/* Clear-environment colour words (the GS clear block's color_r/g/b at 0x20). */
typedef struct {
    unsigned char unmodeled_00[0x20];
    unsigned int color_r;
    unsigned int color_g;
    unsigned int color_b;
} ClearEnvColorView;

extern ClearEnvColorView ClearEnv;

typedef struct { float f[28]; } LightFloats;

typedef struct {
    Matrix4 m;
} Matrix4Block;

extern StudioCamera *xglStudioGetActiveCamera(void);

extern void xglVectorMulMat(Vector4 *destination, const void *matrix,
                            const Vector4 *vector);

extern void xglVectorNormal(Vector4 *destination, const Vector4 *vector);

extern const float cursorDirectionLength;

extern void ACT_updateSequence(ActorHead *actor);

typedef struct {
    unsigned char unmodeled_00[0x0C];
    int mode0;
    int mode1;
    unsigned char unmodeled_14[0x1C];
    float translation[4];
    int unmodeled_40;
    void *translation_constraint;
    unsigned char unmodeled_48[0x488];
    int view_offset[3];
    int unmodeled_4dc;
    int view_mode;
    void *view_constraint;
} CameraChannelRecord;

extern CameraChannelRecord tcamera;

static float cursorSpeed[9];

extern const float cursorPlaneRotateDecrement;

extern const float cursorPlaneRotateIncrement;

extern const char mapHeightFormat[];

extern const char mapHoleHeightText[];

static unsigned short keyData[8];

extern float UnduGet(float x, float z);

void updateCursorModePlane(void);

extern const float cursorMode0RotateDecrement;

extern const float cursorMode0RotateIncrement;

static int toolFlags;

int cursorMode;

static const char * cursorModeName[5];

extern const char cursorPositionFormat[];

extern const char cameraPositionFormat[];

extern const char cursorModeSpeedFormat[];

extern void PAD_getKey(unsigned short *keys, int port, int mask);

typedef struct WindRecord {
    signed char mode;
    unsigned char unmodeled_01[0x0F];
    float x;
    float y;
    float z;
    float w;
    float offset_a;
    float offset_b;
} WindRecord;

typedef union WindPoint {
    Vector4 vector;
    unsigned long long raw[2];
} WindPoint;

typedef struct WindMarker {
    WindPoint from;
    Vector4 to;
    int color[4];
} WindMarker;

extern void EXM_ResetWind(void);

static int dispy_2;

static int csr_1;

extern const char windSettingsText[];

extern const char windTypeText[];

extern const char windOffText[];

extern const char windPointText[];

extern const char windShakeText[];

extern const char windVectorText[];

extern const char cursorMarkerText[];

extern const char windOffSelectedText[];

extern const char windDirectionalSelectedText[];

extern const char windDirectionalText[];

extern const char windPointSelectedText[];

extern const char windPowerFormat[];

extern const char windSpeedFormat[];

extern const char windXFormat[];

extern const char windYFormat[];

extern const char windZFormat[];

extern const char windWFormat[];

extern const char windShakeCommandFormat[];

extern const char directionalWindCommandFormat[];

extern const char pointWindCommandFormat[];

extern const char stopWindCommandFormat[];

void drawVector4(const Vector3 *vector, Matrix4 *matrix, int color_index);

void drawCircle2(Matrix4 *matrix, float radius, int color_index);

typedef struct CursorCameraRecordView {
    unsigned char unmodeled_00[0x0C];
    int mode[4];
    unsigned char unmodeled_1c[0x4D0 - 0x1C];
    Vector4 view_offset;
    int view_mode;
    HomogeneousVector *view_constraint;
} CursorCameraRecordView;

typedef struct UpdateLightKeys {
    unsigned short buttons;
    unsigned short pressed;
    unsigned char unmodeled_04[6];
    signed char stick_x;
    signed char stick_y;
} UpdateLightKeys;

void updateCursorMode0(void);

void updateCursorMode2(void);

enum CursorMode {
    CURSOR_MODE_NONE = -1,
    CURSOR_MODE_0,
    CURSOR_MODE_1,
    CURSOR_MODE_2,
    CURSOR_MODE_3,
    CURSOR_MODE_4
};

void DB_lightWrite(int descriptor)
{
    char buffer[128];
    StudioLight *light;
    int lightNumber;
    int lightIndex;

    xglStudioGetLight(&light);
    sceWrite(descriptor, buffer,
             sprintf(buffer,
                     lightSetColorFormat,
                     0, light->ambient_color.x, light->ambient_color.y,
                     light->ambient_color.z));

    lightIndex = 0;
    do {
        lightNumber = lightIndex + 1;
        sceWrite(descriptor, buffer,
                 sprintf(buffer, lightSetColorFormat, lightNumber,
                         ((LightFloats *)light)->f[lightIndex * 8 + 4],
                         ((LightFloats *)light)->f[lightIndex * 8 + 5],
                         ((LightFloats *)light)->f[lightIndex * 8 + 6]));
        sceWrite(descriptor, buffer,
                 sprintf(buffer, lightSetDirection2Format, lightNumber,
                         ((LightFloats *)light)->f[lightIndex * 8 + 8],
                         ((LightFloats *)light)->f[lightIndex * 8 + 9],
                         ((LightFloats *)light)->f[lightIndex * 8 + 10]));
        lightIndex = lightNumber;
    } while (lightIndex < 3);

    {
        const char *stage_format = stageSetColorFormat;
        double stage_red;
        double stage_green;
        double stage_blue;

        stage_red = fptodp(GameLoopState.stage_color.x);
        stage_green = fptodp(GameLoopState.stage_color.y);
        stage_blue = fptodp(GameLoopState.stage_color.z);
        sceWrite(descriptor, buffer,
                 sprintf(buffer, stage_format, stage_red, stage_green, stage_blue));
    }
}

typedef struct {
    Matrix4 matrix;
    Matrix4 world;
    Vector4 screen_scale;
    Vector4 screen_offset;
    Matrix4 *local;
} DebugDrawTransform;

typedef struct {
    unsigned int count, primitive, registers, reserved;
    unsigned int payload[8][4];
} DebugRectPacket;

typedef struct {
    DebugRectPacket packet;
    unsigned char unmodeled_90[0x400 - 0x90];
    DebugDrawTransform transform;
    unsigned char unmodeled_4a4[0x4e0 - 0x4a4];
    Vector4 positions[5];
} DebugRectScratch;

static unsigned char col[56];
static Vector4 vRect[4] ;
extern void transformVector(Vector4 *destination, const Vector4 *vectors,
                            int count, DebugDrawTransform *params);
extern void xglPrimAddGifTagDirect(XglPacket *packet, const void *tag, int count);

void drawRect2(Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugRectScratch *scratch = (DebugRectScratch *)0x70000000;
    Vector4 *position;
    unsigned int *write;
    const unsigned char *red;
    const unsigned char *green;
    const unsigned char *blue;
    const unsigned char *alpha;
    float pixel_scale;

    color_index *= 2;
    xglStudioGetCamera(&camera, 0);
    xglMatrixStackUnit();
    xglMatrixStackSave(scratch->transform.world);
    scratch->transform.screen_offset = camera->screenOffset;
    scratch->transform.screen_scale.x = 1.0f;
    scratch->transform.screen_scale.y = -1.0f;
    scratch->transform.screen_scale.z = 1.0f;
    scratch->transform.screen_scale.w = 1.0f;
    scratch->transform.local = matrix;
    color_index = color_index * 4 + 4;
    transformVector(scratch->positions, vRect, 4, &scratch->transform);
    write = (unsigned int *)0x70000000;
    write[0] = 0x8004;
    write[1] = 0x20064000;
    write[2] = 0xffffff51;
    write[3] = 0;
    write += 4;
    position = (Vector4 *)0x700004e0;
    red = &col[color_index];
    green = &col[1] + color_index;
    blue = &col[2] + color_index;
    alpha = &col[3] + color_index;
    pixel_scale = 16.0f;
    {
        int vertex_index = 0;

        for (; vertex_index < 4; vertex_index++, position++) {
            write[0] = *red;
            write[1] = *green;
            write[2] = *blue;
            write[3] = *alpha;
            write += 4;
            write[0] = (unsigned int)(position->x * pixel_scale);
            write[1] = (unsigned int)(position->y * pixel_scale);
            write[2] = ~0U;
            write[3] = 0;
            write += 4;
        }
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, 9);
}

static Vector4 vBox[5] ;

void drawBox2(Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugRectScratch *scratch = (DebugRectScratch *)0x70000000;
    int vertex_count = 5;
    Vector4 *position;
    unsigned int (*write)[4];
    const unsigned char *red;
    const unsigned char *green;
    const unsigned char *blue;
    const unsigned char *alpha;
    int vertex_index;

    color_index *= 2;
    xglStudioGetCamera(&camera, 0);
    xglMatrixStackUnit();
    xglMatrixStackSave(scratch->transform.world);
    scratch->transform.screen_offset = camera->screenOffset;
    scratch->transform.screen_scale.x = 1.0f;
    scratch->transform.screen_scale.y = -1.0f;
    scratch->transform.screen_scale.z = 1.0f;
    scratch->transform.screen_scale.w = 1.0f;
    scratch->transform.local = matrix;
    color_index = color_index * 4 + 4;
    transformVector(scratch->positions, vBox, vertex_count, &scratch->transform);
    write = (unsigned int (*)[4])0x70000000;
    (*write)[0] = 0x8000 | vertex_count;
    (*write)[1] = 0x20054000;
    (*write)[2] = 0xffffff51;
    (*write)[3] = 0;
    write++;
    position = (Vector4 *)0x700004e0;
    red = &col[color_index];
    green = &col[1] + color_index;
    blue = &col[2] + color_index;
    alpha = &col[3] + color_index;
    for (vertex_index = 0; vertex_index < vertex_count; vertex_index++, position++) {
        (*write)[0] = *red;
        (*write)[1] = *green;
        (*write)[2] = *blue;
        (*write)[3] = *alpha;
        write++;
        (*write)[0] = (unsigned int)(position->x * 16.0f);
        (*write)[1] = (unsigned int)(position->y * 16.0f);
        (*write)[2] = ~0U;
        (*write)[3] = 0;
        write++;
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, vertex_count * 2 + 1);
}

typedef struct {
    DebugRectPacket packet;
    unsigned char unmodeled_90[0x400 - 0x90];
    DebugDrawTransform transform;
    unsigned char unmodeled_4a4[0x4b0 - 0x4a4];
    Vector4 input[2];
    unsigned char unmodeled_4d0[0x10];
    Vector4 output[2];
} DebugLineScratch;

void drawCircle2(Matrix4 *matrix, float radius, int color_index)
{
    int segment;
    int colour;
    const unsigned char *alpha;
    const unsigned char *blue;
    const unsigned char *green;
    const unsigned char *red_column;
    const unsigned char *green_column;
    const unsigned char *blue_column;
    const unsigned char *alpha_column;
    const unsigned char *red;
    StudioCamera *camera;
    DebugLineScratch *scratch = (DebugLineScratch *)0x70000000;
    unsigned int *write;
    float angle;
    float scale;

    color_index *= 2;
    xglStudioGetCamera(&camera, 0);
    xglMatrixStackUnit();
    xglMatrixStackSave(scratch->transform.world);
    scratch->transform.screen_offset = camera->screenOffset;
    scratch->transform.screen_scale.x = 1.0f;
    scratch->transform.screen_scale.y = -1.0f;
    scratch->transform.screen_scale.z = 1.0f;
    scratch->transform.screen_scale.w = 1.0f;
    scratch->transform.local = matrix;
    scratch->input[0].x = 0.0f;
    scratch->input[0].y = 0.0f;
    scratch->input[0].z = 0.0f;
    scratch->input[0].w = 1.0f;
    transformVector(scratch->output, scratch->input, 1, &scratch->transform);
    colour = color_index * 4 + 4;
    angle = 0.0f;
    scale = 16.0f;
    write = (unsigned int *)scratch;
    ((DebugRectPacket *)write)->count = 0x8011;
    ((DebugRectPacket *)write)->primitive = 0x20054000;
    ((DebugRectPacket *)write)->registers = 0xffffff51;
    ((DebugRectPacket *)write)->reserved = 0;
    write += 4;
    red_column = &col[0];
    green_column = &col[1];
    blue_column = &col[2];
    alpha_column = &col[3];
    green = green_column + colour;
    blue = blue_column + colour;
    alpha = alpha_column + colour;
    red = red_column + colour;
    for (segment = 0; segment < 17; segment++) {
        write[0] = *red;
        write[1] = *green;
        write[2] = *blue;
        write[3] = *alpha;
        write += 4;
        write[0] = (unsigned int)((scratch->output[0].x +
                                   xglSin(angle) * radius) * scale);
        write[1] = (unsigned int)((scratch->output[0].y +
                                   xglCos(angle) * radius) * scale);
        angle += 0.39269908f;
        write[2] = 0xffffffffU;
        write[3] = 0;
        write += 4;
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, 35);
}

void drawVector2(const Vector3 *vector, Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugLineScratch *scratch = (DebugLineScratch *)0x70000000;
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;

    color_index *= 2;
    xglStudioGetCamera(&camera, 0);
    xglMatrixStackUnit();
    xglMatrixStackSave(scratch->transform.world);
    scratch->transform.screen_offset = camera->screenOffset;
    scratch->transform.screen_scale.x = 1.0f;
    scratch->transform.screen_scale.y = -1.0f;
    scratch->transform.screen_scale.z = 1.0f;
    scratch->transform.screen_scale.w = 1.0f;
    scratch->input[0].w = 1.0f;
    scratch->input[0].x = 0.0f;
    scratch->input[0].y = 0.0f;
    scratch->input[0].z = 0.0f;
    scratch->transform.local = matrix;
    scratch->input[1].x = vector->x;
    scratch->input[1].y = vector->y;
    scratch->input[1].z = vector->z;
    scratch->input[1].w = 1.0f;
    transformVector(scratch->output, scratch->input, 2, &scratch->transform);
    write = (unsigned int (*)[4])0x70000000;
    (*write)[0] = 0x8002;
    (*write)[1] = 0x2004c000;
    (*write)[2] = 0xffffff51;
    (*write)[3] = 0;
    write++;
    position = (Vector4 *)0x700004e0;
    for (vertex_index = 0; vertex_index < 2;
         vertex_index++, position++, color_index++) {
        (*write)[0] = col[color_index * 4];
        (*write)[1] = col[color_index * 4 + 1];
        (*write)[2] = col[color_index * 4 + 2];
        (*write)[3] = col[color_index * 4 + 3];
        write++;
        (*write)[0] = (unsigned int)(position->x * 16.0f);
        (*write)[1] = (unsigned int)(position->y * 16.0f);
        (*write)[2] = (unsigned int)(4294967296.0f - position->z);
        (*write)[3] = 0;
        write++;
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, 5);
}

void drawVector4S(const Vector3 *vector, Matrix4 *matrix, int color_index,
                  DebugLineScratch *scratch)
{
    StudioCamera *camera;
    StudioCamera *camera_slot;
    StudioCamera **slot_ptr = &camera_slot;
    unsigned int (*write)[4];
    const unsigned char *colour;
    Vector4 *position;
    int vertex_index;

    color_index *= 2;
    xglStudioGetCamera(&camera_slot, 0);
    camera = *slot_ptr;
    scratch->transform.local = matrix;
    *(Matrix4Block *)scratch->transform.world = *(Matrix4Block *)camera->viewMatrix;
    scratch->transform.screen_offset = camera->screenOffset;
    scratch->transform.screen_scale = camera->screenScale;
    scratch->input[0].w = 1.0f;
    scratch->input[0].x = 0.0f;
    scratch->input[0].y = 0.0f;
    scratch->input[0].z = 0.0f;
    scratch->input[1].x = vector->x;
    scratch->input[1].y = vector->y;
    scratch->input[1].z = vector->z;
    scratch->input[1].w = 1.0f;
    position = scratch->output;
    transformVector(position, scratch->input, 2, &scratch->transform);
    colour = &col[color_index * 4];
    write = (unsigned int (*)[4])scratch;
    (*write)[0] = 0x8002;
    (*write)[1] = 0x2004c000;
    (*write)[2] = 0xffffff51;
    (*write)[3] = 0;
    write++;
    for (vertex_index = 0; vertex_index < 2; vertex_index++, position++) {
        (*write)[0] = colour[0];
        (*write)[1] = colour[1];
        (*write)[2] = colour[2];
        (*write)[3] = colour[3];
        colour += 4;
        write++;
        (*write)[0] = (unsigned int)(position->x * 16.0f);
        (*write)[1] = (unsigned int)(position->y * 16.0f);
        (*write)[2] = (unsigned int)(4294967296.0f - position->z);
        (*write)[3] = 0;
        write++;
    }
    xglPrimAddGifTagDirect(0, scratch, 5);
}

#include "main/xgl_2.h"

extern void xglMatrixStackLoad(Matrix4 matrix);
static Vector4 vGridXZ[5] ;
typedef struct DebugCursorColour {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} DebugCursorColour;

extern DebugCursorColour cCursor[6];

void drawGrid(Matrix4 matrix, float scale)
{
    Vector4 scale_vector;
    Matrix4 local_matrix;
    Matrix4 *local_matrix_pointer;
    StudioCamera *camera;
    StudioCamera *source_camera;
    DebugDrawTransform *params = (DebugDrawTransform *)0x70000400;
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;

    xglStudioGetCamera(&camera, 0);
    source_camera = camera;
    params->screen_offset = source_camera->screenOffset;
    __builtin_memcpy(params->world, source_camera->viewMatrix, sizeof(Matrix4));
    params->screen_scale = source_camera->screenScale;
    local_matrix_pointer = &local_matrix;
    __builtin_memcpy(&params->local, &local_matrix_pointer,
                     sizeof(local_matrix_pointer));
    scale_vector.x = scale;
    scale_vector.y = scale;
    scale_vector.z = scale;
    scale_vector.w = 1.0f;
    xglMatrixStackLoad(matrix);
    xglMatrixStackScale(&scale_vector.x);
    xglMatrixStackSave(local_matrix);
    transformVector((Vector4 *)0x700004e0, vGridXZ, 5, params);
    position = (Vector4 *)0x700004e0;
    for (vertex_index = 0; vertex_index < 5; vertex_index++, position++) {
        if (position->x > 2560.0f)
            return;
        if (position->x < 1536.0f)
            return;
        if (position->y > 2560.0f)
            return;
        if (position->y < 1536.0f)
            return;
        if (position->z < 0.0f)
            return;
    }
    position = (Vector4 *)0x700004e0;
    write = (unsigned int (*)[4])0x70000010;
    ((unsigned int *)0x70000000)[0] = 0x8005;
    ((unsigned int *)0x70000000)[1] = 0x20054000;
    ((unsigned int *)0x70000000)[2] = 0xffffff51;
    ((unsigned int *)0x70000000)[3] = 0;
    for (vertex_index = 0; vertex_index < 5; vertex_index++, position++) {
        (*write)[0] = cCursor[0].red;
        (*write)[1] = cCursor[0].green;
        (*write)[2] = cCursor[0].blue;
        (*write)[3] = cCursor[0].alpha;
        write++;
        (*write)[0] = (unsigned int)(position->x * 16.0f);
        (*write)[1] = (unsigned int)(position->y * 16.0f);
        (*write)[2] = (unsigned int)position->z;
        (*write)[3] = 0;
        write++;
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, 11);
}

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

static Vector4 vCursor[6] ;

void drawAxis(Matrix4 matrix, float scale)
{
    Vector4 scale_vector;
    Matrix4 local_matrix;
    Matrix4 *local_matrix_pointer;
    StudioCamera *camera;
    DebugDrawTransform *params = (DebugDrawTransform *)0x70000400;
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;

    camera = getCurrentCamera();
    params->screen_offset = camera->screenOffset;
    __builtin_memcpy(params->world, camera->viewMatrix, sizeof(Matrix4));
    params->screen_scale = camera->screenScale;
    local_matrix_pointer = &local_matrix;
    __builtin_memcpy(&params->local, &local_matrix_pointer,
                     sizeof(local_matrix_pointer));
    scale_vector.x = scale;
    scale_vector.y = scale;
    scale_vector.z = scale;
    scale_vector.w = 1.0f;
    xglMatrixStackLoad(matrix);
    xglMatrixStackScale(&scale_vector.x);
    xglMatrixStackSave(local_matrix);
    transformVector((Vector4 *)0x700004e0, vCursor, 6, params);
    position = (Vector4 *)0x700004e0;
    if (position->x > 2560.0f)
        return;
    if (position->x < 1536.0f)
        return;
    if (position->y > 2560.0f)
        return;
    if (position->y < 1536.0f)
        return;
    if (position->z < 0.0f)
        return;
    ((unsigned int *)0x70000000)[0] = 0x8006;
    ((unsigned int *)0x70000000)[1] = 0x2004c000;
    ((unsigned int *)0x70000000)[2] = 0xffffff51;
    ((unsigned int *)0x70000000)[3] = 0;
    write = (unsigned int (*)[4])0x70000010;
    for (vertex_index = 0; vertex_index < 6; vertex_index++, position++) {
        (*write)[0] = cCursor[vertex_index].red;
        (*write)[1] = cCursor[vertex_index].green;
        (*write)[2] = cCursor[vertex_index].blue;
        (*write)[3] = cCursor[vertex_index].alpha;
        write++;
        (*write)[0] = (unsigned int)(position->x * 16.0f);
        (*write)[1] = (unsigned int)(position->y * 16.0f);
        (*write)[2] = 0x7fffffff;
        (*write)[3] = 0;
        write++;
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, 13);
}

extern void MATRIX_transpose3(Matrix4 matrix);
extern void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);
extern float dptofp(double value);

void drawCircle3(const Vector4 *position, Matrix4 *matrix, int color_index)
{
    StudioCamera *camera_slot;
    StudioCamera **camera = &camera_slot;
    Matrix rotation;
    unsigned int *write;
    const unsigned char *red;
    const unsigned char *green;
    const unsigned char *blue;
    const unsigned char *alpha;
    const unsigned char *red_column;
    const unsigned char *green_column;
    const unsigned char *blue_column;
    const unsigned char *alpha_column;
    double magnitude;
    double zero;
    float offset_x;
    Vector4 *centre;
    float radius;
    float angle;
    int segment;
    int colour;

    color_index *= 2;
    xglStudioGetCamera(camera, 0);
    ((DebugLineScratch *)0x70000000)->transform.screen_offset = (*camera)->screenOffset;
    *(Matrix4Block *)((DebugLineScratch *)0x70000000)->transform.world = *(Matrix4Block *)(*camera)->viewMatrix;
    ((DebugLineScratch *)0x70000000)->transform.screen_scale = (*camera)->screenScale;
    ((DebugLineScratch *)0x70000000)->transform.local = matrix;
    ((DebugLineScratch *)0x70000000)->input[0].x = position->x;
    ((DebugLineScratch *)0x70000000)->input[0].y = position->y;
    ((DebugLineScratch *)0x70000000)->input[0].z = position->z;
    ((DebugLineScratch *)0x70000000)->input[0].w = 1.0f;
    transformVector(((DebugLineScratch *)0x70000000)->output, ((DebugLineScratch *)0x70000000)->input, 1, &((DebugLineScratch *)0x70000000)->transform);
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r"(&rotation), "r"((*camera)->cameraToWorldMatrix)
        : "$2", "memory");
    rotation.elements[12] = 0.0f;
    rotation.elements[13] = 0.0f;
    rotation.elements[14] = 0.0f;
    MATRIX_transpose3((float (*)[4])rotation.elements);
    ((DebugLineScratch *)0x70000000)->transform.local = (Matrix4 *)rotation.elements;
    xglMatrixMul(*matrix, (float (*)[4])rotation.elements, *matrix);
    ((DebugLineScratch *)0x70000000)->input[0].x = position->w;
    ((DebugLineScratch *)0x70000000)->input[0].y = 0.0f;
    ((DebugLineScratch *)0x70000000)->input[0].z = 0.0f;
    ((DebugLineScratch *)0x70000000)->input[0].w = 1.0f;
    write = (unsigned int *)0x70000000;
    zero = 0.0;
    transformVector(&((DebugLineScratch *)0x70000000)->output[1], ((DebugLineScratch *)0x70000000)->input, 1, &((DebugLineScratch *)0x70000000)->transform);
    write[0] = 0x8011;
    write[1] = 0x20054000;
    write[2] = 0xffffff51;
    write[3] = 0;
    write += 4;
    magnitude = fptodp(((Vector4 *)0x700004f0)->x - 2048.0f);
    if (magnitude < zero)
        magnitude = zero - magnitude;
    radius = dptofp(magnitude);
    colour = color_index * 4 + 4;
    red_column = &col[0];
    green_column = &col[1];
    blue_column = &col[2];
    alpha_column = &col[3];
    green = green_column + colour;
    blue = blue_column + colour;
    alpha = alpha_column + colour;
    red = red_column + colour;
    centre = (Vector4 *)0x700004e0;
    angle = 0.0f;
    for (segment = 16; segment >= 0; segment--) {
        write[0] = *red;
        write[1] = *green;
        write[2] = *blue;
        write[3] = *alpha;
        write += 4;
        write[0] = (centre->x + xglSin(angle) * radius) * 16.0f;
        write[1] = (centre->y + xglCos(angle) * radius) * 16.0f;
        angle += 0.39269908f;
        write[2] = 0xffffffffU;
        write[3] = 0;
        write += 4;
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, 35);
}

void drawTags(const Vector3 *position, Matrix4 *matrix, const char *label)
{
    Vector4 *output;
    StudioCamera *camera;
    StudioCamera *camera_slot;
    DebugLineScratch *scratch = (DebugLineScratch *)0x70000000;

    xglStudioGetCamera(&camera_slot, 0);
    camera = camera_slot;
    scratch->transform.screen_offset = camera->screenOffset;
    *(Matrix4Block *)scratch->transform.world = *(Matrix4Block *)camera->viewMatrix;
    scratch->transform.screen_scale = camera->screenScale;
    scratch->input[0].x = position->x;
    scratch->input[0].y = position->y;
    scratch->input[0].z = position->z;
    scratch->input[0].w = 1.0f;
    scratch->transform.local = matrix;
    output = scratch->output;
    transformVector(output, scratch->input, 1, &scratch->transform);
    if (output->x > 2560.0f)
        return;
    if (output->x < 1536.0f)
        return;
    if (output->y > 2560.0f)
        return;
    if (output->y < 1536.0f)
        return;
    if (output->z < 0.0f)
        return;
    xglFontDebugPrintf((int)((output->x - 2048.0f + 256.0f) * 0.5f),
                       (int)((output->y - 2048.0f + 224.0f) * 0.5f), label);
}

void drawVector4(const Vector3 *vector, Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugLineScratch *scratch = (DebugLineScratch *)0x70000000;
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;
    const unsigned char *colour;

    color_index *= 2;
    camera = xglStudioGetActiveCamera();
    *(Matrix4Block *)scratch->transform.world = *(Matrix4Block *)camera->viewMatrix;
    scratch->transform.screen_offset = camera->screenOffset;
    scratch->transform.screen_scale = camera->screenScale;
    scratch->input[0].w = 1.0f;
    scratch->input[0].x = 0.0f;
    scratch->input[0].y = 0.0f;
    scratch->input[0].z = 0.0f;
    scratch->transform.local = matrix;
    scratch->input[1].x = vector->x;
    scratch->input[1].y = vector->y;
    scratch->input[1].z = vector->z;
    scratch->input[1].w = 1.0f;
    transformVector(scratch->output, scratch->input, 2, &scratch->transform);
    position = scratch->output;
    write = (unsigned int (*)[4])scratch;
    (*write)[0] = 0x8002;
    (*write)[1] = 0x2004c000;
    (*write)[2] = 0xffffff51;
    (*write)[3] = 0;
    write++;
    colour = &col[color_index * 4];
    for (vertex_index = 0; vertex_index < 2; vertex_index++) {
        (*write)[0] = colour[0];
        (*write)[1] = colour[1];
        (*write)[2] = colour[2];
        (*write)[3] = colour[3];
        colour += 4;
        write++;
        (*write)[0] = (unsigned int)(position->x * 16.0f);
        (*write)[1] = (unsigned int)(position->y * 16.0f);
        (*write)[2] = (unsigned int)(4294967296.0f - position->z);
        (*write)[3] = 0;
        position++;
        write++;
    }
    xglPrimAddGifTagDirect(0, (const void *)0x70000000, 5);
}

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

void updateCursorMode1(void)
{
    const struct CursorPadKeyResult *keys =
        (const struct CursorPadKeyResult *)(const void *)keyData;
    StudioCamera *camera;
    HomogeneousVector *cursor_position;
    Vector4 direction;
    Vector4 transformed;
    Matrix4 camera_matrix;
    float direction_length;
    int submode;

    xglStudioGetCamera(&camera, 0);
    cursor_position = &cursor[1];
    if (keys->pressed & 2) {
        CursorState *cursor_base = (CursorState *)(cursor_position - 1);
        cursor_base->submode++;
        if (cursor_base->submode >= 3)
            cursor_base->submode = 0;
    }

    direction_length = 0.0f;
    submode = ((CursorState *)cursor)->submode;
    switch (submode) {
    case 0:
        direction.x = (float)keys->flags[0];
        direction.y = (float)-keys->flags[1];
        direction.z = 0.0f;
        direction.w = 1.0f;
        break;
    case 1:
        direction.x = (float)keys->flags[0];
        direction.y = 0.0f;
        direction.z = 0.0f;
        direction.w = 1.0f;
        break;
    case 2:
        direction.x = 0.0f;
        direction.y = (float)-keys->flags[1];
        direction.z = 0.0f;
        direction.w = 1.0f;
        break;
    }

    xglVectorLength(&direction_length, &direction);
    if (direction_length > 40.0f) {
        __asm__ __volatile__(
            "lq $2,0(%0)\n\t"
            "sq $2,0(%1)\n\t"
            "lq $2,16(%0)\n\t"
            "sq $2,16(%1)\n\t"
            "lq $2,32(%0)\n\t"
            "sq $2,32(%1)\n\t"
            "lq $2,48(%0)\n\t"
            "sq $2,48(%1)"
            :
            : "r" (camera->cameraToWorldMatrix), "r" (camera_matrix)
            : "$2", "memory");
        camera_matrix[3][0] = 0.0f;
        camera_matrix[3][1] = 0.0f;
        camera_matrix[3][2] = 0.0f;
        xglVectorMulMat(&transformed, camera_matrix, &direction);
        xglVectorNormal(&transformed, &transformed);
        direction_length = cursorDirectionLength;
        cursor_position->x += transformed.x * direction_length;
        cursor_position->y += transformed.y * direction_length;
        cursor_position->z += transformed.z * direction_length;
    }
}

void updateCursorMode2(void)
{
    DbLightWritePadSecondPrefix *pad;
    unsigned short keys;
    CursorState *state;
    StudioCamera *camera;
    HomogeneousVector *cursor_position;
    ActorHead *selected_actor;
    ActorHead *candidate;
    Vector4 *actor_position;
    int count;

    selected_actor = 0;
    pad = &PadData.second;
    xglStudioGetCamera(&camera, 0);
    cursor_position = &cursor[1];

    if (pad->half_2a & 8) {
        state = (CursorState *)(cursor_position - 1);
        selected_actor = nextActor(state->actor_index);
        if (selected_actor == 0) {
            state->actor_index = 0;
        } else {
            state->actor_index = selected_actor->number + 1;
        }
    }
    if (pad->half_2a & 2) {
        state = (CursorState *)cursor;
        selected_actor = prevActor(state->actor_index);
        if (selected_actor == 0) {
            state->actor_index = 63;
        } else {
            state->actor_index = selected_actor->number - 1;
        }
    }
    if (selected_actor != 0) {
        actor_position = &selected_actor->position;
        cursor_position->x = actor_position->x;
        cursor_position->y = actor_position->y;
        cursor_position->z = actor_position->z;
    }

    if (((CursorState *)cursor)->attached_actor != 0) {
        if (((CursorState *)cursor)->attach_state == 0) {
            keys = pad->half_28;
            actor_position = &((CursorState *)cursor)->attached_actor->rotation;
            if (keys & 0x1000) {
                ((CursorState *)cursor)->attach_offset.y += 1.0f / 30.0f;
            }
            if (keys & 0x4000) {
                ((CursorState *)cursor)->attach_offset.y -= 1.0f / 30.0f;
            }
            if (keys & 0x2000) {
                actor_position->y += 3.14159265f / 120.0f;
            }
            if (keys & 0x8000) {
                actor_position->y -= 3.14159265f / 120.0f;
            }
        }
    }

    pad = (DbLightWritePadSecondPrefix *)&PadData;
    candidate = ((CursorState *)cursor)->attached_actor;
    if (candidate != 0) {
        if (pad->half_2a & 0x10) {
            selected_actor = candidate;
            ((CursorState *)cursor)->attached_actor = 0;
            selected_actor->velocity.x = 0.0f;
            selected_actor->velocity.y = 0.0f;
            selected_actor->velocity.z = 0.0f;
        } else if (((CursorState *)cursor)->attach_state == 0) {
            selected_actor = candidate;
            actor_position = &selected_actor->position;
            actor_position->x = cursor_position->x + ((CursorState *)cursor)->attach_offset.x;
            actor_position->y = cursor_position->y + ((CursorState *)cursor)->attach_offset.y;
            actor_position->z = cursor_position->z + ((CursorState *)cursor)->attach_offset.z;
            if (mode_004DC5A8 == 0) {
                ACT_updateSequence(selected_actor);
            }
        }
    } else if (pad->half_2a & 0x10) {
        float threshold;

        ((CursorState *)cursor)->attach_offset.x = 0.0f;
        ((CursorState *)cursor)->attach_offset.y = 0.0f;
        ((CursorState *)cursor)->attach_offset.z = 0.0f;
        threshold = ((CursorState *)cursor)->attach_offset.x;
        selected_actor = actor;
        for (count = 63; count >= 0; selected_actor++, count--) {
            if (selected_actor->inUseId == 0) {
                continue;
            }
            if (selected_actor->flags & 8) {
                continue;
            }
            actor_position = &selected_actor->position;
            if (threshold <= ball2point((const Vector4 *)cursor_position, actor_position, 0.5f)) {
                ((CursorState *)cursor)->attached_actor = selected_actor;
            }
        }
    }
}

void updateCursorModePlane(void)
{
    unsigned short *keys;
    Vector4 stick;
    Vector4 direction;
    Matrix rotation;
    StudioCamera *active_camera;
    StudioCamera *camera;
    float length;
    float zero;
    float height;
    const char *height_format;
    const char *missing_format;
    HomogeneousVector *position;

    keys = keyData;
    xglStudioGetCamera(&camera, 0);
    position = &cursor[1];
    stick.x = (float)((signed char *)keys)[8];
    zero = 0.0f;
    stick.z = (float)((signed char *)keys)[9];
    stick.w = 1.0f;
    length = zero;
    stick.y = zero;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the origin-relative camera-to-world transform through one scratch
         * register; remove translation below to transform a movement direction. */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            "lq $2, 16(%1)\n"
            "sq $2, 16(%0)\n"
            "lq $2, 32(%1)\n"
            "sq $2, 32(%0)\n"
            "lq $2, 48(%1)\n"
            "sq $2, 48(%0)\n"
            :
            : "r"(&rotation), "r"(camera->cameraToWorldMatrix)
            : "$2", "memory");
        rotation.elements[12] = zero;
        rotation.elements[13] = zero;
        rotation.elements[14] = zero;
        xglVectorMulMat(&direction, (float (*)[4])rotation.elements, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[((CursorState *)cursor)->step];
        xglVectorNormal(&direction, &direction);
        switch (((CursorState *)cursor)->flag_04) {
        case 0:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            active_camera = camera;
            active_camera->position.x += direction.x * length;
            active_camera->position.y += direction.y * length;
            active_camera->position.z += direction.z * length;
            break;
        case 1:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            break;
        case 2:
            if (((CursorState *)cursor)->mode == 0) {
                active_camera = camera;
                active_camera->position.x += direction.x * length;
                active_camera->position.y += direction.y * length;
                active_camera->position.z += direction.z * length;
            } else {
                tcamera.translation[((CursorState *)cursor)->mode - 1] += direction.y * length;
            }
            break;
        }
    }
    stick.x = (float)((signed char *)keys)[10];
    stick.y = (float)-((signed char *)keys)[11];
    stick.w = 1.0f;
    length = 0.0f;
    stick.z = 0.0f;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the origin-relative camera-to-world transform through one scratch
         * register; remove translation below to transform a movement direction. */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            "lq $2, 16(%1)\n"
            "sq $2, 16(%0)\n"
            "lq $2, 32(%1)\n"
            "sq $2, 32(%0)\n"
            "lq $2, 48(%1)\n"
            "sq $2, 48(%0)\n"
            :
            : "r"(&rotation), "r"(camera->cameraToWorldMatrix)
            : "$2", "memory");
        rotation.elements[12] = 0.0f;
        rotation.elements[13] = 0.0f;
        rotation.elements[14] = 0.0f;
        xglVectorMulMat(&direction, (float (*)[4])rotation.elements, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[((CursorState *)cursor)->step];
        xglVectorNormal(&direction, &direction);
        switch (((CursorState *)cursor)->flag_04) {
        case 0:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            break;
        case 1:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            active_camera = camera;
            active_camera->position.x += direction.x * length;
            active_camera->position.y += direction.y * length;
            active_camera->position.z += direction.z * length;
            break;
        case 2:
            if (((CursorState *)cursor)->mode == 0) {
                active_camera = camera;
                active_camera->position.x += direction.x * length;
                active_camera->position.y += direction.y * length;
                active_camera->position.z += direction.z * length;
            } else {
                tcamera.translation[((CursorState *)cursor)->mode - 1] += direction.y * length;
            }
            break;
        }
    }
    if (keys[0] & 0x2000) {
        camera->rotation.z -= cursorPlaneRotateDecrement;
    }
    if (keys[0] & 0x8000) {
        camera->rotation.z += cursorPlaneRotateIncrement;
    }
    length = cursorSpeed[((CursorState *)cursor)->step];
    if (keys[0] & 4) {
        position->y += length;
        camera->position.y += length;
    }
    if ((unsigned short)(keys[0] & 1)) {
        position->y -= length;
        camera->position.y -= length;
    }
    if (keys[0] & 0x40) {
        height = UnduGet(position->x, position->z);
        if (height != -1000.0f) {
            height_format = mapHeightFormat;
            position->y = height;
            xglFontDebugPrintf(4, 0xC4, height_format, fptodp(height));
        } else {
            missing_format = mapHoleHeightText;
            xglFontDebugPrintf(4, 0xC4, missing_format, fptodp(height));
        }
        if (((CursorState *)cursor)->flag_34 != 0) {
            position->y -= ((CursorState *)cursor)->offset_44;
        }
    }
}

void updateCursorMode0(void)
{
    unsigned short *keys;
    Vector4 stick;
    Vector4 direction;
    Matrix rotation;
    StudioCamera *active_camera;
    StudioCamera *camera;
    float length;
    float zero;
    float height;
    const char *height_format;
    const char *missing_format;
    HomogeneousVector *position;

    keys = keyData;
    xglStudioGetCamera(&camera, 0);
    position = &cursor[1];
    stick.x = (float)((signed char *)keys)[10];
    zero = 0.0f;
    stick.z = (float)((signed char *)keys)[11];
    stick.w = 1.0f;
    length = zero;
    stick.y = zero;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the origin-relative camera-to-world transform through one scratch
         * register; remove translation below to transform a movement direction. */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            "lq $2, 16(%1)\n"
            "sq $2, 16(%0)\n"
            "lq $2, 32(%1)\n"
            "sq $2, 32(%0)\n"
            "lq $2, 48(%1)\n"
            "sq $2, 48(%0)\n"
            :
            : "r"(&rotation), "r"(camera->cameraToWorldMatrix)
            : "$2", "memory");
        rotation.elements[12] = zero;
        rotation.elements[13] = zero;
        rotation.elements[14] = zero;
        xglVectorMulMat(&direction, (float (*)[4])rotation.elements, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[((CursorState *)cursor)->step];
        xglVectorNormal(&direction, &direction);
        switch (((CursorState *)cursor)->flag_04) {
        case 0:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            active_camera = camera;
            active_camera->position.x += direction.x * length;
            active_camera->position.y += direction.y * length;
            active_camera->position.z += direction.z * length;
            break;
        case 1:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            break;
        case 2:
            if (((CursorState *)cursor)->mode == 0) {
                active_camera = camera;
                active_camera->position.x += direction.x * length;
                active_camera->position.y += direction.y * length;
                active_camera->position.z += direction.z * length;
            } else {
                tcamera.translation[((CursorState *)cursor)->mode - 1] += direction.y * length;
            }
            break;
        }
    }
    stick.x = (float)((signed char *)keys)[8];
    stick.y = (float)-((signed char *)keys)[9];
    stick.w = 1.0f;
    length = 0.0f;
    stick.z = 0.0f;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the origin-relative camera-to-world transform through one scratch
         * register; remove translation below to transform a movement direction. */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            "lq $2, 16(%1)\n"
            "sq $2, 16(%0)\n"
            "lq $2, 32(%1)\n"
            "sq $2, 32(%0)\n"
            "lq $2, 48(%1)\n"
            "sq $2, 48(%0)\n"
            :
            : "r"(&rotation), "r"(camera->cameraToWorldMatrix)
            : "$2", "memory");
        rotation.elements[12] = 0.0f;
        rotation.elements[13] = 0.0f;
        rotation.elements[14] = 0.0f;
        xglVectorMulMat(&direction, (float (*)[4])rotation.elements, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[((CursorState *)cursor)->step];
        xglVectorNormal(&direction, &direction);
        switch (((CursorState *)cursor)->flag_04) {
        case 0:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            break;
        case 1:
            if (((CursorState *)cursor)->mode == 0) {
                position->x += direction.x * length;
                position->y += direction.y * length;
                position->z += direction.z * length;
                break;
            }
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            active_camera = camera;
            active_camera->position.x += direction.x * length;
            active_camera->position.y += direction.y * length;
            active_camera->position.z += direction.z * length;
            break;
        case 2:
            if (((CursorState *)cursor)->mode == 0) {
                active_camera = camera;
                active_camera->position.x += direction.x * length;
                active_camera->position.y += direction.y * length;
                active_camera->position.z += direction.z * length;
            } else {
                tcamera.translation[((CursorState *)cursor)->mode - 1] += direction.y * length;
            }
            break;
        }
    }
    if (keys[0] & 0x2000) {
        camera->rotation.z -= cursorMode0RotateDecrement;
    }
    if (keys[0] & 0x8000) {
        camera->rotation.z += cursorMode0RotateIncrement;
    }
    length = cursorSpeed[((CursorState *)cursor)->step];
    if (keys[0] & 4) {
        position->y += length;
        camera->position.y += length;
    }
    if ((unsigned short)(keys[0] & 1)) {
        position->y -= length;
        camera->position.y -= length;
    }
    if (keys[0] & 0x40) {
        height = UnduGet(position->x, position->z);
        if (height != -1000.0f) {
            height_format = mapHeightFormat;
            position->y = height;
            xglFontDebugPrintf(4, 0xC4, height_format, fptodp(height));
        } else {
            missing_format = mapHoleHeightText;
            xglFontDebugPrintf(4, 0xC4, missing_format, fptodp(height));
        }
        if (((CursorState *)cursor)->flag_34 != 0) {
            position->y -= ((CursorState *)cursor)->offset_44;
        }
    }
}

void changeCameraMode(void)
{
    StudioCamera *camera;
    Vector4 difference;
    Vector4 *camera_position;
    float distance;
    CursorState *state;
    int mode;

    xglStudioGetCamera(&camera, 0);
    state = (void *)cursor;
    camera_position = &camera->position;
    difference.x = state->position.x - camera_position->x;
    difference.y = state->position.y - camera_position->y;
    difference.z = state->position.z - camera_position->z;
    xglVectorLength(&distance, &difference);
    if (distance < 1.0f) {
        distance = 1.0f;
    }
    mode = state->mode;
    camera->rotation.z = 0.0f;
    state->flag_04 = 0;
    if (mode != 4) {
        tcamera.mode1 = 20;
        tcamera.view_mode = 1;
        tcamera.view_constraint = &state->position;
        tcamera.view_offset[1] = 0;
        tcamera.view_offset[2] = 0;
        tcamera.view_offset[0] = 0;
    }
    switch (mode) {
    case 0:
        ((CursorState *)cursor)->flag_04 = 2;
        tcamera.mode0 = 16;
        break;
    case 2: {
        float translationDistance = distance;
        tcamera.mode0 = 20;
        tcamera.unmodeled_40 = 1;
        tcamera.translation_constraint = &cursor[1];
        tcamera.translation[1] = translationDistance;
        tcamera.translation[2] = 0.0f;
        tcamera.translation[0] = 0.0f;
        break;
    }
    case 3:
        tcamera.mode0 = 20;
        tcamera.translation_constraint = &cursor[1];
        tcamera.translation[2] = -distance;
        tcamera.translation[0] = 0.0f;
        tcamera.translation[1] = 0.0f;
        tcamera.unmodeled_40 = 1;
        break;
    case 1:
        tcamera.mode0 = 20;
        tcamera.translation_constraint = cursor + 1;
        tcamera.translation[0] = -distance;
        tcamera.translation[1] = 0.0f;
        tcamera.translation[2] = 0.0f;
        tcamera.unmodeled_40 = 1;
        /* fall through */
    case 4:
        if (((CursorState *)cursor)->callback != 0) {
            tcamera.mode0 = 16;
            tcamera.mode1 = 16;
        } else {
            ((CursorState *)cursor)->mode = 0;
            changeCameraMode();
        }
        break;
    }
}

void updateCursor(int mode)
{
    unsigned short *keys;
    int plane;
    int selected;
    StudioCamera *camera;
    HomogeneousVector *position;
    Vector4 *camera_position;
    StudioCamera *reset_camera;
    char text[0x100];
    const char *position_format;
    const char *camera_format;

    keys = keyData;
    switch (mode) {
    case 0:
        toolFlags = 0;
        PAD_getKey(keys, 0, 0xF7FF);
        break;
    case 1:
        toolFlags = 1;
        PAD_getKey(keys, 0, 0x600);
        break;
    case 2:
        toolFlags = 1;
        PAD_getKey(keys, 0, 0x400);
        break;
    case 3:
        toolFlags = 0;
        PAD_getKey(keys, 0, 0x57BF);
        break;
    }
    if ((keys[0] & 0xA0) != 0) {
        if (cursorMode < 0) {
            cursorMode = ((CursorState *)cursor)->mode;
        }
        if (keys[1] & 0x1000) {
            cursorMode += 1;
        }
        if (keys[1] & 0x4000) {
            cursorMode -= 1;
        }
        if (cursorMode < 0) {
            cursorMode = 4;
        }
        if (cursorMode >= 5) {
            cursorMode = 0;
        }
    } else {
        if (cursorMode >= 0) {
            ((CursorState *)cursor)->mode = cursorMode;
            changeCameraMode();
            cursorMode = -1;
        }
        if (keys[1] & 0x1000) {
            ((CursorState *)cursor)->step += 1;
        }
        if (keys[1] & 0x4000) {
            ((CursorState *)cursor)->step -= 1;
        }
        ((CursorState *)cursor)->step = ((CursorState *)cursor)->step & 7;
    }
    position = &cursor[1];
    xglStudioGetCamera(&camera, 0);
    if ((keys[1] & 0x200) != 0) {
        position->x = 0.0f;
        position->y = 0.0f;
        position->z = 0.0f;
    }
    if (keys[1] & 0x400) {
        reset_camera = camera;
        reset_camera->position.y = 0.0f;
        reset_camera->position.z = 4.0f;
        reset_camera->position.x = 0.0f;
    }
    if (keys[1] & 8) {
        ((CursorState *)cursor)->flag_04 -= 1;
    }
    if (keys[1] & 2) {
        ((CursorState *)cursor)->flag_04 += 1;
    }
    plane = ((CursorState *)cursor)->flag_04;
    if (plane < 0) {
        ((CursorState *)cursor)->flag_04 = 2;
        plane = 2;
    }
    if (!(plane < 3)) {
        ((CursorState *)cursor)->flag_04 = 0;
    }
    if (((CursorState *)cursor)->mode != 4) {
        switch (((CursorState *)cursor)->mode) {
        case 0:
        case 1:
        case 3:
            updateCursorMode0();
            break;
        case 2:
            updateCursorModePlane();
            break;
        }
        updateCursorMode2();
        if (toolFlags == 0) {
            camera_position = &camera->position;
            position_format = cursorPositionFormat;
            sprintf(text, position_format, fptodp(position->x), fptodp(position->y),
                    fptodp(position->z));
            xglFontDebugPrintf(0xAA, 0xAA, text);
            camera_format = cameraPositionFormat;
            sprintf(text, camera_format, fptodp(camera_position->x), fptodp(camera_position->y),
                    fptodp(camera_position->z));
            xglFontDebugPrintf(0xAA, 0xC6, text);
        }
    } else {
        if (((CursorState *)cursor)->callback != 0) {
            ((CursorState *)cursor)->callback(((CursorState *)cursor)->callback_argument);
        }
    }
    selected = cursorMode;
    if (selected < 0) {
        selected = ((CursorState *)cursor)->mode;
    }
    if (toolFlags == 0) {
        xglFontDebugPrintf(4, 0xB4, cursorModeSpeedFormat, cursorModeName[selected], ((CursorState *)cursor)->flag_04,
                           ((CursorState *)cursor)->step + 1);
    }
}

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

void initCursor(void)
{
    int camera_index;
    StudioCamera *camera;
    StudioCamera *current;
    HomogeneousVector *cursor_position;
    CursorState *state;

    for (camera_index = 0; camera_index < 8; camera_index++) {
        xglStudioGetCamera(&camera, camera_index);
        current = camera;
        if (camera_index == 0) {
            current->active = 1;
            current->state = 1;
        } else {
            current->active = 0;
        }
    }

    cursor_position = &cursor[1];
    tcamera.mode1 = 20;
    tcamera.view_constraint = cursor_position;
    state = (CursorState *)(cursor_position - 1);
    tcamera.view_offset[0] = 0;
    tcamera.view_offset[1] = 0;
    tcamera.view_offset[2] = 0;
    tcamera.view_mode = 1;
    state->step = 3;
    cursorMode = -1;
    state->position.x = 0.0f;
    state->position.y = 0.0f;
    state->position.z = 0.0f;
    *(enum CursorMode *)&state->mode = CURSOR_MODE_0;
    state->submode = 2;
    changeCameraMode();
}

static void printM(int x, int y, const float *matrix)
{
    char buffer[256];
    const float *rowEnd;
    int rowIndex;

    rowEnd = matrix + 3;
    rowIndex = 0;
    do {
        rowIndex++;
        sprintf(buffer, matrixRowFormat,
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
            sprintf(buffer, partsIndexFormat, partsIndex);
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

void initLight3(void)
{
    StudioLight *light;
    StudioLight *l;
    StudioLight *source;
    Vector4 *slot;
    Vector4 *colour;
    Vector4 *direction;
    float sum;
    int i;
    int filled;

    xglStudioGetLight(&light);
    l = light;
    slot = &lightCol[3];
    slot->x = l->ambient_color.x;
    slot->y = l->ambient_color.y;
    slot->z = l->ambient_color.z;
    sum = slot->x + slot->y + slot->z;
    slot->w = sum / 3.0f;
    colour = lightCol;
    i = 0;
    do {
        filled = i + 1;
        source = l;
        colour->x = source->lights[0].color.x;
        colour->y = source->lights[0].color.y;
        colour->z = source->lights[0].color.z;
        colour->w = (colour->x + colour->y + colour->z) / 3.0f;
        colour++;
        i = filled;
    } while (filled < 3);
    slot = &lightCol[4];
    slot->x = (float)ClearEnv.color_r / 255.0f;
    slot->y = (float)ClearEnv.color_g / 255.0f;
    slot->z = (float)ClearEnv.color_b / 255.0f;
    slot->w = (slot->x + slot->y + slot->z) / 3.0f;
    slot = &lightCol[5];
    slot->w = (slot->x + slot->y + slot->z) / 3.0f;
    source = l;
    i = 0;
    do {
        direction = &lightDir[i];
        direction->x = source->lights[0].direction.x;
        direction->y = source->lights[0].direction.y;
        if (direction->y >= 0.0f) {
            direction->y = 1.0f;
        } else {
            direction->y = -1.0f;
        }
        l = source;
        direction->z = source->lights[0].direction.z;
    } while (++i < 3);
}

void initLight2(void)
{
    StudioLight *light;
    StudioLight *source;
    Vector4 *destination;
    Vector4 *direction;
    float *walk;
    int i;

    xglStudioGetLight(&light);
    source = light;
    destination = &lightCol[3];
    destination->x = source->ambient_color.x;
    destination->y = source->ambient_color.y;
    destination->z = source->ambient_color.z;
    destination->w = (destination->x + destination->y + destination->z) / 3.0f;

    for (i = 0; i < 3; i++) {
        float lred;
        float lgreen;
        float lblue;

        float *walk;

        destination = lightCol + i;
        walk = &source->lights[i].color.z;
        lred = walk[-2];
        destination->x = lred;
        lgreen = walk[-1];
        destination->y = lgreen;
        lblue = walk[0];
        destination->w = (lred + lgreen + lblue) / 3.0f;
        destination->z = lblue;
    }

    destination = &lightCol[4];
    destination->x = (float)ClearEnv.color_r / 255.0f;
    destination->y = (float)ClearEnv.color_g / 255.0f;
    destination->z = (float)ClearEnv.color_b / 255.0f;
    destination->w = (destination->x + destination->y + destination->z) / 3.0f;

    destination = &lightCol[5];
    destination->w = (destination->x + destination->y + destination->z) / 3.0f;

    walk = &source->lights[0].direction.z;
    i = 0;
    do {
        float dgreen;

        direction = &lightDir[i];
        direction->x = walk[-2];
        dgreen = walk[-1];
        direction->y = dgreen;
        if (dgreen >= 0.0f)
            direction->y = 1.0f;
        else
            direction->y = -1.0f;
        direction->z = walk[0];
        walk += 8;
    } while (++i < 3);
}

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

extern float lightSpeedTbl[4];
static int lightIndex;
static int lightColIndex;
static int lightSpeed;
static int lightStateOffset;
extern void xglStudioFlushActiveCamera(void);
extern const char rgbColorFormat[];
extern const char directionVectorFormat[];
extern const char lightSpeedFormat[];
extern const char ambientText[];
extern const char backgroundText[];
extern const char mapText[];

/* The lighting editor copies direction into the bar position with doubleword
   ld/sd pairs: the vector storage is eight-byte aligned. The union gives it
   that alignment without a GNU attribute. */
typedef union LightEditorVector {
    Vector4 vector;
    u64 doublewords[2];
} LightEditorVector;
typedef struct FloatLightEntry {
    RgVector color;
    RgVector direction;
} FloatLightEntry;
typedef struct FloatLight {
    RgVector ambient_color;
    FloatLightEntry lights[3];
} FloatLight;

void updateLight(int mode)
{
    const struct CursorPadKeyResult *keys =
        (const struct CursorPadKeyResult *)(const void *)keyData;
    Matrix4 matrix;
    Matrix4 cameraMatrix;
    Vector4 stick;
    LightEditorVector direction;
    Vector4 barScale;
    LightEditorVector barPosition;
    Vector4 translation;
    char text[256];
    Vector3 flat;
    char dumpText[128];
    float stickLength;
    StudioLight *light;
    FloatLight *dumpLight;
    float horizontal;
    float level;
    StudioCamera *camera;
    Vector4 *source;
    float *component;
    int baseX;
    int i;
    int j;
    int k;

    switch (mode) {
    case 0:
        PAD_getKey(keyData, 0, 0xf7ff);
        break;
    case 1:
        PAD_getKey(keyData, 0, 0xf6ff);
        break;
    }

    if ((keys->held & 0xa0) == 0) {
        float limit;

        limit = 1.0f;

        if ((keys->pressed & 2) != 0) {
            lightColIndex = 3;
            lightIndex++;
        }
        if ((keys->pressed & 8) != 0) {
            lightColIndex = 3;
            lightIndex--;
        }
        if (lightIndex < 0)
            lightIndex = 5;
        lightIndex = lightIndex >= 6 ? 0 : lightIndex;
        if (keys->pressed & 0x1000)
            lightColIndex -= 1;
        if (keys->pressed & 0x4000)
            lightColIndex++;
        if (lightColIndex < 0)
            lightColIndex = 3;
        if (lightColIndex >= 4)
            lightColIndex = 0;
        component = lightCol[lightIndex] + lightColIndex;
        if (lightIndex == 5)
            limit = 10.0f;
        if (lightColIndex == 3) {
            float total = 0.0f;

            for (i = 0; i < 3; i++) {
                float *value = lightCol[lightIndex] + i;

                if (keys->held & 0x2000)
                    *value += 0.01f;
                if (keys->held & 0x8000)
                    *value -= 0.01f;
                level = *value;
                if (level < 0.0f) {
                    *value = 0.0f;
                    level = *value;
                }
                if (limit < level) {
                    *value = limit;
                    level = limit;
                }
                total += level;
            }
            lightCol[lightIndex][3] = total / 3.0f;
        } else {
            float total;

            if (keys->held & 0x2000)
                *component += 0.01f;
            if (keys->held & 0x8000)
                *component -= 0.01f;
            level = *component;
            if (level < 0.0f) {
                *component = 0.0f;
                level = *component;
            }
            if (limit < level) {
                *component = limit;
                level = limit;
            }
            for (total = 0.0f, i = 0; i < 3; i++)
                total += lightCol[lightIndex][i];
            lightCol[lightIndex][3] = total / 3.0f;
        }
    } else if (keys->held & 0x20) {
        if (keys->pressed & 8)
            if (lightStateOffset != 0)
                lightStateOffset = 0;
            else
                lightStateOffset = -380;
        if (keys->pressed & 0x4000)
            lightSpeed -= 1;
        if (keys->pressed & 0x1000)
            lightSpeed += 1;
        lightSpeed &= 3;
    }

    source = &lightDir[lightIndex];
    if ((keys->pressed & 0x200) != 0)
        source->y = -source->y;

    stick.x = (float)keys->flags[2];
    stick.y = 0.0f;
    stick.z = (float)keys->flags[3];
    stick.w = 1.0f;
    xglVectorLength(&stickLength, &stick);
    xglVectorNormal(&stick, &stick);
    if (stickLength > 40.0f && source->w != 0.0f) {
        source->x += stick.x * lightSpeedTbl[lightSpeed];
        source->z += stick.z * lightSpeedTbl[lightSpeed];
        if (__builtin_sqrtf(source->x * source->x +
                            source->z * source->z) > 1.0f) {
            float angle = atan2f(source->x, source->z);
            source->x = xglSin(angle);
            source->z = xglCos(angle);
        }
    }

    horizontal = 1.0f - (source->x * source->x + source->z * source->z);
    direction.vector.x = source->x;
    direction.vector.y = __builtin_sqrtf(horizontal);
    direction.vector.z = source->z;
    if (source->y < 0.0f)
        direction.vector.y = -direction.vector.y;
    xglVectorLength(&stickLength, &direction.vector);

    xglStudioFlushActiveCamera();
    camera = xglStudioGetActiveCamera();
    translation.x = 0.0f;
    translation.y = 0.0f;
    translation.z = 5.0f;
    translation.w = 1.0f;
    __asm__ __volatile__(
        "lq $2,0(%0)\n\t"
        "sq $2,0(%1)\n\t"
        "lq $2,16(%0)\n\t"
        "sq $2,16(%1)\n\t"
        "lq $2,32(%0)\n\t"
        "sq $2,32(%1)\n\t"
        "lq $2,48(%0)\n\t"
        "sq $2,48(%1)"
        :
        : "r" (camera->cameraToWorldMatrix), "r" (cameraMatrix)
        : "$2", "memory");
    xglMatrixStackUnit();
    xglMatrixStackMul((const float (*)[4])cameraMatrix);
    xglMatrixStackTrans(&translation.x);
    xglMatrixStackSave(cameraMatrix);
    cameraMatrix[0][0] = 1.0f;
    cameraMatrix[0][1] = 0.0f;
    cameraMatrix[0][2] = 0.0f;
    cameraMatrix[0][3] = 0.0f;
    cameraMatrix[1][0] = 0.0f;
    cameraMatrix[1][1] = 1.0f;
    cameraMatrix[1][2] = 0.0f;
    cameraMatrix[1][3] = 0.0f;
    cameraMatrix[2][0] = 0.0f;
    cameraMatrix[2][1] = 0.0f;
    cameraMatrix[2][2] = 1.0f;
    cameraMatrix[2][3] = 0.0f;
    drawVector4((const Vector3 *)&direction.vector, &cameraMatrix, 0);

    stick.x = lightCol[lightIndex][0];
    stick.y = lightCol[lightIndex][1];
    stick.z = lightCol[lightIndex][2];
    stick.w = 1.0f;
    switch (lightIndex) {
    case 0:
    case 1:
    case 2:
        xglStudioGetLight(&light);
        xglLightIntensityParallel(light, lightIndex, &stick);
        xglLightDirection(light, lightIndex, &direction.vector);
        break;
    case 3:
        xglStudioGetLight(&light);
        xglLightIntensityAmbient(light, &stick);
        break;
    case 4:
        ClearEnv.color_r = (unsigned int)(stick.x * 255.0f);
        ClearEnv.color_g = (unsigned int)(stick.y * 255.0f);
        ClearEnv.color_b = (unsigned int)(stick.z * 255.0f);
        break;
    case 5:
        GameLoopState.stage_color.x = lightCol[5][0];
        GameLoopState.stage_color.y = lightCol[5][1];
        GameLoopState.stage_color.z = lightCol[5][2];
        break;
    }

    baseX = 192 + lightStateOffset / 2;
    sprintf(text, lightSpeedFormat, 1 + lightSpeed);
    xglFontDebugPrintf(baseX, 160, text);
    sprintf(text, rgbColorFormat, stick.x, stick.y, stick.z);
    xglFontDebugPrintf(baseX, 170, text);
    sprintf(text, directionVectorFormat, direction.vector.x, direction.vector.y, direction.vector.z);
    xglFontDebugPrintf(baseX, 198, text);

    baseX = 200 + lightStateOffset / 2;
    xglFontDebugPrintf(baseX, 88, ambientText);
    xglFontDebugPrintf(baseX + 4, 114, backgroundText);
    xglFontDebugPrintf(baseX, 140, mapText);
    stick.x = 116.0f;
    stick.y = 56.0f;
    stick.z = 1.0f;
    stick.w = 1.0f;
    direction.vector.x = (float)lightStateOffset + 196.0f;
    direction.vector.y = 192.0f - (float)lightIndex * 51.0f;
    direction.vector.z = 0.0f;
    direction.vector.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&direction.vector.x);
    xglMatrixStackScale(&stick.x);
    xglMatrixStackSave(matrix);
    drawBox2(&matrix, 6);

    stick.x = 6.0f;
    stick.y = 6.0f;
    stick.z = 1.0f;
    stick.w = 1.0f;
    direction.vector.x = (float)lightStateOffset + 196.0f;
    direction.vector.y = 192.0f - (float)lightIndex * 51.0f + 12.0f -
                  (float)lightColIndex * 12.0f;
    direction.vector.z = 0.0f;
    direction.vector.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&direction.vector.x);
    xglMatrixStackRotZ(3.14159265f / 4.0f);
    xglMatrixStackScale(&stick.x);
    xglMatrixStackSave(matrix);
    drawBox2(&matrix, 6);

    stick.x = 24.0f;
    stick.y = 24.0f;
    stick.z = 1.0f;
    stick.w = 1.0f;
    direction.vector.y = 192.0f;
    direction.vector.x = (float)lightStateOffset + 166.0f;
    direction.vector.z = 0.0f;
    direction.vector.w = 1.0f;
    for (k = 0; k < 6; k++) {
        float scale;

        source = &lightDir[k];
        xglMatrixStackUnit();
        xglMatrixStackTrans(&direction.vector.x);
        xglMatrixStackScale(&stick.x);
        xglMatrixStackSave(matrix);
        drawCircle2(&matrix, 24.0f, 1);
        if (0.0f != source->w) {
            flat.x = source->x;
            flat.y = -source->z;
            flat.z = 0.0f;
            if (source->y > 0.0f)
                drawVector2(&flat, &matrix, 0);
            else
                drawVector2(&flat, &matrix, 2);
        }
        barPosition = direction;
        barPosition.vector.x += 36.0f;
        barPosition.vector.y += 16.0f;
        barScale.z = 1.0f;
        barScale.w = 1.0f;
        barScale.y = 8.0f;
        scale = 1.0f;
        for (j = 0; j < 4; j++) {
            barScale.x = lightCol[k][j] * 48.0f * scale;
            xglMatrixStackUnit();
            xglMatrixStackTrans(&barPosition.vector.x);
            xglMatrixStackScale(&barScale.x);
            xglMatrixStackSave(matrix);
            if (j < 3)
                drawRect2(&matrix, j);
            else
                drawRect2(&matrix, 6);
            barPosition.vector.y -= 12.0f;
        }
        direction.vector.y -= 51.0f;
    }

    if (keys->pressed & 0x100) {
        MapGameLoopStateView *state;

        i = 0;
        xglStudioGetLight((StudioLight **)&dumpLight);
        sprintf(dumpText, lightSetColorFormat, i, dumpLight->ambient_color[0],
                dumpLight->ambient_color[1], dumpLight->ambient_color[2]);
        state = &GameLoopState;
        do {
            sprintf(dumpText, lightSetColorFormat, i + 1, dumpLight->lights[i].color[0],
                    dumpLight->lights[i].color[1],
                    dumpLight->lights[i].color[2]);
            sprintf(dumpText, lightSetDirection2Format, i + 1, dumpLight->lights[i].direction[0],
                    dumpLight->lights[i].direction[1],
                    dumpLight->lights[i].direction[2]);
            sprintf(dumpText, stageSetColorFormat, state->stage_color.x,
                    state->stage_color.y,
                    state->stage_color.z);
        } while (++i < 3);
    }
}

void updateWind(WindRecord *wind)
{
    StudioCamera *camera;
    DbLightWritePadSecondPrefix *pad;
    char text[256];
    char text_line[256];
    WindMarker marker;
    char text_wide[128];
    char *wide_text;
    unsigned short buttons;
    int stick;
    int selection;
    int mode;
    float step;
    float angle;
    float cos_angle;
    float sin_angle;
    float stick_x;
    float stick_y;
    const char *format1, *format2, *format3, *format4, *format5, *format6, *format7, *format8, *format9, *format10;
    char *line;

    pad = &PadData.second;
    camera = xglStudioGetActiveCamera();
    buttons = pad->half_2c;
    if (buttons & 0x1000) {
        selection = csr_1;
        selection += 4;
        selection %= 5;
        csr_1 = selection;
    }
    if (buttons & 0x4000) {
        selection = csr_1;
        selection += 1;
        selection %= 5;
        csr_1 = selection;
    }
    selection = csr_1;
    if ((pad->half_2a & 0x20) && selection < 3) {
        wind->mode = csr_1;
        selection = csr_1;
    }
    stick = pad->half_28;
    buttons = stick;
    step = (buttons & 0x80) ? 0.001f : 0.01f;

    if (selection == 3) {
        if (buttons & 0x8000) {
            wind->offset_a -= step;
        }
        if (buttons & 0x2000) {
            wind->offset_a += step;
        }
    }
    if (selection == 4) {
        float offset_clamp;

        if (buttons & 0x8000) {
            wind->offset_b -= step;
        }
        if (buttons & 0x2000) {
            wind->offset_b += step;
        }
        offset_clamp = wind->offset_b;
        if (offset_clamp > (3.14159265f * 2.0f)) {
            wind->offset_b = (3.14159265f * 2.0f);
            offset_clamp = (3.14159265f * 2.0f);
        }
        if (offset_clamp < (-3.14159265f * 2.0f)) {
            wind->offset_b = (-3.14159265f * 2.0f);
        }
    }
    if (buttons & 0x4) {
        wind->w = wind->w + step * 0.1f;
    }
    if (buttons & 0x1) {
        wind->w -= step * 0.1f;
    }
    if (wind->mode == 1) {
        step *= 0.1f;
    }
    if (buttons & 0x8) {
        wind->y += step * 5.0f;
    }
    if (buttons & 0x2) {
        wind->y -= step * 5.0f;
    }

    angle = camera->angleVelocity.y;
    cos_angle = xglCos(angle);
    sin_angle = xglSin(angle);

    stick_x = (float)pad->stick_x * step * 0.1f;
    stick_y = (float)pad->stick_y * step * 0.1f;
    wind->x = wind->x + (cos_angle * stick_x - sin_angle * stick_y);
    wind->z = wind->z + (cos_angle * stick_y + sin_angle * stick_x);

    if (pad->half_2a & 0x10) {
        mode = wind->mode;
        EXM_ResetWind();
        wind->mode = mode;
        if (mode == 1) {
            wind->w = 1.0f;
        }
    }

    xglFontDebugPrintf(0, 0, windSettingsText);
    xglFontDebugPrintf(150, dispy_2 - 12, windTypeText);
    mode = wind->mode;
    if (mode == 0) {
        xglFontDebugPrintf(162, dispy_2, windOffSelectedText);
    } else {
        xglFontDebugPrintf(162, dispy_2, windOffText);
    }
    if (mode == 1) {
        xglFontDebugPrintf(162, dispy_2 + 12, windDirectionalSelectedText);
    } else {
        xglFontDebugPrintf(162, dispy_2 + 12, windDirectionalText);
    }
    if (mode == 2) {
        xglFontDebugPrintf(162, dispy_2 + 24, windPointSelectedText);
    } else {
        xglFontDebugPrintf(162, dispy_2 + 24, windPointText);
    }
    xglFontDebugPrintf(150, dispy_2 + 36, windShakeText);
    format1 = windPowerFormat;
    sprintf(text, format1, fptodp(wind->offset_a));
    xglFontDebugPrintf(162, dispy_2 + 48, text);
    format2 = windSpeedFormat;
    sprintf(text, format2, fptodp(wind->offset_b));
    xglFontDebugPrintf(162, dispy_2 + 60, text);
    xglFontDebugPrintf(170, dispy_2 + 72, windVectorText);
    format3 = windXFormat;
    line = text_line;
    sprintf(line, format3, fptodp(wind->x));
    xglFontDebugPrintf(182, dispy_2 + 80, line);
    format4 = windYFormat;
    sprintf(line, format4, fptodp(wind->y));
    xglFontDebugPrintf(182, dispy_2 + 88, line);
    format5 = windZFormat;
    sprintf(line, format5, fptodp(wind->z));
    xglFontDebugPrintf(182, dispy_2 + 96, line);
    format6 = windWFormat;
    sprintf(line, format6, fptodp(wind->w));
    xglFontDebugPrintf(182, dispy_2 + 104, line);
    if (wind->mode == 1) {
        marker.from.raw[0] = *(unsigned long long *)((char *)camera + 288);
        marker.from.raw[1] = *(unsigned long long *)((char *)camera + 296);
        marker.color[0] = 255;
        marker.color[1] = 60;
        marker.color[2] = 60;
        marker.color[3] = 255;
        marker.to.x = marker.from.vector.x + wind->x;
        marker.to.y = marker.from.vector.y + wind->y;
        marker.to.z = marker.from.vector.z + wind->z;
        marker.from.vector.w = 0.05f;
    }
    if (csr_1 < 3) {
        xglFontDebugPrintf(150, dispy_2 + csr_1 * 12, cursorMarkerText);
    } else {
        xglFontDebugPrintf(150, dispy_2 + csr_1 * 12 + 12, cursorMarkerText);
    }
    if (pad->half_2a & 0x100) {
        wide_text = text_wide;
        format7 = windShakeCommandFormat;
        sprintf(wide_text, format7, fptodp(wind->offset_a), fptodp(wind->offset_b));
        switch (wind->mode) {
        case 1:
            format8 = directionalWindCommandFormat;
            sprintf(wide_text, format8, fptodp(wind->x), fptodp(wind->y), fptodp(wind->z), fptodp(wind->w));
            break;
        case 2:
            format9 = pointWindCommandFormat;
            sprintf(wide_text, format9, fptodp(wind->x), fptodp(wind->y), fptodp(wind->z), fptodp(wind->w));
            break;
        case 0:
            format10 = stopWindCommandFormat;
            sprintf(wide_text, format10);
            break;
        }
    }
}

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

static float cursorSpeed[9] = { 0.010000000707805157f, 0.01666666753590107f, 0.066666670143604279f, 0.13333334028720856f, 0.26666668057441711f, 0.3333333432674408f, 0.53333336114883423f, 1.0666667222976685f, 4.2666668891906738f };

static Vector4 vGridXZ[5] = { { .x = -0.5f, .y = 0.0f, .z = -0.5f, .w = 1.0f }, { .x = 0.5f, .y = 0.0f, .z = -0.5f, .w = 1.0f }, { .x = 0.5f, .y = 0.0f, .z = 0.5f, .w = 1.0f }, { .x = -0.5f, .y = 0.0f, .z = 0.5f, .w = 1.0f }, { .x = -0.5f, .y = 0.0f, .z = -0.5f, .w = 1.0f } };

static Vector4 vCursor[6] = { { .x = -0.5f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.5f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = -0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = -0.5f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.5f, .w = 1.0f } };

DebugCursorColour cCursor[6] = { { .red = 192, .green = 192, .blue = 192, .alpha = 128 }, { .red = 192, .green = 0, .blue = 0, .alpha = 128 }, { .red = 192, .green = 192, .blue = 192, .alpha = 128 }, { .red = 0, .green = 192, .blue = 0, .alpha = 128 }, { .red = 192, .green = 192, .blue = 192, .alpha = 128 }, { .red = 0, .green = 0, .blue = 192, .alpha = 128 } };

static Vector4 vRect[4] = { { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = -1.0f, .z = 0.0f, .w = 1.0f }, { .x = 1.0f, .y = -1.0f, .z = 0.0f, .w = 1.0f } };

static Vector4 vBox[5] = { { .x = -0.5f, .y = 0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.5f, .y = 0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.5f, .y = -0.5f, .z = 0.0f, .w = 1.0f }, { .x = -0.5f, .y = -0.5f, .z = 0.0f, .w = 1.0f }, { .x = -0.5f, .y = 0.5f, .z = 0.0f, .w = 1.0f } };

static Vector4 lightDir[6] = { { .x = 0.0f, .y = 1.0f, .z = 0.77999997138977051f, .w = 1.0f }, { .x = 0.77999997138977051f, .y = 1.0f, .z = 0.0f, .w = 1.0f }, { .x = -0.61500000953674316f, .y = -1.0f, .z = -0.61500000953674316f, .w = 1.0f }, { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f } };

float lightSpeedTbl[4] = { 0.004999999888241291f, 0.0099999997764825821f, 0.019999999552965164f, 0.029999999329447746f };

const char lightSetColorFormat[48] = "light.setColor(%2d, %7.3ff, %7.3ff, %7.3ff);\n";

const char lightSetDirection2Format[56] = "light.setDirection2(%2d, %7.3ff, %7.3ff, %7.3ff);\n";

const char stageSetColorFormat[48] = "Stage.setColor(%1.3ff, %1.3ff, %1.3ff);\n";

const char mapHeightFormat[16] = "MAP-HEIGHT: %f";

/* Text: MAP-HEIGHT:{color #800000}HOLE_HEIGHT{color #808080} */
const char mapHoleHeightText[40] = "MAP-HEIGHT:\014\200\000\000HOLE_HEIGHT\014\200\200\200 ";

const char cursorPositionFormat[32] = "IX:%7.3f\nIY:%7.3f\nIZ:%7.3f";

const char cameraPositionFormat[32] = "PX:%7.3f\nPY:%7.3f\nPZ:%7.3f";

const char cursorModeSpeedFormat[16] = "%s %1d SPD:%1d";

const char matrixRowFormat[24] = "%6.2f %6.2f %6.2f %6.2f";

const char rgbColorFormat[32] = "R: %4.3f\nG: %4.3f\nB: %4.3f";

const char directionVectorFormat[32] = "X:%6.3f\nY:%6.3f\nZ:%6.3f";

/* Text: {reset}{color #800000}OFF */
const char windOffSelectedText[16] = "\013\014\200\000\000OFF";

/* Text: {reset}{color #800000}DIRECTIONAL */
const char windDirectionalSelectedText[24] = "\013\014\200\000\000DIRECTIONAL";

/* Text: {reset}DIRECTIONAL */
const char windDirectionalText[16] = "\013DIRECTIONAL";

/* Text: {reset}{color #800000}POINT */
const char windPointSelectedText[16] = "\013\014\200\000\000POINT";

/* Text: {reset}POW:%+2.3f */
const char windPowerFormat[16] = "\013POW:%+2.3f";

/* Text: {reset}SPD:%+2.3f */
const char windSpeedFormat[16] = "\013SPD:%+2.3f";

/* Text: {reset}X: %4.4f */
const char windXFormat[16] = "\013X: %4.4f";

/* Text: {reset}Y: %4.4f */
const char windYFormat[16] = "\013Y: %4.4f";

/* Text: {reset}Z: %4.4f */
const char windZFormat[16] = "\013Z: %4.4f";

/* Text: {reset}W: %4.4f */
const char windWFormat[16] = "\013W: %4.4f";

const char windShakeCommandFormat[32] = "setWindShake(%3.4ff, %3.4ff);\n";

const char directionalWindCommandFormat[56] = "setDirectionalWind(%3.4ff, %3.4ff, %3.4ff, %3.4ff);\n";

const char pointWindCommandFormat[48] = "setPointWind(%3.4ff, %3.4ff, %3.4ff, %3.4ff);\n";

const char stopWindCommandFormat[16] = "stopWind();\n";

const float cursorDirectionLength = 0.13333334028720856f;

const float cursorPlaneRotateDecrement = 0.01163552887737751f;

const float cursorPlaneRotateIncrement = 0.01163552887737751f;

const float cursorMode0RotateDecrement = 0.01163552887737751f;

const float cursorMode0RotateIncrement = 0.01163552887737751f;

static int visible_0 = 0;

const char partsIndexFormat[4] = "%3d";

static int lightColIndex = 3;

static int lightSpeed = 1;

static int lightStateOffset = 0;

const char lightSpeedFormat[8] = "SPD: %d";

const char ambientText[8] = "AMB";

const char backgroundText[8] = "BG";

const char mapText[4] = "MAP";

static int csr_1 = 0;

static int dispy_2 = 32;

/* JP: 風設定
 * EN: Wind settings */
const char windSettingsText[8] = "\311\367\300\337\304\352";

/* Text: {reset}TYPE */
const char windTypeText[8] = "\013TYPE";

/* Text: {reset}OFF */
const char windOffText[8] = "\013OFF";

/* Text: {reset}POINT */
const char windPointText[8] = "\013POINT";

/* Text: {reset}SHAKE */
const char windShakeText[8] = "\013SHAKE";

/* Text: {reset}VECTOR */
const char windVectorText[8] = "\013VECTOR";

/* Text: {reset}> */
const char cursorMarkerText[8] = "\013>";

extern const char DBCursorModeXzLabel[8];
extern const char DBCursorModeFreeLabel[8];
extern const char DBCursorModeYzLabel[8];
extern const char DBCursorModeXyLabel[8];
extern const char DBCursorModeFcvLabel[8];

static const char * cursorModeName[5] = { DBCursorModeFreeLabel, DBCursorModeYzLabel, DBCursorModeXzLabel, DBCursorModeXyLabel, DBCursorModeFcvLabel };

static unsigned char col[56] = { 192, 192, 192, 128, 192, 0, 0, 128, 192, 192, 192, 128, 0, 192, 0, 128, 192, 192, 192, 128, 0, 0, 192, 128, 192, 192, 192, 128, 192, 192, 0, 128, 192, 192, 192, 128, 0, 192, 192, 128, 192, 192, 192, 128, 192, 0, 192, 128, 16, 16, 16, 128, 192, 192, 192, 128 };

static RgVector lightCol[6] = { { 0.6f, 0.6f, 0.6f, 1.0f }, { 0.6f, 0.6f, 0.6f, 1.0f }, { 0.6f, 0.6f, 0.6f, 1.0f }, { 0.2f, 0.2f, 0.2f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };

