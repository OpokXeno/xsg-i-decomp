#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "xgl_mc.h"

/* The request routines zero and use this complete memory-card save workspace. */
unsigned char SaveData[0x163B0] = { 0 };

/* The state machine uses both a word and per-slot halfwords at offset 4. */
typedef struct McRequestState {
    unsigned char phase;
    unsigned char subphase;
    unsigned char unmodeled_02;
    unsigned char next_card;
    union {
        int combined;
        short slots[2];
    } status;
    int descriptor;
    int card_type;
    unsigned char unmodeled_10[4];
    int formatted;
    unsigned char unmodeled_18[8];
} McRequestState;

/* State and its 0x60-byte full-path buffer share the original workspace. */
typedef struct McRequestStorage {
    McRequestState state;
    unsigned char fullPath[0x60];
} McRequestStorage;
static McRequestStorage mw;

static unsigned char queue_top;
static unsigned char queue_end;

static unsigned char mapNameBuffer[17 * 2];
static unsigned char base_0_00491770[18];
static unsigned char n_1[10];
static unsigned char n_2[14];

int sceMcOpen(int port, int slot, const char *name, int mode);
int sceMcClose(int fd);
int sceMcRead(int fd, void *buffer, int size);
int sceMcWrite(int fd, const void *buffer, int size);
int sceMcSync(int mode, int *command, int *result);
int sceMcGetInfo(int port, int slot, int *type, int *free, int *format);

/* One queued memory-card request; the request routines fill the queue slot. */

/* The caller's request parameters: file slot, name, data and size. */

typedef union McRequestPath {
    int empty;
    unsigned char bytes[108];
} McRequestPath;

typedef struct McRequestArguments {
    int slot;
    const unsigned char *path;
    void *buffer;
    int size;
} McRequestArguments;

typedef struct McRequestEntry {
    short card;
    short operation;
    int *result;
    int slot;
    int buffer;
    int size;
    McRequestPath path;
} McRequestEntry;

static McRequestEntry queue[8];

int sceMcGetDir(int port, int slot, unsigned char *entries, int mode,
                int max_entries, int reserved);

int sceMcMkdir(int port, int slot, unsigned char *path);

int sceMcFormat(int port, int slot);

int sceMcUnformat(int port, int slot);

static void execute(McRequestEntry *request);

static int create_sub2(int result);

unsigned char *xglMcSetFullPath(int card, int selector);

void xglMcWriteMapName(char *buffer, int slot);

typedef struct McSavePayload {
    unsigned char unmodeled_00[4];
    int entry_count;
    unsigned char unmodeled_08[8];
    int group_count;
} McSavePayload;

typedef struct McSaveHeader {
    unsigned char unmodeled_00[4];
    int start;
    int end;
    unsigned char unmodeled_0c[0x34];
    char icon_header[0x400];
    McSavePayload payload;
} McSaveHeader;

typedef struct McSaveChecksum {
    unsigned char unmodeled_00[28];
    float checksum;
} McSaveChecksum;

typedef struct McWorkspaceView {
    McRequestState state;
    unsigned char full_path[96];
    unsigned char map_name[36];
    McRequestEntry request;
} McWorkspaceView;

extern float checksumNoiseScale;

float xglFRand(void);

void FlushCache(int mode);

int sceMcDelete(int port, int slot, const char *path);

extern int mcMainStatus[];

static McRequestEntry savedMcRequest;

typedef struct McDirectoryEntry {
    unsigned char unmodeled_00[9];
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
    unsigned char unmodeled_10[32];
    unsigned char name[16];
} McDirectoryEntry;

