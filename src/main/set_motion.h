/*
 * TU-local declarations of main/tu193 (src/main/set_motion.c).
 */

#ifndef SRC_MAIN_SET_MOTION_H
#define SRC_MAIN_SET_MOTION_H

#include "shared.h"

/* The executable exports this 64-entry enemy-work allocation. */
typedef struct EnemyWork {
    unsigned char bytes[0x38b0];
} EnemyWork;

/* Canonical single definition of the shared enemy spline state store for
 * the coherent three-function TU (Script_Action, Set_Spline_By_Route,
 * Set_Spline_By_Random), Spark attempt-cacc3f0a0396.
 *
 * Changed TU-organization hypothesis versus the predecessor padded-struct
 * TU (two separate EnemySplineState definitions with bytes_before/after
 * padding): exactly ONE definition of the store exists in source, held by
 * this header, and every contiguous-extent projection stages these same
 * bytes. The original 232192-byte store (16 entries of 0x38b0 bytes) lives
 * at main .data 0x00379110 under the original GLOBAL symbol `enepc`. The
 * original instructions multiply the actor id byte by the 0x38b0 stride,
 * and the witnessed symbol extent is exactly 16 * 0x38b0, which supports
 * opaque byte-block addressing without proving any historical member
 * layout or route capacity. Access is by witnessed offsets only; unknown
 * layout stays outside the explicit type. No bytes_before/after
 * placeholders, no padded complete structs, no invented bounds, no
 * aliases. The original store is zero-initialized, a witnessed byte fact.
 *
 * Noncontiguous code extents (Script 0x002d0e48+300 versus Route/Random
 * 0x002d1768+224, with Move_BeltConveyer, Check_Discovery, Check_Encount
 * and Enemy_After_Battle between them) force one strict packet per
 * contiguous span under the comparer's original-function-coverage rule,
 * so each linked proof stages this header once; there is exactly one
 * definition in source.
 */
typedef unsigned char EnemyStateBlock[0x38b0];

/* Recovered head of the engine's actor record (task layout-actor-family, the
 * `actor_bytes` named-layout job; the full evidence is in the same type's
 * comment in src/main/near_dir.h, which defines it identically for main/tu257).
 *
 * `actor` is the 64-entry array at main 0x0043c1e0 with a 0xa70 stride
 * (readelf gives `0043c1e0 0x29c00 OBJECT GLOBAL actor`, and 0x29c00 is
 * exactly 64 * 0xa70, so the symbol size corroborates both the stride and
 * the entry count; every site is `lui 0x44` + `addiu -0x3e20`, and reading
 * the `lui` half alone is how 0x0044c1e0 got written here and in three other
 * places, corrected 2026-09-13) that
 * ACT_create hands out (0x00305cd0) and ACT_update walks once a frame
 * (0x00305f58). This TU's `actor` is that record and not a separate object:
 * Enemy_Init reads +0x54 of the pointer it later passes to Set_Spline_By_Random
 * and writes +0x9e4 of it two instructions later (0x002cb3f0/0x002cb408), and
 * the byte at +0x80 both functions index enepc with is the slot number
 * ACT_create writes there (0x00305de0). src/main/enemy_2.c calls the same
 * object EnemyActor.
 *
 * The type stops at +0x70 because that is where the evidence stops: nothing
 * between +0x70 and +0x80 is recovered, so the slot number at +0x80 and the
 * target angle at +0x9e4 stay outside it and keep the byte view below.
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
} Actor;

/* The actor's target facing angle, in radians like Actor.rotation.
 *
 * Enemy_Init seeds it from the actor's own yaw, Actor.rotation.y at +0x54
 * (lwc1 f0,0x54(s3) at 0x002cb3f0; swc1 f0,0x9e4(s3) at 0x002cb408);
 * Enemy_Command_Sac_Turn writes the converted command angle there whenever the
 * turn is instantaneous (0x002d34a4, src/main/enemy_2.c, which reaches the same
 * word through its own `enemy_actor_command_angle`); ACT_updateEnemy and
 * Enemy_Route steer with it (0x002cdcc0, 0x002cc60c). Set_Spline_By_Random below
 * hands it to BSpline_Init as the heading the random spline leaves along.
 *
 * It is 0x974 bytes past the end of the recovered head, with nothing in between
 * recovered, so it cannot be a member of Actor without inventing that span
 * (docs/naming.md); naming the offset is docs/style.md rule 2's documented
 * fallback and expands to the identical expression the accepted body had. */
#define ACTOR_TARGET_ANGLE_OFFSET 0x9e4

