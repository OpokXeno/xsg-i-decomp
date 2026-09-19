/*
 * TU-local declarations of ov12/tu076 (src/ov12/rg_help.c).
 */

#ifndef SRC_OV12_RG_HELP_H
#define SRC_OV12_RG_HELP_H

#include "shared.h"

/*
 * Partial view of the picture object RgBxxGetPic returns for one of the help
 * screen's catalog entries. _InitHelp (ov12:0x00a42130, outside this
 * allocation) resolves each catalog entry through LoadRgBxx_sub/RgBxxGetPic
 * (ov12:0x00a42204/0x00a4229c) and stores the result in RgHelp::pics. The
 * only member any function claimed here reads is the height at +0x38:
 * _paint_bg_lower (ov12:0x00a42b90) draws that picture at y = 464 - height,
 * so its bottom edge sits on screen line 464. The 96-byte extent is
 * rg_bxx.c's own assert "sizeof(RgBxxPic) == 96" (OV12 .rodata); rg_bxx
 * (ov12/tu073) owns this type, and this view must stay identical to its
 * definition there.
 */
typedef struct RgBxxPic {
    unsigned char unmodeled_000[0x38];
    int height;
    unsigned char unmodeled_03C[0x24];
} RgBxxPic;

typedef struct RgHelp RgHelp;

/*
 * _InitHelp (ov12:0x00a42130, outside this allocation) builds this object in
 * the storage CreateRgHelp allocates at its exact size, 0x664 bytes:
 *
 *   +0x000 ended        RgHelpIsEnd's return value; RgHelpPassTime sets it
 *                        once `phase` below reaches 2.
 *   +0x004..+0x557       _InitHelp's own catalog of loaded Bxx files and
 *                        their names; no function claimed here reads or
 *                        writes this span.
 *   +0x558..+0x64f pics  one RgBxxPic per catalog entry _InitHelp resolves
 *                        with RgBxxGetPic. _paint_bg_lower reads pics[60]
 *                        and pics[61] (+0x648/+0x64c) and _paint_bg_higher
 *                        reads pics[46] (+0x610).
 *   +0x650 paintContext  CreateXrgPaint2D_sub's return, stored by _InitHelp
 *                        (ov12:0x00a4219c). RgHelpDisp hands it to
 *                        XrgPaint2DFlush and the paint helpers pass it on as
 *                        their own paint-context argument.
 *   +0x654 phase         the close sequence RgHelpPassTime drives: 0 waits
 *                        for the player to press Start/Batu, 1 counts
 *                        `countdown` down to zero, 2 is the closed phase
 *                        that sets `ended`.
 *   +0x658 cursor        the left-panel selection index _control_mode_left
 *                        and _paint_mode_left use (outside this
 *                        allocation); _init_mode_left resets it to zero.
 *   +0x65c blinkTimer    seconds accumulated every RgHelpPassTime call for
 *                        the left panel's cursor blink, reset past 2.0
 *                        seconds by _control_mode_left (outside this
 *                        allocation).
 *   +0x660 countdown     seconds left before phase 2; RgHelpPassTime seeds
 *                        it at 0.5 and counts it down while phase is 1.
 */
struct RgHelp {
    int ended;
    unsigned char unmodeled_004[0x554];
    RgBxxPic *pics[62];
    void *paintContext;
    int phase;
    int cursor;
    float blinkTimer;
    float countdown;
};

RgHelp *CreateRgHelp(void);
void DisposeRgHelp(RgHelp *pHelp);
int RgHelpIsEnd(RgHelp *pHelp);
void RgHelpPassTime(RgHelp *pHelp, float deltaTime);
void RgHelpDisp(RgHelp *pHelp);

#endif /* SRC_OV12_RG_HELP_H */
