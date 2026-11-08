#include "common.h"
#include "shared.h"
#include "main/get.h"
#include "db_light_write.h"
#include "main/db_light_write.h"
#include "main/xgl_studio.h"
#include "main/xgl_2.h"
#include "main/Undulate.h"
#include "main/exm_2.h"
#include "main/matrix_print.h"
#include "main/transform_vector.h"
#include "main/xgl_font.h"
#include "main/xgl_prim_add.h"
#include "quadword.h"

/* Layout views of game structures used by the debug functions below. */
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

/* One controller of PadData (two 0x68-byte entries). held, pressed and repeat
   are the halfwords at +0x28, +0x2a and +0x2c that src/main/input.c reads. */
typedef struct DbLightWritePad {
    unsigned char unmodeled_00[0x28];
    unsigned short held;
    unsigned short pressed;
    unsigned short repeat;
    unsigned char unmodeled_2e[0x66 - 0x2e];
    signed char stick_x;
    signed char stick_y;
} DbLightWritePad;

/* Clear-environment colour words (the GS clear block's color_r/g/b at 0x20). */
typedef struct {
    unsigned char unmodeled_00[0x20];
    unsigned int color_r;
    unsigned int color_g;
    unsigned int color_b;
} ClearEnvColorView;

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

typedef struct WindMarker {
    DoublewordVector4 from; /* copied whole from camera->chasePosition (a DoublewordVector4 in shared.h): ld/sd needs the union-to-union copy */
    Vector4 to;
    int color[4];
} WindMarker;

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

/* Both scratchpad layouts start with the GIF packet the debug primitives
   build: one quadword (four words) per row, row 0 the GIFtag
   (count, primitive, registers, 0), then colour/position row pairs. */
typedef struct {
    unsigned int packet[0x40][4];
    TransformParams transform;
    unsigned char unmodeled_4a4[0x4e0 - 0x4a4];
    Vector4 positions[5];
} DebugRectScratch;

typedef struct {
    unsigned int packet[0x40][4];
    TransformParams transform;
    unsigned char unmodeled_4a4[0x4b0 - 0x4a4];
    Vector4 input[2];
    unsigned char unmodeled_4d0[0x10];
    Vector4 output[2];
} DebugLineScratch;

typedef struct DebugCursorColour {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} DebugCursorColour;

/* One colour of the light table: read as a float[4] row by the light editor
   and as the Vector4 that the light helpers take. */
typedef union LightColour {
    RgVector components;
    Vector4 vector;
} LightColour;

extern MapGameLoopStateView GameLoopState;
extern DbLightWritePad PadData[2];
extern const char matrixRowFormat[];
extern const char partsIndexFormat[];
extern const char lightSetColorFormat[];
extern const char lightSetDirection2Format[];
extern const char stageSetColorFormat[];
extern ClearEnvColorView ClearEnv;
extern const float cursorDirectionLength;
extern CameraChannelRecord tcamera;
extern const float cursorPlaneRotateDecrement;
extern const float cursorPlaneRotateIncrement;
extern const char mapHeightFormat[];
extern const char mapHoleHeightText[];
extern const float cursorMode0RotateDecrement;
extern const float cursorMode0RotateIncrement;
extern const char cursorPositionFormat[];
extern const char cameraPositionFormat[];
extern const char cursorModeSpeedFormat[];
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
extern DebugCursorColour cCursor[6];
extern float lightSpeedTbl[4];
extern const char rgbColorFormat[];
extern const char directionVectorFormat[];
extern const char lightSpeedFormat[];
extern const char ambientText[];
extern const char backgroundText[];
extern const char mapText[];
extern const char DBCursorModeXzLabel[8];
extern const char DBCursorModeFreeLabel[8];
extern const char DBCursorModeYzLabel[8];
extern const char DBCursorModeXyLabel[8];
extern const char DBCursorModeFcvLabel[8];

extern void nmlModelSetPartsVisible(MapPartListView *parts,
                                    int part_index, int visible);

extern void xglLightIntensityParallel(void *light, int index,
                                      const Vector4 *intensity);

extern void xglLightIntensityAmbient(void *light,
                                     const Vector4 *intensity);

extern void xglLightDirection(void *light, unsigned int index,
                              const Vector4 *direction);

extern float atan2f(float y, float x);
extern int sceWrite(int descriptor, const void *buffer, int size);
extern double fptodp(float value);
extern StudioCamera *xglStudioGetActiveCamera(void);

extern void ACT_updateSequence(Actor *actor);
void updateCursorModePlane(void);
extern void PAD_getKey(struct CursorPadKeyResult *result, int port, int mask);
void drawVector4(const Vector3 *vector, Matrix4 *matrix, int color_index);
void drawCircle2(Matrix4 *matrix, float radius, int color_index);
void updateCursorMode0(void);
void updateCursorMode2(void);

extern void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);
extern float dptofp(double value);
static int visible_0;

static Vector4 lightDir[6];

static LightColour lightCol[6];

static float cursorSpeed[9];

static const char * cursorModeName[5];

static int dispy_2;

static int csr_1;

static unsigned char col[56];

static Vector4 vRect[4] ;

static Vector4 vBox[5] ;

static Vector4 vGridXZ[5] ;

static Vector4 vCursor[6] ;

static int lightColIndex;

static int lightSpeed;

static int lightStateOffset;

static float cursorSpeed[9] = { 0.010000000707805157f, 0.01666666753590107f, 0.066666670143604279f, 0.13333334028720856f, 0.26666668057441711f, 0.3333333432674408f, 0.53333336114883423f, 1.0666667222976685f, 4.2666668891906738f };

static Vector4 vGridXZ[5] = { { .x = -0.5f, .y = 0.0f, .z = -0.5f, .w = 1.0f }, { .x = 0.5f, .y = 0.0f, .z = -0.5f, .w = 1.0f }, { .x = 0.5f, .y = 0.0f, .z = 0.5f, .w = 1.0f }, { .x = -0.5f, .y = 0.0f, .z = 0.5f, .w = 1.0f }, { .x = -0.5f, .y = 0.0f, .z = -0.5f, .w = 1.0f } };

static Vector4 vCursor[6] = { { .x = -0.5f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.5f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = -0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = -0.5f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.5f, .w = 1.0f } };

DebugCursorColour cCursor[6] = { { .red = 192, .green = 192, .blue = 192, .alpha = 128 }, { .red = 192, .green = 0, .blue = 0, .alpha = 128 }, { .red = 192, .green = 192, .blue = 192, .alpha = 128 }, { .red = 0, .green = 192, .blue = 0, .alpha = 128 }, { .red = 192, .green = 192, .blue = 192, .alpha = 128 }, { .red = 0, .green = 0, .blue = 192, .alpha = 128 } };

static Vector4 vRect[4] = { { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = -1.0f, .z = 0.0f, .w = 1.0f }, { .x = 1.0f, .y = -1.0f, .z = 0.0f, .w = 1.0f } };

