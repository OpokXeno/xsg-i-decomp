/* exm_1.h: TU-local declarations of main/tu267 (src/main/exm_1.c).
 */
#ifndef SRC_MAIN_EXM_1_H
#define SRC_MAIN_EXM_1_H

#include "main/xgl_2.h"
#include "main/jnt.h"
#include "main/act_1.h"
#include "shared.h"
#include "main/matrix_print.h"

/*
 * The actor record is Actor (main/act_1.h). EXM_InitMovedHair,
 * EXM_OnMovedHair, EXM_OffMovedHair and EXM_SetDelayMovedHair (this unit,
 * main/tu267) use its moved-hair fields on one entry of the 64-entry `actor`
 * array (main 0x0043c1e0, 0xa70-byte stride):
 *   +0xa40 moved_hair_mode: EXM_OnMovedHair sets 2, EXM_OffMovedHair sets 0,
 *          EXM_InitMovedHair defaults to 2; JNT_computeMatrix reads it with
 *          lbu (0x00314b70).
 *   +0xa44 moved_hair_delay: EXM_SetDelayMovedHair overwrites it from $f12,
 *          EXM_InitMovedHair defaults it to 0.5f; JNT_computeMatrix reads it
 *          as a float (0x00314c24).
 *   +0xa50 moved_hair_sway: a four-float state JNT_computeMatrix filters
 *          every frame (0x00314bb0..0x00314c88, lwc1/swc1); EXM_InitMovedHair
 *          clears all four elements.
 *   +0xa60 previous_joint_matrices: the union member EXM_CalcExMotion reads as
 *          the owner's previous motion matrices (statea60 in ACT_init).
 */

typedef struct ExHairTypeSelection {
    int kind;
    unsigned int flags;
} ExHairTypeSelection;

typedef struct ExCollisionPrimitive {
    unsigned short joint;
    unsigned short kind;
    unsigned char unmodeled_004[12];
    Vector4 start;
    Vector4 end;
} ExCollisionPrimitive;

typedef struct ExCollisionRequest {
    struct ExCollisionPrimitive *primitives;
    unsigned int flags;
    float *correction;
} ExCollisionRequest;

typedef struct ExMotionChannel {
    unsigned short type;
    unsigned char unmodeled_002[12];
    unsigned short matrix_index;
    Vector4 translation;
    Vector4 rotation;
    unsigned char unmodeled_030[16];
} ExMotionChannel;

typedef struct ExHairParameters {
    float gravity;
    float length_response;
    unsigned char unmodeled_008[8];
    float response[3];
    unsigned char unmodeled_01c[4];
    float chain_response;
    unsigned char unmodeled_024[12];
} ExHairParameters;

typedef struct ExGroundQuery {
    unsigned char unmodeled_00[8];
    short attribute_mask;
    unsigned char unmodeled_0a[0x40 - 10];
} ExGroundQuery;

extern ExHairTypeSelection htypebuf[];
extern void *collibuf[1];
extern int ibuf;
extern int startjnt;
extern const float exmHairAnDSize;
extern const float exmHairAnDFootRadius;
extern const float exmHairAbFootRadius;
extern const float exmHairAsBackSize;
extern const float exmHairAsFootRadius;
extern const float exmHairBrDSkirtColliSize;
extern const float exmHairBrDSkirtColliFootRad;
extern const float exmHairBo2SkirtColliSize;
extern const float exmHairBo2SkirtColliFootRad;
extern const float exmHairClSize;
extern const float exmHairClFootRadius;
extern const float exmHairESize;
extern const float exmHairEFootRadius;
extern const float exmHairFrSkirtColliSize;
extern const float exmHairFrSkirtColliSizeB;
extern const float exmHairFrSkirtColliFootRad;
extern const float exmHairGiSkirtColliSize;
extern const float exmHairGiSkirtColliFootRad;
extern const float exmHairHeSkirtColliSize;
extern const float exmHairHeSkirtColliSizeB;
extern const float exmHairHeSkirtColliFootRad;
extern const float exmHairISkirtColliSize;
extern const float exmHairISkirtColliSizeB;
extern const float exmHairISkirtColliFootRad;
extern const float exmHairJr1SkirtColliSize;
extern const float exmHairJr1SkirtColliFootRad;
extern const float exmHairKeNSkirtColliSizeB;
extern const float exmHairKeNSkirtColliSizeS;
extern const float exmHairKeNSkirtColliFootRad;
extern const float exmHairMoHmSkirtColliSize;
extern const float exmHairMoHmSkirtColliFootRad;
extern const float exmHairObSkirtColliSize;
extern const float exmHairObSkirtColliFootRad;
extern const float exmHairShioCSkirtColliSize;
extern const float exmHairShioCSkirtColliFootRad;
extern const float exmHairSeSkirtColliSize;
extern const float exmHairSeSkirtColliFootRad;
extern const float exmHairVSkirtColliSizeB;
extern const float exmHairVSkirtColliSizeS;
extern const float exmHairVSkirtColliFootRad;
extern const float exmHairWSkirtColliSizeB;
extern const float exmHairWSkirtColliFootRad;
extern const float exmHairYSkirtColliFootRad;
extern float vel_0[4];
extern ExHairParameters hairpara[];
extern float JR_coatAdjust[];

void xglMatrixMul(Matrix *destination, Matrix *first, Matrix *second);
void EXM_ResetWave(void);
void UnduParamInit(void *query);
float asinf(float value);
float UnduCheck(const void *position, void *velocity, ExGroundQuery *query);

static int EXM_CheckExCollision(ExMotionJointWork *work, const float position[3],
                               const Matrix4 jointMatrix, ExCollisionRequest *request);

void EXM_GetWindPower(float output[3], ExMotionJointWork *work, const float input[3],
                      const float parent_position[4]);

static ExCollisionPrimitive abelcol[3];

static ExCollisionPrimitive albeltcol[3];

static ExCollisionPrimitive allencol[2];

static ExCollisionPrimitive chaoscol[2];

static ExCollisionPrimitive ellycol[5];

static ExCollisionPrimitive gaignuncol[2];

static ExCollisionPrimitive helmercol[3];

static ExCollisionPrimitive hyakucol[4];

static ExCollisionPrimitive kebincol[3];

static ExCollisionPrimitive kebinhcol[2];

static ExCollisionPrimitive mamcol[4];

static ExCollisionPrimitive miyukicol[3];

static ExCollisionPrimitive momo1col[8];

static ExCollisionPrimitive momo4col[5];

static ExCollisionPrimitive momocol[4];

static ExCollisionPrimitive offcol[2];

static ExCollisionPrimitive robotcol[2];

static ExCollisionPrimitive shionchcol[3];

static ExCollisionPrimitive shioncol[5];

static ExCollisionPrimitive shitancol[4];

static ExCollisionPrimitive tonnycol[4];

#endif
