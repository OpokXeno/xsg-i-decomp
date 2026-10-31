/*
 * OV01 original TU 17: 0x00a314b8..0x00a32db8 (36 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/data_unit_org_get.h"
#include "main/act_1.h"

extern void MOutputDebugStringWarn(const char *format, ...);
/* The message body after the ASCII prefix is encoded in EUC-JP. */
/* JP: MCamReleaseControl: カメラは制御中ではない
 * EN: MCamReleaseControl: The camera is not under control */
const char D_00A50EF0[48] =
    "MCamReleaseControl: \xA5\xAB\xA5\xE1\xA5\xE9\xA4\xCF\xC0\xA9\xB8\xE6\xC3\xE6\xA4\xC7\xA4\xCF\xA4\xCA\xA4\xA4";
const char D_00A51000[40] = "MCamGetAtkRange: Invalid ID (%d)";
const char D_00A51028[40] = "MCamGetDefRange: Invalid ID (%d)";
const char zero_duration_message[40] = "MCamMoveProc_CalcRatio: d == 0 (IP:%d)";
const char invalid_interpolation_message[40] = "MCamMoveProc_CalcRatio: Invalid IPTYPE";

extern const char D_00A50F98[]; /* "MCamStopMove: Invalid point" */
extern const char D_00A50FB8[]; /* "MCamIsMoving: Invalid point" */
extern const char D_00A50FD8[]; /* "MCamGetActorCoord: actor is NULL" */

extern void bcopy(const void *source, void *destination, unsigned int count);
/*
 * Linear interpolation state shared by the per-parameter MoveProc callbacks
 * (mode/duration/elapsed feed MCamMoveProc_CalcRatio; start/end/current hold
 * the eased scalar value).
 */
typedef struct MCamMoveState {
    unsigned int mode;
    unsigned int duration;
    unsigned int elapsed;
    float start;
    float end;
    float current;
} MCamMoveState;

/*
 * Shake state (0x20 bytes, eight words, by member arithmetic) advanced by
 * MCamShakeProc: a hold phase oscillating the offset
 * (sine of a wrapped phase, or a sign flip when there is no phase step),
 * then a decay phase that shrinks the amplitude by decayStep per frame.
 */
typedef struct MCamShakeState {
    /* MCamShake's first argument, stored at +0x00 (`sw $4,0x0($9)`,
     * 0x00a31ccc); MCamShakeProc reads +0x04..+0x1C and never this word,
     * so what it selects is not established. */
    unsigned int mode;
    float amplitude;
    float decayStep;
    float offset;
    float phase;
    float phaseStep;
    unsigned int holdFrames;
    unsigned int decayFrames;
} MCamShakeState;

/*
 * How MCamCalcCoord offsets the base coordinate. In MCamMove_Coord the
 * original stores zero to this mode (`sw $0,0x70($20)`, 0x00a32714) between the
 * request's fixOnArrival halfword load (`lh $3,0xBC($18)`, 0x00a3270c) and the
 * camFlags load (`lw $2,0($4)`, 0x00a32718). The enum typing is required by the
 * tested matching form: with an unsigned int field the compiler moved that
 * store above the halfword load, with the enum it kept the original order.
 * That is a property of the form that matched, not an established original type.
 */
typedef enum MCamOffsetMode {
    MCAM_OFFSET_NONE,
    MCAM_OFFSET_XYZ,
    MCAM_OFFSET_POLAR
} MCamOffsetMode;

/*
 * Per-point camera parameters. Only the coordinate-offset block that
 * MCamCalcCoord applies after MCamCalcBaseCoord is recovered.
 */
typedef struct MCamCoordOffset {
    MCamOffsetMode mode;
    unsigned char unmodeled_04[0xC];
    Vector4 offset;
    float distance;
    float pitch;
    float yaw;
    unsigned char unmodeled_2C[0x8];
} MCamCoordOffset;

/*
 * A point's fixed coordinate. MCamMove_Coord copies whole point records with
 * aligned ld/sd loops. What was tested: copying a plain Vector4 source
 * compiled to ldl/ldr/sdl/sdr, while a union with a u64 pair compiled to ld/sd.
 * The u64 pair (the GameVector8 spelling in src/main/game.c) therefore selects
 * the matching form and carries no meaning of its own.
 */
