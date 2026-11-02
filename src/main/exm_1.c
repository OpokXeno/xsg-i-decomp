#include "common.h"
#include "main/exm_1.h"
#include "exm_1.h"
#include "vu0_macro.h"

/* Motion channel types handled by EXM_CalcExMotion (hair chains). */
#define EX_CHANNEL_CHAIN_JOINT 2 /* queues one joint's translation/rotation */
#define EX_CHANNEL_CHAIN_BEGIN 3 /* sets the chain's base matrix */
#define EX_CHANNEL_CHAIN_SOLVE 4 /* simulates the queued chain */

/* htypebuf[0].kind sentinels; other values index hairpara[]. */
#define EX_HAIR_TYPE_NONE 999
#define EX_HAIR_TYPE_AUTO 1000 /* pick the type from the joint name */
#define EX_HAIR_PARAM_SKIRT 16 /* hairpara[] entry used by the skirt types */

/* ExCollisionPrimitive.kind (0 terminates a table). */
#define EX_COLLISION_END 0
#define EX_COLLISION_SPHERE 1
#define EX_COLLISION_CAPSULE 2
#define EX_COLLISION_SPHERE_INSIDE 3 /* keeps the point inside the sphere */
#define EX_COLLISION_SPHERE_FLAG1 4  /* sphere active only with EX_HAIR_FLAG_ALT_SHAPES */
#define EX_COLLISION_SPHERE_FLAG2 5  /* sphere active only with EX_HAIR_FLAG_KIND5 */
#define EX_COLLISION_SPHERE_FLAG8 6  /* sphere active only with EX_HAIR_FLAG_KIND6_ONLY */
#define EX_COLLISION_SPHERE_FLAG16 7 /* sphere active only with EX_HAIR_FLAG_KIND7_ONLY */

/* Per-type flags (ExHairTypeSelection.flags / ExCollisionRequest.flags). */
#define EX_HAIR_FLAG_ALT_SHAPES 0x1  /* collide against kind-4 shapes only; also bends the chain */
#define EX_HAIR_FLAG_KIND5 0x2
#define EX_HAIR_FLAG_KIND6_ONLY 0x8  /* only kind-6 shapes collide */
#define EX_HAIR_FLAG_KIND7_ONLY 0x10 /* only kind-7 shapes collide */
#define EX_HAIR_FLAG_COAT_WIDTH 0x20 /* AdjustJrCoatWidth on every joint */
#define EX_HAIR_FLAG_SKIRT_SPLIT_A 0x80  /* skirt checked in two halves, joints 16/29 */
#define EX_HAIR_FLAG_SKIRT_SPLIT_B 0x100 /* skirt checked in two halves from the rear basis */
#define EX_HAIR_FLAG_SKIRT 0x400     /* skirt collision instead of primitive tables */
#define EX_SKIRT_FLAG_ANGLE_180 0x800 /* profile angle selectors: 180 deg alone, */
#define EX_SKIRT_FLAG_ANGLE_90 0x1000 /* 90 deg alone, 270 deg alone, 0 deg alone, */
#define EX_SKIRT_FLAG_ANGLE_270 0x2000 /* with a pair giving the diagonal between */
#define EX_SKIRT_FLAG_ANGLE_0 0x4000
#define EX_SKIRT_FLAG_LONG 0x8000    /* two more sections (joints 54..72) */
#define EX_SKIRT_FLAG_HEM_LIFT_A 0x10000 /* keep the hem above the leg ends (sections 0 and 6) */
#define EX_SKIRT_FLAG_HEM_LIFT_B 0x20000 /* same for section 1 */

/* Skirt hair types: EX_HAIR_FLAG_SKIRT plus the profile angle in degrees. */
#define EX_HAIR_SKIRT_0 (EX_HAIR_FLAG_SKIRT | EX_SKIRT_FLAG_ANGLE_0)
#define EX_HAIR_SKIRT_45 (EX_HAIR_SKIRT_0 | EX_SKIRT_FLAG_ANGLE_90)
#define EX_HAIR_SKIRT_315 (EX_HAIR_SKIRT_0 | EX_SKIRT_FLAG_ANGLE_270)
#define EX_HAIR_SKIRT_90 (EX_HAIR_FLAG_SKIRT | EX_SKIRT_FLAG_ANGLE_90)
#define EX_HAIR_SKIRT_270 (EX_HAIR_FLAG_SKIRT | EX_SKIRT_FLAG_ANGLE_270)
#define EX_HAIR_SKIRT_180 (EX_HAIR_FLAG_SKIRT | EX_SKIRT_FLAG_ANGLE_180)
#define EX_HAIR_SKIRT_135 (EX_HAIR_SKIRT_180 | EX_SKIRT_FLAG_ANGLE_90)
#define EX_HAIR_SKIRT_225 (EX_HAIR_SKIRT_180 | EX_SKIRT_FLAG_ANGLE_270)

/* Skeleton joints used as origins by the skirt collision. */
#define EX_JOINT_ROOT 11 /* x/z origin */
#define EX_JOINT_HIP 13  /* height origin */

/* work->animation_flags bits read by EXM_CalcExMotion. */
#define EX_ANIM_FLAG_RELATIVE_PREVIOUS 0x10000 /* previous pose relative to its group base */
#define EX_ANIM_FLAG_NO_WIND 0x100000
#define EX_ANIM_FLAG_NO_GRAVITY 0x200000
#define EX_ANIM_FLAG_NO_GROUND 0x400000
#define EX_ANIM_FLAG_HOLD_PREVIOUS 0x80000 /* copy the previous joint instead of simulating */

#define EX_START_JOINT_DEFAULT 74 /* default first joint of the hair buffer */

#define EXM_SET_JOINT_POSITION(joint, pos)    do {         (joint)->elements[12] = (pos)[0];         (joint)->elements[13] = (pos)[1];         (joint)->elements[14] = (pos)[2];     } while (0)


typedef float ExMatrixRow[4];

float atan2f(float y, float x);

static void CheckSkirtCollisionSub(ExMotionJointWork *work, float position[4],
                                   float referencePosition[4], u32 flags);

static void AdjustJrCoatWidth(float width, Vector4 *position, const Vector4 *joint_position);

/* Palette records are 0x150-byte tables; only their base addresses are used here. */
static u32 kosmoscol[84] = {
    [0] = 0x00010030, [4] = 0x3CCCCCCD, [5] = 0xBD8F5C29,
    [7] = 0x3E000000, [12] = 0x00050030, [16] = 0xBD0F5C29,
    [17] = 0xBDCCCCCD, [19] = 0x3E051EB8, [24] = 0x00020033,
    [31] = 0x3DB851EC, [32] = 0x3DCCCCCD, [35] = 0x3D8F5C29,
    [36] = 0x0002003F, [43] = 0x3DB851EC, [44] = 0x3DCCCCCD,
    [47] = 0x3D8F5C29, [48] = 0x00040027, [52] = 0x3DCCCCCD,
    [53] = 0x3EE66666, [55] = 0x3F0CCCCD, [60] = 0x0004000D,
    [64] = 0x3DCCCCCD, [65] = 0xBEBD70A4, [67] = 0x3F000000
};

static u32 shelleycol[84] = {
    [0] = 0x00010030, [4] = 0x3CF5C28F, [5] = 0xBDA3D70A,
    [7] = 0x3DA3D70A, [12] = 0x00020033, [16] = 0xBCA3D70A,
    [19] = 0x3DCCCCCD, [20] = 0x3DCCCCCD, [23] = 0x3D99999A,
    [24] = 0x0002003F, [28] = 0xBCA3D70A, [31] = 0x3DCCCCCD,
    [32] = 0x3DCCCCCD, [35] = 0x3D99999A, [36] = 0x00010027,
    [40] = 0x3DCCCCCD, [41] = 0x3C23D70A, [42] = 0x3D4CCCCD,
    [43] = 0x3DA3D70A, [48] = 0x00010027, [52] = 0x3DCCCCCD,
    [53] = 0x3C23D70A, [54] = 0xBD4CCCCD, [55] = 0x3DA3D70A,
    [60] = 0x00040027, [64] = 0x3DCCCCCD, [65] = 0x3F051EB8,
    [67] = 0x3F266666
};

static u32 humancol[36] = {
    [0] = 0x00010030, [4] = 0x3C23D70A, [5] = 0xBD8F5C29,
    [7] = 0x3DCCCCCD, [12] = 0x0004002A, [16] = 0x3D4CCCCD,
    [17] = 0x3E19999A, [19] = 0x3E99999A
};

static float skirtColliSize = 1.0f;

static float skirtColliSizeB = 1.0f;

static float skirtColliSizeS = 1.0f;

static float skirtColliFootRad = 0.05f;

static int rootBorn = 0;

static ExCollisionPrimitive shioncol[5] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.029999999329447746f, -0.10000000149011612f, 0.0f, 0.10999999940395355f } },
    { .joint = 51, .kind = EX_COLLISION_CAPSULE, .start = { -0.05000000074505806f, 0.0099999997764825821f, 0.0099999997764825821f, 0.075000002980232239f }, .end = { 0.14000000059604645f, -0.0099999997764825821f, -0.0099999997764825821f, 0.059999998658895493f } },
    { .joint = 45, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { 0.0f, 0.47999998927116394f, 0.0f, 0.60000002384185791f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG8, .start = { 0.0f, 0.25999999046325684f, 0.0f, 0.40000000596046448f } },
    { 0 }
};

static ExCollisionPrimitive chaoscol[2] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.0099999997764825821f, -0.090000003576278687f, 0.0f, 0.11500000208616257f } },
    { 0 }
};

static ExCollisionPrimitive momocol[4] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.10000000149011612f, 0.0f, 0.090000003576278687f } },
    { .joint = 51, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.090000003576278687f }, .end = { 0.10000000149011612f, 0.0f, 0.0f, 0.070000000298023224f } },
    { .joint = 63, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.090000003576278687f }, .end = { 0.10000000149011612f, 0.0f, 0.0f, 0.070000000298023224f } },
    { 0 }
};

static ExCollisionPrimitive momo1col[8] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.10000000149011612f, 0.0f, 0.090000003576278687f } },
    { .joint = 51, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.090000003576278687f }, .end = { 0.10000000149011612f, 0.0f, 0.0f, 0.070000000298023224f } },
    { .joint = 63, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.090000003576278687f }, .end = { 0.10000000149011612f, 0.0f, 0.0f, 0.070000000298023224f } },
    { .joint = 16, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, -0.0f, 0.12999999523162842f }, .end = { 0.40000000596046448f, 0.0f, 0.0f, 0.12999999523162842f } },
    { .joint = 26, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.12999999523162842f }, .end = { 0.40000000596046448f, 0.0f, 0.0f, 0.12999999523162842f } },
    { .joint = 17, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, -0.0f, 0.10000000149011612f }, .end = { 0.40000000596046448f, 0.0f, 0.0f, 0.12999999523162842f } },
    { .joint = 27, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.10000000149011612f }, .end = { 0.40000000596046448f, 0.0f, 0.0f, 0.12999999523162842f } },
    { 0 }
};

static ExCollisionPrimitive momo4col[5] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.10000000149011612f, 0.0f, 0.090000003576278687f } },
    { .joint = 51, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.090000003576278687f }, .end = { 0.10000000149011612f, 0.0f, 0.0f, 0.070000000298023224f } },
    { .joint = 63, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.090000003576278687f }, .end = { 0.10000000149011612f, 0.0f, 0.0f, 0.070000000298023224f } },
    { .joint = 39, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { 0.11999999731779099f, 0.40000000596046448f, 0.0f, 0.55000001192092896f } },
    { 0 }
};

static ExCollisionPrimitive hyakucol[4] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.10000000149011612f, 0.0f, 0.090000003576278687f } },
    { .joint = 51, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.079999998211860657f }, .end = { 0.05000000074505806f, 0.0f, 0.0f, 0.059999998658895493f } },
    { .joint = 63, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.079999998211860657f }, .end = { 0.05000000074505806f, 0.0f, 0.0f, 0.059999998658895493f } },
    { 0 }
};

static ExCollisionPrimitive shitancol[4] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.05000000074505806f, 0.0f, 0.070000000298023224f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { -0.10000000149011612f, 0.62999999523162842f, 0.0f, 0.80000001192092896f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE, .start = { 0.05000000074505806f, -0.15000000596046448f, 0.0f, 0.30000001192092896f } },
    { 0 }
};

static ExCollisionPrimitive ellycol[5] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.0099999997764825821f, -0.079999998211860657f, 0.0f, 0.10000000149011612f } },
    { .joint = 51, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.079999998211860657f }, .end = { 0.079999998211860657f, 0.0f, 0.0f, 0.059999998658895493f } },
    { .joint = 63, .kind = EX_COLLISION_CAPSULE, .start = { 0.0f, 0.0f, 0.0f, 0.079999998211860657f }, .end = { 0.079999998211860657f, 0.0f, 0.0f, 0.059999998658895493f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { -0.079999998211860657f, 0.30000001192092896f, 0.0f, 0.40000000596046448f } },
    { 0 }
};

static ExCollisionPrimitive tonnycol[4] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.10000000149011612f, 0.0f, 0.090000003576278687f } },
    { .joint = 16, .kind = EX_COLLISION_CAPSULE, .start = { -0.10000000149011612f, 0.0f, -0.019999999552965164f, 0.15000000596046448f }, .end = { 0.40000000596046448f, 0.0f, 0.0f, 0.14000000059604645f } },
    { .joint = 26, .kind = EX_COLLISION_CAPSULE, .start = { -0.10000000149011612f, 0.0f, 0.019999999552965164f, 0.15000000596046448f }, .end = { 0.40000000596046448f, 0.0f, 0.0f, 0.14000000059604645f } },
    { 0 }
};

