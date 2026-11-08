#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "main/pp_init.h"
#include "main/xgl_font.h"
#include "main/xgl_render.h"
#include "main/xgl_thread.h"

typedef struct CollisionBoundsFrame {
    Vector4 bounds[2];
    /* The frame reserves 16 bytes after the two bounds corners that
     * this function never accesses. */
    unsigned char unmodeled_20[16];
} CollisionBoundsFrame;

typedef struct FractionCell {
    float value;
} FractionCell;

/* A PpParticle (0x38 bytes) followed by eight bytes this TU never accesses:
 * the original .bss places pos2 0x40 bytes after pos1 (section 2**3). */
typedef struct CollisionParticle {
    PpParticle particle;
    unsigned char unmodeled_38[8];
} CollisionParticle;

typedef struct BallBoxCollisionArgs {
    Vector4 *box_corner_a;
    Vector4 *box_corner_b;
    Vector4 *line_start;
    Vector4 *line_end;
} BallBoxCollisionArgs;

/* Where the segment start lies relative to the box (CheckBallBoxCollision's side). */
enum {
    COLLI_SIDE_Z_LOW = 1,       /* start at or below the near z face */
    COLLI_SIDE_Z_HIGH = 2,      /* start at or beyond the far z face */
    COLLI_SIDE_X_LOW = 4,
    COLLI_SIDE_X_HIGH = 8,
    COLLI_SIDE_HIT_Z = 16       /* the hit lies on a z face, not an x face */
};

/* PadData.state.bits: SELECT held with START pressed ends the test. */
#define COLLI_TEST_EXIT_PADS 0x08000100ULL

typedef union CollisionPadButtons {
    u64 bits;
    struct {
        u16 held;
        u16 pressed;
        u16 unmodeled_2c;
        u16 unmodeled_2e;
    } halves;
} CollisionPadButtons;

typedef struct CollisionPadState {
    u8 unmodeled_00[0x28];
    CollisionPadButtons state;
    u16 movement;
} CollisionPadState;

extern CollisionPadState PadData;

static int CheckIntersect(Vector4 *line_start, Vector4 *line_end,
                          Vector4 *lower, Vector4 *upper);

static int CheckBallBoxCollision(Vector4 *hit_position,
                                 const BallBoxCollisionArgs *args, int mode);

static int InitTest(void);
extern void xglStudioGetCamera(StudioCamera **camera, int camera_index);
static int mode_004DB9D8 = 0;

static int hit = 0;

static unsigned int col1[4] = { 0x80, 0x80, 0x80, 0xff };

static unsigned int col2[4] = { 0xa0, 0x00, 0x00, 0xff };

static unsigned int col3[4] = { 0x00, 0x90, 0x90, 0xff };

static StudioCamera * pCamera_004DC65C;

static float radius;

static CollisionParticle pos1;

static CollisionParticle pos2;

static Vector4 box1;

static Vector4 box2;

static Vector4 hitpos;

static Vector4 box3;

static Vector4 box4;

static void swapf(float *left, float *right)
{
    float value = *left;

    *left = *right;
    *right = value;
}

/* Returns non-zero when the XZ segment line_start-line_end crosses lower-upper. */
static int CheckIntersect(Vector4 *line_start, Vector4 *line_end,
                          Vector4 *lower, Vector4 *upper)
{
    float determinant;
    float line_dz;
    float cross;
    float end_x;
    float upper_z;
    float lower_z;
    float upper_x;
    float lower_x;
    float end_z;
    float start_z;
    float origin_dx;
    float origin_dz;
    int reverse;
    int result;

    determinant = (upper->x - lower->x) * (line_end->z - line_start->z);
    cross = (line_end->x - line_start->x) * (upper->z - lower->z);
    if (determinant == cross)
        return 0;

    determinant -= cross;
    if (determinant < 0.0f) {
        reverse = 1;
        determinant = -determinant;
    } else {
        reverse = 0;
    }

    end_x = line_end->x;
    upper_z = upper->z;
    lower_z = lower->z;
    upper_x = upper->x;
    lower_x = lower->x;
    end_z = line_end->z;
    start_z = line_start->z;
    origin_dx = lower_x - line_start->x;
    origin_dz = lower_z - start_z;
    line_dz = end_z - start_z;
    cross = origin_dz * (end_x - line_start->x) - origin_dx * line_dz;
    if (reverse)
        cross = -cross;
    if (cross < 0.0f || determinant < cross)
        return 0;

    cross = origin_dz * (upper_x - lower_x) - origin_dx * (upper_z - lower_z);
    if (reverse)
        cross = -cross;
    result = 0;
    if (!(cross < 0.0f)) {
        result = 1;
        if (determinant < cross)
            result = 0;
    }
    return result;
}

