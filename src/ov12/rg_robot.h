/*
 * TU-local declarations of ov12/tu004 (src/ov12/rg_robot.c).
 */

#ifndef SRC_OV12_RG_ROBOT_H
#define SRC_OV12_RG_ROBOT_H

#include "shared.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_robot_effect.h"
#include "ov12/rg_shot_thread.h"
#include "ov12/xrg_actor.h"
#include "ov12/xrg_sound.h"
#include "ov12/rg_robot_spec.h"
#include "ov12/rg_dull_flag.h"

typedef struct RgStatus RgStatus;

typedef struct RgWeapon RgWeapon;

/* RgHomingThread.state. */
#define RG_HOMING_IDLE 0
#define RG_HOMING_ACTIVE 1
#define RG_HOMING_STOPPED 2

/*
 * Homing-thread control block (0x18 bytes) for a robot's ranged weapon
 * lock-on. homingSpeed is the rotation speed limit passed to RgRobSubHoming
 * (4*pi by default); sleepTime is read back as a float once the thread has
 * stopped.
 */
typedef struct RgHomingThread {
    RgGeomPoint *geometry;          /* +0x00 */
    RgGeomPoint *target;            /* +0x04 */
    float actionTime;               /* +0x08 */
    float sleepTime;                /* +0x0c */
    float homingSpeed;              /* +0x10 */
    int state;                      /* +0x14: RG_HOMING_* */
} RgHomingThread;

/*
 * The body of one robot: 0xb0 bytes allocated by _CreateBody. Its geometry
 * pointers (geometry, eyeGeometry, advanceGeometry) are all robot geometries;
 * cmdMask collects the (1 << command type) bits _BodyExecCmd executes while
 * _PassTimeRobot runs the queue and prevCmdMask keeps the previous frame's.
 * flags bit 0 selects RgGeomRobotSetMaxRotVel's unlimited rotation cap and
 * _BodyGeomPassTime clears the flags afterward; the weakHit* fields track
 * consecutive weak hits for _ExecWeakDamageCmd.
 */
/* RgBody.flags, cleared and set again every frame. */
#define RG_BODY_FLAG_FREE_TURN 1       /* turns without the speed-based rotation cap (1e8) */
#define RG_BODY_FLAG_NO_AUTO_HOMING 2  /* a rotation or homing already ran: skip the auto homing */

/* A time past the end of any action or sound timer. */
#define RG_ROBOT_TIME_UNLIMITED 100000000.0f

/* Rotation speed cap while RG_BODY_FLAG_FREE_TURN is set. */
#define RG_ROBOT_ROT_VEL_UNLIMITED 1e8f

/* Turn speed limit of a homing status (4*pi), as RgHomingThread's default. */
#define RG_ROBOT_HOMING_TURN_SPEED 12.566371f

typedef struct RgBody {
    unsigned int flags;               /* +0x00 */
    int cmdMask;                      /* +0x04 */
    int prevCmdMask;                  /* +0x08 */
    int grounded;                     /* +0xc */
    float life;                       /* +0x10 */
    float lifeMax;                    /* +0x14 */
    int autoHomingEnv;                /* +0x18 */
    int charID;                       /* +0x1c */
    float weakHitMotion;              /* +0x20 */
    float weakHitPower;               /* +0x24 */
    float weakHitAccum;               /* +0x28 */
    float weakHitWindow;              /* +0x2c */
    int shieldActive;                 /* +0x30 */
    unsigned char unmodeled_34[0xc];  /* +0x34 */
    /*
     * +0x40, read by RgRobotIsInvalidAttack (lwc1 +0x40/+0x44/+0x48) while
     * shieldActive is set: an attack whose direction has a non-negative dot
     * product with it is reported as invalid (blocked).
     */
    RgVector shieldDirection;
    float dashTime;                   /* +0x50 */
    RgRobotSpec *spec;                /* +0x54 */
    RgGeomPoint *target;              /* +0x58 */
    XrgActor *actor;                  /* +0x5c */
    RgGeomPoint *geometry;            /* +0x60 */
    RgGeomPoint *eyeGeometry;         /* +0x64 */
    RgGeomPoint *advanceGeometry;     /* +0x68 */
    float rotate;                     /* +0x6c */
    RgWeapon *weapon[RG_EQUIP_TYPE_NUM]; /* +0x70 */
    RgWeapon *spareWeapon[RG_EQUIP_TYPE_NUM]; /* +0x7c */
    RgShotThread *shotThread[RG_EQUIP_TYPE_NUM]; /* +0x88 */
    RgHomingThread *homingThread;     /* +0x94 */
    RgRobotEffect *effect;            /* +0x98 */
    int motSmooth;                    /* +0x9c */
    int smoothSuppressed;             /* +0xa0 */
    float confuse;                    /* +0xa4 */
    XrgSound *landingSound;           /* +0xa8 */
    float landingSoundTime;           /* +0xac: landing sound age; advanced by _BodyPassTime */
} RgBody;

