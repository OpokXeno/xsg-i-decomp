#include "common.h"
#include "shared.h"

extern float xglAtan2(float x, float y);
extern float xglSin(float angle);
extern float xglCos(float angle);

/*
 * TCamera is opaque: only the two quadwords TCAMERA_setView touches are
 * evidenced (config/units/math-main-w3-00308bf8.json) -- rotation at +0xa0
 * (x=pitch, y=yaw, z=roll) and translation at +0xd0. The rest of the record
 * is not recovered, so it stays reached through named offsets instead of
 * invented struct fields (docs/naming.md).
 */
typedef struct TCamera TCamera;
#define TCAMERA_ROTATION(camera)    ((Vector4 *)((u8 *)(camera) + 0xa0))
#define TCAMERA_TRANSLATION(camera) ((Vector4 *)((u8 *)(camera) + 0xd0))

/*
 * TCAMERA_setFov/TCAMERA_fovSPL (this TU, VA 0x00308b58/0x00308f60) evidence
 * a field-of-view scalar at +0x94.
 */
#define TCAMERA_FOV(camera)         (*(float *)((u8 *)(camera) + 0x94))

/* Every accepted call site of SPL_getValueXYZ (near_dir.c, spl.h) spells this
 * prototype; SPL_getValue has no accepted definition yet, so its second
 * parameter is named from the one value every recovered call site here
 * passes it, literal 0, and its third from the frame argument TCAMERA_update
 * (VA 0x0030931c, 0x00309348) loads into $f12 before calling TCAMERA_rollSPL/
 * TCAMERA_fovSPL, which forward it untouched into SPL_getValue, whose body
 * reads $f12 (c.le.s at 0x0030c0d8). */
extern void SPL_getValueXYZ(float *destination, void *spline, float frame);
extern float SPL_getValue(void *spline, int index, float frame);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_get);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_info);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_transMPack);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_mpackGetInterest);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_transMPack2);

/*
 * TCAMERA_viewMPack: the degrees-of-freedom "MPack" table entry TCAMERA_update
 * (VA 0x00309278) calls for view control mode 0x13, with the same (camera,
 * frame) shape as TCAMERA_transMPack's call site. The body reads neither
 * argument and writes no memory (original bytes are `jr $31; nop`).
 */
void TCAMERA_viewMPack(TCamera *camera, float frame)
{
}

void TCAMERA_setTranslate(TCamera *camera, float x, float y, float z)
{
    Vector4 *translation = TCAMERA_TRANSLATION(camera);

    translation->x = x;
    translation->y = y;
    translation->z = z;
}

/*
 * Degrees-to-radians conversion factor (pi, bits 0x40490fdb) each angle
 * setter below multiplies by. The scaffold still owns .lit4 (data_ownership
 * in config/tu-build.json) and did not merge these into one pool entry: every
 * setter keeps its own copy, at its own .lit4 address, so each is referenced
 * through its own scaffold symbol rather than a shared one.
 */
extern const float D_004D8490;
extern const float D_004D8494;
extern const float D_004D8498;
extern const float D_004D84A0;
extern const float D_004D84A4;

void TCAMERA_setFov(TCamera *camera, float degrees)
{
    TCAMERA_FOV(camera) = degrees / 180.0f * D_004D8490;
}

void TCAMERA_setRoll(TCamera *camera, float degrees)
{
    TCAMERA_ROTATION(camera)->z = degrees / 180.0f * D_004D8494;
}

void TCAMERA_setRotate(TCamera *camera, float pitchDegrees, float yawDegrees, float rollDegrees)
{
    Vector4 *rotation = TCAMERA_ROTATION(camera);

    rotation->x = pitchDegrees / 180.0f * D_004D8498;
    rotation->y = yawDegrees / 180.0f * D_004D8498;
    rotation->z = rollDegrees / 180.0f * D_004D8498;
}

/*
 * TCAMERA_setView: point the camera at the supplied world-space point.
 *
 * The three scalar inputs are homed onto the stack (home[]); the constrained
 * VU0 macro-mode block below loads that homed point into vf3 and the
 * camera's translation quadword (TCAMERA_TRANSLATION) into vf2, subtracts
 * the xyz lanes (vf2.w is preserved) and stores the resulting direction
 * vector to dir. The scalar tail derives yaw = atan2(dx, dz) into
 * rotation->y, then pitch = -atan2(dy, dx*sin(yaw) + dz*cos(yaw)) into
 * rotation->x; roll (rotation->z) is left untouched.
 *
 * TCamera's full layout is not recovered: only the two quadwords this
 * function reads/writes are evidenced (config/units/math-main-w3-00308bf8.json),
 * so the object stays opaque and both are reached through the named offset
 * macros above rather than invented struct fields.
 */
void TCAMERA_setView(TCamera *camera, float x, float y, float z)
{
    float home[4];
    Vector4 dir;
    Vector4 *rotation;
    Vector4 *translation;
    float yaw;
    float sine;
    float cosine;
    float denom;

    translation = TCAMERA_TRANSLATION(camera);
    rotation = TCAMERA_ROTATION(camera);
    home[0] = x;
    home[1] = y;
    home[2] = z;
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(home), "r"(translation), "r"(&dir)
        : "memory");
    yaw = xglAtan2(dir.x, dir.z);
    rotation->y = yaw;
    sine = xglSin(yaw);
    cosine = xglCos(rotation->y);
    denom = dir.x * sine + dir.z * cosine;
    rotation->x = -xglAtan2(dir.y, denom);
}

/*
 * TCAMERA_transSPL: tail-calls SPL_getValueXYZ with the translation quadword
 * as the destination; `spline` and `frame` reach SPL_getValueXYZ unread and
 * unmodified (original bytes only touch $4, then `j SPL_getValueXYZ`), so
 * they are this function's own second and third parameters.
 */
void TCAMERA_transSPL(TCamera *camera, void *spline, float frame)
{
    SPL_getValueXYZ(&TCAMERA_TRANSLATION(camera)->x, spline, frame);
}

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_rotateSPL);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_viewSPL);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_transCNS);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_viewCNS);

/*
 * TCAMERA_rollSPL/TCAMERA_fovSPL: called by TCAMERA_update with `spline`
 * forwarded straight into SPL_getValue's own first parameter (original bytes
 * move the incoming $5 into $4 before the call); every recovered call site
 * passes SPL_getValue's second parameter as the literal 0. `frame` is loaded
 * into $f12 by the caller (TCAMERA_update, VA 0x0030931c/0x00309348) before
 * the call and is neither read nor modified here, so it reaches SPL_getValue
 * untouched as its third parameter, matching TCAMERA_transSPL's shape.
 */
void TCAMERA_rollSPL(TCamera *camera, void *spline, float frame)
{
    Vector4 *rotation = TCAMERA_ROTATION(camera);

    rotation->z = SPL_getValue(spline, 0, frame) / 180.0f * D_004D84A0;
}

void TCAMERA_fovSPL(TCamera *camera, void *spline, float frame)
{
    TCAMERA_FOV(camera) = SPL_getValue(spline, 0, frame) / 180.0f * D_004D84A4;
}

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_update);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_init);

/*
 * TCAMERA_setFCurve: no call site references it in this build (original
 * bytes are `jr $31; nop`, reading no argument register); kept parameterless
 * like the accepted no-op leaves of this shape (e.g. xglDmaInitial).
 */
void TCAMERA_setFCurve(void)
{
}
