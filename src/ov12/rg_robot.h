/*
 * TU-local declarations of ov12/tu004 (src/ov12/rg_robot.c).
 */

#ifndef SRC_OV12_RG_ROBOT_H
#define SRC_OV12_RG_ROBOT_H

#include "shared.h"
#include "ov12/rg_draw.h"

typedef struct RgStatus RgStatus;

/* Opaque here: rg_robot_effect.c (ov12/tu010) owns the RgRobotEffect definition. */
typedef struct RgRobotEffect RgRobotEffect;

/* Number of equip sides a body carries (RgRobotGetWeapon's own range check,
 * ov12:0x00a51be0, "RG_EQUIP_TYPE_MIN <= (eSide) && (eSide) < RG_EQUIP_TYPE_NUM"). */
#define RG_EQUIP_TYPE_NUM 3

/*
 * Homing-thread control block for a robot's ranged weapon lock-on.
 * _CreateHomingThread allocates it with RgHeapAlloc (size 0x18,
 * ov12:0x00a02594) and _InitHomingThread (still asm) zeroes it.
 * _StartHomingThread, _StopHomingThread, _GetStatHomingThread and
 * _GetSleepTimeHomingThread evidence every field but the unmodeled one:
 *   +0x00 geometry   the geometry to home toward (sw 0($16),
 *                     ov12:0x00a02674)
 *   +0x04 target     the homing target object (sw 4($16),
 *                     ov12:0x00a02670)
 *   +0x08 actionTime the requested homing duration (swc1 8($16),
 *                     ov12:0x00a0266c)
 *   +0x0c sleepTime  cleared unconditionally by _StartHomingThread (sw
 *                     0xC($16), ov12:0x00a0267c) and by _StopHomingThread
 *                     (sw 0xC($16), ov12:0x00a026f0); read back as a float
 *                     once state reaches 2 by _GetSleepTimeHomingThread
 *                     (lwc1 0xC($16), ov12:0x00a0278c)
 *   +0x14 state      0 idle, 1 homing, 2 stopped: written by
 *                     _StartHomingThread (ov12:0x00a02680/0x00a02684/
 *                     0x00a0268c), read back by _GetStatHomingThread
 *                     (ov12:0x00a02734) and _GetSleepTimeHomingThread
 *                     (ov12:0x00a02774), and advanced to 2 by
 *                     _StopHomingThread (ov12:0x00a026f4)
 * Byte 0x10 is untouched by any function of this allocation and stays
 * unmodeled.
 */
typedef struct RgHomingThread {
    RgGeomPoint *geometry;          /* +0x00 */
    void *target;                   /* +0x04 */
    float actionTime;               /* +0x08 */
    float sleepTime;                /* +0x0c */
    unsigned char unmodeled_10[4];  /* +0x10 */
    int state;                      /* +0x14 */
} RgHomingThread;

/*
 * Partial RgRobotSpec layout. _CalcMaxSpeed is the only claimed function
 * that dereferences RgBody's spec pointer, reading +0x1c as the float
 * combined with the requested base speed to produce the robot's max speed
 * (lwc1 0x1C($2), ov12:0x00a02890).
 */
/*
 * _BodyGeomPassTime evidences two more RgRobotSpec fields, each read by
 * exactly one lwc1 at the offset shown:
 *   +0x0c baseSpeed the value it passes as _CalcMaxSpeed's own baseSpeed
 *                    argument (lwc1 0xC($18), ov12:0x00a06bc8)
 *   +0x14 turnRate   scaled by the damped rotation ratio and handed to
 *                    RgGeomRobotSetMaxRotVel (lwc1 0x14($18),
 *                    ov12:0x00a06c1c)
 * Bytes 0x10-0x13 and 0x18-0x1b stay unmodeled.
 */