int xglMcRequest(int card, int operation, const McRequestArguments *arguments,
                 int *result)
{
    int entry_index = queue_end;
    McRequestEntry *entry;
    int copied;
    signed char *destination;
    const unsigned char *source;
    const unsigned char *path;

    *result = 0;
    entry = &queue[entry_index];
    entry->card = card;
    entry->result = result;
    entry->operation = operation;
    switch (operation)
    {
    case 1:
    case 9:
    case 10:
        break;
    case 2:
    case 4:
    case 6:
    case 8:
        entry->slot = arguments->slot;
        entry->buffer = (int)arguments->buffer;
        entry->size = arguments->size;
        break;
    case 3:
    case 5:
    case 7:
        entry->slot = arguments->slot;
        entry->buffer = (int)arguments->buffer;
        entry->size = arguments->size;
        path = arguments->path;
        if (path == 0)
        {
            entry->path.empty = 0;
        }
        else
        {
            source = path;
            destination = (signed char *)entry->path.bytes;
            copied = 0;
            if ((*destination = *source) != 0)
            {
                do
                {
                    copied++;
                    source++;
                    destination++;
                    if (copied >= 108)
                        break;
                } while ((*destination = *source) != 0);
            }
        }
        break;
    }
    queue_end = (entry_index + 1) & 7;
    return 0;
}

int xglMcGetState(void)
{
    return mw.state.status.combined;
}

void xglMcReset(void)
{
    queue_top = 0;
    queue_end = 0;
    xglMcSetMapName(0, 0);
    mw.state.phase = 0;
}

/*
 * Converts one EUC-JIS double-byte character to Shift-JIS in place: hi and
 * lo hold the character's lead and trail byte, EUC-encoded on entry and
 * Shift-JIS on return. Called from xglMcSetMapName while building a memory
 * card map name.
 */
static void xglMcEUC2SJIS(u8 *hi, u8 *lo)
{
    u32 hi_byte;
    int lo_byte;
    u8 sjis_hi;
    u8 sjis_lo;

    hi_byte = ((*hi) + 0x80) & 0xFF;
    lo_byte = ((*lo) + 0x80) & 0xFF;
    sjis_hi = hi_byte;
    if (sjis_hi & 1)
    {
        lo_byte = lo_byte + 0x1F;
        sjis_hi = (sjis_hi >> 1) + 0x71;
    }
    else
    {
        lo_byte = lo_byte + 0x7D;
        sjis_hi = (sjis_hi >> 1) + 0x70;
    }
    sjis_lo = lo_byte & 0xFF;
    if (sjis_hi >= 0xA0U)
    {
        sjis_hi = (sjis_hi + 0x40) & 0xFF;
    }
    if (sjis_lo >= 0x7FU)
    {
        sjis_lo = (sjis_lo + 1) & 0xFF;
    }
    if ((sjis_hi == 0x87) && (sjis_lo == 0x54))
    {
        sjis_hi = 0x82;
        sjis_lo = 0x50;
    }
    *hi = sjis_hi;
    *lo = sjis_lo;
}

void xglMcSetMapName(const unsigned char *primary_euc_name,
                     const unsigned char *secondary_euc_name)
{
    const unsigned char *primary;
    unsigned char *destination;
    int character_count;
    unsigned char lead;
    unsigned char trail;

    primary = primary_euc_name;
    destination = mapNameBuffer;
    character_count = 0;

    if (primary != 0)
    {
        lead = primary[0];
        if (lead != 0)
        {
            do
            {
                trail = primary[1];
                primary += 2;
                xglMcEUC2SJIS(&lead, &trail);
                character_count++;
                destination[0] = lead;
                destination[1] = trail;
                destination += 2;
                if (character_count >= 16)
                {
                    break;
                }
                lead = ((const volatile unsigned char *)primary)[0];
            } while (lead != 0);
        }

        if ((character_count < 16) && (secondary_euc_name != 0) &&
            (((const signed char *)secondary_euc_name)[0] != 0))
        {
            destination[0] = 0x81;
            destination[1] = 0x45;
            destination += 2;
            character_count++;
        }
    }

    if ((secondary_euc_name != 0) && (character_count < 16))
    {
        lead = secondary_euc_name[0];
        if (lead != 0)
        {
            do
            {
                trail = secondary_euc_name[1];
                secondary_euc_name += 2;
                xglMcEUC2SJIS(&lead, &trail);
                character_count++;
                destination[0] = lead;
                destination[1] = trail;
                destination += 2;
                if (character_count >= 16)
                {
                    break;
                }
                lead = ((const unsigned char *)secondary_euc_name)[0];
            } while (lead != 0);
        }
    }

    while (character_count < 17)
    {
        destination[0] = 0;
        destination[1] = 0;
        destination += 2;
        character_count++;
    }
}

