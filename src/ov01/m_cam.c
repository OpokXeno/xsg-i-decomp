/*
 * OV01 original TU 17: 0x00a314b8..0x00a32db8 (36 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/data_unit_org_get.h"
#include "main/act_1.h"
#include "quadword.h"
#include "vu0_macro.h"

extern void MOutputDebugStringWarn(const char *format, ...);
/* The message body after the ASCII prefix is encoded in EUC-JP. */
/* Reads: MCamReleaseControl: The camera is not under control */
const char camMCamReleaseControlText[48] =
    "MCamReleaseControl: \xA5\xAB\xA5\xE1\xA5\xE9\xA4\xCF\xC0\xA9\xB8\xE6\xC3\xE6\xA4\xC7\xA4\xCF\xA4\xCA\xA4\xA4";
const char camMCamGetAtkRangeInvalidIDText[40] = "MCamGetAtkRange: Invalid ID (%d)";
const char camMCamGetDefRangeInvalidIDText[40] = "MCamGetDefRange: Invalid ID (%d)";
const char zero_duration_message[40] = "MCamMoveProc_CalcRatio: d == 0 (IP:%d)";
const char invalid_interpolation_message[40] = "MCamMoveProc_CalcRatio: Invalid IPTYPE";

extern const char camMCamStopMoveInvalidPointText[]; /* "MCamStopMove: Invalid point" */
extern const char camMCamIsMovingInvalidPointText[]; /* "MCamIsMoving: Invalid point" */
extern const char camMCamGetActorCoordActorIsNULLText[]; /* "MCamGetActorCoord: actor is NULL" */

extern void bcopy(const void *source, void *destination, unsigned int count);
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
extern Vector4 *MMathCalcDirVector(Vector4 *out, Vector4 *from, Vector4 *to);
extern Vector4 *MMathCalcAngle(Vector4 *out, const Vector4 *from, const Vector4 *to);
extern float MMathCalcDist(const Vector4 *first, const Vector4 *second);
extern float MMathCalcRotNear(float first, float second);
extern StudioCamera *xglStudioGetCamera2(int camera_id);
extern void xglCameraInit(StudioCamera *camera);
extern const char camMCamTakeControlText[]; /* "MCamTakeControl: ..." */
extern const char camMCamSetInvalidPointText[]; /* "MCamSet: Invalid point" */
extern const char camMCamMoveFrame1AdjustedTo1Text[]; /* "MCamMove: frame < 1, adjusted to 1" */
extern const char camMCamMoveInvalidPointPOVPLABANKText[]; /* "MCamMove: Invalid point" */
/*
 * The interpolation curve MCamMoveProc_CalcRatio applies to a move's ratio
 * (its "Invalid IPTYPE" message names the selector; the cases are its
 * switch: linear, (1 - cos(pi r)) / 2, 1 - cos(pi r / 2), sin(pi r / 2)).
 */
typedef enum MCamIpType {
    MCAM_IP_LINEAR = 0,
    MCAM_IP_SINE_IN_OUT = 1,
    MCAM_IP_SINE_IN = 2,
    MCAM_IP_SINE_OUT = 3
} MCamIpType;

/*
 * The point selector MCamSet, MCamMove, MCamStopMove and MCamIsMoving switch on.
 * 0 and 1 are the two camera points (coordinate moves, CAM_FLAG_MOVE_COORD and
 * CAM_FLAG_MOVE_TARGET), 2 the bank and 3 the perspective angle.
 */
enum MCamPoint {
    MCAM_POINT_COORD = 0,
    MCAM_POINT_TARGET = 1,
    MCAM_POINT_BANK = 2,
    MCAM_POINT_PERS = 3
};

/*
 * Linear interpolation state shared by the per-parameter MoveProc callbacks
 * (mode/duration/elapsed feed MCamMoveProc_CalcRatio; start/end/current hold
 * the eased scalar value). The counters compare unsigned (`sltu $2,$2,$3`,
 * 0x00a32bf8 in MCamMoveProc_Bank).
 */
