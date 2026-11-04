/*
 * OV12 original TU 58: 0x00a2f650..0x00a303a8 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/xrg_rand_int.h"

/* Referenced original data recovered from this translation unit. */
const char D_00A552F8[32] = "../rg_geom_collision.euc.c";

const char D_00A55328[16] = "pArg != NIL";

extern const char D_00A552E8[];
/* "pRobot1 != NIL" */
extern const char D_00A55318[];
/* "pPoly2 != NIL" */
extern int RgGeomPillarCheckPoint(RgGeom *pillar, RgGeomPoint *point, RgVector contact);
extern float XrgNormalizeVector(RgVector destination, RgVector source);
extern void XrgNegateVector(RgVector destination, RgVector source);
extern int RgGeomPillarCheckBall(RgGeom *pillar, RgGeomPoint *point, RgVector worldResults[3]);
extern void XrgScaleVector(RgVector destination, RgVector source, float scale);
extern float XrgInnerVector(RgVector first, RgVector second);
extern void RgGeomPointSetPos(RgGeomPoint *point, RgVector position);
extern int RgGeomPolyCheckBall(RgGeom *poly, float *position, float *results);
extern void assert_prog(const char *expression, const char *source_file, int line);
extern void __RgGeomPointGetPos(RgGeomPoint *point, RgVector destination, const char *source_file, int source_line);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern float RgGeomBallGetRadius(RgGeomPoint *ball);
extern void XrgAddVector(RgVector destination, RgVector first, RgVector second);

/*
 * A pairwise collision-response callback, stored by RgGeomColiArg's own
 * words (data[1]/data[3] above): called with the OTHER side's object
 * pointer and the contact point _BallVsPillar computes.
 */
typedef void (*RgGeomColiCallback)(void *object, RgVector contact);

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

/*
 * The ball-pair contact record the ball checks fill: the two contact points
 * at +0x00 and +0x10, the separating direction at +0x20 and the distance at
 * +0x30. Only the members _BallVsBall reads are named.
 */
typedef struct RgGeomBallCheck {
    RgVector position1;
    RgVector position2;
    RgVector penetration;
    float distance;
} RgGeomBallCheck;

/* A ball's status word, which sits at +0x08 of its RgGeom body. */
typedef struct RgGeomBallHead {
    unsigned char unmodeled_00[8];
    unsigned int status;
} RgGeomBallHead;

extern int RgGeomGetType(RgGeom *pGeom);
extern float RgGeomPointGetWeight(RgGeomPoint *point);
extern int RgGeomBallCheckBall(RgGeom *ball1, RgGeom *ball2, RgGeomBallCheck *contact);
extern int RgGeomBallCheckBallDist(RgGeom *ball1, RgGeom *ball2, RgGeomBallCheck *contact);

