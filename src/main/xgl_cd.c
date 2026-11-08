#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "main/xgl_thread.h"
#include "main/xgl_hdd.h"
#include "main/xgl_timer0.h"

/* StrList holds a single global stream slot. The control thread's stream case,
 * xglCdStreamOpen and xglCdStreamClose walk it as a one-entry list. */
#define STR_LIST_LAST_INDEX 0

/* The CD reader's control block (the single LW instance). */
#define CD_READ_CONTROL (&LW)

/* A CD sector is fixed at 2048 bytes on this drive/read path (see also
 * xglCdStreamReadRing's own `& -2048` sector rounding later in this TU). */
#define CD_SECTOR_BYTES 2048

#define ARC_RECORD_EXTENDED        0x80

#define ARC_RECORD_TYPE_EXTENDED   0x40
#define ARC_RECORD_LENGTH_MASK     0x3f
#define ARC_RECORD_HEADER_BYTES    7
#define ARC_RECORD_TYPE_EXTRA_BYTES 3
#define ARC_TABLE_ALIGN_BYTES      64
#define CD_PACKED_MAGIC 0x585241
#define ARC_ENTRY_UNUSED 0xff

/* CdReadControl.dispatch_state: what the control thread is doing. */
#define CD_STATE_IDLE 0
#define CD_STATE_SYNC_READ 1
#define CD_STATE_ASYNC_READ 2
#define CD_STATE_STREAM 3

/* The request queue is a 32-entry ring. */
#define CD_QUEUE_SIZE 32
#define CD_QUEUE_MASK (CD_QUEUE_SIZE - 1)

/* Host and HDD files are read in pieces of at most 64 KiB. */
#define CD_FILE_CHUNK_BYTES 0x10000
#define CD_SECTOR_MASK (CD_SECTOR_BYTES - 1)
#define CD_STREAM_FILE_SECTORS 32
#define CD_STREAM_XSS_SECTORS 0x4000
#define CD_STREAM_IOP_ALIGN 128
#define CD_STREAM_DEFAULT_SECTORS 64
#define CD_STREAM_DEFAULT_BLOCKS 16
#define CD_CLOCK_READ_INTERVAL 10
#define CD_PATH_MAX 256
#define CD_SYSTEM_CNF_SIZE 80
#define CD_ARC_LOOKAHEAD_BYTES 0x2D000
#define CD_ARC_HEADER_RESERVE 0x2000

/* sceCdGetDiskType() / sceCdStatus() / sceCdDiskReady() values. */
#define CD_DISC_NONE 0
#define CD_DISC_DETECTING 1
#define CD_DISC_PS2_DVD 20
#define CD_STATUS_SHELL_OPEN 1
#define CD_DISK_READY_COMPLETE 2

/* Calendar arithmetic of xglClockDayTime2UInt()/xglClockUInt2DayTime(). */
#define CLOCK_BASE_YEAR 2000
#define CLOCK_SECONDS_PER_MINUTE 60
#define CLOCK_SECONDS_PER_HOUR 3600
#define CLOCK_SECONDS_PER_DAY 86400
#define CLOCK_SECONDS_PER_COMMON_YEAR 31536000

typedef void CdCompletionCallback(int event, int value);

/*
 * The queue entries occupy 0x80 bytes; only their used prefix is named.
 * xglCdReadFilePart stores its buffer argument at entry+0x00 and copies its
 * name argument into entry+0x10. queue_next hands those two fields back to it
 * as its name and buffer arguments.
 */
typedef struct CdRequestPartial {
    void *buffer;
    int offset;
    int length;
    CdCompletionCallback *completion_callback;
    char name[0x70];
} CdRequestPartial;

/*
 * Partial view of LW, limited to the fields this TU uses.
 * unmodeled_XX spans are opaque bytes between the used fields; naming them
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
    void *extract_source;                         /* +0x20 */
    void *extract_destination;                    /* +0x24 */
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

/* Four 0x18-byte archive slots; the initialization helper models each prefix. */
typedef union CdArchiveStorage {
    CdArchiveEntry entry;
    unsigned char bytes[0x18];
} CdArchiveStorage;

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
    int list_index;                                /* +0x224 */
    char *list_position;                           /* +0x228 */
    unsigned char unmodeled_22c[0x268 - 0x22c];    /* +0x22c */
} CdFileSelect;

typedef struct CdPackedHeader {
    int magic;
    int size_bytes;
    int unmodeled_08;
    int mode;
} CdPackedHeader;

/* FileSelect reads the first pad record at +0x2a and +0x2c. */
typedef struct CdSelectPad {
    unsigned char unmodeled_00[0x28];
    unsigned short held;
    unsigned short pressed;
    unsigned short repeated;
    unsigned char unmodeled_2e[0x3a];
} CdSelectPad;

extern const char xglMenuPfs0Path[];
extern const char xglMenuPfs1Path[];
extern const char xglMenuHddPath[];
extern const char fileSelectPathFormat[];
extern const char fileSelectUpArrowText[];
extern const char fileSelectCursorText[];
extern const char fileSelectParentText[];
extern const char fileSelectDownArrowText[];
extern const char xenosagaPfs0Path[];
extern const char xenosagaArc00Name[];
extern CdSelectPad PadData;

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

/* Defined later in this file; declared here, like BCD2INT below, so its
 * static definition does not follow a non-static declaration. */
