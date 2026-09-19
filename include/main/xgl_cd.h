#ifndef INCLUDE_MAIN_XGL_CD_H
#define INCLUDE_MAIN_XGL_CD_H

#include "shared.h"

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);

void xglClockRead(XglClock *clock);

#endif /* INCLUDE_MAIN_XGL_CD_H */