/* Stores where the segment reaches plane x in *intersection. Returns 0 when x is constant along it. */
static int GetIntersectionPointLineX(float *intersection, const float *line_start,
                                     const float *line_end, float x)
{
    if (line_start[0] == line_end[0]) {
        *intersection = 0;
        return 0;
    }

    *intersection = (x - line_start[0]) / (line_end[0] - line_start[0]);
    return 1;
}

/* Stores where the segment reaches plane z in *intersection. Returns 0 when z is constant along it. */
static int GetIntersectionPointLineZ(float *intersection, const float *line_start,
                                     const float *line_end, float z)
{
    if (line_start[2] == line_end[2]) {
        *intersection = 0;
        return 0;
    }

    *intersection = (z - line_start[2]) / (line_end[2] - line_start[2]);
    return 1;
}

/* Returns the sign (1, -1 or 0) of the XZ cross product of p1-p0 and p3-p2. */
static int CheckSlope(const Vector4 *p0, const Vector4 *p1,
                      const Vector4 *p2, const Vector4 *p3)
{
    float cross = (p1->x - p0->x) * (p3->z - p2->z)
                - (p1->z - p0->z) * (p3->x - p2->x);

    if (cross == 0.0f)
        return 0;
    if (0.0f < cross)
        return 1;
    return -1;
}

/* Returns 1 when point lies strictly inside the XZ box spanned by lower and upper. */
static int CheckInBox(const Vector4 *point, const Vector4 *lower,
                      const Vector4 *upper)
{
    if (point->x < upper->x && lower->x < point->x
            && point->z < upper->z && lower->z < point->z)
        return 1;
    return 0;
}

/*
 * Tests the XZ segment line_start to line_end against the box between
 * box_corner_a and box_corner_b, grown by line_start->w. Returns 1 on a hit
 * and stores the hit point when hit_position is not null. Mode 1 keeps the
 * coordinate along the hit face at the segment end.
 */
