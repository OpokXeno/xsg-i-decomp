#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "xgl_mc.h"
#include "main/xgl_mc.h"
#include "main/xgl_2.h"

/* McRequestState.phase: what the next sceMcSync completion belongs to. */
#define MC_PHASE_IDLE 0
#define MC_PHASE_POLL_INFO 1  /* sceMcGetInfo of the polled card */
#define MC_PHASE_OPEN_CLOSE 2 /* open, then close, a file */
#define MC_PHASE_WRITE 3
#define MC_PHASE_READ 4
#define MC_PHASE_CREATE 5     /* create_sub2 steps */
#define MC_PHASE_DIRECTORY 6  /* sceMcGetDir listing */

/* Pending requests: a ring of eight entries. */
#define MC_QUEUE_SIZE 8
#define MC_QUEUE_MASK (MC_QUEUE_SIZE - 1)

/* Longest request path copied into a queue entry. */
#define MC_PATH_MAX 108

/* Map names are 16 double-byte characters plus a double-byte terminator. */
#define MC_MAP_NAME_CHARS 16

/* sceMcGetDir entry limit for the save directory scan. */
#define MC_DIRECTORY_MAX_ENTRIES 100

/* sceMcOpen modes (SCE_RDONLY, SCE_WRONLY | SCE_CREAT). */
#define MC_OPEN_READ 1
#define MC_OPEN_WRITE_CREATE 0x202

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

/* The state and its 0x60-byte full-path buffer are stored together. */
typedef struct McRequestStorage {
    McRequestState state;
    char fullPath[0x60];
} McRequestStorage;

typedef union McRequestPath {
    int empty;
    char bytes[MC_PATH_MAX];
} McRequestPath;

/* The caller's request parameters: file slot, name, data and size. */
typedef struct McRequestArguments {
    int slot;
    const unsigned char *path;
    void *buffer;
    int size;
} McRequestArguments;

/* One queued memory-card request; the request routines fill the queue slot. */
typedef struct McRequestEntry {
    short card;
    short operation;
    int *result;
    int slot;
    int buffer;
    int size;
    McRequestPath path;
} McRequestEntry;

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
    char full_path[96];
    unsigned char map_name[36];
    McRequestEntry request;
} McWorkspaceView;

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

extern float checksumNoiseScale;
extern int mcMainStatus[];

int sceMcOpen(int port, int slot, const char *name, int mode);
int sceMcClose(int fd);
int sceMcRead(int fd, void *buffer, int size);
int sceMcWrite(int fd, const void *buffer, int size);
int sceMcSync(int mode, int *command, int *result);
int sceMcGetInfo(int port, int slot, int *type, int *free, int *format);

int sceMcGetDir(int port, int slot, const char *name, int mode,
                int max_entries, int reserved);

int sceMcMkdir(int port, int slot, const char *path);
int sceMcFormat(int port, int slot);
int sceMcUnformat(int port, int slot);
static void execute(McRequestEntry *request);
static int create_sub2(int result);
void FlushCache(int mode);
int sceMcDelete(int port, int slot, const char *path);

/* The complete save-data image, MC_SAVE_DATA_SIZE bytes, shared with the game's save code. */
unsigned char SaveData[MC_SAVE_DATA_SIZE] = { 0 };

static char base_0_00491770[18];

static char n_1[10];

static char n_2[14];

static char base_0_00491770[18] = "/BASLUS-20469Xeno";

static char n_1[10] = "/icon.sys";

static char n_2[14] = "/xenosaga.ico";

static McRequestStorage mw;

static unsigned char queue_top;

static unsigned char queue_end;

static unsigned char mapNameBuffer[(MC_MAP_NAME_CHARS + 1) * 2];

static McRequestEntry queue[MC_QUEUE_SIZE];

static McRequestEntry savedMcRequest;

/*
 * Queues a memory-card request for card in the next free slot and copies its
 * arguments there; result receives the outcome. Paths are copied up to
 * MC_PATH_MAX bytes.
 */
