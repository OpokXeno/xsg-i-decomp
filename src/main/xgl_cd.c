#include "common.h"

#include "shared.h"

#include "main/xgl_cd.h"

#include "main/xgl_thread.h"

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
static CdFilePosition *xglCdGetFilePos(CdFilePosition *file_position, const char *path,
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



/*
 * File-local functions (LOCAL in the original symbol table) that the shared
 * header declares `extern`: declare them static first so their static
 * definitions below do not follow a non-static declaration.
 */

static int BCD2INT(unsigned char x);

/* extract (below) is still INCLUDE_ASM; declare it so its callers do not see
 * an implicit declaration. */

static void extract(void *destination, void *source, int byte_count);

extern int sceCdStRead(int sectors, void *buffer, int mode, int *result);

extern int sceRead(int descriptor, void *buffer, int bytes);

extern const char D_004DC2D0[];

extern const char D_004DC2D8[];

extern const char D_004DC2E0[];

extern const char D_004D2498[];
extern const char D_004DC2F8[];
extern const char D_004DC300[];
extern const char D_004DC308[];
extern const char D_004DC310[];

/* StrList holds a single global stream slot; queue_next's stream scan below
 * walks it as a one-entry list, as xglCdStreamClose already does. */

#define STR_LIST_LAST_INDEX 0

/* Keep the direct state expression used by both queue loops. */

#define CD_READ_CONTROL (&LW)

/* Twelve month lengths followed by twelve cumulative month offsets. */

static unsigned short monthday[24] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
};

unsigned char system_cnf[80] = {0};

static int StreamReadRingCoreSub(CdStreamParam *stream, void *buffer, int sectors);

static void StreamReadRingCoreNormal(CdStreamParam *stream);

static void StreamReadRingCoreXss(CdStreamParam *stream);

typedef void CdCompletionCallback(int event, int value);

/*
 * The queue entries occupy 0x80 bytes; only their used prefix is named.
 * xglCdReadFilePart (0x0021DEB8..0x0021E30C, still INCLUDE_ASM) enqueues by
 * storing its own second argument (buffer, a single `sw` at 0x0021DFB4) at
 * entry+0x00 and strcpy'ing its own first argument (name, the lbu/sb loop at
 * 0x0021DF90..0x0021DFA8) into entry+0x10; queue_next below hands those two
 * fields back to it in that same order, matching xglCdReadFile's
 * (name, buffer, ...) shape in shared.h.
 */

typedef struct CdRequestPartial {
    void *buffer;
    int offset;
    int length;
    CdCompletionCallback *completion_callback;
    char name[0x70];
} CdRequestPartial;

/*
 * Partial view of LW, limited to fields evidenced by this TU.
 * unmodeled_XX spans are opaque bytes between evidenced fields; naming them
 * `unmodeled_<offset>[<size>]` follows the convention used across the
 * project (e.g. src/main/game.c, src/main/ssd_init.h) instead of inventing
 * members to complete the struct's size.
 */

typedef struct CdReadControl {
    int active_file_descriptor;                 /* +0x00 */
    u8 *write_cursor;                            /* +0x04 */
    int remaining_bytes;                         /* +0x08 */
    int read_poll_mode;                          /* +0x0c */
    CdCompletionCallback *completion_callback;   /* +0x10 */
    u8 unmodeled_014[8];                         /* +0x14..0x1b */
    u8 disk_state;                               /* +0x1c */
    u8 disk_flags;                               /* +0x1d */
    u8 unmodeled_01e[2];                         /* +0x1e..0x1f */
    u8 *extract_source;                          /* +0x20 */
    u8 *extract_destination;                     /* +0x24 */
    int extract_byte_count;                      /* +0x28 */
    u8 unmodeled_02c[0x4];                       /* +0x2c..0x2f */
    signed char dispatch_state;                  /* +0x30 */
    u8 spindle_control;                          /* +0x31 */
    u8 search_layer;                             /* +0x32 */
    u8 dual_layer_disc;                          /* +0x33 */
    u8 power_off_pending;                        /* +0x34 */
    u8 unmodeled_035[0x9];                       /* +0x35..0x3d */
    u8 queue_cursor;                             /* +0x3e */
    u8 queue_tail;                               /* +0x3f */
    CdRequestPartial requests[32];               /* +0x40..0x103f */
} CdReadControl;

static CdReadControl LW;

/* Four 0x18-byte archive slots; the initialization helper models each prefix. */

typedef union CdArchiveStorage {
    CdArchiveEntry entry;
    unsigned char bytes[0x18];
} CdArchiveStorage;

static CdArchiveStorage ArcHeader[4];

static signed char ReadClockInterval;

static unsigned char PresentTime[8];

static CdStreamParam *StrList;

extern int xglCdReadFilePart(const char *name, void *buffer, int mode,
                             CdCompletionCallback *callback, int offset, int length);

extern int sceDevctl(const char *device, int command, const void *input,
                     unsigned int input_size, void *output, unsigned int output_size);

extern int sceCdPause(void);

extern int sceCdPowerOff(u32 *result);

extern u32 sceCdGetReadPos(void);

/*
 * Converts a calendar record to the number of seconds elapsed since
 * 2000-01-01 00:00:00.  monthday[12..23] holds the cumulative day offset of
 * each month; the leap day of the current year counts from March onwards,
 * and ((year - 1997) >> 2) counts the leap days of the whole years before it.
 */

static CdCompletionCallback *callback;

/* A CD sector is fixed at 2048 bytes on this drive/read path (see also
 * xglCdStreamReadRing's own `& -2048` sector rounding earlier in this TU). */

#define CD_SECTOR_BYTES 2048

/*
 * Archive table-of-contents walk: archive_data is a header buffer filled by
 * xglCdArcInitSub1/xglCdArcInit, and this finds the byte right after the
 * table's last record so the caller can place fresh data there.
 *
 * Each record begins with a one-byte length/flags field:
 *   - ARC_RECORD_EXTENDED set: no fixed header past this byte.
 *   - ARC_RECORD_EXTENDED clear: a fixed ARC_RECORD_HEADER_BYTES-byte header
 *     follows, extended by ARC_RECORD_TYPE_EXTRA_BYTES more bytes when
 *     ARC_RECORD_TYPE_EXTENDED is also set.
 *   - ARC_RECORD_LENGTH_MASK (the low bits) is the record's payload length,
 *     walked past after the fixed header, if any.
 * A zero length/flags byte ends the table.
 */

#define ARC_RECORD_EXTENDED        0x80

#define ARC_RECORD_TYPE_EXTENDED   0x40

#define ARC_RECORD_LENGTH_MASK     0x3f

#define ARC_RECORD_HEADER_BYTES    7