static Vector4 vBox[5] = { { .x = -0.5f, .y = 0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.5f, .y = 0.5f, .z = 0.0f, .w = 1.0f }, { .x = 0.5f, .y = -0.5f, .z = 0.0f, .w = 1.0f }, { .x = -0.5f, .y = -0.5f, .z = 0.0f, .w = 1.0f }, { .x = -0.5f, .y = 0.5f, .z = 0.0f, .w = 1.0f } };

static Vector4 lightDir[6] = { { .x = 0.0f, .y = 1.0f, .z = 0.77999997138977051f, .w = 1.0f }, { .x = 0.77999997138977051f, .y = 1.0f, .z = 0.0f, .w = 1.0f }, { .x = -0.61500000953674316f, .y = -1.0f, .z = -0.61500000953674316f, .w = 1.0f }, { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f } };

float lightSpeedTbl[4] = { 0.004999999888241291f, 0.0099999997764825821f, 0.019999999552965164f, 0.029999999329447746f };

static const char * cursorModeName[5] = { DBCursorModeFreeLabel, DBCursorModeYzLabel, DBCursorModeXzLabel, DBCursorModeXyLabel, DBCursorModeFcvLabel };

static unsigned char col[56] = { 192, 192, 192, 128, 192, 0, 0, 128, 192, 192, 192, 128, 0, 192, 0, 128, 192, 192, 192, 128, 0, 0, 192, 128, 192, 192, 192, 128, 192, 192, 0, 128, 192, 192, 192, 128, 0, 192, 192, 128, 192, 192, 192, 128, 192, 0, 192, 128, 16, 16, 16, 128, 192, 192, 192, 128 };

static LightColour lightCol[6] = { { { 0.6f, 0.6f, 0.6f, 1.0f } }, { { 0.6f, 0.6f, 0.6f, 1.0f } }, { { 0.6f, 0.6f, 0.6f, 1.0f } }, { { 0.2f, 0.2f, 0.2f, 1.0f } }, { { 0.0f, 0.0f, 0.0f, 1.0f } }, { { 1.0f, 1.0f, 1.0f, 1.0f } } };

static int mode_004DC5A8;

CursorState cursor;

static int partsIndex;

static struct CursorPadKeyResult keyData;

static int toolFlags;

int cursorMode;

static int lightIndex;

/*
 * Writes the light setup to descriptor as script lines: the ambient colour, the
 * colour and direction of lights 1 to 3, and the stage colour.
 */
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
                         light->lights[lightIndex].color_row[0],
                         light->lights[lightIndex].color_row[1],
                         light->lights[lightIndex].color_row[2]));
        sceWrite(descriptor, buffer,
                 sprintf(buffer, lightSetDirection2Format, lightNumber,
                         light->lights[lightIndex].direction_row[0],
                         light->lights[lightIndex].direction_row[1],
                         light->lights[lightIndex].direction_row[2]));
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

/*
 * Draws the unit rectangle vRect transformed by matrix, in the colour from the
 * col table selected by color_index, as a GIF packet in the scratchpad.
 */
void drawRect2(Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugRectScratch *scratch = (DebugRectScratch *)SCRATCHPAD_BASE;
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
    scratch->transform.screenOffset = camera->screenOffset;
    scratch->transform.screenScale.x = 1.0f;
    scratch->transform.screenScale.y = -1.0f;
    scratch->transform.screenScale.z = 1.0f;
    scratch->transform.screenScale.w = 1.0f;
    scratch->transform.local = matrix;
    color_index = color_index * 4 + 4;
    transformVector(scratch->positions, vRect, 4, &scratch->transform);
    write = scratch->packet[0];
    write[0] = 0x8004;
    write[1] = 0x20064000;
    write[2] = 0xffffff51;
    write[3] = 0;
    write += 4;
    position = scratch->positions;
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
    xglPrimAddGifTagDirect(0, scratch, 9);
}

/* Draws the box vBox transformed by matrix, in the colour from the col table at color_index. */
void drawBox2(Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugRectScratch *scratch = (DebugRectScratch *)SCRATCHPAD_BASE;
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
    scratch->transform.screenOffset = camera->screenOffset;
    scratch->transform.screenScale.x = 1.0f;
    scratch->transform.screenScale.y = -1.0f;
    scratch->transform.screenScale.z = 1.0f;
    scratch->transform.screenScale.w = 1.0f;
    scratch->transform.local = matrix;
    color_index = color_index * 4 + 4;
    transformVector(scratch->positions, vBox, vertex_count, &scratch->transform);
    write = scratch->packet;
    (*write)[0] = 0x8000 | vertex_count;
    (*write)[1] = 0x20054000;
    (*write)[2] = 0xffffff51;
    (*write)[3] = 0;
    write++;
    position = scratch->positions;
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
    xglPrimAddGifTagDirect(0, scratch, vertex_count * 2 + 1);
}

/*
 * Draws a circle of the given radius around the origin of matrix, as a closed
 * line loop of 16 segments in the colour selected by color_index.
 */
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
    DebugLineScratch *scratch = (DebugLineScratch *)SCRATCHPAD_BASE;
    unsigned int *write;
    float angle;
    float scale;

    color_index *= 2;
    xglStudioGetCamera(&camera, 0);
    xglMatrixStackUnit();
    xglMatrixStackSave(scratch->transform.world);
    scratch->transform.screenOffset = camera->screenOffset;
    scratch->transform.screenScale.x = 1.0f;
    scratch->transform.screenScale.y = -1.0f;
    scratch->transform.screenScale.z = 1.0f;
    scratch->transform.screenScale.w = 1.0f;
    scratch->transform.local = matrix;
    scratch->input[0].x = 0.0f;
    scratch->input[0].y = 0.0f;
    scratch->input[0].z = 0.0f;
    scratch->input[0].w = 1.0f;
    transformVector(scratch->output, scratch->input, 1, &scratch->transform);
    colour = color_index * 4 + 4;
    angle = 0.0f;
    scale = 16.0f;
    write = scratch->packet[0];
    scratch->packet[0][0] = 0x8011;
    scratch->packet[0][1] = 0x20054000;
    scratch->packet[0][2] = 0xffffff51;
    scratch->packet[0][3] = 0;
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
    xglPrimAddGifTagDirect(0, scratch, 35);
}

/* Draws a line from the origin to vector, transformed by matrix, in the colour selected by color_index. */
void drawVector2(const Vector3 *vector, Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugLineScratch *scratch = (DebugLineScratch *)SCRATCHPAD_BASE;
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;

    color_index *= 2;
    xglStudioGetCamera(&camera, 0);
    xglMatrixStackUnit();
    xglMatrixStackSave(scratch->transform.world);
    scratch->transform.screenOffset = camera->screenOffset;
    scratch->transform.screenScale.x = 1.0f;
    scratch->transform.screenScale.y = -1.0f;
    scratch->transform.screenScale.z = 1.0f;
    scratch->transform.screenScale.w = 1.0f;
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
    write = scratch->packet;
    (*write)[0] = 0x8002;
    (*write)[1] = 0x2004c000;
    (*write)[2] = 0xffffff51;
    (*write)[3] = 0;
    write++;
    position = scratch->output;
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
    xglPrimAddGifTagDirect(0, scratch, 5);
}