/*
 * _ExecAccelarateCmd and _BodyAdvanceForAttack evidence two trailing
 * RgRobotSpec fields, each read by exactly one lwc1 at the offset shown:
 *   +0x20 accelRate         the scale _ExecAccelarateCmd hands
 *                            RgRobSubAcceralate directly (lwc1 0x20($18),
 *                            ov12:0x00a075dc) and _BodyAdvanceForAttack
 *                            multiplies by attackAdvanceRate (lwc1 0x20($16),
 *                            ov12:0x00a06008)
 *   +0x34 attackAdvanceRate multiplied by accelRate for
 *                            _BodyAdvanceForAttack's own acceleration scale
 *                            (lwc1 0x34($16), ov12:0x00a0600c)
 * Bytes 0x24-0x33 stay unmodeled; the struct is not known to end at 0x38.
 */
/*
 * _ExecDashContinueCmd reads one leading field, gating whether the DashVR
 * status is re-entered when compared against 1 (lw 0x4($2), ov12:0x00a07ad0):
 *   +0x04 type
 * Bytes 0x00-0x03 and 0x08-0x0b stay unmodeled.
 */
/*
 * _ExecRotateCmd evidences one more field in that gap, read by exactly one
 * lwc1 at the offset shown:
 *   +0x24 rotateForce scale applied to the queued command's own rotate
 *                      sign before RgGeomRobotAddRotForce (lwc1 0x24($2),
 *                      ov12:0x00a07688)
 */
/*
 * _BodySetGeom evidences two more fields within that same gap, each read
 * by exactly one lwc1 at the offset shown:
 *   +0x28 moveResist scaled linear move resistance, handed to
 *                     RgGeomPointSetMoveResist (lwc1 0x28($16),
 *                     ov12:0x00a061e4)
 *   +0x2c rotResist   scaled rotational resistance, handed to
 *                     RgGeomRobotSetRotResist (lwc1 0x2c($16),
 *                     ov12:0x00a061fc)
 */
typedef struct RgRobotSpec {
    unsigned char unmodeled_00[4];   /* +0x00 */
    int type;                        /* +0x04 */
    unsigned char unmodeled_08[4];   /* +0x08 */
    float baseSpeed;                 /* +0x0c */
    unsigned char unmodeled_10[4];   /* +0x10 */
    float turnRate;                  /* +0x14 */
    unsigned char unmodeled_18[4];   /* +0x18 */
    float speedRating;                /* +0x1c */
    float accelRate;                 /* +0x20 */
    float rotateForce;               /* +0x24 */
    float moveResist;                /* +0x28 */
    float rotResist;                 /* +0x2c */
    unsigned char unmodeled_30[4];   /* +0x30 */
    float attackAdvanceRate;          /* +0x34 */
} RgRobotSpec;

/*
 * Partial RgBody layout. The geometry pointer at +0x60 was already evidenced
 * by _ExecBreakCmd. ov12/tu004's RgRobotGetXxx/SetXxx accessors evidence the
 * remaining named fields, each read or written by exactly one lw/sw/lwc1/swc1
 * at the offset shown, with no other byte of the gaps ever touched by a
 * claimed function of this TU:
 *   +0x0c grounded  _BodyOnGround (lw 12($16), ov12:0x00a02948)
 *   +0x10 life      RgRobotGetLife (lwc1 16($2), ov12:0x00a08a2c),
 *                    RgRobotIsDead (lwc1 16($2), ov12:0x00a089cc)
 *   +0x14 lifeMax   RgRobotGetLifeMax (lwc1 20($2), ov12:0x00a08a74)
 *   +0x1c charID    RgRobotGetCharID (lw 28($3), ov12:0x00a08b04)
 *   +0x50 dashTime  RgRobotGetDashTime (lwc1 80($2), ov12:0x00a08bec)
 *   +0x54 spec      RgRobotGetSpec (lw 84($3), ov12:0x00a08c34); _CalcMaxSpeed
 *                    dereferences it (lw 0x54($3), ov12:0x00a02888)
 *   +0x58 target    RgRobotGetTarget (lw 88($3), ov12:0x00a08984)
 *   +0x5c actor     RgRobotGetActor (lw 92($3), ov12:0x00a08abc)
 *   +0x60 geometry  RgRobotGetGeom (lw 96($3), ov12:0x00a088bc); _CalcMaxSpeed
 *                    passes it to RgGeomPointGetWeight (lw 0x60($3),
 *                    ov12:0x00a02884)
 *   +0x70 weapon[3] RgRobotGetWeapon (lw 112($3) indexed by eSide,
 *                    ov12:0x00a0893c)
 *   +0x9c motSmooth RgRobotSetMotSmooth (sw 156($2), ov12:0x00a08764)
 *   +0xa4 confuse   RgRobotSetConfuse (swc1 164($2), ov12:0x00a087b4)
 *   +0xa8 landingSoundID _BodyOnGround (lw 168($16), ov12:0x00a02954)
 * target/actor/weapon are read and returned but never dereferenced by any
 * claimed function, so their pointee stays unresolved (void *). Every other
 * byte stays unmodeled.
 *
 * +0x7c spareWeapon[3] is _BodySetSpareWeapon's own array (extern below);
 * _DropPassTime (ov12:0x00a05af0) reads spareWeapon[eSide], hands it to
 * _BodyEquipWeapon and clears the slot once the drop's timed removal point
 * is reached.
 */