typedef union MCamVector {
    Vector4 vector;
    u64 pair[2];
} MCamVector;

/*
 * The selector MCamCalcBaseCoord switches on for a point's base coordinate
 * (a compare chain at 0x00a31ef4..0x00a31f10, not a jump table): what each
 * case computes names the value.
 * Constants only; the field stays an unsigned int (an enum-typed field
 * changes the compiler's scheduling, see MCamOffsetMode).
 */
enum MCamBaseMode {
    MCAM_BASE_FIXED = 0,  /* copies fixedCoord */
    MCAM_BASE_ACTOR = 1,  /* one actor's coordinate (actor, partSelector) */
    MCAM_BASE_RATIO = 2,  /* actorA..actorB interpolated by ratio */
    MCAM_BASE_RANGE = 3   /* actorA..actorB interpolated by the attack and
                           * defence ranges of atkUnitId/defUnitId */
};

/*
 * MCamCalcBaseCoord selects the base coordinate by mode (MCamBaseMode).
 * Size-preserving for master's type: 0xB0 by member arithmetic (fixedCoord
 * +0x10, ratio +0x6C, coordOffset +0x70..0xA4, perspectiveAngle +0xA4, tail
 * to 0xB0).
 */
typedef struct MCamPointParams {
    unsigned int mode;
    unsigned char unmodeled_04[0xC];
    MCamVector fixedCoord;
    ActRecord *actor;
    unsigned char unmodeled_24[0x4];
    int partSelector;
    ActRecord *actorA;
    int atkUnitId;
    int partSelectorA;
    ActRecord *actorB;
    int defUnitId;
    int partSelectorB;
    unsigned char unmodeled_44[0x28];
    float ratio;
    MCamCoordOffset coordOffset;
    /* MCamMove_Pers reads a request's perspective angle (degrees) here. */
    float perspectiveAngle;
    unsigned char unmodeled_A8[0x8];
} MCamPointParams;

/*
 * Coordinate move state of one point, filled by MCamMove_Coord: the point
 * record at the start and at the target, the other point's coordinate as
 * pivot, the start, end and current coordinates, then the request's path,
 * mode and duration and the elapsed frames (cleared). MCamMoveProc_Coord
 * (still asm) consumes them: it branches three ways on path (0, 1, other:
 * `beqz`/`beq $2,$3` at 0x00a32a7c..0x00a32a84 on `lw $2,0x1A0($18)`),
 * and first hands mode (`lw $4,0x1A4($18)`, 0x00a32a18), the incremented
 * elapsed (`lw $5,0x1AC($18)`/`addiu $5,$5,0x1`, stored back by
 * `sw $5,0x1AC($18)` at 0x00a32a28) and duration (`lw $6,0x1A8($18)`) to
 * MCamMoveProc_CalcRatio (0x00a32a24). The names path and mode are
 * MCamMoveRequest's for the same two words; what the values select is not
 * established.
 */
typedef struct MCamCoordMove {
    MCamPointParams start;
    MCamPointParams target;
    Vector4 pivot;
    Vector4 startCoord;
    Vector4 endCoord;
    /* MCamVector: MCamExec_Control copies it whole into a point's fixedCoord
     * (`ld $3,0x390($2)` at 0x00a32240; as a Vector4 it compiled `ld $2,408($16)`). */
    MCamVector currentCoord;
    int path;
    int mode;
    int duration;
    int elapsed;
} MCamCoordMove;

/*
 * MCamInit clears this complete 0x5B0-byte camera-parameter store (unchanged
 * size, confirmed by member arithmetic: points 0x160, angle/baseCoord/coord/eye/
 * rotation to 0x1F0, coordMove 2 x 0x1B0 from 0x200, tail to 0x5B0). The two
 * point records, the derived base coordinates, coordinates, angles, eye and
 * rotation are the views MCamExec_SetParams_Base and the getters address; the
 * other bytes remain part of the live store but their individual meanings are
 * not recovered yet.
 */