/*
 * Draws the same line as drawVector2, with the view taken from camera slot 0 and
 * the scratch area the caller supplies.
 */
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
    scratch->transform.screenOffset = camera->screenOffset;
    scratch->transform.screenScale = camera->screenScale;
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
    write = scratch->packet;
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

/*
 * Draws the ground grid vGridXZ, scaled by scale and transformed by matrix, in
 * the cCursor[0] colour. Draws nothing when a corner is outside the visible range.
 */
void drawGrid(Matrix4 matrix, float scale)
{
    Vector4 scale_vector;
    Matrix4 local_matrix;
    Matrix4 *local_matrix_pointer;
    StudioCamera *camera;
    StudioCamera *source_camera;
    TransformParams *params = (TransformParams *)(SCRATCHPAD_BASE + 0x400);
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;

    xglStudioGetCamera(&camera, 0);
    source_camera = camera;
    params->screenOffset = source_camera->screenOffset;
    __builtin_memcpy(params->world, source_camera->viewMatrix, sizeof(Matrix4));
    params->screenScale = source_camera->screenScale;
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
    transformVector((Vector4 *)(SCRATCHPAD_BASE + 0x4e0), vGridXZ, 5, params);
    position = (Vector4 *)(SCRATCHPAD_BASE + 0x4e0);
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
    position = (Vector4 *)(SCRATCHPAD_BASE + 0x4e0);
    write = (unsigned int (*)[4])(SCRATCHPAD_BASE + 0x10);
    ((unsigned int *)SCRATCHPAD_BASE)[0] = 0x8005;
    ((unsigned int *)SCRATCHPAD_BASE)[1] = 0x20054000;
    ((unsigned int *)SCRATCHPAD_BASE)[2] = 0xffffff51;
    ((unsigned int *)SCRATCHPAD_BASE)[3] = 0;
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
    xglPrimAddGifTagDirect(0, (const void *)SCRATCHPAD_BASE, 11);
}

/* Returns the first active studio camera among the first eight, or 0 when none is active. */
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

/*
 * Draws the axis markers vCursor around matrix, scaled by scale, using the
 * current camera. Draws nothing when the first point is outside the visible range.
 */
void drawAxis(Matrix4 matrix, float scale)
{
    Vector4 scale_vector;
    Matrix4 local_matrix;
    Matrix4 *local_matrix_pointer;
    StudioCamera *camera;
    TransformParams *params = (TransformParams *)(SCRATCHPAD_BASE + 0x400);
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;

    camera = getCurrentCamera();
    params->screenOffset = camera->screenOffset;
    __builtin_memcpy(params->world, camera->viewMatrix, sizeof(Matrix4));
    params->screenScale = camera->screenScale;
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
    transformVector((Vector4 *)(SCRATCHPAD_BASE + 0x4e0), vCursor, 6, params);
    position = (Vector4 *)(SCRATCHPAD_BASE + 0x4e0);
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
    ((unsigned int *)SCRATCHPAD_BASE)[0] = 0x8006;
    ((unsigned int *)SCRATCHPAD_BASE)[1] = 0x2004c000;
    ((unsigned int *)SCRATCHPAD_BASE)[2] = 0xffffff51;
    ((unsigned int *)SCRATCHPAD_BASE)[3] = 0;
    write = (unsigned int (*)[4])(SCRATCHPAD_BASE + 0x10);
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
    xglPrimAddGifTagDirect(0, (const void *)SCRATCHPAD_BASE, 13);
}

/*
 * Draws a circle around position, turned to face the camera, in the colour from
 * color_index. The radius is the absolute x offset from 2048 of the transformed point (position->w, 0, 0).
 */
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
    ((DebugLineScratch *)SCRATCHPAD_BASE)->transform.screenOffset = (*camera)->screenOffset;
    *(Matrix4Block *)((DebugLineScratch *)SCRATCHPAD_BASE)->transform.world = *(Matrix4Block *)(*camera)->viewMatrix;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->transform.screenScale = (*camera)->screenScale;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->transform.local = matrix;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].x = position->x;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].y = position->y;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].z = position->z;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].w = 1.0f;
    transformVector(((DebugLineScratch *)SCRATCHPAD_BASE)->output, ((DebugLineScratch *)SCRATCHPAD_BASE)->input, 1, &((DebugLineScratch *)SCRATCHPAD_BASE)->transform);
    /* Build the camera rotation (translation cleared, transposed) and apply it to matrix. */
    QuadwordCopyMatrix(&rotation, (*camera)->cameraToWorldMatrix);
    rotation.elements[12] = 0.0f;
    rotation.elements[13] = 0.0f;
    rotation.elements[14] = 0.0f;
    MATRIX_transpose3((float (*)[4])rotation.elements);
    ((DebugLineScratch *)SCRATCHPAD_BASE)->transform.local = (Matrix4 *)rotation.elements;
    xglMatrixMul(*matrix, (float (*)[4])rotation.elements, *matrix);
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].x = position->w;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].y = 0.0f;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].z = 0.0f;
    ((DebugLineScratch *)SCRATCHPAD_BASE)->input[0].w = 1.0f;
    write = (unsigned int *)SCRATCHPAD_BASE;
    zero = 0.0;
    transformVector(&((DebugLineScratch *)SCRATCHPAD_BASE)->output[1], ((DebugLineScratch *)SCRATCHPAD_BASE)->input, 1, &((DebugLineScratch *)SCRATCHPAD_BASE)->transform);
    write[0] = 0x8011;
    write[1] = 0x20054000;
    write[2] = 0xffffff51;
    write[3] = 0;
    write += 4;
    magnitude = fptodp(((Vector4 *)(SCRATCHPAD_BASE + 0x4f0))->x - 2048.0f);
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
    centre = (Vector4 *)(SCRATCHPAD_BASE + 0x4e0);
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
    xglPrimAddGifTagDirect(0, (const void *)SCRATCHPAD_BASE, 35);
}

