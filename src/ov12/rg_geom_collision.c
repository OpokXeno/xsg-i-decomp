/*
 * OV12 original TU 58: 0x00a2f650..0x00a303a8 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_colidata.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_collision.h"
#include "ov12/rg_geom_pillar.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_geom_poly.h"
#include "ov12/rg_geom_tray.h"
#include "ov12/xrg_rand_int.h"

/* Defined here: this TU's own .rodata. */
const char geomCollisionSourcePath[32] = "../rg_geom_collision.euc.c";

const char geomCollisionAssertArgNotNil[16] = "pArg != NIL";

/* Declared here: defined elsewhere in the original image. */
extern const char geomCollisionAssertRobot1NotNil[]; /* "pRobot1 != NIL" */
extern const char geomCollisionAssertPoly2NotNil[]; /* "pPoly2 != NIL" */

/*
 * A hit-normal scratch record _BallVsPillar builds beside the contact point:
 * a cleared RgVector whose Y lane is then set to 1.0 (main 0x00a2fe64/
 * 0x00a2fe6c), followed by the other collider's own pointer (main
 * 0x00a2fe78/0x00a2fe80). Neither of this function's own calls reads it
 * back, so its consumer is outside this allocation's evidence.
 */
typedef struct {
    RgVector normal;      /* +0x00 */
    void *otherObject;    /* +0x10 */
    unsigned char unmodeled_14[0xC];
} RgGeomColiHit;

/* The pair-dispatch table, scaffold data at 0x00a50090: handlers for the
 * [typeA][typeB] geometry pairs. */
typedef int (*RgGeomCollisionHandler)(RgGeomCollisionPair *pPair);
static RgGeomCollisionHandler s_apJobFuncTbl[RG_GEOM_TYPE_COUNT][RG_GEOM_TYPE_COUNT];

static int _BallVsBall(RgGeomCollisionPair *pArg)
{
    int result;
    RgGeomBall *ball1;
    RgGeomBall *ball2;
    RgGeomBallContact check;
    RgVector shift;
    RgVector newPosition;
    RgVector contact;
    RgGeomColiHit hit;
    int type1;
    int type2;
    float weight1;
    float weight2;
    float totalWeight;
    unsigned int bothStatus;
    unsigned int ball2Status;

    ball1 = (RgGeomBall *) pArg->pGeom1;
    ball2 = (RgGeomBall *) pArg->pGeom2;
    type1 = RgGeomGetType(&ball1->point.geom);
    type2 = RgGeomGetType(&ball2->point.geom);
    if (type1 == RG_GEOM_TYPE_ROBOT && type2 == type1) {
        result = RgGeomBallCheckBallDist(ball1, ball2, &check);
    } else if (type1 != type2 && (type1 == RG_GEOM_TYPE_ROBOT || type2 == RG_GEOM_TYPE_ROBOT)) {
        result = RgGeomBallCheckBall(ball1, ball2, &check);
    } else {
        result = RgGeomBallCheckBallDist(ball1, ball2, &check);
    }
    if (result != 0) {
        ball2Status = ball2->point.geom.status;
        bothStatus = ball1->point.geom.status & ball2Status;
        weight1 = RgGeomPointGetWeight(&ball1->point);
        bothStatus &= RG_GEOM_STATUS_PUSH_OUT;
        weight2 = RgGeomPointGetWeight(&ball2->point);
        if (bothStatus != 0) {
            totalWeight = weight1 + weight2;
            XrgScaleVector(shift, check.penetration, weight2 / totalWeight);
            XrgAddVector(newPosition, check.position1, shift);
            RgGeomPointSetPos(&ball1->point, newPosition);
            XrgScaleVector(shift, check.penetration, weight1 / totalWeight);
            XrgSubVector(newPosition, check.position2, shift);
            RgGeomPointSetPos(&ball2->point, newPosition);
        }
        __asm__ __volatile__(
            "lqc2 vf30, 0(%0)\n\t"
            "lqc2 vf31, 0(%1)\n\t"
            "vadd.xyzw vf30, vf30, vf31\n\t"
            "sqc2 vf30, 0(%2)\n\t"
            : : "r"(check.position1), "r"(check.position2), "r"(contact) : "memory");
        contact[0] *= 0.5f;
        contact[1] *= 0.5f;
        contact[2] *= 0.5f;
        contact[3] *= 0.5f;
        XrgCopyVector(hit.normal, check.penetration);
        if (pArg->callback1 != 0) {
            hit.otherObject = ball2;
            pArg->callback1(ball1, contact);
        }
        if (pArg->callback2 != 0) {
            XrgNegateVector(hit.normal, hit.normal);
            hit.otherObject = ball1;
            pArg->callback2(ball2, contact);
        }
        return 1;
    }
    return 0;
}