typedef struct MCamParameterStorage {
    MCamPointParams point[2];
    Vector4 angle[2];
    Vector4 baseCoord[2];
    float actorLength[2];
    unsigned char unmodeled_1A8[0x8];
    Vector4 coord[2];
    Vector4 eye;
    Vector4 rotation;
    float bank;
    float perspectiveStart;
    unsigned char unmodeled_1F8[0x8];
    MCamCoordMove coordMove[2];
    /* MCamExec_Control passes &camParams + 0x560 (D_00A5B5E0) to
     * MCamMoveProc_Bank and copies its current (+0x14, `lwc1 $f0,0x574`)
     * to bank. */
    MCamMoveState bankMove;
    MCamMoveState perspectiveMove;
    MCamShakeState shake;
} MCamParameterStorage;

extern Vector4 *MMathCalcOffset(Vector4 *destination, const Vector4 *first, const Vector4 *second);
extern Vector4 *MMathCalcAngleCam(Vector4 *out, Vector4 *source, Vector4 *target);
extern float MMathCalcDistXZ(const Vector4 *first, const Vector4 *second);
extern Vector4 *MMathVectorInterpolation(Vector4 *destination, const Vector4 *first,
                                          const Vector4 *second, float parameter);
/*
 * main/m_math.h declares this void; MCamCalcBaseCoord (like
 * m_ef_create_amp_02.c's MMathCalcDir use) reads the srsAtan2 result MMathCalcDir
 * leaves in $f0 (it calls srsAtan2 last and returns), so this TU redeclares it
 * float-returning.
 */
/* The definition in src/main/m_math.c returns void (m_math.h:22) while the
 * original code here reads $f0 after the call. This declaration is an inherited
 * ABI workaround, also present at src/ov01/m_ef_create_amp_02.c:72; it is not a
 * claim that the signatures agree. */
extern float MMathCalcDir(const void *first, const void *second);
extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);
extern float fmodf(float value, float modulus);
extern Vector4 *MMathCalcOffsetXYZ(Vector4 *destination, const Vector4 *first, const Vector4 *second);

static void *sysCam;
/* MCamTakeControl saves the StudioCamera returned by xglStudioGetCamera2. */
static StudioCamera sysCamBuff;
static int camFlags;
static MCamParameterStorage camParams;

/* Original local labels are interior views in camParams, not extra storage. */
#define D_00A5B220 (camParams.actorLength[0])
#define D_00A5B224 (camParams.actorLength[1])
#define D_00A5B5F8 (camParams.perspectiveMove)

/* Bits of camFlags. MCamMove_Coord picks CAM_FLAG_FIX_COORD (point 0) or
 * CAM_FLAG_FIX_TARGET (point 1) at its head (`addiu $21,$0,0x2000` at
 * 0x00a325e0, `addiu $21,$0,0x4000` at 0x00a325fc) and ORs it in
 * (`or $2,$2,$21`, 0x00a32728) when the request's fixOnArrival halfword
 * (`lh $3,0xBC($18)`, 0x00a3270c) is nonzero, after the move bit
 * (`or $2,$2,$22`, 0x00a3271c); the bits the older functions test keep
 * their names. Enum constants, not macros, as the project publishes them
 * (CALC_NORMAL_TECHNIQUE_COUNT). */
enum CamFlag {
    CAM_FLAG_ACTIVE = 0x1,
    CAM_FLAG_DUMP_DISABLED = 0x10,
    CAM_FLAG_MOVE_COORD = 0x100,
    CAM_FLAG_MOVE_TARGET = 0x200,
    CAM_FLAG_MOVE_BANK = 0x400,
    CAM_FLAG_MOVE_PERS = 0x800,
    CAM_FLAG_FIX_COORD = 0x2000,
    CAM_FLAG_FIX_TARGET = 0x4000
};
#define CAM_SHAKE_BIT 12

void *MCamGetSysCam(void)
{
    return sysCam;
}

/*
 * R5900 native 128-bit vector copy (lq/sq through $8). The getters below copy
 * through this one inline helper: the original shares a single lq/sq tail
 * between both cases, which gcc's cross-jumping only produces when the two
 * inlined asm statements come from the same source line.
 */
static __inline__ void MCamCopyVector(Vector4 *destination, const Vector4 *source)
{
    __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                         : : "r"(destination), "r"(source)
                         : "$8", "memory");
}

