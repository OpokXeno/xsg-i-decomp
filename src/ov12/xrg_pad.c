/*
 * OV12 original TU 92: 0x00a4e488..0x00a4e780 (15 functions)
 */
#include "common.h"
#include "shared.h"

extern int s_nPadID;

/*
 * PadData is a 0xd0-byte table of two 0x68-byte per-pad entries at
 * 0x00490d90; s_nPadID selects the entry, and every accessor below scales
 * it by sizeof(PadDataEntry) == 0x68 (XrgPadIsLR in this same TU computes
 * the identical s_nPadID*3*4*2+0x20 == s_nPadID*0x68+0x20 stride with its
 * own sll/addu sequence). Each entry starts with the canonical PadPrefix
 * (config/header-canon.json PadData decision: `extern PadPrefix PadData;`,
 * 0x2c bytes) and this TU also reads one further halfword at +0x2c that
 * PadPrefix does not model (XrgPadIsUp/IsDown/IsLeft/IsRight), so this
 * entry type extends the prefix instead of aliasing it.
 *
 * PadData is reached through its own symbol, not a bare literal address:
 * XrgPadIsLR (INCLUDE_ASM below, outside this allocation) loads the table
 * with `lui %hi(PadData)` / `addiu %lo(PadData)`, the same relocation this
 * extern produces.
 *
 * Button identity bits are the same PS2 digital-pad values in every
 * halfword of the table: +0x28 (PadPrefix.half_28) tests them as held,
 * +0x2a (PadPrefix.half_2a) as pressed, and +0x2c (direction_buttons,
 * below) as a separate word that only the four d-pad direction predicates
 * of this TU read.
 */
#define PAD_L2      0x0001
#define PAD_R2      0x0002
#define PAD_L1      0x0004
#define PAD_R1      0x0008
#define PAD_SANKAKU 0x0010
#define PAD_MARU    0x0020
#define PAD_BATU    0x0040
#define PAD_SHIKAKU 0x0080
#define PAD_SELECT  0x0100
#define PAD_START   0x0800
#define PAD_UP      0x1000
#define PAD_RIGHT   0x2000
#define PAD_DOWN    0x4000
#define PAD_LEFT    0x8000

typedef struct PadDataEntry {
    PadPrefix prefix;
    u16 direction_buttons;
    u8 unmodeled_2e[0x68 - 0x2e];
} PadDataEntry;

extern PadDataEntry PadData[2];

void XrgPadSetID(int pad_id)
{
    s_nPadID = pad_id;
}

int XrgPadIsUp(void)
{
    return PadData[s_nPadID].direction_buttons & PAD_UP;
}

int XrgPadIsDown(void)
{
    return PadData[s_nPadID].direction_buttons & PAD_DOWN;
}

int XrgPadIsLeft(void)
{
    return PadData[s_nPadID].direction_buttons & PAD_LEFT;
}

int XrgPadIsRight(void)
{
    return PadData[s_nPadID].direction_buttons & PAD_RIGHT;
}

int XrgPadIsMaru(void)
{
    return PadData[s_nPadID].prefix.half_2a & PAD_MARU;
}

int XrgPadIsBatu(void)
{
    return PadData[s_nPadID].prefix.half_2a & PAD_BATU;
}

int XrgPadIsSankaku(void)
{
    return PadData[s_nPadID].prefix.half_2a & PAD_SANKAKU;
}

int XrgPadIsShikaku(void)
{
    return PadData[s_nPadID].prefix.half_2a & PAD_SHIKAKU;
}

int XrgPadIsL12(void)
{
    return PadData[s_nPadID].prefix.half_2a & (PAD_L1 | PAD_L2);
}

int XrgPadIsR12(void)
{
    return PadData[s_nPadID].prefix.half_2a & (PAD_R1 | PAD_R2);
}

int XrgPadIsStart(void)
{
    return PadData[s_nPadID].prefix.half_2a & PAD_START;
}

int XrgPadIsSelect(void)
{
    return PadData[s_nPadID].prefix.half_2a & PAD_SELECT;
}

int XrgPadIsSelectLevelLR(void)
{
    return (PadData[s_nPadID].prefix.half_28 & (PAD_L1 | PAD_R1)) == (PAD_L1 | PAD_R1);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_pad", XrgPadIsLR);