static CdFilePosition *xglCdGetFilePos(CdFilePosition *file_position, const char *path,
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

/*
 * File-local functions (LOCAL in the original symbol table) that the shared
 * header declares `extern`: declare them static first so their static
 * definitions below do not follow a non-static declaration.
 */
static int BCD2INT(unsigned char x);

/* Declared before its callers, so they do not see an implicit declaration. */
static void extract(u32 *destination, u32 *source, int byte_count);

extern int sceCdStRead(int sectors, void *buffer, int mode, int *result);
extern int sceRead(int descriptor, void *buffer, int bytes);
static int StreamReadRingCoreSub(CdStreamParam *stream, void *buffer, int sectors);
static void StreamReadRingCoreNormal(CdStreamParam *stream);
static void StreamReadRingCoreXss(CdStreamParam *stream);

extern int sceDevctl(const char *device, int command, const void *input,
                     unsigned int input_size, void *output, unsigned int output_size);

extern int sceCdPause(void);
extern int sceCdPowerOff(u32 *result);
extern u32 sceCdGetReadPos(void);

extern int sceCdGetError(void);
extern int sceCdLayerSearchFile(CdFilePosition *file, const char *name, int layer);
const char xenosagaArc10Name[16] = "xenosaga.10";

const char xenosagaArc20Name[16] = "xenosaga.20";

const char xenosagaArc30Name[16] = "xenosaga.30";

const char systemConfigName[16] = "system.cnf";

const char fileSelectPathFormat[16] = "PATH:/%s";

const char xenosagaPfs0Path[24] = "pfs0:/xenosaga.00";

const char xenosagaArc00Name[16] = "xenosaga.00";

/* Twelve month lengths followed by twelve cumulative month offsets. */
static unsigned short monthday[24] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
};

unsigned char system_cnf[CD_SYSTEM_CNF_SIZE] = {0};

static char listname_7[] = ".filelist";

static char head_8[] = "cdrom0:\\IOP\\";

static char head_10[] = "cdrom0:\\";

static char root_0[] = "data\\";

static char hdd_head_6[] = "pfs0:/";

static char arcname_1[] = "xenosaga.00";

static char arcname_2[] = "xenosaga.10";

static char arcname_3[] = "xenosaga.20";

static char arcname_4[] = "xenosaga.30";

static char vcd_head_5[] = "host0:/home/xeno/vcd-u/";

static char tail_9[] = ".IRX;1";

static char tail_11[] = ".OVL;1";

const char fileSelectUpArrowText[8] = "^";

const char fileSelectCursorText[8] = ">";

const char fileSelectParentText[8] = "..";

const char fileSelectDownArrowText[8] = "v";

static CdCompletionCallback *callback;

static CdReadControl LW;

static CdArchiveStorage ArcHeader[4];

static CdCompletionCallback *callback;

static signed char ReadClockInterval;

static unsigned char PresentTime[8];

static CdStreamParam *StrList;

static unsigned char loaded_overlay;

static unsigned char loaded_overlay;

/* Declared here, not at the top: moving it up changes the original code. */
extern signed char ReadClockInterval;

extern unsigned char PresentTime[8];
extern CdStreamParam *StrList;

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

/*
 * Copies byte_count bytes word by word from source to destination. A packed
 * archive record at source is expanded by xglArxExtract instead.
 */
static void extract(u32 *destination, u32 *source, int byte_count)
{
    if (source[0] == CD_PACKED_MAGIC && source[3] == 0) {
        do {
            xglArxExtract(destination, source);
        } while (0);
    } else if (destination != source) {
        while (byte_count > 0) {
            *destination++ = *source++;
            byte_count -= 4;
        }
    }
}

/*
 * Reports the finished request to the completion callback, then starts the
 * next queued request. Returns 1 when the queue is empty.
 */
static int queue_next(void)
{
    CdRequestPartial *request;

    CD_READ_CONTROL->completion_callback(CD_EVENT_DONE,
                                         (CD_READ_CONTROL->queue_tail
                                          - CD_READ_CONTROL->queue_cursor) & CD_QUEUE_MASK);
    CD_READ_CONTROL->dispatch_state = CD_STATE_IDLE;
    if (CD_READ_CONTROL->queue_tail != CD_READ_CONTROL->queue_cursor) {
        request = &CD_READ_CONTROL->requests[CD_READ_CONTROL->queue_cursor];
        xglCdReadFilePart(request->name, request->buffer, 1,
                          request->completion_callback, request->offset,
                          request->length);
        CD_READ_CONTROL->queue_cursor = (CD_READ_CONTROL->queue_cursor + 1) & CD_QUEUE_MASK;
        return 0;
    }
    return 1;
}

