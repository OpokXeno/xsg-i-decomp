/*
 * OV12 original TU 0: 0x00a00000..0x00a00088 (1 functions)
 */
#include "common.h"
#include "shared.h"

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

/*
 * PARTIAL ACCESSED PREFIX of PadData (0x00490d90, 0xd0 bytes). NisimoriTest
 * reads the doubleword at +0x28 -- the shared half_28/half_2a pair,
 * extended two halfwords further than the shared PadPrefix view (evidenced
 * only to +0x2c) reaches. src/main/game.h's PadDataDebugLayout keeps the
 * same kind of wider local view of this object (config/header-canon.json
 * "PadData" lists the accepted spellings and why each unit needs its own).
 * NisimoriTest addresses the object through the array symbol, the shape
 * its compiled code (lui+addiu into a temporary, reused for both tests)
 * depends on.
 */
typedef struct PadDataButtonsView {
    unsigned char unmodeled_00[0x28];
    unsigned long long buttons;
} PadDataButtonsView;

extern unsigned char PadData[];

/* This TU's own .rodata (scaffold-owned; kept under its splat name,
 * docs/naming.md "Scaffold-owned data keeps its splat name"): the debug
 * font control sequence NisimoriTest passes to xglFontDebugPrintf. */
extern const char D_00A51430[];

/*
 * Waits for the debug pad combination (0x08000100, held+pressed bits at
 * PadData+0x28/+0x2a) while pumping xglFontDebugPrintf/xglSleep once per
 * frame -- the usual busy-wait pattern for a developer-kit gate.
 */
void NisimoriTest(void)
{
    if ((((PadDataButtonsView *)PadData)->buttons & 0x08000100) != 0x08000100) {
        do {
            xglFontDebugPrintf(0, 0, D_00A51430);
            xglSleep();
        } while ((((PadDataButtonsView *)PadData)->buttons & 0x08000100) != 0x08000100);
    }
}