/*
 * _BodySetEyeGeom/_BodySetAdvanceGeom evidence two more geometry pointers,
 * each read and overwritten by lw/sw at the offset shown:
 *   +0x64 eyeGeometry      _BodySetEyeGeom (lw 0x64($16), sw 0x64($16),
 *                           ov12:0x00a0627c/0x00a06284)
 *   +0x68 advanceGeometry  _BodySetAdvanceGeom (lw 0x68($17), sw 0x68($17),
 *                           ov12:0x00a062e8/0x00a0630c); _AdvanceGeomPassTime
 *                           reads it and hands it to RgGeomPassTime/
 *                           RgGeomPointSetPos/RgGeomPointMovePos
 *                           (lw 0x68($4), ov12:0x00a06d44)
 * Bytes 0x6c-0x6f stay unmodeled.
 *
 * _BodyPlayMotion/_BodyPlayMotionLoop evidence one more boolean-shaped
 * field, tested by exactly one lw at the offset shown:
 *   +0xa0 smoothSuppressed when nonzero, forces XrgActorSetSmoothPlay's
 *          smoothPlay argument to 0 instead of motSmooth (lw 0xA0($4),
 *          ov12:0x00a05ee4/0x00a05f64)
 */
/*
 * _BodyExecCmd (still asm) ORs each executed command's (1 << type) bit into
 * +0x04 while _PassTimeRobot runs the queue; _PassTimeRobot itself moves that
 * accumulated mask to +0x08 and clears +0x04 before running the new frame's
 * commands (lw/sw 0x4($16)/0x8($16), ov12:0x00a07f48/0x00a07f4c/0x00a07f50).
 * Nothing claimed reads +0x08 back; it only holds the previous frame's mask.
 */
/*
 * _BodyGeomPassTime evidences two more RgBody fields, each touched by
 * exactly one instruction at the offset shown:
 *   +0x00 flags         bit 0 forces RgGeomRobotSetMaxRotVel's escape branch
 *                        (RG_FCONST(1e8)) instead of the speed-based damped
 *                        rotation cap when set (lw/andi 0($16),
 *                        ov12:0x00a06bd8/0x00a06be4); cleared unconditionally
 *                        afterward (lw/and/sw 0($16), ov12:0x00a06c30/
 *                        0x00a06c40/0x00a06c50/0x00a06c58)
 *   +0x18 autoHomingEnv written by _BodySetAutoHomingEnv from its own
 *                        argument (sw 24($16), ov12:0x00a06adc)
 */
/*
 * _CreateBody sizes the whole allocation at 0xb0 bytes (RgHeapAlloc size
 * 0xB0, ov12:0x00a074c0), 4 bytes past landingSoundID; nothing claimed
 * touches that trailing span.
 */
/*
 * _BodySetGeom evidences the last unmodeled geometry-pointer byte as the
 * robot's own damped rotation, written from RgGeomRobotGetRotate's return
 * every time the geometry changes (swc1 0x6c($18), ov12:0x00a0620c).
 */
/*
 * _DashVRExit reads one more field, passed unchanged as the argument to
 * both RgRobotEffectTermJet and RgRobotEffectTermDash (lw 0x98($16),
 * ov12:0x00a05660/0x00a05668):
 *   +0x98 effect
 * Bytes 0x88-0x97 stay unmodeled.
 */
