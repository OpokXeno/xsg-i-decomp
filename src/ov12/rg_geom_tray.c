/*
 * OV12 original TU 56: 0x00a2dd58..0x00a2e698 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_tray.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"

/* Defined at the end of this file. */
extern const char geomTrayAssertTrayNotNil[];
extern const char geomTraySourcePath[];
extern const char geomTrayAssertCheckNotNil[];
extern const char geomTrayAssertResultNotNil[];

/* Clip-direction bits of _CalcIntersect: which tray bound already produced a
 * contact on that axis. */
#define RG_GEOM_TRAY_HIT_Z_LOW 0x1
#define RG_GEOM_TRAY_HIT_Z_HIGH 0x2
#define RG_GEOM_TRAY_HIT_X_LOW 0x4
#define RG_GEOM_TRAY_HIT_X_HIGH 0x8
#define RG_GEOM_TRAY_HIT_FLOOR 0x10

/* Movement along an axis below this threshold does not cross the bound. */
#define RG_GEOM_TRAY_MOVE_EPSILON 0.001f

/* Default width and height of a new tray. */
#define RG_GEOM_TRAY_DEFAULT_SIZE 100.0f

static void _InitRgGeomTray(RgGeom *geom)
{
    RgGeomTray *tray = (RgGeomTray *)geom;

    if (geom == 0) {
        assert_prog(geomTrayAssertTrayNotNil, geomTraySourcePath, 19);
    }
    RgGeomInit(geom);
    RgGeomSetType(geom, RG_GEOM_TYPE_TRAY);
    tray->width = RG_GEOM_TRAY_DEFAULT_SIZE;
    tray->height = RG_GEOM_TRAY_DEFAULT_SIZE;
    XrgUnitMatrix(tray->local);
    XrgUnitMatrix(tray->inverseLocal);
}

void InitRgGeomTray(RgGeom *geom)
{
    _InitRgGeomTray(geom);
}

RgGeom *CreateRgGeomTray(void)
{
    RgGeomTray *tray;

    tray = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomTray),
                       geomTraySourcePath, 37);
    _InitRgGeomTray(&tray->geom);
    return &tray->geom;
}

void RgGeomTraySetSize(void *tray, float width, float height)
{
    RgGeomTray *pTray = tray;

    if (pTray == 0) {
        assert_prog(geomTrayAssertTrayNotNil, geomTraySourcePath, 48);
    }
    if (width <= 0.0f) {
        width = 0.0f;
    }
    if (height <= 0.0f) {
        height = 0.0f;
    }
    pTray->width = width;
    pTray->height = height;
}

void RgGeomTraySetLocal(void *tray, RgMatrix source)
{
    RgGeomTray *pTray = tray;

    XrgCopyMatrix(pTray->local, source);
    XrgInvMatrix(pTray->inverseLocal, source);
}

void RgGeomTrayGetLocal(void *tray, RgMatrix destination)
{
    RgGeomTray *pTray = tray;

    XrgCopyMatrix(destination, pTray->local);
}

/*
 * Clip the old-to-current movement against the tray's two horizontal bounds
 * and its floor.  Each axis contributes at most one contact normal; when a
 * boundary crossing lies outside the movement segment, the endpoint is
 * clamped and the contact point is moved from the ball center to the surface.
 */
