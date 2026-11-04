/*
 * OV12 original TU 0: 0x00a00000..0x00a00088 (1 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_font.h"
#include "main/xgl_pad.h"


/*
 * NisimoriTest addresses the pad record through the array symbol and reads the
 * held and newly-pressed halfwords at +0x28/+0x2a together as button_state, the
 * shape its compiled code (lui+addiu into a temporary, reused for both tests)
 * needs.
 */
extern XglPadRecord PadData[];

/* The debug font control sequence NisimoriTest passes to xglFontDebugPrintf. */
/* JP: 西森
 * EN: Nishimori */
const char nisimoriTestTitleText[16] = "\x0b\xc0\xbe\xbf\xb9";

/* Waits for the SELECT + START exit combination, drawing the test text each frame. */
void NisimoriTest(void)
{
    if ((PadData[0].button_state & PAD_EXIT_COMBO) != PAD_EXIT_COMBO) {
        do {
            xglFontDebugPrintf(0, 0, nisimoriTestTitleText);
            xglSleep();
        } while ((PadData[0].button_state & PAD_EXIT_COMBO) != PAD_EXIT_COMBO);
    }
}