/*
 * _AttackExit reads one more field within that same span, the homing
 * thread the Attack status started for the robot's own ranged weapon,
 * read to stop it (lw 0x94($18), ov12:0x00a03324):
 *   +0x94 homingThread
 * Bytes 0x88-0x93 stay unmodeled.
 */
/*
 * _ExecWeakDamageCmd evidences four more fields, each touched at the
 * offset shown:
 *   +0x20 weakHitMotion  the queued command's own scratch3, stored back
 *                        only while the accumulated hits stay below the
 *                        command's own threshold (swc1 0x20($16),
 *                        ov12:0x00a07d94)
 *   +0x24 weakHitPower   the queued command's own scratch4, stored back
 *                        alongside weakHitMotion (swc1 0x24($16),
 *                        ov12:0x00a07d98)
 *   +0x28 weakHitAccum   the running total of RgCmd.scratch0 across
 *                        consecutive weak hits, compared against
 *                        RgCmd.scratch1 to trigger Damage status (swc1
 *                        0x28($16), ov12:0x00a07d64)
 *   +0x2c weakHitWindow  reseeded from the queued command's own scratch2
 *                        whenever it has decayed to zero or less, alongside
 *                        clearing weakHitAccum (swc1 0x2c($16),
 *                        ov12:0x00a07d54)
 */
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
    unsigned char unmodeled_30[0x20]; /* +0x30 */
    float dashTime;                   /* +0x50 */
    RgRobotSpec *spec;                /* +0x54 */
    void *target;                     /* +0x58 */
    void *actor;                      /* +0x5c */
    RgGeomPoint *geometry;            /* +0x60 */
    RgGeomPoint *eyeGeometry;         /* +0x64 */
    RgGeomPoint *advanceGeometry;     /* +0x68 */
    float rotate;                     /* +0x6c */
    void *weapon[RG_EQUIP_TYPE_NUM];  /* +0x70 */
    int spareWeapon[RG_EQUIP_TYPE_NUM]; /* +0x7c */
    unsigned char unmodeled_88[0xc];  /* +0x88 */
    RgHomingThread *homingThread;     /* +0x94 */
    RgRobotEffect *effect;            /* +0x98 */
    int motSmooth;                    /* +0x9c */
    int smoothSuppressed;             /* +0xa0 */
    float confuse;                    /* +0xa4 */
    int landingSoundID;               /* +0xa8 */
    unsigned char unmodeled_ac[4];    /* +0xac */
} RgBody;

/*
 * Partial RgStatus layout, evidenced by ov12/tu004's own-object accessors
 * (every field after body has exactly one claimed reader):
 *   +0x00 body               shared by every RgRobotGetXxx/SetXxx accessor
 *                             above (lw 0($16), e.g. RgRobotGetLife at
 *                             ov12:0x00a08a24)
 *   +0x08 cmdQueue            RgRobotBreak (lw 8($16), ov12:0x00a08f2c)
 *   +0x0c acceptedCommandMask RgRobotIsAcceptedCommand (lw 12($16),
 *                             ov12:0x00a08cdc)
 *   +0x10 statusFlags         RgRobotGetStatusFlags (lw 16($16),
 *                             ov12:0x00a08d34)
 * Unread bytes stay unmodeled.
 */
/*
 * _PassTimeRobot (ov12:0x00a07f08) reads +0x04 back as the pointer
 * _InitRobot stores there (see the RgRobotStatus comment below) and forwards
 * it to _BodyExecCmd/_BodyPassTime, confirming the field as a RgRobotStatus
 * pointer. CreateRgRobot's RgHeapAlloc call (ov12:0x00a08194) sizes the whole
 * allocation at 0x60 bytes; RgRobotSetRgDrawView (ov12:0x00a086fc) is the
 * only claimed writer past +0x10, storing its RgDrawView argument at +0x48.
 * Nothing claimed touches +0x14..0x47 or +0x4c..0x5f.
 */
