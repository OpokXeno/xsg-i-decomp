/*
 * TU-local declarations of ov12/tu026 (src/ov12/rg_weapon.c).
 *
 * RgWeapon is needed by only this TU so far: the other accepted-side caller
 * of the addresses below, rg_robot.c's _BodyEquipWeapon (ov12:0x00a06618),
 * still reaches them through its own INCLUDE_ASM body.
 *
 * Field offsets are attested by the loads/stores/address computations of the
 * allocation this header supports (ov12:0x00a1b530..0x00a1bd78):
 *   controlFlags 0x00, robotId 0x04, shotRequested 0x10, dispose 0x14,
 *   shoot 0x20, actor 0x68, equip 0x70, shotMotionTable 0xac (18 ints:
 *   RG_ACTOR_CHAR_ROBNUM rows of 3, RgWeaponRobotShotMotion's own bound
 *   check proves the row count and its address arithmetic the row width),
 *   targeting 0x1a0, lockedOn 0x1a4, shotMotionOffset 0x1a8, shotNum 0x1ac,
 *   weight 0x1b0, lockOnEffector 0x1b4, ess 0x1b8, equipped 0x1bc,
 *   lockOnStopped 0x1c0, sound 0x1c4. Every span between them that no
 * function of this allocation reads or writes is `unmodeled_XX`.
 *
 * The allocation ov12:0x00a1a6e8..0x00a1b52c adds the trailing span
 * 0x1c8..0x1f0 as `unmodeled_1c8` and fixes the common object's size at 0x1f0:
 * _CreateWeaponShieldType (ov12:0x00a1ae10) allocates exactly 0x1f0 bytes for
 * a shield-type weapon and this allocation's shield functions
 * (_ShotShieldType, _ShotStopShieldType, _PassTimeShieldType,
 * _DestructShieldType) touch nothing past the null check, so a shield-type
 * weapon is a bare RgWeapon. The larger sibling allocations
 * (_CreateWeaponAttackType/_CreateWeaponUnArmedType at 0x230,
 * _CreateWeaponEnergyType at 0x210) extend it with the type-specific trailing
 * fields RgWeaponAttackType and RgWeaponEnergyType attest below.
 */

#ifndef SRC_OV12_RG_WEAPON_H
#define SRC_OV12_RG_WEAPON_H

/*
 * RG_ACTOR_CHAR_ROBNUM is the original identifier baked into the assert
 * string at ov12:0x00a537d8 ("0 <= eCharID && eCharID < RG_ACTOR_CHAR_ROBNUM")
 * that RgWeaponRobotShotMotion cites.
 */
#define RG_ACTOR_CHAR_ROBNUM 6

typedef struct RgWeapon RgWeapon;

/*
 * The actor handle RgWeaponGetActor/RgWeaponPlayMotion/RgWeaponDisposeLockon
 * pass to the Xrg actor API (XrgActorSetMotion, XrgActorDisposeEffector,
 * RgRobotGetActor) is a pointer: annotations/overlays/ov12_annotations.csv
 * describes RgRobotGetActor (ov12:0x00a08a88) as returning "the robot body's
 * actor pointer", and XrgActorSetMotion/XrgActorDisposeEffector are defined
 * by src/ov12/xrg_actor.c, not yet recovered, so its members are unattested
 * here; only the pointer identity this allocation's loads/stores/comparisons
 * evidence is claimed.
 */
typedef struct XrgActor XrgActor;

/*
 * The embedded equip record RgWeaponGetEquip exposes by address (addiu
 * $2,$16,0x70 at ov12:0x00a1b90c, consumed as a pointer argument by
 * rg_robot.c's _BodyEquipWeapon at ov12:0x00a066bc/0x00a066e0). Its interior
 * is outside this allocation's evidence: only its start (weapon+0x70) and
 * its extent up to the next evidenced member (shotMotionTable at 0xac, so
 * 0x3c bytes) are attested.
 */