void xglMcWriteMapName(char *buffer, int slot)
{
    int value = slot + 1;
    int digit = value % 10;

    buffer[229] = digit + 'O';
    value /= 10;
    buffer[227] = value % 10 + 'O';
}

unsigned char *xglMcSetFullPath(int card, int selector)
{
    signed char *dest;
    const unsigned char *source;
    const signed char *prefix_end;

    dest = (signed char *)mw.fullPath;
    source = base_0_00491770;
    while ((*dest = *source) != 0)
    {
        dest++;
        source++;
    }

    if (card < 0)
    {
        *dest = '*';
        dest++;
    }
    else
    {
        dest[1] = '0' + card % 10;
        dest[0] = '0' + (card / 10) % 10;
        dest += 2;
    }

    switch (selector)
    {
    case 0:
        prefix_end = dest;
        for (source = mw.fullPath; source != (const unsigned char *)prefix_end; source++)
        {
            *dest = *source;
            dest++;
        }
        *dest = 0;
        break;
    case 1:
        {
            source = n_1;
        }
        while ((*dest = *source) != 0)
        {
            dest++;
            source++;
        }
        break;
    case 2:
        {
            source = n_2;
        }
        while ((*dest = *source) != 0)
        {
            dest++;
            source++;
        }
        break;
    case -1:
        *dest = 0;
        break;
    default:
        *dest = '/';
        dest++;
        source = (const unsigned char *)selector;
        while ((*dest = *source) != 0)
        {
            dest++;
            source++;
        }
        break;
    }
    return mw.fullPath;
}

static void execute(McRequestEntry *request)
{
    int command = request->operation;

    switch (command)
    {
    case 1:
        sceMcGetInfo(request->card, 0, 0, request->result, 0);
        return;
    case 2:
        if (request->slot == -1)
        {
            xglMcSetFullPath(-1, -1);
            sceMcGetDir(request->card, 0, mw.fullPath, 0, 100,
                        (request->buffer + 0xA3) & ~0x3F);
            mw.state.phase = 6;
            return;
        }
        xglMcSetFullPath(request->slot, 0);
        sceMcOpen(request->card, 0, (const char *)mw.fullPath, 1);
        mw.state.phase = 2;
        return;
    case 3:
        xglMcSetFullPath(0, (int)&request->path.empty);
        sceMcOpen(request->card, 0, (const char *)mw.fullPath, 1);
        mw.state.phase = 2;
        return;
    case 5:
    case 7:
        xglMcSetFullPath(0, (int)&request->path.empty);
        if (request->operation == 7)
        {
            sceMcOpen(request->card, 0, (const char *)mw.fullPath, 0x202);
            mw.state.phase = 3;
        }
        else
        {
            sceMcOpen(request->card, 0, (const char *)mw.fullPath, 1);
            mw.state.phase = 4;
        }
        mw.state.subphase = 0;
        return;
    case 8:
        xglMcSetFullPath(request->slot, -1);
        sceMcMkdir(request->card, 0, mw.fullPath);
        mw.state.subphase = 0;
        mw.state.phase = 5;
        return;
    case 6:
        xglMcSetFullPath(request->slot, 0);
        sceMcOpen(request->card, 0, (const char *)mw.fullPath, 0x202);
        mw.state.subphase = 0;
        mw.state.phase = 3;
        return;
    case 4:
        xglMcSetFullPath(request->slot, 0);
        sceMcOpen(request->card, 0, (const char *)mw.fullPath, 1);
        mw.state.subphase = 0;
        mw.state.phase = 4;
        return;
    case 9:
        sceMcFormat(request->card, 0);
        return;
    case 10:
        if (1)
        {
            sceMcUnformat(request->card, 0);
        }
        if (request)
        {
            return;
        }
        else
        {
            return;
        }
    default:
        return;
    }
}

