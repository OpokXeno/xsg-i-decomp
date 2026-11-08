#include "common.h"
#include "shared.h"
#include "align.h"
#include "main/matrix_print.h"

/* The CUR_MATRIX_* helpers are defined in matrix_print.c. Their angle
 * arguments point at float values stored as int bit patterns. */
extern void CUR_MATRIX_Unit(void);
void alignXYAxis(void) {

}

/*
 * Gives the yaw/pitch Euler pair that turns the local X axis onto direction.
 * The VU0 block leaves the heading's sin/cos in components.y/x and the
 * pitch's sin/cos in components.z/w; euler->x is 0 and euler->w is 1.
 */
void alignXAxisEuler(const Vector4 *direction, Vector4 *euler) {
    Vector4 components;
    float pitch;

    __asm__ __volatile__(
        "lqc2 vf1, 0(%1)\n\t"
        "vsubw.y vf11, vf0, vf0w\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vmul.xyz vf2, vf1, vf1\n\t"
        "vmul.y vf1, vf1, vf11\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vaddy.x vf3, vf2, vf2y\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf3x\n\t"
        "vaddz.x vf4, vf3, vf2z\n\t"
        "vwaitq\n\t"
        "vaddq.x vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf4x\n\t"
        "vmulx.xy vf10, vf1, vf5x\n\t"
        "vwaitq\n\t"
        "vaddq.w vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vdiv Q, vf0w, vf5x\n\t"
        "vmulw.z vf10, vf1, vf5w\n\t"
        "vwaitq\n\t"
        "vmulq.w vf10, vf5, Q\n\t"
        "sqc2 vf10, 0(%0)"
        :
        : "r"(&components), "r"(direction)
        : "memory");

    euler->w = 1.0f;
    euler->z = xglAtan2(components.y, components.x);
    pitch = xglAtan2(components.z, components.w);
    euler->x = 0.0f;
    euler->y = pitch;
}

/* Each angle is written as a float (atan2) and read back as its bit pattern
 * by the CUR_MATRIX_R*4s helpers. */
typedef union {
    float value[3];
    int bits[3];
} AlignRotationAngles;

/* Writes into matrix the rotation (Y, then Z) that turns the X axis onto
 * direction. */
void alignXAxis(const Vector4 *direction, Matrix4 matrix) {
    AlignRotationAngles rotation_angles;
    Vector4 components;

    __asm__ __volatile__(
        "lqc2 vf1, 0(%1)\n\t"
        "vsubw.y vf11, vf0, vf0w\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vmul.xyz vf2, vf1, vf1\n\t"
        "vmul.y vf1, vf1, vf11\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vaddy.x vf3, vf2, vf2y\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf3x\n\t"
        "vaddz.x vf4, vf3, vf2z\n\t"
        "vwaitq\n\t"
        "vaddq.x vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf4x\n\t"
        "vmulx.xy vf10, vf1, vf5x\n\t"
        "vwaitq\n\t"
        "vaddq.w vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vdiv Q, vf0w, vf5x\n\t"
        "vmulw.z vf10, vf1, vf5w\n\t"
        "vwaitq\n\t"
        "vmulq.w vf10, vf5, Q\n\t"
        "sqc2 vf10, 0(%0)"
        :
        : "r"(&components), "r"(direction)
        : "memory");

    rotation_angles.value[0] = 0.0f;
    rotation_angles.value[1] = xglAtan2(components.z, components.w);
    rotation_angles.value[2] = xglAtan2(components.y, components.x);
    CUR_MATRIX_Unit();
    CUR_MATRIX_Ry4s(&rotation_angles.bits[1]);
    CUR_MATRIX_Rz4s(&rotation_angles.bits[2]);
    CUR_MATRIX_Get(matrix);
}

/* Writes into matrix the rotation (X, then Z) that turns the Y axis onto
 * direction. */
void alignYAxis(const Vector4 *direction, Matrix4 matrix) {
    AlignRotationAngles rotation_angles;
    Vector4 components;

    __asm__ __volatile__(
        "lqc2 vf1, 0(%1)\n\t"
        "vsubw.z vf11, vf0, vf0w\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vmul.xyz vf2, vf1, vf1\n\t"
        "vmul.z vf1, vf1, vf11\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vaddy.z vf3, vf2, vf2y\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf3z\n\t"
        "vaddz.x vf4, vf2, vf3z\n\t"
        "vwaitq\n\t"
        "vaddq.x vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf4x\n\t"
        "vmulx.yz vf10, vf1, vf5x\n\t"
        "vwaitq\n\t"
        "vaddq.w vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vdiv Q, vf0w, vf5x\n\t"
        "vmulw.x vf10, vf1, vf5w\n\t"
        "vwaitq\n\t"
        "vmulq.w vf10, vf5, Q\n\t"
        "sqc2 vf10, 0(%0)"
        :
        : "r"(&components), "r"(direction)
        : "memory");

    rotation_angles.value[0] = xglAtan2(components.z, components.y);
    rotation_angles.value[1] = 0.0f;
    rotation_angles.value[2] = xglAtan2(components.x, components.w);
    CUR_MATRIX_Unit();
    CUR_MATRIX_Rx4s(&rotation_angles.bits[0]);
    CUR_MATRIX_Rz4s(&rotation_angles.bits[2]);
    CUR_MATRIX_Get(matrix);
}

/* Writes into matrix the rotation (X, then Y) that turns the Z axis onto
 * direction. */
void alignZAxis(const Vector4 *direction, Matrix4 matrix) {
    AlignRotationAngles rotation_angles;
    Vector4 components;

    __asm__ __volatile__(
        "lqc2 vf1, 0(%1)\n\t"
        "vsubw.x vf11, vf0, vf0w\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vmul.xyz vf2, vf1, vf1\n\t"
        "vmul.x vf1, vf1, vf11\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vaddy.z vf3, vf2, vf2y\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf3z\n\t"
        "vaddz.x vf4, vf2, vf3z\n\t"
        "vwaitq\n\t"
        "vaddq.x vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf4x\n\t"
        "vmulx.yz vf10, vf1, vf5x\n\t"
        "vwaitq\n\t"
        "vaddq.w vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vdiv Q, vf0w, vf5x\n\t"
        "vmulw.x vf10, vf1, vf5w\n\t"
        "vwaitq\n\t"
        "vmulq.w vf10, vf5, Q\n\t"
        "sqc2 vf10, 0(%0)"
        :
        : "r"(&components), "r"(direction)
        : "memory");

    rotation_angles.value[0] = xglAtan2(components.y, components.z);
    rotation_angles.value[1] = xglAtan2(components.x, components.w);
    rotation_angles.value[2] = 0.0f;
    CUR_MATRIX_Unit();
    CUR_MATRIX_Rx4s(&rotation_angles.bits[0]);
    CUR_MATRIX_Ry4s(&rotation_angles.bits[1]);
    CUR_MATRIX_Get(matrix);
}
