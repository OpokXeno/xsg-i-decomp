/*
 * TU-local declarations of ov01/tu029 (src/ov01/m_ef_create_solb.c).
 */

#ifndef SRC_OV01_M_EF_CREATE_SOLB_H
#define SRC_OV01_M_EF_CREATE_SOLB_H

#include "shared.h"

typedef void (*SOLBCallback)(void);

/* The neighboring MSP02 constructor establishes the process, model-draw,
 * draw-packet and post-process callback ordering.  SOLB initializes the
 * process, draw-packet and post-process roles; the intervening work word is
 * outside these bounded role views and is intentionally left unresolved.
 */
typedef struct SOLBProcessSlot {
    SOLBCallback process_callback;
} SOLBProcessSlot;

typedef struct SOLBDrawPacketSlot {
    SOLBCallback draw_packet_callback;
} SOLBDrawPacketSlot;

typedef struct SOLBPostProcessSlot {
    SOLBCallback post_process_callback;
} SOLBPostProcessSlot;

/* The SOLB parameter/trail record, at +32 of the effect work object that
 * MEfCreate_SOLB is handed. Recovered from the TU's own bodies:
 *
 *  - fnSOLB_PR000 (0x00a36f90) takes the record in a1 and, before it appends a
 *    new entry, shifts three parallel histories down by one: u16 at
 *    +0x72..+0xae, u16 at +0xb0..+0xec and qwords at +0x100..+0x2d0 read from
 *    +0xf0..+0x2c0 (thirty copies each). Their bases and extents are exactly
 *    the three regions MEfCreate_SOLB clears (+0x72 and +0xb0 for 62 bytes,
 *    +0xf0 for 496), so each history holds SOLB_TRAIL_LENGTH entries.
 *  - the same function writes active[0] = 1 and birth_age[0] = age, and calls
 *    makeHermiteCoord with &trail[0] as the destination.
 *  - fnSOLB_PO000 (0x00a37a48) increments age once per call, fires
 *    sefHitEffect at 0x17 and MEfObjDestroy at 0x35, which is what makes +0x70
 *    a frame count and not a length.
 *  - makePath (0x00a36cb8) passes +0x08 and +0x0c to MEfGetActorMatrix exactly
 *    as MEfCreate_SOLB passes them to MEfGetActorCoord, and fills
 *    SOLB_CONTROL_POINTS entries at +0x2f0 in a four-iteration loop whose
 *    per-step rotation is the +0x20 angle divided by three.
 *  - fnSOLB_PR000 advances +0x2e2 per frame against timetbl[+0x2e0] and then
 *    steps +0x2e0, and makeHermiteParam (0x00a36ed0) hands +0x330/+0x340 to
 *    MMathCalcHermitePrm as its two outputs over the +0x2f0 points.
 *
 * Spans with no evidenced role keep no members: +0x00, +0x04 (a record whose
 * +0x54 is the yaw makePath applies), +0x10..+0x20 (the local-space point
 * makePath runs through the actor matrix), +0x20 (the sweep angle in degrees),
 * +0x22..+0x70, +0xee and +0x2e4..+0x2f0. The MGs packet record that
 * MEfCreate_SOLB initializes sits past the end of this type, at +0x350.
 */
#define SOLB_TRAIL_LENGTH 31
#define SOLB_CONTROL_POINTS 4

typedef struct SolbState {
    unsigned int : 32;                  /* +0x000 */
    unsigned int : 32;                  /* +0x004 */
    u32 actor;                          /* +0x008 */
    u32 coord;                          /* +0x00c */
    unsigned int : 32;                  /* +0x010 */
    unsigned int : 32;                  /* +0x014 */
    unsigned int : 32;                  /* +0x018 */
    unsigned int : 32;                  /* +0x01c */
    unsigned int : 32;                  /* +0x020 */
    unsigned int : 32;                  /* +0x024 */
    unsigned int : 32;                  /* +0x028 */
    unsigned int : 32;                  /* +0x02c */
    unsigned int : 32;                  /* +0x030 */
    unsigned int : 32;                  /* +0x034 */
    unsigned int : 32;                  /* +0x038 */
    unsigned int : 32;                  /* +0x03c */
    unsigned int : 32;                  /* +0x040 */
    unsigned int : 32;                  /* +0x044 */
    unsigned int : 32;                  /* +0x048 */
    unsigned int : 32;                  /* +0x04c */
    unsigned int : 32;                  /* +0x050 */
    unsigned int : 32;                  /* +0x054 */
    unsigned int : 32;                  /* +0x058 */
    unsigned int : 32;                  /* +0x05c */
    unsigned int : 32;                  /* +0x060 */
    unsigned int : 32;                  /* +0x064 */
    unsigned int : 32;                  /* +0x068 */
    unsigned int : 32;                  /* +0x06c */
    short age;                          /* +0x070: frames since creation */
    short active[SOLB_TRAIL_LENGTH];    /* +0x072 */
    short birth_age[SOLB_TRAIL_LENGTH]; /* +0x0b0: age when the entry was made */
    unsigned short : 16;                /* +0x0ee */
    HermiteVector trail[SOLB_TRAIL_LENGTH];         /* +0x0f0 */
    short segment;                      /* +0x2e0: current path segment */
    short frame;                        /* +0x2e2: frame inside that segment */
    unsigned int : 32;                  /* +0x2e4 */
    unsigned int : 32;                  /* +0x2e8 */
    unsigned int : 32;                  /* +0x2ec */
    HermiteVector control[SOLB_CONTROL_POINTS];     /* +0x2f0 */
    HermiteVector tangent_at_segment_start;         /* +0x330 */
    HermiteVector tangent_at_segment_end;           /* +0x340 */
} SolbState;

int MEfCreate_SOLB(void *work);

extern int timetbl[];

#endif /* SRC_OV01_M_EF_CREATE_SOLB_H */