struct RgStatus {
    RgBody *body;                     /* +0x00 */
    struct RgRobotStatus *robotStatus; /* +0x04 */
    void *cmdQueue;                   /* +0x08 */
    unsigned int acceptedCommandMask; /* +0x0c */
    unsigned int statusFlags;         /* +0x10 */
    unsigned char unmodeled_14[0x34]; /* +0x14 */
    RgDrawView *drawView;             /* +0x48 */
    unsigned char unmodeled_4c[0x14]; /* +0x4c */
};

typedef struct RgRobotStatus RgRobotStatus;

typedef int (*RgStatusExecCmdFunc)(RgRobotStatus *status, int param,
                                    RgBody *body);
typedef int (*RgStatusPassTimeFunc)(RgRobotStatus *status, RgBody *body,
                                     float deltaTime);
typedef void (*RgStatusDispFunc)(RgRobotStatus *status, RgBody *body);
typedef void (*RgStatusExitFunc)(RgRobotStatus *status, RgBody *body);

/*
 * The robot's status-machine block: a separate 0x60-byte allocation
 * (RgHeapAlloc size 0x60, ov12:0x00a029f0) that _CreateStatue creates and
 * hands straight to _InitStatus (ov12:0x00a02a00); _InitRobot keeps the
 * pointer at robot+0x04 (ov12:0x00a08114). It is distinct from the robot
 * object (struct RgStatus above) the pRobot accessors use -- its own assert
 * message is "pStatus != NIL" (D_00A518F0), not the robot's "pRobot != NIL"
 * (D_00A51BB0).
 * _InitStatus/_SetStatus/_DisposeStatus/_StatusExecCmd/_StatusPassTime/
 * _StatusDisp evidence the fields after the unmodeled header:
 *   +0x40 type                written by _SetStatus from its own type
 *                             argument (sw 0x40($16), ov12:0x00a02a78);
 *                             cleared by _InitStatus (sw 0x40($16),
 *                             ov12:0x00a029a8)
 *   +0x44 elapsedTime         accumulated by _StatusPassTime (swc1
 *                             0x44($16), ov12:0x00a02bfc); cleared by
 *                             _InitStatus (sw 0x44($16), ov12:0x00a029ac)
 *   +0x48 execCmdMethod       called by _StatusExecCmd (lw 0x48($16),
 *                             ov12:0x00a02b3c); cleared by _InitStatus
 *                             (sw 0x48($16), ov12:0x00a029b0)
 *   +0x4c passTimeMethod      called by _StatusPassTime (lw 0x4C($16),
 *                             ov12:0x00a02bc4); cleared by _InitStatus
 *                             (sw 0x4C($16), ov12:0x00a029b4)
 *   +0x50 dispMethod          called by _StatusDisp (lw 0x50($16),
 *                             ov12:0x00a02c44); cleared by _InitStatus
 *                             (sw 0x50($16), ov12:0x00a029b8)
 *   +0x54 exitMethod          called by _SetStatus before it resets the
 *                             block (lw 0x54($16), ov12:0x00a02a5c);
 *                             cleared by _InitStatus (sw 0x54($16),
 *                             ov12:0x00a029a4)
 * Bytes 0x00-0x3f are untouched by any claimed function of this TU and stay
 * unmodeled; the allocation continues to 0x60, but nothing beyond 0x54 is
 * evidenced either.
 *
 * Bytes 0x00-0x0f are in fact a private scratch area each status type's
 * own _Init<Xxx>Status (still asm) seeds on entry; the generic _InitStatus
 * above does not clear it, so its meaning depends on the current type.
 * ov12/tu004's own Drop and Damage/DashVR status handlers evidence every
 * scratch word they touch (rest of 0x10-0x3f stays unmodeled):
 *   scratch0 (+0x00) Drop's target equip side, read/written by
 *                     _ExecCmdInDrop and _DropPassTime (ov12:0x00a05a50/
 *                     0x00a05af0); DashVR's follow-state tag, written 1 by
 *                     _DashVRMainToFollow (ov12:0x00a04e98)
 *   scratch1 (+0x04) Drop's drop-motion id (see _GetDropMotion), read by
 *                     _DropPassTime; Damage's dash-escape gate, read by
 *                     _DamageExecCmd (ov12:0x00a05868)
 *   scratch2 (+0x08) Drop's pending spare-weapon-removal flag, read and
 *                     cleared by _ExecCmdInDrop/_DropPassTime
 *   scratchTimer (+0x0c) DashVR's countdown timer, set to 0.1 by
 *                     _DashVRMainToFollow
 */