static int _BallVsBall(unsigned int *pArg)
{
    int result;
    RgGeomPoint *ball1;
    RgGeomPoint *ball2;
    RgGeomBallCheck check;
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

    ball1 = (RgGeomPoint *) pArg[0];
    ball2 = (RgGeomPoint *) pArg[2];
    type1 = RgGeomGetType((RgGeom *) ball1);
    type2 = RgGeomGetType((RgGeom *) ball2);
    if (type1 == 2 && type2 == type1) {
        result = RgGeomBallCheckBallDist((RgGeom *) ball1, (RgGeom *) ball2, &check);
    } else if (type1 != type2 && (type1 == 2 || type2 == 2)) {
        result = RgGeomBallCheckBall((RgGeom *) ball1, (RgGeom *) ball2, &check);
    } else {
        result = RgGeomBallCheckBallDist((RgGeom *) ball1, (RgGeom *) ball2, &check);
    }
    if (result != 0) {
        ball2Status = ((RgGeomBallHead *) ball2)->status;
        bothStatus = ((RgGeomBallHead *) ball1)->status & ball2Status;
        weight1 = RgGeomPointGetWeight(ball1);
        bothStatus &= 0x10;
        weight2 = RgGeomPointGetWeight(ball2);
        if (bothStatus != 0) {
            totalWeight = weight1 + weight2;
            XrgScaleVector(shift, check.penetration, weight2 / totalWeight);
            XrgAddVector(newPosition, check.position1, shift);
            RgGeomPointSetPos(ball1, newPosition);
            XrgScaleVector(shift, check.penetration, weight1 / totalWeight);
            XrgSubVector(newPosition, check.position2, shift);
            RgGeomPointSetPos(ball2, newPosition);
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
        if (pArg[1] != 0) {
            hit.otherObject = ball2;
            ((RgGeomColiCallback) pArg[1])(ball1, contact);
        }
        if (pArg[3] != 0) {
            XrgNegateVector(hit.normal, hit.normal);
            hit.otherObject = ball1;
            ((RgGeomColiCallback) pArg[3])(ball2, contact);
        }
        return 1;
    }
    return 0;
}

extern int RgGeomTrayCheckBall(RgGeom *tray, RgGeomPoint *point, RgVector contact);
extern int RgGeomPolyCheckRay(RgGeom *poly, RgVector ray[2], RgVector rayHit[2]);

/*
 * The ball's motion record as _BallVsPoly reads it: the current position at
 * +0x20 and the old position at +0x30; the movement ray runs from the old
 * position to the current one.
 */
typedef struct RgGeomPointMotion {
    unsigned char unmodeled_00[0x20];
    RgVector position;
    RgVector oldPosition;
} RgGeomPointMotion;

/*
 * The polygon's local transform as RgGeomPolySetLocal stores it: a 4x4
 * matrix at +0x60.
 */
typedef struct RgGeomPolyTransform {
    unsigned char unmodeled_00[0x60];
    RgMatrix local;
} RgGeomPolyTransform;

static int _BallVsBall(unsigned int *pArg);

static int _BallVsPoly(unsigned int *pArg)
{
    RgGeomPoint *ball;
    RgGeom *poly;
    RgGeomPointMotion *motion;
    RgVector ray[2];
    RgVector rayHit[2];
    RgVector localOldPosition;
    RgVector localPosition;
    RgVector contact;
    float *contactPtr;
    RgGeomColiHit hit;

    ball = (RgGeomPoint *) pArg[0];
    poly = (RgGeom *) pArg[2];
    motion = (RgGeomPointMotion *) ball;
    __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(localPosition), "r"(motion->position) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%2)" : : "r"(localOldPosition), "r"(motion->oldPosition), "r"(localOldPosition) : "memory");
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
        : : "r"(localPosition), "r"(((RgGeomPolyTransform *) poly)->local) : "memory");
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
        : : "r"(localOldPosition), "r"(((RgGeomPolyTransform *) poly)->local) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(motion->oldPosition) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(ray[0]) : "memory");
    __asm__ __volatile__(
        "lqc2 vf30, 0(%0)\n"
        "lqc2 vf31, 0(%1)\n"
        "vsub.xyzw vf30, vf30, vf31\n"
        "sqc2 vf30, 0(%2)\n"
        : : "r"(motion->position), "r"(motion->oldPosition), "r"(ray[1]) : "memory");
    if (RgGeomPolyCheckRay(poly, ray, rayHit) == 0) {
        return 0;
    }
    contact[0] = ray[1][0] * rayHit[1][0];
    contact[1] = ray[1][1] * rayHit[1][0];
    contact[2] = ray[1][2] * rayHit[1][0];
    contact[3] = ray[1][3] * rayHit[1][0];
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf30, 0(%0)" : : "r"(contactPtr) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(ray[0]) : "memory");
    __asm__ __volatile__("vadd.xyzw vf30, vf30, vf31" : : : "memory");
    __asm__ __volatile__("sqc2 vf30, 0(%0)" : : "r"(contactPtr) : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(hit.normal), "r"(rayHit[0]) : "memory");
    hit.normal[3] = 1.0f;
    if (pArg[1] != 0) {
        hit.otherObject = poly;
        ((RgGeomColiCallback) pArg[1])(ball, contactPtr);
    }
    if (pArg[3] != 0) {
        XrgNegateVector(hit.normal, hit.normal);
        hit.otherObject = ball;
        ((RgGeomColiCallback) pArg[3])(poly, contactPtr);
    }
    return 1;
}

