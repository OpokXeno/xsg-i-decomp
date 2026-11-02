#include "common.h"
#include "shared.h"
#include "main/exm_2.h"
#include "main/matrix_print.h"
#include "main/xgl_2.h"

/* SHAKE_ANGLE_MAX/SHAKE_ANGLE_MIN are the two clamp values, +/- 2*pi. */
#define SHAKE_ANGLE_MAX 6.2831855f

#define SHAKE_ANGLE_MIN -6.2831855f

/* EXM_WindState.mode values. */
#define EXM_WIND_OFF 0
#define EXM_WIND_DIRECTIONAL 1 /* direction.xyz is the force direction */
#define EXM_WIND_POINT 2 /* direction.xyz is the source point */

#define EXM_WIND_WAVE_STEP 1.3f
#define EXM_WAVE_RAD_STEP 0.5f

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

void MATRIX_identity4s(Matrix4 destination);
static EXM_WindState _wind;

static EXM_WindState *wind = &_wind;

static float wave = 0.0f;

static float waverad = 0.0f;

static EXM_WindState _wind = { 0 };

MapUnitRecord MapUnit[MAP_UNIT_COUNT] = { 0 };

UnitSequenceEntry unitSequence[MAP_UNIT_COUNT] = { 0 };

void EXM_GetWindPower(float *result, const EXM_WindUnit *unit, const float *position)
{
    DoublewordAlignedVector4 force;
    Vector4 *force_vector;
    Matrix4 matrix;
    EXM_WindFrame *frame;
    float power;
    float scale;
    float axis_cos;
    float axis_sin;
    float x;
    float z;

    if (wind->mode == EXM_WIND_OFF) {
        result[0] = position[0];
    } else {
        wave += EXM_WIND_WAVE_STEP;
        power = xglSin(wind->shake_rad - wave) * wave;
        axis_cos = unit->axis_cos;
        axis_sin = unit->axis_sin;
        switch (wind->mode) {
        case EXM_WIND_DIRECTIONAL:
            scale = wind->direction.w * (power * wind->shake_power + 1.0f);
            force = wind->direction;
            force.x *= scale;
            force.y *= scale;
            force.z *= scale;
            force_vector = &force;
            break;
        case EXM_WIND_POINT:
            force.x = unit->position[0] + position[0] * axis_cos + position[2] * axis_sin - wind->direction.x;
            force.y = unit->position[1] + position[1] - wind->direction.y;
            force.z = unit->position[2] + position[2] * axis_cos - position[0] * axis_sin - wind->direction.z;
            xglVectorNormal(&force, &force);
            scale = wind->direction.w * (power * wind->shake_power + 1.0f);
            force.x *= scale;
            force.y *= scale;
            force.z *= scale;
            force_vector = &force;
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
    wind->mode = EXM_WIND_OFF;
    reset_value = 0.0f;
    wind->direction.w = reset_value;
    wind->direction.z = reset_value;
    wind->direction.y = reset_value;
    wind->direction.x = reset_value;
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
    wind->mode = EXM_WIND_OFF;
    wind->shake_rad = 0.0f;
}

void EXM_SetShakePower(float power)
{
    wind->shake_power = power;
}

void EXM_SetShakeTime(float time)
{
    if (time > SHAKE_ANGLE_MAX) {
        time = SHAKE_ANGLE_MAX;
    } else if (time < SHAKE_ANGLE_MIN) {
        time = SHAKE_ANGLE_MIN;
    }
    wind->shake_time = time;
}

void EXM_SetDirectionalWind(Vector4 *direction)
{
    EXM_WindState *state;

    if (direction != 0) {
        state = wind;
        state->direction = *direction;
    } else {
        state = wind;
    }
    state->mode = EXM_WIND_DIRECTIONAL;
}

void EXM_SetPointWind(Vector4 *point)
{
    EXM_WindState *state;

    if (point != 0) {
        state = wind;
        state->direction = *point;
    } else {
        state = wind;
    }
    state->mode = EXM_WIND_POINT;
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
        if (wind->shake_rad > SHAKE_ANGLE_MAX) {
            wind->shake_rad -= SHAKE_ANGLE_MAX;
        } else if (wind->shake_rad < 0.0f) {
            wind->shake_rad += SHAKE_ANGLE_MAX;
        }
    }
    waverad += EXM_WAVE_RAD_STEP;
    if (waverad > SHAKE_ANGLE_MAX) {
        waverad -= SHAKE_ANGLE_MAX;
    } else if (waverad < 0.0f) {
        waverad += SHAKE_ANGLE_MAX;
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