static int CheckBallBoxCollision(Vector4 *hit_position,
                                 const BallBoxCollisionArgs *args, int mode)
{
    CollisionBoundsFrame frame;
    FractionCell fraction_cell;
    Vector4 *line_start = args->line_start;
    Vector4 *corner_a = args->box_corner_a;
    Vector4 *corner_b = args->box_corner_b;
    int side;
    int slope;
    int intersection;
    fraction_cell.value = line_start->w;

    if (corner_a->x > corner_b->x) {
        frame.bounds[1].x = corner_a->x + fraction_cell.value;
        frame.bounds[0].x = corner_b->x - fraction_cell.value;
    } else {
        frame.bounds[1].x = corner_b->x + fraction_cell.value;
        frame.bounds[0].x = corner_a->x - fraction_cell.value;
    }
    if (corner_a->z > corner_b->z) {
        frame.bounds[1].z = corner_a->z + fraction_cell.value;
        frame.bounds[0].z = corner_b->z - fraction_cell.value;
    } else {
        frame.bounds[1].z = corner_b->z + fraction_cell.value;
        frame.bounds[0].z = corner_a->z - fraction_cell.value;
    }
    if (CheckInBox(line_start, &frame.bounds[0], &frame.bounds[1])) {
        if (hit_position) {
            /* Copy all four original lanes. VF31 is transient VU0 scratch;
             * no ACC/Q/VI/flags are read or modified. The compiler does not
             * allocate C values in VF registers. Caller point storage is
             * 16-byte aligned and the memory clobber covers the full copy. */
            __asm__ __volatile__(
                "lqc2 vf31, 0(%1)\n\tsqc2 vf31, 0(%0)"
                : : "r"(hit_position), "r"(args->line_start) : "memory");
        }
        return 1;
    }
    if (!CheckInBox(args->line_end, &frame.bounds[0], &frame.bounds[1])
            && !CheckIntersect(args->line_start, args->line_end,
                               &frame.bounds[0], &frame.bounds[1])) {
        swapf(&frame.bounds[0].x, &frame.bounds[1].x);
        if (!CheckIntersect(args->line_start, args->line_end,
                            &frame.bounds[0], &frame.bounds[1]))
            return 0;
        swapf(&frame.bounds[0].x, &frame.bounds[1].x);
    }
    /* Which face region of the box the segment start lies beside. */
    line_start = args->line_start;
    side = 0;
    if (line_start->z <= frame.bounds[0].z)
        side = COLLI_SIDE_Z_LOW;
    else if (frame.bounds[1].z <= line_start->z)
        side = COLLI_SIDE_Z_HIGH;
    if (line_start->x <= frame.bounds[0].x)
        side |= COLLI_SIDE_X_LOW;
    else if (frame.bounds[1].x <= line_start->x)
        side |= COLLI_SIDE_X_HIGH;

    /* Find the crossing with the box face the segment must pass through. */
    switch (side) {
    case COLLI_SIDE_Z_LOW:
        intersection = GetIntersectionPointLineZ(&fraction_cell.value, &args->line_start->x,
                                                &args->line_end->x, frame.bounds[0].z);
        if (!intersection)
            return intersection;
        side |= COLLI_SIDE_HIT_Z;
        break;
    case COLLI_SIDE_Z_HIGH:
        intersection = GetIntersectionPointLineZ(&fraction_cell.value, &args->line_start->x,
                                                &args->line_end->x, frame.bounds[1].z);
        if (!intersection)
            return intersection;
        side |= COLLI_SIDE_HIT_Z;
        break;
    case COLLI_SIDE_X_LOW:
        if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                       &args->line_end->x, frame.bounds[0].x))
            return 0;
        break;
    case COLLI_SIDE_X_HIGH:
        if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                       &args->line_end->x, frame.bounds[1].x))
            return 0;
        break;
    case COLLI_SIDE_X_LOW | COLLI_SIDE_Z_LOW:
        slope = CheckSlope(line_start, args->line_end, line_start, &frame.bounds[0]);
        if (slope != -1) {
            if (slope != 1)
                return 0;
            intersection = GetIntersectionPointLineZ(&fraction_cell.value,
                &args->line_start->x, &args->line_end->x, frame.bounds[0].z);
            if (!intersection)
                return intersection;
            side |= COLLI_SIDE_HIT_Z;
        } else {
            if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                           &args->line_end->x, frame.bounds[0].x))
                return 0;
        }
        break;
    case COLLI_SIDE_X_LOW | COLLI_SIDE_Z_HIGH:
        swapf(&frame.bounds[0].z, &frame.bounds[1].z);
        slope = CheckSlope(args->line_start, args->line_end,
                           args->line_start, &frame.bounds[1]);
        if (slope != -1) {
            if (slope != 1)
                return 0;
            if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                           &args->line_end->x, frame.bounds[0].x))
                return 0;
        } else {
            intersection = GetIntersectionPointLineZ(&fraction_cell.value,
                &args->line_start->x, &args->line_end->x, frame.bounds[0].z);
            if (!intersection)
                return intersection;
            side |= COLLI_SIDE_HIT_Z;
        }
        swapf(&frame.bounds[0].z, &frame.bounds[1].z);
        break;
    case COLLI_SIDE_X_HIGH | COLLI_SIDE_Z_LOW:
        swapf(&frame.bounds[0].x, &frame.bounds[1].x);
        slope = CheckSlope(args->line_start, args->line_end,
                           args->line_start, &frame.bounds[0]);
        if (slope != -1) {
            if (slope != 1)
                return 0;
            if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                           &args->line_end->x, frame.bounds[0].x))
                return 0;
        } else {
            intersection = GetIntersectionPointLineZ(&fraction_cell.value,
                &args->line_start->x, &args->line_end->x, frame.bounds[0].z);
            if (!intersection)
                return intersection;
            side |= COLLI_SIDE_HIT_Z;
        }
        swapf(&frame.bounds[0].x, &frame.bounds[1].x);
        break;
    case COLLI_SIDE_X_HIGH | COLLI_SIDE_Z_HIGH:
        slope = CheckSlope(line_start, args->line_end, line_start, &frame.bounds[1]);
        if (slope != -1) {
            if (slope != 1)
                return 0;
            intersection = GetIntersectionPointLineZ(&fraction_cell.value,
                &args->line_start->x, &args->line_end->x, frame.bounds[1].z);
            if (!intersection)
                return intersection;
            side |= COLLI_SIDE_HIT_Z;
        } else {
            if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                           &args->line_end->x, frame.bounds[1].x))
                return 0;
        }
        break;
    default:
        return 0;
    }
    /* Interpolate the hit point, backed off slightly along the segment. */
    if (hit_position) {
        fraction_cell.value -= 0.001f;
        switch (mode) {
        case 0:
        default:
            hit_position->x = (args->line_end->x - args->line_start->x) * fraction_cell.value
                + args->line_start->x;
            hit_position->y = (args->line_end->y - args->line_start->y) * fraction_cell.value
                + args->line_start->y;
            hit_position->z = (args->line_end->z - args->line_start->z) * fraction_cell.value
                + args->line_start->z;
            break;
        case 1:
            if (side & COLLI_SIDE_HIT_Z) {
                hit_position->x = args->line_end->x;
                hit_position->z = (args->line_end->z - args->line_start->z) * fraction_cell.value
                    + args->line_start->z;
            } else {
                hit_position->x = (args->line_end->x - args->line_start->x) * fraction_cell.value
                    + args->line_start->x;
                hit_position->z = args->line_end->z;
            }
            hit_position->y = (args->line_end->y - args->line_start->y) * fraction_cell.value
                + args->line_start->y;
            break;
        }
    }
    return 1;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* The debug payload begins with four in-band control bytes, including a
 * separator, followed by the CollisionHit!! label text. */
