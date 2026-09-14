#ifndef RECOVERY_00309D50_PRIVATE_H
#define RECOVERY_00309D50_PRIVATE_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    void *spline;
    short frame;
    short last_frame;
    u16 flags;
} SplineTrack;

typedef struct {
    u32 flags;
    u32 state_flags;
} SequenceState;

typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

/*
 * The engine's actor record, recovered head.
 *
 * `actor` is the 64-entry array at main 0x0043c1e0 with a 0xa70 stride
 * (readelf gives `0043c1e0 0x29c00 OBJECT GLOBAL actor`, and 0x29c00 is
 * exactly 64 * 0xa70, so the symbol size corroborates both the stride and
 * the entry count; every site is `lui 0x44` + `addiu -0x3e20`, and reading
 * the `lui` half alone is how 0x0044c1e0 got written here and in three other
 * places, corrected 2026-09-13):
 * ACT_init (0x00305af8) initializes all 64 entries, ACT_create (0x00305cd0)
 * hands out the first free one, ACT_update (0x00305f58) walks the array once a
 * frame and ACT_draw (0x00305ef8) draws it. Both functions below are handed one
 * of its entries: Java_xeno_Chr_move (0x002fd2e8) reads the Chr peer out of the
 * Java object and passes it here, and Enemy_Init (0x002cb278) passes the same
 * kind of pointer to Set_Spline_By_Random in main/tu193, reading +0x54 from it
 * and writing +0x9e4 of it in one breath (0x002cb3f0/0x002cb408). So
 * src/main/near_dir.c's actor, src/main/set_motion.c's actor and
 * src/main/enemy_2.c's EnemyActor are one object; the earlier `SequenceActor`
 * name was specific to this file and is retired here.
 *
 * The type stops at +0x70 because that is where the evidence stops, not at a
 * recovered end: 0xa70 bytes follow and only scattered offsets of them are
 * known (+0x80 the actor's own slot number, +0x86 its id, +0x6f0 a second flags
 * word, +0x6f8 its speed, +0x704 its motion number, +0x8fc/+0x900/+0x904 the
 * parent and child links, +0x9e4 the turn target). Nothing between +0x70 and
 * +0x80 is recovered, so `number` is not a member and the two functions below
 * keep reading it through the byte view, as does main/tu193.
 *
 * Field evidence, mostly from the game's own debug printer ACT_info
 * (0x00306090), which prints this record with its own column headings:
 *
 *   +0x00 flags       printed with "FLAGS %08x" (0x0030622c/0x00306240).
 *                     ACT_init writes 0x20, ACT_createChr 1 and
 *                     Java_xeno_Chr_move ORs bit 1 in (0x002fd3ec); ACT_dispose2
 *                     clears it (0x00305eb0).
 *   +0x04 update      ACT_update loads it, skips the entry when it is null and
 *                     otherwise calls it with the actor as its only argument
 *                     (0x00305f94..0x00305fa4). ACT_createChr stores
 *                     ACT_updateSequence (0x0030b5b4), ACT_createNPC
 *                     ACT_updateNPC and MenuModelUnitActorSet ACT_updateDefault
 *                     (0x00280108).
 *   +0x08 draw        the second hook: MenuModelUnitActorSet writes
 *                     MenuModelAlphaDraw here two instructions after it writes
 *                     `update` (0x00280110), and ACT_init (0x00305b68) and
 *                     ACT_dispose2 (0x00305eb0) clear it beside `update`. No
 *                     call site of it is in main.
 *   +0x0c             not a field: see the note below.
 *   +0x10 position    ACT_info prints +0x10/+0x14/+0x18 under its "     POS
 *                     ROT" heading (0x00306258..0x003062c8); ACT_init writes
 *                     1.0f into the fourth lane (0x00305b80), so the record is a
 *                     homogeneous point; SEQ_moveSPL loads all 16 bytes with one
 *                     lqc2 and GameCfPlayerMove copies them with ld/sd pairs
 *                     (0x00249a34/0x00249a4c).
 *   +0x20 previous_position
 *                     GameCfPlayerMove copies the position into it at the top of
 *                     the frame (0x002494d8..0x002494f8) and subtracts it from
 *                     the position afterwards (0x00249ac0..0x00249ae0).
 *   +0x30 velocity    added to the position once the frame's motion is settled
 *                     (0x00249a54..0x00249a74); ACT_init writes 0 into the
 *                     fourth lane (0x00305b88), so it is a direction.
 *   +0x40 acceleration
 *                     added to the velocity in the same place
 *                     (0x00249a2c..0x00249a44); GameCfPlayerMove seeds its y
 *                     lane with a constant per-frame step (0x002494ec) and zeroes
 *                     the other three; ACT_init writes 0 at +0x4c (0x00305b8c).
 *   +0x50 rotation    ACT_info prints +0x50/+0x54/+0x58 in the "ROT" column,
 *                     dividing each by pi and scaling by 180.0f
 *                     (0x00306274/0x00306278), so they are radians; SEQ_rotateSPL
 *                     writes all three and SEQ_moveSPL writes the y (yaw) one.
 *                     This is the same member src/main/enemy_2.c indexes as
 *                     `rotation[axis]`.
 *   +0x60 scale       ACT_info prints +0x60/+0x64/+0x68 under "     SCL"
 *                     (0x00306308..0x00306338); ACT_init and ACT_create set all
 *                     four lanes to 1.0f (0x00305b70, 0x00305df0).
 *
 * +0x0c is the one word of this head that no instruction in any of the six
 * images reads or writes, and it is the only member here that names no field.
 * It is the gap the quadword-aligned vector block forces after three 4-byte
 * members: `position` is loaded with lqc2 by SEQ_moveSPL below, which requires
 * 16-byte alignment, and the array's base and stride are both 16-byte multiples,
 * so the original's vector type carried that alignment. This tree cannot spell
 * that alignment - AGENTS.md admits no GNU attribute in game code - so the gap
 * is written out as a word instead. It claims no semantics and no name.
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

/* actSequence is an external original witness at 0x0046f460 with witnessed
 * size 0x9800 (38912 bytes, see match.json references). The size annotation
 * only informs addressability; this declaration defines and emits no data.
 * The original materializes it with an absolute lui/addiu pair, never
 * gp-relative, so the size must stay visible instead of an unsized array. */
extern u8 actSequence[0x9800];
extern const float sequence_pi;

extern void SPL_getValueXYZ(float *destination, void *spline, float frame);
extern float xglAtan2(float x, float y);
extern void xglVectorLength(float *destination, const Vector4 *vector);

#endif