/*
 * The robot object (0x60 bytes): its body, the status machine block
 * (robotStatus), the command ring (cmdQueue), the commands accepted this
 * frame, the status flags and the three dull flags RgRobotPassTime ages.
 */
struct RgStatus {
    RgBody *body;                     /* +0x00 */
    struct RgRobotStatus *robotStatus; /* +0x04 */
    void *cmdQueue;                   /* +0x08 */
    unsigned int acceptedCommandMask; /* +0x0c */
    unsigned int statusFlags;         /* +0x10 */
    unsigned char unmodeled_14[0x0c]; /* +0x14 */
    RgDullFlag dullFlag0;             /* +0x20 */
    unsigned char unmodeled_28[0x08]; /* +0x28 */
    RgDullFlag dullFlag1;             /* +0x30 */
    unsigned char unmodeled_38[0x08]; /* +0x38 */
    RgDullFlag dullFlag2;             /* +0x40 */
    RgDrawView *drawView;             /* +0x48 */
    int passTimePhase;                /* +0x4c: RgRobotPassTime phase (0 idle, 1 timing, 2 done) */
    float passTimeTimer;              /* +0x50: RgRobotPassTime countdown */
    unsigned char unmodeled_54[0x0c]; /* +0x54 */
};

/* RgRobotStatus.type: the status the robot is in. */
enum {
    RG_STATUS_NONE,
    RG_STATUS_MOVING,
    RG_STATUS_ATTACK,
    RG_STATUS_SHIELD,
    RG_STATUS_TARGETTING,
    RG_STATUS_DASH,
    RG_STATUS_DAMAGE,
    RG_STATUS_DASH_VR,
    RG_STATUS_BREAKING,
    RG_STATUS_DROP,
    RG_STATUS_HISSATU
};

/* RgCmd.type, one value per queuing function (RgRobotAccelarate, ...). */
enum {
    RG_CMD_ACCELARATE,
    RG_CMD_ACCELARATE_ROTATE,
    RG_CMD_SHOT,
    RG_CMD_TARGETTING,
    RG_CMD_BREAK,
    RG_CMD_DASH,
    RG_CMD_DASH_CONTINUE,
    RG_CMD_DROP_WEAPON,
    RG_CMD_DAMAGE,
    RG_CMD_WEAK_DAMAGE,
    RG_CMD_HIT_WEAPON_ATTACK,
    RG_CMD_INVALID_ATTACK,
    RG_CMD_HIT_BG,
    RG_CMD_NEAR_BG,
    RG_CMD_HIT_BY_BODY
};

typedef struct RgRobotStatus RgRobotStatus;
typedef struct RgCmd RgCmd;
/* Defined by ov12/tu013 (src/ov12/rg_motion_info_db.h); opaque here. */
typedef struct RgMotionShotInfo RgMotionShotInfo;

/*
 * A status's command handler: _StatusExecCmd passes the body in $5 and
 * the command in $6 (ov12:0x00a02b3c), the order every recovered handler
 * (_ExecCmdInDrop, _ExecCmdInHissatu, _DamageExecCmd, ...) reads them in.
 */
typedef int (*RgStatusExecCmdFunc)(RgRobotStatus *status, RgBody *body,
                                    RgCmd *command);
typedef int (*RgStatusPassTimeFunc)(RgRobotStatus *status, RgBody *body,
                                     float deltaTime);
typedef void (*RgStatusDispFunc)(RgRobotStatus *status, RgBody *body);
typedef void (*RgStatusExitFunc)(RgRobotStatus *status, RgBody *body);

/*
 * The robot's status-machine block: a separate 0x60-byte allocation that
 * _CreateStatue creates and _InitRobot keeps at RgStatus.robotStatus. Its
 * first 0x40 bytes are a scratch area each status type (RgRobotStatus.type)
 * seeds on entry and reads with its own meaning; the fields of the status
 * types sharing a slot sit in one union. _InitStatus clears the handlers and
 * type; _SetStatus runs the previous status' exitMethod first.
 */