static ExCollisionPrimitive albeltcol[3] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.05000000074505806f, -0.079999998211860657f, 0.0f, 0.079999998211860657f } },
    { .joint = 36, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { -0.05000000074505806f, 0.20000000298023224f, 0.0f, 0.40000000596046448f } },
    { 0 }
};

static ExCollisionPrimitive kebincol[3] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.059999998658895493f, 0.0f, 0.10000000149011612f } },
    { .joint = 48, .kind = EX_COLLISION_SPHERE_INSIDE, .start = { 0.041999999433755875f, -0.059999998658895493f, 0.0f, 0.10999999940395355f } },
    { 0 }
};

static ExCollisionPrimitive kebinhcol[2] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.014999999664723873f, -0.070000000298023224f, 0.0f, 0.10000000149011612f } },
    { 0 }
};

static ExCollisionPrimitive shionchcol[3] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.0099999997764825821f, -0.079999998211860657f, 0.0f, 0.10000000149011612f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { -0.079999998211860657f, 0.30000001192092896f, 0.0f, 0.40000000596046448f } },
    { 0 }
};

static ExCollisionPrimitive helmercol[3] = {
    { .joint = 55, .kind = EX_COLLISION_SPHERE_FLAG8, .start = { 0.34999999403953552f, 0.0f, -0.2199999988079071f, 0.30000001192092896f } },
    { .joint = 67, .kind = EX_COLLISION_SPHERE_FLAG16, .start = { 0.34999999403953552f, 0.0f, 0.2199999988079071f, 0.30000001192092896f } },
    { 0 }
};

static ExCollisionPrimitive allencol[2] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.029999999329447746f, -0.090000003576278687f, 0.0f, 0.090000003576278687f } },
    { 0 }
};

static ExCollisionPrimitive abelcol[3] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.0099999997764825821f, -0.070000000298023224f, 0.0f, 0.10000000149011612f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { 0.0f, 0.20000000298023224f, 0.0f, 0.30000001192092896f } },
    { 0 }
};

static ExCollisionPrimitive mamcol[4] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.0099999997764825821f, -0.070000000298023224f, 0.0f, 0.10000000149011612f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { 0.05000000074505806f, 0.15000000596046448f, 0.0f, 0.2800000011920929f } },
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG8, .start = { 0.05000000074505806f, -0.15000000596046448f, 0.0f, 0.2800000011920929f } },
    { 0 }
};

static ExCollisionPrimitive gaignuncol[2] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE_FLAG8, .start = { 0.0099999997764825821f, -0.070000000298023224f, 0.0f, 0.10000000149011612f } },
    { 0 }
};

static ExCollisionPrimitive offcol[2] = {
    { .joint = 42, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { 0.05000000074505806f, 0.18000000715255737f, 0.0f, 0.30000001192092896f } },
    { 0 }
};

static ExCollisionPrimitive miyukicol[3] = {
    { .joint = 48, .kind = EX_COLLISION_SPHERE, .start = { 0.039999999105930328f, -0.10000000149011612f, 0.0f, 0.090000003576278687f } },
    { .joint = 39, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { 0.11999999731779099f, 0.30000001192092896f, 0.0f, 0.40000000596046448f } },
    { 0 }
};

static ExCollisionPrimitive robotcol[2] = {
    { .joint = 39, .kind = EX_COLLISION_SPHERE_FLAG1, .start = { 0.079999998211860657f, -0.20000000298023224f, 0.0f, 0.40000000596046448f } },
    { 0 }
};

static ExHairParameters hairpara[22] = { { .gravity = 0.019999999552965164f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 0.89999997615814209f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.5f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 1.0f, 1.0f, 1.0f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 100.0f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.019999999552965164f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 0.89999997615814209f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.5f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.029999999329447746f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.69999998807907104f, 0.69999998807907104f, 0.69999998807907104f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.60000002384185791f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.019999999552965164f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 0.89999997615814209f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.30000001192092896f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0099999997764825821f, .length_response = 0.89999997615814209f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 1.0f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.69999998807907104f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0099999997764825821f, .length_response = 0.89999997615814209f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 1.0f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 1.2000000476837158f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0099999997764825821f, .length_response = 0.89999997615814209f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.5f, 0.5f, 0.5f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 2.0f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0f, .length_response = 0.80000001192092896f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.5f, 0.5f, 0.5f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 1.0f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.80000001192092896f, 0.80000001192092896f, 0.80000001192092896f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 1.7000000476837158f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.029999999329447746f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 0.89999997615814209f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 2.0f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.05000000074505806f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.0f, 0.0f, 0.0f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.5f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.05000000074505806f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.20000000298023224f, 0.20000000298023224f, 0.20000000298023224f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.5f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.039999999105930328f, .length_response = 0.89999997615814209f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.69999998807907104f, 0.69999998807907104f, 0.69999998807907104f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.20000000298023224f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.05000000074505806f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.5f, 0.5f, 0.5f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.10000000149011612f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.5f, .length_response = 1.0f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 0.89999997615814209f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 1.0f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0099999997764825821f, .length_response = 0.80000001192092896f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.20000000298023224f, 0.20000000298023224f, 0.20000000298023224f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 1.0f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0f, .length_response = 0.89999997615814209f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.89999997615814209f, 0.89999997615814209f, 0.89999997615814209f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.30000001192092896f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.05000000074505806f, .length_response = 0.80000001192092896f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.75f, 0.75f, 0.75f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.5f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0f, .length_response = 0.80000001192092896f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.94999998807907104f, 0.94999998807907104f, 0.94999998807907104f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 2.0f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.05000000074505806f, .length_response = 0.10000000149011612f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.0f, 0.0f, 0.0f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.5f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .gravity = 0.0f, .length_response = 0.0099999997764825821f, .unmodeled_008 = { 0, 0, 0, 0, 0, 0, 0, 0 }, .response = { 0.0f, 0.0f, 0.0f }, .unmodeled_01c = { 0, 0, 0, 0 }, .chain_response = 0.5f, .unmodeled_024 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } };

static float JR_coatAdjust[3] = { 0.098499998450279236f, 0.12849999964237213f, 0.19850000739097595f };

static float vel_0[4];

static ExHairTypeSelection htypebuf[24];

static inline Matrix *CopyExMatrix(float *destination, const float *source)
{
    return __builtin_memcpy(destination, source, sizeof(Matrix));
}

static inline float *ExMatrixData(Matrix *matrix)
{
    return matrix->elements;
}

static inline ExMatrixRow *ExMatrixRows(Matrix *matrix)
{
    return matrix->rows;
}

static inline const ExMatrixRow *ExMatrixConstRows(const Matrix *matrix)
{
    return matrix->rows;
}

static inline Matrix *CopyExMatrixObject(Matrix *destination, const Matrix *source)
{
    *destination = *source;
    return destination;
}

static inline Vector4 *TransformExDirection(const Matrix *matrix,
                                        const Vector4 *source,
                                        Vector4 *destination)
{
    float x = source->x;
    float y = source->y;
    float z = source->z;
    destination->x = matrix->elements[0] * x + matrix->elements[1] * y + matrix->elements[2] * z;
    destination->y = matrix->elements[4] * x + matrix->elements[5] * y + matrix->elements[6] * z;
    destination->z = matrix->elements[8] * x + matrix->elements[9] * y + matrix->elements[10] * z;
    return destination;
}

static inline Matrix *ExMotionMatrices(ExMotionJointWork *work)
{
    return (Matrix *)work->matrix_buffer;
}

static inline void OrientExMotionJoint(ExMotionJointWork *work, int index,
                                      Vector4 *direction)
{
    Matrix *current = ExMotionMatrices(work) + work->saved_index + index;
    Vector4 *transformed;
    float horizontalLength;
    float length;
    float yawSin;
    float yawCos;
    float pitchAngle;
    float yawAngle;
    int matrixIndex;

    transformed = TransformExDirection(current, direction, direction);
    horizontalLength = __builtin_sqrtf(transformed->x * transformed->x + transformed->y * transformed->y);
    length = __builtin_sqrtf(transformed->x * transformed->x + transformed->y * transformed->y + transformed->z * transformed->z);
    yawSin = -transformed->y / horizontalLength;
    yawCos = transformed->x / horizontalLength;
    pitchAngle = -atan2f(transformed->z / length, horizontalLength / length);
    MATRIX_rotY4s(ExMotionMatrices(work)[work->saved_index + index].rows, pitchAngle);
    yawAngle = -atan2f(yawSin, yawCos);
    matrixIndex = work->saved_index + index;
    MATRIX_rotZ4s(ExMotionMatrices(work)[matrixIndex].rows, yawAngle);
}

static signed char GetLastWord(signed char *text)
{
    int index;

    index = 0;
    if (text != 0) {
        if (text[0] != 0 && !(text[0] & 0x80)) {
            do {
                index++;
            } while (text[index] != 0 && !(text[index] & 0x80));
        }
        if (index != 0) {
            return text[index - 1];
        }
        return 0;
    }
    return 0;
}

