/*
 * OV12 original TU 58: 0x00a2f650..0x00a303a8 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/xrg_rand_int.h"

/* Referenced original data recovered from this translation unit. */
const char D_00A552F8[32] = "../rg_geom_collision.euc.c";
const char D_00A55328[16] = "pArg != NIL";

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _BallVsBall);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _BallVsPoly);

static void _BallVsPoly(unsigned int *pArg);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _RobotVsPoly);

static void _RobotVsPoly(unsigned int *pArg);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _BallVsTray);

static void _BallVsTray(unsigned int *pArg);

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

extern int RgGeomPillarCheckPoint(RgGeom *pillar, RgGeomPoint *point, RgVector contact);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _RobVsTray);

/* Still INCLUDE_ASM in this TU. hit: a four-word pair record whose halves
 * _TrayVsBall swaps before calling _BallVsTray (layout not yet recovered). */
static void _TrayVsBall(void *hit);

/* Reuses tray-versus-ball collision handling for a robot collider (elf_names
 * annotation); a plain tail call to _TrayVsBall with the caller's record. */
static void _TrayVsRob(void *hit)
{
    _TrayVsBall(hit);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _RobVsPillar);

static void _RobVsPillar(RgGeomColiArg *pArg);

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

extern void assert_prog(const char *expression, const char *source_file, int line);

extern const char D_00A55328[]; /* "pArg != NIL" */
extern const char D_00A552F8[]; /* "../rg_geom_collision.euc.c" */

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", RgGeomCollisionJob);
