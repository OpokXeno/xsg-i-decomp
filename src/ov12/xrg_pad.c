/*
 * OV12 original TU 92: 0x00a4e488..0x00a4e780 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/xrg_pad.h"

static int s_nPadID = 0;

/*
 * Partial view of XglPadRecord (include/main/xgl_pad.h), 0x68 bytes per pad
 * (measured: with the canonical record's 8-byte alignment XrgPadIsLR no longer
 * folds its +0x20 base offset into one address, so the original's 2-byte
 * aligned prefix is kept).
 */
typedef struct XglPadRecordView {
    u8 unmodeled_00[0x28];
    u16 buttons;
    u16 pressed;
    u16 repeated;
    u8 unmodeled_2e[0x68 - 0x2e];
} XglPadRecordView;

/* The per-pad records of the controller table, indexed by s_nPadID. */
extern XglPadRecordView PadData[2];

void XrgPadSetID(int pad_id)
{
    s_nPadID = pad_id;
}

int XrgPadIsUp(void)
{
    return PadData[s_nPadID].repeated & PAD_UP;
}

int XrgPadIsDown(void)
{
    return PadData[s_nPadID].repeated & PAD_DOWN;
}

int XrgPadIsLeft(void)
{
    return PadData[s_nPadID].repeated & PAD_LEFT;
}

int XrgPadIsRight(void)
{
    return PadData[s_nPadID].repeated & PAD_RIGHT;
}

int XrgPadIsMaru(void)
{
    return PadData[s_nPadID].pressed & PAD_CIRCLE;
}

int XrgPadIsBatu(void)
{
    return PadData[s_nPadID].pressed & PAD_CROSS;
}

int XrgPadIsSankaku(void)
{
    return PadData[s_nPadID].pressed & PAD_TRIANGLE;
}

int XrgPadIsShikaku(void)
{
    return PadData[s_nPadID].pressed & PAD_SQUARE;
}

int XrgPadIsL12(void)
{
    return PadData[s_nPadID].pressed & (PAD_L1 | PAD_L2);
}

int XrgPadIsR12(void)
{
    return PadData[s_nPadID].pressed & (PAD_R1 | PAD_R2);
}

int XrgPadIsStart(void)
{
    return PadData[s_nPadID].pressed & PAD_START;
}

int XrgPadIsSelect(void)
{
    return PadData[s_nPadID].pressed & PAD_SELECT;
}

/* Returns 1 while L1 and R1 are both held. */
int XrgPadIsSelectLevelLR(void)
{
    return (PadData[s_nPadID].buttons & (PAD_L1 | PAD_R1)) == (PAD_L1 | PAD_R1);
}

/* Returns 1 when L1 was pressed while R1 is held, or R1 was pressed while L1 is held. */
int XrgPadIsLR(void)
{
    u16 pressed;
    u16 held;

    pressed = PadData[s_nPadID].pressed;
    held = PadData[s_nPadID].buttons;
    if ((pressed & PAD_L1) != 0 && (held & PAD_R1) != 0) {
        return 1;
    }
    if ((held & PAD_L1) != 0 && (pressed & PAD_R1) != 0) {
        return 1;
    }
    return 0;
}