static int _BallVsPoly(unsigned int *pArg);

static void _PolyVsBall(unsigned int *pArg)
{
    unsigned int reordered[4];
    unsigned int first;
    unsigned int second;
    unsigned int third;
    unsigned int fourth;

    third = pArg[2];
    first = pArg[0];
    fourth = pArg[3];
    second = pArg[1];
    reordered[0] = third;
    reordered[2] = first;
    reordered[1] = fourth;
    reordered[3] = second;
    _BallVsPoly(reordered);
}

/*
 * The robot's head as _RobVsPillar reads it: the status word at +8 (tested
 * against the 0x10 bit) and the velocity vector at +0x40.
 */
typedef struct RgGeomRobotHead {
    unsigned char unmodeled_00[8];
    unsigned int status;
    unsigned char unmodeled_0c[0x34];
    RgVector velocity;
} RgGeomRobotHead;

static int _RobotVsPoly(unsigned int *pArg)
{
    RgGeomPoint *robot;
    RgGeom *poly;
    struct {
        RgVector center;
        float radius;
    } ball;
    RgVector results[2];
    RgVector contact;
    RgGeomColiHit hit;
    RgVector position;
    RgVector normal;
    RgVector offset;
    RgVector velocity;
    RgGeomRobotHead *head;
    float *contactPtr;
    float dot;

    robot = (RgGeomPoint *) pArg[0];
    poly = (RgGeom *) pArg[2];
    if (robot == 0) {
        assert_prog(D_00A552E8, D_00A552F8, 161);
    }
    if (poly == 0) {
        assert_prog(D_00A55318, D_00A552F8, 162);
    }
    __RgGeomPointGetPos(robot, ball.center, D_00A552F8, 164);
    ball.radius = RgGeomBallGetRadius(robot);
    if (RgGeomPolyCheckBall(poly, ball.center, results[0]) == 0) {
        return 0;
    }
    contactPtr = contact;
    XrgNormalizeVector(hit.normal, results[1]);
    if (pArg[1] != 0) {
        hit.otherObject = poly;
        ((RgGeomColiCallback) pArg[1])(robot, contactPtr);
    }
    if (pArg[3] != 0) {
        hit.otherObject = robot;
        ((RgGeomColiCallback) pArg[3])(poly, contactPtr);
    }
    head = (RgGeomRobotHead *) robot;
    if ((head->status & 0x10) != 0) {
        __RgGeomPointGetPos(robot, position, D_00A552F8, 185);
        __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(offset), "r"(results[1]) : "memory");
        XrgAddVector(position, position, offset);
        RgGeomPointSetPos(robot, position);
    }
    XrgNormalizeVector(normal, results[1]);
    RgGeomPointGetVel(robot, velocity);
    dot = XrgInnerVector(normal, velocity);
    XrgScaleVector(position, normal, dot);
    XrgSubVector(head->velocity, head->velocity, position);
    return 1;
}

static int _RobotVsPoly(unsigned int *pArg);

static void _PolyVsRobot(unsigned int *pArg)
{
    unsigned int reordered[4];
    unsigned int first;
    unsigned int second;
    unsigned int third;
    unsigned int fourth;

    third = pArg[2];
    first = pArg[0];
    fourth = pArg[3];
    second = pArg[1];
    reordered[0] = third;
    reordered[2] = first;
    reordered[1] = fourth;
    reordered[3] = second;
    _RobotVsPoly(reordered);
}

/*
 * The tray check's output record: the contact point written at +0x00, the
 * position at +0x10 (the vector _RobVsTray hands to RgGeomPointSetPos), the
 * three-vector scratch the normal is averaged from at +0x20, and its element
 * count at +0x50.
 */