/*
 * _AttackExit evidences the Attack status's own reuse of two scratch words
 * and one more field of the 0x10-0x3f span:
 *   scratch1 (+0x04) the ranged weapon handle passed to RgWeaponGetEss and
 *                     RgWeaponPlayMotion (lw 0x4($17), ov12:0x00a0330c);
 *                     also the RgWeaponShotStop argument on entry
 *   scratch2 (+0x08) the weapon handle passed to RgWeaponRestartLockon on
 *                     exit (lw 0x8($17), ov12:0x00a0336c)
 *   eSide (+0x18) the attacking equip side, combined with the robot's
 *                 charID to index the weapon-essence motion table (lw
 *                 0x18($17), ov12:0x00a0334c)
 */
struct RgRobotStatus {
    int scratch0;                         /* +0x00 */
    int scratch1;                         /* +0x04 */
    int scratch2;                         /* +0x08 */
    float scratchTimer;                   /* +0x0c */
    unsigned char unmodeled_10[8];        /* +0x10 */
    int eSide;                            /* +0x18 */
    unsigned char unmodeled_1c[0x24];     /* +0x1c */
    int type;                            /* +0x40 */
    float elapsedTime;                   /* +0x44 */
    RgStatusExecCmdFunc execCmdMethod;   /* +0x48 */
    RgStatusPassTimeFunc passTimeMethod; /* +0x4c */
    RgStatusDispFunc dispMethod;          /* +0x50 */
    RgStatusExitFunc exitMethod;          /* +0x54 */
};

/*
 * One queued robot command. _BodyExecCmd (still asm, ov12:0x00a06b5c) reads
 * +0x30 as the command's type to build the (1 << type) bit it ORs into
 * RgBody's cmdMask; _ExecDropWeaponCmd reads +0x00 of the command it is
 * given as the equip side to drop. No other byte of either allocation is
 * claimed.
 */
/*
 * RgRobotAccelarate/RgRobotShot/RgRobotTargetting/RgRobotDash/
 * RgRobotGiveDamage/RgRobotDropWeapon/RgRobotInvalidAttack/RgRobotHitBG/
 * RgRobotNearBG evidence every queuer's own writes into the entry
 * _EntryCmdQueue (still asm) returns; bytes 0x00-0x0f hold a direction
 * vector for the vector-shaped commands (Accelarate/Dash/GiveDamage/
 * InvalidAttack/HitBG/NearBG, written through XrgCopyVector/
 * XrgNormalizeVector's destination cast at that address) and reuse +0x00 as
 * a plain int for the value-shaped commands (param below, written by
 * Shot/Targetting/DropWeapon). scratch0/scratch1 are each written by
 * exactly one command's queuer, at the offset shown:
 *   +0x10 scratch0 RgRobotGiveDamage's own damage magnitude (swc1 0x10($2),
 *                   ov12:0x00a09064); RgRobotInvalidAttack's own scale,
 *                   reusing the same offset for its own command type
 *                   (swc1 0x10($16), ov12:0x00a09348)
 *   +0x14 scratch1 RgRobotGiveDamage's own damage type (sw 0x14($2),
 *                   ov12:0x00a09068)
 * Bytes 0x18-0x2f stay unmodeled.
 */
