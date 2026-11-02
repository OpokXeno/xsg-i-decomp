/*
 * TU-local declarations of main/tu263 (src/main/look.c).
 */

#ifndef SRC_MAIN_LOOK_H
#define SRC_MAIN_LOOK_H

#include "shared.h"
#include "main/look.h"
#include "main/matrix_print.h"
#include "main/xgl_2.h"

/* Head bounds, position adjustment and smoothing steps. */
#define LOOK_HEAD_MIN_WEIGHT 0.0001f

#define LOOK_HEAD_POSITION_Y_OFFSET 0.15f
#define LOOK_HEAD_Z_MAX_ANGLE 1.0471975803375244f
#define LOOK_HEAD_Z_MIN_ANGLE (-1.0471975803375244f)
#define LOOK_HEAD_Y_MAX_ANGLE 1.483529806137085f
#define LOOK_HEAD_Y_MIN_ANGLE (-1.483529806137085f)
#define LOOK_HEAD_Y_SCALE (-0.7f)
#define LOOK_HEAD_Z_SCALE 0.7f
#define LOOK_HEAD_Y_STEP 0.1745329350233078f
#define LOOK_HEAD_Z_STEP 0.0872664675116539f
#define LOOK_HEAD_Y_LIMIT 0.1745329350233078f
#define LOOK_HEAD_Z_LIMIT 0.0872664675116539f
#define LOOK_SEBO_MIN_WEIGHT 0.0001f
#define LOOK_SEBO_LIMIT 0.13962633907794952f
#define LOOK_SEBO_STEP 0.13962633907794952f
#define LOOK_SEBO_ROTATION_SCALE 0.2f

/* MARK provisional: original .lit4 external witnesses, no original symbols. */
#define xglAsin_cubic (-0.01872929931f)

/* MARK provisional: .lit4 0x004d8554 */
#define xglAsin_quadratic 0.07426100224f

/* MARK provisional: .lit4 0x004d8558 */
#define xglAsin_linear 0.2121143937f

/* MARK provisional: .lit4 0x004d855c */
#define xglAsin_pi_over_two_approx 1.570728779f

/* MARK provisional: .lit4 0x004d8560 */
#define xglAsin_pi_over_two 1.570796371f

/* MARK provisional: .lit4 0x004d85a8 */
#define LOOK_EYE_L_OFFSET 0.03999999911f

/* MARK provisional: .lit4 0x004d85ac */
#define LOOK_EYE_R_OFFSET (-0.03999999911f)

/* Eye scale, step and rotation constants. */
#define LOOK_EYE_IDLE_SCALE 0.4f

#define LOOK_EYE_TRACK_STEP 0.005235988f
#define LOOK_EYE_STEP 0.13962634f
#define LOOK_EYE_LIMIT 0.13962634f
#define LOOK_EYE_ROTATION_SCALE 0.3f
#define LOOK_EYE_YAW_ROTATION_SCALE 0.6f

/* Articulation elements have a 64-byte stride and three Euler rotation slots. */
typedef struct LookJointElement {
    float scale[3];
    unsigned char unmodeled_0c[4];
    float rotation[3];
    unsigned char unmodeled_1c[4];
    Vector4 translation;
    unsigned char unmodeled_30[0x40 - 0x30];
} LookJointElement;

typedef struct LookJointWork {
    unsigned char unmodeled_000[0x2c];
    int jointIndex;
    unsigned char unmodeled_030[0x720 - 0x30];
    Matrix4 rootMatrix;
    unsigned char unmodeled_760[0x7a0 - 0x760];
    Matrix4 *matrixBuffer;
    unsigned char unmodeled_7a4[0x818 - 0x7a4];
    LookJointElement *elements;
} LookJointWork;

typedef struct LookEyeState {
    int frame;
    int timer;
    float yaw;
    float pitch;
} LookEyeState;

typedef struct LookCharacter {
    unsigned char unmodeled_000[0x70];
    Vector4 nearPosition;
    unsigned char number;
    unsigned char unmodeled_081[2];
    unsigned char headPositionValid;
    unsigned char unmodeled_084[0x90 - 0x84];
    Vector4 eyePosition;
    unsigned char unmodeled_0a0[0x600 - 0xa0];
    /*
     * Look angle rows: [n][0] and [n][3] are eye n's yaw and pitch
     * (LOOK_eye_cont), [0][1] the sebo angle, [1][1]/[1][2] the head Y/Z
     * angles. A float row array, not Vector4: LOOK_eye_cont's [side] stores
     * keep the 0x600 in the displacement.
     */
    float lookAngles[2][4];
    Vector4 worldPosition;
    Vector4 headPosition;
    /*
     * Eye states; LOOK_eye_cont reads each eye's yaw/pitch back as row
     * elements [side][2]/[side][3] of the same storage.
     */
    union {
        LookEyeState eyes[2];
        float eyeAngles[2][4];
    };
    float eyeYaw;
    float eyePitch;
    unsigned char unmodeled_668[4];
    float lookWeight;
    unsigned char unmodeled_670[4];
    unsigned short lookMode;
    unsigned char unmodeled_676[2];
    unsigned int eyeMode;
    struct LookCharacter *lookTarget;
    unsigned char unmodeled_680[0x8d0 - 0x680];
    struct LookModelRecord *model;
    unsigned char unmodeled_8d4[0x8fc - 0x8d4];
    struct LookCharacter *eyeOwner;
    unsigned char unmodeled_900[0x9a0 - 0x900];
    unsigned int headFlags;
} LookCharacter;

typedef struct LookModelRecord {
    unsigned char unmodeled_00[0x11];
    char name[0x48 - 0x11];
    int detail;
} LookModelRecord;

/* Four-float vector with 8-byte alignment: the double member gives the
   alignment that the SDK quadword copies assume, without a GNU attribute. */
typedef union LookAlignedVector4 {
    double alignment;
    Vector4 v;
} LookAlignedVector4;

void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);
void xglVectorMulMat(Vector4 *destination, Matrix4 matrix, const Vector4 *source);
void xglMatrixRotX(Matrix4 destination, Matrix4 source, float angle);
void xglMatrixRotY(Matrix4 destination, Matrix4 source, float angle);
float xglAsin(float value);
void look_limit(float *angle, float limit);
void look_limit_add(float *angle, float target, float step);
#endif /* SRC_MAIN_LOOK_H */