/* Prints label at the screen position of position, when that position is inside the visible range. */
void drawTags(const Vector3 *position, Matrix4 *matrix, const char *label)
{
    Vector4 *output;
    StudioCamera *camera;
    StudioCamera *camera_slot;
    DebugLineScratch *scratch = (DebugLineScratch *)SCRATCHPAD_BASE;

    xglStudioGetCamera(&camera_slot, 0);
    camera = camera_slot;
    scratch->transform.screenOffset = camera->screenOffset;
    *(Matrix4Block *)scratch->transform.world = *(Matrix4Block *)camera->viewMatrix;
    scratch->transform.screenScale = camera->screenScale;
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

/* Draws a line from the origin to vector, transformed by matrix, with the active camera's view. */
void drawVector4(const Vector3 *vector, Matrix4 *matrix, int color_index)
{
    StudioCamera *camera;
    DebugLineScratch *scratch = (DebugLineScratch *)SCRATCHPAD_BASE;
    Vector4 *position;
    unsigned int (*write)[4];
    int vertex_index;
    const unsigned char *colour;

    color_index *= 2;
    camera = xglStudioGetActiveCamera();
    *(Matrix4Block *)scratch->transform.world = *(Matrix4Block *)camera->viewMatrix;
    scratch->transform.screenOffset = camera->screenOffset;
    scratch->transform.screenScale = camera->screenScale;
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
    write = scratch->packet;
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
    xglPrimAddGifTagDirect(0, scratch, 5);
}

/* Returns the distance between the two positions when it is below radius, otherwise -1. */
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

/* Returns the first actor in use and not flagged 8 at or below startIndex, or 0. */
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

/* Returns the first actor in use and not flagged 8 at or above startIndex, or 0. */
Actor *nextActor(int startIndex)
{
    Actor *entry;
    int i;

    for (i = startIndex; i < ACTOR_COUNT; i++) {
        entry = &actor[i];
        if (entry->inUseId != 0 && !(entry->flags & 8)) {
            return entry;
        }
    }
    return 0;
}

/*
 * Moves the cursor along the pad direction, taken in the camera's frame. R2
 * cycles the stick submode (0 to 2). The cursor moves only when the stick is
 * past 40 units.
 */
void updateCursorMode1(void)
{
    const struct CursorPadKeyResult *keys = &keyData;
    StudioCamera *camera;
    HomogeneousVector *cursor_position;
    Vector4 direction;
    Vector4 transformed;
    Matrix4 camera_matrix;
    float direction_length;
    int submode;

    xglStudioGetCamera(&camera, 0);
    cursor_position = &cursor.position;
    if (keys->pressed & PAD_R2) {
        CursorState *cursor_base = &cursor;
        cursor_base->submode++;
        if (cursor_base->submode >= 3)
            cursor_base->submode = 0;
    }

    direction_length = 0.0f;
    submode = cursor.submode;
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
        QuadwordCopyMatrix(camera_matrix, camera->cameraToWorldMatrix);
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

/*
 * Actor picking: R1 and R2 step the selection through actors, the cursor follows
 * the selected actor, and the attached actor follows the cursor. Triangle
 * attaches an actor within 0.5 units of the cursor, or releases the attached one.
 */
void updateCursorMode2(void)
{
    DbLightWritePad *pad;
    unsigned short keys;
    CursorState *state;
    StudioCamera *camera;
    HomogeneousVector *cursor_position;
    Actor *selected_actor;
    Actor *candidate;
    Vector4 *actor_position;
    int count;

    selected_actor = 0;
    pad = &PadData[1];
    xglStudioGetCamera(&camera, 0);
    cursor_position = &cursor.position;

    /* R1 selects the next actor and R2 the previous one, wrapping at the ends. */
    if (pad->pressed & PAD_R1) {
        state = &cursor;
        selected_actor = nextActor(state->actor_index);
        if (selected_actor == 0) {
            state->actor_index = 0;
        } else {
            state->actor_index = selected_actor->number + 1;
        }
    }
    if (pad->pressed & PAD_R2) {
        state = &cursor;
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

    if (cursor.attached_actor != 0) {
        if (cursor.attach_state == 0) {
            keys = pad->held;
            actor_position = &cursor.attached_actor->rotation;
            if (keys & PAD_UP) {
                cursor.attach_offset.y += 1.0f / 30.0f;
            }
            if (keys & PAD_DOWN) {
                cursor.attach_offset.y -= 1.0f / 30.0f;
            }
            if (keys & PAD_RIGHT) {
                actor_position->y += 3.14159265f / 120.0f;
            }
            if (keys & PAD_LEFT) {
                actor_position->y -= 3.14159265f / 120.0f;
            }
        }
    }

    pad = &PadData[0];
    candidate = cursor.attached_actor;
    if (candidate != 0) {
        if (pad->pressed & PAD_TRIANGLE) {
            selected_actor = candidate;
            cursor.attached_actor = 0;
            selected_actor->velocity.x = 0.0f;
            selected_actor->velocity.y = 0.0f;
            selected_actor->velocity.z = 0.0f;
        } else if (cursor.attach_state == 0) {
            selected_actor = candidate;
            actor_position = &selected_actor->position;
            actor_position->x = cursor_position->x + cursor.attach_offset.x;
            actor_position->y = cursor_position->y + cursor.attach_offset.y;
            actor_position->z = cursor_position->z + cursor.attach_offset.z;
            if (mode_004DC5A8 == 0) {
                ACT_updateSequence(selected_actor);
            }
        }
    } else if (pad->pressed & PAD_TRIANGLE) {
        float threshold;

        cursor.attach_offset.x = 0.0f;
        cursor.attach_offset.y = 0.0f;
        cursor.attach_offset.z = 0.0f;
        threshold = cursor.attach_offset.x;
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
                cursor.attached_actor = selected_actor;
            }
        }
    }
}

/*
 * Plane cursor mode: both sticks move the cursor in the camera's plane, and the
 * camera or the tcamera translation follows depending on flag_04 and mode. Held
 * buttons turn the camera or change the height, and while Cross is held the
 * cursor snaps to the ground height from UnduGet when there is one.
 */
void updateCursorModePlane(void)
{
    const struct CursorPadKeyResult *keys;
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

    keys = &keyData;
    xglStudioGetCamera(&camera, 0);
    position = &cursor.position;
    stick.x = (float)keys->flags[0];
    zero = 0.0f;
    stick.z = (float)keys->flags[1];
    stick.w = 1.0f;
    length = zero;
    stick.y = zero;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the camera-to-world matrix and clear its translation, so it transforms a direction. */
        QuadwordCopyMatrix(&rotation, camera->cameraToWorldMatrix);
        rotation.elements[12] = zero;
        rotation.elements[13] = zero;
        rotation.elements[14] = zero;
        xglVectorMulMat(&direction, rotation.rows, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[cursor.step];
        xglVectorNormal(&direction, &direction);
        switch (cursor.flag_04) {
        case 0:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            active_camera = camera;
            (&active_camera->position.vector)->x += direction.x * length;
            (&active_camera->position.vector)->y += direction.y * length;
            (&active_camera->position.vector)->z += direction.z * length;
            break;
        case 1:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            break;
        case 2:
            if (cursor.mode == 0) {
                active_camera = camera;
                (&active_camera->position.vector)->x += direction.x * length;
                (&active_camera->position.vector)->y += direction.y * length;
                (&active_camera->position.vector)->z += direction.z * length;
            } else {
                tcamera.translation[cursor.mode - 1] += direction.y * length;
            }
            break;
        }
    }
    stick.x = (float)keys->flags[2];
    stick.y = (float)-keys->flags[3];
    stick.w = 1.0f;
    length = 0.0f;
    stick.z = 0.0f;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the camera-to-world matrix and clear its translation, so it transforms a direction. */
        QuadwordCopyMatrix(&rotation, camera->cameraToWorldMatrix);
        rotation.elements[12] = 0.0f;
        rotation.elements[13] = 0.0f;
        rotation.elements[14] = 0.0f;
        xglVectorMulMat(&direction, rotation.rows, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[cursor.step];
        xglVectorNormal(&direction, &direction);
        switch (cursor.flag_04) {
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
            (&active_camera->position.vector)->x += direction.x * length;
            (&active_camera->position.vector)->y += direction.y * length;
            (&active_camera->position.vector)->z += direction.z * length;
            break;
        case 2:
            if (cursor.mode == 0) {
                active_camera = camera;
                (&active_camera->position.vector)->x += direction.x * length;
                (&active_camera->position.vector)->y += direction.y * length;
                (&active_camera->position.vector)->z += direction.z * length;
            } else {
                tcamera.translation[cursor.mode - 1] += direction.y * length;
            }
            break;
        }
    }
    if (keys->held & PAD_RIGHT) {
        (&camera->rotation.vector)->z -= cursorPlaneRotateDecrement;
    }
    if (keys->held & PAD_LEFT) {
        (&camera->rotation.vector)->z += cursorPlaneRotateIncrement;
    }
    length = cursorSpeed[cursor.step];
    if (keys->held & PAD_L1) {
        position->y += length;
        (&camera->position.vector)->y += length;
    }
    if ((unsigned short)(keys->held & PAD_L2)) {
        position->y -= length;
        (&camera->position.vector)->y -= length;
    }
    if (keys->held & PAD_CROSS) {
        height = UnduGet(position->x, position->z);
        if (height != GROUND_HEIGHT_NONE) {
            height_format = mapHeightFormat;
            position->y = height;
            xglFontDebugPrintf(4, 0xC4, height_format, fptodp(height));
        } else {
            missing_format = mapHoleHeightText;
            xglFontDebugPrintf(4, 0xC4, missing_format, fptodp(height));
        }
        if (cursor.flag_34 != 0) {
            position->y -= cursor.offset_44;
        }
    }
}

/*
 * Cursor mode 0: moves the cursor and camera from both sticks, like
 * updateCursorModePlane with the stick pairs swapped. Left and Right turn the camera.
 */
void updateCursorMode0(void)
{
    const struct CursorPadKeyResult *keys;
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

    keys = &keyData;
    xglStudioGetCamera(&camera, 0);
    position = &cursor.position;
    stick.x = (float)keys->flags[2];
    zero = 0.0f;
    stick.z = (float)keys->flags[3];
    stick.w = 1.0f;
    length = zero;
    stick.y = zero;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the camera-to-world matrix and clear its translation, so it transforms a direction. */
        QuadwordCopyMatrix(&rotation, camera->cameraToWorldMatrix);
        rotation.elements[12] = zero;
        rotation.elements[13] = zero;
        rotation.elements[14] = zero;
        xglVectorMulMat(&direction, rotation.rows, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[cursor.step];
        xglVectorNormal(&direction, &direction);
        switch (cursor.flag_04) {
        case 0:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            active_camera = camera;
            (&active_camera->position.vector)->x += direction.x * length;
            (&active_camera->position.vector)->y += direction.y * length;
            (&active_camera->position.vector)->z += direction.z * length;
            break;
        case 1:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            break;
        case 2:
            if (cursor.mode == 0) {
                active_camera = camera;
                (&active_camera->position.vector)->x += direction.x * length;
                (&active_camera->position.vector)->y += direction.y * length;
                (&active_camera->position.vector)->z += direction.z * length;
            } else {
                tcamera.translation[cursor.mode - 1] += direction.y * length;
            }
            break;
        }
    }
    stick.x = (float)keys->flags[0];
    stick.y = (float)-keys->flags[1];
    stick.w = 1.0f;
    length = 0.0f;
    stick.z = 0.0f;
    xglVectorLength(&length, &stick);
    if (length > 40.0f) {
        /* Copy the camera-to-world matrix and clear its translation, so it transforms a direction. */
        QuadwordCopyMatrix(&rotation, camera->cameraToWorldMatrix);
        rotation.elements[12] = 0.0f;
        rotation.elements[13] = 0.0f;
        rotation.elements[14] = 0.0f;
        xglVectorMulMat(&direction, rotation.rows, &stick);
        xglVectorNormal(&stick, &stick);
        xglVectorLength(&length, &stick);
        length *= cursorSpeed[cursor.step];
        xglVectorNormal(&direction, &direction);
        switch (cursor.flag_04) {
        case 0:
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            break;
        case 1:
            if (cursor.mode == 0) {
                position->x += direction.x * length;
                position->y += direction.y * length;
                position->z += direction.z * length;
                break;
            }
            position->x += direction.x * length;
            position->y += direction.y * length;
            position->z += direction.z * length;
            active_camera = camera;
            (&active_camera->position.vector)->x += direction.x * length;
            (&active_camera->position.vector)->y += direction.y * length;
            (&active_camera->position.vector)->z += direction.z * length;
            break;
        case 2:
            if (cursor.mode == 0) {
                active_camera = camera;
                (&active_camera->position.vector)->x += direction.x * length;
                (&active_camera->position.vector)->y += direction.y * length;
                (&active_camera->position.vector)->z += direction.z * length;
            } else {
                tcamera.translation[cursor.mode - 1] += direction.y * length;
            }
            break;
        }
    }
    if (keys->held & PAD_RIGHT) {
        (&camera->rotation.vector)->z -= cursorMode0RotateDecrement;
    }
    if (keys->held & PAD_LEFT) {
        (&camera->rotation.vector)->z += cursorMode0RotateIncrement;
    }
    length = cursorSpeed[cursor.step];
    if (keys->held & PAD_L1) {
        position->y += length;
        (&camera->position.vector)->y += length;
    }
    if ((unsigned short)(keys->held & PAD_L2)) {
        position->y -= length;
        (&camera->position.vector)->y -= length;
    }
    if (keys->held & PAD_CROSS) {
        height = UnduGet(position->x, position->z);
        if (height != GROUND_HEIGHT_NONE) {
            height_format = mapHeightFormat;
            position->y = height;
            xglFontDebugPrintf(4, 0xC4, height_format, fptodp(height));
        } else {
            missing_format = mapHoleHeightText;
            xglFontDebugPrintf(4, 0xC4, missing_format, fptodp(height));
        }
        if (cursor.flag_34 != 0) {
            position->y -= cursor.offset_44;
        }
    }
}

/*
 * Sets up the camera for the current cursor mode: its view is locked to the
 * cursor, and the tcamera translation is set for modes 1 to 3.
 */
void changeCameraMode(void)
{
    StudioCamera *camera;
    Vector4 difference;
    Vector4 *camera_position;
    float distance;
    CursorState *state;
    int mode;

    xglStudioGetCamera(&camera, 0);
    state = &cursor;
    camera_position = &camera->position.vector;
    difference.x = state->position.x - camera_position->x;
    difference.y = state->position.y - camera_position->y;
    difference.z = state->position.z - camera_position->z;
    xglVectorLength(&distance, &difference);
    if (distance < 1.0f) {
        distance = 1.0f;
    }
    mode = state->mode;
    (&camera->rotation.vector)->z = 0.0f;
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
        cursor.flag_04 = 2;
        tcamera.mode0 = 16;
        break;
    case 2: {
        float translationDistance = distance;
        tcamera.mode0 = 20;
        tcamera.unmodeled_40 = 1;
        tcamera.translation_constraint = &cursor.position;
        tcamera.translation[1] = translationDistance;
        tcamera.translation[2] = 0.0f;
        tcamera.translation[0] = 0.0f;
        break;
    }
    case 3:
        tcamera.mode0 = 20;
        tcamera.translation_constraint = &cursor.position;
        tcamera.translation[2] = -distance;
        tcamera.translation[0] = 0.0f;
        tcamera.translation[1] = 0.0f;
        tcamera.unmodeled_40 = 1;
        break;
    case 1:
        tcamera.mode0 = 20;
        tcamera.translation_constraint = &cursor.position;
        tcamera.translation[0] = -distance;
        tcamera.translation[1] = 0.0f;
        tcamera.translation[2] = 0.0f;
        tcamera.unmodeled_40 = 1;
        /* fall through */
    case 4:
        if (cursor.callback != 0) {
            tcamera.mode0 = 16;
            tcamera.mode1 = 16;
        } else {
            cursor.mode = 0;
            changeCameraMode();
        }
        break;
    }
}