static int _BallVsPoly(RgGeomCollisionPair *pArg)
{
    RgGeomBall *ball;
    RgGeomPoly *poly;
    RgColiSegment ray;
    RgColiSegmentHit rayHit;
    RgVector localOldPosition;
    RgVector localPosition;
    RgVector contact;
    float *contactPtr;
    RgGeomColiHit hit;

    ball = (RgGeomBall *) pArg->pGeom1;
    poly = (RgGeomPoly *) pArg->pGeom2;
    __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(localPosition), "r"(ball->point.position) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%2)" : : "r"(localOldPosition), "r"(ball->point.oldPosition), "r"(localOldPosition) : "memory");
    localPosition[3] = 1.0f;
    localOldPosition[3] = 1.0f;
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
        : : "r"(localPosition), "r"(poly->inverseLocal) : "memory");
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
        : : "r"(localOldPosition), "r"(poly->inverseLocal) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(ball->point.oldPosition) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(ray.m_start) : "memory");
    __asm__ __volatile__(
        "lqc2 vf30, 0(%0)\n"
        "lqc2 vf31, 0(%1)\n"
        "vsub.xyzw vf30, vf30, vf31\n"
        "sqc2 vf30, 0(%2)\n"
        : : "r"(ball->point.position), "r"(ball->point.oldPosition), "r"(ray.m_vector) : "memory");
    if (RgGeomPolyCheckRay(&poly->geom, &ray, &rayHit) == 0) {
        return 0;
    }
    contact[0] = ray.m_vector[0] * rayHit.m_fraction;
    contact[1] = ray.m_vector[1] * rayHit.m_fraction;
    contact[2] = ray.m_vector[2] * rayHit.m_fraction;
    contact[3] = ray.m_vector[3] * rayHit.m_fraction;
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf30, 0(%0)" : : "r"(contactPtr) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(ray.m_start) : "memory");
    __asm__ __volatile__("vadd.xyzw vf30, vf30, vf31" : : : "memory");
    __asm__ __volatile__("sqc2 vf30, 0(%0)" : : "r"(contactPtr) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(hit.normal), "r"(rayHit.m_plane) : "memory");
    hit.normal[3] = 1.0f;
    if (pArg->callback1 != 0) {
        hit.otherObject = poly;
        pArg->callback1(ball, contactPtr);
    }
    if (pArg->callback2 != 0) {
        XrgNegateVector(hit.normal, hit.normal);
        hit.otherObject = ball;
        pArg->callback2(poly, contactPtr);
    }
    return 1;
}

static void _PolyVsBall(RgGeomCollisionPair *pArg)
{
    RgGeomCollisionPair reordered;
    RgGeom *geom1;
    RgGeom *geom2;
    RgGeomColiCallback callback1;
    RgGeomColiCallback callback2;

    geom2 = pArg->pGeom2;
    geom1 = pArg->pGeom1;
    callback2 = pArg->callback2;
    callback1 = pArg->callback1;
    reordered.pGeom1 = geom2;
    reordered.pGeom2 = geom1;
    reordered.callback1 = callback2;
    reordered.callback2 = callback1;
    _BallVsPoly(&reordered);
}

