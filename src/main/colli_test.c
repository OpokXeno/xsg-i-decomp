#include "common.h"

typedef struct CollisionPoint {
    float x;
    float y;
    float z;
    float w;
} CollisionPoint;

static void swapf(float *left, float *right)
{
    float value = *left;

    *left = *right;
    *right = value;
}

INCLUDE_ASM("asm/main/nonmatchings/colli_test", CheckIntersect);

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

INCLUDE_ASM("asm/main/nonmatchings/colli_test", CheckBallBoxCollision);

INCLUDE_ASM("asm/main/nonmatchings/colli_test", InitTest_002DBE88);

#include "shared.h"
typedef struct ParticleState {
    CollisionPoint position;
    CollisionPoint previous_position;
    CollisionPoint velocity;
    float gravity;
    float damping;
} ParticleState;
extern void ppSetPos(ParticleState *particle, float x, float y, float z);
extern int hit;
extern int mode_004DB9D8;
extern float radius;
extern CollisionPoint box1;
extern CollisionPoint box2;
extern CollisionPoint box3;
extern CollisionPoint box4;
extern CollisionPoint hitpos;
extern ParticleState pos1;
extern ParticleState pos2;
typedef struct BallBoxCollisionArgs {
    CollisionPoint *box_corner_a;
    CollisionPoint *box_corner_b;
    CollisionPoint *line_start;
    CollisionPoint *line_end;
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
extern const float D_004D828C;
extern const char D_004CBC58[];
extern const char D_004CBC70[];
extern void xglRenderClearFrame(void);
extern void xglSleep(void);
extern void xglFontDebugPrintf(int x, int y, const char *text, ...);
extern void ppNextStart(ParticleState *particle);
extern void ppNextEnd(ParticleState *particle);

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

    move_step = D_004D828C;
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
            xglFontDebugPrintf(0, 8, D_004CBC58);
        xglFontDebugPrintf(0, 0, D_004CBC70);
        xglSleep();
    } while ((PadData.state.bits & 0x08000100ULL) != 0x08000100ULL);
}
