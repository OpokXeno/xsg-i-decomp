/*
 * Declarations owned by main/tu097 (src/main/xgl_jpeg.c).
 */

#ifndef SRC_MAIN_XGL_JPEG_H
#define SRC_MAIN_XGL_JPEG_H

#include "shared.h"
#include "main/game_over.h"

/* ASM accepts the request address; callers use several compatible views. */
extern int xglJpegDecode(void *request_address);

#endif /* SRC_MAIN_XGL_JPEG_H */