typedef struct MCamMoveState {
    MCamIpType mode;
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
    unsigned char unmodeled_2C[0x4]; /* ends at MCamPointParams +0xA0, where bank sits */
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
 * Size-preserving for the type: 0xB0 by member arithmetic (fixedCoord
 * +0x10, ratio +0x6C, coordOffset +0x70..0xA4, perspectiveAngle +0xA4, tail
 * to 0xB0).
 */
typedef struct MCamPointParams {
    unsigned int mode;
    unsigned char unmodeled_04[0xC];
    MCamVector fixedCoord;
    Actor *actor;
    unsigned char unmodeled_24[0x4];
    int partSelector;
    Actor *actorA;
    int atkUnitId;
    int partSelectorA;
    Actor *actorB;
    int defUnitId;
    int partSelectorB;
    unsigned char unmodeled_44[0x28];
    float ratio;
    MCamCoordOffset coordOffset;
    /* Bank angle: MCamMove_Bank reads a request's target bank here
     * (`lwc1 $f1,0xA0($4)`, 0x00a3277c); MCamSet mode 2 copies the same word
     * to camParams.bank (`lwc1 $f0,0xA0($5)`, 0x00a31a78). */
    float bank;
    /* MCamMove_Pers reads a request's perspective angle (degrees) here. */
    float perspectiveAngle;
    unsigned char unmodeled_A8[0x8];
} MCamPointParams;

/*
 * Coordinate move state of one point, filled by MCamMove_Coord: the point
 * record at the start and at the target, the other point's coordinate as
 * pivot, the start, end and current coordinates, then the request's path,
 * mode and duration and the elapsed frames (cleared). MCamMoveProc_Coord
 * consumes them: it branches three ways on path (0, 1, other:
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
    /* MCamExec_Control passes &camParams + 0x560 (camControlParams) to
     * MCamMoveProc_Bank and copies its current (+0x14, `lwc1 $f0,0x574`)
     * to bank. */
    MCamMoveState bankMove;
    MCamMoveState perspectiveMove;
    MCamShakeState shake;
} MCamParameterStorage;

static StudioCamera *sysCam;
/* MCamTakeControl saves the StudioCamera returned by xglStudioGetCamera2. */
static StudioCamera sysCamBuff;

/* Bits of camFlags. MCamMove_Coord picks CAM_FLAG_FIX_COORD (point 0) or
 * CAM_FLAG_FIX_TARGET (point 1) at its head (`addiu $21,$0,0x2000` at
 * 0x00a325e0, `addiu $21,$0,0x4000` at 0x00a325fc) and ORs it in
 * (`or $2,$2,$21`, 0x00a32728) when the request's fixOnArrival halfword
 * (`lh $3,0xBC($18)`, 0x00a3270c) is nonzero, after the move bit
 * (`or $2,$2,$22`, 0x00a3271c); the bits the older functions test keep
 * their names. Enum constants, not macros, as the project publishes them
 * (CALC_NORMAL_TECHNIQUE_COUNT). camFlags has this type because an int camFlags
 * shares the alias class of MCamMoveState's counters, and `sw $3,0($5)` then
 * schedules ahead of `swc1 $f0,0x14($2)` (0x00a32784, MCamMove_Bank). */
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

/* MCamShake's mode selects the rotation axes the shake offsets (rotation[0..2]). */
#define CAM_SHAKE_AXIS_X 0x1
#define CAM_SHAKE_AXIS_Y 0x10
#define CAM_SHAKE_AXIS_Z 0x100

/* Angle constants: degrees to radians, the 40 degree default perspective, and 2 pi. */
#define CAM_DEG_TO_RAD 0.017453292f
#define CAM_DEFAULT_PERSPECTIVE 0.6981317f
#define CAM_TWO_PI 6.283185f

static enum CamFlag camFlags;
static MCamParameterStorage camParams;

/* Original local labels are interior views in camParams, not extra storage. */
#define camActorLengthFirst (camParams.actorLength[0])
#define camActorLengthSecond (camParams.actorLength[1])
#define camPerspectiveMove (camParams.perspectiveMove)

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
        length = camActorLengthFirst;
        break;
    case 1:
        length = camActorLengthSecond;
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

/*
 * Saves the studio camera and starts from it: point 0 sits at the camera's
 * position, point 1 one unit along the direction its rotation gives. The
 * perspective starts at 40 degrees (0.6981317 radians).
 */
