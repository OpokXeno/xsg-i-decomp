#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "main/xgl_font.h"
#include "main/xgl_render.h"

typedef struct YosikawaPadInputPrefix {
    u8 unmodeled_00[0x28];
    union {
        u64 snapshot;
        struct {
            u16 held;
            u16 pressed;
            u16 shoulder_buttons;
            u16 unmodeled_2e;
        } buttons;
    } input;
} YosikawaPadInputPrefix;

/* Number of test entries (funclist). */
#define YOSIKAWA_TEST_COUNT 3

extern YosikawaPadInputPrefix PadData;
extern const char yosikawaTestCursorControl[];
extern const char CallJavaMethodYosikawaTestText[];
extern const char CallJavaMethodHairTestLabel[];
extern const char CallJavaMethodWindTestLabel[];
extern const char CallJavaMethodColliTestLabel[];

extern void HairTest(void);
extern void WindTest(void);
extern void ColliTest(void);
static int csr;

static void (*funclist[YOSIKAWA_TEST_COUNT])(void);

static int csr = 0;

/* Text: {reset}{glyph 28} */
const char yosikawaTestCursorControl[8] = "\013\033\034";

static void (*funclist[YOSIKAWA_TEST_COUNT])(void) = { HairTest, WindTest, ColliTest };

/*
 * Debug menu loop: draws the test list, moves the cursor with the shoulder buttons, and runs the
 * selected test when Circle is pressed. Returns after that test or when the exit combo is held.
 */
void YosikawaTest(void)
{
    int selection;
    int wrapped_selection;
    int loop_cursor;
    u32 shoulder_buttons;

    xglRenderClearFrame();
    xglSleep();

    while ((PadData.input.snapshot & PAD_EXIT_COMBO) != PAD_EXIT_COMBO) {
        shoulder_buttons = PadData.input.buttons.shoulder_buttons;
        if (shoulder_buttons & PAD_DOWN) {
            selection = csr;
            selection++;
            csr = selection;
        } else {
            selection = csr;
        }
        if (shoulder_buttons & PAD_UP) {
            selection--;
            csr = selection;
        }

        /* Wrap a negative cursor into the test range. */
        loop_cursor = selection;
        if (selection < 0) {
            do {
                wrapped_selection = loop_cursor + YOSIKAWA_TEST_COUNT;
                loop_cursor = wrapped_selection;
            } while (wrapped_selection < 0);
            csr = wrapped_selection;
            selection = wrapped_selection;
        }

        /* Wrap a cursor past the last test back to the first. */
        {
            int function_count = YOSIKAWA_TEST_COUNT;

            if (selection >= function_count) {
                do {
                    wrapped_selection = selection - function_count;
                    selection = wrapped_selection;
                } while (wrapped_selection >= function_count);
                csr = wrapped_selection;
            }
        }

        if (PadData.input.buttons.pressed & PAD_CIRCLE) {
            funclist[csr]();
            return;
        }

        xglFontDebugPrintf(0, 0, CallJavaMethodYosikawaTestText);
        xglFontDebugPrintf(16, 16, CallJavaMethodHairTestLabel);
        xglFontDebugPrintf(16, 24, CallJavaMethodWindTestLabel);
        xglFontDebugPrintf(16, 32, CallJavaMethodColliTestLabel);
        xglFontDebugPrintf(8, (csr * 8) + 16, yosikawaTestCursorControl);
        xglSleep();
    }
}
