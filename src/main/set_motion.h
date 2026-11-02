/*
 * TU-local declarations of main/tu193 (src/main/set_motion.c).
 */

#ifndef SRC_MAIN_SET_MOTION_H
#define SRC_MAIN_SET_MOTION_H

#include "shared.h"

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

/* The enepc entries are the canonical EnemyWorkEntry (include/main/enemy_1.h).
 * Its spline walk parameter (+0x40) counts this many units per spline point:
 * Enemy_Route wraps it at (point count << 7) (0x002cc5d0). */
#define ENEMY_SPLINE_UNITS_PER_POINT     0x80

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

struct Actor;

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
typedef struct ActorHead {
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
} ActorHead;

/* The 0x70-byte ActorHead remains the embedded prefix. Set_Motion and
 * Sound_FootStep additionally read or write the named Actor members below at
 * offsets confirmed by their field accesses and neighboring actor code. */

#endif /* SRC_MAIN_SET_MOTION_H */
