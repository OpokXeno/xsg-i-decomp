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
 * Declared float-returning here: MCamCalcBaseCoord reads the srsAtan2 result that
 * MMathCalcDir leaves in $f0. The definition in src/main/m_math.c returns void, so
 * this is an ABI workaround, not a claim that the two signatures agree.
 */
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
 * the eased scalar value). The counters are compared unsigned.
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
    /* MCamShake's first argument, stored at +0x00. MCamShakeProc reads
     * +0x04..+0x1C and never this word, so what it selects is not established. */
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
 * How MCamCalcCoord offsets the base coordinate. MCamMove_Coord stores zero to
 * this mode between the request's fixOnArrival halfword load and the camFlags
 * load. The enum type keeps that store in its original order: with an unsigned
 * int field the compiler moved it above the halfword load. The enum is a codegen
 * choice, not a recovered original type.
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
 * aligned ld/sd loops, which the u64 pair produces; a plain Vector4 copy compiles
 * to ldl/ldr/sdl/sdr instead. The pair carries no meaning of its own.
 */
typedef union MCamVector {
    Vector4 vector;
    u64 pair[2];
} MCamVector;

/*
 * The selector MCamCalcBaseCoord switches on for a point's base coordinate. It is
 * a compare chain, not a jump table. The enum is used for its constants only; the
 * field stays an unsigned int, since an enum-typed field changes the compiler's
 * scheduling (see MCamOffsetMode).
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
 * The struct is 0xB0 bytes (fixedCoord at +0x10, ratio at +0x6C, coordOffset at
 * +0x70..0xA0, perspectiveAngle at +0xA4, tail to 0xB0).
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
    /* Bank angle: MCamMove_Bank reads a request's target bank here, and
     * MCamSet mode 2 copies the same word to camParams.bank. */
    float bank;
    /* MCamMove_Pers reads a request's perspective angle (degrees) here. */
    float perspectiveAngle;
    unsigned char unmodeled_A8[0x8];
} MCamPointParams;

/*
 * Coordinate move state of one point, filled by MCamMove_Coord: the point
 * records at the start and at the target, the other point's coordinate as
 * pivot, the start, end and current coordinates, then the request's path,
 * mode and duration, and the elapsed frames (cleared). MCamMoveProc_Coord
 * branches on path (0, 1, other), increments elapsed, and passes mode, elapsed
 * and duration to MCamMoveProc_CalcRatio. The names path and mode are
 * MCamMoveRequest's for the same two words; what the values select is not
 * established.
 */
typedef struct MCamCoordMove {
    MCamPointParams start;
    MCamPointParams target;
    Vector4 pivot;
    Vector4 startCoord;
    Vector4 endCoord;
    /* MCamVector: MCamExec_Control copies it whole into a point's fixedCoord.
     * The union type keeps that copy as a 64-bit load/store pair. */
    MCamVector currentCoord;
    int path;
    int mode;
    int duration;
    int elapsed;
} MCamCoordMove;

/*
 * MCamInit clears this 0x5B0-byte camera-parameter store. The two point records,
 * the base coordinates, coordinates, angles, eye and rotation are the views
 * MCamExec_SetParams_Base and the getters address; the other bytes remain part of
 * the live store, but their individual meanings are not recovered yet.
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
    /* MCamExec_Control passes &camParams + 0x560 to MCamMoveProc_Bank and
     * copies its current value (+0x14) to bank. */
    MCamMoveState bankMove;
    MCamMoveState perspectiveMove;
    MCamShakeState shake;
} MCamParameterStorage;

static StudioCamera *sysCam;
/* MCamTakeControl saves the StudioCamera returned by xglStudioGetCamera2. */
static StudioCamera sysCamBuff;

/* Bits of camFlags. MCamMove_Coord ORs in CAM_FLAG_FIX_COORD (point 0) or
 * CAM_FLAG_FIX_TARGET (point 1) when the request's fixOnArrival halfword is
 * nonzero, after the move bit. Enum constants, not macros. camFlags has this
 * type because an int camFlags shares the alias class of MCamMoveState's
 * counters, which changes how MCamMove_Bank schedules its stores. */
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

