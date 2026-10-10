/*
 * TU-local declarations of main/tu092 (src/main/xgl_cd.c).
 */

#ifndef SRC_MAIN_XGL_CD_H
#define SRC_MAIN_XGL_CD_H

#include "shared.h"

extern void xglHddErrorScreen(void);

typedef struct StreamXssBuffer StreamXssBuffer;

typedef struct StreamRing StreamRing;

typedef struct CdStreamParam CdStreamParam;

struct StreamRing {
    unsigned char *buffer;
    int capacity;
    int write_position;
    int read_position;
};

struct StreamXssBuffer {
    unsigned char *source;
    unsigned char *position;
    unsigned char value;
    unsigned char mode;
    short sectors;
};

struct CdStreamParam {
    int iop_buffer;
    unsigned short sector_count;
    unsigned short block_count;
    unsigned char state;
    unsigned char mode;
    /* xss is naturally aligned at 0x0c; no field is asserted at 0x0a. */
    StreamXssBuffer *xss;
    int descriptor;
    int file_byte_count;
    int remaining; /* Bytes not yet transferred into the stream rings. */
    int ring_size;
    StreamRing ring;
    StreamRing alternate_ring;
};

typedef struct CdReadMode {
    unsigned char try_count;
    unsigned char spindle_control;
    unsigned char data_pattern;
} CdReadMode;

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

/* Only the fields xglCdArcInitSub1 writes are evidenced here. */
typedef struct CdArchiveEntry {
    unsigned char state;
    unsigned char unmodeled_01;
    unsigned char packed;
    unsigned char unmodeled_03;
    unsigned char *destination;
    int lbn;
    int file_lbn_offset;
    int stored_bytes;
    int size_bytes;
} CdArchiveEntry;

/* xglCdGetFilePos fills a 0x30-byte file-position record; only its leading
 * lbn word is evidenced by its callers in this TU. */
typedef struct CdFilePosition {
    int lbn;
    int stored_bytes;
    unsigned char unmodeled_08[0x1c];
    unsigned char source;
    unsigned char unmodeled_25;
    unsigned char packed;
    unsigned char unmodeled_27;
    int descriptor;
    int size_bytes;
} CdFilePosition;

/* Defined later in this file (INCLUDE_ASM); declared here, like BCD2INT
 * above, so its LOCAL definition does not follow a non-static declaration. */
static int xglCdGetFilePos(CdFilePosition *file_position, const char *path,
                           void (*callback)(int event, int value));

int xglCdGetFileSize(const char *path);

void xglCdPowerOffCB(void);


extern signed char ReadClockInterval;

extern unsigned char PresentTime[8];

extern int sceCdReadClock(unsigned char *clock);

int xglCdSync(void);

extern CdStreamParam *StrList;

int sceCdStSeek(int descriptor);

int sceLseek(int descriptor, int offset, int whence);

int sceCdStStop(void);

int sceSifFreeIopHeap(void *buffer);

void xglCdStreamReadRingCore(CdStreamParam *stream);

int xglCdStreamReadRing(CdStreamParam *stream, int bytes);

int xglCdStreamRewind(CdStreamParam *stream);

int xglCdStreamClose(CdStreamParam *stream);

void xglCdStreamParamInit(CdStreamParam *param);

extern int sceCdSync(int mode);

extern int sceCdDiskReady(int mode);

extern int sceCdRead(int lbn, int sectors, void *destination, CdReadMode *read_mode);

void xglCdArcCheck(void);


static void xglCdArcInit(void);

#endif /* SRC_MAIN_XGL_CD_H */
