#include "common.h"
#include "near_dir.h"

INCLUDE_ASM("asm/main/nonmatchings/near_dir", nearDir);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_scaleSPL);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_scale);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_rotate);

void SEQ_rotateSPL(Actor *actor)
{
    /* Byte +0x80 is the actor's own slot in the 64-entry `actor` array, the
     * number ACT_create stores there (0x00305de0) and the one ACT_info prints
     * as "ACT[%02x]"; it selects this actor's 0x260-byte actSequence entry.
     * It is past the recovered head of Actor, so it keeps the byte view. */
    u8 *actor_bytes = (u8 *)actor;
    SequenceState *sequence =
        (SequenceState *)(actSequence + actor_bytes[0x80] * 0x260u);
    /* The sequence entry carries several spline tracks; +0xb8 is the rotation
     * track, as +0x38 is the movement track SEQ_moveSPL below walks and
     * Java_xeno_Chr_move fills in (0x002fd398..0x002fd3e4). */
    SplineTrack *track = (SplineTrack *)((u8 *)sequence + 0xb8);
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & 0xeu) == 0) {
        sequence->flags |= 0xeu;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    actor->rotation.x = sample[0] / 180.0f * sequence_pi;
    actor->rotation.y = sample[1] / 180.0f * sequence_pi;
    actor->rotation.z = sample[2] / 180.0f * sequence_pi;

    track->frame++;
    if (track->last_frame < (short)track->frame)
        sequence->state_flags &= ~0xeu;
}

void SEQ_moveSPL(Actor *actor)
{
    /* +0x80 is this actor's slot number and +0x38 the sequence entry's movement
     * track, the one Java_xeno_Chr_move fills in before it installs this
     * routine (0x002fd398..0x002fd3e4). */
    u8 *actor_bytes = (u8 *)actor;
    SequenceState *sequence =
        (SequenceState *)(actSequence + actor_bytes[0x80] * 0x260u);
    SplineTrack *track = (SplineTrack *)((u8 *)sequence + 0x38);
    void *spline = track->spline;
    Vector4 sample;

    if ((sequence->flags & 0x1u) == 0) {
        sequence->flags |= 0x1u;
        track->frame = 0;
    }

    SPL_getValueXYZ(&sample.x, spline, (float)track->frame);

    if ((sequence->state_flags & 0x4u) == 0) {
        if (track->flags & 0x10u) {
            float dx = sample.x - actor->position.x;
            float dz = sample.z - actor->position.z;
            actor->rotation.y = xglAtan2(dx, dz);
        }
    }

    if ((sequence->state_flags & 0x10u) == 0) {
        /* +0x6f0 is the actor's second flags word: ACT_info prints it with the
         * same "FLAGS %08x" it uses for +0x00 (0x00306374) and ACT_pauseUpdate
         * sets and clears bit 0x80000 in it across all 64 entries
         * (0x00305ff0/0x00306060). It is past the recovered head of Actor. */
        u32 *move_flags = (u32 *)(actor_bytes + 0x6f0);
        Vector4 delta;
        float length;

        __asm__ __volatile__(
            "lqc2 $vf3,0(%1)\n\t"
            "lqc2 $vf2,0(%2)\n\t"
            "vsub.xyz $vf2xyz,$vf2xyz,$vf3xyz\n\t"
            "sqc2 $vf2,0(%0)\n\t"
            :
            : "r"(&delta), "r"(&actor->position), "r"(&sample)
            : "memory"
        );

        /* The original computes the distance and never reads it: `length` is
         * written by the call and dead afterwards. The call is the observable
         * effect, not its result. */
        xglVectorLength(&length, &delta);

        /* The original really does test the flag before setting it - the branch
         * is in its bytes - so this is not a redundant read-modify-write. */
        if ((*move_flags & 0x8u) == 0)
            *move_flags |= 0x8u;
    }

    actor->position.x = sample.x;
    actor->position.y = sample.y;
    actor->position.z = sample.z;

    track->frame++;
    if (track->last_frame < (short)track->frame)
        sequence->state_flags &= ~0x1u;
}

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_moveChr);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_setMotion);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_setPositionRAND);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_setRotY2Player);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_moveNPC_XZ);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_moveXZ);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", SEQ_motion);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_updateSequence);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_updateNPC);

/* Defined below in this file; forward-declared for the two callers here. */
extern void ACT_updateMotion(Actor *actor);

/* Undulate.c (main); no header is published for it yet. */
extern float UnduGet(float x, float z);

void ACT_updateDefault(Actor *actor)
{
    if (actor->flags & 0x40)
        actor->position.y = UnduGet(actor->position.x, actor->position.z);
    ACT_updateMotion(actor);
}

void ACT_updateRECTRand(Actor *actor)
{
    ACT_updateMotion(actor);
}

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_updatePlayer);