/* Evidenced offsets inside one enepc entry. The entry's layout as a whole is
 * not recovered - the store above stays an opaque byte block - so only the
 * offsets these two functions and their neighbours witness are named, and the
 * spans between them get no members.
 *
 *  +0x40 / +0x42  Enemy_Route hands +0x40 to GetBSplineLoop as the walk
 *                 parameter next to the constant 0x80 (0x002cc51c), wraps it
 *                 to 0 at (point count << 7) (0x002cc5d0) and steps it by 2 or
 *                 by 0x10 per frame; whenever (parameter & 0x7f) == 0 it
 *                 decrements +0x42 while that is positive (0x002cc85c), and it
 *                 takes the 0x10 step exactly while +0x42 is nonzero
 *                 (0x002cc534). So the parameter counts 0x80 units per spline
 *                 point and +0x42 counts the points still walked at the fast
 *                 rate. ACT_createEnemy (0x002d425c) clears both.
 *  +0x80          the spline control points, 0x10 apart: Enemy_Route passes
 *                 this address to GetBSplineLoop, and Set_Spline_By_Random
 *                 lets BSpline_Init write four of them here.
 *  +0x1080        their count, the second GetBSplineLoop argument and the
 *                 value the walk parameter wraps against.
 *  +0x1090        the route points, also 0x10 apart: Enemy_Init (0x002cb448)
 *                 copies the script's 12-byte triples into
 *                 +0x1090/+0x1094/+0x1098 of successive entries.
 *  +0x3094        the route point count Enemy_Init writes from that loop
 *                 (0x002cb434) and rewrites when it mirrors the route.
 *
 * Why these are named offsets and not struct members (docs/style.md rule 2,
 * the middle grade). The original addresses every one of them from the entry
 * itself: `lh v0,0x3094(a0)`, `sh a0,0x1080(a0)`, `sh v0,0x40(a0)`. Only a
 * struct whose base is the entry reproduces that, and the entry is 0x38b0
 * bytes with nothing recovered between +0x42 and +0x1080, so such a struct
 * could only be reached by inventing the spans between the named offsets,
 * which the store's own record above refuses. The alternative that keeps the
 * source honest - a typed reference per offset, `short *route_point_count =
 * (short *)(state + 0x3094)` and a two-member view over +0x40/+0x42 - was
 * compiled and measured: it costs one `addiu` per reference and grows
 * Set_Spline_By_Route from 0x88 to 0x8c bytes and Set_Spline_By_Random from
 * 0x58 to 0x68 (0x5c with the references taken after the call), so `ld`
 * refuses the main link with "cannot move location counter backwards (from
 * 002d3840 to 002d3828)". The accessors below expand to exactly the
 * expression the accepted source already had, so the bytes are unchanged and
 * each offset gains the name it was missing.
 */
#define ENEMY_SPLINE_PARAMETER_OFFSET    0x40
#define ENEMY_SPLINE_FAST_POINTS_OFFSET  0x42
#define ENEMY_SPLINE_POINTS_OFFSET       0x80
#define ENEMY_SPLINE_POINT_COUNT_OFFSET  0x1080
#define ENEMY_ROUTE_POINTS_OFFSET        0x1090
#define ENEMY_ROUTE_POINT_COUNT_OFFSET   0x3094
#define ENEMY_SPLINE_UNITS_PER_POINT     0x80

#define ENEMY_SPLINE_PARAMETER(entry) \
    (*(short *)((unsigned char *)(entry) + ENEMY_SPLINE_PARAMETER_OFFSET))
#define ENEMY_SPLINE_FAST_POINTS(entry) \
    (*(short *)((unsigned char *)(entry) + ENEMY_SPLINE_FAST_POINTS_OFFSET))
#define ENEMY_SPLINE_POINTS(entry) \
    ((float (*)[4])((unsigned char *)(entry) + ENEMY_SPLINE_POINTS_OFFSET))
#define ENEMY_SPLINE_POINT_COUNT(entry) \
    (*(short *)((unsigned char *)(entry) + ENEMY_SPLINE_POINT_COUNT_OFFSET))
#define ENEMY_ROUTE_POINTS(entry) \
    ((float (*)[4])((unsigned char *)(entry) + ENEMY_ROUTE_POINTS_OFFSET))
#define ENEMY_ROUTE_POINT_COUNT(entry) \
    (*(short *)((unsigned char *)(entry) + ENEMY_ROUTE_POINT_COUNT_OFFSET))

/* The per-actor table Get_DefaultMotion below indexes by a motion number.
 * Its callers pass the actor pointer itself as the first argument: main/
 * tu194's Enemy_ActionReady calls it with the actor in a0 and a small
 * literal motion number in a1 (`move a0,s2` / `li a1,2` / `jal
 * Get_DefaultMotion` at 0x002d3b3c..0x002d3b4c), and Before_Talk in this
 * unit calls it the same way with the actor it was given and the literal 3
 * (`li a1,3` at 0x002d0a54, `jal Get_DefaultMotion` at 0x002d0a5c), then
 * hands the returned motion id to Set_Motion (0x002d0a64..0x002d0a6c).
 * Get_DefaultMotion's own body adds twice the
 * motion number to the actor pointer and reads a halfword there (lh
 * v0,0x130(a0) at 0x002d0a10), so the table is an array of short motion ids
 * starting at +0x130. It is 0xc0 bytes past the end of the recovered Actor
 * type, with nothing in between recovered, so it stays a named offset
 * (docs/style.md rule 2) instead of a member of Actor. */