/*
 * RgRobotGiveWeakDamage/RgRobotHitByBody evidence four more scratch words
 * of that same span, each written by exactly one queuer, plus a float
 * reuse of scratch1; _ExecWeakDamageCmd/_ExecHitByBody read them back:
 *   +0x14 scratch1 read as a float threshold by _ExecWeakDamageCmd (lwc1
 *                   0x14($2), ov12:0x00a07d38), compared against the
 *                   accumulated scratch0 hits
 *   +0x18 scratch2 RgRobotGiveWeakDamage's own third float argument (swc1
 *                   0x18($16), ov12:0x00a09130); the reseed value
 *                   _ExecWeakDamageCmd stores into RgBody's own
 *                   weakHitWindow (lwc1 0x18($2), ov12:0x00a07d48)
 *   +0x1c scratch3 RgRobotGiveWeakDamage's own fourth float argument (swc1
 *                   0x1c($16), ov12:0x00a09134); read back into RgBody's
 *                   weakHitMotion (lwc1 0x1c($2), ov12:0x00a07d90)
 *   +0x20 scratch4 RgRobotGiveWeakDamage's own fifth float argument (swc1
 *                   0x20($16), ov12:0x00a0912c), read back into RgBody's
 *                   weakHitPower (lwc1 0x20($2), ov12:0x00a07d98);
 *                   RgRobotHitByBody's own damage magnitude, reusing the
 *                   same offset for its own command type (swc1 0x20($16),
 *                   ov12:0x00a091e0), subtracted from RgBody's life by
 *                   _ExecHitByBody (lwc1 0x20($16), ov12:0x00a07c9c)
 *   +0x24 scratch5 RgRobotHitByBody's own force scale (swc1 0x24($16),
 *                   ov12:0x00a091dc), handed to XrgScaleVector by
 *                   _ExecHitByBody (lwc1 0x24($16), ov12:0x00a07c74)
 * RgRobotHitByBody also copies its own direction argument into
 * scratch0..scratch3 as an inline RgVector (XrgCopyVector destination cast
 * at &scratch0, ov12:0x00a091c8), and its own position argument into
 * param..+0xc the same way (ov12:0x00a091d4), both read back by
 * _ExecHitByBody through the same casts (ov12:0x00a07c6c/0x00a07c74).
 * Bytes 0x28-0x2f stay unmodeled.
 */
typedef struct RgCmd {
    int param;                       /* +0x00 */
    unsigned char unmodeled_04[0xc]; /* +0x04 */
    float scratch0;                  /* +0x10 */
    int scratch1;                    /* +0x14 */
    float scratch2;                  /* +0x18 */
    float scratch3;                  /* +0x1c */
    float scratch4;                  /* +0x20 */
    float scratch5;                  /* +0x24 */
    unsigned char unmodeled_28[8];   /* +0x28 */
    int type;                        /* +0x30 */
    unsigned char unmodeled_34[0xc]; /* +0x34 */
} RgCmd;

/*
 * RgStatus's cmdQueue (+0x08) points to this fixed-size ring: _PassTimeRobot
 * (ov12:0x00a07f54/0x00a07f8c) reads a count at +0x400 and, when nonzero,
 * walks that many 0x40-byte RgCmd entries starting at +0x000, which is
 * exactly RG_CMD_QUEUE_CAPACITY * sizeof(RgCmd).
 *
 * _CreateCmdQueue (still asm, ov12:0x00a05e10) allocates the whole ring
 * with RgHeapAlloc(size 0x410), so the ring is 0xc bytes larger than
 * cmd[]+count; _InitCmdQueue clears an additional word at +0x404 that
 * _EntryCmdQueue (still asm, ov12:0x00a05d00/0x00a05d18) reads back and
 * increments each time it accepts a command of a valid type while the
 * ring is not full, so it counts total accepted entries. _ClearCmdQueue
 * only resets count, leaving that counter running across frames. Bytes
 * 0x408..0x410 stay unmodeled.
 */
#define RG_CMD_QUEUE_CAPACITY 16

typedef struct RgCmdQueue {
    RgCmd cmd[RG_CMD_QUEUE_CAPACITY]; /* +0x000 */
    unsigned int count;               /* +0x400 */
    unsigned int entryCount;          /* +0x404 */
    unsigned char unmodeled_408[8];   /* +0x408 */
} RgCmdQueue;

extern void RgRobSubBreak(RgGeomPoint *geometry, float linear_scale,
                           float rotational_scale);

static void _InitBreakingStatus(RgStatus *status, RgBody *body);

/* Returns the accepted entry, or 0 when the ring is full or the type invalid. */
static RgCmd *_EntryCmdQueue(void *cmdQueue, int cmdType);

static void _BodyEquipWeapon(RgBody *body, int eSide, int weaponID);

static void _BodySetSpareWeapon(RgBody *body, int eSide, int weaponID);

#endif /* SRC_OV12_RG_ROBOT_H */