typedef struct RgEquip {
    unsigned char unmodeled_00[0x3c];
} RgEquip;

/*
 * _EnergyIsBusy (ov12:0x00a1af68) takes the address of RgWeapon+0x40 and
 * passes it to _GetNumAttach (ov12:0x00a19448, local). That function and
 * its siblings _InitAttach/_ReleaseAttach/_AddShotAttach/_CheckAttach/
 * _GetAllAttach null-check the same pointer ("pAttach != NIL", D_00A53648)
 * and _AddShotAttach bounds it with "pAttach->m_uAttachNum < ATTACH_MAX"
 * (D_00A53670): a count at +0x20 following up to 8 pointer-sized slots,
 * fixing this member at 0x24 bytes. _EnergyIsBusy itself only takes the
 * address; the slots' pointee type belongs to another translation unit and
 * no byte of the interior is claimed here.
 */
/*
 * ATTACH_MAX is the original identifier baked into the assert string at
 * ov12:0x00a53670 ("pAttach->m_uAttachNum < ATTACH_MAX") that _AddShotAttach
 * cites; RgWeaponAttach's slot count below evidences its value as 8.
 */
#define ATTACH_MAX 8

/*
 * _ReleaseAttach and _GetAllAttach (ov12:0x00a193c8/0x00a19488) evidence the
 * slot array's own identity: _ReleaseAttach passes each entry unchanged to
 * RgShotRelease(RgShot *), and _GetAllAttach copies each entry to its
 * caller's array unchanged. Only the pointer identity is claimed; RgShot's
 * interior belongs to src/ov12/rg_shot.c, not yet recovered here.
 *
 * RgShot *shots[ATTACH_MAX] is ATTACH_MAX (8) 4-byte pointers = 0x20 bytes,
 * exactly the unmodeled_00[0x20] span it replaces; the trailing
 * unsigned int m_uAttachNum is untouched, so RgWeaponAttach's total size
 * (0x24 bytes) is unchanged.
 */
typedef struct RgShot RgShot;

typedef struct RgWeaponAttach {
    RgShot *shots[ATTACH_MAX];
    unsigned int m_uAttachNum;
} RgWeaponAttach;