void EXM_PresetHair(ExMotionJointWork *work)
{
    int lastLetter = GetLastWord(work->hair_joint_name);
    signed char *name;

    ibuf = 0;
    name = work->hair_joint_name;
    htypebuf[0].kind = EX_HAIR_TYPE_NONE;
    startjnt = EX_START_JOINT_DEFAULT;
    collibuf[0] = 0;
    if (name == 0) {
        return;
    }
    switch (name[0]) {
    case 'A':
        switch (name[1]) {
        case 'L':
            switch (name[2]) {
            case 'B':
                collibuf[0] = albeltcol;
                if (lastLetter == 'H') {
                    htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags =
                        htypebuf[3].flags = htypebuf[4].flags = htypebuf[5].flags =
                        htypebuf[6].flags = EX_HAIR_FLAG_ALT_SHAPES;
                    htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind =
                        htypebuf[3].kind = htypebuf[4].kind = htypebuf[5].kind =
                        htypebuf[6].kind = 18;
                    htypebuf[7].flags = 0;
                    htypebuf[7].kind = 6;
                    htypebuf[8].flags = htypebuf[9].flags = 0;
                    htypebuf[8].kind = htypebuf[9].kind = 7;
                    htypebuf[10].flags = 0;
                    htypebuf[10].kind = 6;
                    htypebuf[11].flags = htypebuf[12].flags = htypebuf[13].flags = 0;
                    htypebuf[11].kind = htypebuf[12].kind = htypebuf[13].kind = 8;
                } else {
                    htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags =
                        htypebuf[3].flags = htypebuf[4].flags = EX_HAIR_FLAG_ALT_SHAPES;
                    htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind =
                        htypebuf[3].kind = htypebuf[4].kind = 18;
                }
                break;
            case 'L':
                collibuf[0] = allencol;
                htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags = 0;
                htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind = 10;
                htypebuf[3].flags = htypebuf[4].flags = htypebuf[5].flags = 0;
                htypebuf[3].kind = htypebuf[4].kind = htypebuf[5].kind = 1;
                htypebuf[6].flags = htypebuf[7].flags = 0;
                htypebuf[6].kind = htypebuf[7].kind = 10;
                break;
            }
            break;
        case 'N':
            switch (name[4]) {
            case 'W': {
                float size = 1.1f;
                float unitSize = 1.0f;
                float footRadius = 0.05f;
                skirtColliSizeB = skirtColliSize = size;
                skirtColliSizeS = unitSize;
                skirtColliFootRad = footRadius;
                htypebuf[0].flags = EX_HAIR_SKIRT_0;
                htypebuf[1].flags = EX_HAIR_SKIRT_180;
                htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
                return;
            }
            case 'D': {
                float size = exmHairAnDSize;
                float unitSize = 1.0f;
                float footRadius = exmHairAnDFootRadius;
                collibuf[0] = humancol;
                skirtColliSizeB = skirtColliSize = size;
                skirtColliSizeS = unitSize;
                skirtColliFootRad = footRadius;
                htypebuf[0].flags = EX_HAIR_SKIRT_45 | EX_SKIRT_FLAG_HEM_LIFT_B;
                htypebuf[1].flags = EX_HAIR_SKIRT_315 | EX_SKIRT_FLAG_HEM_LIFT_B;
                htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
                htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
                htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
                htypebuf[4].flags = htypebuf[5].flags = 0;
                htypebuf[4].kind = htypebuf[5].kind = 7;
                htypebuf[6].flags = 0;
                htypebuf[6].kind = 8;
                break;
            }
            }
            break;
        case 'B': {
            float footRadius = exmHairAbFootRadius;
            collibuf[0] = abelcol;
            skirtColliSizeS = skirtColliSizeB = skirtColliSize = 1.0f;
            skirtColliFootRad = footRadius;
            htypebuf[0].flags = EX_HAIR_SKIRT_45;
            htypebuf[1].flags = EX_HAIR_SKIRT_315;
            htypebuf[2].flags = EX_HAIR_SKIRT_135;
            htypebuf[3].flags = EX_HAIR_SKIRT_225;
            htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            htypebuf[4].flags = EX_HAIR_FLAG_ALT_SHAPES;
            htypebuf[4].kind = 4;
            htypebuf[5].flags = htypebuf[6].flags = 0;
            htypebuf[5].kind = htypebuf[6].kind = 8;
            break;
        }
        case 'S': {
            float unitSize = 1.0f;
            float backSize = exmHairAsBackSize;
            float footRadius = exmHairAsFootRadius;
            skirtColliSizeB = backSize;
            skirtColliSizeS = skirtColliSize = unitSize;
            skirtColliFootRad = footRadius;
            htypebuf[0].flags = EX_HAIR_SKIRT_45;
            htypebuf[1].flags = EX_HAIR_SKIRT_315;
            htypebuf[2].flags = EX_HAIR_SKIRT_135;
            htypebuf[3].flags = EX_HAIR_SKIRT_225;
            htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            break;
        }
        }
        break;
    case 'B':
        switch (name[1]) {
        case 'R':
            if (lastLetter == 'D') {
                skirtColliSize = exmHairBrDSkirtColliSize;
                skirtColliSizeS = skirtColliSizeB = 1.0f;
                skirtColliFootRad = exmHairBrDSkirtColliFootRad;
                htypebuf[0].flags = EX_HAIR_SKIRT_180 | EX_SKIRT_FLAG_HEM_LIFT_A;
                htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
                break;
            }
            htypebuf[0].kind = 4;
            collibuf[0] = shelleycol;
            htypebuf[1].flags = htypebuf[0].flags = 0;
            break;
        case 'A':
            collibuf[0] = humancol;
            htypebuf[0].flags = EX_HAIR_FLAG_ALT_SHAPES;
            htypebuf[0].kind = 4;
            break;
        case 'O':
            switch (name[5]) {
            case '2':
                skirtColliSize = exmHairBo2SkirtColliSize;
                startjnt = 136;
                skirtColliSizeS = skirtColliSizeB = 1.0f;
                skirtColliFootRad = exmHairBo2SkirtColliFootRad;
                htypebuf[0].flags = EX_HAIR_SKIRT_90 | EX_HAIR_FLAG_SKIRT_SPLIT_A;
                htypebuf[1].flags = EX_HAIR_SKIRT_270 | EX_HAIR_FLAG_SKIRT_SPLIT_A;
                htypebuf[2].flags = EX_HAIR_SKIRT_0 | EX_HAIR_FLAG_SKIRT_SPLIT_A;
                htypebuf[3].flags = EX_HAIR_SKIRT_45 | EX_HAIR_FLAG_SKIRT_SPLIT_A;
                htypebuf[4].flags = EX_HAIR_SKIRT_315 | EX_HAIR_FLAG_SKIRT_SPLIT_A;
                htypebuf[4].kind = htypebuf[3].kind = htypebuf[2].kind =
                    htypebuf[1].kind = htypebuf[0].kind = 13;
                break;
            case '3':
                htypebuf[0].kind = EX_HAIR_TYPE_AUTO;
                break;
            }
            break;
        }
        break;
    case 'C':
        switch (name[1]) {
        case 'H':
            collibuf[0] = chaoscol;
            if (lastLetter == 'H') {
                htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags =
                    htypebuf[3].flags = htypebuf[4].flags = 0;
                htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind =
                    htypebuf[3].kind = htypebuf[4].kind = 5;
                htypebuf[5].flags = htypebuf[6].flags = htypebuf[7].flags =
                    htypebuf[8].flags = htypebuf[9].flags = 0;
                htypebuf[5].kind = htypebuf[6].kind = htypebuf[7].kind =
                    htypebuf[8].kind = htypebuf[9].kind = 5;
                htypebuf[10].flags = htypebuf[11].flags = htypebuf[12].flags =
                    htypebuf[13].flags = 0;
                htypebuf[10].kind = htypebuf[11].kind = htypebuf[12].kind =
                    htypebuf[13].kind = 9;
            } else {
                htypebuf[0].flags = htypebuf[1].flags = 0;
                htypebuf[0].kind = htypebuf[1].kind = 6;
                htypebuf[2].flags = htypebuf[3].flags = 0;
                htypebuf[2].kind = htypebuf[3].kind = 9;
            }
            break;
        case 'A':
            collibuf[0] = humancol;
            if (lastLetter == '1') {
                htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags =
                    htypebuf[3].flags = htypebuf[4].flags = 0;
                htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind =
                    htypebuf[3].kind = htypebuf[4].kind = 6;
                htypebuf[5].flags = htypebuf[6].flags = 0;
                htypebuf[5].kind = htypebuf[6].kind = 1;
            } else {
                htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags = 0;
                htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind = 7;
                htypebuf[3].flags = htypebuf[4].flags = 0;
                htypebuf[3].kind = htypebuf[4].kind = 7;
            }
            break;
        case 'L': {
            float size = exmHairClSize;
            float unitSize = 1.0f;
            float footRadius = exmHairClFootRadius;
            skirtColliSizeB = skirtColliSize = size;
            skirtColliSizeS = unitSize;
            skirtColliFootRad = footRadius;
            htypebuf[0].flags = EX_HAIR_SKIRT_0;
            htypebuf[1].flags = EX_HAIR_SKIRT_180;
            htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            break;
        }
        }
        break;
    case 'E': {
        float size = exmHairESize;
        float unitSize = 1.0f;
        float footRadius = exmHairEFootRadius;
        collibuf[0] = ellycol;
        skirtColliSizeB = skirtColliSize = size;
        skirtColliSizeS = unitSize;
        skirtColliFootRad = footRadius;
        htypebuf[0].flags = EX_HAIR_SKIRT_45;
        htypebuf[1].flags = EX_HAIR_SKIRT_315;
        htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
        htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
        htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
        htypebuf[4].flags = EX_HAIR_FLAG_ALT_SHAPES;
        htypebuf[4].kind = 2;
        htypebuf[5].flags = htypebuf[6].flags = 0;
        htypebuf[5].kind = htypebuf[6].kind = 5;
        htypebuf[7].flags = htypebuf[8].flags = 0;
        htypebuf[7].kind = htypebuf[8].kind = 4;
        break;
    }
    case 'F':
        switch (name[1]) {
        case 'R':
            collibuf[0] = humancol;
            htypebuf[0].flags = htypebuf[1].flags = 0;
            htypebuf[0].kind = htypebuf[1].kind = 5;
            break;
        case 'E':
            skirtColliSize = exmHairFrSkirtColliSize;
            skirtColliSizeB = exmHairFrSkirtColliSizeB;
            skirtColliSizeS = 1.0f;
            skirtColliFootRad = exmHairFrSkirtColliFootRad;
            htypebuf[0].flags = EX_HAIR_SKIRT_45;
            htypebuf[1].flags = EX_HAIR_SKIRT_315;
            htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
            htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
            htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            htypebuf[4].flags = 0;
            htypebuf[4].kind = 7;
            htypebuf[5].flags = htypebuf[6].flags = 0;
            htypebuf[5].kind = htypebuf[6].kind = 7;
            break;
        }
        break;
    case 'G':
        switch (name[1]) {
        case 'A':
            collibuf[0] = gaignuncol;
            htypebuf[0].flags = htypebuf[2].flags = EX_HAIR_FLAG_KIND6_ONLY;
            htypebuf[0].kind = htypebuf[2].kind = 7;
            htypebuf[1].flags = EX_HAIR_FLAG_KIND6_ONLY;
            htypebuf[1].kind = 5;
            htypebuf[3].flags = htypebuf[4].flags = 0;
            htypebuf[3].kind = htypebuf[4].kind = 7;
            htypebuf[5].flags = 0;
            htypebuf[5].kind = 8;
            break;
        case 'I':
            skirtColliSize = exmHairGiSkirtColliSize;
            skirtColliSizeS = skirtColliSizeB = 1.0f;
            skirtColliFootRad = exmHairGiSkirtColliFootRad;
            htypebuf[0].flags = EX_HAIR_SKIRT_45 | EX_SKIRT_FLAG_HEM_LIFT_B;
            htypebuf[1].flags = EX_HAIR_SKIRT_315 | EX_SKIRT_FLAG_HEM_LIFT_B;
            htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
            htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
            htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            break;
        }
        break;
    case 'H':
        switch (name[1]) {
        case 'A':
            htypebuf[0].flags = htypebuf[1].flags = 0;
            htypebuf[0].kind = htypebuf[1].kind = 8;
            break;
        case 'E':
            collibuf[0] = helmercol;
            skirtColliSize = exmHairHeSkirtColliSize;
            skirtColliSizeB = exmHairHeSkirtColliSizeB;
            skirtColliSizeS = 1.0f;
            skirtColliFootRad = exmHairHeSkirtColliFootRad;
            htypebuf[0].flags = EX_HAIR_SKIRT_45;
            htypebuf[1].flags = EX_HAIR_SKIRT_315;
            htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
            htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
            htypebuf[4].flags = EX_HAIR_SKIRT_90;
            htypebuf[5].flags = EX_HAIR_SKIRT_270;
            htypebuf[5].kind = htypebuf[4].kind = htypebuf[3].kind =
                htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            htypebuf[6].flags = EX_SKIRT_FLAG_ANGLE_90 | EX_HAIR_FLAG_KIND6_ONLY;
            htypebuf[7].flags = EX_SKIRT_FLAG_ANGLE_270 | EX_HAIR_FLAG_KIND7_ONLY;
            htypebuf[7].kind = htypebuf[6].kind = 18;
            break;
        case 'Y':
            collibuf[0] = hyakucol;
            htypebuf[0].flags = htypebuf[1].flags = 0;
            htypebuf[0].kind = htypebuf[1].kind = 4;
            htypebuf[2].flags = htypebuf[3].flags = 0;
            htypebuf[2].kind = htypebuf[3].kind = 7;
            htypebuf[4].flags = htypebuf[5].flags = 0;
            htypebuf[4].kind = htypebuf[5].kind = 4;
            break;
        }
        break;
    case 'I':
        skirtColliSize = exmHairISkirtColliSize;
        skirtColliSizeB = exmHairISkirtColliSizeB;
        skirtColliSizeS = 1.0f;
        skirtColliFootRad = exmHairISkirtColliFootRad;
        htypebuf[0].flags = EX_HAIR_SKIRT_45 | EX_SKIRT_FLAG_HEM_LIFT_B;
        htypebuf[1].flags = EX_HAIR_SKIRT_315 | EX_SKIRT_FLAG_HEM_LIFT_B;
        htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
        htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
        htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
        break;
    case 'J':
        switch (name[1]) {
        case 'R':
            if (name[2] == '1') {
                htypebuf[0].kind = 11;
                htypebuf[0].flags = 0;
                return;
            }
            skirtColliSize = exmHairJr1SkirtColliSize;
            skirtColliSizeS = skirtColliSizeB = 1.0f;
            skirtColliFootRad = exmHairJr1SkirtColliFootRad;
            htypebuf[0].flags = EX_HAIR_SKIRT_0;
            htypebuf[1].flags = EX_HAIR_SKIRT_90;
            htypebuf[2].flags = EX_HAIR_SKIRT_270;
            htypebuf[3].flags = EX_HAIR_SKIRT_45 | EX_HAIR_FLAG_COAT_WIDTH;
            htypebuf[4].flags = EX_HAIR_SKIRT_315 | EX_HAIR_FLAG_COAT_WIDTH;
            htypebuf[4].kind = htypebuf[3].kind = htypebuf[2].kind =
                    htypebuf[1].kind = htypebuf[0].kind = 13;
            htypebuf[5].flags = 0;
            htypebuf[5].kind = 11;
            break;
        case 'O':
            collibuf[0] = kebinhcol;
            htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags = htypebuf[3].flags = 0;
            htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind = htypebuf[3].kind = 7;
            break;
        }
        break;
    case 'K':
        switch (name[1]) {
        case 'O':
            collibuf[0] = kosmoscol;
            if (lastLetter == 'H') {
                htypebuf[0].flags = EX_HAIR_FLAG_ALT_SHAPES;
                htypebuf[0].kind = 3;
                htypebuf[1].flags = htypebuf[2].flags = htypebuf[3].flags = 0;
                htypebuf[1].kind = htypebuf[2].kind = htypebuf[3].kind = 5;
                htypebuf[4].flags = htypebuf[5].flags = EX_HAIR_FLAG_KIND5;
                htypebuf[4].kind = htypebuf[5].kind = 4;
                htypebuf[6].flags = htypebuf[7].flags = 0;
                htypebuf[6].kind = htypebuf[7].kind = 8;
                return;
            }
            if (lastLetter != '2') {
                htypebuf[0].flags = EX_HAIR_FLAG_ALT_SHAPES;
                htypebuf[0].kind = 3;
                htypebuf[1].flags = htypebuf[2].flags = 0;
                htypebuf[1].kind = htypebuf[2].kind = 5;
                htypebuf[3].flags = htypebuf[4].flags = EX_HAIR_FLAG_KIND5;
                htypebuf[3].kind = htypebuf[4].kind = 6;
                htypebuf[5].flags = 0;
                htypebuf[5].kind = 8;
            } else {
                htypebuf[0].kind = EX_HAIR_TYPE_AUTO;
            }
            break;
        case 'E': {
            ExCollisionPrimitive *defaultPalette = kebincol;
            if (lastLetter == 'H') {
                collibuf[0] = kebinhcol;
                htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags = 0;
                htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind = 6;
                htypebuf[3].flags = htypebuf[4].flags = htypebuf[5].flags =
                    htypebuf[6].flags = htypebuf[7].flags = htypebuf[8].flags = 0;
                htypebuf[3].kind = htypebuf[4].kind = htypebuf[5].kind =
                    htypebuf[6].kind = htypebuf[7].kind = htypebuf[8].kind = 6;
            } else {
                collibuf[0] = defaultPalette;
                if (lastLetter == 'N') {
                    skirtColliSize = 2.0f;
                    skirtColliSizeB = exmHairKeNSkirtColliSizeB;
                    skirtColliSizeS = exmHairKeNSkirtColliSizeS;
                    skirtColliFootRad = exmHairKeNSkirtColliFootRad;
                    htypebuf[0].flags = EX_HAIR_SKIRT_0;
                    htypebuf[1].flags = EX_HAIR_SKIRT_90 | EX_SKIRT_FLAG_LONG;
                    htypebuf[2].flags = EX_HAIR_SKIRT_270 | EX_SKIRT_FLAG_LONG;
                    htypebuf[3].flags = EX_HAIR_SKIRT_45 | EX_SKIRT_FLAG_LONG;
                    htypebuf[4].flags = EX_HAIR_SKIRT_315 | EX_SKIRT_FLAG_LONG;
                    htypebuf[4].kind = htypebuf[3].kind = htypebuf[2].kind =
                    htypebuf[1].kind = htypebuf[0].kind = 13;
                    htypebuf[5].flags = htypebuf[6].flags = 0;
                    htypebuf[5].kind = htypebuf[6].kind = 7;
                } else {
                    htypebuf[0].flags = htypebuf[1].flags = 0;
                    htypebuf[0].kind = htypebuf[1].kind = 7;
                }
            }
            break;
        }
        }
        break;
    case 'L':
        collibuf[0] = momocol;
        htypebuf[0].flags = htypebuf[1].flags = 0;
        htypebuf[0].kind = htypebuf[1].kind = 5;
        htypebuf[2].flags = htypebuf[3].flags = 0;
        htypebuf[2].kind = htypebuf[3].kind = 4;
        break;
    case 'M':
        switch (name[1]) {
        case 'O':
            if (lastLetter == 'H') {
                if (name[2] == 'M') {
                    collibuf[0] = momocol;
                    htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags =
                        htypebuf[3].flags = htypebuf[4].flags = 0;
                    htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind =
                        htypebuf[3].kind = htypebuf[4].kind = 5;
                    htypebuf[5].flags = htypebuf[6].flags = htypebuf[7].flags = 0;
                    htypebuf[5].kind = htypebuf[6].kind = htypebuf[7].kind = 10;
                    htypebuf[8].flags = htypebuf[9].flags = 0;
                    htypebuf[8].kind = htypebuf[9].kind = 2;
                    htypebuf[10].flags = 0;
                    htypebuf[10].kind = 8;
                    return;
                }
                skirtColliSize = exmHairMoHmSkirtColliSize;
                skirtColliSizeS = skirtColliSizeB = 1.0f;
                skirtColliFootRad = exmHairMoHmSkirtColliFootRad;
                htypebuf[0].flags = EX_HAIR_SKIRT_0;
                htypebuf[1].flags = EX_HAIR_SKIRT_180;
                htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            } else if (lastLetter == '1') {
                collibuf[0] = momo1col;
                htypebuf[0].flags = htypebuf[1].flags = 0;
                htypebuf[0].kind = htypebuf[1].kind = 14;
                htypebuf[2].flags = 0;
                htypebuf[2].kind = 8;
                htypebuf[3].flags = htypebuf[4].flags = 0;
                htypebuf[3].kind = htypebuf[4].kind = 5;
                htypebuf[5].flags = htypebuf[6].flags = 0;
                htypebuf[5].kind = htypebuf[6].kind = 4;
                htypebuf[7].flags = 0;
                htypebuf[7].kind = 8;
            } else if (lastLetter == '2') {
                collibuf[0] = momocol;
                skirtColliSize = 1.3f;
                skirtColliSizeB = 0.8f;
                skirtColliSizeS = 1.0f;
                skirtColliFootRad = 0.05f;
                htypebuf[0].flags = EX_HAIR_SKIRT_45;
                htypebuf[1].flags = EX_HAIR_SKIRT_315;
                htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
                htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
                htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
                htypebuf[4].flags = htypebuf[5].flags = htypebuf[6].flags = htypebuf[7].flags = 0;
                htypebuf[4].kind = htypebuf[5].kind = htypebuf[6].kind = htypebuf[7].kind = 17;
                htypebuf[8].flags = 0;
                htypebuf[8].kind = 8;
                htypebuf[9].flags = htypebuf[10].flags = 0;
                htypebuf[9].kind = htypebuf[10].kind = 5;
                htypebuf[11].flags = htypebuf[12].flags = 0;
                htypebuf[11].kind = htypebuf[12].kind = 4;
                htypebuf[13].flags = 0;
                htypebuf[13].kind = 8;
            } else if (lastLetter == '4') {
                collibuf[0] = momo4col;
                htypebuf[0].flags = EX_HAIR_FLAG_ALT_SHAPES;
                htypebuf[0].kind = 4;
                htypebuf[1].flags = htypebuf[2].flags = 0;
                htypebuf[1].kind = htypebuf[2].kind = 5;
                htypebuf[3].flags = htypebuf[4].flags = 0;
                htypebuf[3].kind = htypebuf[4].kind = 4;
            } else {
                collibuf[0] = momocol;
                htypebuf[0].flags = 0;
                htypebuf[0].kind = 8;
                htypebuf[1].flags = htypebuf[2].flags = 0;
                htypebuf[1].kind = htypebuf[2].kind = 5;
                htypebuf[3].flags = htypebuf[4].flags = 0;
                htypebuf[3].kind = htypebuf[4].kind = 4;
                htypebuf[5].flags = 0;
                htypebuf[5].kind = 8;
            }
            break;
        case 'A':
            collibuf[0] = momocol;
            htypebuf[0].flags = htypebuf[1].flags = htypebuf[2].flags = htypebuf[3].flags = 0;
            htypebuf[0].kind = htypebuf[1].kind = htypebuf[2].kind = htypebuf[3].kind = 4;
            break;
        case 'I':
            collibuf[0] = miyukicol;
            htypebuf[0].flags = htypebuf[2].flags = 0;
            htypebuf[0].kind = htypebuf[2].kind = 4;
            htypebuf[1].flags = EX_HAIR_FLAG_ALT_SHAPES;
            htypebuf[1].kind = 2;
            htypebuf[3].flags = htypebuf[4].flags = htypebuf[5].flags = 0;
            htypebuf[3].kind = htypebuf[4].kind = htypebuf[5].kind = 7;
            break;
        }
        break;
    case 'O':
        switch (name[1]) {
        case 'B':
            skirtColliSize = exmHairObSkirtColliSize;
            skirtColliSizeS = skirtColliSizeB = 1.0f;
            skirtColliFootRad = exmHairObSkirtColliFootRad;
            htypebuf[0].flags = EX_HAIR_SKIRT_45;
            htypebuf[1].flags = EX_HAIR_SKIRT_315;
            htypebuf[2].flags = EX_HAIR_SKIRT_135;
            htypebuf[3].flags = EX_HAIR_SKIRT_225;
            htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            break;
        case 'F': {
            const u32 channelFlags = 1;
            const int channelKind = 4;
            collibuf[0] = offcol;
            htypebuf[0].flags = channelFlags;
            htypebuf[0].kind = channelKind;
            break;
        }
        }
        break;
    case 'P':
        htypebuf[0].kind = EX_HAIR_TYPE_AUTO;
        break;
    case 'R': {
        float unitSize = 1.0f;
        float size = 1.3f;
        float footRadius = 0.15f;
        collibuf[0] = robotcol;
        skirtColliSize = size;
        skirtColliSizeS = skirtColliSizeB = unitSize;
        skirtColliFootRad = footRadius;
        htypebuf[0].flags = EX_HAIR_SKIRT_0;
        htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
        htypebuf[1].flags = EX_HAIR_SKIRT_0;
        htypebuf[1].kind = EX_HAIR_PARAM_SKIRT;
        htypebuf[2].flags = EX_HAIR_SKIRT_180;
        htypebuf[2].kind = EX_HAIR_PARAM_SKIRT;
        htypebuf[3].flags = EX_HAIR_SKIRT_180;
        htypebuf[3].kind = EX_HAIR_PARAM_SKIRT;
        htypebuf[4].flags = EX_HAIR_FLAG_ALT_SHAPES;
        htypebuf[4].kind = 14;
        break;
    }
    case 'S':
        switch (name[1]) {
        case 'H':
            switch (name[2]) {
            case 'I':
                switch (name[3]) {
                case 'O':
                    collibuf[0] = shioncol;
                    if (lastLetter == 'H') {
                        if (name[6] == 'C') {
                            collibuf[0] = shionchcol;
                            skirtColliSize = exmHairShioCSkirtColliSize;
                            skirtColliSizeS = skirtColliSizeB = 1.0f;
                            skirtColliFootRad = exmHairShioCSkirtColliFootRad;
                            htypebuf[0].flags = EX_HAIR_SKIRT_45;
                            htypebuf[1].flags = EX_HAIR_SKIRT_315;
                            htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
                            htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
                            htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = 15;
                            htypebuf[4].flags = EX_HAIR_FLAG_ALT_SHAPES;
                            htypebuf[4].kind = 2;
                            htypebuf[5].flags = htypebuf[6].flags = htypebuf[7].flags = 0;
                            htypebuf[5].kind = htypebuf[6].kind = htypebuf[7].kind = 7;
                        } else {
                            htypebuf[0].flags = EX_HAIR_FLAG_ALT_SHAPES;
                            htypebuf[0].kind = 2;
                            htypebuf[8].flags = EX_HAIR_FLAG_KIND6_ONLY;
                            htypebuf[8].kind = 4;
                            htypebuf[1].flags = htypebuf[2].flags = htypebuf[7].flags = 0;
                            htypebuf[1].kind = htypebuf[2].kind = htypebuf[7].kind = 8;
                            htypebuf[3].flags = 0;
                            htypebuf[3].kind = 4;
                            htypebuf[4].flags = htypebuf[5].flags = htypebuf[6].flags = 0;
                            htypebuf[4].kind = htypebuf[5].kind = htypebuf[6].kind = 5;
                        }
                    } else {
                        htypebuf[0].flags = 0;
                        htypebuf[0].kind = 6;
                        htypebuf[1].flags = htypebuf[4].flags = 0;
                        htypebuf[1].kind = htypebuf[4].kind = 8;
                        htypebuf[2].flags = EX_HAIR_FLAG_ALT_SHAPES;
                        htypebuf[2].kind = 2;
                        htypebuf[3].flags = 0;
                        htypebuf[3].kind = 4;
                    }
                    break;
                case 'T':
                    collibuf[0] = shitancol;
                    if (lastLetter == 'H') {
                        htypebuf[0].kind = 10;
                        htypebuf[1].flags = htypebuf[0].flags = EX_HAIR_FLAG_ALT_SHAPES;
                        htypebuf[1].kind = 4;
                        htypebuf[2].flags = htypebuf[3].flags = 0;
                        htypebuf[2].kind = htypebuf[3].kind = 4;
                    } else {
                        htypebuf[0].flags = EX_HAIR_FLAG_ALT_SHAPES;
                        htypebuf[0].kind = 4;
                        htypebuf[1].flags = htypebuf[2].flags = 0;
                        htypebuf[1].kind = htypebuf[2].kind = 4;
                    }
                    break;
                case '_':
                    switch (name[4]) {
                    case 'D':
                        collibuf[0] = humancol;
                        htypebuf[0].flags = htypebuf[1].flags = 0;
                        htypebuf[0].kind = htypebuf[1].kind = 6;
                        htypebuf[2].flags = 0;
                        htypebuf[2].kind = 5;
                        break;
                    case 'M':
                        collibuf[0] = mamcol;
                        htypebuf[0].kind = 7;
                        htypebuf[1].flags = EX_HAIR_FLAG_ALT_SHAPES;
                        htypebuf[1].kind = 2;
                        htypebuf[0].flags = EX_HAIR_FLAG_KIND6_ONLY;
                        htypebuf[2].flags = htypebuf[3].flags = htypebuf[4].flags = 0;
                        htypebuf[2].kind = htypebuf[3].kind = htypebuf[4].kind = 8;
                        break;
                    }
                    break;
                }
                break;
            case 'E':
                htypebuf[0].kind = EX_HAIR_TYPE_AUTO;
                break;
            }
            break;
        case 'E':
            skirtColliSize = exmHairSeSkirtColliSize;
            skirtColliSizeS = skirtColliSizeB = 1.0f;
            skirtColliFootRad = exmHairSeSkirtColliFootRad;
            htypebuf[0].flags = EX_HAIR_SKIRT_45;
            htypebuf[1].flags = EX_HAIR_SKIRT_315;
            htypebuf[2].flags = EX_HAIR_SKIRT_135;
            htypebuf[3].flags = EX_HAIR_SKIRT_225;
            htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
            htypebuf[4].flags = htypebuf[5].flags = 0;
            htypebuf[4].kind = htypebuf[5].kind = 5;
            break;
        case 'T':
            collibuf[0] = humancol;
            htypebuf[0].flags = htypebuf[1].flags = 0;
            htypebuf[0].kind = htypebuf[1].kind = 5;
            htypebuf[2].flags = 0;
            htypebuf[2].kind = 7;
            htypebuf[3].flags = htypebuf[4].flags = 0;
            htypebuf[3].kind = htypebuf[4].kind = 8;
            break;
        }
        break;
    case 'T':
        collibuf[0] = tonnycol;
        htypebuf[0].flags = htypebuf[1].flags = 0;
        htypebuf[0].kind = htypebuf[1].kind = 4;
        htypebuf[2].flags = htypebuf[3].flags = htypebuf[4].flags = 0;
        htypebuf[2].kind = htypebuf[3].kind = htypebuf[4].kind = 5;
        break;
    case 'U':
        collibuf[0] = momocol;
        htypebuf[0].flags = htypebuf[1].flags = 0;
        htypebuf[0].kind = htypebuf[1].kind = 4;
        break;
    case 'V':
        skirtColliSize = 2.0f;
        skirtColliSizeB = exmHairVSkirtColliSizeB;
        skirtColliSizeS = exmHairVSkirtColliSizeS;
        skirtColliFootRad = exmHairVSkirtColliFootRad;
        htypebuf[0].flags = EX_HAIR_SKIRT_0;
        htypebuf[1].flags = EX_HAIR_SKIRT_90 | EX_SKIRT_FLAG_LONG;
        htypebuf[2].flags = EX_HAIR_SKIRT_270 | EX_SKIRT_FLAG_LONG;
        htypebuf[3].flags = EX_HAIR_SKIRT_45 | EX_SKIRT_FLAG_LONG;
        htypebuf[4].flags = EX_HAIR_SKIRT_315 | EX_SKIRT_FLAG_LONG;
        htypebuf[4].kind = htypebuf[3].kind = htypebuf[2].kind =
                    htypebuf[1].kind = htypebuf[0].kind = 13;
        break;
    case 'W':
        skirtColliSize = 1.5f;
        skirtColliSizeB = exmHairWSkirtColliSizeB;
        skirtColliSizeS = 1.0f;
        skirtColliFootRad = exmHairWSkirtColliFootRad;
        htypebuf[0].flags = EX_HAIR_SKIRT_45;
        htypebuf[1].flags = EX_HAIR_SKIRT_315;
        htypebuf[2].flags = EX_HAIR_SKIRT_135 | EX_SKIRT_FLAG_HEM_LIFT_A;
        htypebuf[3].flags = EX_HAIR_SKIRT_225 | EX_SKIRT_FLAG_HEM_LIFT_A;
        htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
        break;
    case 'Y':
        skirtColliSizeS = skirtColliSizeB = skirtColliSize = 1.0f;
        skirtColliFootRad = exmHairYSkirtColliFootRad;
        htypebuf[0].flags = EX_HAIR_SKIRT_45;
        htypebuf[1].flags = EX_HAIR_SKIRT_315;
        htypebuf[2].flags = EX_HAIR_SKIRT_135;
        htypebuf[3].flags = EX_HAIR_SKIRT_225;
        htypebuf[3].kind = htypebuf[2].kind = htypebuf[1].kind = htypebuf[0].kind = EX_HAIR_PARAM_SKIRT;
        htypebuf[4].flags = htypebuf[5].flags = 0;
        htypebuf[4].kind = htypebuf[5].kind = 7;
        break;
    case 'Z':
        htypebuf[0].flags = htypebuf[1].flags = 0;
        htypebuf[0].kind = htypebuf[1].kind = 5;
        break;
    }
}