int xglMcRequest(int card, int operation, const McRequestArguments *arguments,
                 int *result)
{
    int entry_index = queue_end;
    McRequestEntry *entry;
    int copied;
    char *destination;
    const unsigned char *source;
    const unsigned char *path;

    *result = 0;
    entry = &queue[entry_index];
    entry->card = card;
    entry->result = result;
    entry->operation = operation;
    switch (operation)
    {
    case MC_REQUEST_FREE_SPACE:
    case MC_REQUEST_FORMAT:
    case MC_REQUEST_UNFORMAT:
        break;
    case MC_REQUEST_DIRECTORY:
    case MC_REQUEST_LOAD:
    case MC_REQUEST_SAVE:
    case MC_REQUEST_CREATE:
        entry->slot = arguments->slot;
        entry->buffer = (int)arguments->buffer;
        entry->size = arguments->size;
        break;
    case MC_REQUEST_PATH_EXISTS:
    case MC_REQUEST_LOAD_PATH:
    case MC_REQUEST_SAVE_PATH:
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
            destination = entry->path.bytes;
            copied = 0;
            if ((*destination = *source) != 0)
            {
                do
                {
                    copied++;
                    source++;
                    destination++;
                    if (copied >= MC_PATH_MAX)
                        break;
                } while ((*destination = *source) != 0);
            }
        }
        break;
    }
    queue_end = (entry_index + 1) & MC_QUEUE_MASK;
    return 0;
}

int xglMcGetState(void)
{
    return mw.state.status.combined;
}

/* Clears the request queue and the request phase, and resets the map name. */
void xglMcReset(void)
{
    queue_top = 0;
    queue_end = 0;
    xglMcSetMapName(0, 0);
    mw.state.phase = MC_PHASE_IDLE;
}

/*
 * Converts one EUC-JP double-byte character to Shift-JIS in place: hi and
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

/*
 * Fills mapNameBuffer with the Shift-JIS map name made from the two EUC-JP names:
 * the primary name, a 0x8145 separator when the secondary name also fits, the
 * secondary name, then zero characters up to MC_MAP_NAME_CHARS + 1 entries.
 */
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

    /* Primary name, converted up to the character limit. */
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
                if (character_count >= MC_MAP_NAME_CHARS)
                {
                    break;
                }
                lead = ((const volatile unsigned char *)primary)[0];
            } while (lead != 0);
        }

        if ((character_count < MC_MAP_NAME_CHARS) && (secondary_euc_name != 0) &&
            (((const signed char *)secondary_euc_name)[0] != 0))
        {
            destination[0] = 0x81;
            destination[1] = 0x45;
            destination += 2;
            character_count++;
        }
    }

    /* Secondary name, converted into the room left. */
    if ((secondary_euc_name != 0) && (character_count < MC_MAP_NAME_CHARS))
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
                if (character_count >= MC_MAP_NAME_CHARS)
                {
                    break;
                }
                lead = secondary_euc_name[0];
            } while (lead != 0);
        }
    }

    /* Zero-fill the rest of the buffer. */
    while (character_count < MC_MAP_NAME_CHARS + 1)
    {
        destination[0] = 0;
        destination[1] = 0;
        destination += 2;
        character_count++;
    }
}

/* Writes the slot number (slot + 1) as two digit codes starting at 'O', at buffer[227] and buffer[229]. */
void xglMcWriteMapName(char *buffer, int slot)
{
    int value = slot + 1;
    int digit = value % 10;

    buffer[229] = digit + 'O';
    value /= 10;
    buffer[227] = value % 10 + 'O';
}

/*
 * Builds the path in mw.fullPath from the base name and card (two digits, or '*'
 * when card is negative). selector 0 repeats the prefix, 1 appends /icon.sys,
 * 2 appends /xenosaga.ico, -1 ends the path, and any other value appends '/'
 * and the string at that address. Returns mw.fullPath.
 */
char *xglMcSetFullPath(int card, int selector)
{
    char *dest;
    const char *source;
    const char *prefix_end;

    dest = mw.fullPath;
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
        for (source = mw.fullPath; source != prefix_end; source++)
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
        source = (const char *)selector;
        while ((*dest = *source) != 0)
        {
            dest++;
            source++;
        }
        break;
    }
    return mw.fullPath;
}