Vector4 *MCamGetCurrentBaseCoord(Vector4 *baseCoord, int point)
{
    __asm__ __volatile__("sq $0,0(%0)" : : "r"(baseCoord) : "memory");
    switch (point) {
    case 0:
        MCamCopyVector(baseCoord, &camParams.baseCoord[0]);
        break;
    case 1:
        MCamCopyVector(baseCoord, &camParams.baseCoord[1]);
        break;
    }
    return baseCoord;
}

/* The clear is VU0's constant $vf0 (0, 0, 0, 1); w is then reset to 1.0f. */
Vector4 *MCamGetCurrentCoord(Vector4 *coord, int point)
{
    __asm__ __volatile__("sqc2 $vf0,0(%0)" : : "r"(coord) : "memory");
    switch (point) {
    case 0:
        MCamCopyVector(coord, &camParams.coord[0]);
        break;
    case 1:
        MCamCopyVector(coord, &camParams.coord[1]);
        break;
    }
    coord->w = 1.0f;
    return coord;
}

Vector4 *MCamGetCurrentAngle(Vector4 *angle, int point)
{
    __asm__ __volatile__("sq $0,0(%0)" : : "r"(angle) : "memory");
    switch (point) {
    case 0:
        MCamCopyVector(angle, &camParams.angle[0]);
        break;
    case 1:
        MCamCopyVector(angle, &camParams.angle[1]);
        break;
    }
    return angle;
}

/* Vector copies below are the R5900 native 128-bit lq/sq pair through $8. */
Vector4 *MCamGetCurrentEye(Vector4 *eye)
{
    __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                         : : "r"(eye), "r"(&camParams.eye)
                         : "$8", "memory");
    return eye;
}

float MCamGetLengthActor(int selector)
{
    float length = 0.0f;

    switch (selector) {
    case 0:
        length = D_00A5B220;
        break;
    case 1:
        length = D_00A5B224;
        break;
    }
    return length;
}

void MCamInit(void)
{
    camFlags = 0;
    memset(&camParams, 0, sizeof(camParams));
}

static void MCamExec_Control(void);
static void MCamExec_SetParams(void);
static void MCamExec_DispParams(void);

void MCamExec(void)
{
    if (camFlags & CAM_FLAG_ACTIVE) {
        MCamExec_Control();
        MCamExec_SetParams();
        MCamExec_DispParams();
    }
}