#define ARC_RECORD_TYPE_EXTRA_BYTES 3

#define ARC_TABLE_ALIGN_BYTES      64

#include "main/xgl_hdd.h"

/*
 * File-local functions (LOCAL in the original symbol table) that the shared
 * header declares `extern`: declare them static first so their static
 * definitions below do not follow a non-static declaration.
 */

/* extract (below) is still INCLUDE_ASM; declare it so its callers do not see
 * an implicit declaration. */

extern int sceCdStatus(void);

extern int sceOpen(const char *path, int flags, ...);

extern int sceCdBreak(void);

extern int sceSifAllocIopHeap(int size);

extern int sceCdStStat(void);

extern int sceCdInit(int mode);

extern int sceCdMmode(int media);

extern int sceCdReadDvdDualInfo(int *on_dual);

extern void FlushCache(int mode);

extern int sceSifLoadElf(const char *path, void *exec_data);

extern int sceSifLoadModule(const char *path, int args_length, const char *args);

extern int sceCdGetDiskType(void);

/* StrList holds a single global stream slot; queue_next's stream scan below
 * walks it as a one-entry list, as xglCdStreamClose already does. */

/* Keep the direct state expression used by both queue loops. */

/* Twelve month lengths followed by twelve cumulative month offsets. */

const char D_004D2468[16] = "xenosaga.10";

const char D_004D2478[16] = "xenosaga.20";

const char D_004D2488[16] = "xenosaga.30";

const char D_004D24D0[16] = "system.cnf";

static char listname_7[] = ".filelist";

static char head_8[] = "cdrom0:\\IOP\\";

static char head_10[] = "cdrom0:\\";

typedef struct CdFileSelect {
    int x;                                         /* +0x000 */
    int y;                                         /* +0x004 */
    int rows;                                      /* +0x008 */
    int state;                                     /* +0x00c */
    char directory[0x100];                         /* +0x010 */
    char *list;                                    /* +0x110 */
    unsigned int list_size;                        /* +0x114 */
    void (*print)(int x, int y, const char *format, ...); /* +0x118 */
    char path[0x100];                              /* +0x11c */
    unsigned short entry_count;                    /* +0x21c */
    short first_row;                               /* +0x21e */
    short cursor;                                  /* +0x220 */
    unsigned char unmodeled_222[2];                /* +0x222 */
    int unmodeled_224;                             /* +0x224 */
    char *list_position;                           /* +0x228 */
    unsigned char unmodeled_22c[0x268 - 0x22c];    /* +0x22c */
} CdFileSelect;

/* Four 0x18-byte archive slots; the initialization helper models each prefix. */

extern unsigned char loaded_overlay;

static char tail_9[] = ".IRX;1";

static char tail_11[] = ".OVL;1";

extern int sceCdGetError(void);

extern int sceCdLayerSearchFile(CdFilePosition *file, const char *name, int layer);

extern int xglHddCheck(void);

extern const char D_004D24A8[];

extern const char D_004D24C0[];

static char arcname_1[] = "xenosaga.00";

static char arcname_2[] = "xenosaga.10";

static char arcname_3[] = "xenosaga.20";

static char arcname_4[] = "xenosaga.30";

static char vcd_head_5[] = "host0:/home/xeno/vcd-u/";

static char hdd_head_6[] = "pfs0:/";

static char root_0[] = "data\\";

#define CD_PACKED_MAGIC 0x585241

typedef struct CdPackedHeader {
    int magic;
    int size_bytes;
    int unmodeled_08;
    int mode;
} CdPackedHeader;

static void hdd_error(void)
{
    int *descriptor = &LW.active_file_descriptor;

    sceClose(*descriptor);
    *descriptor = -1;
    xglHddErrorScreen();
}

