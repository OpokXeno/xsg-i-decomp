/*
 * TU-local declarations of main/tu194 (src/main/enemy_2.c).
 */

#ifndef SRC_MAIN_ENEMY_2_H
#define SRC_MAIN_ENEMY_2_H

#include "shared.h"

/*
 * The engine's actor record.
 *
 * `actor` is the 64-entry array at main 0x0043c1e0 with a 0xa70 stride. The
 * executable exports it: readelf gives `actor` GLOBAL OBJECT 0x0043c1e0 size
 * 0x29c00, and 0x29c00 is exactly 64 * 0xa70. Every site materialises it the
 * same way, `lui 0x44` followed by `addiu -0x3e20` - ACT_init
 * 0x00305b28/0x00305b34, ACT_create 0x00305cf4/0x00305cf8, ACT_update
 * 0x00305f7c/0x00305f80, ACT_info 0x003061b8/0x003061bc - so the `lui` half
 * alone is not the address. ACT_init (0x00305af8) initializes all 64 entries,
 * ACT_create (0x00305cd0) hands out the first free one, ACT_update
 * (0x00305f58) walks the array once a frame and ACT_draw (0x00305ef8) draws it.
 *
 * This TU used to call the same object `EnemyActor` and declare a two-member
 * head of it (`unsigned char prefix[0x50]; float rotation[3];`); it is the
 * record src/main/near_dir.h (main/tu257) and src/main/set_motion.h
 * (main/tu193) define as `Actor`, and the name is reconciled here. The tie is
 * Enemy_Init (0x002cb278): it takes the pointer in s3 (move s3,a0 at
 * 0x002cb28c), reads +0x54 of it (lwc1 f0,0x54(s3) at 0x002cb3f0), writes +0x9e4
 * of that same pointer six instructions later (swc1 f0,0x9e4(s3) at 0x002cb408,
 * in the delay slot of the bnez at 0x002cb404), indexes `enepc` with +0x80 of it
 * and hands it to Set_Spline_By_Random and Set_Spline_By_Route (0x002cb660,
 * 0x002cb668) - the three offsets this TU, main/tu193 and main/tu257 each use on
 * their own name for the object. Two further witnesses: ACT_createEnemy stores
 * ACT_updateEnemy into the entry's +0x04 (0x002d432c), which ACT_update calls
 * with the entry as its only argument (0x00305f94), and Java_xeno_Chr_move
 * reads +0x80 of the Chr peer (lbu v0,0x80(s5) at 0x002fd370).
 *
 * The head (+0x00..+0x6f) is the type src/main/near_dir.h recovered. Its
 * evidence is that file's and is summarised here so the type reads in this TU;
 * only the two members below it - `global_position` and `number` - were
 * evidenced by the job that added them, and only those two carry a claim this
 * file stands behind. Most of the head comes from the game's own debug printer
 * ACT_info (0x00306090), which prints an actor under its own column headings:
 *
 *   +0x00 flags       "FLAGS %08x" (0x0030622c/0x00306240). ACT_init writes
 *                     0x20, ACT_createChr 1, Java_xeno_Chr_move ORs bit 1.
 *   +0x04 update      ACT_update loads it, skips the entry when it is null and
 *                     otherwise calls it with the actor as its only argument
 *                     (0x00305f94..0x00305fa4).
 *   +0x08 draw        the second hook: MenuModelUnitActorSet writes
 *                     MenuModelAlphaDraw here two instructions after it writes
 *                     `update` (0x00280110); ACT_init and ACT_dispose2 clear it
 *                     beside `update`.
 *   +0x0c             declared not a field: the gap the quadword-aligned
 *                     vector block forces after three 4-byte members. The
 *                     sweep behind that admission is near_dir.h's and was
 *                     re-run independently; the handful of apparent hits are
 *                     false positives on registers redefined inside long
 *                     functions (Actor_LookAt 0x002d2c44 is an `enepc` entry
 *                     + 0xc, ACT_createEnemy 0x002d4190 is actor + 0xc0 + 0xc,
 *                     Set_PlayerHistory 0x002d18b4 is an unrelated global).
 *                     A genuine access at +0x0c would retire the admission.
 *   +0x10 position    ACT_info's "     POS      ROT" column; SEQ_moveSPL loads
 *                     all 16 bytes with one lqc2.
 *   +0x20 previous_position
 *                     a ONE-WITNESS inference, kept only so this TU spells the
 *                     type the way main/tu257 and main/tu193 do. What the six
 *                     images actually show is a single write, GameCfPlayerMove
 *                     storing the position there at the top of the frame
 *                     (0x002494f0), and no read at all; the subtraction later
 *                     in that function (0x00249ac0..0x00249ae0) works on a
 *                     stack-saved copy and writes +0x30 and +0x38, not +0x20.
 *                     This TU uses none of it.
 *   +0x30 velocity    added to the position once the frame's motion is settled.
 *   +0x40 acceleration
 *                     added to the velocity in the same place.
 *   +0x50 rotation    ACT_info's "ROT" column, printed / pi * 180.0f, so
 *                     radians. This is the member Enemy_Command_Sac_Turn below
 *                     selects one lane of.
 *   +0x60 scale       ACT_info's "     SCL" column; ACT_init and ACT_create set
 *                     all four lanes to 1.0f.
 *
 * Two members are recovered here, past where near_dir.h stopped
 * (task style-enemy-actor-family-20260913):
 *
 *   +0x70 global_position
 *                     ACT_modelDrawSub (0x003074b8, its base register is the
 *                     actor: it reads the +0x6f0 flags word at 0x00307508, the
 *                     +0x8fc parent at 0x003075c8 and the +0x60 scale at
 *                     0x003076d8) calls nmlModelGetGblPosition(actor + 0x70)
 *                     after the model is entered and after it is drawn
 *                     (addiu a0,s1,0x70 at 0x00307990 and 0x003079c4).
 *                     nmlModelGetGblPosition (0x0022f2c0) copies the 16-byte
 *                     s_inGblPos quadword into its argument with lq/sq and
 *                     writes 1.0f at +0xc of it, which fixes the member's
 *                     extent at 16 bytes, its quadword alignment and its w
 *                     lane. The same function calls
 *                     xglPointLengthXZ(actor + 0x70, ...) at 0x00307754
 *                     (addiu a0,s1,0x70), and xglPointLengthXZ (0x002297b8)
 *                     lqc2s both arguments and returns the XZ distance between
 *                     them, so +0x70 is read as a point. ACT_init seeds its w
 *                     lane with 1.0f (swc1 f20,0x7c(s0) at 0x00305b84) exactly
 *                     as it seeds position.w (+0x1c) and scale.w (+0x6c).
 *                     Nothing in any of the six images reads or writes +0x70,
 *                     +0x74 or +0x78 of an actor, which is why the member is
 *                     named after the call that fills it and its lanes are not
 *                     named one by one.
 *   +0x80 number      the actor's own slot in the 64-entry array. ACT_create
 *                     stores the index it hands out there (sb a2,0x80(s0) at
 *                     0x00305de0) and ACT_info prints an actor's parent and
 *                     each of its children by reading +0x80 of the linked
 *                     actor (lbu a1,0x80(v0) at 0x003064f8, lbu a2,0x80(v0) at
 *                     0x00306534) under "PARENT %d" / "CHILD[%d] %d". 51
 *                     distinct call sites index `enepc` (stride 0x38b0) or
 *                     `actSequence` (stride 0x260) with it; this TU is one of
 *                     them and main/tu193 is another. It is one byte: every
 *                     access in the six images is lbu or sb.
 *
 * The type stops after `number` because that is where the evidence stops. The
 * record is 0xa70 bytes and the offsets past +0x80 that are known are
 * scattered, so they cannot become members without inventing the spans between
 * them (docs/naming.md), and they keep the named-offset spelling of
 * docs/style.md rule 2 instead:
 *
 *   +0x82/+0x83/+0x84/+0x86  ACT_create writes the caller's id word into +0x86
 *                     (sh t0,0x86) and its bits 16..23 into +0x82 (sb v1,0x82
 *                     after sra v1,t0,16); +0x86 is also the in-use marker
 *                     ACT_create scans for and ACT_update skips on. +0x81 has
 *                     one witness in the whole game (ACT_initSequence clears
 *                     it), +0x83 and +0x84 only ACT_init clearing them, so the
 *                     block +0x81..+0x87 is not named here and `number` is the
 *                     last member.
 *   +0x6f0            a second flags word, printed by ACT_info with the same
 *                     "FLAGS %08x" as +0x00 (0x00306374); ACT_pauseUpdate sets
 *                     and clears bit 0x80000 in it across all 64 entries.
 *   +0x6f8            the actor's speed, ACT_info's "SPEED %8.3f" (0x00306378).
 *   +0x704            its motion number, ACT_info's "NO    %4d" (0x00306360,
 *                     read with lhu).
 *   +0x8fc/+0x900/+0x904
 *                     the parent link, the child count and the child array,
 *                     ACT_info's "PARENT %d" and "CHILD[%d] %d".
 *   +0x9e4            the target facing angle, declared below.
 *
 * None of those five is used by this TU, so none of them is declared here.
 */