int MCamTakeControl(void)
{
    StudioCamera *saved;
    MCamVector direction;
    float sinY;
    float cosX;
    float sinX;
    float cosY;
    float cosX2;
    u64 directionLow;
    u64 directionHigh;

    if (camFlags & CAM_FLAG_ACTIVE) {
        MOutputDebugStringWarn(camMCamTakeControlText);
        return 0;
    }
    sysCam = xglStudioGetCamera2(0);
    saved = &sysCamBuff;
    bcopy(sysCam, &sysCamBuff, sizeof(sysCamBuff));
    xglCameraInit(sysCam);
    sysCam->active = 1;
    sysCam->state = 1;
    memset(&camParams, 0, sizeof(camParams));
    camParams.point[0].mode = MCAM_BASE_FIXED;
    camParams.point[0].fixedCoord.pair[0] = saved->position.words[0];
    camParams.point[0].fixedCoord.pair[1] = saved->position.words[1];
    camParams.point[0].coordOffset.mode = MCAM_OFFSET_NONE;
    Vu0Sin(sinY, saved->rotation.vector.y);
    Vu0Cos(cosX, saved->rotation.vector.x);
    direction.vector.x = -sinY * cosX;
    Vu0Sin(sinX, saved->rotation.vector.x);
    direction.vector.y = sinX;
    Vu0Cos(cosY, saved->rotation.vector.y);
    Vu0Cos(cosX2, saved->rotation.vector.x);
    direction.vector.z = -cosY * cosX2;
    Vu0SefAddVectorXYZ(&direction, &direction, &saved->position);
    directionLow = direction.pair[0];
    directionHigh = direction.pair[1];
    camFlags |= CAM_FLAG_ACTIVE;
    camParams.perspectiveStart = CAM_DEFAULT_PERSPECTIVE;
    camParams.point[1].fixedCoord.pair[0] = directionLow;
    camParams.point[1].fixedCoord.pair[1] = directionHigh;
    camParams.point[1].coordOffset.mode = MCAM_OFFSET_NONE;
    camParams.point[1].mode = MCAM_BASE_FIXED;
    return 1;
}

int MCamReleaseControl(void)
{
    if (!(camFlags & CAM_FLAG_ACTIVE)) {
        MOutputDebugStringWarn(camMCamReleaseControlText);
        return 0;
    }

    bcopy(&sysCamBuff, sysCam, sizeof(sysCamBuff));
    camFlags &= ~CAM_FLAG_ACTIVE;
    return 1;
}

void MCamStoreParams(void)
{
    StudioCamera *camera;
    MCamParameterStorage *params;
    float bank;
    float perspective;

    QuadwordCopyXyzT0(&sysCamBuff.position, &camParams.coord[0]);
    QuadwordCopyXyzT0(&sysCamBuff.rotation, &camParams.rotation);
    camera = &sysCamBuff;
    params = &camParams;
    bank = params->bank;
    perspective = params->perspectiveStart;
    camera->fov = perspective;
    camera->rotation.vector.z = bank;
}

int MCamIsActive(void)
{
    return camFlags & CAM_FLAG_ACTIVE;
}

/*
 * MCamSet's point record: the first 0xA0 bytes of a MCamPointParams (the
 * base block of 0x70 bytes and the coordinate-offset tail of 0x30 bytes),
 * then the bank word at +0xA0 and the perspective angle word at +0xA4.
 * Modes 0 and 1 copy the first 0xA0 bytes into camParams.point[0] or [1]
 * (the rest of each point record is left alone); mode 2 stores the bank,
 * mode 3 the perspective angle converted from degrees to radians (0.6981317
 * radians, 40 degrees, when the angle is zero).
 */
typedef struct MCamSetBlock {
    long long v[14];
} MCamSetBlock;
typedef struct MCamSetTail {
    long long v[6];
} MCamSetTail;
typedef struct MCamSetPoint {
    MCamSetBlock base;
    MCamSetTail tail;
    float fA0;
    float fA4;
    unsigned char pad[8];
} MCamSetPoint;
typedef struct MCamSetParamsView {
    MCamSetBlock p0;
    MCamSetTail t0;
    unsigned char pad_a0[0x10];
    MCamSetBlock p1;
    MCamSetTail t1;
    unsigned char pad_150[0xA4];
    float pers;
} MCamSetParamsView;

static void MCamExec_SetParams_Base(void);