static int _CalcIntersect(RgVector clipped_position, RgVector last_contact,
                          RgVector start_position, RgVector end_position,
                          RgVector normals[3], float width, float height,
                          float radius)
{
    struct RgGeomTrayMovement {
        float x;
        float y;
        float z;
    } movement;
    RgVector current_position;
    float boundary;
    float left_boundary;
    float right_boundary;
    float time;
    unsigned int hit_axes;
    int hit_count;

    hit_axes = 0;
    hit_count = 0;
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        : : "r"(current_position), "r"(end_position) : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        : : "r"(last_contact), "r"(current_position) : "memory");

    XrgSubVectorXYZ(&movement.x, current_position, start_position);
    if (movement.z < -RG_GEOM_TRAY_MOVE_EPSILON) {
        boundary = radius - height;
        time = (boundary - start_position[2]) / movement.z;
        if (time >= 0.0f && time <= 1.0f) {
            current_position[2] = boundary;
            last_contact[0] = start_position[0] + time * movement.x;
            last_contact[1] = start_position[1] + time * movement.y;
            last_contact[2] = start_position[2] + time * movement.z;
            XrgSetVectorXYZ(normals[hit_count], 0.0f, 0.0f, 1.0f);
            hit_count++;
            hit_axes |= RG_GEOM_TRAY_HIT_Z_LOW;
        }
    }

    if ((hit_axes & RG_GEOM_TRAY_HIT_Z_LOW) == 0 && current_position[2] < radius - height) {
        current_position[2] = radius - height;
        __asm__ __volatile__(
            "lqc2 vf31, 0(%1)\n\t"
            "sqc2 vf31, 0(%0)\n\t"
            : : "r"(last_contact), "r"(current_position) : "memory");
        last_contact[2] -= radius;
        XrgSetVectorXYZ(normals[hit_count], 0.0f, 0.0f, 1.0f);
        hit_count++;
        hit_axes |= RG_GEOM_TRAY_HIT_Z_LOW;
    }

    XrgSubVectorXYZ(&movement.x, current_position, start_position);
    if (movement.z > RG_GEOM_TRAY_MOVE_EPSILON) {
        boundary = height - radius;
        time = (boundary - start_position[2]) / movement.z;
        if (time >= 0.0f && time <= 1.0f) {
            current_position[2] = boundary;
            last_contact[0] = start_position[0] + time * movement.x;
            last_contact[1] = start_position[1] + time * movement.y;
            last_contact[2] = start_position[2] + time * movement.z;
            XrgSetVectorXYZ(normals[hit_count], 0.0f, 0.0f, -1.0f);
            hit_count++;
            hit_axes |= RG_GEOM_TRAY_HIT_Z_HIGH;
        }
    }
    if ((hit_axes & RG_GEOM_TRAY_HIT_Z_HIGH) == 0 && current_position[2] > height - radius) {
        current_position[2] = height - radius;
        __asm__ __volatile__(
            "lqc2 vf31, 0(%1)\n\t"
            "sqc2 vf31, 0(%0)\n\t"
            : : "r"(last_contact), "r"(current_position) : "memory");
        last_contact[2] += radius;
        XrgSetVectorXYZ(normals[hit_count], 0.0f, 0.0f, -1.0f);
        hit_count++;
        hit_axes |= RG_GEOM_TRAY_HIT_Z_HIGH;
    }

    XrgSubVectorXYZ(&movement.x, current_position, start_position);
    if (movement.x < -RG_GEOM_TRAY_MOVE_EPSILON) {
        left_boundary = radius - width;
        time = (left_boundary - start_position[0]) / movement.x;
        if (time >= 0.0f && time <= 1.0f) {
            current_position[0] = left_boundary;
            last_contact[0] = start_position[0] + time * movement.x;
            last_contact[1] = start_position[1] + time * movement.y;
            last_contact[2] = start_position[2] + time * movement.z;
            XrgSetVectorXYZ(normals[hit_count], 1.0f, 0.0f, 0.0f);
            hit_count++;
            hit_axes |= RG_GEOM_TRAY_HIT_X_LOW;
        }
    }
    if ((hit_axes & RG_GEOM_TRAY_HIT_X_LOW) == 0 && current_position[0] < radius - width) {
        current_position[0] = radius - width;
        __asm__ __volatile__(
            "lqc2 vf31, 0(%1)\n\t"
            "sqc2 vf31, 0(%0)\n\t"
            : : "r"(last_contact), "r"(current_position) : "memory");
        last_contact[0] -= radius;
        XrgSetVectorXYZ(normals[hit_count], 1.0f, 0.0f, 0.0f);
        hit_count++;
        hit_axes |= RG_GEOM_TRAY_HIT_X_LOW;
    }

    XrgSubVectorXYZ(&movement.x, current_position, start_position);
    if (movement.x > RG_GEOM_TRAY_MOVE_EPSILON) {
        right_boundary = width - radius;
        time = (right_boundary - start_position[0]) / movement.x;
        if (time >= 0.0f && time <= 1.0f) {
            current_position[0] = right_boundary;
            last_contact[0] = start_position[0] + time * movement.x;
            last_contact[1] = start_position[1] + time * movement.y;
            last_contact[2] = start_position[2] + time * movement.z;
            XrgSetVectorXYZ(normals[hit_count], -1.0f, 0.0f, 0.0f);
            hit_count++;
            hit_axes |= RG_GEOM_TRAY_HIT_X_HIGH;
        }
    }
    if ((hit_axes & RG_GEOM_TRAY_HIT_X_HIGH) == 0 && current_position[0] > width - radius) {
        current_position[0] = width - radius;
        __asm__ __volatile__(
            "lqc2 vf31, 0(%1)\n\t"
            "sqc2 vf31, 0(%0)\n\t"
            : : "r"(last_contact), "r"(current_position) : "memory");
        last_contact[0] += radius;
        XrgSetVectorXYZ(normals[hit_count], -1.0f, 0.0f, 0.0f);
        hit_count++;
        hit_axes |= RG_GEOM_TRAY_HIT_X_HIGH;
    }

    if ((hit_axes & RG_GEOM_TRAY_HIT_FLOOR) == 0 && current_position[1] < radius) {
        current_position[1] = radius;
        __asm__ __volatile__(
            "lqc2 vf31, 0(%1)\n\t"
            "sqc2 vf31, 0(%0)\n\t"
            : : "r"(last_contact), "r"(current_position) : "memory");
        last_contact[1] = 0.0f;
        XrgSetVectorXYZ(normals[hit_count], 0.0f, 1.0f, 0.0f);
        hit_count++;
    }

    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        : : "r"(clipped_position), "r"(current_position) : "memory");
    return hit_count;
}