void MCamDumpDisabled(short disable)
{
    if (disable != 0) {
        camFlags |= CAM_FLAG_DUMP_DISABLED;
    } else {
        camFlags &= ~CAM_FLAG_DUMP_DISABLED;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamTakeControl);

int MCamReleaseControl(void)
{
    if (!(camFlags & CAM_FLAG_ACTIVE)) {
        MOutputDebugStringWarn(D_00A50EF0);
        return 0;
    }

    bcopy(&sysCamBuff, sysCam, sizeof(sysCamBuff));
    camFlags &= ~CAM_FLAG_ACTIVE;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamStoreParams);

int MCamIsActive(void)
{
    return camFlags & CAM_FLAG_ACTIVE;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamSet);

/*
 * The caller's move request handed to MCamMove (MCamMove clamps a zero
 * duration to 1). Not size-preserving: master's type ended at 0xBC (mode at
 * 0xB4, duration at 0xB8); this one is reshaped (target point record 0xB0, path
 * 0xB0, mode 0xB4, duration 0xB8) and extended by the halfword at 0xBC. MCamMove_Coord copies its leading 0xB0 bytes as the target
 * point record and sets the point's fix flag when fixOnArrival (lh +0xBC)
 * is non-zero.
 */
typedef struct MCamMoveRequest {
    MCamPointParams target;
    int path;
    int mode;
    int duration;
    short fixOnArrival;
} MCamMoveRequest;

extern const char D_00A50F38[]; /* "MCamMove: frame < 1, adjusted to 1" */
extern const char D_00A50F60[]; /* "MCamMove: Invalid point" */

static void MCamMove_Coord(unsigned int pointSelector, MCamMoveRequest *request);
static void MCamMove_Bank(MCamMoveRequest *request);
static void MCamMove_Pers(MCamMoveRequest *request);

void MCamMove(unsigned int point, MCamMoveRequest *request)
{
    if (request->duration == 0) {
        MOutputDebugStringWarn(D_00A50F38);
        request->duration = 1;
    }
    switch (point) {
    case 0:
        MCamMove_Coord(0, request);
        break;
    case 1:
        MCamMove_Coord(1, request);
        break;
    case 2:
        MCamMove_Bank(request);
        break;
    case 3:
        MCamMove_Pers(request);
        break;
    default:
        MOutputDebugStringWarn(D_00A50F60);
        break;
    }
}

void MCamStopMove(unsigned int point)
{
    int moveFlag;

    switch (point) {
    case 0:
        moveFlag = CAM_FLAG_MOVE_COORD;
        break;
    case 1:
        moveFlag = CAM_FLAG_MOVE_TARGET;
        break;
    case 2:
        moveFlag = CAM_FLAG_MOVE_BANK;
        break;
    case 3:
        moveFlag = CAM_FLAG_MOVE_PERS;
        break;
    default:
        MOutputDebugStringWarn(D_00A50F98);
        return;
    }
    camFlags &= ~moveFlag;
}

int MCamIsMoving(unsigned int point)
{
    int moveFlag;

    switch (point) {
    case 0:
        moveFlag = CAM_FLAG_MOVE_COORD;
        break;
    case 1:
        moveFlag = CAM_FLAG_MOVE_TARGET;
        break;
    case 2:
        moveFlag = CAM_FLAG_MOVE_BANK;
        break;
    case 3:
        moveFlag = CAM_FLAG_MOVE_PERS;
        break;
    default:
        MOutputDebugStringWarn(D_00A50FB8);
        return 0;
    }
    return (camFlags & moveFlag) != 0;
}

void MCamShake(unsigned int mode, unsigned int amplitude, unsigned int phaseStep,
               unsigned int holdFrames, unsigned int decayFrames)
{
    MCamShakeState *shake = &camParams.shake;
    float piOverStep;
    float decayStep;

    if (amplitude == 0) {
        camFlags &= ~(1 << CAM_SHAKE_BIT);
        return;
    }

    /* 0.017453292f is pi/180 (degrees to radians) and 0.5f halves the result;
     * 1.5707964f is pi/2, divided by phaseStep to give the per-frame phase step. */
    shake->mode = mode;
    shake->amplitude = (float)amplitude * 0.017453292f * 0.5f;
    shake->decayStep = 0.0f;
    shake->offset = 0.0f;
    shake->phase = 0.0f;
    if (phaseStep != 0) {
        piOverStep = 1.5707964f / (float)phaseStep;
        shake->phaseStep = piOverStep;
    } else {
        shake->phaseStep = 0.0f;
    }
    shake->holdFrames = holdFrames;
    shake->decayFrames = decayFrames;
    if (decayFrames != 0) {
        decayStep = shake->amplitude / (float)decayFrames;
        shake->decayStep = decayStep;
    }

    camFlags |= 1 << CAM_SHAKE_BIT;
}

int MCamIsShaking(void)
{
    return (camFlags >> CAM_SHAKE_BIT) & 1;
}

/* Local in the original: splat emits `glabel MCamGetActorCoord, local`; no reference outside m_cam.c. */
static Vector4 *MCamGetActorCoord(Vector4 *coord, ActRecord *actor, int part)
{
    __asm__ __volatile__("sq $0,0(%0)" : : "r"(coord) : "memory");
    if (actor == 0) {
        MOutputDebugStringWarn(D_00A50FD8);
        return coord;
    }
    if (part == 0) {
        __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                             : : "r"(coord), "r"(&actor->position)
                             : "$8", "memory");
    } else {
        sefGetNullPosition(coord, actor, part - 1, 0);
    }
    coord->w = fmodf(actor->rotation.y + 3.1415927f, 6.283185f);
    return coord;
}

static float MCamGetAtkRange(int id);
static float MCamGetDefRange(int id);

