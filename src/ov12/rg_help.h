/*
 * TU-local declarations of ov12/tu076 (src/ov12/rg_help.c).
 */

#ifndef SRC_OV12_RG_HELP_H
#define SRC_OV12_RG_HELP_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/rg_help.h"

/* A catalog entry has a 64-byte archive name followed by its loaded archive. */
typedef struct RgHelpArchiveEntry {
    char archiveName[0x40];
    RgBxx *archive;
} RgHelpArchiveEntry;

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
    RgHelpArchiveEntry archives[20];
    int bxxCount;
    RgBxxPic *pics[62];
    XrgPaint2D *paintContext;
    int phase;
    int cursor;
    float blinkTimer;
    float countdown;
};

#endif /* SRC_OV12_RG_HELP_H */