static const char collisionHitLabel[24] = "\013\014\200\000\000CollisionHit!!";

/* Text: {reset}CollisionTest */
static const char collisionTestLabel[16] = "\013CollisionTest";

static inline void SetPlanarPoint(Vector4 *point, float x, float z)
{
    point->x = x;
    point->z = z;
    point->y = 0.0f;
}

/* Sets up the studio camera, the two boxes and the two test particles. */
static int InitTest(void)
{
    xglStudioInit();
    xglStudioChange(0);
    xglStudioMainCameraInit();
    xglStudioGetCamera(&pCamera_004DC65C, 0);

    pCamera_004DC65C->position.vector.y = 16.0f;
    pCamera_004DC65C->rotation.vector.x = 4.712389f;
    pCamera_004DC65C->position.vector.x = 0.0f;
    pCamera_004DC65C->position.vector.z = 0.0f;
    pCamera_004DC65C->rotation.vector.y = 0.0f;
    pCamera_004DC65C->rotation.vector.z = 0.0f;
    SetPlanarPoint(&box1, -2.45f, -0.401f);
    SetPlanarPoint(&box2, 1.7f, 2.2f);
    SetPlanarPoint(&box3, -3.0f, -3.2f);
    SetPlanarPoint(&box4, 1.7f, -1.41f);

    ppInit(&pos1.particle);
    ppInit(&pos2.particle);
    ppSetPos(&pos1.particle, 0.0f, 0.0f, 3.0f);
    radius = 0.5f;
    ppSetPos(&pos2.particle, 0.0f, 0.0f, 3.0f);

    pos2.particle.gravity = pos1.particle.gravity = 0.0f;
    pos2.particle.damping = pos1.particle.damping = 0.8f;
    hitpos.y = hitpos.z = hitpos.x = 0.0f;
    hitpos.w = radius;
    mode_004DB9D8 = 0;
    hit = 0;
    return 1;
}