struct RgWeapon {
    int controlFlags;
    int robotId;
    int id;
    unsigned char unmodeled_0c[4];
    int shotRequested;
    void (*dispose)(RgWeapon *weapon);
    unsigned char unmodeled_18[4];
    void (*passTime)(RgWeapon *weapon, float dt);
    int (*shoot)(RgWeapon *weapon);
    void (*shotStop)(RgWeapon *weapon);
    void (*hitRobot)(RgWeapon *weapon, int robotId, int damage);
    void (*hitBG)(RgWeapon *weapon, int bgObject, int damage);
    unsigned char unmodeled_30[4];
    void (*setFree)(RgWeapon *weapon);
    unsigned char unmodeled_38[8];
    RgWeaponAttach shotAttach;
    unsigned char unmodeled_64[4];
    XrgActor *actor;
    unsigned char unmodeled_6c[4];
    RgEquip equip;
    int shotMotionTable[RG_ACTOR_CHAR_ROBNUM][3];
    int shotStartMotionTable[RG_ACTOR_CHAR_ROBNUM][3];
    unsigned char unmodeled_13c[0x48];
    /*
     * _CheckLockOn (ov12:0x00a1c098) computes the locked-on target's angle
     * in the weapon's own local XZ-plane (atan2f of the inverse-local-space
     * transform's x/z) and compares it against this inclusive [min, max]
     * range to decide whether the target counts as locked on.
     *
     * The published unmodeled_13c[0x64] (0x13c..0x1a0, 100 bytes) is split
     * here into unmodeled_13c[0x48] (0x13c..0x184, 72 bytes) + lockOnAngleMin
     * (4) + lockOnAngleMax (4) + unmodeled_18c[4] (0x18c..0x190, 4 bytes) +
     * targetPosition (RgVector, 16 bytes): 0x48+4+4+4+0x10 = 0x64, so the
     * total size and the following member's offset (targeting at 0x1a0) are
     * unchanged.
     */
    float lockOnAngleMin;
    float lockOnAngleMax;
    unsigned char unmodeled_18c[4];
    /*
     * _CheckLockOn (ov12:0x00a1c098) copies the locked-on target's last read
     * world position here (XrgCopyVector from __RgGeomPointGetPos's result)
     * whenever a target is present, whether or not it is within
     * lockOnAngleMin/lockOnAngleMax.
     */
    RgVector targetPosition;
    int targeting;
    int lockedOn;
    int shotMotionOffset;
    float shotNum;
    float weight;
    int lockOnEffector;
    int ess;
    int equipped;
    int lockOnStopped;
    int sound;
    /*
     * _ShotAttackType (ov12:0x00a1a540) reads shotSound/shotVolume as the
     * soundId/volume arguments of XrgSoundRingVol alongside sound as the
     * driver; no other function of this allocation touches either field.
     */
    int shotSound;
    /*
     * _HitRobotAttackType (ov12:0x00a1a720) plays a distinct impact sound
     * through XrgSoundRingVol using sound as the driver: this is its own
     * soundId, separate from shotSound above. This int exactly fills the
     * published unmodeled_1cc[4] span (4 bytes), so bgHitSound's offset
     * (0x1d0) is unchanged.
     */
    int robotHitSound;
    int bgHitSound;
    unsigned char unmodeled_1d4[4];
    int shotVolume;
    /*
     * _HitRobotAttackType (ov12:0x00a1a720) reads this as XrgSoundRingVol's
     * volume argument for robotHitSound above.
     *
     * The published unmodeled_1dc[0x14] (0x1dc..0x1f0, 20 bytes) is split
     * into this int robotHitVolume (4 bytes) + unmodeled_1e0[0x10]
     * (0x1e0..0x1f0, 16 bytes): 4+0x10 = 0x14, so RgWeapon's total size
     * (0x1f0 bytes) is unchanged.
     */
    int robotHitVolume;
    unsigned char unmodeled_1e0[0x10];
};

/*
 * The per-instance data _CreateWeaponAttackType (ov12:0x00a1a9c0) and
 * _CreateWeaponUnArmedType (ov12:0x00a1abc8) allocate on top of RgWeapon
 * (0x230 bytes total). _HitBgAttackType (ov12:0x00a1a6e8) reads damage as a
 * float at 0x1fc (lwc1, passed to RgBgObjTryToBreak) and clears active as an
 * int at 0x220 once the background object breaks; the span in between is
 * unattested by this allocation.
 */
typedef struct RgWeaponAttackType {
    RgWeapon common;
    /*
     * _PassTimeAttackType (ov12:0x00a1a5f0) reads followJoint/followActor as
     * XrgActorGetJointLocal's joint/actor pair (only sampled while
     * followActor is non-zero) to keep the attack's tracking geometry point
     * riding the actor's joint each frame.
     *
     * The published unmodeled_1f0[0x0c] (0x1f0..0x1fc, 12 bytes) is split
     * into this int followJoint (4) + unmodeled_1f4[4] (0x1f4..0x1f8, 4
     * bytes) + XrgActor *followActor (4): 4+4+4 = 0xc, so damage's offset
     * (0x1fc) is unchanged.
     */
    int followJoint;
    unsigned char unmodeled_1f4[4];
    XrgActor *followActor;
    float damage;
    /*
     * _DestructAttackType (ov12:0x00a1a6d0) reads this as the collision
     * geometry handle it passes to RgGeomFree; no other function of this
     * allocation touches it.
     */
    int geom;
    unsigned char unmodeled_204[0x0c];
    /*
     * _PassTimeAttackType (ov12:0x00a1a5f0) keeps the tracking geometry
     * point's own copy of the position RgGeomPointMovePos last moved it to,
     * so a frame with no new joint sample (followActor == 0) still has a
     * position to fall back to.
     *
     * The published unmodeled_204[0x1c] (0x204..0x220, 28 bytes) is split
     * into unmodeled_204[0x0c] (0x204..0x210, 12 bytes) + this RgVector
     * followPosition (16 bytes): 0xc+0x10 = 0x1c, so active's offset (0x220)
     * is unchanged.
     */
    RgVector followPosition;
    int active;
    /*
     * _ShotAttackType/_ShotStopAttackType (ov12:0x00a1a540/0x00a1a5a8) guard
     * XrgSoundRingVol with this flag: set once the attack's fire sound has
     * rung, cleared when the shot stops.
     */
    int shotSoundPlaying;
} RgWeaponAttackType;