static int _RobotVsPoly(RgGeomCollisionPair *pArg)
{
    RgGeomRobot *robot;
    RgGeom *poly;
    RgColiBall ball;
    RgColiBallResult results;
    RgVector contact;
    RgGeomColiHit hit;
    RgVector position;
    RgVector normal;
    RgVector offset;
    RgVector velocity;
    float *contactPtr;
    float dot;

    robot = (RgGeomRobot *) pArg->pGeom1;
    poly = pArg->pGeom2;
    if (robot == 0) {
        assert_prog(geomCollisionAssertRobot1NotNil, geomCollisionSourcePath, 161);
    }
    if (poly == 0) {
        assert_prog(geomCollisionAssertPoly2NotNil, geomCollisionSourcePath, 162);
    }
    __RgGeomPointGetPos(&robot->ball.point, ball.m_center, geomCollisionSourcePath, 164);
    ball.m_radius = RgGeomBallGetRadius(&robot->ball);
    if (RgGeomPolyCheckBall(poly, &ball, &results) == 0) {
        return 0;
    }
    contactPtr = contact;
    XrgNormalizeVector(hit.normal, results.m_correction);
    if (pArg->callback1 != 0) {
        hit.otherObject = poly;
        pArg->callback1(robot, contactPtr);
    }
    if (pArg->callback2 != 0) {
        hit.otherObject = robot;
        pArg->callback2(poly, contactPtr);
    }
    if ((robot->ball.point.geom.status & RG_GEOM_STATUS_PUSH_OUT) != 0) {
        __RgGeomPointGetPos(&robot->ball.point, position, geomCollisionSourcePath, 185);
        __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(offset), "r"(results.m_correction) : "memory");
        XrgAddVector(position, position, offset);
        RgGeomPointSetPos(&robot->ball.point, position);
    }
    XrgNormalizeVector(normal, results.m_correction);
    RgGeomPointGetVel(&robot->ball.point, velocity);
    dot = XrgInnerVector(normal, velocity);
    XrgScaleVector(position, normal, dot);
    XrgSubVector(robot->ball.point.velocity, robot->ball.point.velocity, position);
    return 1;
}

static void _PolyVsRobot(RgGeomCollisionPair *pArg)
{
    RgGeomCollisionPair reordered;
    RgGeom *geom1;
    RgGeom *geom2;
    RgGeomColiCallback callback1;
    RgGeomColiCallback callback2;

    geom2 = pArg->pGeom2;
    geom1 = pArg->pGeom1;
    callback2 = pArg->callback2;
    callback1 = pArg->callback1;
    reordered.pGeom1 = geom2;
    reordered.pGeom2 = geom1;
    reordered.callback1 = callback2;
    reordered.callback2 = callback1;
    _RobotVsPoly(&reordered);
}

static int _BallVsTray(RgGeomCollisionPair *pArg)
{
    RgGeomTrayContact check;
    RgVector contact;
    RgGeomColiHit hit;
    RgGeomBall *point;
    RgGeomTray *tray;
    float *contactPtr;
    RgVector accumulated;
    unsigned int i;
    int result;

    point = (RgGeomBall *) pArg->pGeom1;
    tray = (RgGeomTray *) pArg->pGeom2;
    result = RgGeomTrayCheckBall(tray, point, &check);
    if (result == 0) {
        return 0;
    }
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)" : : "r"(check.last_contact), "r"(contactPtr) : "memory");
    if (check.hit_count >= 2) {
        XrgClearVector(accumulated);
        for (i = 0; i < check.hit_count; i++) {
            __asm__ __volatile__(
                "lqc2 vf30, 0(%0)\n"
                "lqc2 vf31, 0(%1)\n"
                "vadd.xyzw vf30, vf30, vf31\n"
                "sqc2 vf30, 0(%2)\n"
                : : "r"(accumulated), "r"(check.normals[i]), "r"(accumulated) : "memory");
        }
        XrgNormalizeVector(hit.normal, accumulated);
    } else {
        __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(hit.normal), "r"(check.normals[0]) : "memory");
    }
    if (pArg->callback1 != 0) {
        hit.otherObject = tray;
        pArg->callback1(point, contactPtr);
    }
    if (pArg->callback2 != 0) {
        XrgNegateVector(hit.normal, hit.normal);
        hit.otherObject = point;
        pArg->callback2(tray, contactPtr);
    }
    return 1;
}

static void _TrayVsBall(RgGeomCollisionPair *pArg)
{
    RgGeomCollisionPair reordered;
    RgGeom *geom1;
    RgGeom *geom2;
    RgGeomColiCallback callback1;
    RgGeomColiCallback callback2;

    geom2 = pArg->pGeom2;
    geom1 = pArg->pGeom1;
    callback2 = pArg->callback2;
    callback1 = pArg->callback1;
    reordered.pGeom1 = geom2;
    reordered.pGeom2 = geom1;
    reordered.callback1 = callback2;
    reordered.callback2 = callback1;
    _BallVsTray(&reordered);
}