struct RgRobotStatus {
    union {
        int scratch0;                     /* +0x00: integer view for Drop/DashVR statuses */
        float scratch0AsFloat;            /* +0x00: float view for Breaking/Damage statuses */
    };
    union {
        int scratch1;                     /* +0x04 */
        float scratch1AsFloat;            /* +0x04: float view for the Targetting and DashVR statuses */
        RgMotionShotInfo *dropMotionInfo; /* +0x04: Drop's motion-info record */
        RgWeapon *attackWeapon;           /* +0x04: Attack's ranged weapon handle */
    };
    union {
        int scratch2;                     /* +0x08 */
        float scratch2AsFloat;            /* +0x08: float view for the Targetting status */
        RgWeapon *attackLockonWeapon;     /* +0x08: Attack's lock-on weapon */
        RgMotionShotInfo *hissatuMotionInfo; /* +0x08: Hissatu's motion-info record */
    };
    union {
        float scratchTimer;               /* +0x0c */
        int scratch3;                     /* +0x0c */
        RgWeapon *shieldWeapon;           /* +0x0c: Shield's ranged weapon handle */
        RgMotionShotInfo *attackMotionInfo; /* +0x0c: Attack's motion-info record */
        struct RgMotionAction *currentAction; /* +0x0c: Hissatu's current action */
    };
    union {
        RgMotionShotInfo *motionInfo;     /* +0x10: Shield's motion-info record; Dash's vector view (x +0x10, y +0x14, z +0x18) */
        int attackActive;                 /* +0x10: Attack's active flag */
        int scratch10;                    /* +0x10: seeded 0 by _InitStatusHissatu */
        int shotCounter;                  /* +0x10: Hissatu's alternating-shot count */
    };
    union {
        int scratch4;                     /* +0x14 */
        int boosting;                     /* +0x14: Hissatu's boost flag, cleared on a new action */
    };
    union {
        int eSide;                        /* +0x18 */
        int shotPending;                  /* +0x18: Hissatu's pending shot-stop request */
    };
    union {
        int scratch5;                     /* +0x1c: seeded 0 by _InitStatusHissatu */
        int effectStarted;                /* +0x1c: Hissatu's dash effect started */
    };
    int dashMotionPlayed;                 /* +0x20; Shield copies a vector over +0x20..+0x2f */
    union {
        int dashVRMode;                   /* +0x24 */
        float dashPhaseTimer;             /* +0x24 */
    };
    float dashTimeStart;                  /* +0x28 */
    int dashLimited;                      /* +0x2c */
    int restoreDashTime;                  /* +0x30 */
    int dashVRAllowed;                    /* +0x34: set by _InitDashStatus */
    unsigned char unmodeled_38[8];        /* +0x38 */
    int type;                            /* +0x40 */
    float elapsedTime;                   /* +0x44 */
    RgStatusExecCmdFunc execCmdMethod;   /* +0x48 */
    RgStatusPassTimeFunc passTimeMethod; /* +0x4c */
    RgStatusDispFunc dispMethod;          /* +0x50 */
    RgStatusExitFunc exitMethod;          /* +0x54 */
};

/*
 * One queued robot command (0x40 bytes). The first 16 bytes hold the
 * direction, velocity or position of the vector-shaped commands
 * (Accelarate, Dash, GiveDamage, InvalidAttack, HitBG, NearBG) or a plain
 * value for the others (Shot, Targetting, DropWeapon, Rotate). The scratch
 * words carry per-type values: GiveDamage's magnitude and type, the weak
 * damage threshold/window/motion/power, HitByBody's damage and force scale
 * (it also copies its direction into scratch0..scratch3 as an inline
 * vector). type selects the command and is the bit _BodyExecCmd sets in
 * RgBody.cmdMask.
 */
struct RgCmd {
    union {
        int param;                   /* +0x00 */
        RgWeapon *weapon;            /* +0x00: the Attack command's weapon handle */
        float rotate;                /* +0x00: the Rotate command's signed input */
        RgVector vector;             /* +0x00: the direction, velocity or position the command carries */
    };
    float scratch0;                  /* +0x10 */
    int scratch1;                    /* +0x14 */
    float scratch2;                  /* +0x18 */
    float scratch3;                  /* +0x1c */
    float scratch4;                  /* +0x20 */
    float scratch5;                  /* +0x24 */
    unsigned char unmodeled_28[8];   /* +0x28 */
    int type;                        /* +0x30 */
    unsigned char unmodeled_34[0xc]; /* +0x34 */
};

/*
 * RgStatus.cmdQueue points to this ring: _CreateCmdQueue allocates 0x410
 * bytes (cmd[] plus count and the accepted-entry counter _EntryCmdQueue
 * increments and _ClearCmdQueue leaves running).
 */
#define RG_CMD_QUEUE_CAPACITY 16

typedef struct RgCmdQueue {
    RgCmd cmd[RG_CMD_QUEUE_CAPACITY]; /* +0x000 */
    unsigned int count;               /* +0x400 */
    unsigned int entryCount;          /* +0x404 */
    unsigned char unmodeled_408[8];   /* +0x408 */
} RgCmdQueue;

static void _InitBreakingStatus(RgRobotStatus *status, RgBody *body);

/* Returns the accepted entry, or 0 when the ring is full or the type invalid. */
static RgCmd *_EntryCmdQueue(void *cmdQueue, int cmdType);

static int _BodyEquipWeapon(RgBody *body, int side, RgWeapon *weapon);

static void _BodySetSpareWeapon(RgBody *body, int eSide, RgWeapon *weapon);

#endif /* SRC_OV12_RG_ROBOT_H */