void MCamSet(unsigned int point, MCamPointParams *params)
{
    MCamSetPoint *src = (MCamSetPoint *)params;
    float f;
    MCamSetParamsView *view;
    switch (point) {
    case MCAM_POINT_COORD:
        ((MCamSetParamsView *)&camParams)->p0 = src->base;
        ((MCamSetParamsView *)&camParams)->t0 = src->tail;
        break;
    case MCAM_POINT_TARGET:
        ((MCamSetParamsView *)&camParams)->p1 = src->base;
        ((MCamSetParamsView *)&camParams)->t1 = src->tail;
        break;
    case MCAM_POINT_BANK:
        camParams.bank = src->fA0;
        break;
    case MCAM_POINT_PERS:
        view = (MCamSetParamsView *)&camParams;
        if (src->fA4 != 0.0f) {
            f = src->fA4 * CAM_DEG_TO_RAD;
        } else {
            f = CAM_DEFAULT_PERSPECTIVE;
        }
        view->pers = f;
        break;
    default:
        MOutputDebugStringWarn(camMCamSetInvalidPointText);
        return;
    }
    MCamExec_SetParams_Base();
}

/*
 * The caller's move request handed to MCamMove (MCamMove clamps a zero
 * duration to 1). Not size-preserving: the type ends at 0xBC (mode at
 * 0xB4, duration at 0xB8); this one is reshaped (target point record 0xB0, path
 * 0xB0, mode 0xB4, duration 0xB8) and extended by the halfword at 0xBC. MCamMove_Coord copies its leading 0xB0 bytes as the target
 * point record and sets the point's fix flag when fixOnArrival (lh +0xBC)
 * is non-zero. mode is the MCamIpType selector MCamMove_Bank and MCamMove_Pers
 * copy to MCamMoveState.mode. The original loads duration first (`lw $7,0xB8($4)`
 * at 0x00a32774, `lw $6,0xB4($4)` at 0x00a32780); as an int, mode loads first.
 */
typedef struct MCamMoveRequest {
    MCamPointParams target;
    int path;
    MCamIpType mode;
    int duration;
    short fixOnArrival;
} MCamMoveRequest;

static void MCamMove_Coord(unsigned int pointSelector, MCamMoveRequest *request);
static void MCamMove_Bank(MCamMoveRequest *request);
static void MCamMove_Pers(MCamMoveRequest *request);

void MCamMove(unsigned int point, MCamMoveRequest *request)
{
    if (request->duration == 0) {
        MOutputDebugStringWarn(camMCamMoveFrame1AdjustedTo1Text);
        request->duration = 1;
    }
    switch (point) {
    case MCAM_POINT_COORD:
        MCamMove_Coord(MCAM_POINT_COORD, request);
        break;
    case MCAM_POINT_TARGET:
        MCamMove_Coord(MCAM_POINT_TARGET, request);
        break;
    case MCAM_POINT_BANK:
        MCamMove_Bank(request);
        break;
    case MCAM_POINT_PERS:
        MCamMove_Pers(request);
        break;
    default:
        MOutputDebugStringWarn(camMCamMoveInvalidPointPOVPLABANKText);
        break;
    }
}

void MCamStopMove(unsigned int point)
{
    int moveFlag;

    switch (point) {
    case MCAM_POINT_COORD:
        moveFlag = CAM_FLAG_MOVE_COORD;
        break;
    case MCAM_POINT_TARGET:
        moveFlag = CAM_FLAG_MOVE_TARGET;
        break;
    case MCAM_POINT_BANK:
        moveFlag = CAM_FLAG_MOVE_BANK;
        break;
    case MCAM_POINT_PERS:
        moveFlag = CAM_FLAG_MOVE_PERS;
        break;
    default:
        MOutputDebugStringWarn(camMCamStopMoveInvalidPointText);
        return;
    }
    camFlags &= ~moveFlag;
}

