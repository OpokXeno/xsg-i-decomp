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

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_updateDefault);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_updateRECTRand);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_updatePlayer);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_initSequenceAt);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_initSequence);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_initVMObject);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_createChr);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_createNPC);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_info_0030B6B0);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", getCurrentCamera_0030B6E0);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_updateMPack);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_initScene);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_setHand);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_filterGuno);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", ACT_filterStealth);
