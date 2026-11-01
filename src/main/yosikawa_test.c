#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"

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

extern YosikawaPadInputPrefix PadData;
extern const char yosikawaTestCursorControl[];
extern const char CallJavaMethodYosikawaTestText[];
extern const char CallJavaMethodHairTestLabel[];
extern const char CallJavaMethodWindTestLabel[];
extern const char CallJavaMethodColliTestLabel[];

extern void HairTest(void);
extern void WindTest(void);
extern void ColliTest(void);
extern void xglRenderClearFrame(void);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

static int csr;

static void (*funclist[3])(void);

static int csr = 0;

/* Text: {reset}{glyph 28} */
const char yosikawaTestCursorControl[8] = "\013\033\034";

static void (*funclist[3])(void) = { HairTest, WindTest, ColliTest };

void YosikawaTest(void)
{
    int selection;
    int wrapped_selection;
    int loop_cursor;
    u32 shoulder_buttons;

    xglRenderClearFrame();
    xglSleep();

    while ((PadData.input.snapshot & 0x08000100ULL) != 0x08000100ULL) {
        shoulder_buttons = PadData.input.buttons.shoulder_buttons;
        if (shoulder_buttons & 0x4000) {
            selection = csr;
            selection++;
            csr = selection;
        } else {
            selection = csr;
        }
        if (shoulder_buttons & 0x1000) {
            selection--;
            csr = selection;
        }

        loop_cursor = selection;
        if (selection < 0) {
            do {
                wrapped_selection = loop_cursor + 3;
                loop_cursor = wrapped_selection;
            } while (wrapped_selection < 0);
            csr = wrapped_selection;
            selection = wrapped_selection;
        }

        {
            int function_count = 3;

            if (selection >= function_count) {
                do {
                    wrapped_selection = selection - function_count;
                    selection = wrapped_selection;
                } while (wrapped_selection >= function_count);
                csr = wrapped_selection;
            }
        }

        if (PadData.input.buttons.pressed & 0x0020) {
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