void xglCdPowerOffCB(void)
{
    LW.power_off_pending = 1;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", extract);

static int queue_next(void)
{
    CdRequestPartial *request;

    CD_READ_CONTROL->completion_callback(4,
                                         (CD_READ_CONTROL->queue_tail
                                          - CD_READ_CONTROL->queue_cursor) & 0x1f);
    CD_READ_CONTROL->dispatch_state = 0;
    if (CD_READ_CONTROL->queue_tail != CD_READ_CONTROL->queue_cursor) {
        request = &CD_READ_CONTROL->requests[CD_READ_CONTROL->queue_cursor];
        xglCdReadFilePart(request->name, request->buffer, 1,
                          request->completion_callback, request->offset,
                          request->length);
        CD_READ_CONTROL->queue_cursor = (CD_READ_CONTROL->queue_cursor + 1) & 0x1f;
        return 0;
    }
    return 1;
}

void xglCdControlThread(void)
{
    u32 power_status;

    for (;;) {
        int requested_bytes;
        int transferred;

        xglSleep();
        if (ReadClockInterval > 0)
            ReadClockInterval--;

        if (CD_READ_CONTROL->power_off_pending != 0) {
            sceDevctl(D_004DC2D0, 0x5003, 0, 0, 0, 0);
            sceDevctl(D_004DC2D8, 0x5003, 0, 0, 0, 0);
            sceDevctl(D_004DC2E0, 0x4806, 0, 0, 0, 0);
            sceCdPowerOff(&power_status);
        }

        switch (CD_READ_CONTROL->dispatch_state) {
        case 2:
            switch (CD_READ_CONTROL->read_poll_mode) {
            case 0:
                CD_READ_CONTROL->completion_callback(3, sceCdGetReadPos());
                if (sceCdSync(1) != 0)
                    continue;
                extract(CD_READ_CONTROL->extract_destination,
                        CD_READ_CONTROL->extract_source,
                        CD_READ_CONTROL->extract_byte_count);
                if (queue_next() == 0)
                    continue;
                sceCdPause();
                continue;
            case 1:
            case 2:
                requested_bytes = CD_READ_CONTROL->remaining_bytes;
                if (requested_bytes > 0x10000)
                    requested_bytes = 0x10000;
                transferred = sceRead(CD_READ_CONTROL->active_file_descriptor,
                                      CD_READ_CONTROL->write_cursor,
                                      requested_bytes);
                if (transferred < 0) {
                    hdd_error();
                    CD_READ_CONTROL->queue_cursor =
                        (CD_READ_CONTROL->queue_cursor + 31) & 31;
                    continue;
                }

                CD_READ_CONTROL->write_cursor += transferred;
                CD_READ_CONTROL->remaining_bytes -= transferred;
                CD_READ_CONTROL->completion_callback(
                    3, sceLseek(CD_READ_CONTROL->active_file_descriptor, 0, 1));
                if (CD_READ_CONTROL->remaining_bytes > 0)
                    continue;

                {
                    int partial_sector = transferred & 0x7ff;
                    if (partial_sector != 0) {
                        int remaining_padding = (-partial_sector) & 0x7ff;
                        while (remaining_padding > 0) {
                            *CD_READ_CONTROL->write_cursor++ =
                                (unsigned char)remaining_padding;
                            remaining_padding--;
                        }
                    }
                }
                sceClose(CD_READ_CONTROL->active_file_descriptor);
                CD_READ_CONTROL->active_file_descriptor = -1;
                extract(CD_READ_CONTROL->extract_destination,
                        CD_READ_CONTROL->extract_source,
                        CD_READ_CONTROL->extract_byte_count);
                queue_next();
                break;
            default:
                continue;
            }
            break;
        case 3: {
            CdStreamParam **stream_cursor;
            int stream_index;
            for (stream_cursor = &StrList, stream_index = 0;
                 stream_index <= STR_LIST_LAST_INDEX;
                 stream_index++, stream_cursor++) {
                if (*stream_cursor != 0)
                    xglCdStreamReadRingCore(*stream_cursor);
            }
            break;
        }
        case 0:
        case 1:
            break;
        default:
            CD_READ_CONTROL->dispatch_state = 0;
        }
    }
}

static int BCD2INT(unsigned char x)
{
    return (x >> 4) * 10 + (x & 15);
}

void xglClockRead(XglClock *clock)
{
    if (ReadClockInterval == 0) {
        ReadClockInterval = 10;
        sceCdReadClock(PresentTime);
    }

    clock->year = BCD2INT(PresentTime[7]) + 2000;
    clock->month = BCD2INT(PresentTime[6]);
    clock->day = BCD2INT(PresentTime[5]);
    clock->hour = BCD2INT(PresentTime[3]);
    clock->minute = BCD2INT(PresentTime[2]);
    clock->second = BCD2INT(PresentTime[1]);
    clock->status = 0;
}

unsigned int xglClockDayTime2UInt(XglClock *clock_time)
{
    unsigned int seconds;
    unsigned int year;
    unsigned char month;

    month = clock_time->month;
    seconds = (monthday[month + 11] + clock_time->day - 1) * 86400
        + clock_time->hour * 3600 + clock_time->minute * 60
        + clock_time->second;
    year = clock_time->year;
    if (((year - 2000) & 3) == 0 && month >= 3) {
        seconds += 86400;
    }
    return seconds
        + ((year - 2000) * 31536000 + ((year - 1997) >> 2) * 86400);
}

void xglClockUInt2DayTime(XglClock *clock_time, unsigned int elapsed)
{
    XglClock *clock;
    unsigned int year_length = 366;
    unsigned int year = 0;
    unsigned int common_year_length = 365;
    unsigned int leap_year_length = 366;
    unsigned int days;
    unsigned int remainder;
    unsigned int month;
    unsigned int month_length;

    clock = clock_time;
    days = elapsed / 86400;
    remainder = elapsed % 86400;
    clock->hour = remainder / 3600;
    remainder -= clock->hour * 3600;
    clock->minute = remainder / 60;
    remainder -= clock->minute * 60;
    clock->second = remainder;

    while (days >= year_length) {
        year += 1;
        days -= year_length;
        year_length = (year & 3) == 0 ? leap_year_length : common_year_length;
    }
    clock->year = year + 2000;

    for (month = 0; ; month += 1) {
        month_length = monthday[month];
        if (month == 1 && (clock->year & 3) == 0)
            month_length += 1;
        if (days < month_length)
            break;
        days -= month_length;
    }
    clock->month = month + 1;
    clock->day = days + 1;
}

int xglCdDiskCheck(void)
{
    unsigned char state;
    int disk_type;

    if (CD_READ_CONTROL->dual_layer_disc != 0) {
        CD_READ_CONTROL->disk_state = 1;
        CD_READ_CONTROL->disk_flags = 127;
    }
    state = CD_READ_CONTROL->disk_state;
    if (state != 1) {
        CD_READ_CONTROL->disk_state = 0;
        if (sceCdStatus() == 1)
            return -2;
        if (sceCdDiskReady(1) != 2)
            return -3;
        disk_type = sceCdGetDiskType();
        if (disk_type == 1)
            return -3;
        if (disk_type == 0)
            return 0;
        if (disk_type != 20)
            return -1;
        CD_READ_CONTROL->disk_flags = 0;
        if (xglCdGetFileSize(D_004D2468) > 0)
            CD_READ_CONTROL->disk_flags |= 1;
        if (xglCdGetFileSize(D_004D2478) > 0)
            CD_READ_CONTROL->disk_flags |= 2;
        if (xglCdGetFileSize(D_004D2488) > 0)
            CD_READ_CONTROL->disk_flags |= 4;
        CD_READ_CONTROL->disk_state = 1;
    } else if (sceCdStatus() == state) {
        CD_READ_CONTROL->disk_state = 0;
        return -2;
    }
    return CD_READ_CONTROL->disk_flags;
}

static void xglCdDefaultCallback(int event, int value)
{
    (void)event;
    (void)value;
}

static void xglCdDummyCallback(int event, int value)
{
    (void)event;
    (void)value;
}

void xglCdReset(void)
{
    CdReadControl *control = CD_READ_CONTROL;

    callback = xglCdDefaultCallback;
    control->queue_cursor = 0;
    control->queue_tail = 0;
    control->dispatch_state = 0;
    StrList = 0;
    sceCdPause();
    sceCdSync(0);
}

CdCompletionCallback *xglCdSetCallback(int callback_address)
{
    CdCompletionCallback *old_callback = callback;

    switch (callback_address) {
    case 0:
        callback = xglCdDefaultCallback;
        break;
    case 1:
        callback = xglCdDummyCallback;
        break;
    case -1:
        break;
    default:
        callback = (CdCompletionCallback *)callback_address;
        break;
    }
    return old_callback;
}

static int xglCdGetFilePosSub(CdArchiveEntry *entry, const char *path)
{
    char name[256];
    char *directory[16];
    const char *source;
    const char *left;
    const char *right;
    const unsigned char *record;
    char *prefix_end;
    char *destination;
    unsigned char length;
    int flags;
    int packed;
    int depth;
    int found;

    source = root_0;
    prefix_end = name;
    record = entry->destination + 1;
    while ((*prefix_end = *source) != 0) {
        source++;
        prefix_end++;
    }
    directory[0] = prefix_end;
    flags = *record;
    depth = 0;
    while (flags != 0) {
        packed = flags & 0x40;
        length = flags & 0x3f;
        if ((flags & 0x80) != 0) {
            depth -= record[1];
            record += 2;
            length -= 2;
            destination = directory[depth];
            while (length != 0) {
                *destination++ = *record++;
                length--;
            }
            depth++;
            destination[0] = '\\';
            destination[1] = 0;
            directory[depth] = destination + 1;
        } else {
            length--;
            destination = directory[depth];
            record++;
            while (length != 0) {
                *destination++ = *record++;
                length--;
            }
            *destination = 0;
            left = name;
            right = path;
            found = 0;
            while (*right == *left) {
                if (*right == 0) {
                    found = 1;
                    break;
                }
                right++;
                left++;
            }
            if (found != 0) {
                entry->file_lbn_offset =
                    (record[2] << 16) + (record[1] << 8) + record[0];
                entry->stored_bytes = (record[6] << 24) + (record[5] << 16)
                    + (record[4] << 8) + record[3];
                entry->packed = packed;
                if (packed != 0) {
                    entry->size_bytes =
                        (record[9] << 16) + (record[8] << 8) + record[7];
                } else {
                    entry->size_bytes = entry->stored_bytes;
                }
                return 1;
            }
            record += 7;
            if (packed != 0)
                record += 3;
        }
        flags = *record;
    }
    return 0;
}

static CdFilePosition *xglCdGetFilePos(CdFilePosition *position, const char *name,
                                       CdCompletionCallback *callback)
{
    char buffer[256];
    const char *path = name;
    CdArchiveEntry *entry;
    const char *cursor;
    char *destination;
    const char *absolute_source;
    char *absolute_destination;
    char *scan;
    int descriptor;
    long offset;
    u8 *header;
    int size;
    int source;

    position->source = 0;
    if (path[0] != 92) {
        for (scan = (char *)path; *scan != 0; scan++) {
            if (*scan == 47)
                *scan = 92;
        }
    }
    for (;;) {
        entry = &ArcHeader[0].entry;
        if (entry->state != 0xff && xglCdGetFilePosSub(entry, name) != 0) {
            name = arcname_1;
            position->source = entry->state;
            break;
        }
        entry = &ArcHeader[1].entry;
        if (entry->state != 0xff && xglCdGetFilePosSub(entry, name) != 0) {
            name = arcname_2;
            position->source = entry->state;
            goto resolved;
        }
        entry++;
        if (entry->state != 0xff && xglCdGetFilePosSub(entry, name) != 0) {
            name = arcname_3;
            position->source = entry->state;
            goto resolved;
        }
        entry++;
        if (entry->state != 0xff && xglCdGetFilePosSub(entry, name) != 0) {
            name = arcname_4;
            position->source = entry->state;
            goto resolved;
        }
        entry = 0;
        goto resolved;
    }
resolved:
    if (name[0] == 92) {
        absolute_source = name + 1;
        absolute_destination = buffer;
        while ((*absolute_destination = *absolute_source) != 0) {
            absolute_source++;
            absolute_destination++;
        }
        position->source = 1;
    } else {
        destination = buffer;
        source = position->source;
        switch (source) {
        case 0:
            buffer[0] = 92;
            destination = buffer + 1;
            for (cursor = name; *cursor != 0; cursor++, destination++) {
                if ((char)*cursor < 97)
                    *destination = *cursor;
                else
                    *destination = *cursor - 32;
            }
            destination[0] = 59;
            destination[1] = 49;
            destination[2] = 0;
            break;
        case 1:
        case 2: {
            if (position->source == 1)
                cursor = vcd_head_5;
            else
                cursor = hdd_head_6;
            while ((*destination = *cursor) != 0) {
                cursor++;
                destination++;
            }
            cursor = name;
            for (;;) {
                if (*cursor == 92)
                    *destination = 47;
                else
                    *destination = *cursor;
                if (*cursor == 0)
                    break;
                cursor++;
                destination++;
            }
            break;
        }
        }
    }
    callback(0, (int)path);
    switch (position->source) {
    case 0:
        if (entry != 0) {
            position->lbn = entry->lbn + entry->file_lbn_offset;
            position->stored_bytes = entry->stored_bytes;
            position->packed = entry->packed;
            position->size_bytes = entry->size_bytes;
        } else {
            sceCdSync(0);
            sceCdDiskReady(0);
            if (sceCdLayerSearchFile(position, buffer, LW.search_layer) == 0)
                return 0;
            position->packed = 1;
            position->size_bytes = position->stored_bytes;
        }
        break;
    case 2:
        if (xglHddCheck() < 0)
            return xglCdGetFilePos(position, path, callback);
    case 1:
        descriptor = sceOpen(buffer, 1);
        if (descriptor < 0)
            return 0;
        if (entry != 0) {
            position->lbn = entry->lbn + entry->file_lbn_offset;
            position->stored_bytes = entry->stored_bytes;
            position->packed = entry->packed;
            position->size_bytes = entry->size_bytes;
        } else {
            position->lbn = 0;
            header = (u8 *)(((u32)WorkEnd + 63) & -64);
            sceRead(descriptor, header, 16);
            if (*(int *)header == 0x585241 && *(int *)(header + 12) == 0) {
                position->packed = 1;
                position->size_bytes = *(int *)(header + 4);
            } else {
                position->packed = 0;
            }
            size = sceLseek(descriptor, 0, 2);
            position->stored_bytes = size;
            if (position->packed == 0)
                position->size_bytes = size;
        }
        offset = (long)(unsigned int)position->lbn << 11;
        sceLseek(descriptor, 0, 0);
        while (offset > 0x40000000) {
            sceLseek(descriptor, 0x40000000, 1);
            offset -= 0x40000000;
        }
        sceLseek(descriptor, (int)offset, 1);
        position->descriptor = descriptor;
        break;
    }
    return position;
}

int xglCdGetFileData(const char *path, CdFilePosition *file_position)
{
    return (xglCdGetFilePos(file_position, path, xglCdDummyCallback) != 0) ? 0 : -1;
}

int xglCdGetFileSize(const char *path)
{
    CdFilePosition position;

    if (xglCdGetFilePos(&position, path, xglCdDummyCallback) == 0)
        return -1;
    switch (position.source) {
    case 0:
        break;
    case 1:
    case 2:
        sceClose(position.descriptor);
        break;
    }
    return position.size_bytes;
}

int xglCdReadFilePart(const char *name, void *buffer, int mode,
                      CdCompletionCallback *notify, int offset, int length)
{
    CdFilePosition position;
    CdReadMode read_mode;
    CdRequestPartial *request;
    const char *source;
    char *destination;
    int asynchronous;
    int result; /* sceRead byte count, or sceCdGetError status */
    int padding;
    int partial;

    switch ((int)notify) {
    case 0:
        notify = callback;
        break;
    case 1:
        notify = xglCdDummyCallback;
        break;
    case 2:
        notify = xglCdDefaultCallback;
        break;
    }
    asynchronous = mode & 1;
    if (asynchronous != 0) {
        if (CD_READ_CONTROL->dispatch_state != 0) {
            if (CD_READ_CONTROL->dispatch_state != 3) {
                request = &CD_READ_CONTROL->requests[CD_READ_CONTROL->queue_tail];
                source = name;
                destination = request->name;
                while ((*destination++ = *source++) != 0)
                    ;
                request->buffer = buffer;
                request->offset = offset;
                request->length = length;
                request->completion_callback = notify;
                CD_READ_CONTROL->queue_tail = (CD_READ_CONTROL->queue_tail + 1) & 0x1f;
                return 0;
            }
        }
    }
    if (CD_READ_CONTROL->dispatch_state != 0)
        return -3;
    if (xglCdGetFilePos(&position, name, notify) == 0)
        return -1;
    position.lbn += offset >> 11;
    if (length > 0)
        position.stored_bytes = length;
    notify(1, (position.stored_bytes + 2047) & -2048);
    CD_READ_CONTROL->completion_callback = notify;
    CD_READ_CONTROL->extract_destination = buffer;
    CD_READ_CONTROL->extract_byte_count = (position.stored_bytes + 2047) & -2048;
    if (position.packed == 0 || (mode & 2) != 0)
        CD_READ_CONTROL->extract_source = buffer;
    else
        CD_READ_CONTROL->extract_source = (u8 *)buffer + ((position.size_bytes + 2047) & -2048)
            - CD_READ_CONTROL->extract_byte_count;
    CD_READ_CONTROL->read_poll_mode = position.source;
    switch (position.source) {
    case 0:
    retry_cd:
        sceCdSync(0);
        sceCdDiskReady(0);
        read_mode.try_count = 0;
        read_mode.spindle_control = CD_READ_CONTROL->spindle_control;
        read_mode.data_pattern = 0;
        sceCdRead(position.lbn, CD_READ_CONTROL->extract_byte_count / 2048, CD_READ_CONTROL->extract_source,
                  &read_mode);
        if (asynchronous != 0) {
            CD_READ_CONTROL->dispatch_state = 2;
            return position.size_bytes;
        }
        CD_READ_CONTROL->dispatch_state = 1;
        while (sceCdSync(1) > 0)
            notify(2, sceCdGetReadPos());
        result = sceCdGetError();
        if ((unsigned int)result >= 2) {
            notify(-2, result);
            goto retry_cd;
        }
        break;
    case 1:
    case 2:
        CD_READ_CONTROL->active_file_descriptor = position.descriptor;
        CD_READ_CONTROL->write_cursor = CD_READ_CONTROL->extract_source;
        CD_READ_CONTROL->remaining_bytes = position.stored_bytes;
        if (asynchronous != 0) {
            CD_READ_CONTROL->dispatch_state = 2;
            return position.size_bytes;
        }
        CD_READ_CONTROL->dispatch_state = 1;
        /* Host/HDD files are read synchronously in chunks of at most 64 KiB;
         * a failed read reports the HDD error and restarts the request. */
        do {
            if (CD_READ_CONTROL->remaining_bytes > 0xffff)
                result = sceRead(CD_READ_CONTROL->active_file_descriptor,
                                 CD_READ_CONTROL->write_cursor, 0x10000);
            else
                result = sceRead(CD_READ_CONTROL->active_file_descriptor,
                                 CD_READ_CONTROL->write_cursor,
                                 CD_READ_CONTROL->remaining_bytes);
            if (result < 0) {
                hdd_error();
                CD_READ_CONTROL->dispatch_state = 0;
                return xglCdReadFilePart(name, buffer, mode, notify, offset, length);
            }
            CD_READ_CONTROL->write_cursor += result;
            CD_READ_CONTROL->remaining_bytes -= result;
            notify(2, position.stored_bytes - CD_READ_CONTROL->remaining_bytes);
        } while (CD_READ_CONTROL->remaining_bytes > 0);
        partial = result & 0x7ff;
        if (partial != 0) {
            padding = -partial & 0x7ff;
            while (padding > 0) {
                *CD_READ_CONTROL->write_cursor++ = padding;
                padding--;
            }
        }
        sceClose(CD_READ_CONTROL->active_file_descriptor);
        CD_READ_CONTROL->active_file_descriptor = -1;
        break;
    }
    if ((mode & 2) == 0)
        extract(CD_READ_CONTROL->extract_destination, CD_READ_CONTROL->extract_source,
                CD_READ_CONTROL->extract_byte_count);
    notify(4, 0);
    CD_READ_CONTROL->dispatch_state = 0;
    return (position.size_bytes + 2047) & -2048;
}

int xglCdReadFile(const char *name, void *buffer, int mode, int flags)
{
    return xglCdReadFilePart(name, buffer, mode, (CdCompletionCallback *)flags, 0, -1);
}

void xglCdReadCancel(void)
{
    LW.queue_tail = LW.queue_cursor;
    if (LW.dispatch_state == 1) {
        switch (LW.read_poll_mode) {
        case 0:
            sceCdBreak();
            sceCdPause();
            break;
        case 1:
        case 2:
            LW.remaining_bytes = 0;
            break;
        }
    } else if (LW.dispatch_state == 2) {
        switch (LW.read_poll_mode) {
        case 0:
            sceCdBreak();
            sceCdPause();
            break;
        case 1:
        case 2:
            if (LW.active_file_descriptor >= 0) {
                sceClose(LW.active_file_descriptor);
                LW.active_file_descriptor = -1;
            }
            break;
        }
    }
    LW.dispatch_state = 0;
}

int xglCdSync(void)
{
    return LW.dispatch_state != 0;
}

int xglCdStreamOpen(CdStreamParam *stream, const char *path)
{
    CdFilePosition position;
    CdReadMode read_mode;
    CdStreamParam **slot;
    int index;
    int mode;

    LW.dispatch_state = 3;
    if (xglCdGetFilePos(&position, path, xglCdDummyCallback) == 0) {
        LW.dispatch_state = 0;
        return -1;
    }
    stream->mode = position.source;
    stream->file_byte_count = position.stored_bytes;
    mode = position.source;
    switch (mode) {
    case 0:
        sceCdSync(0);
        sceCdDiskReady(0);
        stream->descriptor = position.lbn;
        stream->iop_buffer = sceSifAllocIopHeap(((short)stream->sector_count << 11) + 128);
        sceCdStInit((short)stream->sector_count, (short)stream->block_count,
                    (stream->iop_buffer + 127) & -128);
        read_mode.try_count = 0;
        read_mode.spindle_control = 0;
        read_mode.data_pattern = 0;
        sceCdStStart(stream->descriptor, &read_mode);
        break;
    case 1:
    case 2:
        stream->descriptor = position.descriptor;
        break;
    }
    stream->remaining = stream->file_byte_count;
    stream->ring_size = 2048;
    stream->state = 0;
    if (stream->ring.buffer != 0) {
        stream->ring.read_position = 0;
        stream->ring.write_position = 0;
        for (slot = &StrList, index = 0; index <= STR_LIST_LAST_INDEX; index++, slot++) {
            if (*slot == 0) {
                *slot = stream;
                break;
            }
        }
    }
    return 0;
}

int xglCdStreamRead(CdStreamParam *stream, unsigned char *buffer, int bytes)
{
    int bytes_read;
    int remaining;
    int pad;

    /* bytes is a byte count; StreamReadRingCoreSub takes a sector count
     * (sll ...,0xb at the call site), matching CD_SECTOR_BYTE_SHIFT below. */
    bytes_read = StreamReadRingCoreSub(stream, buffer, bytes >> 11);
    remaining = stream->remaining - bytes_read;
    if (remaining <= 0) {
        remaining = 0;
        stream->state = 1;
    }
    /* ring_size is a power of two; pad is the read's own tail within the
     * ring, zero-filled below and folded into the returned byte count. */
    pad = bytes_read & (stream->ring_size - 1);
    stream->remaining = remaining;
    if (pad > 0) {
        buffer += bytes_read;
        bytes_read += pad;
        do {
            pad--;
            *buffer++ = 0;
        } while (pad > 0);
    }
    return bytes_read;
}

static int StreamReadRingCoreSub(CdStreamParam *stream, void *buffer, int sectors)
{
    /*
     * CD sectors are CD_SECTOR_BYTES bytes; CD_SECTOR_BYTE_SHIFT converts a
     * sector count to a byte count with a left shift, matching the
     * original sll ...,0xb.
     */
    enum { CD_SECTOR_BYTE_SHIFT = 11 };
    int read_error;
    int mode;
    int result = 0;

    mode = stream->mode;
    switch (mode) {
    case 0:
        result = sceCdStRead(sectors, buffer, 1, &read_error);
        result <<= CD_SECTOR_BYTE_SHIFT;
        break;
    case 1:
    case 2:
        result = sceRead(stream->descriptor, buffer, sectors << CD_SECTOR_BYTE_SHIFT);
        break;
    }
    return result;
}

static void StreamReadRingCoreNormal(CdStreamParam *stream)
{
    /* Sectors to read: [0] up to the first boundary, [1] after the wrap. */
    int sectors[2];
    int available;
    int mode;
    int bytes;
    int end_offset;
    int padding;
    int remaining;

    u8 *cursor;
    u8 *end;

    if (stream->remaining > 0) {
        if (stream->ring.write_position <= stream->ring.read_position) {
            sectors[1] = 0;
            sectors[0] = (stream->ring.read_position - stream->ring.write_position) / 2048;
        } else {
            sectors[0] = (stream->ring.capacity - stream->ring.write_position) / 2048;
            sectors[1] = stream->ring.read_position / 2048;
        }
        available = 0;
        mode = stream->mode;
        switch (mode) {
        case 0:
            available = sceCdStStat();
            break;
        case 1:
        case 2:
            available = 32;
            break;
        }
        if (available < sectors[0]) {
            sectors[0] = available;
            sectors[1] = 0;
        } else if (available - sectors[0] < sectors[1]) {
            sectors[1] = available - sectors[0];
        }
        if (sectors[0] > 0) {
            cursor = stream->ring.buffer + stream->ring.write_position;
            bytes = StreamReadRingCoreSub(stream, cursor, sectors[0]);
            if (sectors[1] > 0) {
                cursor = stream->ring.buffer;
                bytes = StreamReadRingCoreSub(stream, cursor, sectors[1]);
            }
            remaining = stream->remaining - ((sectors[0] + sectors[1]) << 11);
            if (remaining <= 0) {
                end = cursor + bytes;
                stream->state = 1;
                end_offset = (end - stream->ring.buffer) & (stream->ring_size - 1);
                remaining = 0;
                if (end_offset > 0) {
                    padding = stream->ring_size - end_offset;
                    cursor = end;
                    if (padding > 0) {
                        do {
                            padding--;
                            *cursor++ = 0;
                        } while (padding > 0);
                    }
                }
            }
            stream->remaining = remaining;
            stream->ring.write_position =
                (stream->ring.write_position + ((sectors[0] + sectors[1]) << 11))
                % stream->ring.capacity;
        }
    }
}

static void StreamReadRingCoreXss(CdStreamParam *stream)
{
    StreamXssBuffer *xss;
    StreamRing *ring;
    int available;
    int mode;

    if (stream->remaining > 0) {
        available = 0;
        mode = stream->mode;
        xss = stream->xss;
        switch (mode) {
        case 0:
            available = sceCdStStat();
            break;
        case 1:
        case 2:
            available = 32;
            break;
        }
        while (available > 0 && stream->remaining > 0) {
            if (xss->sectors <= 0) {
                StreamReadRingCoreSub(stream, xss->source, 1);
                xss->position = xss->source;
                xss->value = 0;
                xss->mode = 0;
                xss->sectors = 0x4000;
            } else {
                if (xss->mode == 0) {
                    xss->value = *xss->position;
                    xss->position += 1;
                    xss->mode = 8;
                }
                ring = ((xss->value >> (8 - xss->mode)) & 1) == 0
                    ? &stream->ring : &stream->alternate_ring;
                if (ring->write_position == ring->read_position)
                    break;
                xss->mode--;
                xss->sectors--;
                StreamReadRingCoreSub(stream, ring->buffer + ring->write_position, 1);
                ring->write_position = (ring->write_position + 2048) % ring->capacity;
            }
            available--;
            stream->remaining -= 2048;
        }
        if (stream->remaining <= 0) {
            stream->remaining = 0;
            stream->state = 1;
        }
    }
}

void xglCdStreamReadRingCore(CdStreamParam *stream)
{
    if (stream->xss)
        StreamReadRingCoreXss(stream);
    else
        StreamReadRingCoreNormal(stream);
}

int xglCdStreamReadRing(CdStreamParam *stream, int bytes)
{
    int aligned;

    aligned = (bytes + 2047) & -2048;
    stream->ring.read_position =
        (stream->ring.read_position + aligned) % stream->ring.capacity;
    do {
        xglCdStreamReadRingCore(stream);
    } while (stream->remaining != 0 &&
             stream->ring.write_position != stream->ring.read_position);
    return 0;
}

int xglCdStreamRewind(CdStreamParam *stream)
{
    int mode;

    stream->state = 0;
    stream->remaining = stream->file_byte_count;
    mode = stream->mode;
    switch (mode) {
    case 0:                                 /* CD stream */
        sceCdStSeek(stream->descriptor);
        break;
    case 1:                                 /* host / memory-card file */
    case 2:
        sceLseek(stream->descriptor, 0, 0);
        break;
    }
    return 0;
}

int xglCdStreamClose(CdStreamParam *stream)
{
    CdStreamParam **slot;
    int index;
    int mode;

    slot = &StrList;
    for (index = 0; index < 1; ++index, ++slot) {
        if (*slot == stream) {
            *slot = 0;
            break;
        }
    }

    if (stream->descriptor == -1)
        return 0;

    mode = stream->mode;
    switch (mode) {
    case 0:                                 /* CD stream */
        sceCdStStop();
        if (stream->iop_buffer != 0)
            sceSifFreeIopHeap((void *)stream->iop_buffer);
        stream->descriptor = -1;
        break;
    case 1:                                 /* host / memory-card file */
    case 2:
        sceClose(stream->descriptor);
        break;
    }
    LW.dispatch_state = 0;
    return 0;
}

void xglCdStreamParamInit(CdStreamParam *param)
{
    param->sector_count = 64;
    param->block_count = 16;
    param->descriptor = -1;
    param->iop_buffer = 0;
    param->state = 0;
    param->xss = 0;
    param->ring.buffer = 0;
}

static void FileSelectListReload(CdFileSelect *select)
{
    char path[256];
    unsigned int index;
    char *list;
    char *line;
    char *destination;
    const char *source;
    int descriptor;
    unsigned char character;

    list = select->list;
    for (index = 0; index < select->list_size; index++)
        *list++ = 0;
    destination = path;
    for (source = select->directory; *source != 0; source++, destination++)
        *destination = *source;
    for (source = listname_7; *source != 0; source++, destination++)
        *destination = *source;
    *destination = 0;
    descriptor = sceOpen(path, 1);
    sceRead(descriptor, select->list, select->list_size);
    sceClose(descriptor);
    line = select->list;
    character = *line;
    if (character != 0) {
        do {
            if (character == '\n')
                *line = 0;
            line++;
            character = *line;
        } while (character != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", FileSelectSub);

/* FileSelect reads the first pad record at +0x2a and +0x2c. */
typedef struct CdSelectPad {
    unsigned char unmodeled_00[0x28];
    unsigned short held;
    unsigned short pressed;
    unsigned short repeated;
    unsigned char unmodeled_2e[0x3a];
} CdSelectPad;
extern CdSelectPad PadData;

int xglCdFileSelect(CdFileSelect *select)
{
    int index;
    int skipped;
    int end_row;
    int rows;
    int count;
    int x;
    int y;
    unsigned char *list;
    char *last;
    char *path;
    char *tail;
    unsigned char *entry;

    FileSelectSub(select);
    if ((PadData.pressed & 0x40) != 0)
        return 2;
    if (PadData.repeated & 0x1000)
        select->cursor--;
    if (select->cursor < 0)
        select->cursor = select->entry_count - 1;
    if (select->cursor - select->first_row <= 0)
        select->first_row = select->cursor - 1;
    if (select->first_row < 0)
        select->first_row = 0;
    if (PadData.repeated & 0x4000)
        select->cursor++;
    if (select->entry_count - 1 < select->cursor)
        select->cursor = 0;
    if (select->cursor - select->first_row > select->rows - 2)
        select->first_row = select->cursor - select->rows + 2;
    if ((unsigned int)(select->entry_count - select->rows) < (unsigned int)select->first_row)
        select->first_row = select->entry_count - select->rows;
    if (PadData.pressed & 0x20) {
        entry = (unsigned char *)select->list_position;
        if (select->path[0] != 0)
            select->cursor--;
        if (select->cursor < 0) {
            path = select->path;
            select->cursor = 0;
            select->state = 3;
            select->first_row = 0;
        } else {
            for (skipped = 0; skipped < select->cursor; skipped++) {
                while (*entry++ != 0)
                    ;
            }
            last = (char *)entry;
            while (last[1] != 0)
                last++;
            path = select->path;
            tail = path;
            while (*tail != 0)
                tail++;
            do {
                *tail++ = *entry;
            } while (*entry++ != 0);
            if (*last == '/') {
                select->cursor = 0;
                select->state = 1;
                select->first_row = 0;
            } else {
                select->cursor = 0;
                select->state = 3;
                select->first_row = 0;
                return 1;
            }
        }
    } else {
        path = select->path;
    }
    x = select->x;
    y = select->y;
    list = (unsigned char *)select->list_position;
    select->print(x, y, D_004D2498, path);
    count = select->entry_count;
    rows = select->rows;
    if (rows < count)
        end_row = rows;
    else
        end_row = count;
    end_row += select->first_row;
    index = select->path[0] != 0;
    y += 12;
    if (select->first_row > 0)
        select->print(x, y, D_004DC2F8);
    for (; index < select->first_row; index++) {
        while (*list++ != 0)
            ;
    }
    for (index = select->first_row; index < end_row; index++) {
        if (index == select->cursor)
            select->print(x + 8, y, D_004DC300);
        if (index == 0 && select->path[0] != 0) {
            select->print(x + 16, y, D_004DC308);
        } else {
            select->print(x + 16, y, list);
            while (*list++ != 0)
                ;
            if (*list == 0)
                return 0;
        }
        y += 8;
    }
    if (*list != 0)
        select->print(x, y - 8, D_004DC310);
    return 0;
}

void xglCdSifLoadModule(const char *path, const char *args)
{
    char name[256];
    const char *src;
    char *dst;
    int args_length;

    src = head_8;
    dst = name;
    while ((*dst = *src) != 0) {
        src++;
        dst++;
    }
    for (src = path; *src != 0; src++, dst++) {
        if ((char)*src < 'a')
            *dst = *src;
        else
            *dst = *src - ('a' - 'A');
    }
    src = tail_9;
    while ((*dst = *src) != 0) {
        src++;
        dst++;
    }
    args_length = 0;
    if (args != 0) {
        const char *scan = args;

        args_length = 1;
        if (scan[0] != 0 || scan[1] != 0) {
            for (;;) {
                scan++;
                args_length++;
                if (*scan == 0 && scan[1] == 0)
                    break;
            }
        }
    }
    sceSifLoadModule(name, args_length, args);
}

void xglCdLoadOverlay(int overlay)
{
    char exec_data[16];
    char name[256];
    const char *src;
    char *dst;

    if (overlay != loaded_overlay) {
        loaded_overlay = overlay;
        src = head_10;
        dst = name;
        while ((*dst = *src) != 0) {
            src++;
            dst++;
        }
        dst[0] = 'O';
        dst[1] = 'V';
        dst[2] = overlay / 10 + '0';
        dst[3] = overlay % 10 + '0';
        dst += 4;
        src = tail_11;
        while ((*dst = *src) != 0) {
            src++;
            dst++;
        }
        FlushCache(0);
        FlushCache(2);
        sceSifLoadElf(name, exec_data);
        FlushCache(0);
        FlushCache(2);
    }
}

static void xglCdArcInitSub0(const int *archive_entry, void *destination, int sectors)
{
    int lbn;
    CdReadMode read_mode;

    sceCdSync(0);
    sceCdDiskReady(0);
    if (sectors == 0) {
        lbn = archive_entry[0];
        sectors = 1;
    } else {
        lbn = archive_entry[0] + 1;
    }
    read_mode.spindle_control = 1;
    read_mode.try_count = 0;
    read_mode.data_pattern = 0;
    sceCdRead(lbn, sectors, destination, &read_mode);
    while (sceCdSync(1) > 0)
        ;
}

static int xglCdArcInitSub1(CdArchiveEntry *archive_entry,
                            const char *path, unsigned char *destination)
{
    CdFilePosition file_position;
    int header_count;

    if (xglCdGetFilePos(&file_position, path, xglCdDummyCallback) == 0)
        return 0;

    archive_entry->destination = destination;
    archive_entry->lbn = file_position.lbn;
    archive_entry->state = 0;

    /* xglCdArcInitSub0 only reads its `archive_entry` argument's leading
     * word as an LBN, so the file position's own lbn field (the one
     * evidenced member of CdFilePosition) is exactly what it needs. */
    xglCdArcInitSub0(&file_position.lbn, destination, 0);

    /* Sector 0 of the file is the archive header; its first byte is the
     * header's own sector count. */
    header_count = destination[0];
    if (header_count >= 2) {
        /* The header spans more than one sector: read the remaining
         * header_count - 1 sectors right after the first. */
        xglCdArcInitSub0(&file_position.lbn, destination + CD_SECTOR_BYTES,
                         header_count - 1);
    }
    return header_count + 1;
}

static unsigned char *xglCdArcInitSub2(unsigned char *archive_data)
{
    unsigned char *cursor = archive_data + 1;
    unsigned char value = *cursor;

    while (value != 0) {
        int high_bit = value & ARC_RECORD_EXTENDED;
        int type_flag = value & ARC_RECORD_TYPE_EXTENDED;
        if (high_bit == 0) {
            cursor += ARC_RECORD_HEADER_BYTES;
            if (type_flag != 0)
                cursor += ARC_RECORD_TYPE_EXTRA_BYTES;
        }
        value &= ARC_RECORD_LENGTH_MASK;
        cursor += value;
        value = *cursor;
    }

    /* Round up to the next ARC_TABLE_ALIGN_BYTES-byte boundary and write a
     * two-byte zero terminator marking the end of the table. */
    cursor = (unsigned char *)(((u32)cursor + ARC_TABLE_ALIGN_BYTES)
        & (u32)-ARC_TABLE_ALIGN_BYTES);
    cursor[1] = 0;
    cursor[0] = 0;
    return cursor;
}

static void xglCdArcInit(void)
{
    u8 *work = WorkEnd;
    unsigned char header_sectors = 0;
    int descriptor;

    while (xglHddActivate(-1) != 0 && (descriptor = sceOpen(D_004D24A8, 1)) >= 0) {
        ArcHeader[0].entry.state = 2;
        ArcHeader[0].entry.destination = work;
        ArcHeader[0].entry.lbn = 0;
        sceRead(descriptor, work, CD_SECTOR_BYTES);
        header_sectors = work[0];
        sceRead(descriptor, work + CD_SECTOR_BYTES,
                (header_sectors << 11) - CD_SECTOR_BYTES);
        sceClose(descriptor);
        break;
    }
    if (header_sectors == 0) {
        if (xglCdArcInitSub1(&ArcHeader[0].entry, D_004D24C0, work) == 0)
            return;
        xglHddActivate(0x100);
    }
    do {
        if (LW.dual_layer_disc != 0) {
            work = xglCdArcInitSub2(work);
            do {
            } while (xglCdArcInitSub1(&ArcHeader[1].entry, D_004D2468, work) <= 0);
            work = xglCdArcInitSub2(work);
            LW.search_layer = 1;
            do {
            } while (xglCdArcInitSub1(&ArcHeader[2].entry, D_004D2478, work) <= 0);
            LW.search_layer = 0;
            work += ((xglCdArcInitSub2(work) - work) + 2047) & -2048;
        } else {
            work += 0x2D000;
            while (xglCdArcInitSub1(&ArcHeader[1].entry, D_004D2468, work) <= 0
                   && xglCdArcInitSub1(&ArcHeader[1].entry, D_004D2478, work) <= 0) {
                if (xglCdArcInitSub1(&ArcHeader[1].entry, D_004D2488, work) <= 0) {
                    ArcHeader[1].entry.state = 0xff;
                    work[0] = 0;
                    work[1] = 0;
                }
                break;
            }
            work += 0x2000;
        }
    } while (0);
    WorkEnd = work;
}

void xglCdArcCheck(void)
{
    u8 *saved_work_end = WorkEnd;

    WorkEnd = ArcHeader[0].entry.destination;
    xglCdArcInit();
    WorkEnd = saved_work_end;
}

void xglCdInitial(void)
{
    int on_dual;
    u8 *src;
    u8 *dst;
    int count;

    LW.spindle_control = 0;
    LW.search_layer = 0;
    LW.dual_layer_disc = 0;
    LW.power_off_pending = 0;
    LW.disk_state = 0;
    LW.disk_flags = 0;
    sceCdInit(0);
    sceCdMmode(2);
    xglCdReset();
    on_dual = 0;
    sceCdReadDvdDualInfo(&on_dual);
    loaded_overlay = 2;
    LW.dual_layer_disc = on_dual;
    src = WorkEnd;
    ArcHeader[0].entry.state = 0xff;
    ArcHeader[1].entry.state = 0xff;
    ArcHeader[2].entry.state = 0xff;
    ArcHeader[3].entry.state = 0xff;
    dst = system_cnf;
    xglCdReadFile(D_004D24D0, src, 0, 1);
    for (count = 79; count >= 0; count--)
        *dst++ = *src++;
    xglHddActivate(0x100);
    xglHddMount();
    xglCdArcInit();
}