int MCamIsMoving(unsigned int point)
{
    int moveFlag;

    switch (point) {
    case MCAM_POINT_COORD:
        moveFlag = CAM_FLAG_MOVE_COORD;
        break;
    case MCAM_POINT_TARGET:
        moveFlag = CAM_FLAG_MOVE_TARGET;
        break;
    case MCAM_POINT_BANK:
        moveFlag = CAM_FLAG_MOVE_BANK;
        break;
    case MCAM_POINT_PERS:
        moveFlag = CAM_FLAG_MOVE_PERS;
        break;
    default:
        MOutputDebugStringWarn(camMCamIsMovingInvalidPointText);
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

    /* CAM_DEG_TO_RAD is pi/180 (degrees to radians) and 0.5f halves the result;
     * 1.5707964f is pi/2, divided by phaseStep to give the per-frame phase step. */
    shake->mode = mode;
    shake->amplitude = (float)amplitude * CAM_DEG_TO_RAD * 0.5f;
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
    /* Arithmetic shift (`sra $2,$2,12`, 0x00a31e10); enum CamFlag is unsigned. */
    return ((int)camFlags >> CAM_SHAKE_BIT) & 1;
}

/* Local in the original: splat emits `glabel MCamGetActorCoord, local`; no use outside m_cam.c. */
static Vector4 *MCamGetActorCoord(Vector4 *coord, Actor *actor, int part)
{
    __asm__ __volatile__("sq $0,0(%0)" : : "r"(coord) : "memory");
    if (actor == 0) {
        MOutputDebugStringWarn(camMCamGetActorCoordActorIsNULLText);
        return coord;
    }
    if (part == 0) {
        __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                             : : "r"(coord), "r"(&actor->position)
                             : "$8", "memory");
    } else {
        sefGetNullPosition(coord, actor, part - 1, 0);
    }
    coord->w = fmodf(actor->rotation.y + 3.1415927f, CAM_TWO_PI);
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
        MOutputDebugStringWarn(camMCamGetAtkRangeInvalidIDText, id);
        return 0.0f;
    }
    return (float)unit->atkRange / 100.0f;
}

static float MCamGetDefRange(int id)
{
    UnitInitData *unit = dataUnitInitGet(id);

    if (unit == 0) {
        MOutputDebugStringWarn(camMCamGetDefRangeInvalidIDText, id);
        return 0.0f;
    }
    return (float)unit->defRange / 100.0f;
}

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

/*
 * Publishes the frame to the studio camera: eye direction from the two points,
 * then coordinate, rotation (with the shake offset on the axes shake.mode
 * selects: bits 0, 4 and 8) and field of view.
 */
static void MCamExec_SetParams(void)
{
    float rotation[3];
    int axes;

    MCamExec_SetParams_Base();
    MMathCalcDirVector(&camParams.eye, &camParams.coord[0], &camParams.coord[1]);
    QuadwordCopyXyzT0(rotation, &camParams.rotation);
    if (camFlags & (1 << CAM_SHAKE_BIT)) {
        axes = camParams.shake.mode;
        if (axes & CAM_SHAKE_AXIS_X) {
            rotation[0] += camParams.shake.offset;
        }
        if (axes & CAM_SHAKE_AXIS_Y) {
            rotation[1] += camParams.shake.offset;
        }
        if (axes & CAM_SHAKE_AXIS_Z) {
            rotation[2] += camParams.shake.offset;
        }
    }
    QuadwordCopyXyzT0(&sysCam->position, &camParams.coord[0]);
    QuadwordCopyXyzT0(&sysCam->rotation, rotation);
    sysCam->fov = camParams.perspectiveStart;
}

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

/*
 * Starts the bank (roll) move from the current bank to the request's target
 * bank. The start store precedes the request's bank load (`swc1 $f0,0xC($2)`
 * 0x00a32770, `lwc1 $f1,0xA0($4)` 0x00a3277c: both float class).
 */
static void MCamMove_Bank(MCamMoveRequest *request)
{
    MCamMoveState *state = &camParams.bankMove;
    float start;
    int duration;
    MCamIpType mode;
    float end;

    start = camParams.bank;
    state->start = start;
    mode = request->mode;
    duration = request->duration;
    end = request->target.bank;
    state->mode = mode;
    state->duration = duration;
    state->end = end;
    state->elapsed = 0;
    state->current = start;
    camFlags |= CAM_FLAG_MOVE_BANK;
}