/*
 * Control thread loop: services the active read according to dispatch_state
 * and read_poll_mode, and handles the power-off request.
 */
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
            sceDevctl(xglMenuPfs0Path, HDD_DEVCTL_CLOSE_ALL, 0, 0, 0, 0);
            sceDevctl(xglMenuPfs1Path, HDD_DEVCTL_CLOSE_ALL, 0, 0, 0, 0);
            sceDevctl(xglMenuHddPath, HDD_DEVCTL_DEV9_OFF, 0, 0, 0, 0);
            sceCdPowerOff(&power_status);
        }

        switch (CD_READ_CONTROL->dispatch_state) {
        case CD_STATE_ASYNC_READ:
            switch (CD_READ_CONTROL->read_poll_mode) {
            case CD_SOURCE_DISC:
                CD_READ_CONTROL->completion_callback(CD_EVENT_PROGRESS_ASYNC, sceCdGetReadPos());
                if (sceCdSync(1) != 0)
                    continue;
                extract(CD_READ_CONTROL->extract_destination,
                        CD_READ_CONTROL->extract_source,
                        CD_READ_CONTROL->extract_byte_count);
                if (queue_next() == 0)
                    continue;
                sceCdPause();
                continue;
            /* Host and HDD reads move at most one 64 KiB chunk per pass. */
            case CD_SOURCE_HOST:
            case CD_SOURCE_HDD:
                requested_bytes = CD_READ_CONTROL->remaining_bytes;
                if (requested_bytes > CD_FILE_CHUNK_BYTES)
                    requested_bytes = CD_FILE_CHUNK_BYTES;
                transferred = sceRead(CD_READ_CONTROL->active_file_descriptor,
                                      CD_READ_CONTROL->write_cursor,
                                      requested_bytes);
                if (transferred < 0) {
                    hdd_error();
                    CD_READ_CONTROL->queue_cursor =
                        (CD_READ_CONTROL->queue_cursor + CD_QUEUE_SIZE - 1) & CD_QUEUE_MASK;
                    continue;
                }

                CD_READ_CONTROL->write_cursor += transferred;
                CD_READ_CONTROL->remaining_bytes -= transferred;
                CD_READ_CONTROL->completion_callback(
                    CD_EVENT_PROGRESS_ASYNC, sceLseek(CD_READ_CONTROL->active_file_descriptor, 0, 1));
                if (CD_READ_CONTROL->remaining_bytes > 0)
                    continue;

                {
                    int partial_sector = transferred & CD_SECTOR_MASK;
                    if (partial_sector != 0) {
                        int remaining_padding = (-partial_sector) & CD_SECTOR_MASK;
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
        /* Refill each registered stream. */
        case CD_STATE_STREAM: {
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
        case CD_STATE_IDLE:
        case CD_STATE_SYNC_READ:
            break;
        default:
            CD_READ_CONTROL->dispatch_state = CD_STATE_IDLE;
        }
    }
}

static int BCD2INT(unsigned char x)
{
    return (x >> 4) * 10 + (x & 15);
}

/*
 * Fills clock from the drive's BCD clock. The drive is queried again once
 * every CD_CLOCK_READ_INTERVAL calls.
 */
void xglClockRead(XglClock *clock)
{
    if (ReadClockInterval == 0) {
        ReadClockInterval = CD_CLOCK_READ_INTERVAL;
        sceCdReadClock(PresentTime);
    }

    clock->year = BCD2INT(PresentTime[7]) + CLOCK_BASE_YEAR;
    clock->month = BCD2INT(PresentTime[6]);
    clock->day = BCD2INT(PresentTime[5]);
    clock->hour = BCD2INT(PresentTime[3]);
    clock->minute = BCD2INT(PresentTime[2]);
    clock->second = BCD2INT(PresentTime[1]);
    clock->status = 0;
}

/*
 * Converts a calendar record to the number of seconds elapsed since
 * 2000-01-01 00:00:00.  monthday[12..23] holds the cumulative day offset of
 * each month; the leap day of the current year counts from March onwards,
 * and ((year - 1997) >> 2) counts the leap days of the whole years before it.
 */
unsigned int xglClockDayTime2UInt(XglClock *clock_time)
{
    unsigned int seconds;
    unsigned int year;
    unsigned char month;

    month = clock_time->month;
    seconds = (monthday[month + 11] + clock_time->day - 1) * CLOCK_SECONDS_PER_DAY
        + clock_time->hour * CLOCK_SECONDS_PER_HOUR + clock_time->minute * CLOCK_SECONDS_PER_MINUTE
        + clock_time->second;
    year = clock_time->year;
    if (((year - CLOCK_BASE_YEAR) & 3) == 0 && month >= 3) {
        seconds += CLOCK_SECONDS_PER_DAY;
    }
    return seconds
        + ((year - CLOCK_BASE_YEAR) * CLOCK_SECONDS_PER_COMMON_YEAR
           + ((year - 1997) >> 2) * CLOCK_SECONDS_PER_DAY);
}

/* Splits elapsed seconds since 2000-01-01 00:00:00 into clock_time's date and time. */
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
    days = elapsed / CLOCK_SECONDS_PER_DAY;
    remainder = elapsed % CLOCK_SECONDS_PER_DAY;
    clock->hour = remainder / CLOCK_SECONDS_PER_HOUR;
    remainder -= clock->hour * CLOCK_SECONDS_PER_HOUR;
    clock->minute = remainder / CLOCK_SECONDS_PER_MINUTE;
    remainder -= clock->minute * CLOCK_SECONDS_PER_MINUTE;
    clock->second = remainder;

    while (days >= year_length) {
        year += 1;
        days -= year_length;
        year_length = (year & 3) == 0 ? leap_year_length : common_year_length;
    }
    clock->year = year + CLOCK_BASE_YEAR;

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

/*
 * Polls the disc. Returns a CD_CHECK_* code while no usable disc is ready, 0
 * when no disc is present, or the CD_DISC_FLAG_* bits for the archives found
 * on a PS2 DVD.
 */
int xglCdDiskCheck(void)
{
    unsigned char state;
    int disk_type;

    if (CD_READ_CONTROL->dual_layer_disc != 0) {
        CD_READ_CONTROL->disk_state = 1;
        CD_READ_CONTROL->disk_flags = CD_DISC_FLAGS_DUAL_LAYER;
    }
    state = CD_READ_CONTROL->disk_state;
    if (state != 1) {
        CD_READ_CONTROL->disk_state = 0;
        if (sceCdStatus() == CD_STATUS_SHELL_OPEN)
            return CD_CHECK_SHELL_OPEN;
        if (sceCdDiskReady(1) != CD_DISK_READY_COMPLETE)
            return CD_CHECK_NOT_READY;
        disk_type = sceCdGetDiskType();
        if (disk_type == CD_DISC_DETECTING)
            return CD_CHECK_NOT_READY;
        if (disk_type == CD_DISC_NONE)
            return 0;
        if (disk_type != CD_DISC_PS2_DVD)
            return CD_CHECK_UNSUPPORTED;
        CD_READ_CONTROL->disk_flags = 0;
        if (xglCdGetFileSize(xenosagaArc10Name) > 0)
            CD_READ_CONTROL->disk_flags |= CD_DISC_FLAG_DISC1;
        if (xglCdGetFileSize(xenosagaArc20Name) > 0)
            CD_READ_CONTROL->disk_flags |= CD_DISC_FLAG_DISC2;
        if (xglCdGetFileSize(xenosagaArc30Name) > 0)
            CD_READ_CONTROL->disk_flags |= CD_DISC_FLAG_DISC3;
        CD_READ_CONTROL->disk_state = 1;
    } else if (sceCdStatus() == state) {
        CD_READ_CONTROL->disk_state = 0;
        return CD_CHECK_SHELL_OPEN;
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

/*
 * Walks one archive's directory records looking for path. On a match, fills
 * entry's file location, stored and size byte counts, and packed flag, and
 * returns 1.
 */
static int xglCdGetFilePosSub(CdArchiveEntry *entry, const char *path)
{
    char name[CD_PATH_MAX];
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
        packed = flags & ARC_RECORD_TYPE_EXTENDED;
        length = flags & ARC_RECORD_LENGTH_MASK;
        if ((flags & ARC_RECORD_EXTENDED) != 0) {
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

/*
 * Resolves name to a position on the disc, a host file or an HDD file.
 * Archive entries are searched first. Returns position, or 0 when the file
 * cannot be found.
 */
static CdFilePosition *xglCdGetFilePos(CdFilePosition *position, const char *name,
                                       CdCompletionCallback *callback)
{
    char buffer[CD_PATH_MAX];
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
    if (path[0] != '\\') {
        for (scan = (char *)path; *scan != 0; scan++) {
            if (*scan == '/')
                *scan = '\\';
        }
    }
    do {
        entry = &ArcHeader[0].entry;
        if (entry->state != ARC_ENTRY_UNUSED && xglCdGetFilePosSub(entry, name) != 0) {
            name = arcname_1;
            position->source = entry->state;
            break;
        }
        entry = &ArcHeader[1].entry;
        if (entry->state != ARC_ENTRY_UNUSED && xglCdGetFilePosSub(entry, name) != 0) {
            name = arcname_2;
            position->source = entry->state;
        } else {
            entry++;
            if (entry->state != ARC_ENTRY_UNUSED && xglCdGetFilePosSub(entry, name) != 0) {
                name = arcname_3;
                position->source = entry->state;
            } else {
                entry++;
                if (entry->state != ARC_ENTRY_UNUSED && xglCdGetFilePosSub(entry, name) != 0) {
                    name = arcname_4;
                    position->source = entry->state;
                } else {
                    entry = 0;
                }
            }
        }
    } while (0);
    /* Absolute names are copied as they are; others get the prefix of their source. */
    if (name[0] == '\\') {
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
        case CD_SOURCE_DISC:
            buffer[0] = '\\';
            destination = buffer + 1;
            for (cursor = name; *cursor != 0; cursor++, destination++) {
                if (*cursor < 'a')
                    *destination = *cursor;
                else
                    *destination = *cursor - ('a' - 'A');
            }
            destination[0] = ';';
            destination[1] = '1';
            destination[2] = 0;
            break;
        case CD_SOURCE_HOST:
        case CD_SOURCE_HDD: {
            if (position->source == CD_SOURCE_HOST)
                cursor = vcd_head_5;
            else
                cursor = hdd_head_6;
            while ((*destination = *cursor) != 0) {
                cursor++;
                destination++;
            }
            cursor = name;
            for (;;) {
                if (*cursor == '\\')
                    *destination = '/';
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
    callback(CD_EVENT_LOOKUP, (int)path);
    /* Fill the position from the archive entry, or from the file itself when no entry matched. */
    switch (position->source) {
    case CD_SOURCE_DISC:
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
    case CD_SOURCE_HDD:
        if (xglHddCheck() < 0)
            return xglCdGetFilePos(position, path, callback);
    case CD_SOURCE_HOST:
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
            header = (u8 *)(((u32)WorkEnd + ARC_TABLE_ALIGN_BYTES - 1) & -ARC_TABLE_ALIGN_BYTES);
            sceRead(descriptor, header, 16);
            if (*(int *)header == CD_PACKED_MAGIC && *(int *)(header + 12) == 0) {
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
        offset = (long)position->lbn << 11;
        sceLseek(descriptor, 0, 0);
        while (offset > 0x40000000) {
            sceLseek(descriptor, 0x40000000, 1);
            offset -= 0x40000000;
        }
        sceLseek(descriptor, offset, 1);
        position->descriptor = descriptor;
        break;
    }
    return position;
}

int xglCdGetFileData(const char *path, CdFilePosition *file_position)
{
    return (xglCdGetFilePos(file_position, path, xglCdDummyCallback) != 0) ? 0 : -1;
}

/* Returns the size of path in bytes, or -1 when it cannot be found. */
int xglCdGetFileSize(const char *path)
{
    CdFilePosition position;

    if (xglCdGetFilePos(&position, path, xglCdDummyCallback) == 0)
        return -1;
    switch (position.source) {
    case CD_SOURCE_DISC:
        break;
    case CD_SOURCE_HOST:
    case CD_SOURCE_HDD:
        sceClose(position.descriptor);
        break;
    }
    return position.size_bytes;
}

/*
 * Reads length bytes of name, starting offset bytes into the file, into
 * buffer. Bit 0 of mode makes the read asynchronous; while another read is
 * active, asynchronous requests are queued. Returns 0 when queued, a negative
 * value when the read cannot start, or the file size otherwise.
 */
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
        if (CD_READ_CONTROL->dispatch_state != CD_STATE_IDLE) {
            if (CD_READ_CONTROL->dispatch_state != CD_STATE_STREAM) {
                request = &CD_READ_CONTROL->requests[CD_READ_CONTROL->queue_tail];
                source = name;
                destination = request->name;
                while ((*destination++ = *source++) != 0)
                    ;
                request->buffer = buffer;
                request->offset = offset;
                request->length = length;
                request->completion_callback = notify;
                CD_READ_CONTROL->queue_tail = (CD_READ_CONTROL->queue_tail + 1) & CD_QUEUE_MASK;
                return 0;
            }
        }
    }
    if (CD_READ_CONTROL->dispatch_state != CD_STATE_IDLE)
        return -3;
    if (xglCdGetFilePos(&position, name, notify) == 0)
        return -1;
    position.lbn += offset >> 11;
    if (length > 0)
        position.stored_bytes = length;
    notify(CD_EVENT_SIZE, (position.stored_bytes + CD_SECTOR_BYTES - 1) & -CD_SECTOR_BYTES);
    CD_READ_CONTROL->completion_callback = notify;
    CD_READ_CONTROL->extract_destination = buffer;
    CD_READ_CONTROL->extract_byte_count = (position.stored_bytes + CD_SECTOR_BYTES - 1) & -CD_SECTOR_BYTES;
    if (position.packed == 0 || (mode & 2) != 0)
        CD_READ_CONTROL->extract_source = buffer;
    else
        CD_READ_CONTROL->extract_source = (u8 *)buffer + ((position.size_bytes + CD_SECTOR_BYTES - 1) & -CD_SECTOR_BYTES)
            - CD_READ_CONTROL->extract_byte_count;
    CD_READ_CONTROL->read_poll_mode = position.source;
    /* Start the transfer on the source the path resolved to. */
    switch (position.source) {
    case CD_SOURCE_DISC:
    retry_cd:
        sceCdSync(0);
        sceCdDiskReady(0);
        read_mode.try_count = 0;
        read_mode.spindle_control = CD_READ_CONTROL->spindle_control;
        read_mode.data_pattern = 0;
        sceCdRead(position.lbn, CD_READ_CONTROL->extract_byte_count / CD_SECTOR_BYTES, CD_READ_CONTROL->extract_source,
                  &read_mode);
        if (asynchronous != 0) {
            CD_READ_CONTROL->dispatch_state = CD_STATE_ASYNC_READ;
            return position.size_bytes;
        }
        CD_READ_CONTROL->dispatch_state = CD_STATE_SYNC_READ;
        while (sceCdSync(1) > 0)
            notify(CD_EVENT_PROGRESS, sceCdGetReadPos());
        result = sceCdGetError();
        if ((unsigned int)result >= 2) {
            notify(CD_EVENT_READ_ERROR, result);
            goto retry_cd;
        }
        break;
    case CD_SOURCE_HOST:
    case CD_SOURCE_HDD:
        CD_READ_CONTROL->active_file_descriptor = position.descriptor;
        CD_READ_CONTROL->write_cursor = CD_READ_CONTROL->extract_source;
        CD_READ_CONTROL->remaining_bytes = position.stored_bytes;
        if (asynchronous != 0) {
            CD_READ_CONTROL->dispatch_state = CD_STATE_ASYNC_READ;
            return position.size_bytes;
        }
        CD_READ_CONTROL->dispatch_state = CD_STATE_SYNC_READ;
        /* Host/HDD files are read synchronously in chunks of at most 64 KiB;
         * a failed read reports the HDD error and restarts the request. */
        do {
            if (CD_READ_CONTROL->remaining_bytes > CD_FILE_CHUNK_BYTES - 1)
                result = sceRead(CD_READ_CONTROL->active_file_descriptor,
                                 CD_READ_CONTROL->write_cursor, CD_FILE_CHUNK_BYTES);
            else
                result = sceRead(CD_READ_CONTROL->active_file_descriptor,
                                 CD_READ_CONTROL->write_cursor,
                                 CD_READ_CONTROL->remaining_bytes);
            if (result < 0) {
                hdd_error();
                CD_READ_CONTROL->dispatch_state = CD_STATE_IDLE;
                return xglCdReadFilePart(name, buffer, mode, notify, offset, length);
            }
            CD_READ_CONTROL->write_cursor += result;
            CD_READ_CONTROL->remaining_bytes -= result;
            notify(CD_EVENT_PROGRESS, position.stored_bytes - CD_READ_CONTROL->remaining_bytes);
        } while (CD_READ_CONTROL->remaining_bytes > 0);
        partial = result & CD_SECTOR_MASK;
        if (partial != 0) {
            padding = -partial & CD_SECTOR_MASK;
            while (padding > 0) {
                *CD_READ_CONTROL->write_cursor++ = padding;
                padding--;
            }
        }
        sceClose(CD_READ_CONTROL->active_file_descriptor);
        CD_READ_CONTROL->active_file_descriptor = -1;
        break;
    }
    /* Mode bit 1 skips the extract step and leaves the data as read. */
    if ((mode & 2) == 0)
        extract(CD_READ_CONTROL->extract_destination, CD_READ_CONTROL->extract_source,
                CD_READ_CONTROL->extract_byte_count);
    notify(CD_EVENT_DONE, 0);
    CD_READ_CONTROL->dispatch_state = CD_STATE_IDLE;
    return (position.size_bytes + CD_SECTOR_BYTES - 1) & -CD_SECTOR_BYTES;
}

int xglCdReadFile(const char *name, void *buffer, int mode, int flags)
{
    return xglCdReadFilePart(name, buffer, mode, (CdCompletionCallback *)flags, 0, -1);
}

/* Cancels the active read and empties the request queue. */
void xglCdReadCancel(void)
{
    LW.queue_tail = LW.queue_cursor;
    if (LW.dispatch_state == CD_STATE_SYNC_READ) {
        switch (LW.read_poll_mode) {
        case CD_SOURCE_DISC:
            sceCdBreak();
            sceCdPause();
            break;
        case CD_SOURCE_HOST:
        case CD_SOURCE_HDD:
            LW.remaining_bytes = 0;
            break;
        }
    } else if (LW.dispatch_state == CD_STATE_ASYNC_READ) {
        switch (LW.read_poll_mode) {
        case CD_SOURCE_DISC:
            sceCdBreak();
            sceCdPause();
            break;
        case CD_SOURCE_HOST:
        case CD_SOURCE_HDD:
            if (LW.active_file_descriptor >= 0) {
                sceClose(LW.active_file_descriptor);
                LW.active_file_descriptor = -1;
            }
            break;
        }
    }
    LW.dispatch_state = CD_STATE_IDLE;
}

int xglCdSync(void)
{
    return LW.dispatch_state != CD_STATE_IDLE;
}

/*
 * Opens path as the source of stream. Disc streams are started with the
 * drive's streaming API. The stream is registered in StrList when it has a
 * ring buffer. Returns 0, or -1 when the path cannot be found.
 */
int xglCdStreamOpen(CdStreamParam *stream, const char *path)
{
    CdFilePosition position;
    CdReadMode read_mode;
    CdStreamParam **slot;
    int index;
    int mode;

    LW.dispatch_state = CD_STATE_STREAM;
    if (xglCdGetFilePos(&position, path, xglCdDummyCallback) == 0) {
        LW.dispatch_state = CD_STATE_IDLE;
        return -1;
    }
    stream->mode = position.source;
    stream->file_byte_count = position.stored_bytes;
    mode = position.source;
    switch (mode) {
    case CD_SOURCE_DISC:
        sceCdSync(0);
        sceCdDiskReady(0);
        stream->descriptor = position.lbn;
        stream->iop_buffer = sceSifAllocIopHeap((stream->sector_count << 11) + CD_STREAM_IOP_ALIGN);
        sceCdStInit(stream->sector_count, stream->block_count,
                    (stream->iop_buffer + CD_STREAM_IOP_ALIGN - 1) & -CD_STREAM_IOP_ALIGN);
        read_mode.try_count = 0;
        read_mode.spindle_control = 0;
        read_mode.data_pattern = 0;
        sceCdStStart(stream->descriptor, &read_mode);
        break;
    case CD_SOURCE_HOST:
    case CD_SOURCE_HDD:
        stream->descriptor = position.descriptor;
        break;
    }
    stream->remaining = stream->file_byte_count;
    stream->ring_size = CD_SECTOR_BYTES;
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

/*
 * Copies up to bytes of stream data into buffer through the ring. Bytes up to
 * the next ring_size boundary are zero-filled. Returns the byte count,
 * including that padding.
 */
int xglCdStreamRead(CdStreamParam *stream, unsigned char *buffer, int bytes)
{
    int bytes_read;
    int remaining;
    int pad;

    /* bytes is a byte count; StreamReadRingCoreSub takes a sector count, so
     * it is divided by CD_SECTOR_BYTES (2^11) here. */
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
     * sector count to a byte count with a left shift.
     */
    enum { CD_SECTOR_BYTE_SHIFT = 11 };
    int read_error;
    int mode;
    int result = 0;

    mode = stream->mode;
    switch (mode) {
    case CD_SOURCE_DISC:
        result = sceCdStRead(sectors, buffer, 1, &read_error);
        result <<= CD_SECTOR_BYTE_SHIFT;
        break;
    case CD_SOURCE_HOST:
    case CD_SOURCE_HDD:
        result = sceRead(stream->descriptor, buffer, sectors << CD_SECTOR_BYTE_SHIFT);
        break;
    }
    return result;
}

/*
 * Moves stream data from the source into the ring buffer, up to the wrap
 * point. When the stream ends inside a sector, the rest of that ring block is
 * zero-filled.
 */
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
            sectors[0] = (stream->ring.read_position - stream->ring.write_position) / CD_SECTOR_BYTES;
        } else {
            sectors[0] = (stream->ring.capacity - stream->ring.write_position) / CD_SECTOR_BYTES;
            sectors[1] = stream->ring.read_position / CD_SECTOR_BYTES;
        }
        available = 0;
        mode = stream->mode;
        switch (mode) {
        case CD_SOURCE_DISC:
            available = sceCdStStat();
            break;
        case CD_SOURCE_HOST:
        case CD_SOURCE_HDD:
            available = CD_STREAM_FILE_SECTORS;
            break;
        }
        /* Clamp the sector counts to what the source can deliver now. */
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

/*
 * Moves XSS stream data into the main or alternate ring one sector at a time.
 * Each bit of the source flag bytes selects which ring receives the next sector.
 */
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
        case CD_SOURCE_DISC:
            available = sceCdStStat();
            break;
        case CD_SOURCE_HOST:
        case CD_SOURCE_HDD:
            available = CD_STREAM_FILE_SECTORS;
            break;
        }
        while (available > 0 && stream->remaining > 0) {
            if (xss->sectors <= 0) {
                StreamReadRingCoreSub(stream, xss->source, 1);
                xss->position = xss->source;
                xss->value = 0;
                xss->mode = 0;
                xss->sectors = CD_STREAM_XSS_SECTORS;
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
                ring->write_position = (ring->write_position + CD_SECTOR_BYTES) % ring->capacity;
            }
            available--;
            stream->remaining -= CD_SECTOR_BYTES;
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

/*
 * Advances the ring read position by bytes, rounded up to whole sectors, and
 * refills the ring until the stream ends or the ring is full.
 */
int xglCdStreamReadRing(CdStreamParam *stream, int bytes)
{
    int aligned;

    aligned = (bytes + CD_SECTOR_BYTES - 1) & -CD_SECTOR_BYTES;
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
    case CD_SOURCE_DISC:
        sceCdStSeek(stream->descriptor);
        break;
    case CD_SOURCE_HOST:
    case CD_SOURCE_HDD:
        sceLseek(stream->descriptor, 0, 0);
        break;
    }
    return 0;
}

/*
 * Removes stream from StrList and releases its drive or host resources.
 */
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
    case CD_SOURCE_DISC:
        sceCdStStop();
        if (stream->iop_buffer != 0)
            sceSifFreeIopHeap((void *)stream->iop_buffer);
        stream->descriptor = -1;
        break;
    case CD_SOURCE_HOST:
    case CD_SOURCE_HDD:
        sceClose(stream->descriptor);
        break;
    }
    LW.dispatch_state = CD_STATE_IDLE;
    return 0;
}

void xglCdStreamParamInit(CdStreamParam *param)
{
    param->sector_count = CD_STREAM_DEFAULT_SECTORS;
    param->block_count = CD_STREAM_DEFAULT_BLOCKS;
    param->descriptor = -1;
    param->iop_buffer = 0;
    param->state = 0;
    param->xss = 0;
    param->ring.buffer = 0;
}

/*
 * Reloads the file list for the directory of select from its .filelist file.
 * Each newline in the file becomes a NUL terminator, so the list holds one
 * entry per string.
 */
static void FileSelectListReload(CdFileSelect *select)
{
    char path[CD_PATH_MAX];
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

/*
 * Advances the file selector state machine. State 0 loads the list, state 1
 * builds the prefix for the current path and counts the entries under it, and
 * state 3 steps back to the parent directory.
 */
static void FileSelectSub(CdFileSelect *select)
{
    char prefix[CD_PATH_MAX];
    char *src;
    char *dst;
    char *name;
    char *probe;
    char *part;
    char *cursor;
    char *last;
    char first;
    int prefix_start;
    unsigned char character;
    char expected;
    int mismatch;

    switch (select->state) {
    case 0:
    default:
        FileSelectListReload(select);
        select->path[0] = 0;
        select->list_index = 0;
        select->list_position = select->list;
        select->entry_count = 1;
        select->first_row = 0;
        select->state = 1;
    case 1:
        /* Build the prefix of the current path, then find the first list entry under it. */
        src = select->path;
        dst = prefix;
        character = *src;
        if (*src == 0) {
            prefix[0] = '.';
            prefix[1] = ':';
            prefix[2] = 0;
        } else {
            do {
                *dst++ = character;
                src++;
                character = *src;
            } while (*src != 0);
            *dst = 0;
            dst[-1] = ':';
        }
        name = select->list;
        prefix_start = 0;
        while (*name != 0) {
            first = *name;
            if (first == '.')
                name += 2;
            mismatch = 0;
            part = prefix + prefix_start;
            if (*part != 0) {
                if (*name != 0 && *name == *part) {
                    for (;;) {
                        name++;
                        expected = *++part;
                        if (expected == 0)
                            break;
                        if (*name == 0 || *name != expected) {
                            mismatch = 1;
                            break;
                        }
                    }
                } else {
                    mismatch = 1;
                }
            }
            if (!mismatch) {
                do {
                    select->list_position = name + 1;
                } while (0);
                break;
            }
            {
                unsigned char character;

                while ((character = *name++) != 0) {
                    do {
                        character = *name++;
                    } while (character != 0);
                }
            }
        }
        probe = select->list_position;
        select->entry_count = 0;
        if (select->path[0] != 0)
            select->entry_count = 1;
        if (*probe != 0) {
            unsigned char character;

            probe++;
            for (;;) {
                character = *probe++;
                if (character != 0)
                    continue;
                select->entry_count++;
                character = *probe;
                if (character == 0)
                    break;
            }
        }
        select->state = 2;
        break;
    case 2:
        return;
    case 3:
        last = select->path;
        for (cursor = last; *cursor != 0; cursor++) {
            if (*cursor == '/') {
                if (cursor[1] == 0) {
                    *last = 0;
                    break;
                }
                last = cursor + 1;
            }
        }
        *last = 0;
        select->state = 1;
        break;
    }
}

/*
 * Runs one frame of the file selector: applies pad input to the cursor and
 * visible rows, and draws the entries. Returns 2 when Cross is pressed.
 */
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
    if ((PadData.pressed & PAD_CROSS) != 0)
        return 2;
    if (PadData.repeated & PAD_UP)
        select->cursor--;
    if (select->cursor < 0)
        select->cursor = select->entry_count - 1;
    if (select->cursor - select->first_row <= 0)
        select->first_row = select->cursor - 1;
    if (select->first_row < 0)
        select->first_row = 0;
    if (PadData.repeated & PAD_DOWN)
        select->cursor++;
    if (select->entry_count - 1 < select->cursor)
        select->cursor = 0;
    if (select->cursor - select->first_row > select->rows - 2)
        select->first_row = select->cursor - select->rows + 2;
    if ((unsigned int)(select->entry_count - select->rows) < (unsigned int)select->first_row)
        select->first_row = select->entry_count - select->rows;
    /* Circle enters the entry under the cursor, or steps up when the cursor is on the parent row. */
    if (PadData.pressed & PAD_CIRCLE) {
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
    select->print(x, y, fileSelectPathFormat, path);
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
        select->print(x, y, fileSelectUpArrowText);
    for (; index < select->first_row; index++) {
        while (*list++ != 0)
            ;
    }
    for (index = select->first_row; index < end_row; index++) {
        if (index == select->cursor)
            select->print(x + 8, y, fileSelectCursorText);
        if (index == 0 && select->path[0] != 0) {
            select->print(x + 16, y, fileSelectParentText);
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
        select->print(x, y - 8, fileSelectDownArrowText);
    return 0;
}

/*
 * Loads the IOP module IRX named path from the IOP directory of cdrom0, with
 * path upper-cased. args is a double-NUL-terminated argument list; NULL passes
 * no arguments.
 */
void xglCdSifLoadModule(const char *path, const char *args)
{
    char name[CD_PATH_MAX];
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
        if (*src < 'a')
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

/*
 * Loads overlay number overlay (OVnn.OVL) from the disc as an ELF, unless it
 * is already loaded.
 */
void xglCdLoadOverlay(int overlay)
{
    char exec_data[16];
    char name[CD_PATH_MAX];
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

/*
 * Reads from the disc into destination and waits for completion. With
 * sectors == 0 it reads the first sector at the entry LBN; otherwise it reads
 * sectors sectors starting after that first sector.
 */
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

/*
 * Looks up path on the disc, records its location in archive_entry, and
 * reads its header sectors into destination. Returns 0 when the file is not
 * found, otherwise the header sector count plus one.
 */
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

    /* xglCdArcInitSub0 reads only the leading word of its argument as an LBN,
     * so passing the file position's lbn field is enough. */
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

/*
 * Fills ArcHeader from the PFS0 archive header when present, otherwise from
 * the disc, and advances WorkEnd past the archive data it locates.
 */
static void xglCdArcInit(void)
{
    u8 *work = WorkEnd;
    unsigned char header_sectors = 0;
    int descriptor;

    while (xglHddActivate(-1) != 0 && (descriptor = sceOpen(xenosagaPfs0Path, 1)) >= 0) {
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
        if (xglCdArcInitSub1(&ArcHeader[0].entry, xenosagaArc00Name, work) == 0)
            return;
        xglHddActivate(0x100);
    }
    do {
        if (LW.dual_layer_disc != 0) {
            work = xglCdArcInitSub2(work);
            do {
            } while (xglCdArcInitSub1(&ArcHeader[1].entry, xenosagaArc10Name, work) <= 0);
            work = xglCdArcInitSub2(work);
            LW.search_layer = 1;
            do {
            } while (xglCdArcInitSub1(&ArcHeader[2].entry, xenosagaArc20Name, work) <= 0);
            LW.search_layer = 0;
            work += ((xglCdArcInitSub2(work) - work) + CD_SECTOR_BYTES - 1) & -CD_SECTOR_BYTES;
        } else {
            work += CD_ARC_LOOKAHEAD_BYTES;
            while (xglCdArcInitSub1(&ArcHeader[1].entry, xenosagaArc10Name, work) <= 0
                   && xglCdArcInitSub1(&ArcHeader[1].entry, xenosagaArc20Name, work) <= 0) {
                if (xglCdArcInitSub1(&ArcHeader[1].entry, xenosagaArc30Name, work) <= 0) {
                    ArcHeader[1].entry.state = ARC_ENTRY_UNUSED;
                    work[0] = 0;
                    work[1] = 0;
                }
                break;
            }
            work += CD_ARC_HEADER_RESERVE;
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

/*
 * Resets the drive state, copies SYSTEM.CNF into system_cnf, and locates the
 * archives.
 */
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
    ArcHeader[0].entry.state = ARC_ENTRY_UNUSED;
    ArcHeader[1].entry.state = ARC_ENTRY_UNUSED;
    ArcHeader[2].entry.state = ARC_ENTRY_UNUSED;
    ArcHeader[3].entry.state = ARC_ENTRY_UNUSED;
    dst = system_cnf;
    xglCdReadFile(systemConfigName, src, 0, 1);
    for (count = CD_SYSTEM_CNF_SIZE - 1; count >= 0; count--)
        *dst++ = *src++;
    xglHddActivate(0x100);
    xglHddMount();
    xglCdArcInit();
}
