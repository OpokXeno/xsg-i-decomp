#ifndef INCLUDE_MAIN_XGL_CD_H
#define INCLUDE_MAIN_XGL_CD_H

#include "shared.h"

typedef void CdCompletionCallback(int event, int value);

/*
 * Converts a calendar record to the number of seconds elapsed since
 * 2000-01-01 00:00:00.  monthday[12..23] holds the cumulative day offset of
 * each month; the leap day of the current year counts from March onwards,
 * and ((year - 1997) >> 2) counts the leap days of the whole years before it.
 */
unsigned int xglClockDayTime2UInt(XglClock *clock_time);

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
void xglClockRead(XglClock *clock);
extern int xglCdGetFileSize(const char *name);

extern int xglCdReadFilePart(const char *name, void *buffer, int mode,
                             CdCompletionCallback *callback, int offset, int length);

#endif /* INCLUDE_MAIN_XGL_CD_H */