/*
 * Sweeps the ball from its previous position to its current one through the
 * tray.  Both path endpoints are taken into the tray's local space with the
 * inverse-local matrix and clipped by _CalcIntersect; the clipped position,
 * the last contact point and each contact normal are then brought back to
 * world space with the local matrix.  Returns non-zero when the ball touched
 * the tray.
 */
int RgGeomTrayCheckBall(RgGeomTray *tray, RgGeomBall *ball,
                        RgGeomTrayContact *contact)
{
    RgVector clipped_position;
    RgVector last_contact;
    RgVector start_position;
    RgVector end_position;
    RgVector spare_position;
    unsigned int i;

    if (tray == 0) {
        assert_prog(geomTrayAssertTrayNotNil, geomTraySourcePath, 226);
    }
    if (ball == 0) {
        assert_prog(geomTrayAssertCheckNotNil, geomTraySourcePath, 227);
    }
    if (contact == 0) {
        assert_prog(geomTrayAssertResultNotNil, geomTraySourcePath, 228);
    }

    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(start_position), "r"(ball->point.oldPosition) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(end_position), "r"(ball->point.position) : "memory");

    __asm__ __volatile__(
        "lqc2 $vf2, 0(%0)\n\t"
        "lqc2 $vf3, 0(%1)\n\t"
        "lqc2 $vf4, 16(%1)\n\t"
        "lqc2 $vf5, 32(%1)\n\t"
        "lqc2 $vf6, 48(%1)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(start_position), "r"(tray->inverseLocal) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf2, 0(%0)\n\t"
        "lqc2 $vf3, 0(%1)\n\t"
        "lqc2 $vf4, 16(%1)\n\t"
        "lqc2 $vf5, 32(%1)\n\t"
        "lqc2 $vf6, 48(%1)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(end_position), "r"(tray->inverseLocal) : "memory");

    contact->hit_count = _CalcIntersect(clipped_position, last_contact,
                                        start_position, end_position,
                                        contact->normals, tray->width,
                                        tray->height, ball->radius);

    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(contact->last_contact), "r"(last_contact),
            "r"(tray->local) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(contact->clipped_position), "r"(clipped_position),
            "r"(tray->local) : "memory");

    for (i = 0; i < contact->hit_count; i++) {
        contact->normals[i][3] = 0.0f;
        __asm__ __volatile__(
            "lqc2 $vf2, 0(%1)\n\t"
            "lqc2 $vf3, 0(%2)\n\t"
            "lqc2 $vf4, 16(%2)\n\t"
            "lqc2 $vf5, 32(%2)\n\t"
            "lqc2 $vf6, 48(%2)\n\t"
            "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
            "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
            "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
            "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
            "sqc2 $vf2, 0(%0)\n\t"
            : : "r"(contact->normals[i]), "r"(contact->normals[i]),
                "r"(tray->local) : "memory");
    }

    return contact->hit_count != 0;
}

const char geomTrayAssertTrayNotNil[16] = "pTray != NIL";

const char geomTraySourcePath[24] = "../rg_geom_tray.euc.c";