static float MCamCalcBaseCoord(Vector4 *coord, MCamPointParams *point)
{
    Vector4 coordA, coordB;
    float length = 0.0f;

    __asm__ __volatile__("sq $0,0(%0)" : : "r"(coord) : "memory");
    switch (point->mode) {
    case MCAM_BASE_FIXED:
        MCamCopyVector(coord, &point->fixedCoord.vector);
        coord->w = 0.0f;
        break;
    case MCAM_BASE_ACTOR:
        MCamGetActorCoord(coord, point->actor, point->partSelector);
        break;
    case MCAM_BASE_RATIO:
        MCamGetActorCoord(&coordA, point->actorA, point->partSelectorA);
        MCamGetActorCoord(&coordB, point->actorB, point->partSelectorB);
        MMathVectorInterpolation(coord, &coordA, &coordB, point->ratio);
        length = MMathCalcDistXZ(&coordA, &coordB);
        coord->w = MMathCalcDir(&coordB, &coordA);
        break;
    case MCAM_BASE_RANGE: {
        float atkRange, defRange, numerator;

        MCamGetActorCoord(&coordA, point->actorA, point->partSelectorA);
        MCamGetActorCoord(&coordB, point->actorB, point->partSelectorB);
        length = MMathCalcDistXZ(&coordA, &coordB);
        atkRange = MCamGetAtkRange(point->atkUnitId);
        defRange = MCamGetDefRange(point->defUnitId);
        numerator = (length + atkRange - defRange) * 0.5f;
        if (length == 0.0f || numerator == 0.0f) {
            MCamCopyVector(coord, &coordA);
        } else {
            MMathVectorInterpolation(coord, &coordA, &coordB, numerator / length);
        }
        coord->w = MMathCalcDir(&coordB, &coordA);
        break;
    }
    }
    return length;
}

static float MCamCalcCoord(Vector4 *coord, MCamPointParams *point)
{
    MCamCoordOffset *coordOffset = &point->coordOffset;
    Vector4 polar;
    float length;

    length = MCamCalcBaseCoord(coord, point);
    switch (coordOffset->mode) {
    case MCAM_OFFSET_NONE:
        break;
    case MCAM_OFFSET_XYZ:
        MMathCalcOffsetXYZ(coord, coord, &coordOffset->offset);
        break;
    case MCAM_OFFSET_POLAR:
        polar.x = coordOffset->pitch;
        polar.y = coordOffset->yaw;
        polar.w = coordOffset->distance;
        MMathCalcOffset(coord, coord, &polar);
        break;
    }
    return length;
}

static float MCamGetAtkRange(int id)
{
    UnitInitData *unit = dataUnitInitGet(id);

    if (unit == 0) {
        MOutputDebugStringWarn(D_00A51000, id);
        return 0.0f;
    }
    return (float)unit->atkRange / 100.0f;
}

static float MCamGetDefRange(int id)
{
    UnitInitData *unit = dataUnitInitGet(id);

    if (unit == 0) {
        MOutputDebugStringWarn(D_00A51028, id);
        return 0.0f;
    }
    return (float)unit->defRange / 100.0f;
}

/* Declared in src/ov01/cmd.h; MCamExec_Control hands it a move's target point. */
extern void MCamSet(int mode, void *params);
static int MCamMoveProc_Coord(MCamCoordMove *move);
static int MCamMoveProc_Bank(MCamMoveState *state);
static int MCamMoveProc_Pers(MCamMoveState *state);
static void MCamShakeProc(void);

/*
 * Advances every running move. While a coordinate move runs, its point
 * follows the move's current coordinate; when it ends, the target goes to
 * MCamSet and, with the fix flag, the point is pinned to the camera's own
 * coordinate with no offset.
 */