#define ACTOR_DEFAULT_MOTION_TABLE_OFFSET 0x130

#define ACTOR_DEFAULT_MOTION_TABLE(actor) \
    ((short *)((unsigned char *)(actor) + ACTOR_DEFAULT_MOTION_TABLE_OFFSET))

/* The actor's enepc slot number, set by ACT_create when it hands the record
 * out (see the Actor struct comment above). EnemySoundEnd reads it right
 * before tearing the sound down and adds one for the stop-call flags
 * (`lbu v1,0x80(s0)` / `addiu a1,v1,1` at 0x002d0860/0x002d086c). It cannot
 * be a member of Actor without inventing the unrecovered span between +0x70
 * and +0x80 (docs/naming.md); naming the offset is docs/style.md rule 2's
 * documented fallback. */
#define ACTOR_NUMBER_OFFSET 0x80

#define ACTOR_NUMBER(actor) \
    (((unsigned char *)(actor))[ACTOR_NUMBER_OFFSET])

/* The actor's active sound-effect id, matched against RES_GetEnemySeBank's and
 * RES_GetEnemySeType's own EnemySeBank[].id table entries. EnemySoundEnd reads
 * it right before tearing the sound down (`lh a0,0x86(s0)` at 0x002d085c,
 * `jal RES_GetEnemySeBank`). It begins 5 bytes after `number` (+0x80) ends,
 * with nothing between them recovered, so it cannot be a member of Actor
 * without inventing that span (docs/naming.md); naming the offset is
 * docs/style.md rule 2's documented fallback. */
#define ACTOR_SOUND_EFFECT_ID_OFFSET 0x86

#define ACTOR_SOUND_EFFECT_ID(actor) \
    ((short *)((unsigned char *)(actor) + ACTOR_SOUND_EFFECT_ID_OFFSET))

/* The actor's look-at countdown, in frames. Actor_LookAt_Init clears it to 0
 * (`sh $0,0x9EC($4)` at main:0x002d2d2c); Actor_LookAt treats a value below 2
 * as "not looking" and returns without reading the target below (`lhu
 * $2,0x9EC($5)` / `addiu $2,$2,-1` / `sltiu $2,$2,2` / `beqz` at
 * main:0x002d2ba8..0x002d2bbc). It is 8 bytes past ACTOR_TARGET_ANGLE_OFFSET,
 * with nothing between them recovered, so it cannot be a member of Actor
 * without inventing that span (docs/naming.md); naming the offset is
 * docs/style.md rule 2's documented fallback. */
#define ACTOR_LOOKAT_TIMER_OFFSET 0x9ec

#define ACTOR_LOOKAT_TIMER(actor) \
    ((short *)((unsigned char *)(actor) + ACTOR_LOOKAT_TIMER_OFFSET))

/* The actor's look-at target: another `actor` slot index, or -1 for none.
 * Actor_LookAt_Init sets it to -1 (`addiu $2,$0,-1` / `sh $2,0x9EE($4)` at
 * main:0x002d2d28/0x002d2d30); Actor_LookAt reads it and treats -1 as "no
 * target" (`lh $4,0x9EE($5)` / `beq $4,$2,.L002D2C54` at
 * main:0x002d2bc4..0x002d2bcc), otherwise multiplying it by the `actor`
 * array's own 0xa70 stride to reach the target's record. */
#define ACTOR_LOOKAT_TARGET_OFFSET 0x9ee

#define ACTOR_LOOKAT_TARGET(actor) \
    ((short *)((unsigned char *)(actor) + ACTOR_LOOKAT_TARGET_OFFSET))

/* A per-enepc-entry pointer, cleared by Actor_LookAt_Init (`sw
 * $0,0x386C($1)` at main:0x002d2d40) and set by Actor_LookAt to the address
 * of the same entry's +0x3890 once a look-at target is found (`addiu
 * $5,$6,0x3890` / `sw $5,0x386C($6)` at main:0x002d2be0/0x002d2be4). Only the
 * pointer identity is claimed here; +0x3890's own contents belong to
 * Actor_LookAt, still unrecovered. */
#define ENEMY_LOOKAT_POINT_OFFSET 0x386c

#define ENEMY_LOOKAT_POINT(entry) \
    ((void **)((unsigned char *)(entry) + ENEMY_LOOKAT_POINT_OFFSET))

/* The per-enepc-entry reaction value Get_JAVAReaction reads and returns
 * unchanged (`lw $2,%lo(enepc + 0x37E0)($2)` at main:0x002d09f4). */
#define ENEMY_JAVA_REACTION_OFFSET 0x37e0

#define ENEMY_JAVA_REACTION(entry) \
    ((int *)((unsigned char *)(entry) + ENEMY_JAVA_REACTION_OFFSET))

#endif /* SRC_MAIN_SET_MOTION_H */