typedef struct Actor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    Vector4 global_position;
    u8 number;
} Actor;

/* The actor's target facing angle, in radians like Actor.rotation.
 *
 * Enemy_Init seeds it from the actor's own yaw, Actor.rotation.y at +0x54
 * (lwc1 f0,0x54(s3) at 0x002cb3f0; swc1 f0,0x9e4(s3) at 0x002cb408);
 * Enemy_Command_Sac_Turn below writes the converted command angle there
 * whenever the turn is instantaneous (swc1 f12,0x9e4(a0) at 0x002d34a4);
 * ACT_updateEnemy and Enemy_Route steer with it (0x002cdcc0, 0x002cc60c) and
 * Set_Spline_By_Random (src/main/set_motion.c) hands it to BSpline_Init as the
 * heading the random spline leaves along.
 *
 * It is 0x964 bytes past the end of the recovered type, with nothing in
 * between recovered, so it cannot be a member of Actor without inventing that
 * span (docs/naming.md); naming the offset is docs/style.md rule 2's
 * documented fallback. src/main/set_motion.h spells the same offset under the
 * same name for main/tu193. */
#define ACTOR_TARGET_ANGLE_OFFSET 0x9e4

#define ACTOR_TARGET_ANGLE(actor) \
    (*(float *)((unsigned char *)(actor) + ACTOR_TARGET_ANGLE_OFFSET))