static int _BallVsPillar(RgGeomCollisionPair *pArg)
{
    RgGeomBall *point;
    RgGeomTray *pillar;
    RgVector contact;
    RgVector hitContact;
    float *hitContactPtr;
    RgGeomColiHit hit;
    RgGeomColiCallback callback;
    int result;

    point = (RgGeomBall *) pArg->pGeom1;
    pillar = (RgGeomTray *) pArg->pGeom2;
    result = RgGeomPillarCheckPoint(pillar, point, contact);
    hitContactPtr = hitContact;
    if (result != 0) {
        __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(contact) : "memory");
        __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(hitContactPtr) : "memory");
        XrgClearVector(hit.normal);
        callback = pArg->callback1;
        hit.normal[1] = 1.0f;
        if (callback != 0) {
            hit.otherObject = pillar;
            callback(point, hitContactPtr);
        }
        callback = pArg->callback2;
        if (callback != 0) {
            hit.otherObject = point;
            callback(pillar, hitContactPtr);
        }
        result = 1;
    }
    return result;
}

static void _PillarVsBall(RgGeomCollisionPair *pArg)
{
    RgGeomCollisionPair reordered;
    RgGeom *geom1;
    RgGeom *geom2;
    RgGeomColiCallback callback1;
    RgGeomColiCallback callback2;

    geom2 = pArg->pGeom2;
    geom1 = pArg->pGeom1;
    callback2 = pArg->callback2;
    callback1 = pArg->callback1;
    reordered.pGeom1 = geom2;
    reordered.pGeom2 = geom1;
    reordered.callback1 = callback2;
    reordered.callback2 = callback1;
    _BallVsPillar(&reordered);
}

static int _RobVsTray(RgGeomCollisionPair *pArg)
{
    RgGeomTrayContact check;
    RgVector contact;
    RgGeomColiHit hit;
    RgGeomTray *tray;
    RgGeomRobot *robot;
    float *contactPtr;
    RgVector scratch;
    unsigned int i;
    int result;

    robot = (RgGeomRobot *) pArg->pGeom1;
    tray = (RgGeomTray *) pArg->pGeom2;
    result = RgGeomTrayCheckBall(tray, &robot->ball, &check);
    if (result == 0) {
        return 0;
    }
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(check.last_contact) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(contactPtr) : "memory");
    if (check.hit_count >= 2) {
        XrgClearVector(scratch);
        for (i = 0; i < check.hit_count; i++) {
            __asm__ __volatile__(
                "lqc2 vf30, 0(%0)\n"
                "lqc2 vf31, 0(%1)\n"
                "vadd.xyzw vf30, vf30, vf31\n"
                "sqc2 vf30, 0(%2)\n"
                : : "r"(scratch), "r"(check.normals[i]), "r"(scratch) : "memory");
        }
        XrgNormalizeVector(hit.normal, scratch);
    } else {
        __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(hit.normal), "r"(check.normals[0]) : "memory");
    }
    if (pArg->callback1 != 0) {
        hit.otherObject = tray;
        pArg->callback1(robot, contactPtr);
    }
    if (pArg->callback2 != 0) {
        hit.otherObject = robot;
        pArg->callback2(tray, contactPtr);
    }
    if ((robot->ball.point.geom.status & RG_GEOM_STATUS_PUSH_OUT) != 0) {
        RgGeomPointSetPos(&robot->ball.point, check.clipped_position);
    }
    for (i = 0; i < check.hit_count; i++) {
        RgVector velocity;
        float *point;
        float dot;

        point = check.normals[i];
        __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(robot->ball.point.velocity) : "memory");
        __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(velocity) : "memory");
        dot = point[0] * velocity[0] + point[1] * velocity[1] + point[2] * velocity[2];
        if (dot < 0.0f) {
            XrgScaleVector(scratch, point, dot);
            XrgSubVector(robot->ball.point.velocity, robot->ball.point.velocity, scratch);
        }
    }
    return 1;
}

/* Reuses tray-versus-ball collision handling for a robot collider (elf_names
 * annotation); a plain tail call to _TrayVsBall with the caller's record. */
static void _TrayVsRob(RgGeomCollisionPair *pArg)
{
    _TrayVsBall(pArg);
}