typedef struct RgGeomTrayCheck {
    RgVector contact;
    RgVector position;
    RgVector points[3];
    unsigned int count;
} RgGeomTrayCheck;

static int _BallVsTray(unsigned int *pArg)
{
    RgGeomTrayCheck check;
    RgVector contact;
    RgGeomColiHit hit;
    RgGeomPoint *point;
    RgGeom *tray;
    float *contactPtr;
    RgVector accumulated;
    unsigned int i;
    int result;

    point = (RgGeomPoint *) pArg[0];
    tray = (RgGeom *) pArg[2];
    result = RgGeomTrayCheckBall(tray, point, check.contact);
    if (result == 0) {
        return 0;
    }
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)" : : "r"(check.contact), "r"(contactPtr) : "memory");
    if (check.count >= 2) {
        XrgClearVector(accumulated);
        for (i = 0; i < check.count; i++) {
            __asm__ __volatile__(
                "lqc2 vf30, 0(%0)\n"
                "lqc2 vf31, 0(%1)\n"
                "vadd.xyzw vf30, vf30, vf31\n"
                "sqc2 vf30, 0(%2)\n"
                : : "r"(accumulated), "r"(check.points[i]), "r"(accumulated) : "memory");
        }
        XrgNormalizeVector(hit.normal, accumulated);
    } else {
        __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(hit.normal), "r"(check.points[0]) : "memory");
    }
    if (pArg[1] != 0) {
        hit.otherObject = tray;
        ((RgGeomColiCallback) pArg[1])(point, contactPtr);
    }
    if (pArg[3] != 0) {
        XrgNegateVector(hit.normal, hit.normal);
        hit.otherObject = point;
        ((RgGeomColiCallback) pArg[3])(tray, contactPtr);
    }
    return 1;
}

static int _BallVsTray(unsigned int *pArg);

static void _TrayVsBall(void *pArg)
{
    unsigned int *argWords = pArg;
    unsigned int reordered[4];
    unsigned int first;
    unsigned int second;
    unsigned int third;
    unsigned int fourth;

    third = argWords[2];
    first = argWords[0];
    fourth = argWords[3];
    second = argWords[1];
    reordered[0] = third;
    reordered[2] = first;
    reordered[1] = fourth;
    reordered[3] = second;
    _BallVsTray(reordered);
}

/*
 * The pairwise collision-check argument this TU's _XxxVsYyy functions use
 * (still INCLUDE_ASM in this allocation, so only InitRgGeomColiArg's own
 * writes are evidenced here: it clears exactly these four words to a common
 * role, so they are modelled as one array rather than four distinct fields).
 */
typedef struct RgGeomColiArg {
    unsigned int data[4];
} RgGeomColiArg;

static int _BallVsPillar(RgGeomColiArg *pArg)
{
    RgGeomPoint *point;
    RgGeom *pillar;
    RgVector contact;
    RgVector hitContact;
    float *hitContactPtr;
    RgGeomColiHit hit;
    RgGeomColiCallback callback;
    int result;

    point = (RgGeomPoint *) pArg->data[0];
    pillar = (RgGeom *) pArg->data[2];
    result = RgGeomPillarCheckPoint(pillar, point, contact);
    hitContactPtr = hitContact;
    if (result != 0) {
        __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(contact) : "memory");
        __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(hitContactPtr) : "memory");
        XrgClearVector(hit.normal);
        callback = (RgGeomColiCallback) pArg->data[1];
        hit.normal[1] = 1.0f;
        if (callback != 0) {
            hit.otherObject = pillar;
            callback(point, hitContactPtr);
        }
        callback = (RgGeomColiCallback) pArg->data[3];
        if (callback != 0) {
            hit.otherObject = point;
            callback(pillar, hitContactPtr);
        }
        result = 1;
    }
    return result;
}