/*
 * Reads the pad for tool mode 0 to 3 and updates the cursor. Circle or Square
 * with Up and Down cycles the cursor mode; otherwise Up and Down change the step.
 * L3 zeroes the cursor, R3 resets the camera, and R1 and R2 cycle flag_04.
 */
void updateCursor(int mode)
{
    struct CursorPadKeyResult *keys;
    int plane;
    int selected;
    StudioCamera *camera;
    HomogeneousVector *position;
    Vector4 *camera_position;
    StudioCamera *reset_camera;
    char text[0x100];
    const char *position_format;
    const char *camera_format;

    keys = &keyData;
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
    if ((keys->held & (PAD_CIRCLE | PAD_SQUARE)) != 0) {
        if (cursorMode < 0) {
            cursorMode = cursor.mode;
        }
        if (keys->pressed & PAD_UP) {
            cursorMode += 1;
        }
        if (keys->pressed & PAD_DOWN) {
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
            cursor.mode = cursorMode;
            changeCameraMode();
            cursorMode = -1;
        }
        if (keys->pressed & PAD_UP) {
            cursor.step += 1;
        }
        if (keys->pressed & PAD_DOWN) {
            cursor.step -= 1;
        }
        cursor.step = cursor.step & 7;
    }
    position = &cursor.position;
    xglStudioGetCamera(&camera, 0);
    if ((keys->pressed & PAD_L3) != 0) {
        position->x = 0.0f;
        position->y = 0.0f;
        position->z = 0.0f;
    }
    if (keys->pressed & PAD_R3) {
        reset_camera = camera;
        (&reset_camera->position.vector)->y = 0.0f;
        (&reset_camera->position.vector)->z = 4.0f;
        (&reset_camera->position.vector)->x = 0.0f;
    }
    if (keys->pressed & PAD_R1) {
        cursor.flag_04 -= 1;
    }
    if (keys->pressed & PAD_R2) {
        cursor.flag_04 += 1;
    }
    plane = cursor.flag_04;
    if (plane < 0) {
        cursor.flag_04 = 2;
        plane = 2;
    }
    if (!(plane < 3)) {
        cursor.flag_04 = 0;
    }
    if (cursor.mode != 4) {
        switch (cursor.mode) {
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
            camera_position = &camera->position.vector;
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
        if (cursor.callback != 0) {
            cursor.callback(cursor.callback_argument);
        }
    }
    selected = cursorMode;
    if (selected < 0) {
        selected = cursor.mode;
    }
    if (toolFlags == 0) {
        xglFontDebugPrintf(4, 0xB4, cursorModeSpeedFormat, cursorModeName[selected], cursor.flag_04,
                           cursor.step + 1);
    }
}

void drawCursor(void)
{
    Matrix4 matrix;
    HomogeneousVector *position;

    position = &cursor.position;
    position->w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&position->x);
    xglMatrixStackSave(matrix);
    drawAxis(matrix, 1.0f);
}