/* Debug collision test loop: the pad moves a particle and the boxes are tested until SELECT+START. */
void ColliTest(void)
{
    BallBoxCollisionArgs collision;
    BallBoxCollisionArgs opposing_collision;
    PpParticle *particle;
    float move_step;
    Vector4 *position;

    xglRenderClearFrame();
    if (!InitTest())
        return;
    while (PadData.state.halves.pressed & PAD_CIRCLE)
        xglSleep();
    if ((PadData.state.bits & COLLI_TEST_EXIT_PADS) == COLLI_TEST_EXIT_PADS)
        return;

    move_step = 0.1f;
    do {
        switch (mode_004DB9D8) {
        case 0:
        default:
            particle = &pos1.particle;
            break;
        case 1:
            particle = &pos2.particle;
            break;
        }
        ppNextStart(particle);

        position = &particle->position;
        /* The d-pad moves the particle in the XZ plane. */
        if (PadData.movement & PAD_UP)
            position->z -= move_step;
        if (PadData.movement & PAD_DOWN)
            position->z += move_step;
        if (PadData.movement & PAD_LEFT)
            position->x -= move_step;
        if (PadData.movement & PAD_RIGHT)
            position->x += move_step;

        /* Mode 0 tests the particle's path against both boxes; mode 1 tests the segment between the two particles. */
        switch (mode_004DB9D8) {
        case 0:
        default:
            if (!(PadData.state.halves.held & PAD_CROSS)) {
                collision.box_corner_a = &box1;
                collision.box_corner_b = &box2;
                collision.line_start = &particle->previous_position;
                collision.line_end = position;
                /* previous_position is the vector right after position. */
                position[1].w = radius;
                position->w = radius;
                hit = CheckBallBoxCollision(&hitpos, &collision, 1);
                if (hit) {
                    position->x = hitpos.x;
                    position->y = hitpos.y;
                    position->z = hitpos.z;
                }
                collision.box_corner_a = &box3;
                collision.box_corner_b = &box4;
                hit = CheckBallBoxCollision(&hitpos, &collision, 1);
                if (hit) {
                    position->x = hitpos.x;
                    position->y = hitpos.y;
                    position->z = hitpos.z;
                }
            }
            ppSetPos(&pos2.particle, pos1.particle.position.x,
                     pos1.particle.position.y,
                     pos1.particle.position.z);
            break;
        case 1:
            opposing_collision.box_corner_a = &box1;
            opposing_collision.box_corner_b = &box2;
            opposing_collision.line_start = &pos1.particle.position;
            opposing_collision.line_end = &pos2.particle.position;
            pos2.particle.position.w = radius;
            pos1.particle.position.w = radius;
            hit = CheckBallBoxCollision(&hitpos, &opposing_collision, 0);
            opposing_collision.box_corner_a = &box3;
            opposing_collision.box_corner_b = &box4;
            hit = CheckBallBoxCollision(&hitpos, &opposing_collision, 0);
            break;
        }
        ppNextEnd(particle);

        /* Circle advances the mode and copies the current position into the other particle. */
        if (PadData.state.halves.pressed & PAD_CIRCLE) {
            switch (mode_004DB9D8) {
            case 0:
            default:
                mode_004DB9D8 = 1;
                ppSetPos(&pos2.particle, pos1.particle.position.x,
                         pos1.particle.position.y,
                         pos1.particle.position.z);
                break;
            case 1:
                mode_004DB9D8 = 2;
                ppSetPos(&pos1.particle, pos2.particle.position.x,
                         pos2.particle.position.y,
                         pos2.particle.position.z);
                break;
            }
        }
        if (mode_004DB9D8 == 1)
            pos2.particle.position.w = radius;
        pos1.particle.position.w = radius;
        if (hit)
            xglFontDebugPrintf(0, 8, collisionHitLabel);
        xglFontDebugPrintf(0, 0, collisionTestLabel);
        xglSleep();
    } while ((PadData.state.bits & COLLI_TEST_EXIT_PADS) != COLLI_TEST_EXIT_PADS);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: ＴＩＴＬＥ
 * EN: TITLE */
const char ColliTestTitleLabel[16] = "\n\243\324\243\311\243\324\243\314\243\305";

/* JP: ＤＥＢＵＧ
 * EN: DEBUG */
const char ColliTestDebugLabel[16] = "\n\243\304\243\305\243\302\243\325\243\307";

/* JP: ＣＦＤＥＢＵＧ
 * EN: CFDEBUG */
const char ColliTestCfDebugLabel[16] = "\n\243\303\243\306\243\304\243\305\243\302\243\325\243\307";

/* JP: ＢＡＴＴＬＥＤＥＢＵＧ
 * EN: BATTLEDEBUG */
const char ColliTestBattleDebugLabel[24] = "\n\243\302\243\301\243\324\243\324\243\314\243\305\243\304\243\305\243\302\243\325\243\307";

/* JP: ＢＡＴＴＬＥ
 * EN: BATTLE */
const char ColliTestBattleLabel[16] = "\n\243\302\243\301\243\324\243\324\243\314\243\305";