/*
 * The per-instance data _CreateWeaponShotType (ov12:0x00a1a410) allocates on
 * top of RgWeapon (0x200 bytes total). _ShotTypeIsBusy/_ShotTypeFree
 * (ov12:0x00a1a1b8/0x00a1a218) read and clear a single float at 0x1f4: the
 * time remaining before the shot type can fire again.
 */
typedef struct RgWeaponShotType {
    RgWeapon common;
    unsigned char unmodeled_1f0[4];
    float cooldown;
    unsigned char unmodeled_1f8[8];
} RgWeaponShotType;

/*
 * _CreateWeaponShotType (ov12:0x00a1a410) only null-checks its essence and
 * creation-info arguments ("pEss != NIL", "pInfo != NIL",
 * ov12:0x00a53710/0x00a53720) and forwards them to the still-unrecovered
 * _InitWeaponShot; no interior of RgWeaponShotEssence is evidenced here.
 */
typedef struct RgWeaponShotEssence RgWeaponShotEssence;

/*
 * The per-instance data _CreateWeaponEnergyType (ov12:0x00a1b2c8) allocates on
 * top of RgWeapon (0x210 bytes total). _EnergyFree (ov12:0x00a1afc0) clears
 * both as ints (sw): active at 0x1f0, energyCharge at 0x1f8.
 */
typedef struct RgWeaponEnergyType {
    RgWeapon common;
    int active;
    unsigned char unmodeled_1f4[4];
    int energyCharge;
} RgWeaponEnergyType;

/*
 * Opaque per-type essence and creation-info handles _CreateWeaponAttackType,
 * _CreateWeaponUnArmedType, _CreateWeaponShieldType and _CreateWeaponEnergyType
 * only null-check ("pEss != NIL", "pInfo != NIL", ov12:0x00a53710/0x00a53720)
 * and forward to their still-unrecovered _InitWeaponAttack/_InitWeaponUnArmedType/
 * _InitWeaponShield/_InitEnergyType; no interior is evidenced here.
 */
typedef struct RgWeaponCreateInfo RgWeaponCreateInfo;
typedef struct RgWeaponAttackEssence RgWeaponAttackEssence;
typedef struct RgWeaponUnArmedEssence RgWeaponUnArmedEssence;
typedef struct RgWeaponShieldEssence RgWeaponShieldEssence;
typedef struct RgWeaponEnergyEssence RgWeaponEnergyEssence;

/*
 * CreateRgWeaponFromEssence (ov12:0x00a1b420) is the shared essence -> weapon
 * front end every concrete essence type's create-info pair goes through: it
 * only ever reads the create-method slot every essence type places at +0x00
 * (RgWeaponShieldEssence.m_pCreateMethod below, RgWeaponUnArmedEssenceInit's
 * own m_pCreateMethod further below), so only that one member is claimed
 * through this generic view.
 */
typedef struct RgWeaponEssenceCommon {
    RgWeapon *(*m_pCreateMethod)(void *essence, RgWeaponCreateInfo *info);
} RgWeaponEssenceCommon;

