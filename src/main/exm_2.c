#include "common.h"

#include "shared.h"

typedef struct EXM_WindState EXM_WindState;

static EXM_WindState _wind;

static EXM_WindState *wind = &_wind;

static float wave = 0.0f;

static float waverad = 0.0f;

/* The +0x10 vector fields are named from the allocated reset and setter stores.
 * The u64 at +0x08 preserves the 8-byte alignment of the complete wind state. */

void EXM_ResetWind(void);

/*
 * EXM_GetWindPower/EXM_ResetWind/EXM_SetDirectionalWind/EXM_SetPointWind/
 * EXM_StepShakeWind (this unit, main/tu268) also read or write this struct:
 * +0x00 is the mode EXM_StopWind and EXM_ResetWind clear to 0,
 * EXM_SetDirectionalWind sets to 1 and EXM_SetPointWind sets to 2; +0x10 is a
 * direction/point vector (four floats: EXM_ResetWind zeroes +0x10/+0x14/+0x18/
 * +0x1c individually) that EXM_Set{Directional,Point}Wind load with a 16-byte
 * copy. None of those bytes are written by a function claimed here, so they
 * stay modeled only for their evidenced size and 8-byte alignment (`u64`,
 * not the attribute-qualified float type the ordinary-C admission refuses):
 * EXM_SetWindPara/EXM_GetWindPara move the whole 0x30-byte block in six ld/sd
 * pairs, which needs that alignment -- a struct whose largest member is only
 * 4-byte aligned instead compiles the same `*wind = *para;` assignment to
 * ldl/ldr/sdl/sdr.
 */

typedef union EXM_WindVector {
    Vector4 vector;
    unsigned long long words[2];
} EXM_WindVector;

/* The 64 unitSequence entries have a 0x260-byte stride in the named callers. */

typedef struct UnitSequenceEntry {
    unsigned char unmodeled_00[0x04];
    int state;
    unsigned char unmodeled_08[0x240 - 0x08];
    float pivot_x;
    float pivot_y;
    float pivot_z;
    unsigned char unmodeled_24c[0x250 - 0x24c];
    float axis_x;
    float axis_y;
    float axis_z;
    float axis_w;
} UnitSequenceEntry;

struct EXM_WindState {
    char mode;              /* +0x00, read signed (lb) by EXM_GetWindPower */
    u8 unmodeled_01[7];
    u64 unmodeled_08;
    EXM_WindVector direction; /* +0x10, four-component direction/position */
    float shake_power;      /* +0x20, EXM_SetShakePower */
    float shake_time;       /* +0x24, EXM_SetShakeTime */
    float shake_rad;        /* +0x28, EXM_SetShakeWind/EXM_GetShakeRad */
    u8 unmodeled_2c[4];
};

static EXM_WindState _wind = { 0 };

/*
 * MapUnit is a 64-entry array with a 0x300-byte stride.  These are additive
 * record views from existing callers: map_2 names the update/render callback,
 * resource and model-state fields; map_create_unit_peer names transforms and
 * the unit sequence byte; init_drill names the cleanup value; and the door
 * TU names its position vector.  A union keeps those independently evidenced
 * views without asserting that same-offset fields have one shared meaning.
 */

typedef struct MapUnitDrawView {
    unsigned int flags; /* +0x00 */
    void (*update)(void *unit); /* +0x04 */
    unsigned char unmodeled_08[4];
    void (*type_update)(void *unit); /* +0x0c */
    unsigned char unmodeled_10[0x94];
    short serial; /* +0xa4 */
    unsigned char unmodeled_a6[0x2e];
    void *resource_model; /* +0xd4 */
    unsigned char unmodeled_d8[8];
    void *model; /* +0xe0 */
    int resource_status; /* +0xe4 */
    unsigned char unmodeled_e8[0x158];
    void *model_state; /* +0x240 */
    unsigned char unmodeled_244[0xbc];
} MapUnitDrawView;

typedef struct MapUnitTransformView {
    unsigned int flags; /* +0x00 */
    unsigned char unmodeled_04[0x0c];
    Vector4 position; /* +0x10 */
    Vector4 rotation; /* +0x20 */
    Vector4 scale; /* +0x30 */
    Matrix4 matrix; /* +0x40 */
    unsigned char unmodeled_80[0x20];
    unsigned char serial; /* +0xa0 */
    unsigned char signal; /* +0xa1 */
    unsigned char unmodeled_a2[0x1a2 - 0xa2];
    unsigned short drill_reset_value; /* +0x1a2 */
    unsigned char unmodeled_1a4[0x15c];
} MapUnitTransformView;

typedef struct MapUnitDoorView {
    unsigned char unmodeled_00[0xc0];
    Vector4 position; /* +0xc0 */
    unsigned char unmodeled_d0[0x230];
} MapUnitDoorView;

typedef struct MapUnitRecord {
    union {
        MapUnitDrawView draw;
        MapUnitTransformView transform;
        MapUnitDoorView door;
    };
} MapUnitRecord;

MapUnitRecord MapUnit[64] = { 0 };

UnitSequenceEntry unitSequence[64] = { 0 };

/* D_004D8774/D_004D8778 are the two clamp values, +/- 2*pi. */

#define D_004D8774 6.2831855f

#define D_004D8778 -6.2831855f

typedef union EXM_WindVectorStorage {
    Vector4 vector;
    u64 pair[2];
} EXM_WindVectorStorage;


typedef struct EXM_WindFrame {
    unsigned char unmodeled_00[0x54];
    float angle; /* +0x54, read by EXM_GetWindPower */
} EXM_WindFrame;