static int create_sub2(int result)
{
    int slot = ((McWorkspaceView *)&mw)->request.slot;
    McSaveHeader *save;
    McSavePayload *payload;
    McSaveChecksum *trailer;
    float *checksum;
    int size;
    unsigned char *write_data;

    if (mw.state.subphase < 2 && (result >= 0 || result == -4)) {
        if (mw.state.subphase == 0) {
            xglMcSetFullPath(slot, 0);
            sceMcDelete(((McWorkspaceView *)&mw)->request.card, 0, (char *)((McWorkspaceView *)&mw)->full_path);
        } else {
            xglMcSetFullPath(slot, 1);
            sceMcOpen(((McWorkspaceView *)&mw)->request.card, 0, (char *)&mw + 32, 0x202);
        }
    } else {
        if (result < 0)
            goto store_error;
        save = (McSaveHeader *)((McWorkspaceView *)&mw)->request.buffer;
        switch (mw.state.subphase) {
        case 0:
        case 1:
        case 2:
            mw.state.descriptor = result;
            write_data = (unsigned char *)save->icon_header;
            xglMcWriteMapName((char *)write_data, slot);
            sceMcWrite(mw.state.descriptor, write_data, 964);
            break;
        case 3:
            sceMcClose(mw.state.descriptor);
            break;
        case 4:
            xglMcSetFullPath(slot, 2);
            {
                McWorkspaceView *view = (McWorkspaceView *)&mw;
                sceMcOpen(view->request.card, 0, (char *)&mw + 32, 0x202);
            }
            break;
        case 5:
            mw.state.descriptor = result;
            write_data = (unsigned char *)&save->payload;
            payload = (McSavePayload *)write_data;
            trailer = (McSaveChecksum *)((unsigned char *)payload + (payload->entry_count * 8 + 16) * payload->group_count);
            checksum = &trailer->checksum;
            size = save->end - save->start - 40;
            *checksum = (xglFRand() - 1.5f) * checksumNoiseScale + 1.0f;
            FlushCache(0);
            sceMcWrite(mw.state.descriptor, write_data, size);
            break;
        case 6:
            sceMcClose(mw.state.descriptor);
            return 1;
        default:
            break;
        }
    }
    mw.state.subphase++;
    *((McWorkspaceView *)&mw)->request.result = mw.state.subphase;
    return 0;

store_error:
    *((McWorkspaceView *)&mw)->request.result = result;
    return -1;
}