/*
 * InitRgWeaponShotEssence (ov12:0x00a1a4d8) evidences an in-memory
 * essence/init object at RgWeaponShotEssence's own address: a create-method
 * slot at +0x00 it points at _CreateWeaponShotType, a mode word at +0x04 it
 * sets to 0x20 (the same bit _CommonDisp tests as weapon->controlFlags &
 * 0x20), and a float at +0x3B0 it sets to 0.4f: the same offset
 * include/ov12/rg_weapon_db.h's own RgWeaponShotEssence.busyTime names from
 * the weapon database's "busy" key, so this allocation's own view of that
 * span keeps the same field name. No byte outside these spans is claimed.
 */
typedef struct RgWeaponShotEssenceInit {
    RgWeaponShotType *(*m_pCreateMethod)(RgWeaponShotEssence *pEss,
                                          RgWeaponCreateInfo *pInfo);
    int controlMode;
    unsigned char unmodeled_08[0x3a8];
    float busyTime;
} RgWeaponShotEssenceInit;

/*
 * InitRgWeaponUnArmedEssence (ov12:0x00a1ac90) evidences an in-memory
 * essence/init object at RgWeaponUnArmedEssence's own address: a create-method
 * slot at +0x00 it points at _CreateWeaponUnArmedType (the same
 * m_pCreateMethod pattern InitRgWeaponShieldEssence's RgWeaponShieldEssence
 * uses below), a mode word at +0x04 it sets to 76, a six-row table at +0xB0
 * (RG_ACTOR_CHAR_ROBNUM rows of 3, the same offset and shape as
 * RgWeaponShieldEssence's own table) it fills with motion IDs 21 and 22 and a
 * trailing -1 per row, an int at +0x188 it always clears to 0 (no function of
 * this allocation reads it back), and a terminator int at +0x3B0 it always
 * sets to -1. No byte outside these spans is claimed.
 *
 * ov12/tu027's src/ov12/rg_weapon_db.c (include/ov12/rg_weapon_db.h)
 * independently completes the SAME tag, RgWeaponUnArmedEssence, as a
 * data-driven weapon-database record: unmodeled_000[0x3b8] + damage at +0x3b8
 * + hitEffectName at +0x3bc (0x3e0 bytes total). That is a different field
 * layout at overlapping offsets (a float at +0x3b8 where this evidence claims
 * no member up to the terminator at +0x3b0..+0x3b4), not a partial view of
 * this same object, so RgWeaponUnArmedEssence itself is left exactly as
 * published above (an opaque forward declaration only: _CreateWeaponUnArmedType
 * already uses it that way) and this allocation's own evidence is named
 * RgWeaponUnArmedEssenceInit instead, cast from the RgWeaponUnArmedEssence
 * pointer InitRgWeaponUnArmedEssence receives.
 */
typedef struct RgWeaponUnArmedEssenceInit {
    RgWeapon *(*m_pCreateMethod)(RgWeaponUnArmedEssence *pEss,
                                  RgWeaponCreateInfo *pInfo);
    int controlMode;
    unsigned char unmodeled_08[0xa8];
    int shotMotionTable[RG_ACTOR_CHAR_ROBNUM][3];
    unsigned char unmodeled_f8[0x90];
    int resetFlag;
    unsigned char unmodeled_18c[0x224];
    int terminator;
} RgWeaponUnArmedEssenceInit;

/*
 * InitRgWeaponShieldEssence (ov12:0x00a1aed8) evidences the interior of
 * RgWeaponShieldEssence: a create-method slot at +0x00 that it points at
 * _CreateWeaponShieldType (the assert_prog string at ov12:0x00a53788,
 * "pEss->m_pCreateMethod != NIL", cited by the sibling
 * CreateRgWeaponFromEssence, names this member), a mode word at +0x04 that
 * it sets to 3, and a six-row table at +0xB0 (RG_ACTOR_CHAR_ROBNUM rows of
 * 3, the same shape as RgWeapon's own shotMotionTable) that it fills with
 * motion IDs 0x2B and 0x2C and a trailing -1 per row. No byte outside
 * these spans is claimed.
 */
