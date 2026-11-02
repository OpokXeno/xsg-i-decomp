/*
 * TU-local declarations of main/tu092 (src/main/xgl_cd.c).
 */

#ifndef SRC_MAIN_XGL_CD_H
#define SRC_MAIN_XGL_CD_H

#include "shared.h"
#include "main/xgl_cd.h"
#include "main/xgl_hdd.h"

extern signed char ReadClockInterval;
extern unsigned char PresentTime[8];
extern CdStreamParam *StrList;

/* SDK entry points used by this translation unit. */
extern int sceCdGetDiskType(void);
extern int sceCdStatus(void);
extern int sceCdReadDvdDualInfo(int *on_dual);
extern int sceCdBreak(void);
extern int sceCdInit(int mode);
extern int sceCdMmode(int media);
extern int sceCdStStat(void);

extern int sceCdStInit(unsigned int buffer_count, unsigned int bank_count,
                       int buffer);

extern int sceCdStStart(unsigned int lbn, CdReadMode *read_mode);
extern int sceOpen(const char *path, int flags, ...);
extern int sceSifLoadModule(const char *path, int args_length, const char *args);
extern int sceSifLoadElf(const char *path, void *exec_data);
extern int sceSifAllocIopHeap(int bytes);
extern void FlushCache(int mode);

/* Defined later in this file (INCLUDE_ASM); declared here, so its LOCAL
 * definition does not follow a non-static declaration. */
static int xglCdGetFilePos(CdFilePosition *file_position, const char *path,
                           void (*callback)(int event, int value));

extern int sceCdReadClock(unsigned char *clock);
int sceCdStSeek(int descriptor);
int sceLseek(int descriptor, int offset, int whence);
int sceCdStStop(void);
int sceSifFreeIopHeap(void *buffer);
extern int sceCdSync(int mode);
extern int sceCdDiskReady(int mode);
extern int sceCdRead(int lbn, int sectors, void *destination, CdReadMode *read_mode);
static void xglCdArcInit(void);

#endif /* SRC_MAIN_XGL_CD_H */
