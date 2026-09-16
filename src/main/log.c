#include "common.h"
#include "main/xgl_thread.h"

extern void xglRenderClearFrame(void);

void LOG(const char *format, ...)
{
}

void ItaiTest(void)
{
    xglRenderClearFrame();
    for (;;) {
        xglSleep();
    }
}