static void _PillarVsBall(RgGeomColiArg *pArg)
{
    RgGeomColiArg reordered;
    unsigned int first;
    unsigned int second;
    unsigned int third;
    unsigned int fourth;

    third = pArg->data[2];
    first = pArg->data[0];
    fourth = pArg->data[3];
    second = pArg->data[1];
    reordered.data[0] = third;
    reordered.data[2] = first;
    reordered.data[1] = fourth;
    reordered.data[3] = second;
    _BallVsPillar(&reordered);
}

static int _RobVsTray(RgGeomColiArg *pArg)
{
    RgGeomTrayCheck check;
    RgVector contact;
    RgGeomColiHit hit;
    RgGeom *tray;
    RgGeomPoint *robot;
    float *contactPtr;
    RgGeomRobotHead *head;
    RgVector scratch;
    unsigned int i;
    int result;

    robot = (RgGeomPoint *) pArg->data[0];
    tray = (RgGeom *) pArg->data[2];
    result = RgGeomTrayCheckBall(tray, robot, check.contact);
    if (result == 0) {
        return 0;
    }
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(check.contact) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(contactPtr) : "memory");
    if (check.count >= 2) {
        XrgClearVector(scratch);
        for (i = 0; i < check.count; i++) {
            __asm__ __volatile__(
                "lqc2 vf30, 0(%0)\n"
                "lqc2 vf31, 0(%1)\n"
                "vadd.xyzw vf30, vf30, vf31\n"
                "sqc2 vf30, 0(%2)\n"
                : : "r"(scratch), "r"(check.points[i]), "r"(scratch) : "memory");
        }
        XrgNormalizeVector(hit.normal, scratch);
    } else {
        __asm__ __volatile__("lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)" : : "r"(hit.normal), "r"(check.points[0]) : "memory");
    }
    if (pArg->data[1] != 0) {
        hit.otherObject = tray;
        ((RgGeomColiCallback) pArg->data[1])(robot, contactPtr);
    }
    if (pArg->data[3] != 0) {
        hit.otherObject = robot;
        ((RgGeomColiCallback) pArg->data[3])(tray, contactPtr);
    }
    head = (RgGeomRobotHead *) robot;
    if ((head->status & 0x10) != 0) {
        RgGeomPointSetPos(robot, check.position);
    }
    for (i = 0; i < check.count; i++) {
        RgVector velocity;
        float *point;
        float dot;

        point = check.points[i];
        __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(head->velocity) : "memory");
        __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(velocity) : "memory");
        dot = point[0] * velocity[0] + point[1] * velocity[1] + point[2] * velocity[2];
        if (dot < 0.0f) {
            XrgScaleVector(scratch, point, dot);
            XrgSubVector(head->velocity, head->velocity, scratch);
        }
    }
    return 1;
}

/* Still INCLUDE_ASM in this TU. hit: a four-word pair record whose halves
 * _TrayVsBall swaps before calling _BallVsTray (layout not yet recovered). */
static void _TrayVsBall(void *hit);

/* Reuses tray-versus-ball collision handling for a robot collider (elf_names
 * annotation); a plain tail call to _TrayVsBall with the caller's record. */
static void _TrayVsRob(void *hit)
{
    _TrayVsBall(hit);
}