static void MCamMove_Pers(MCamMoveRequest *request)
{
    MCamMoveState *state = &camPerspectiveMove;
    MCamIpType mode;
    int duration;
    float angle;
    float angleRad;
    float savedStart;

    mode = request->mode;
    duration = request->duration;
    /* The start is the perspective angle the camera currently uses. */
    state->start = camParams.perspectiveStart;
    state->mode = mode;
    angle = request->target.perspectiveAngle;
    state->duration = duration;
    state->elapsed = 0;
    if (angle != 0.0f) {
        angleRad = angle * CAM_DEG_TO_RAD;
    } else {
        angleRad = CAM_DEFAULT_PERSPECTIVE;
    }
    savedStart = state->start;
    state->end = angleRad;
    state->current = savedStart;
    camFlags |= CAM_FLAG_MOVE_PERS;
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
    case MCAM_IP_SINE_IN_OUT: {
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
    case MCAM_IP_LINEAR:
        break;
    case MCAM_IP_SINE_IN: {
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
    case MCAM_IP_SINE_OUT: {
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

/* One helper: the original shares one add tail between both paths, which gcc's
 * cross-jumping only produces from a single source line (as MCamCopyVector). */
static __inline__ void MCamAddDrift(MCamCoordMove *move, Vector4 *coord, Vector4 *drift)
{
    Vu0SefAddVectorXYZ(&move->currentCoord, coord, drift);
}

/*
 * One frame of a coordinate move. The target may itself be moving: the
 * difference between its current and its stored end coordinate, scaled by the
 * eased ratio, is added to the path's point. Path 0 is straight, path 1 an arc
 * around the pivot's x, y and z.
 */
static int MCamMoveProc_Coord(MCamCoordMove *move)
{
    Vector4 drift;
    Vector4 origin;
    Vector4 coord;
    Vector4 difference;
    Vector4 offset;
    float ratio;

    ratio = MCamMoveProc_CalcRatio(move->mode, ++move->elapsed, move->duration);
    MCamCalcCoord(&coord, &move->target);
    Vu0SefSubVectorXYZ(&difference, &coord, &move->endCoord);
    Vu0SefScaleVectorXYZNI(&drift, &difference, ratio);
    switch (move->path) {
    case 0:
        MMathVectorInterpolation(&coord, &move->startCoord, &move->endCoord, ratio);
        MCamAddDrift(move, &coord, &drift);
        break;
    case 1:
        QuadwordZero(&origin);
        QuadwordCopyXyzT0(&origin, &move->pivot);
        MMathCalcAngle(&coord, &origin, &move->startCoord);
        coord.w = MMathCalcDist(&origin, &move->startCoord);
        MMathCalcAngle(&difference, &origin, &move->endCoord);
        difference.w = MMathCalcDist(&origin, &move->endCoord);
        QuadwordZero(&offset);
        offset.x = coord.x + MMathCalcRotNear(coord.x, difference.x) * ratio;
        offset.y = coord.y + MMathCalcRotNear(coord.y, difference.y) * ratio;
        offset.w = coord.w + (difference.w - coord.w) * ratio;
        MMathCalcOffset(&coord, &origin, &offset);
        MCamAddDrift(move, &coord, &drift);
        break;
    }
    return (unsigned int)move->elapsed < (unsigned int)move->duration;
}

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
            shake->phase = fmodf(shake->phase + shake->phaseStep, CAM_TWO_PI);
            shake->offset = shake->amplitude * sinf(shake->phase);
        } else {
            /* No phase step: flip the offset to the other side. */
            shake->offset = (shake->offset < 0.0f) ? shake->amplitude : -shake->amplitude;
        }
    } else if (shake->decayFrames != 0) {
        shake->decayFrames--;
        shake->amplitude -= shake->decayStep;
        if (shake->phaseStep != 0.0f) {
            shake->phase = fmodf(shake->phase + shake->phaseStep, CAM_TWO_PI);
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



const char camMCamTakeControlText[40] = "MCamTakeControl: \264\373\244\313\245\253\245\341\245\351\244\317\300\251\270\346\303\346";

const char camMCamSetInvalidPointText[24] = "MCamSet: Invalid point";

const char camMCamMoveFrame1AdjustedTo1Text[40] = "MCamMove: frame < 1, adjusted to 1";

const char camMCamMoveInvalidPointPOVPLABANKText[56] = "MCamMove: Invalid point\000POV\000\000\000\000\000PLA\000\000\000\000\000BANK\000\000\000\000PERS";

const char camMCamStopMoveInvalidPointText[32] = "MCamStopMove: Invalid point";

const char camMCamIsMovingInvalidPointText[32] = "MCamIsMoving: Invalid point";

const char camMCamGetActorCoordActorIsNULLText[40] = "MCamGetActorCoord: actor is NULL";