static u32 GetHairTypeSelection(u32 *flags, void **palette, ExMotionJointWork *work)
{
    u32 result;
    signed char *jointName;
    signed char kind;

    jointName = work->hair_joint_name;
    result = 0;
    *flags = 0;
    if (jointName != 0) {
        kind = jointName[0];
        switch (kind) {
        case 'P':
            {
                s16 hairTypeCode = work->hair_type_code;
                float unitSize = 1.0f;
                float sizeValue = 0.8f;
                float footRadius = 0.05f;

                *palette = humancol;
                skirtColliSize = sizeValue;
                skirtColliSizeS = (skirtColliSizeB = unitSize);
                skirtColliFootRad = footRadius;
                switch (hairTypeCode) {
                case 'J':
                    *flags = EX_HAIR_SKIRT_0;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 'O':
                    *flags = EX_HAIR_SKIRT_180;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 'T':
                    result = 8;
                    break;
                case 'W':
                case '[':
                    result = 5;
                    break;
                case '_':
                case 'd':
                    result = 7;
                    break;
                default:
                    break;
                }
            }
            break;
        case 'S':
            {
                s16 hairTypeCode = work->hair_type_code;
                float unitSize = 1.0f;

                *palette = shelleycol;
                skirtColliSize = unitSize;
                skirtColliSizeB = unitSize;
                skirtColliSizeS = unitSize;
                skirtColliFootRad = 0.05f;
                switch (hairTypeCode) {
                case 'J':
                    *flags = EX_HAIR_SKIRT_0;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 'M':
                    *flags = EX_HAIR_SKIRT_180;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 'P':
                    *flags = EX_HAIR_FLAG_ALT_SHAPES;
                    result = 2;
                    break;
                case 'V':
                case 'Z':
                    result = 5;
                    break;
                case '^':
                case 'e':
                    result = 4;
                    break;
                default:
                    break;
                }
            }
            break;
        case 'B':
            {
                s16 hairTypeCode = work->hair_type_code;
                float unitSize = 1.0f;

                skirtColliFootRad = 0.15f;
                skirtColliSize = unitSize;
                skirtColliSizeB = unitSize;
                skirtColliSizeS = unitSize;
                switch (hairTypeCode) {
                case 'q':
                    *flags = EX_HAIR_SKIRT_45 | EX_HAIR_FLAG_SKIRT_SPLIT_B;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 'u':
                    *flags = EX_HAIR_SKIRT_0 | EX_HAIR_FLAG_SKIRT_SPLIT_B;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 'y':
                    *flags = EX_HAIR_SKIRT_315 | EX_HAIR_FLAG_SKIRT_SPLIT_B;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 0x7d:
                    *flags = EX_HAIR_SKIRT_135 | EX_HAIR_FLAG_SKIRT_SPLIT_B | EX_SKIRT_FLAG_HEM_LIFT_A;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 0x80:
                    *flags = EX_HAIR_SKIRT_225 | EX_HAIR_FLAG_SKIRT_SPLIT_B | EX_SKIRT_FLAG_HEM_LIFT_A;
                    result = EX_HAIR_PARAM_SKIRT;
                    break;
                case 0x83:
                case 0x87:
                case 0x8b:
                case 0x96:
                case 0x9b:
                    result = 4;
                    break;
                case 0x8e:
                case 0x92:
                    result = 5;
                    break;
                default:
                    break;
                }
            }
            break;
        case 'K':
            {
                s16 hairTypeCode = work->hair_type_code;

                *palette = kosmoscol;
                switch (hairTypeCode) {
                case 'J':
                case 'M':
                case 'P':
                case 'S':
                    result = 12;
                    break;
                case 'V':
                    *flags |= EX_HAIR_FLAG_ALT_SHAPES;
                    result = 3;
                    break;
                case 0x5c:
                case '_':
                    result = 5;
                    break;
                case 'b':
                case 'g':
                    *flags |= EX_HAIR_FLAG_KIND5;
                    result = 4;
                    break;
                case 'l':
                    result = 8;
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
    return result;
}

static int EXM_CheckExCollision(ExMotionJointWork *work, const float position[3],
                               const Matrix4 jointMatrix, ExCollisionRequest *request)
{
    JntMatrix startMatrix;
    Vector4 difference;
    float correctionScale;
    int count = 0;
    int index;

    vel_0[0] = vel_0[1] = vel_0[2] = 0.0f;
    index = 0;
    while (request->primitives[index].kind != EX_COLLISION_END) {
        const ExCollisionPrimitive *primitive = &request->primitives[index];
        u32 filterFlags = request->flags;

        if ((filterFlags & EX_HAIR_FLAG_KIND6_ONLY) && primitive->kind != EX_COLLISION_SPHERE_FLAG8) {
            index++;
            continue;
        }
        if ((filterFlags & EX_HAIR_FLAG_KIND7_ONLY) && primitive->kind != EX_COLLISION_SPHERE_FLAG16) {
            index++;
            continue;
        }
        __builtin_memcpy(startMatrix.elements, work->matrix_buffer[primitive->joint].elements,
                         sizeof(startMatrix.elements));
        MATRIX_translate4s(startMatrix.elements, primitive->start.x,
                           primitive->start.y, primitive->start.z);
        switch (primitive->kind) {
        case EX_COLLISION_SPHERE_FLAG2:
            if (!(request->flags & EX_HAIR_FLAG_KIND5)) {
                break;
            }
            goto check_sphere;
        case EX_COLLISION_SPHERE_FLAG16:
            if (!(request->flags & EX_HAIR_FLAG_KIND7_ONLY)) {
                break;
            }
            goto check_sphere;
        case EX_COLLISION_SPHERE_FLAG8:
            if (!(request->flags & EX_HAIR_FLAG_KIND6_ONLY)) {
                break;
            }
        case EX_COLLISION_SPHERE:
        case EX_COLLISION_SPHERE_INSIDE:
        check_sphere:
        {
            float radiusSquared;
            float separationMeasure;
            if (request->flags & EX_HAIR_FLAG_ALT_SHAPES) {
                break;
            }
            difference.x = position[0] - startMatrix.elements[3][0];
            difference.y = position[1] - startMatrix.elements[3][1];
            difference.z = position[2] - startMatrix.elements[3][2];
            separationMeasure = difference.x * difference.x +
                              difference.y * difference.y +
                              difference.z * difference.z;
            correctionScale = primitive->start.w * jointMatrix[0][3];
            if (separationMeasure == 0.0f) {
                break;
            }
            radiusSquared = correctionScale * correctionScale;
            if (primitive->kind == EX_COLLISION_SPHERE_INSIDE) {
                if (radiusSquared < separationMeasure) {
                    float correctedX;
                    float correctedY;
                    float correctedZ;
                    correctionScale = correctionScale / __builtin_sqrtf(separationMeasure) - 1.0f;
                    count++;
                    correctedX = vel_0[0] + difference.x * correctionScale;
                    correctedY = vel_0[1] + difference.y * correctionScale;
                    correctedZ = vel_0[2] + difference.z * correctionScale;
                    vel_0[2] = correctedZ;
                    vel_0[0] = correctedX;
                    vel_0[1] = correctedY;
                }
            } else {
                if (separationMeasure < radiusSquared) {
                    float correctedX;
                    float correctedY;
                    float correctedZ;
                    correctionScale = correctionScale / __builtin_sqrtf(separationMeasure) - 1.0f;
                    count++;
                    correctedX = vel_0[0] + difference.x * correctionScale;
                    correctedY = vel_0[1] + difference.y * correctionScale;
                    correctedZ = vel_0[2] + difference.z * correctionScale;
                    vel_0[2] = correctedZ;
                    vel_0[0] = correctedX;
                    vel_0[1] = correctedY;
                }
            }
            break;
        }
        case EX_COLLISION_SPHERE_FLAG1:
            if (request->flags & EX_HAIR_FLAG_ALT_SHAPES) {
                float radiusSquared;
                float separationMeasure;
                difference.x = position[0] - startMatrix.elements[3][0];
                difference.y = position[1] - startMatrix.elements[3][1];
                difference.z = position[2] - startMatrix.elements[3][2];
                separationMeasure = difference.x * difference.x +
                                  difference.y * difference.y +
                                  difference.z * difference.z;
                correctionScale = primitive->start.w * jointMatrix[0][3];
                if (separationMeasure == 0.0f) {
                    break;
                }
                radiusSquared = correctionScale * correctionScale;
                if (separationMeasure < radiusSquared) {
                    correctionScale = correctionScale / __builtin_sqrtf(separationMeasure) - 1.0f;
                    count++;
                    {
                        float correctedX = vel_0[0] + difference.x * correctionScale;
                        float correctedY = vel_0[1] + difference.y * correctionScale;
                        float correctedZ = vel_0[2] + difference.z * correctionScale;
                        vel_0[2] = correctedZ;
                        vel_0[0] = correctedX;
                        vel_0[1] = correctedY;
                    }
                }
            }
            break;
        case EX_COLLISION_CAPSULE:
            if (!(request->flags & EX_HAIR_FLAG_ALT_SHAPES)) {
                JntMatrix endMatrix;
                Vector4 axis;
                float projection;
                float axisOriginDot;
                float pointProjection;
                float capsuleRadius;
                float separationMeasure;
                float originX;
                float originY;
                float originZ;
                __builtin_memcpy(endMatrix.elements, startMatrix.elements, sizeof(endMatrix.elements));
                MATRIX_translate4s(endMatrix.elements, primitive->end.x,
                                   primitive->end.y, primitive->end.z);
                originX = startMatrix.elements[3][0];
                originY = startMatrix.elements[3][1];
                axis.x = endMatrix.elements[3][0] - originX;
                axis.y = endMatrix.elements[3][1] - originY;
                originZ = startMatrix.elements[3][2];
                axis.z = endMatrix.elements[3][2] - originZ;
                axisOriginDot = axis.x * originX;
                axisOriginDot += axis.y * originY;
                axisOriginDot += axis.z * originZ;
                projection = -axisOriginDot;
                pointProjection = axis.x * position[0] + axis.y * position[1];
                pointProjection += axis.z * position[2];
                projection += pointProjection;
                separationMeasure = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
                if (separationMeasure != 0.0f) {
                    projection /= separationMeasure;
                } else {
                    projection = 0.0f;
                }
                if (projection <= -0.1f) {
                    break;
                }
                if (projection >= 1.1f) {
                    break;
                }
                axis.x = position[0] - (originX + axis.x * projection);
                axis.y = position[1] - (originY + axis.y * projection);
                axis.z = position[2] - (originZ + axis.z * projection);
                separationMeasure = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
                {
                    float startRadius = primitive->start.w;
                    float endRadius = primitive->end.w;
                    if (endRadius == startRadius) {
                        capsuleRadius = startRadius;
                    } else {
                        capsuleRadius = startRadius + (endRadius - startRadius) * projection;
                    }
                }
                capsuleRadius = projection <= 0.0f ?
                    capsuleRadius * ((projection + 0.1f) * 10.0f) : capsuleRadius;
                capsuleRadius = projection >= 1.0f ?
                    capsuleRadius * ((1.1f - projection) * 10.0f) : capsuleRadius;
                capsuleRadius *= jointMatrix[0][3];
                if (separationMeasure < capsuleRadius * capsuleRadius) {
                    if (separationMeasure != 0.0f) {
                        separationMeasure = capsuleRadius / __builtin_sqrtf(separationMeasure) - 1.0f;
                        {
                            float correctedX = vel_0[0] + axis.x * separationMeasure;
                            float correctedY = vel_0[1] + axis.y * separationMeasure;
                            float correctedZ = vel_0[2] + axis.z * separationMeasure;
                            vel_0[2] = correctedZ;
                            vel_0[0] = correctedX;
                            vel_0[1] = correctedY;
                        }
                    }
                    count++;
                }
            }
            break;
        }
        index++;
    }
    if (count != 0) {
        float averageX;
        float averageY;
        float averageZ;
        correctionScale = 1.0f / count;
        averageX = vel_0[0] * correctionScale;
        averageY = vel_0[1] * correctionScale;
        averageZ = vel_0[2] * correctionScale;
        vel_0[0] = averageX;
        vel_0[2] = averageZ;
        vel_0[1] = averageY;
    }
    request->correction = vel_0;
    return count;
}

static void CheckSkirtCollisionSub(ExMotionJointWork *work, float position[4],
                                   float referencePosition[4], u32 flags)
{
    float orientationSin;
    float orientationCos;
    float responseX;
    float responseZ;
    float distanceScale;
    float lengthMeasure;
    float firstZ;
    float (*firstStart)[4];
    float (*firstEnd)[4];
    float (*secondStart)[4];
    float (*secondEnd)[4];
    u32 profileFlags;

    {
        const float defaultResponse = 0.2f;
        float horizontalResponse = defaultResponse;
        float depthResponse = defaultResponse;
        float profileAngle = 0.0f;
        profileFlags = flags;

        if (profileFlags & EX_SKIRT_FLAG_ANGLE_180) {
            if (profileFlags & EX_SKIRT_FLAG_ANGLE_90) {
                const float selectedProfileResponse = 0.7f;
                const float selectedProfileAngle = 2.3561945f;
                horizontalResponse = selectedProfileResponse;
                profileAngle = selectedProfileAngle;
                depthResponse = selectedProfileResponse;
            } else if (profileFlags & EX_SKIRT_FLAG_ANGLE_270) {
                const float selectedProfileResponse = 0.7f;
                const float selectedProfileAngle = 3.926991f;
                horizontalResponse = selectedProfileResponse;
                profileAngle = selectedProfileAngle;
                depthResponse = selectedProfileResponse;
            } else {
                const float selectedProfileAngle = 3.1415927f;
                horizontalResponse = 1.0f;
                profileAngle = selectedProfileAngle;
            }
        } else if (profileFlags & EX_SKIRT_FLAG_ANGLE_0) {
            if (profileFlags & EX_SKIRT_FLAG_ANGLE_90) {
                horizontalResponse = 0.7f;
                profileAngle = 0.7853982f;
                depthResponse = horizontalResponse;
            } else if (profileFlags & EX_SKIRT_FLAG_ANGLE_270) {
                horizontalResponse = 0.7f;
                profileAngle = 5.4977875f;
                depthResponse = horizontalResponse;
            } else {
                horizontalResponse = 1.0f;
            }
        } else if (profileFlags & EX_SKIRT_FLAG_ANGLE_90) {
            depthResponse = 1.0f;
            profileAngle = 1.5707964f;
        } else {
            depthResponse = 1.0f;
            profileAngle = 4.712389f;
        }
        {
            const int reverseSide = flags & EX_HAIR_FLAG_SKIRT_SPLIT_B;
            JntMatrix basis;
            JntMatrix *joints;
            float (*originRow)[4];
            if (reverseSide) {
                __builtin_memcpy(basis.elements, work->matrix_buffer[14].elements, sizeof(basis.elements));
                basis.elements[3][0] = basis.elements[3][1] = basis.elements[3][2] = 0.0f;
                MATRIX_translate4s(basis.elements, 0.0f, 0.0f, 1.0f);
                orientationCos = basis.elements[3][0];
                orientationSin = -basis.elements[3][2];
            } else {
                __builtin_memcpy(basis.elements, work->matrix_buffer[36].elements, sizeof(basis.elements));
                basis.elements[3][0] = basis.elements[3][1] = basis.elements[3][2] = 0.0f;
                MATRIX_translate4s(basis.elements, 0.0f, 0.0f, 1.0f);
                orientationCos = basis.elements[3][0];
                orientationSin = -basis.elements[3][2];
            }
            joints = work->matrix_buffer;
            originRow = &joints[EX_JOINT_HIP].elements[3];
            switch (rootBorn) {
            case 0:
                firstStart = &joints[16].elements[3];
                secondStart = &joints[26].elements[3];
                firstEnd = &joints[17].elements[3];
                secondEnd = &joints[27].elements[3];
                break;
            case 2:
                firstStart = &joints[54].elements[3];
                secondStart = &joints[66].elements[3];
                firstEnd = &joints[56].elements[3];
                secondEnd = &joints[68].elements[3];
                break;
            case 3:
                firstStart = &joints[54].elements[3];
                secondStart = &joints[66].elements[3];
                firstEnd = &joints[60].elements[3];
                secondEnd = &joints[72].elements[3];
                break;
            case 4:
                firstStart = &joints[16].elements[3];
                secondStart = &joints[29].elements[3];
                firstEnd = &joints[17].elements[3];
                secondEnd = &joints[30].elements[3];
                break;
            case 5:
                firstStart = &joints[16].elements[3];
                secondStart = &joints[29].elements[3];
                firstEnd = &joints[20].elements[3];
                secondEnd = &joints[33].elements[3];
                break;
            case 6:
                firstStart = &joints[16].elements[3];
                secondStart = &joints[26].elements[3];
                firstEnd = &joints[18].elements[3];
                secondEnd = &joints[28].elements[3];
                break;
            case 1:
            case 7:
                firstStart = &joints[16].elements[3];
                secondStart = &joints[26].elements[3];
                firstEnd = &joints[20].elements[3];
                secondEnd = &joints[30].elements[3];
                break;
            default:
                return;
            }
            {
                if (reverseSide) {
                    float angle = -1.5707964f - profileAngle;
                    float sine;
                    responseZ = xglCos(angle);
                    sine = xglSin(angle);
                    __builtin_memcpy(&basis.elements, &work->matrix_buffer[14].elements, sizeof(basis.elements));
                    MATRIX_translate4s(basis.elements, 0.0f, responseZ, sine);
                    {
                        float (*translatedOrigin)[4] = &basis.elements[3];
                        responseX = (*translatedOrigin)[0] - (*originRow)[0];
                        {
                            float profileDepth = (*translatedOrigin)[2];
                            distanceScale = profileDepth - (*originRow)[2];
                        }
                    }
                    responseZ = orientationCos * distanceScale + orientationSin * responseX;
                    responseX = orientationCos * responseX - orientationSin * distanceScale;
                } else {
                    float sine;
                    responseZ = xglCos(profileAngle);
                    sine = xglSin(profileAngle);
                    __builtin_memcpy(basis.elements, work->matrix_buffer[EX_JOINT_HIP].elements, sizeof(basis.elements));
                    MATRIX_translate4s(basis.elements, 0.0f, responseZ, sine);
                    {
                        float (*translatedOrigin)[4] = &basis.elements[3];
                        distanceScale = (*translatedOrigin)[0] - (*originRow)[0];
                        responseX = (*translatedOrigin)[2] - (*originRow)[2];
                    }
                    responseZ = orientationCos * distanceScale - orientationSin * responseX;
                    responseX = orientationCos * responseX + orientationSin * distanceScale;
                }
            }
        }
        {
            JntMatrix *motionMatrices = work->matrix_buffer;
            position[0] -= motionMatrices[EX_JOINT_ROOT].elements[3][0];
            position[2] -= motionMatrices[EX_JOINT_ROOT].elements[3][2];
            (*firstStart)[0] -= motionMatrices[EX_JOINT_ROOT].elements[3][0];
            (*firstStart)[2] -= motionMatrices[EX_JOINT_ROOT].elements[3][2];
            (*secondStart)[0] -= motionMatrices[EX_JOINT_ROOT].elements[3][0];
            (*secondStart)[2] -= motionMatrices[EX_JOINT_ROOT].elements[3][2];
            (*firstEnd)[0] -= motionMatrices[EX_JOINT_ROOT].elements[3][0];
            (*firstEnd)[2] -= motionMatrices[EX_JOINT_ROOT].elements[3][2];
            (*secondEnd)[0] -= motionMatrices[EX_JOINT_ROOT].elements[3][0];
            (*secondEnd)[2] -= motionMatrices[EX_JOINT_ROOT].elements[3][2];
        }
        {
            Vector4 normalizedPosition;
            normalizedPosition.x = position[0];
            normalizedPosition.y = position[1];
            normalizedPosition.z = position[2];
            xglVectorNormal(&normalizedPosition, &normalizedPosition);
        }
        {
            float deltaX = position[0] - (*firstStart)[0];
            float deltaY = position[1] - (*firstStart)[1];
            float deltaZ = position[2] - (*firstStart)[2];
            float firstX;
            float segmentScale;
            float fullFootRadius;
            deltaX *= deltaX;
            deltaY *= deltaY;
            deltaZ *= deltaZ;
            {
                const float firstPointDistanceSquared = deltaX + deltaY + deltaZ;
                distanceScale = __builtin_sqrtf(firstPointDistanceSquared);
            }
            {
                float segmentX = (*firstEnd)[0] - (*firstStart)[0];
                float segmentY = (*firstEnd)[1] - (*firstStart)[1];
                float segmentZ = (*firstEnd)[2] - (*firstStart)[2];
                segmentX *= segmentX;
                segmentY *= segmentY;
                segmentZ *= segmentZ;
                lengthMeasure = distanceScale / __builtin_sqrtf(segmentX + segmentY + segmentZ);
            }
            segmentScale = skirtColliSize;
            firstX = ((*firstEnd)[0] - (*firstStart)[0]) * lengthMeasure + (*firstStart)[0];
            firstZ = ((*firstEnd)[2] - (*firstStart)[2]) * lengthMeasure + (*firstStart)[2];
            if (profileAngle < 1.5707964f || 4.712389f < profileAngle) {
                segmentScale *= skirtColliSizeB;
            }
            if (profileAngle < 2.3561945f && 0.7853982f < profileAngle) {
                segmentScale *= skirtColliSizeS;
            } else if (3.926991f < profileAngle && profileAngle < 5.4977875f) {
                segmentScale *= skirtColliSizeS;
            }
            segmentScale = lengthMeasure > 1.0f ? segmentScale / lengthMeasure : segmentScale;
            firstX *= segmentScale;
            {
                float pointX = position[0] - (*secondStart)[0];
                float pointY = position[1] - (*secondStart)[1];
                float pointZ = position[2] - (*secondStart)[2];
                firstZ *= segmentScale;
                pointX *= pointX;
                pointY *= pointY;
                pointZ *= pointZ;
                distanceScale = __builtin_sqrtf(pointX + pointY + pointZ);
            }
            {
                float segmentX = (*secondEnd)[0] - (*secondStart)[0];
                float segmentY = (*secondEnd)[1] - (*secondStart)[1];
                float segmentZ = (*secondEnd)[2] - (*secondStart)[2];
                segmentX *= segmentX;
                segmentY *= segmentY;
                segmentZ *= segmentZ;
                lengthMeasure = distanceScale / __builtin_sqrtf(segmentX + segmentY + segmentZ);
            }
            segmentScale = skirtColliSize;
            {
                float secondX = ((*secondEnd)[0] - (*secondStart)[0]) * lengthMeasure + (*secondStart)[0];
                float secondZ = ((*secondEnd)[2] - (*secondStart)[2]) * lengthMeasure + (*secondStart)[2];
                if (profileAngle < 1.5707964f || 4.712389f < profileAngle) {
                    segmentScale *= skirtColliSizeB;
                }
                if (profileAngle < 2.3561945f && 0.7853982f < profileAngle) {
                    segmentScale *= skirtColliSizeS;
                } else if (3.926991f < profileAngle && profileAngle < 5.4977875f) {
                    segmentScale *= skirtColliSizeS;
                }
                segmentScale = lengthMeasure > 1.0f ? segmentScale / lengthMeasure : segmentScale;
                secondZ *= segmentScale;
                secondX *= segmentScale;
                distanceScale = firstX;
                firstX = orientationCos * distanceScale - orientationSin * firstZ;
                firstZ = orientationCos * firstZ + orientationSin * distanceScale;
                distanceScale = secondX;
                secondX = orientationCos * distanceScale - orientationSin * secondZ;
                secondZ = orientationCos * secondZ + orientationSin * distanceScale;
                if (firstX < secondX) {
                    distanceScale = firstX;
                    firstX = secondX;
                    secondX = distanceScale;
                }
                if (firstZ < secondZ) {
                    distanceScale = firstZ;
                    firstZ = secondZ;
                    secondZ = distanceScale;
                }
                fullFootRadius = skirtColliFootRad;
                {
                    const float halfFootRadius = skirtColliFootRad * 0.5f;
                    firstX += halfFootRadius;
                    secondX -= halfFootRadius;
                    firstZ += fullFootRadius;
                    secondZ -= fullFootRadius;
                    distanceScale = orientationCos * position[0] - orientationSin * position[2];
                    {
                        {
                            lengthMeasure = orientationCos * position[2] + orientationSin * position[0];
                            if (responseZ > 0.0f) {
                                if (distanceScale < firstX) {
                                    float localXDisplacement = (firstX - distanceScale) * responseZ * depthResponse;
                                    distanceScale += localXDisplacement;
                                }
                            } else if (responseZ < 0.0f) {
                                if (secondX < distanceScale) {
                                    float localXDisplacement = (distanceScale - secondX) * responseZ * depthResponse;
                                    distanceScale += localXDisplacement;
                                }
                            }
                            if (responseX > 0.0f) {
                                if (lengthMeasure < firstZ) {
                                    float localZDisplacement = (firstZ - lengthMeasure) * responseX * horizontalResponse;
                                    lengthMeasure += localZDisplacement;
                                }
                            } else if (responseX < 0.0f) {
                                if (secondZ < lengthMeasure) {
                                    float localZDisplacement = (lengthMeasure - secondZ) * responseX * horizontalResponse;
                                    lengthMeasure += localZDisplacement;
                                }
                            }
                            position[0] = orientationCos * distanceScale + orientationSin * lengthMeasure;
                            position[2] = orientationCos * lengthMeasure - orientationSin * distanceScale;
                        }
                    }
                }
            }
        }
    }
    if (flags & EX_SKIRT_FLAG_HEM_LIFT_A) {
        if (rootBorn == 0) {
            float (*endpoint)[4] = 0;
            position[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*firstEnd)[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*secondEnd)[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            referencePosition[0] -= work->matrix_buffer[EX_JOINT_ROOT].elements[3][0];
            referencePosition[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            referencePosition[2] -= work->matrix_buffer[EX_JOINT_ROOT].elements[3][2];
            for (; endpoint != secondEnd;) {
                if (endpoint == 0) {
                    endpoint = firstEnd;
                } else {
                    endpoint = secondEnd;
                }
                firstZ = orientationCos * (*endpoint)[2] + orientationSin * (*endpoint)[0];
                if (firstZ < 0.0f) {
                    continue;
                }
                {
                    float referenceRadius;
                    lengthMeasure = __builtin_sqrtf((*endpoint)[0] * (*endpoint)[0] + (*endpoint)[2] * (*endpoint)[2]);
                    referenceRadius = __builtin_sqrtf(referencePosition[0] * referencePosition[0] + referencePosition[2] * referencePosition[2]);
                    if (!(referenceRadius < lengthMeasure)) {
                        continue;
                    }
                    distanceScale = __builtin_sqrtf(position[0] * position[0] + position[2] * position[2]);
                    if (distanceScale != 0.0f) {
                        distanceScale = lengthMeasure / distanceScale;
                    } else {
                        distanceScale = 1.0f;
                    }
                    {
                        const float pointHeight = position[1];
                        lengthMeasure = pointHeight * distanceScale;
                        if (distanceScale != 0.0f) {
                            float minimumHeight;
                            minimumHeight = (*endpoint)[1] + skirtColliFootRad + skirtColliFootRad;
                            if (lengthMeasure < minimumHeight) {
                                position[1] = minimumHeight / distanceScale;
                            }
                        }
                    }
                }
            }
            position[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*firstEnd)[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*secondEnd)[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            referencePosition[0] += work->matrix_buffer[EX_JOINT_ROOT].elements[3][0];
            referencePosition[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            referencePosition[2] += work->matrix_buffer[EX_JOINT_ROOT].elements[3][2];
        } else if (rootBorn == 6) {
            float (*endpoint)[4] = 0;
            position[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*firstEnd)[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*secondEnd)[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            while (endpoint != secondEnd) {
                if (endpoint == 0) {
                    endpoint = firstEnd;
                } else {
                    endpoint = secondEnd;
                }
                lengthMeasure = __builtin_sqrtf((*endpoint)[0] * (*endpoint)[0] + (*endpoint)[2] * (*endpoint)[2]);
                distanceScale = __builtin_sqrtf(position[0] * position[0] + position[2] * position[2]);
                if (distanceScale != 0.0f) {
                    distanceScale = lengthMeasure / distanceScale;
                } else {
                    distanceScale = 1.0f;
                }
                    {
                        const float pointHeight = position[1];
                        lengthMeasure = pointHeight * distanceScale;
                        if (distanceScale != 0.0f) {
                            float minimumHeight;
                            minimumHeight = (*endpoint)[1] + skirtColliFootRad + skirtColliFootRad;
                            if (lengthMeasure < minimumHeight) {
                                position[1] = minimumHeight / distanceScale;
                            }
                        }
                    }
            }
            position[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*firstEnd)[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
            (*secondEnd)[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        }
    }
    if ((flags & EX_SKIRT_FLAG_HEM_LIFT_B) && rootBorn == 1) {
        float (*endpoint)[4] = 0;
        position[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        (*firstEnd)[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        (*secondEnd)[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        referencePosition[0] -= work->matrix_buffer[EX_JOINT_ROOT].elements[3][0];
        referencePosition[1] -= work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        referencePosition[2] -= work->matrix_buffer[EX_JOINT_ROOT].elements[3][2];
        for (; endpoint != secondEnd;) {
            if (endpoint == 0) {
                endpoint = firstEnd;
            } else {
                endpoint = secondEnd;
            }
            firstZ = orientationCos * (*endpoint)[2] + orientationSin * (*endpoint)[0];
            if (firstZ > 0.0f) {
                continue;
            }
            {
                float referenceRadius;
                lengthMeasure = __builtin_sqrtf((*endpoint)[0] * (*endpoint)[0] + (*endpoint)[2] * (*endpoint)[2]);
                referenceRadius = __builtin_sqrtf(referencePosition[0] * referencePosition[0] + referencePosition[2] * referencePosition[2]);
                if (!(referenceRadius < lengthMeasure)) {
                    continue;
                }
                distanceScale = __builtin_sqrtf(position[0] * position[0] + position[2] * position[2]);
                if (distanceScale != 0.0f) {
                    distanceScale = lengthMeasure / distanceScale;
                } else {
                    distanceScale = 1.0f;
                }
                {
                    const float pointHeight = position[1];
                    lengthMeasure = pointHeight * distanceScale;
                    if (distanceScale != 0.0f) {
                        float minimumHeight;
                        minimumHeight = (*endpoint)[1] + skirtColliFootRad + skirtColliFootRad;
                        if (lengthMeasure < minimumHeight) {
                            position[1] = minimumHeight / distanceScale;
                        }
                    }
                }
            }
        }
        position[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        (*firstEnd)[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        (*secondEnd)[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        referencePosition[0] += work->matrix_buffer[EX_JOINT_ROOT].elements[3][0];
        referencePosition[1] += work->matrix_buffer[EX_JOINT_HIP].elements[3][1];
        referencePosition[2] += work->matrix_buffer[EX_JOINT_ROOT].elements[3][2];
    }
    {
        JntMatrix *motionMatrices = work->matrix_buffer;
        position[0] += motionMatrices[EX_JOINT_ROOT].elements[3][0];
        position[2] += motionMatrices[EX_JOINT_ROOT].elements[3][2];
        (*firstStart)[0] += motionMatrices[EX_JOINT_ROOT].elements[3][0];
        (*firstStart)[2] += motionMatrices[EX_JOINT_ROOT].elements[3][2];
        (*secondStart)[0] += motionMatrices[EX_JOINT_ROOT].elements[3][0];
        (*secondStart)[2] += motionMatrices[EX_JOINT_ROOT].elements[3][2];
        (*firstEnd)[0] += motionMatrices[EX_JOINT_ROOT].elements[3][0];
        (*firstEnd)[2] += motionMatrices[EX_JOINT_ROOT].elements[3][2];
        (*secondEnd)[0] += motionMatrices[EX_JOINT_ROOT].elements[3][0];
        (*secondEnd)[2] += motionMatrices[EX_JOINT_ROOT].elements[3][2];
    }
}

static void EXM_CheckSkirtCollision(ExMotionJointWork *work, float *position, float *matrix, u32 flags)
{
    if (flags & EX_HAIR_FLAG_SKIRT_SPLIT_A) {
        rootBorn = 4;
        CheckSkirtCollisionSub(work, position, matrix, flags);
        rootBorn = 5;
        CheckSkirtCollisionSub(work, position, matrix, flags);
        return;
    }
    if (flags & EX_HAIR_FLAG_SKIRT_SPLIT_B) {
        rootBorn = 6;
        CheckSkirtCollisionSub(work, position, matrix, flags);
        rootBorn = 7;
        CheckSkirtCollisionSub(work, position, matrix, flags);
        return;
    }
    rootBorn = 0;
    CheckSkirtCollisionSub(work, position, matrix, flags);
    rootBorn = 1;
    CheckSkirtCollisionSub(work, position, matrix, flags);
    if (flags & EX_SKIRT_FLAG_LONG) {
        rootBorn = 2;
        CheckSkirtCollisionSub(work, position, matrix, flags);
        rootBorn = 3;
        CheckSkirtCollisionSub(work, position, matrix, flags);
        return;
    }
}

static void AdjustJrCoatWidth(float width, Vector4 *position, const Vector4 *joint_position)
{
    Vector4 difference;
    float adjusted_x;
    float adjusted_y;
    float adjusted_z;
    float adjusted_width;

    difference.x = position->x - joint_position->x;
    difference.y = position->y - joint_position->y;
    difference.z = position->z - joint_position->z;
    xglVectorLength(&difference.w, &difference);
    if (difference.w == 0.0f) {
        difference.w = 1.0f;
    }
    adjusted_width = width / difference.w;
    adjusted_x = ((joint_position->x + difference.x * adjusted_width) * 0.5f) +
                 (position->x * 0.5f);
    adjusted_y = ((joint_position->y + difference.y * adjusted_width) * 0.5f) +
                 (position->y * 0.5f);
    adjusted_z = ((joint_position->z + difference.z * adjusted_width) * 0.5f) +
                 (position->z * 0.5f);
    position->x = adjusted_x;
    position->y = adjusted_y;
    position->z = adjusted_z;
}

ExMotionChannel *EXM_CalcExMotion(ExMotionJointWork *work, ExMotionChannel *channel)
{
    Matrix matrix;
    const Vector4 *translation = &channel->translation;
    const Vector4 *rotation = &channel->rotation;
    float position[4];
    float previousPosition[4];
    Vector4 difference;
    Vector4 direction;
    Vector4 sway;
    Matrix inverse;
    Matrix relative;
    ExGroundQuery ground;
    ExCollisionRequest collision;
    Vector4 firstDirection;
    Vector4 secondDirection;
    unsigned int flags;
    void *collisionTable;
    Actor *owner = work->owner;
    ExMotionChannel *next = channel + 1;
    Matrix *previousMatrices;
    ExHairParameters *parameters;
    ExMotionJointState *state;
    Matrix *currentMatrix;
    int kind;
    int index;
    int jointCount;
    int jointIndex;
    unsigned int baseMatrixIndex;
    float response;
    unsigned short groupStart;

    switch (channel->type) {
    case EX_CHANNEL_CHAIN_BEGIN:
        groupStart = work->current_index;
        baseMatrixIndex = channel->matrix_index;
        work->saved_index = groupStart;
        work->matrix_index = 0;
        MATRIX_translate4s(ExMatrixRows(CopyExMatrix(matrix.elements, ExMotionMatrices(work)[baseMatrixIndex].elements)),
                           translation->x, translation->y, translation->z);
        MATRIX_rotZ4s(matrix.rows, rotation->x);
        MATRIX_rotY4s(matrix.rows, rotation->y);
        MATRIX_rotX4s(matrix.rows, rotation->z);
        jointIndex = work->current_index;
        currentMatrix = CopyExMatrixObject(&ExMotionMatrices(work)[jointIndex], &matrix);
        CopyExMatrix(work->saved_matrix.elements, currentMatrix->elements);
        break;
    case EX_CHANNEL_CHAIN_SOLVE:
        state = &work->channels[work->matrix_index];
        work->matrix_index++;
        state->translation = *translation;
        collisionTable = 0;
        state->rotation = *rotation;
        flags = 0;
        switch (htypebuf[0].kind) {
        case EX_HAIR_TYPE_NONE:
            kind = 0;
            flags = 0;
            collisionTable = 0;
            break;
        case EX_HAIR_TYPE_AUTO:
            kind = GetHairTypeSelection(&flags, &collisionTable,
                                        work);
            break;
        default:
            flags = htypebuf[ibuf].flags;
            kind = htypebuf[ibuf].kind;
            collisionTable = collibuf[0];
            ibuf++;
            break;
        }
        parameters = &hairpara[kind];
        if (owner != 0 && (owner->moved_hair_mode & 1) && owner->moved_hair_sway.w > 0.0f) {
            CopyExMatrix(matrix.elements, ExMotionMatrices(work)[EX_JOINT_HIP].elements);
            MATRIX_translate4s(ExMatrixRows(&matrix), 0.0f, 1.0f, 0.0f);
            sway.x = (matrix.elements[12] - ExMotionMatrices(work)[EX_JOINT_HIP].elements[12]) * owner->moved_hair_sway.w;
            sway.y = (matrix.elements[13] - ExMotionMatrices(work)[EX_JOINT_HIP].elements[13]) * owner->moved_hair_sway.w;
            sway.z = (matrix.elements[14] - ExMotionMatrices(work)[EX_JOINT_HIP].elements[14]) * owner->moved_hair_sway.w;
        } else {
            sway.x = 0.0f;
            sway.y = 0.0f;
            sway.z = 0.0f;
        }
        state = &work->channels[0];
        matrix = work->saved_matrix;
        MATRIX_translate4s(ExMatrixRows(&matrix), state->translation.x,
                           state->translation.y, state->translation.z);
        MATRIX_rotZ4s(matrix.rows, state->rotation.x);
        MATRIX_rotY4s(matrix.rows, state->rotation.y);
        MATRIX_rotX4s(matrix.rows, state->rotation.z);
        {
            int matrixIndex = work->saved_index;
            ExMotionMatrices(work)[matrixIndex + 1] = matrix;
        }
        if (owner == 0 || owner->previous_joint_matrices == 0) {
            previousMatrices = ExMotionMatrices(work);
        } else {
            previousMatrices = owner->previous_joint_matrices;
        }
        index = 1;
        EXM_ResetWave();
        jointCount = work->matrix_index;
        for (; index < jointCount; ) {
            Matrix *current = &ExMotionMatrices(work)[work->saved_index + index + 1];
            Matrix *parent;
            float amount;
            float distance;
            state = &work->channels[index];
            if (work->animation_flags & EX_ANIM_FLAG_RELATIVE_PREVIOUS) {
                int matrixIndex = work->saved_index;
                Matrix *previousGroup = &previousMatrices[matrixIndex];
                xglMatrixInverse(ExMatrixRows(&inverse),
                                 ExMatrixRows(&previousGroup[1]));
                xglMatrixMul(&relative, &inverse, &previousMatrices[work->saved_index + index + 1]);
                matrixIndex = work->saved_index;
                xglMatrixMul(&inverse, &ExMotionMatrices(work)[matrixIndex + 1], &relative);
                position[0] = previousPosition[0] = inverse.elements[12];
                position[1] = previousPosition[1] = inverse.elements[13];
                position[2] = previousPosition[2] = inverse.elements[14];
            } else {
                position[0] = previousPosition[0] = previousMatrices[work->saved_index + index + 1].elements[12];
                position[1] = previousPosition[1] = previousMatrices[work->saved_index + index + 1].elements[13];
                position[2] = previousPosition[2] = previousMatrices[work->saved_index + index + 1].elements[14];
            }
            if (work->animation_flags & EX_ANIM_FLAG_HOLD_PREVIOUS) {
                CopyExMatrix(ExMatrixData(current),
                    ExMatrixData(&previousMatrices[work->saved_index + index]));
            } else {
                parent = current - 1;
                CopyExMatrix(ExMatrixData(current), ExMatrixData(parent));
                MATRIX_translate4s(ExMatrixRows(current), state->translation.x, state->translation.y, state->translation.z);
                MATRIX_rotZ4s(current->rows, state->rotation.x);
                MATRIX_rotY4s(current->rows, state->rotation.y);
                MATRIX_rotX4s(current->rows, state->rotation.z);
                if (!(work->animation_flags & EX_ANIM_FLAG_NO_GRAVITY)) {
                    position[1] -= parameters->gravity;
                }
                if (!(work->animation_flags & EX_ANIM_FLAG_NO_WIND)) {
                    EXM_GetWindPower(position, work, position, &parent->elements[12]);
                }
                amount = (float)index * 0.3f + 0.4f;
                response = parameters->chain_response / ((float)(index - 1) + parameters->chain_response);
                position[0] += sway.x * amount;
                position[1] += sway.y * amount;
                position[2] += sway.z * amount;
                position[0] += (current->elements[12] - position[0]) * parameters->response[0] * response;
                position[1] += (current->elements[13] - position[1]) * parameters->response[1] * response;
                position[2] += (current->elements[14] - position[2]) * parameters->response[2] * response;
                if (flags & EX_HAIR_FLAG_COAT_WIDTH) {
                    int coatIndex;
                    AdjustJrCoatWidth(0.25f, (Vector4 *)position,
                                      (Vector4 *)&ExMotionMatrices(work)[index + 74].elements[12]);
                    coatIndex = 0;
                    switch (work->saved_index) {
                    case 91:
                        coatIndex = index + 80;
                        break;
                    case 96:
                        coatIndex = index + 86;
                        break;
                    }
                    if (index < 4) {
                        Vector4 *coatJoint = (Vector4 *)&ExMotionMatrices(work)[coatIndex].elements[12];
                        AdjustJrCoatWidth(JR_coatAdjust[index - 1], coatJoint, (Vector4 *)position);
                    } else {
                        AdjustJrCoatWidth(JR_coatAdjust[2],
                                          (Vector4 *)&ExMotionMatrices(work)[coatIndex].elements[12],
                                          (Vector4 *)position);
                        AdjustJrCoatWidth(JR_coatAdjust[2],
                                          (Vector4 *)&ExMotionMatrices(work)[coatIndex + 1].elements[12],
                                          (Vector4 *)position);
                    }
                }
                if (!(work->animation_flags & EX_ANIM_FLAG_NO_GROUND)) {
                    if (owner != 0 && owner->translate_y != GROUND_HEIGHT_NONE) {
                        response = owner->translate_y - owner->position.y + 0.07f;
                    } else {
                        UnduParamInit(&ground);
                        ground.attribute_mask = 0x800;
                        response = UnduCheck(&work->root_matrix.elements[12], 0, &ground) - work->root_matrix.elements[13] + 0.07f;
                    }
                    if (position[1] < response) {
                        position[1] = response;
                    }
                }
                difference.x = position[0] - parent->elements[12];
                difference.y = position[1] - parent->elements[13];
                difference.z = position[2] - parent->elements[14];
                distance = __builtin_sqrtf(difference.x * difference.x + difference.y * difference.y + difference.z * difference.z);
                if (distance != 0.0f) {
                    float weightedX;
                    float weightedY;
                    float weightedZ;
                    response = parameters->length_response / distance;
                    weightedX = response * difference.x;
                    weightedY = response * difference.y;
                    weightedZ = response * difference.z;
                    position[0] += weightedX * (state->translation.x * current->elements[3] - distance);
                    position[1] += weightedY * (state->translation.x * current->elements[7] - distance);
                    position[2] += weightedZ * (state->translation.x * current->elements[11] - distance);
                } else {
                    position[1] -= parameters->gravity;
                }
                if (flags & EX_HAIR_FLAG_SKIRT) {
                    EXM_CheckSkirtCollision(work, position,
                                            &ExMotionMatrices(work)[work->saved_index + index].elements[12], flags);
                } else if (collisionTable != 0) {
                    collision.primitives = collisionTable;
                    collision.flags = flags;
                    if (EXM_CheckExCollision(work, position, ExMatrixConstRows(current), &collision)) {
                        position[0] = (position[0] - previousPosition[0]) * 0.9f + collision.correction[0] + previousPosition[0];
                        position[1] += collision.correction[1];
                        position[2] = (position[2] - previousPosition[2]) * 0.9f + collision.correction[2] + previousPosition[2];
                    }
                }
            }
            EXM_SET_JOINT_POSITION(current, position);
            jointCount = work->matrix_index;
            index++;
        }
        if (flags & 1) {
            CopyExMatrix(matrix.elements, ExMotionMatrices(work)[39].elements);
            MATRIX_translate4s(ExMatrixRows(&matrix), 0.0f, 0.0f, 1.0f);
            firstDirection.x = matrix.elements[12] - ExMotionMatrices(work)[39].elements[12];
            firstDirection.y = 0.0001f;
            firstDirection.z = matrix.elements[14] - ExMotionMatrices(work)[39].elements[14];
            CopyExMatrix(matrix.elements, ExMotionMatrices(work)[48].elements);
            MATRIX_translate4s(ExMatrixRows(&matrix), 1.0f, 0.0f, 0.0f);
            secondDirection.x = matrix.elements[12] - ExMotionMatrices(work)[48].elements[12];
            secondDirection.y = 0.0001f;
            secondDirection.z = matrix.elements[14] - ExMotionMatrices(work)[48].elements[14];
            xglVectorNormal(&firstDirection, &firstDirection);
            xglVectorNormal(&secondDirection, &secondDirection);
            xglVectorInner(&response, &firstDirection, &secondDirection);
            response = asinf(response);
            jointCount = work->matrix_index;
        }
        response /= (float)jointCount;
        index = 1;
        if (index < jointCount) {
            do {
                Matrix *current = ExMotionMatrices(work) + work->saved_index + index;
                Vu0SubVector(&direction, &current[1].elements[12], &current->elements[12]);
                OrientExMotionJoint(work, index, &direction);
                if (flags & 1) {
                    MATRIX_rotX4s(ExMotionMatrices(work)[work->saved_index + index].rows,
                                   (float)index * response);
                }
                index++;
            } while (index < work->matrix_index);
        }
        jointIndex = work->current_index;
        break;
    case EX_CHANNEL_CHAIN_JOINT:
        jointIndex = work->current_index;
        state = &work->channels[work->matrix_index];
        work->matrix_index++;
        state->translation = *translation;
        state->rotation = *rotation;
        break;
    case 0:
    case 1:
    case 5:
        jointIndex = work->current_index;
        break;
    default:
        jointIndex = work->current_index;
        break;
    }
    work->current_index = jointIndex + 1;
    return next;
}

void EXM_InitMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 2;
        actor->moved_hair_delay = 0.5f;
        actor->moved_hair_sway.x = 0.0f;
        actor->moved_hair_sway.y = 0.0f;
        actor->moved_hair_sway.z = 0.0f;
        actor->moved_hair_sway.w = 0.0f;
    }
}

void EXM_OnMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 2;
    }
}

void EXM_OffMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 0;
    }
}

void EXM_SetDelayMovedHair(Actor *actor, float delay)
{
    if (actor != 0) {
        actor->moved_hair_delay = delay;
    }
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const float exmHairAnDSize = 1.1000000238418579f;

const float exmHairAnDFootRadius = 0.05000000074505806f;

const float exmHairAbFootRadius = 0.05000000074505806f;

const float exmHairAsBackSize = 0.80000001192092896f;

const float exmHairAsFootRadius = 0.05000000074505806f;

const float exmHairBrDSkirtColliSize = 1.2999999523162842f;

const float exmHairBrDSkirtColliFootRad = 0.05000000074505806f;

const float exmHairBo2SkirtColliSize = 1.3999999761581421f;

const float exmHairBo2SkirtColliFootRad = 0.05000000074505806f;

const float exmHairClSize = 1.1000000238418579f;

const float exmHairClFootRadius = 0.070000000298023224f;

const float exmHairESize = 1.1000000238418579f;

const float exmHairEFootRadius = 0.070000000298023224f;

const float exmHairFrSkirtColliSize = 1.3999999761581421f;

const float exmHairFrSkirtColliSizeB = 1.1000000238418579f;

const float exmHairFrSkirtColliFootRad = 0.05000000074505806f;

const float exmHairGiSkirtColliSize = 1.3999999761581421f;

const float exmHairGiSkirtColliFootRad = 0.05000000074505806f;

const float exmHairHeSkirtColliSize = 1.7000000476837158f;

const float exmHairHeSkirtColliSizeB = 1.1000000238418579f;

const float exmHairHeSkirtColliFootRad = 0.10000000149011612f;

const float exmHairISkirtColliSize = 1.3999999761581421f;

const float exmHairISkirtColliSizeB = 0.80000001192092896f;

const float exmHairISkirtColliFootRad = 0.05000000074505806f;

const float exmHairJr1SkirtColliSize = 1.3999999761581421f;

const float exmHairJr1SkirtColliFootRad = 0.05000000074505806f;

const float exmHairKeNSkirtColliSizeB = 0.40000000596046448f;

const float exmHairKeNSkirtColliSizeS = 0.60000002384185791f;

const float exmHairKeNSkirtColliFootRad = 0.05000000074505806f;

const float exmHairMoHmSkirtColliSize = 1.1000000238418579f;

const float exmHairMoHmSkirtColliFootRad = 0.059999998658895493f;

const float exmHairObSkirtColliSize = 1.2999999523162842f;

const float exmHairObSkirtColliFootRad = 0.05000000074505806f;

const float exmHairShioCSkirtColliSize = 1.2999999523162842f;

const float exmHairShioCSkirtColliFootRad = 0.05000000074505806f;

const float exmHairSeSkirtColliSize = 1.3999999761581421f;

const float exmHairSeSkirtColliFootRad = 0.05000000074505806f;

const float exmHairVSkirtColliSizeB = 0.40000000596046448f;

const float exmHairVSkirtColliSizeS = 0.60000002384185791f;

const float exmHairVSkirtColliFootRad = 0.05000000074505806f;

const float exmHairWSkirtColliSizeB = 0.80000001192092896f;

const float exmHairWSkirtColliFootRad = 0.05000000074505806f;

const float exmHairYSkirtColliFootRad = 0.05000000074505806f;

static void * collibuf[1] = { 0 };

static int ibuf = 0;

static int startjnt = EX_START_JOINT_DEFAULT;