static void MCamExec_Control(void)
{
    if (camFlags & CAM_FLAG_MOVE_COORD) {
        MCamCoordMove *move = &camParams.coordMove[0];

        if (MCamMoveProc_Coord(move)) {
            camParams.point[0].fixedCoord = camParams.coordMove[0].currentCoord;
        } else {
            MCamSet(0, &move->target);
            if (camFlags & CAM_FLAG_FIX_COORD) {
                camParams.point[0].mode = MCAM_BASE_FIXED;
                MCamCopyVector(&camParams.point[0].fixedCoord.vector, &camParams.coord[0]);
                camParams.point[0].coordOffset.mode = MCAM_OFFSET_NONE;
            }
            camFlags &= ~(CAM_FLAG_MOVE_COORD | CAM_FLAG_FIX_COORD);
        }
    }
    if (camFlags & CAM_FLAG_MOVE_TARGET) {
        MCamCoordMove *move = &camParams.coordMove[1];

        if (MCamMoveProc_Coord(move)) {
            camParams.point[1].fixedCoord = camParams.coordMove[1].currentCoord;
        } else {
            MCamSet(1, &move->target);
            if (camFlags & CAM_FLAG_FIX_TARGET) {
                camParams.point[1].mode = MCAM_BASE_FIXED;
                MCamCopyVector(&camParams.point[1].fixedCoord.vector, &camParams.coord[1]);
                camParams.point[1].coordOffset.mode = MCAM_OFFSET_NONE;
            }
            camFlags &= ~(CAM_FLAG_MOVE_TARGET | CAM_FLAG_FIX_TARGET);
        }
    }
    if (camFlags & CAM_FLAG_MOVE_BANK) {
        if (!MCamMoveProc_Bank(&camParams.bankMove)) {
            camFlags &= ~CAM_FLAG_MOVE_BANK;
        }
        camParams.bank = camParams.bankMove.current;
    }
    if (camFlags & CAM_FLAG_MOVE_PERS) {
        if (!MCamMoveProc_Pers(&camParams.perspectiveMove)) {
            camFlags &= ~CAM_FLAG_MOVE_PERS;
        }
        camParams.perspectiveStart = camParams.perspectiveMove.current;
    }
    if (camFlags & (1 << CAM_SHAKE_BIT)) {
        MCamShakeProc();
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_SetParams);

static void MCamExec_SetParams_Base(void)
{
    camParams.actorLength[0] = MCamCalcBaseCoord(&camParams.baseCoord[0], &camParams.point[0]);
    camParams.actorLength[1] = MCamCalcBaseCoord(&camParams.baseCoord[1], &camParams.point[1]);
    MCamCalcCoord(&camParams.coord[0], &camParams.point[0]);
    MCamCalcCoord(&camParams.coord[1], &camParams.point[1]);
    MMathCalcAngleCam(&camParams.angle[0], &camParams.coord[0], &camParams.coord[1]);
    __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                         : : "r"(&camParams.rotation), "r"(&camParams.angle[0])
                         : "$8", "memory");
    camParams.rotation.z = camParams.bank;
    MMathCalcAngleCam(&camParams.angle[1], &camParams.coord[1], &camParams.coord[0]);
}

static void MCamExec_DispParams(void)
{
}