/* Copies the base coordinate of point 0 or 1 into baseCoord and returns it. Other
 * values give zero. */
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

/*
 * Copies the current coordinate of point 0 or 1 into coord and returns it, with w
 * set to 1.0f. For other values the result stays VU0's $vf0 constant (0, 0, 0, 1).
 */
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

/* Copies the angle of point 0 or 1 into angle and returns it. Other values give zero. */
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

/* Returns the length of actor 0 or 1, or 0 for other selectors. */
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
 * The caller's move request handed to MCamMove, which clamps a zero duration
 * to 1. The target point record fills the first 0xB0 bytes, followed by path
 * (0xB0), mode (0xB4), duration (0xB8) and the fixOnArrival halfword (0xBC).
 * MCamMove_Coord copies the leading 0xB0 bytes as the target point record and
 * sets the point's fix flag when fixOnArrival is nonzero. mode is the MCamIpType
 * selector that MCamMove_Bank and MCamMove_Pers copy to MCamMoveState.mode.
 * The original loads duration before mode; the enum type keeps that order,
 * where an int mode would load first.
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

/* Starts a camera shake with the given mode and amplitude. A zero amplitude turns it off. */
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
    /* The arithmetic shift needs the int cast: enum CamFlag is unsigned. */
    return ((int)camFlags >> CAM_SHAKE_BIT) & 1;
}

/*
 * Writes the position of an actor part into coord and returns it. Part 0 is the
 * actor's own position, other parts come from sefGetNullPosition. w is the actor's
 * rotation.y plus pi, modulo 2 pi. A NULL actor leaves coord zeroed.
 */
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

/*
 * Computes a point's base coordinate into coord, selected by point->mode. Returns
 * the XZ distance between the two actors for the ratio and range modes, else 0.
 */
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

/* Computes a point's base coordinate, then applies its coordinate offset. Returns the base length. */
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
 * Writes the frame to the studio camera: eye direction from the two points,
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

/*
 * Recomputes the base coordinates, coordinates and angles of both points, then the
 * rotation from the first angle with the bank applied to z.
 */
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

/*
 * Starts a coordinate move of point 0 or 1 from its current coordinate to the
 * request's target. fixOnArrival sets the matching fix flag.
 */
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

/* Starts the bank move from the current bank to the request's target bank. */
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

/*
 * Starts the perspective move from the current angle to the request's target
 * angle. A zero target angle means the default 40 degrees.
 */
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


/*
 * Maps elapsed/duration to the ratio of the move's interpolation mode (MCamIpType).
 * A zero duration warns and returns 0.
 */
static float MCamMoveProc_CalcRatio(unsigned int mode, unsigned int elapsed, unsigned int duration)
{
    float ratio;

    if (duration == 0) {
        MOutputDebugStringWarn(zero_duration_message, mode);
        return 0.0f;
    }

    ratio = (float)elapsed / (float)duration;

    /* The sine cases evaluate their curve with a VU0 micro-subroutine call. */
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

/* A single helper: the original shares one add tail between both paths, which gcc's
 * cross-jumping only produces when the add comes from a single source line (as
 * MCamCopyVector does). */
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

/* Advances the bank move one frame. Returns nonzero while frames remain. */
static int MCamMoveProc_Bank(MCamMoveState *state)
{
    float start;
    float ratio;

    ratio = MCamMoveProc_CalcRatio(state->mode, ++state->elapsed, state->duration);
    start = state->start;
    state->current = start + (state->end - start) * ratio;
    return state->elapsed < state->duration;
}

/* Advances the perspective move one frame. Returns nonzero while frames remain. */
static int MCamMoveProc_Pers(MCamMoveState *state)
{
    float start;
    float ratio;

    ratio = MCamMoveProc_CalcRatio(state->mode, ++state->elapsed, state->duration);
    start = state->start;
    state->current = start + (state->end - start) * ratio;
    return state->elapsed < state->duration;
}

/*
 * Advances the shake one frame: the hold phase oscillates the offset, the decay
 * phase shrinks the amplitude, and after both the offset is zeroed and the shake
 * bit is cleared.
 */
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
