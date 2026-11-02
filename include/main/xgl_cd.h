#ifndef INCLUDE_MAIN_XGL_CD_H
#define INCLUDE_MAIN_XGL_CD_H

#include "shared.h"

typedef void CdCompletionCallback(int event, int value);

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
    short sector_count;
    short block_count;
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
    unsigned int lbn;
    int stored_bytes;
    unsigned char unmodeled_08[0x1c];
    unsigned char source;
    unsigned char unmodeled_25;
    unsigned char packed;
    unsigned char unmodeled_27;
    int descriptor;
    int size_bytes;
} CdFilePosition;

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

/* Functions and data used by other translation units. */
void xglCdPowerOffCB(void);
void xglCdInitial(void);
int xglCdSync(void);
void xglCdReset(void);
void xglCdArcCheck(void);
void xglClockUInt2DayTime(XglClock *clock_time, unsigned int elapsed);
int xglCdStreamClose(CdStreamParam *stream);
void xglCdStreamParamInit(CdStreamParam *param);
void xglCdStreamReadRingCore(CdStreamParam *stream);
int xglCdStreamReadRing(CdStreamParam *stream, int bytes);
int xglCdStreamOpen(CdStreamParam *stream, const char *path);
int xglCdStreamRewind(CdStreamParam *stream);

#endif /* INCLUDE_MAIN_XGL_CD_H */
