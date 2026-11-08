#include "common.h"
#include "main/xgl_thread.h"
#include "main/xgl_render.h"

/* Logging stub: the format and arguments are ignored. */
void LOG(const char *format, ...)
{
}

/* Clears the render frame, then sleeps forever. */
void ItaiTest(void)
{
    xglRenderClearFrame();
    for (;;) {
        xglSleep();
    }
}
