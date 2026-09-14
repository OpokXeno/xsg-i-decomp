/*
 * OV01 original TU 29: 0x00a36be8..0x00a37ab8 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/m_math.h"
#include "m_ef_create_solb.h"

extern void *memset(void *destination, int value, unsigned int count);
extern void MEfGetActorCoord(void *dst, u32 actor, u32 coord);
extern void makePath(void *work);
extern void makeHermiteParam(int mode, void *work);
extern void MGsGPInit(void *work, int flags, int mode);
extern void fnSOLB_PR000(void);
extern void fnSOLB_DP000(void);
extern void fnSOLB_PO000(void);

int MEfCreate_SOLB(void *work)
{
    unsigned char *base = (unsigned char *)work;
    SolbState *state = (SolbState *)(base + 32);

    state->age = 0;
    memset(state->active, 0, sizeof(state->active));
    memset(state->birth_age, 0, sizeof(state->birth_age));
    memset(state->trail, 0, sizeof(state->trail));
    MEfGetActorCoord(state->trail, state->actor, state->coord);
    state->segment = 0;
    state->frame = 0;
    makePath(state);
    makeHermiteParam(0, state);
    /* The MGs packet record follows the SOLB state, at state + 0x350. */
    MGsGPInit(base + 880, 0, 0);
    ((SOLBProcessSlot *)(base + 4))->process_callback = fnSOLB_PR000;
    ((SOLBDrawPacketSlot *)(base + 12))->draw_packet_callback = fnSOLB_DP000;
    ((SOLBPostProcessSlot *)(base + 16))->post_process_callback = fnSOLB_PO000;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_solb", makePath_00A36CB8);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_solb", makeHermiteParam);

/* makeHermiteCoord: corrected tangent/endpoint attribution.
 *
 * Proof of the corrected names from the actual helper bodies:
 * - MMathCalcHermitePrm (main 0x002ef1d0) takes its two outputs in a0/a1 and
 *   stores (vf3-vf1)*k to (a0) and (vf4-vf2)*k to (a1). Its caller
 *   makeHermiteParams passes effect+0x330 in a0 and effect+0x340 in a1, so
 *   +0x330/+0x340 hold the computed segment TANGENTS, not endpoints.
 * - MMathCalcHermite (main 0x002ef160) loads its middle operand vectors from
 *   (a1)/(a2) and weights them with the parameter-derived tangent
 *   coefficients, while the (a3)/(t0) vectors take the endpoint weights; the
 *   caller passes +0x330/+0x340 in a1/a2 and the segment-indexed +0x2f0/+0x300
 *   pair in a3/t0. Hence a1/a2 are tangent_at_segment_start/end and a3/t0
 *   are control_at_segment_start/end. Start/end ordering within each pair is
 *   unchanged from the prior exact form; only the category names and this
 *   comment are corrected. Codegen is unchanged (LOCAL binding preserved).
 *
 * Those four addresses are now the SolbState members tangent_at_segment_start,
 * tangent_at_segment_end and control[segment]/control[segment + 1]; the effect
 * this is called with is the same record MEfCreate_SOLB sets up, which
 * fnSOLB_PR000 (0x00a37010) proves by passing it in a1 to both.
 */

static void makeHermiteCoord(float *destination, void *effect)
{
    SolbState *state = (SolbState *)effect;
    short segment_index = state->segment;
    short frame_index = state->frame;

    MMathCalcHermite(destination, (float)frame_index / (float)timetbl[segment_index],
                     &state->tangent_at_segment_start,
                     &state->tangent_at_segment_end,
                     &state->control[segment_index],
                     &state->control[segment_index + 1]);
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_solb", fnSOLB_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_solb", fnSOLB_DP000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_solb", fnSOLB_PO000);