int xglMcMain(void)
{
    XglClock clock;
    int command;
    int result;
    int sync;
    short *status;
    int stored_result;
    unsigned char *slots;
    McDirectoryEntry *directory;
    unsigned int latest;
    unsigned int timestamp;
    int latest_slot;
    int index;
    int slot;
    int created;
    int card_type;
    void *buffer;
    int size;
    unsigned char phase;
    unsigned char card;
    McRequestEntry *request;
    unsigned char *clear_slot;

    sync = sceMcSync(1, &command, &result);
    if (sync == 0) {
        if (mw.state.phase == 1 && queue_end == queue_top)
            return mw.state.status.combined;
        return 0xFF00FF;
    }
    if (sync == -1) {
        result = 0;
        command = 0;
    }
    switch (mw.state.phase) {
    case 1:
        if (queue_end == queue_top) {
            status = &mw.state.status.slots[mw.state.descriptor];
            switch (mw.state.card_type) {
            case 1: *status = 0x81; break;
            case 2: *status = 1; break;
            case 3: *status = 0x82; break;
            default: *status = 0; break;
            }
            if (result == -1 || result == -2) {
                *status = 0x80;
            } else {
                card_type = mw.state.card_type;
                if (card_type == 2 && result == 0 && mw.state.formatted == 0)
                    *status = card_type;
            }
        }
        mw.state.phase = 0;
    case 0:
        if (queue_end != queue_top) {
            request = &queue[queue_top];
            savedMcRequest = *request;
            execute(request);
            queue_top = (queue_top + 1) & 7;
            return 0xFF00FF;
        }
        card = mw.state.next_card & 1;
        mw.state.descriptor = card;
        sceMcGetInfo(card, 0, &mw.state.card_type, 0, &mw.state.formatted);
        mw.state.phase = 1;
        mw.state.next_card ^= 1;
        break;
    case 2:
        *((McWorkspaceView *)&mw)->request.result = result;
        mw.state.phase = 0;
        if (result < 0)
            break;
        sceMcClose(result);
        return 0xFF00FF;
    case 6:
        stored_result = result;
        *((McWorkspaceView *)&mw)->request.result = stored_result;
        slots = (unsigned char *)((McWorkspaceView *)&mw)->request.buffer;
        clear_slot = &slots[100];
        index = 100;
        do {
            index--;
            *clear_slot = 0;
            clear_slot--;
        } while (index >= 0);
        if (result > 0) {
            latest = 0;
            index = 0;
            latest_slot = 0;
            directory = (McDirectoryEntry *)(((unsigned int)((McWorkspaceView *)&mw)->request.buffer + 163) & ~63);
            while (index < result) {
                    slot = directory->name[0] * 10 + directory->name[1] - 528;
                    slots[slot] = 1;
                    clock.year = directory->year;
                    clock.month = directory->month;
                    clock.day = directory->day;
                    clock.hour = directory->hour;
                    clock.minute = directory->minute;
                    clock.second = directory->second;
                    clock.status = 0;
                    timestamp = xglClockDayTime2UInt(&clock);
                    if (latest < timestamp) {
                        latest = timestamp;
                        latest_slot = slot;
                    }
                    index++;
                    directory++;
            }
            slots[100] = latest_slot;
        }
        mw.state.phase = 0;
        break;
    case 5:
        created = create_sub2(result);
        if (created == 0)
            return 0xFF00FF;
        mw.state.phase = 0;
        if (created < 0)
            mw.state.status.combined = 0xFF00FF;
        break;
    case 3:
    case 4:
        if (result >= 0) {
            if (mw.state.subphase == 0) {
                mw.state.descriptor = result;
                phase = mw.state.phase;
                size = ((McWorkspaceView *)&mw)->request.size;
                buffer = (void *)((McWorkspaceView *)&mw)->request.buffer;
                switch (phase) {
                case 3:
                    sceMcWrite(result, buffer, size);
                    break;
                case 4:
                    sceMcRead(result, buffer, size);
                    break;
                }
                mw.state.subphase = 1;
                return 0xFF00FF;
            }
            sceMcClose(mw.state.descriptor);
        } else {
            *((McWorkspaceView *)&mw)->request.result = result;
            mw.state.status.combined = 0xFF00FF;
        }
        mw.state.phase = 0;
        break;
    }
    return mcMainStatus[0];
}

int xglMcEasyLoad(const char *path, void *buffer, int size)
{
    int card_type;
    int command;
    int result;
    int descriptor;
    int read_result;

    sceMcGetInfo(0, 0, 0, 0, &card_type);
    sceMcSync(0, &command, &result);
    if (card_type == 0)
    {
        return -1;
    }
    if (result < -1)
    {
        return result;
    }

    sceMcOpen(0, 0, path, 1);
    sceMcSync(0, &command, &result);
    descriptor = result;
    if (descriptor < 0)
    {
        return descriptor;
    }

    sceMcRead(descriptor, buffer, size);
    sceMcSync(0, &command, &result);
    read_result = result;
    if (read_result < 0)
    {
        return read_result;
    }

    sceMcClose(descriptor);
    sceMcSync(0, &command, &result);
    if (result < 0)
    {
        return result;
    }
    return read_result;
}

int xglMcEasySave(const char *path, const void *buffer, int size)
{
    int card_type;
    int command;
    int result;
    int descriptor;

    sceMcGetInfo(0, 0, 0, 0, &card_type);
    sceMcSync(0, &command, &result);
    if (result < -1 || card_type == 0)
    {
        return -1;
    }

    sceMcOpen(0, 0, path, 0x202);
    sceMcSync(0, &command, &result);
    descriptor = result;
    if (descriptor < 0)
    {
        return -1;
    }

    sceMcWrite(descriptor, buffer, size);
    sceMcSync(0, &command, &result);
    if (result < 0)
    {
        return -1;
    }

    sceMcClose(descriptor);
    sceMcSync(0, &command, &result);
    return result >= 0 ? 0 : -1;
}

void xglMcInitial(void)
{
    sceMcInit();
    xglMcReset();
}

static unsigned char base_0_00491770[18] = "/BASLUS-20469Xeno";

static unsigned char n_1[10] = "/icon.sys";

static unsigned char n_2[14] = "/xenosaga.ico";

float checksumNoiseScale = 0.60000002384185791f;

