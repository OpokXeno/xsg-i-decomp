#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "main/xgl_thread.h"
#include "xgl_cd.h"

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

/* StrList holds a single global stream slot; queue_next's stream scan below
 * walks it as a one-entry list, as xglCdStreamClose already does. */
#define STR_LIST_LAST_INDEX 0

/*
 * queue_next and xglCdControlThread share LW through this typed view.
 * LW is the shared, absolutely-addressed unsigned-byte array (hdd_error's
 * comment above), so the cast cannot move into LW's own declaration without
 * retyping it for every other user of this TU. A per-function local variable
 * initialized once from this cast was tried and measured to change cc1
 * 2.96's register allocation/scheduling for both functions below (fewer
 * callee-saved registers, reordered address arithmetic), so the cast stays
 * this one named, TU-local macro instead of a declared object.
 */
#define CD_READ_CONTROL ((CdReadControl *)LW)

/* Twelve month lengths followed by twelve cumulative month offsets. */
extern unsigned short monthday[24];

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
    u8 unmodeled_014[0xc];                       /* +0x14..0x1f */
    u8 *extract_source;                          /* +0x20 */
    u8 *extract_destination;                     /* +0x24 */
    int extract_byte_count;                      /* +0x28 */
    u8 unmodeled_02c[0x4];                       /* +0x2c..0x2f */
    signed char dispatch_state;                  /* +0x30 */
    u8 unmodeled_031[0x3];                       /* +0x31..0x33 */
    u8 power_off_pending;                        /* +0x34 */
    u8 unmodeled_035[0x9];                       /* +0x35..0x3d */
    u8 queue_cursor;                             /* +0x3e */
    u8 queue_tail;                               /* +0x3f */
    CdRequestPartial requests[32];               /* +0x40..0x103f */
} CdReadControl;

extern int xglCdReadFilePart(const char *name, void *buffer, int mode,
                             CdCompletionCallback *callback, int offset, int length);
extern int sceDevctl(const char *device, int command, const void *input,
                     unsigned int input_size, void *output, unsigned int output_size);
extern int sceCdPause(void);
extern int sceCdPowerOff(u32 *result);
extern u32 sceCdGetReadPos(void);

/*
 * The descriptor word at LW+0 (hdd_error) is read through the shared
 * unsigned byte view of LW: that incomplete type keeps LW absolutely
 * addressed under this object's -G8, as in the original.
 */
static void hdd_error(void)
{
    int *descriptor = (int *)LW;

    sceClose(*descriptor);
    *descriptor = -1;
    xglHddErrorScreen();
}

void xglCdPowerOffCB(void)
{
    LW[0x34] = 1;
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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdDiskCheck);

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

extern CdCompletionCallback *callback;

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdSetCallback);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdGetFilePosSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdGetFilePos);

int xglCdGetFileData(const char *path, CdFilePosition *file_position)
{
    return (xglCdGetFilePos(file_position, path, xglCdDummyCallback) != 0) ? 0 : -1;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdGetFileSize);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdReadFilePart);

int xglCdReadFile(const char *name, void *buffer, int mode, int flags)
{
    return xglCdReadFilePart(name, buffer, mode, (CdCompletionCallback *)flags, 0, -1);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdReadCancel);

int xglCdSync(void)
{
    /*
     * Byte +0x30 is read signed (lb at 0x0021e404; config/header-canon.json
     * LW byte_required io-main-0021e400); the shared LW is the unsigned view.
     */
    return (signed char)LW[0x30] != 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdStreamOpen);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", StreamReadRingCoreNormal);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", StreamReadRingCoreXss);

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
    LW[0x30] = 0;
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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", FileSelectListReload);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", FileSelectSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdFileSelect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdSifLoadModule);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdLoadOverlay);

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

/* A CD sector is fixed at 2048 bytes on this drive/read path (see also
 * xglCdStreamReadRing's own `& -2048` sector rounding earlier in this TU). */
#define CD_SECTOR_BYTES 2048

static int xglCdArcInitSub1(CdArchiveEntry *archive_entry,
                            const char *path, unsigned char *destination)
{
    CdFilePosition file_position;
    int result;
    int header_count;

    result = xglCdGetFilePos(&file_position, path, xglCdDummyCallback);
    if (result != 0) {
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
        result = header_count + 1;
    }
    return result;
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
#define ARC_RECORD_EXTENDED        0x80
#define ARC_RECORD_TYPE_EXTENDED   0x40
#define ARC_RECORD_LENGTH_MASK     0x3f
#define ARC_RECORD_HEADER_BYTES    7
#define ARC_RECORD_TYPE_EXTRA_BYTES 3
#define ARC_TABLE_ALIGN_BYTES      64

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdArcInit);

/*
 * ArcHeader is a four-entry table of 0x18-byte archive records (xglCdInitial
 * clears the state byte of entry 0..3 at +0x00/+0x18/+0x30/+0x48).  Only the
 * first two words of entry 0 are evidenced here: +0x00 is the state byte
 * (xglCdArcInit stores 2 there, xglCdInitial -1) and +0x04 is the work-buffer
 * pointer xglCdArcInit read the archive into (`sw WorkEnd, 0x4($18)` at
 * 0x0021F85C).  The rest of the record is not recovered, so the entry cannot
 * become a struct without inventing the span between them.
 */
#define ARC_HEADER_WORK_END_OFFSET 4

void xglCdArcCheck(void)
{
    u8 *saved_work_end = WorkEnd;

    WorkEnd = *(u8 **)(ArcHeader + ARC_HEADER_WORK_END_OFFSET);
    xglCdArcInit();
    WorkEnd = saved_work_end;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdInitial);