/* The executable exports this 64-entry enemy-work allocation. */
typedef struct EnemyWork {
    unsigned char bytes[0x38b0];
} EnemyWork;

/* canon: config/header-canon.json chose src/math/main/002d33f8-sac-turn/private.h over 0 other accepted spellings */
extern EnemyWork enepc[16];

/* The turn state inside one `enepc` entry.
 *
 * The entry's layout as a whole is not recovered - src/main/set_motion.h keeps
 * the same store an opaque 0x38b0-byte block and names only the offsets its own
 * two functions witness (+0x40, +0x42, +0x80, +0x1080, +0x1090, +0x3094) - so
 * the spans this TU does not witness get no members either, and the entry does
 * not become a struct. It is the same store: Enemy_Command_Sac_Turn indexes
 * `enepc` with Actor.number, which is what Set_Spline_By_Route and
 * Set_Spline_By_Random index it with too.
 *
 * +0x3824..+0x382f IS recovered, as one twelve-byte record, because the game
 * reads it as one. Script_Action (0x002d0ee8..0x002d0f28) skips the record when
 * the halfword at +0x382c is -1 and otherwise computes
 * `start + (target - start) * frame / duration` from +0x3824, +0x3828, +0x382c
 * and +0x382e in that order, incrementing +0x382c by one on the way
 * (addiu a0,a0,1 at 0x002d0f0c); this function writes exactly those four,
 * seeding +0x3824 from the actor's current rotation lane and +0x3828 from the
 * converted command angle; and ACT_createEnemy builds a fresh entry with -1 in
 * +0x382c and 0 in +0x382e (sh t0,0x382c(s1) at 0x002d42f8 with t0 = -1 from
 * 0x002d4258, sh zero,0x382e(s1)), which is the same "no turn in progress"
 * marker this function's instantaneous branch writes. So the four are the
 * start angle, the target angle, the elapsed frame count and the frame count
 * the turn is to take, and `EnemyTurn` names them.
 *
 * It stays a view taken at a named offset rather than a member of the entry
 * (docs/style.md rule 2, the middle grade): nothing between the entry's base
 * and +0x3824 is recovered, so a struct whose base is the entry could only be
 * written by inventing 0x3824 bytes, which docs/naming.md refuses. The view
 * was compiled and measured (form 04): ee-gcc 2.96 folds ENEMY_TURN_OFFSET into
 * the displacement of each store, so the function keeps its original 216 bytes
 * and all six EE images stay identical. That is the opposite of what the same
 * store cost main/tu193, where a typed reference per offset cost one addiu per
 * reference - the difference is that all four fields here are addressed from
 * one base in one basic block.
 *
 * +0x37e0 is the turn request flags word and lies 0x44 bytes before the
 * record, with nothing in between recovered, so it keeps its own named offset
 * rather than joining EnemyTurn. This function ORs ENEMY_TURN_REQUEST into it
 * to ask for a timed turn; nineteen other functions of main read or write the
 * same word, among them Script_Action, ACT_updateEnemy, Enemy_Route and
 * ACT_createEnemy, which clears it when it builds the entry (sw zero,0x37e0(s1)
 * at 0x002d42b8).
 */
#define ENEMY_TURN_FLAGS_OFFSET 0x37e0
#define ENEMY_TURN_OFFSET       0x3824

typedef struct EnemyTurn {
    float start_angle;
    float target_angle;
    short frame;
    short duration;
} EnemyTurn;

#define ENEMY_TURN(work) \
    ((EnemyTurn *)((unsigned char *)(work) + ENEMY_TURN_OFFSET))

#define ENEMY_TURN_FLAGS(work) \
    (*(unsigned int *)((unsigned char *)(work) + ENEMY_TURN_FLAGS_OFFSET))

/* The bit this function sets in ENEMY_TURN_FLAGS to ask for a timed turn. */
#define ENEMY_TURN_REQUEST 0x10000

void Enemy_Command_Sac_Turn(Actor *actor, int duration,
                            float angle_degrees, float angle_mode,
                            short axis);

/* These are distinct literal-pool witnesses despite equal numeric values. */
extern const float sac_turn_pi;
extern volatile const float sac_turn_two_pi_subtract;
extern volatile const float sac_turn_two_pi_add;

#endif /* SRC_MAIN_ENEMY_2_H */