typedef struct EXM_WindUnit {
    unsigned char unmodeled_000[0x720];
    float axis_cos; /* +0x720 */
    unsigned char unmodeled_724[0x1c];
    float axis_sin; /* +0x740 */
    unsigned char unmodeled_744[0x0c];
    float position[3]; /* +0x750 */
    unsigned char unmodeled_75c[0x820 - 0x75c];
    EXM_WindFrame *frame; /* +0x820 */
} EXM_WindUnit;

float xglSin(float angle);

void xglVectorNormal(Vector4 *dest, const Vector4 *src);

void MATRIX_identity4s(Matrix4 destination);

void MATRIX_rotY4s(Matrix4 matrix, float angle);

void MATRIX_translate4s(Matrix4 matrix, float x, float y, float z);

void EXM_GetWindPower(float *result, const EXM_WindUnit *unit, const float *position)
{
    EXM_WindVector force;
    Vector4 *force_vector;
    Matrix4 matrix;
    EXM_WindFrame *frame;
    float power;
    float scale;
    float axis_cos;
    float axis_sin;
    float x;
    float z;

    if (wind->mode == 0) {
        result[0] = position[0];
    } else {
        wave += 1.3f;
        power = xglSin(wind->shake_rad - wave) * wave;
        axis_cos = unit->axis_cos;
        axis_sin = unit->axis_sin;
        switch (wind->mode) {
        case 1:
            scale = wind->direction.vector.w * (power * wind->shake_power + 1.0f);
            force = wind->direction;
            force.vector.x *= scale;
            force.vector.y *= scale;
            force.vector.z *= scale;
            force_vector = &force.vector;
            break;
        case 2:
            force.vector.x = unit->position[0] + position[0] * axis_cos + position[2] * axis_sin - wind->direction.vector.x;
            force.vector.y = unit->position[1] + position[1] - wind->direction.vector.y;
            force.vector.z = unit->position[2] + position[2] * axis_cos - position[0] * axis_sin - wind->direction.vector.z;
            xglVectorNormal(&force.vector, &force.vector);
            scale = wind->direction.vector.w * (power * wind->shake_power + 1.0f);
            force.vector.x *= scale;
            force.vector.y *= scale;
            force.vector.z *= scale;
            force_vector = &force.vector;
            break;
        default:
            result[0] = position[0];
            return;
        }
        x = force_vector->x * axis_cos - force_vector->z * axis_sin;
        z = force_vector->z * axis_cos + force_vector->x * axis_sin;
        frame = unit->frame;
        if (frame != 0) {
            MATRIX_identity4s(matrix);
            MATRIX_translate4s(matrix, x, 0.0f, z);
            MATRIX_rotY4s(matrix, -frame->angle);
            x = matrix[3][0];
            z = matrix[3][2];
        }
        result[0] = position[0] + x;
        result[1] = position[1] + force_vector->y;
        result[2] = position[2] + z;
    }
}

void EXM_ResetWind(void)
{
    float reset_value;
    wind->mode = 0;
    reset_value = 0.0f;
    wind->direction.vector.w = reset_value;
    wind->direction.vector.z = reset_value;
    wind->direction.vector.y = reset_value;
    wind->direction.vector.x = reset_value;
    wind->shake_power = reset_value;
    wind->shake_time = reset_value;
    wind->shake_rad = reset_value;
}

void EXM_SetWindPara(EXM_WindState *para)
{
    if (para != 0) {
        *wind = *para;
    }
}

void EXM_GetWindPara(EXM_WindState *para)
{
    if (para != 0) {
        *para = *wind;
    }
}

void EXM_InitWind(void)
{
    wind = &_wind;
    EXM_ResetWind();
}

void EXM_StopWind(void)
{
    wind->mode = 0;
    wind->shake_rad = 0.0f;
}

void EXM_SetShakePower(float power)
{
    wind->shake_power = power;
}

void EXM_SetShakeTime(float time)
{
    if (time > D_004D8774) {
        time = D_004D8774;
    } else if (time < D_004D8778) {
        time = D_004D8778;
    }
    wind->shake_time = time;
}

void EXM_SetDirectionalWind(Vector4 *direction)
{
    EXM_WindState *state;

    if (direction != 0) {
        state = wind;
        state->direction.vector = *direction;
    } else {
        state = wind;
    }
    state->mode = 1;
}

void EXM_SetPointWind(Vector4 *point)
{
    EXM_WindState *state;

    if (point != 0) {
        state = wind;
        state->direction.vector = *point;
    } else {
        state = wind;
    }
    state->mode = 2;
}

void EXM_SetWindStruct(EXM_WindState *state)
{
    wind = state;
}

void EXM_ClearWindStruct(void)
{
    wind = &_wind;
}

void EXM_StepShakeWind(void)
{
    if (wind != 0) {
        wind->shake_rad += wind->shake_time;
        if (wind->shake_rad > D_004D8774) {
            wind->shake_rad -= D_004D8774;
        } else if (wind->shake_rad < 0.0f) {
            wind->shake_rad += D_004D8774;
        }
    }
    waverad += 0.5f;
    if (waverad > D_004D8774) {
        waverad -= D_004D8774;
    } else if (waverad < 0.0f) {
        waverad += D_004D8774;
    }
}

void EXM_SetShakeWind(float rad)
{
    wind->shake_rad = rad;
}

void EXM_ResetWave(void)
{
    wave = 0.0f;
}

float EXM_GetWaveRad(void)
{
    return waverad;
}

float EXM_GetShakeRad(void)
{
    return wind->shake_rad;
}