static void MCamMove_Coord(unsigned int pointSelector, MCamMoveRequest *request)
{
    MCamCoordMove *move;
    MCamPointParams *params;
    MCamPointParams *other;
    int moveFlag;
    int fixFlag;
    int path;
    int mode;
    int duration;

    if (pointSelector == 0) {
        moveFlag = CAM_FLAG_MOVE_COORD;
        move = &camParams.coordMove[0];
        fixFlag = CAM_FLAG_FIX_COORD;
        other = &camParams.point[1];
        params = &camParams.point[0];
    } else {
        moveFlag = CAM_FLAG_MOVE_TARGET;
        move = &camParams.coordMove[1];
        fixFlag = CAM_FLAG_FIX_TARGET;
        other = &camParams.point[0];
        params = &camParams.point[1];
    }

    move->start = *params;
    move->target = request->target;
    MCamCalcCoord(&move->pivot, other);
    MCamCalcCoord(&move->startCoord, &move->start);
    MCamCalcCoord(&move->endCoord, &move->target);
    MCamCopyVector(&move->currentCoord.vector, &move->startCoord);
    duration = request->duration;
    path = request->path;
    mode = request->mode;
    move->path = path;
    move->mode = mode;
    move->duration = duration;
    move->elapsed = 0;

    /* Start the point as a fixed coordinate at the current one, with no offset. */
    params->mode = MCAM_BASE_FIXED;
    MCamCopyVector(&params->fixedCoord.vector, &move->currentCoord.vector);
    params->coordOffset.mode = MCAM_OFFSET_NONE;
    camFlags |= moveFlag;
    if (request->fixOnArrival != 0) {
        camFlags |= fixFlag;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMove_Bank);

static void MCamMove_Pers(MCamMoveRequest *request)
{
    MCamMoveState *state = &D_00A5B5F8;
    int mode;
    int duration;
    float angle;
    float angleRad;
    float savedStart;

    mode = request->mode;
    duration = request->duration;
    /* MCamMove_Pers's start value is the float at state - 0x384. */
    state->start = camParams.perspectiveStart;
    state->mode = mode;
    angle = request->target.perspectiveAngle;
    state->duration = duration;
    state->elapsed = 0;
    if (angle != 0.0f) {
        angleRad = angle * 0.017453292f;
    } else {
        angleRad = 0.6981317f;
    }
    savedStart = state->start;
    state->end = angleRad;
    state->current = savedStart;
    camFlags |= 0x800;
}


static float MCamMoveProc_CalcRatio(unsigned int mode, unsigned int elapsed, unsigned int duration)
{
    float ratio;

    if (duration == 0) {
        MOutputDebugStringWarn(zero_duration_message, mode);
        return 0.0f;
    }

    ratio = (float)elapsed / (float)duration;

    switch (mode) {
    case 1: {
        float sine1;
        ratio = ratio * 3.14159265f + 4.712389f;
        __asm__ __volatile__(
            "mfc1    $8, %1\n\t"
            "qmtc2   $8, $vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8, $vf1\n\t"
            "mtc1    $8, %0\n\t"
            : "=f"(sine1)
            : "f"(ratio)
            : "$8", "memory"
        );
        ratio = (sine1 + 1.0f) * 0.5f;
        break;
    }
    case 0:
        break;
    case 2: {
        float sine2;
        ratio = ratio * 1.57079633f + 4.712389f;
        __asm__ __volatile__(
            "mfc1    $8, %1\n\t"
            "qmtc2   $8, $vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8, $vf1\n\t"
            "mtc1    $8, %0\n\t"
            : "=f"(sine2)
            : "f"(ratio)
            : "$8", "memory"
        );
        ratio = sine2 + 1.0f;
        break;
    }
    case 3: {
        float sine3;
        ratio = ratio * 1.57079633f;
        __asm__ __volatile__(
            "mfc1    $8, %1\n\t"
            "qmtc2   $8, $vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8, $vf1\n\t"
            "mtc1    $8, %0\n\t"
            : "=f"(sine3)
            : "f"(ratio)
            : "$8", "memory"
        );
        ratio = sine3;
        break;
    }
    default:
        MOutputDebugStringWarn(invalid_interpolation_message);
        break;
    }

    return ratio;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMoveProc_Coord);

static int MCamMoveProc_Bank(MCamMoveState *state)
{
    float start;
    float ratio;

    ratio = MCamMoveProc_CalcRatio(state->mode, ++state->elapsed, state->duration);
    start = state->start;
    state->current = start + (state->end - start) * ratio;
    return state->elapsed < state->duration;
}

static int MCamMoveProc_Pers(MCamMoveState *state)
{
    float start;
    float ratio;

    ratio = MCamMoveProc_CalcRatio(state->mode, ++state->elapsed, state->duration);
    start = state->start;
    state->current = start + (state->end - start) * ratio;
    return state->elapsed < state->duration;
}

static void MCamShakeProc(void)
{
    MCamShakeState *shake = &camParams.shake;

    if (shake->holdFrames != 0) {
        shake->holdFrames--;
        if (shake->phaseStep != 0.0f) {
            shake->phase = fmodf(shake->phase + shake->phaseStep, 6.283185f);
            shake->offset = shake->amplitude * sinf(shake->phase);
        } else {
            /* No phase step: flip the offset to the other side. */
            shake->offset = (shake->offset < 0.0f) ? shake->amplitude : -shake->amplitude;
        }
    } else if (shake->decayFrames != 0) {
        shake->decayFrames--;
        shake->amplitude -= shake->decayStep;
        if (shake->phaseStep != 0.0f) {
            shake->phase = fmodf(shake->phase + shake->phaseStep, 6.283185f);
            shake->offset = shake->amplitude * sinf(shake->phase);
        } else {
            /* No phase step: flip the offset to the other side. */
            shake->offset = (shake->offset < 0.0f) ? shake->amplitude : -shake->amplitude;
        }
    } else {
        shake->offset = 0.0f;
        camFlags &= ~(1 << CAM_SHAKE_BIT);
    }
}