/*
 * Resets the cameras (camera 0 active), points the tcamera view at the cursor and
 * puts the cursor at the origin in mode 0.
 */
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

    cursor_position = &cursor.position;
    tcamera.mode1 = 20;
    tcamera.view_constraint = cursor_position;
    state = &cursor;
    tcamera.view_offset[0] = 0;
    tcamera.view_offset[1] = 0;
    tcamera.view_offset[2] = 0;
    tcamera.view_mode = 1;
    state->step = 3;
    cursorMode = -1;
    state->position.x = 0.0f;
    state->position.y = 0.0f;
    state->position.z = 0.0f;
    state->mode = CURSOR_MODE_0;
    state->submode = 2;
    changeCameraMode();
}

/* Prints a 4x4 matrix as four text rows starting at (x, y), 8 pixels apart. */
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

/*
 * Debug view of the map model's parts. Up and Down step the selected part index,
 * which is shown with its matrix; the selected part's visibility toggles every 16 calls.
 */
void MAP_serach(void)
{
    MapStageModelView *model;
    MapPartListView *parts;
    DbLightWritePad *mapPad;
    unsigned char *matrix_base;
    void *matrix_data;
    char buffer[256];

    model = GameLoopState.model;
    mapPad = &PadData[1];
    visible_0++;
    if (model != 0) {
        parts = model->parts;
        if (parts != 0) {
            matrix_base = (unsigned char *)parts +
                          parts->matrix_data_offset;
            if (mapPad->repeat & PAD_DOWN) {
                nmlModelSetPartsVisible(parts, partsIndex, 1);
                visible_0 = 0;
                partsIndex--;
            }

            if (mapPad->repeat & PAD_UP) {
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

/*
 * Per-frame debug update. Triangle with Select toggles mode_004DC5A8; in mode 1
 * the game update runs too. The cursor is then updated and drawn.
 */
void EvtTools(void)
{
    if ((PadData[0].pressed & PAD_TRIANGLE) && (PadData[0].held & PAD_SELECT)) {
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

/*
 * Loads the light table from the studio light: entries 0 to 2 from the colours of
 * light 0, entry 3 from the ambient colour and entry 4 from the clear colour.
 * Directions in lightDir come from light 0.
 */
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
    slot = &lightCol[3].vector;
    slot->x = l->ambient_color.x;
    slot->y = l->ambient_color.y;
    slot->z = l->ambient_color.z;
    sum = slot->x + slot->y + slot->z;
    slot->w = sum / 3.0f;
    colour = &lightCol[0].vector;
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
    slot = &lightCol[4].vector;
    slot->x = (float)ClearEnv.color_r / 255.0f;
    slot->y = (float)ClearEnv.color_g / 255.0f;
    slot->z = (float)ClearEnv.color_b / 255.0f;
    slot->w = (slot->x + slot->y + slot->z) / 3.0f;
    slot = &lightCol[5].vector;
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

/*
 * Loads the same light table as initLight3, reading the colours and directions
 * of lights 0 to 2 in turn.
 */
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
    destination = &lightCol[3].vector;
    destination->x = source->ambient_color.x;
    destination->y = source->ambient_color.y;
    destination->z = source->ambient_color.z;
    destination->w = (destination->x + destination->y + destination->z) / 3.0f;

    for (i = 0; i < 3; i++) {
        float lred;
        float lgreen;
        float lblue;

        float *walk;

        destination = &lightCol[i].vector;
        walk = &source->lights[i].color.z;
        lred = walk[-2];
        destination->x = lred;
        lgreen = walk[-1];
        destination->y = lgreen;
        lblue = walk[0];
        destination->w = (lred + lgreen + lblue) / 3.0f;
        destination->z = lblue;
    }

    destination = &lightCol[4].vector;
    destination->x = (float)ClearEnv.color_r / 255.0f;
    destination->y = (float)ClearEnv.color_g / 255.0f;
    destination->z = (float)ClearEnv.color_b / 255.0f;
    destination->w = (destination->x + destination->y + destination->z) / 3.0f;

    destination = &lightCol[5].vector;
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

/*
 * Normalises the three directions in lightDir, derives their z from the unit
 * length, and sends the light colours and directions to the studio light.
 */
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

        xglLightIntensityParallel(light, lightIndex, &lightCol[lightIndex].vector);
        xglLightDirection(light, lightIndex, &direction);
    } while (++lightIndex < 3);

    xglLightIntensityAmbient(light, &lightCol[3].vector);
}

/*
 * Light editor for the debug view. Selects one of six lights or colour entries
 * (lightIndex 0 to 5) and a colour channel, edits the colour, and aims the
 * selected light's direction. The result goes to the studio light, the clear
 * colour or the stage colour. The light table and direction widget are drawn
 * every call.
 */
void updateLight(int mode)
{
    const struct CursorPadKeyResult *keys = &keyData;
    Matrix4 matrix;
    Matrix4 cameraMatrix;
    Vector4 stick;
    AlignedVector4 direction;
    Vector4 barScale;
    AlignedVector4 barPosition;
    Vector4 translation;
    char text[256];
    Vector3 flat;
    char dumpText[128];
    float stickLength;
    StudioLight *light;
    StudioLight *dumpLight;
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
        PAD_getKey(&keyData, 0, 0xf7ff);
        break;
    case 1:
        PAD_getKey(&keyData, 0, 0xf6ff);
        break;
    }

    /* Without Circle or Square, the pad selects a light and edits its colour. */
    if ((keys->held & (PAD_CIRCLE | PAD_SQUARE)) == 0) {
        float limit;

        limit = 1.0f;

        if ((keys->pressed & PAD_R2) != 0) {
            lightColIndex = 3;
            lightIndex++;
        }
        if ((keys->pressed & PAD_R1) != 0) {
            lightColIndex = 3;
            lightIndex--;
        }
        if (lightIndex < 0)
            lightIndex = 5;
        lightIndex = lightIndex >= 6 ? 0 : lightIndex;
        if (keys->pressed & PAD_UP)
            lightColIndex -= 1;
        if (keys->pressed & PAD_DOWN)
            lightColIndex++;
        if (lightColIndex < 0)
            lightColIndex = 3;
        if (lightColIndex >= 4)
            lightColIndex = 0;
        /*
         * Right and Left add or subtract 0.01 to the selected channel, clamped to
         * 0 to limit. Channel 3 edits red, green and blue together. Component 3
         * always holds their average.
         */
        component = lightCol[lightIndex].components + lightColIndex;
        if (lightIndex == 5)
            limit = 10.0f;
        if (lightColIndex == 3) {
            float total = 0.0f;

            for (i = 0; i < 3; i++) {
                float *value = lightCol[lightIndex].components + i;

                if (keys->held & PAD_RIGHT)
                    *value += 0.01f;
                if (keys->held & PAD_LEFT)
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
            lightCol[lightIndex].components[3] = total / 3.0f;
        } else {
            float total;

            if (keys->held & PAD_RIGHT)
                *component += 0.01f;
            if (keys->held & PAD_LEFT)
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
                total += lightCol[lightIndex].components[i];
            lightCol[lightIndex].components[3] = total / 3.0f;
        }
    } else if (keys->held & PAD_CIRCLE) {
        /* Circle: R1 toggles the panel offset, Up and Down change the light speed. */
        if (keys->pressed & PAD_R1)
            if (lightStateOffset != 0)
                lightStateOffset = 0;
            else
                lightStateOffset = -380;
        if (keys->pressed & PAD_DOWN)
            lightSpeed -= 1;
        if (keys->pressed & PAD_UP)
            lightSpeed += 1;
        lightSpeed &= 3;
    }

    /* keys->flags[2] and flags[3] turn the selected light direction; L3 flips its y sign. */
    source = &lightDir[lightIndex];
    if ((keys->pressed & PAD_L3) != 0)
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
    direction.x = source->x;
    direction.y = __builtin_sqrtf(horizontal);
    direction.z = source->z;
    if (source->y < 0.0f)
        direction.y = -direction.y;
    xglVectorLength(&stickLength, &direction);

    /* Draws the light direction through the camera matrix translated 5 units along z, with its rotation reset to identity. */
    xglStudioFlushActiveCamera();
    camera = xglStudioGetActiveCamera();
    translation.x = 0.0f;
    translation.y = 0.0f;
    translation.z = 5.0f;
    translation.w = 1.0f;
    QuadwordCopyMatrix(cameraMatrix, camera->cameraToWorldMatrix);
    xglMatrixStackUnit();
    xglMatrixStackMul(cameraMatrix);
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
    drawVector4((const Vector3 *)&direction, &cameraMatrix, 0);

    stick.x = lightCol[lightIndex].components[0];
    stick.y = lightCol[lightIndex].components[1];
    stick.z = lightCol[lightIndex].components[2];
    stick.w = 1.0f;
    /* Lights 0 to 2 and ambient go to the studio light; 4 and 5 are the clear and stage colours. */
    switch (lightIndex) {
    case 0:
    case 1:
    case 2:
        xglStudioGetLight(&light);
        xglLightIntensityParallel(light, lightIndex, &stick);
        xglLightDirection(light, lightIndex, &direction);
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
        GameLoopState.stage_color.x = lightCol[5].components[0];
        GameLoopState.stage_color.y = lightCol[5].components[1];
        GameLoopState.stage_color.z = lightCol[5].components[2];
        break;
    }

    baseX = 192 + lightStateOffset / 2;
    sprintf(text, lightSpeedFormat, 1 + lightSpeed);
    xglFontDebugPrintf(baseX, 160, text);
    sprintf(text, rgbColorFormat, stick.x, stick.y, stick.z);
    xglFontDebugPrintf(baseX, 170, text);
    sprintf(text, directionVectorFormat, direction.x, direction.y, direction.z);
    xglFontDebugPrintf(baseX, 198, text);

    baseX = 200 + lightStateOffset / 2;
    xglFontDebugPrintf(baseX, 88, ambientText);
    xglFontDebugPrintf(baseX + 4, 114, backgroundText);
    xglFontDebugPrintf(baseX, 140, mapText);
    stick.x = 116.0f;
    stick.y = 56.0f;
    stick.z = 1.0f;
    stick.w = 1.0f;
    direction.x = (float)lightStateOffset + 196.0f;
    direction.y = 192.0f - (float)lightIndex * 51.0f;
    direction.z = 0.0f;
    direction.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&direction.x);
    xglMatrixStackScale(&stick.x);
    xglMatrixStackSave(matrix);
    drawBox2(&matrix, 6);

    stick.x = 6.0f;
    stick.y = 6.0f;
    stick.z = 1.0f;
    stick.w = 1.0f;
    direction.x = (float)lightStateOffset + 196.0f;
    direction.y = 192.0f - (float)lightIndex * 51.0f + 12.0f -
                  (float)lightColIndex * 12.0f;
    direction.z = 0.0f;
    direction.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&direction.x);
    xglMatrixStackRotZ(3.14159265f / 4.0f);
    xglMatrixStackScale(&stick.x);
    xglMatrixStackSave(matrix);
    drawBox2(&matrix, 6);

    stick.x = 24.0f;
    stick.y = 24.0f;
    stick.z = 1.0f;
    stick.w = 1.0f;
    direction.y = 192.0f;
    direction.x = (float)lightStateOffset + 166.0f;
    direction.z = 0.0f;
    direction.w = 1.0f;
    /* Draws the light table: a circle and direction arrow per light, and a bar per channel. */
    for (k = 0; k < 6; k++) {
        float scale;

        source = &lightDir[k];
        xglMatrixStackUnit();
        xglMatrixStackTrans(&direction.x);
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
        barPosition.x += 36.0f;
        barPosition.y += 16.0f;
        barScale.z = 1.0f;
        barScale.w = 1.0f;
        barScale.y = 8.0f;
        scale = 1.0f;
        for (j = 0; j < 4; j++) {
            barScale.x = lightCol[k].components[j] * 48.0f * scale;
            xglMatrixStackUnit();
            xglMatrixStackTrans(&barPosition.x);
            xglMatrixStackScale(&barScale.x);
            xglMatrixStackSave(matrix);
            if (j < 3)
                drawRect2(&matrix, j);
            else
                drawRect2(&matrix, 6);
            barPosition.y -= 12.0f;
        }
        direction.y -= 51.0f;
    }

    /* Formats the light setup into dumpText; the text is not output here. */
    if (keys->pressed & PAD_SELECT) {
        MapGameLoopStateView *state;

        i = 0;
        xglStudioGetLight(&dumpLight);
        sprintf(dumpText, lightSetColorFormat, i, dumpLight->ambient_color_row[0],
                dumpLight->ambient_color_row[1], dumpLight->ambient_color_row[2]);
        state = &GameLoopState;
        do {
            sprintf(dumpText, lightSetColorFormat, i + 1, dumpLight->lights[i].color_row[0],
                    dumpLight->lights[i].color_row[1],
                    dumpLight->lights[i].color_row[2]);
            sprintf(dumpText, lightSetDirection2Format, i + 1, dumpLight->lights[i].direction_row[0],
                    dumpLight->lights[i].direction_row[1],
                    dumpLight->lights[i].direction_row[2]);
            sprintf(dumpText, stageSetColorFormat, state->stage_color.x,
                    state->stage_color.y,
                    state->stage_color.z);
        } while (++i < 3);
    }
}

/*
 * Wind debug editor. Up and Down move the selection through the wind rows.
 * Circle applies the mode on rows 0 to 2, rows 3 and 4 adjust the shake offsets,
 * and the pad stick moves the wind's x and z. Triangle resets the wind and keeps
 * its mode. The settings text is drawn every call.
 */
void updateWind(WindRecord *wind)
{
    StudioCamera *camera;
    DbLightWritePad *pad;
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

    pad = &PadData[1];
    camera = xglStudioGetActiveCamera();
    buttons = pad->repeat;
    if (buttons & PAD_UP) {
        selection = csr_1;
        selection += 4;
        selection %= 5;
        csr_1 = selection;
    }
    if (buttons & PAD_DOWN) {
        selection = csr_1;
        selection += 1;
        selection %= 5;
        csr_1 = selection;
    }
    selection = csr_1;
    if ((pad->pressed & PAD_CIRCLE) && selection < 3) {
        wind->mode = csr_1;
        selection = csr_1;
    }
    stick = pad->held;
    buttons = stick;
    step = (buttons & PAD_SQUARE) ? 0.001f : 0.01f;

    if (selection == 3) {
        if (buttons & PAD_LEFT) {
            wind->offset_a -= step;
        }
        if (buttons & PAD_RIGHT) {
            wind->offset_a += step;
        }
    }
    if (selection == 4) {
        float offset_clamp;

        if (buttons & PAD_LEFT) {
            wind->offset_b -= step;
        }
        if (buttons & PAD_RIGHT) {
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

    if (pad->pressed & PAD_TRIANGLE) {
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
    /* Fills the marker for directional winds; this function does not draw it. */
    if (wind->mode == 1) {
        marker.from = camera->chasePosition;
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
    /* Formats the wind command into text_wide; the text is not output here. */
    if (pad->pressed & PAD_SELECT) {
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
    CursorState *state = &cursor;

    state->mode = mode;
    changeCameraMode();
}

void VW_setCursorFunc(CursorCallback callback, void *argument)
{
    CursorState *state = &cursor;

    state->callback = callback;
    state->callback_argument = argument;
}

void VW_setCursor(const Vector4 *position)
{
    cursor.position.x = position->x;
    cursor.position.y = position->y;
    cursor.position.z = position->z;
}

void VW_getCursor(HomogeneousVector *destination)
{
    destination->x = cursor.position.x;
    destination->y = cursor.position.y;
    destination->z = cursor.position.z;
    destination->w = 1.0f;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
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