void ACT_initSequenceAt(Actor *actor)
{
    SequenceState *sequence =
        (SequenceState *)(actSequence + actor->number * 0x260u);

    sequence->flags = 0;
    sequence->state_flags = 0;
    sequence->cleared_on_init = 0;
    sequence->cleared_on_init_run[0] = 0;
    sequence->cleared_on_init_run[1] = 0;
    sequence->cleared_on_init_run[2] = 0;
    sequence->cleared_on_init_run[3] = 0;
    sequence->handler[0] = 0;
    sequence->handler[1] = 0;
    sequence->handler[2] = 0;
    sequence->handler[3] = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_initSequence);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_initVMObject);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_createChr);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_createNPC);

void ACT_info(void)
{
    int i;

    for (i = 0; i < 64; i++) {
    }
}

INCLUDE_ASM("asm/main/nonmatchings/near_dir", getCurrentCamera_0030B6E0);

/* play.c (main); no header is published for it yet. */
extern void *PLAY_getCurrent(void);
/* Defined below in this file. */
extern void SEQ_motion(Actor *actor);
/* Undulate.c (main); no header is published for it yet. */
extern float UnduGet2(void *destination, float x, float z);
/* include/main/xgl_2.h's own declaration: not included directly because it
 * (through include/shared.h) redefines struct Vector4, which this TU's own
 * near_dir.h already defines locally. */
extern void xglMatrixStackUnit(void);

void ACT_updateMPack(Actor *actor)
{
    void *play = PLAY_getCurrent();
    Actor *linked_actor;

    xglMatrixStackUnit();
    SEQ_motion(actor);
    ACT_updateMotion(actor);

    /* PLAY_getCurrent's object (play.c) is not recovered; +0x44 is the one
     * float this TU reads from it, mirrored into motion_time. */
    actor->motion_time = *(float *)((u8 *)play + 0x44);
    actor->undulation = 0;

    linked_actor = actor->linked_actor;
    UnduGet2(&actor->undulation, linked_actor->velocity.x,
             linked_actor->velocity.z);
}

/* Undulate.c (main); no header is published for it yet, and its own
 * UnduWork type is TU-local there, so the destination stays opaque here,
 * exactly as UnduGet2 above already does. */
extern void UnduParamInit(void *undulation);
/* The 64-entry array itself (see the Actor doc comment above); no accepted
 * function in this TU has needed the symbol until now. */
extern u8 actor[64 * 0xa70];

void ACT_initScene(void)
{
    Actor *entry;
    int remaining;

    entry = (Actor *)actor;
    remaining = 63;
    do {
        remaining--;
        entry->flags = 0x20;
        entry->shadow_kind = 1;
        entry->shadow_size = 0x50;
        entry->cleared_on_scene_init = 0;
        entry->state_flags = 0;
        entry->update = 0;
        entry->draw = 0;
        entry->position.w = 1.0f;
        entry->velocity.w = 0.0f;
        entry->acceleration.w = 0.0f;
        UnduParamInit(&entry->undulation);
        entry = (Actor *)((u8 *)entry + 0xa70);
    } while (remaining >= 0);
}

/* Still original asm (0x003083b8, main); ACT_setHand's own call to it is a
 * genuine tail call (j, not jal), so its return value, if any, is never
 * observed here. */
extern void ACT_setHumanHand(Actor *actor, int hand);

void ACT_setHand(Actor *actor, int hand)
{
    int category;
    int code;

    if (actor->state_flags & 0xf000) {
        return;
    }

    category = hand & 0xf0;
    code = hand & 0xf;
    switch (category) {
    case 0x00:
        code |= code << 8;
        break;
    case 0x10:
        code |= 0x8000;
        break;
    case 0x20:
        code = (code << 8) | 0x80;
        break;
    }
    ACT_setHumanHand(actor, code);
}

/* nml_model_set.c (main); no header is published for it yet. */
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetZwrite(int enabled);
extern void nmlModelSetStencil(int enabled);
extern void nmlModelSetFilter(int mode, float filter_param_1, float filter_param_2);
extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetReflTransparency(float transparency);

void ACT_filterGuno(Actor *actor)
{
    nmlModelSetToumei(1);
    nmlModelSetZwrite(1);
    nmlModelSetStencil(1);
    nmlModelSetFilter(1, actor->filter_param_1, actor->filter_param_2);
    nmlModelSetTransparency(actor->transparency);
    nmlModelSetReflTransparency(actor->refl_transparency);
}

void ACT_filterStealth(Actor *actor)
{
    nmlModelSetToumei(1);
    nmlModelSetZwrite(1);
    nmlModelSetStencil(1);
    nmlModelSetFilter(2, actor->filter_param_1, actor->filter_param_2);
    nmlModelSetTransparency(actor->transparency);
    nmlModelSetReflTransparency(actor->refl_transparency);
}
