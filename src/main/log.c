#include "common.h"
#include "main/xgl_thread.h"
#include "main/xgl_render.h"

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