struct RgWeaponShieldEssence {
    RgWeapon *(*m_pCreateMethod)(RgWeaponShieldEssence *pEss,
                                  RgWeaponCreateInfo *pInfo);
    int controlMode;
    unsigned char unmodeled_08[0xa8];
    int shotMotionTable[RG_ACTOR_CHAR_ROBNUM][3];
};

/*
 * The type-specific callback slots RgWeaponShotStop/RgWeaponHitRobot/
 * RgWeaponHitBG/RgWeaponPassTime/RgWeaponSetFree invoke when installed
 * (ov12:0x00a1bd78..0x00a1c000): shotStop 0x24 (lw 36($16) in
 * RgWeaponShotStop), hitRobot 0x28 and hitBG 0x2c (lw 40($16)/44($16),
 * annotations/overlays/ov12_annotations.csv: "the weapon's robot-hit
 * callback stored at offset 0x28"/"the optional background-hit callback at
 * 0x2c"), passTime 0x1c (lw 28($16) in RgWeaponPassTime, called with the
 * same float RgWeaponPassTime received) and setFree 0x34 (lw 52($16),
 * annotations: "the weapon's optional free-state callback"). hitRobot and
 * hitBG forward the two ints RgHandlerWeaponVsRobot/RgHandlerWeaponVsBG
 * report from the collision (annotations/overlays/ov12_annotations.csv);
 * _HitBgAttackType (ov12:0x00a1a6e8, the attack-type weapon's hitBG) reads
 * only the first of them as its bgObject argument, so the second is named
 * from the domain (damage) without being read by that one implementation.
 *
 * RgWeaponHitBG additionally reads a second sound handle at 0x1d0
 * (lw 464($16)) alongside the driver at 0x1c4, passing the same value as
 * both the soundId and volume arguments of XrgSoundRingVol (annotations:
 * "plays the ring sound using handles at 0x1c4 and 0x1d0").
 *
 * _GetShotInfo (ov12:0x00a1c678) reads a scalar at 0x08 (lw 8($18)) and
 * copies it unmodified into the shot descriptor it builds; no other
 * function of this allocation touches it, so only its presence next to
 * robotId is claimed, named `id` for the weapon's own instance identifier.
 *
 * _PlayShotMotion (ov12:0x00a1c330, "starts shot motion") indexes
 * weapon+0xf4 with the same (charID*3+shotMotionOffset) arithmetic
 * RgWeaponRobotShotMotion uses on shotMotionTable at 0xac, one full table
 * (0x48 bytes) further in: a second RG_ACTOR_CHAR_ROBNUM-by-3 table this
 * allocation names shotStartMotionTable. Only the one cell this function
 * reads is evidenced past that; nothing claims the remaining span.
 */

/*
 * The descriptor _GetShotInfo (ov12:0x00a1c678) fills in for its callers
 * _ShotTypeShot and _EnergyPassTime (both still INCLUDE_ASM in this TU):
 * weapon at +0x00, a position/direction callback at +0x04 (always set to
 * _GetDefShotPosDir, ov12:0x00a1c3d0, "writes default shot position and
 * direction when enabled, returning whether data was produced"), the
 * weapon's target at +0x08 (RgRobotGetTarget's own return value, a
 * pointer), weapon->id copied unmodified at +0x0c, and the target's
 * confusion state at +0x10 (RgRobotIsConfused's return value). Nothing
 * past +0x10 is written by this allocation, so the descriptor's full
 * extent is not claimed here.
 */
typedef struct RgWeaponShotRequest RgWeaponShotRequest;
typedef int (*RgWeaponGetPosDirFunc)(RgWeaponShotRequest *info, RgVector position,
                                     RgVector direction);

struct RgWeaponShotRequest {
    RgWeapon *weapon;
    RgWeaponGetPosDirFunc getPosDir;
    void *target;
    int id;
    int targetConfused;
};

#endif /* SRC_OV12_RG_WEAPON_H */