static int _RobVsPillar(RgGeomColiArg *pArg)
{
    RgGeom *pillar;
    RgGeomPoint *robot;
    RgVector results[3];
    RgVector contact;
    RgGeomColiHit hit;
    RgVector normalComponent;
    RgVector velocity;
    RgGeomRobotHead *head;
    float *contactPtr;
    float *normalPtr;
    float dot;
    int result;

    robot = (RgGeomPoint *) pArg->data[0];
    pillar = (RgGeom *) pArg->data[2];
    result = RgGeomPillarCheckBall(pillar, robot, results);
    if (result == 0) {
        return 0;
    }
    contactPtr = contact;
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(results[0]) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(contactPtr) : "memory");
    normalPtr = hit.normal;
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(results[2]) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(normalPtr) : "memory");
    if (pArg->data[1] != 0) {
        hit.otherObject = pillar;
        ((RgGeomColiCallback) pArg->data[1])(robot, contactPtr);
    }
    if (pArg->data[3] != 0) {
        XrgNegateVector(hit.normal, hit.normal);
        hit.otherObject = robot;
        ((RgGeomColiCallback) pArg->data[3])(pillar, contactPtr);
    }
    head = (RgGeomRobotHead *) robot;
    if ((head->status & 0x10) != 0) {
        RgGeomPointSetPos(robot, results[1]);
    }
    XrgCopyVector(velocity, head->velocity);
    dot = XrgInnerVector(results[2], velocity);
    if (dot < 0.0f) {
        XrgScaleVector(normalComponent, results[2], dot);
        XrgSubVector(head->velocity, head->velocity, normalComponent);
    }
    return 1;
}

static int _RobVsPillar(RgGeomColiArg *pArg);

static void _PillarVsRob(RgGeomColiArg *pArg)
{
    RgGeomColiArg reordered;
    unsigned int first;
    unsigned int second;
    unsigned int third;
    unsigned int fourth;

    third = pArg->data[2];
    first = pArg->data[0];
    fourth = pArg->data[3];
    second = pArg->data[1];
    reordered.data[0] = third;
    reordered.data[2] = first;
    reordered.data[1] = fourth;
    reordered.data[3] = second;
    _RobVsPillar(&reordered);
}

extern const char D_00A55328[];
/* "pArg != NIL" */
extern const char D_00A552F8[];

/* "../rg_geom_collision.euc.c" */
void InitRgGeomColiArg(RgGeomColiArg *pArg)
{
    if (pArg == 0) {
        assert_prog(D_00A55328, D_00A552F8, 465);
    }
    pArg->data[0] = 0;
    pArg->data[1] = 0;
    pArg->data[2] = 0;
    pArg->data[3] = 0;
}


/*
 * The head of an RgGeom as RgGeomCollisionJob reads it: the status word at
 * +8 (tested against the active bit) and the event flag at +12 that a
 * successful pair marks. RgGeom's complete layout is owned by ov12/tu051.
 */
typedef struct RgGeomCollisionHead {
    unsigned char unmodeled_00[8];
    unsigned int status;
    int eventFlag;
} RgGeomCollisionHead;

typedef int (*RgGeomCollisionHandler)(RgGeomColiArg *pArg);

/* The 6-by-6 handler table indexed by [typeA][typeB] (scaffold data at
 * 0x00A50090). */
extern RgGeomCollisionHandler s_apJobFuncTbl[6][6];


int RgGeomCollisionJob(RgGeomColiArg *pArg)
{
    RgGeomCollisionHandler handler;
    RgGeomCollisionHead *objA;
    RgGeomCollisionHead *objB;
    int typeA;
    int typeB;
    int result;

    result = 0;
    if (pArg == 0) {
        assert_prog(D_00A55328, D_00A552F8, 480);
    }
    objA = (RgGeomCollisionHead *) pArg->data[0];
    if (objA == 0 || (objB = (RgGeomCollisionHead *) pArg->data[2]) == 0) {
        return 0;
    } else {
        if ((objA->status & objB->status & 1) == 0) {
            return 0;
        }
        typeA = RgGeomGetType((RgGeom *) objA);
        typeB = RgGeomGetType((RgGeom *) pArg->data[2]);
        if (typeA >= 0 && typeB >= 0) {
            handler = s_apJobFuncTbl[typeA][typeB];
            if (handler != 0) {
                result = handler(pArg);
            }
            if (result != 0) {
                RgGeomCollisionHead *hitA;
                RgGeomCollisionHead *hitB;

                hitA = (RgGeomCollisionHead *) pArg->data[0];
                hitB = (RgGeomCollisionHead *) pArg->data[2];
                hitA->eventFlag = 1;
                hitB->eventFlag = 1;
            }
        }
    }
    return result;
}
