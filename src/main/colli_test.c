#include "common.h"

typedef struct CollisionPoint {
    float x;
    float y;
    float z;
    float w;
} CollisionPoint;

int CheckIntersect(CollisionPoint *line_start, CollisionPoint *line_end,
                    CollisionPoint *lower, CollisionPoint *upper);

typedef struct CollisionBoundsFrame {
    CollisionPoint bounds[2];
    /* The frame reserves 16 bytes after the two bounds corners that
     * this function never accesses. */
    unsigned char unmodeled_20[16];
} CollisionBoundsFrame;

typedef struct FractionCell {
    float value;
} FractionCell;

static void swapf(float *left, float *right)
{
    float value = *left;

    *left = *right;
    *right = value;
}

static int CheckIntersect(CollisionPoint *line_start, CollisionPoint *line_end,
                          CollisionPoint *lower, CollisionPoint *upper)
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

static int CheckSlope(const CollisionPoint *p0, const CollisionPoint *p1,
                      const CollisionPoint *p2, const CollisionPoint *p3)
{
    float cross = (p1->x - p0->x) * (p3->z - p2->z)
                - (p1->z - p0->z) * (p3->x - p2->x);

    if (cross == 0.0f)
        return 0;
    if (0.0f < cross)
        return 1;
    return -1;
}

static int CheckInBox(const CollisionPoint *point, const CollisionPoint *lower,
                      const CollisionPoint *upper)
{
    if (point->x < upper->x && lower->x < point->x
            && point->z < upper->z && lower->z < point->z)
        return 1;
    return 0;
}

#include "shared.h"
#include "main/xgl_studio.h"
/* Original TU-local pointer storage remains in the unchanged scaffold. */
static StudioCamera * pCamera_004DC65C;
typedef struct ParticleState {
    CollisionPoint position;
    CollisionPoint previous_position;
    CollisionPoint velocity;
    float gravity;
    float damping;
    /* The symbol's 0x40-byte extent is evidenced, while the last eight
     * bytes have no known semantic use in this translation unit. */
    unsigned char unmodeled_38[8];
} ParticleState;
extern void ppSetPos(ParticleState *particle, float x, float y, float z);
static int mode_004DB9D8 = 0;
static int hit = 0;
static float radius;
static ParticleState pos1;
static ParticleState pos2;
static CollisionPoint box1;
static CollisionPoint box2;
static CollisionPoint hitpos;
static CollisionPoint box3;
static CollisionPoint box4;
typedef struct BallBoxCollisionArgs {
    struct CollisionPoint *box_corner_a;
    struct CollisionPoint *box_corner_b;
    struct CollisionPoint *line_start;
    struct CollisionPoint *line_end;
} BallBoxCollisionArgs;
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
static int CheckBallBoxCollision(CollisionPoint *hit_position,
                                 const BallBoxCollisionArgs *args, int mode);
static int InitTest(void);

