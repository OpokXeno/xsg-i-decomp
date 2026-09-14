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

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_get);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_info);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_transMPack);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_mpackGetInterest);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_transMPack2);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_viewMPack);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_setTranslate);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_setFov);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_setRoll);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_setRotate);

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

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_transSPL);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_rotateSPL);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_viewSPL);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_transCNS);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_viewCNS);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_rollSPL);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_fovSPL);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_update);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_init);

INCLUDE_ASM("asm/main/nonmatchings/tcamera", TCAMERA_setFCurve);