/* Starts a queued request: issues the first memory-card call for it and sets the phase its completion is handled in. */
static void execute(McRequestEntry *request)
{
    int command = request->operation;

    /* Each case issues one card call and sets the phase its completion uses. */
    switch (command)
    {
    case MC_REQUEST_FREE_SPACE:
        sceMcGetInfo(request->card, 0, 0, request->result, 0);
        return;
    case MC_REQUEST_DIRECTORY:
        if (request->slot == -1)
        {
            xglMcSetFullPath(-1, -1);
            sceMcGetDir(request->card, 0, mw.fullPath, 0,
                        MC_DIRECTORY_MAX_ENTRIES,
                        (request->buffer + 0xA3) & ~0x3F);
            mw.state.phase = MC_PHASE_DIRECTORY;
            return;
        }
        xglMcSetFullPath(request->slot, 0);
        sceMcOpen(request->card, 0, mw.fullPath, MC_OPEN_READ);
        mw.state.phase = MC_PHASE_OPEN_CLOSE;
        return;
    case MC_REQUEST_PATH_EXISTS:
        xglMcSetFullPath(0, (int)&request->path.empty);
        sceMcOpen(request->card, 0, mw.fullPath, MC_OPEN_READ);
        mw.state.phase = MC_PHASE_OPEN_CLOSE;
        return;
    case MC_REQUEST_LOAD_PATH:
    case MC_REQUEST_SAVE_PATH:
        xglMcSetFullPath(0, (int)&request->path.empty);
        if (request->operation == MC_REQUEST_SAVE_PATH)
        {
            sceMcOpen(request->card, 0, mw.fullPath, MC_OPEN_WRITE_CREATE);
            mw.state.phase = MC_PHASE_WRITE;
        }
        else
        {
            sceMcOpen(request->card, 0, mw.fullPath, MC_OPEN_READ);
            mw.state.phase = MC_PHASE_READ;
        }
        mw.state.subphase = 0;
        return;
    case MC_REQUEST_CREATE:
        xglMcSetFullPath(request->slot, -1);
        sceMcMkdir(request->card, 0, mw.fullPath);
        mw.state.subphase = 0;
        mw.state.phase = MC_PHASE_CREATE;
        return;
    case MC_REQUEST_SAVE:
        xglMcSetFullPath(request->slot, 0);
        sceMcOpen(request->card, 0, mw.fullPath, MC_OPEN_WRITE_CREATE);
        mw.state.subphase = 0;
        mw.state.phase = MC_PHASE_WRITE;
        return;
    case MC_REQUEST_LOAD:
        xglMcSetFullPath(request->slot, 0);
        sceMcOpen(request->card, 0, mw.fullPath, MC_OPEN_READ);
        mw.state.subphase = 0;
        mw.state.phase = MC_PHASE_READ;
        return;
    case MC_REQUEST_FORMAT:
        sceMcFormat(request->card, 0);
        return;
    case MC_REQUEST_UNFORMAT:
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

/*
 * Steps the save-file creation: delete the old file, write the icon and system
 * files, then the save data with its checksum, and close it. Returns 1 when the
 * sequence is finished, -1 on error and 0 while steps remain.
 */
static int create_sub2(int result)
{
    int slot = ((McWorkspaceView *)&mw)->request.slot;
    McSaveHeader *save;
    McSavePayload *payload;
    McSaveChecksum *trailer;
    float *checksum;
    int size;
    void *write_data;

    if (mw.state.subphase < 2 && (result >= 0 || result == -4)) {
        if (mw.state.subphase == 0) {
            xglMcSetFullPath(slot, 0);
            sceMcDelete(((McWorkspaceView *)&mw)->request.card, 0, ((McWorkspaceView *)&mw)->full_path);
        } else {
            xglMcSetFullPath(slot, 1);
            sceMcOpen(((McWorkspaceView *)&mw)->request.card, 0, (char *)&mw + 32, MC_OPEN_WRITE_CREATE);
        }
    } else {
        /* Subphases: 0 to 2 write the icon file, 3 closes it, 4 opens the save data, 5 writes the payload, 6 closes it. */
        if (result < 0)
            goto store_error;
        save = (McSaveHeader *)((McWorkspaceView *)&mw)->request.buffer;
        switch (mw.state.subphase) {
        case 0:
        case 1:
        case 2:
            mw.state.descriptor = result;
            write_data = save->icon_header;
            xglMcWriteMapName(write_data, slot);
            sceMcWrite(mw.state.descriptor, write_data, MC_ICON_SYS_SIZE);
            break;
        case 3:
            sceMcClose(mw.state.descriptor);
            break;
        case 4:
            xglMcSetFullPath(slot, 2);
            {
                McWorkspaceView *view = (McWorkspaceView *)&mw;
                sceMcOpen(view->request.card, 0, (char *)&mw + 32, MC_OPEN_WRITE_CREATE);
            }
            break;
        case 5:
            mw.state.descriptor = result;
            write_data = &save->payload;
            payload = write_data;
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

/*
 * Advances the memory-card state machine once per call: completes the current
 * phase from the sync result, then starts the next queued request or polls the
 * next card. Returns the card status.
 */
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
        if (mw.state.phase == MC_PHASE_POLL_INFO && queue_end == queue_top)
            return mw.state.status.combined;
        return MC_STATUS_BUSY_BOTH;
    }
    if (sync == -1) {
        result = 0;
        command = 0;
    }
    /* Each phase handles the completion of the call it started. */
    switch (mw.state.phase) {
    case MC_PHASE_POLL_INFO:
        if (queue_end == queue_top) {
            status = &mw.state.status.slots[mw.state.descriptor];
            switch (mw.state.card_type) {
            case MC_TYPE_PS1: *status = MC_STATUS_PS1_CARD; break;
            case MC_TYPE_PS2: *status = MC_STATUS_READY; break;
            case MC_TYPE_POCKETSTATION: *status = MC_STATUS_POCKETSTATION; break;
            default: *status = MC_STATUS_NONE; break;
            }
            if (result == -1 || result == -2) {
                *status = MC_STATUS_ERROR;
            } else {
                card_type = mw.state.card_type;
                if (card_type == MC_TYPE_PS2 && result == 0 && mw.state.formatted == 0)
                    *status = card_type;
            }
        }
        mw.state.phase = MC_PHASE_IDLE;
    /* Idle; also reached after a finished info poll. Start a queued request or poll the next card. */
    case MC_PHASE_IDLE:
        if (queue_end != queue_top) {
            request = &queue[queue_top];
            savedMcRequest = *request;
            execute(request);
            queue_top = (queue_top + 1) & MC_QUEUE_MASK;
            return MC_STATUS_BUSY_BOTH;
        }
        card = mw.state.next_card & 1;
        mw.state.descriptor = card;
        sceMcGetInfo(card, 0, &mw.state.card_type, 0, &mw.state.formatted);
        mw.state.phase = MC_PHASE_POLL_INFO;
        mw.state.next_card ^= 1;
        break;
    case MC_PHASE_OPEN_CLOSE:
        *((McWorkspaceView *)&mw)->request.result = result;
        mw.state.phase = MC_PHASE_IDLE;
        if (result < 0)
            break;
        sceMcClose(result);
        return MC_STATUS_BUSY_BOTH;
    case MC_PHASE_DIRECTORY:
        /* Clear the slot table, then mark each listed save's slot and record the latest save. */
        stored_result = result;
        *((McWorkspaceView *)&mw)->request.result = stored_result;
        slots = (unsigned char *)((McWorkspaceView *)&mw)->request.buffer;
        clear_slot = &slots[MC_LATEST_SLOT_INDEX];
        index = MC_LATEST_SLOT_INDEX;
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
            slots[MC_LATEST_SLOT_INDEX] = latest_slot;
        }
        mw.state.phase = MC_PHASE_IDLE;
        break;
    case MC_PHASE_CREATE:
        created = create_sub2(result);
        if (created == 0)
            return MC_STATUS_BUSY_BOTH;
        mw.state.phase = MC_PHASE_IDLE;
        if (created < 0)
            mw.state.status.combined = MC_STATUS_BUSY_BOTH;
        break;
    case MC_PHASE_WRITE:
    case MC_PHASE_READ:
        if (result >= 0) {
            if (mw.state.subphase == 0) {
                mw.state.descriptor = result;
                phase = mw.state.phase;
                size = ((McWorkspaceView *)&mw)->request.size;
                buffer = (void *)((McWorkspaceView *)&mw)->request.buffer;
                switch (phase) {
                case MC_PHASE_WRITE:
                    sceMcWrite(result, buffer, size);
                    break;
                case MC_PHASE_READ:
                    sceMcRead(result, buffer, size);
                    break;
                }
                mw.state.subphase = 1;
                return MC_STATUS_BUSY_BOTH;
            }
            sceMcClose(mw.state.descriptor);
        } else {
            *((McWorkspaceView *)&mw)->request.result = result;
            mw.state.status.combined = MC_STATUS_BUSY_BOTH;
        }
        mw.state.phase = MC_PHASE_IDLE;
        break;
    }
    return mcMainStatus[0];
}

/*
 * Loads size bytes of the file at path into buffer, waiting for each card call.
 * Returns the bytes read, or a negative error.
 */
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

    sceMcOpen(0, 0, path, MC_OPEN_READ);
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

/* Writes size bytes from buffer to a file at path, waiting for each card call. Returns 0, or -1 on failure. */
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

    sceMcOpen(0, 0, path, MC_OPEN_WRITE_CREATE);
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

/* Initializes the memory-card library and resets the request queue. */
void xglMcInitial(void)
{
    sceMcInit();
    xglMcReset();
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
float checksumNoiseScale = 0.60000002384185791f;