static int CheckBallBoxCollision(CollisionPoint *hit_position,
                                 const BallBoxCollisionArgs *args, int mode)
{
    CollisionBoundsFrame frame;
    FractionCell fraction_cell;
    CollisionPoint *line_start = args->line_start;
    CollisionPoint *corner_a = args->box_corner_a;
    CollisionPoint *corner_b = args->box_corner_b;
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
    line_start = args->line_start;
    side = 0;
    if (line_start->z <= frame.bounds[0].z)
        side = 1;
    else if (frame.bounds[1].z <= line_start->z)
        side = 2;
    if (line_start->x <= frame.bounds[0].x)
        side |= 4;
    else if (frame.bounds[1].x <= line_start->x)
        side |= 8;

    switch (side) {
    case 1:
        intersection = GetIntersectionPointLineZ(&fraction_cell.value, &args->line_start->x,
                                                &args->line_end->x, frame.bounds[0].z);
        if (!intersection)
            return intersection;
        side |= 16;
        break;
    case 2:
        intersection = GetIntersectionPointLineZ(&fraction_cell.value, &args->line_start->x,
                                                &args->line_end->x, frame.bounds[1].z);
        if (!intersection)
            return intersection;
        side |= 16;
        break;
    case 4:
        if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                       &args->line_end->x, frame.bounds[0].x))
            return 0;
        break;
    case 8:
        if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                       &args->line_end->x, frame.bounds[1].x))
            return 0;
        break;
    case 5:
        slope = CheckSlope(line_start, args->line_end, line_start, &frame.bounds[0]);
        if (slope != -1) {
            if (slope != 1)
                return 0;
            intersection = GetIntersectionPointLineZ(&fraction_cell.value,
                &args->line_start->x, &args->line_end->x, frame.bounds[0].z);
            if (!intersection)
                return intersection;
            side |= 16;
        } else {
            if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                           &args->line_end->x, frame.bounds[0].x))
                return 0;
        }
        break;
    case 6:
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
            side |= 16;
        }
        swapf(&frame.bounds[0].z, &frame.bounds[1].z);
        break;
    case 9:
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
            side |= 16;
        }
        swapf(&frame.bounds[0].x, &frame.bounds[1].x);
        break;
    case 10:
        slope = CheckSlope(line_start, args->line_end, line_start, &frame.bounds[1]);
        if (slope != -1) {
            if (slope != 1)
                return 0;
            intersection = GetIntersectionPointLineZ(&fraction_cell.value,
                &args->line_start->x, &args->line_end->x, frame.bounds[1].z);
            if (!intersection)
                return intersection;
            side |= 16;
        } else {
            if (!GetIntersectionPointLineX(&fraction_cell.value, &args->line_start->x,
                                           &args->line_end->x, frame.bounds[1].x))
                return 0;
        }
        break;
    default:
        return 0;
    }
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
            if (side & 16) {
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

/* The debug payload begins with four in-band control bytes, including a
 * separator, followed by the CollisionHit!! label text. */
static const char collisionHitLabel[24] = "\013\014\200\000\000CollisionHit!!";
/* Text: {reset}CollisionTest */
static const char collisionTestLabel[16] = "\013CollisionTest";
extern void xglRenderClearFrame(void);
extern void xglSleep(void);
extern void xglFontDebugPrintf(int x, int y, const char *text, ...);
extern void ppNextStart(ParticleState *particle);
extern void ppNextEnd(ParticleState *particle);

/* Defining-owner interface proposals; transport belongs to integration. */
extern void xglStudioInit(void);
extern void xglStudioChange(int mode);
extern void xglStudioMainCameraInit(void);
extern void xglStudioGetCamera(StudioCamera **camera, int camera_index);
extern void ppInit(ParticleState *particle);

static inline void SetPlanarPoint(CollisionPoint *point, float x, float z)
{
    point->x = x;
    point->z = z;
    point->y = 0.0f;
}

static int InitTest(void)
{
    xglStudioInit();
    xglStudioChange(0);
    xglStudioMainCameraInit();
    xglStudioGetCamera(&pCamera_004DC65C, 0);

    pCamera_004DC65C->position.y = 16.0f;
    pCamera_004DC65C->rotation.x = 4.712389f;
    pCamera_004DC65C->position.x = 0.0f;
    pCamera_004DC65C->position.z = 0.0f;
    pCamera_004DC65C->rotation.y = 0.0f;
    pCamera_004DC65C->rotation.z = 0.0f;
    SetPlanarPoint(&box1, -2.45f, -0.401f);
    SetPlanarPoint(&box2, 1.7f, 2.2f);
    SetPlanarPoint(&box3, -3.0f, -3.2f);
    SetPlanarPoint(&box4, 1.7f, -1.41f);

    ppInit(&pos1);
    ppInit(&pos2);
    ppSetPos(&pos1, 0.0f, 0.0f, 3.0f);
    radius = 0.5f;
    ppSetPos(&pos2, 0.0f, 0.0f, 3.0f);

    pos2.gravity = pos1.gravity = 0.0f;
    pos2.damping = pos1.damping = 0.8f;
    hitpos.y = hitpos.z = hitpos.x = 0.0f;
    hitpos.w = radius;
    mode_004DB9D8 = 0;
    hit = 0;
    return 1;
}

void ColliTest(void)
{
    BallBoxCollisionArgs collision;
    BallBoxCollisionArgs opposing_collision;
    ParticleState *particle;
    float move_step;

    xglRenderClearFrame();
    if (!InitTest())
        return;
    while (PadData.state.halves.pressed & 0x20)
        xglSleep();
    if ((PadData.state.bits & 0x08000100ULL) == 0x08000100ULL)
        return;

    move_step = 0.1f;
    do {
        switch (mode_004DB9D8) {
        case 0:
        default:
            particle = &pos1;
            break;
        case 1:
            particle = &pos2;
            break;
        }
        ppNextStart(particle);

        if (PadData.movement & 0x1000)
            particle->position.z -= move_step;
        if (PadData.movement & 0x4000)
            particle->position.z += move_step;
        if (PadData.movement & 0x8000)
            particle->position.x -= move_step;
        if (PadData.movement & 0x2000)
            particle->position.x += move_step;

        switch (mode_004DB9D8) {
        case 0:
        default:
            if (!(PadData.state.halves.held & 0x40)) {
                collision.box_corner_a = &box1;
                collision.box_corner_b = &box2;
                collision.line_start = &particle->previous_position;
                collision.line_end = &particle->position;
                particle->previous_position.w = radius;
                particle->position.w = radius;
                hit = CheckBallBoxCollision(&hitpos, &collision, 1);
                if (hit) {
                    particle->position.x = hitpos.x;
                    particle->position.y = hitpos.y;
                    particle->position.z = hitpos.z;
                }
                collision.box_corner_a = &box3;
                collision.box_corner_b = &box4;
                hit = CheckBallBoxCollision(&hitpos, &collision, 1);
                if (hit) {
                    particle->position.x = hitpos.x;
                    particle->position.y = hitpos.y;
                    particle->position.z = hitpos.z;
                }
            }
            ppSetPos(&pos2, pos1.position.x, pos1.position.y, pos1.position.z);
            break;
        case 1:
            opposing_collision.box_corner_a = &box1;
            opposing_collision.box_corner_b = &box2;
            opposing_collision.line_start = &pos1.position;
            opposing_collision.line_end = &pos2.position;
            pos2.position.w = radius;
            pos1.position.w = radius;
            hit = CheckBallBoxCollision(&hitpos, &opposing_collision, 0);
            opposing_collision.box_corner_a = &box3;
            opposing_collision.box_corner_b = &box4;
            hit = CheckBallBoxCollision(&hitpos, &opposing_collision, 0);
            break;
        }
        ppNextEnd(particle);

        if (PadData.state.halves.pressed & 0x20) {
            switch (mode_004DB9D8) {
            case 0:
            default:
                mode_004DB9D8 = 1;
                ppSetPos(&pos2, pos1.position.x, pos1.position.y,
                         pos1.position.z);
                break;
            case 1:
                mode_004DB9D8 = 2;
                ppSetPos(&pos1, pos2.position.x, pos2.position.y,
                         pos2.position.z);
                break;
            }
        }
        if (mode_004DB9D8 == 1)
            pos2.position.w = radius;
        pos1.position.w = radius;
        if (hit)
            xglFontDebugPrintf(0, 8, collisionHitLabel);
        xglFontDebugPrintf(0, 0, collisionTestLabel);
        xglSleep();
    } while ((PadData.state.bits & 0x08000100ULL) != 0x08000100ULL);
}

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

static unsigned int col1[4] = { 0x80, 0x80, 0x80, 0xff };

static unsigned int col2[4] = { 0xa0, 0x00, 0x00, 0xff };

static unsigned int col3[4] = { 0x00, 0x90, 0x90, 0xff };
