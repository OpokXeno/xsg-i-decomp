#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "xgl_cd.h"

/*
 * File-local functions (LOCAL in the original symbol table) that the shared
 * header declares `extern`: declare them static first so their static
 * definitions below do not follow a non-static declaration.
 */
static int BCD2INT(unsigned char x);

/* Twelve month lengths followed by twelve cumulative month offsets. */
extern unsigned short monthday[24];

static void StreamReadRingCoreNormal(CdStreamParam *stream);
static void StreamReadRingCoreXss(CdStreamParam *stream);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", queue_next);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdControlThread);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdReset);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdSetCallback);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdGetFilePosSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdGetFilePos);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdGetFileData);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdGetFileSize);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdReadFilePart);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdReadFile);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdStreamRead);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", StreamReadRingCoreSub);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdArcInitSub1);

INCLUDE_ASM("asm/main/nonmatchings/xgl_cd", xglCdArcInitSub2);

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