static int _RobVsPillar(RgGeomCollisionPair *pArg)
{
    RgGeomTray *pillar;
    RgGeomRobot *robot;
    RgVector results[3];
    RgVector contact;
    RgGeomColiHit hit;
    RgVector normalComponent;
    RgVector velocity;
    float *contactPtr;
    float *normalPtr;
    float dot;
    int result;

    robot = (RgGeomRobot *) pArg->pGeom1;
    pillar = (RgGeomTray *) pArg->pGeom2;
    result = RgGeomPillarCheckBall(pillar, &robot->ball, results);
    if (result == 0) {
        return 0;
    }
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(results[0]) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(contactPtr) : "memory");
    normalPtr = hit.normal;
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(results[2]) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(normalPtr) : "memory");
    if (pArg->callback1 != 0) {
        hit.otherObject = pillar;
        pArg->callback1(robot, contactPtr);
    }
    if (pArg->callback2 != 0) {
        XrgNegateVector(hit.normal, hit.normal);
        hit.otherObject = robot;
        pArg->callback2(pillar, contactPtr);
    }
    if ((robot->ball.point.geom.status & RG_GEOM_STATUS_PUSH_OUT) != 0) {
        RgGeomPointSetPos(&robot->ball.point, results[1]);
    }
    XrgCopyVector(velocity, robot->ball.point.velocity);
    dot = XrgInnerVector(results[2], velocity);
    if (dot < 0.0f) {
        XrgScaleVector(normalComponent, results[2], dot);
        XrgSubVector(robot->ball.point.velocity, robot->ball.point.velocity, normalComponent);
    }
    return 1;
}

static void _PillarVsRob(RgGeomCollisionPair *pArg)
{
    RgGeomCollisionPair reordered;
    RgGeom *geom1;
    RgGeom *geom2;
    RgGeomColiCallback callback1;
    RgGeomColiCallback callback2;

    geom2 = pArg->pGeom2;
    geom1 = pArg->pGeom1;
    callback2 = pArg->callback2;
    callback1 = pArg->callback1;
    reordered.pGeom1 = geom2;
    reordered.pGeom2 = geom1;
    reordered.callback1 = callback2;
    reordered.callback2 = callback1;
    _RobVsPillar(&reordered);
}

void InitRgGeomColiArg(RgGeomCollisionPair *pArg)
{
    if (pArg == 0) {
        assert_prog(geomCollisionAssertArgNotNil, geomCollisionSourcePath, 465);
    }
    pArg->pGeom1 = 0;
    pArg->callback1 = 0;
    pArg->pGeom2 = 0;
    pArg->callback2 = 0;
}


int RgGeomCollisionJob(RgGeomCollisionPair *pArg)
{
    RgGeomCollisionHandler handler;
    RgGeom *objA;
    RgGeom *objB;
    int typeA;
    int typeB;
    int result;

    result = 0;
    if (pArg == 0) {
        assert_prog(geomCollisionAssertArgNotNil, geomCollisionSourcePath, 480);
    }
    objA = pArg->pGeom1;
    if (objA == 0 || (objB = pArg->pGeom2) == 0) {
        return 0;
    } else {
        if ((objA->status & objB->status & 1) == 0) {
            return 0;
        }
        typeA = RgGeomGetType(objA);
        typeB = RgGeomGetType(pArg->pGeom2);
        if (typeA >= 0 && typeB >= 0) {
            handler = s_apJobFuncTbl[typeA][typeB];
            if (handler != 0) {
                result = handler(pArg);
            }
            if (result != 0) {
                RgGeom *hitA;
                RgGeom *hitB;

                hitA = pArg->pGeom1;
                hitB = pArg->pGeom2;
                hitA->eventFlag = 1;
                hitB->eventFlag = 1;
            }
        }
    }
    return result;
}



static RgGeomCollisionHandler s_apJobFuncTbl[RG_GEOM_TYPE_COUNT][RG_GEOM_TYPE_COUNT] = {
    { 0, 0, 0, 0, 0, 0 },
    { 0, _BallVsBall, _BallVsBall, _BallVsPoly, _BallVsTray, _BallVsPillar },
    { 0, _BallVsBall, _BallVsBall, _RobotVsPoly, _RobVsTray, _RobVsPillar },
    { 0, (RgGeomCollisionHandler)_PolyVsBall, (RgGeomCollisionHandler)_PolyVsRobot, 0, 0, 0 },
    { 0, (RgGeomCollisionHandler)_TrayVsBall, (RgGeomCollisionHandler)_TrayVsRob, 0, 0, 0 },
    { 0, (RgGeomCollisionHandler)_PillarVsBall, (RgGeomCollisionHandler)_PillarVsRob, 0, 0, 0 }
};

const char geomCollisionAssertRobot1NotNil[16] = "pRobot1 != NIL";

const char geomCollisionAssertPoly2NotNil[16] = "pPoly2 != NIL";
