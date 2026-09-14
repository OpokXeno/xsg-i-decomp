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

void xglCdPowerOffCB(void);

extern unsigned char LW[];

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

extern unsigned char ArcHeader[];

extern void xglCdArcInit(void);

#endif /* SRC_MAIN_XGL_CD_H */
