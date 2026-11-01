#include "common.h"
#include "main/e_battle_win_open.h"

void WindowSPSet(WindowSPCursor *cursor);
void WindowSPMain(WindowSPCursor *cursor);
void WindowSPSelectJump(WindowSPCursor *cursor, int selection);

#include "main/open_close_main.h"
#include "main/end_print.h"
#include "main/e_number_main.h"
#include "main/menu_1.h"

#include "shared.h"

#include "file.h"

#include "main/xgl_jpeg.h"

#include "main/menu_top_command_check.h"

typedef struct FileThumbnailRequest {
    u32 source;
    u32 destination;
    u8 unmodeled_08[8];
    u16 destination_x;
    u16 destination_y;
    u16 width;
    u16 height;
    u8 unmodeled_18[8];
} FileThumbnailRequest;

/* FileWork+0x09 is the signed current-slot byte used by the JPEG routines. */

typedef struct FileWorkCurrentSlotView {
    unsigned char unmodeled_00[0x09];
    signed char current_slot;
} FileWorkCurrentSlotView;

/* Five decoder buffers are 0xe100 bytes apart; their signed object number is +1. */

typedef struct FileJpegSlotView {
    unsigned char unmodeled_00;
    signed char number;
    unsigned char unmodeled_02[0xe100 - 0x02];
} FileJpegSlotView;

typedef struct FileJpegDecodeSlotView {
    unsigned char unmodeled_00;
    signed char number;
    unsigned char unmodeled_02[0x80 - 0x02];
    unsigned int dma_qwc;
    unsigned int dma_address;
    unsigned int vif_nop;
    unsigned int vif_directhl;
    unsigned int gif_tag_low;
    unsigned int gif_tag_high;
    unsigned int gif_regs_low;
    unsigned int gif_regs_high;
    unsigned char image[0xe100 - 0xa0];
} FileJpegDecodeSlotView;

typedef struct FileObjectEntryView {
    struct FileJpegDecodeSlotView *jpeg;
    unsigned char thumbnail[0x1000];
    unsigned char state;
    signed char number;
    unsigned char unmodeled_1006;
    unsigned char chapter;
    unsigned char leader;
    unsigned char unmodeled_1009[3];
    int map_number;
    int party_ids[16];
    u64 play_time;
    XglClock elapsed;
} FileObjectEntryView;

void FileJpegDecode(int number);

extern char MenuFileSlot1Text[];

extern char MenuFileSlot2Text[];

extern char MenuFileHardDiskDriveText[];

static char *name_0_0036D628[3] = {
    MenuFileSlot1Text,
    MenuFileSlot2Text,
    MenuFileHardDiskDriveText,
};

FileWorkBlock *FileWork = 0;

byte *FileJpegDec = 0;

byte *FileObjectData = 0;

#include "file.private.h"

struct FilePassWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    char *parameters;
    signed char state;
    unsigned char unmodeled_11[3];
    int (*draw)(MenuStatusPanelWindow *window, MenuTopMenuPanel *data);
    void *data;
    unsigned char unmodeled_1c[0x194 - 0x1c];
};

struct FileInfoWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    char *parameters;
    signed char state;
    unsigned char unmodeled_11[3];
    int (*draw)(MenuStatusPanelWindow *window, MenuTopInfoPanel *data);
    void *data;
    unsigned char unmodeled_1c[0x194 - 0x1c];
};

struct FilePassClip {
    short x;
    short y;
    int color;
    short width;
    short height;
};

struct FileTagFont {
    signed char state;
    signed char sub_state;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    signed char channels[4];
    unsigned char unmodeled_10[0x1c - 0x10];
    const char *text;
};

struct FileNumber {
    short x;
    short y;
    int color;
    signed char channels[4];
    signed char mode;
    unsigned char unmodeled_0d;
    unsigned char format;
    unsigned char digits;
    unsigned char flags[4];
    int value;
    unsigned char unmodeled_18[0x90 - 0x18];
};

struct FileFaceSprite {
    signed char state;
    unsigned char unmodeled_01[3];
    short x;
    short y;
    int color;
    signed char channels[4];
    unsigned char unmodeled_10[0x24 - 0x10];
    short sprite_id;
    unsigned char unmodeled_26[2];
};

struct FileRibbon {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[0xe0 - 0x0c];
};

struct FileThumbnailPacket {
    short x;
    short y;
    int color;
    signed char channels[4];
    void *packet;
};

struct FileThumbnailContext {
    unsigned char state;
    unsigned char visible;
    unsigned char slot;
    unsigned char unmodeled_03;
    short x;
    short y;
    int color;
    struct FilePassWindow window;
    struct FileTagFont labels[2];
    unsigned char unmodeled_1e0[0x200 - 0x1e0];
    FileMessage messages[2];
    struct FileNumber numbers[5];
    unsigned char unmodeled_558[0x638 - 0x558];
    struct FileFaceSprite faces[6];
    unsigned char unmodeled_728[0x778 - 0x728];
    struct FileTagFont slot_label;
    struct FileNumber slot_number;
    struct FileRibbon ribbon;
    struct FileThumbnailPacket thumbnail;
};

struct FileCursor {
    signed char state;
    signed char active;
    signed char timer;
    signed char width;
    short x;
    short y;
    int work;
    signed char color[4];
    unsigned char unmodeled_10[0x24 - 0x10];
};

struct FileDataContext {
    unsigned char state;
    unsigned char visible;
    unsigned char extra_panel;
    unsigned char initial_flags;
    int initial_value;
    int work;
    struct FileThumbnailContext panels[4];
    struct FileCursor cursor;
    struct FilePassClip clip;
};

typedef struct FilePasWork {
    unsigned char unmodeled_00[0x04];
    short x;
    short y;
    int color;
    struct FilePassWindow window;
    FileMessage messages[6];
    unsigned char unmodeled_338[0x3c0 - 0x338];
    struct FilePassClip rect;
} FilePasWork;

static char * msg00_1_0036D638[6];

extern int MenuPasLengthGet(const char *text);

struct FileInfoWindowData {
    short x;
    short y;
    unsigned char unmodeled_04[4];
    int mode;
    char *text;
};

struct FileInfoContext {
    unsigned char unmodeled_00[4];
    int work;
    struct FileInfoWindow window;
    unsigned char unmodeled_19c[0x1b0 - 0x19c];
    struct FileInfoWindowData contents;
    unsigned char unmodeled_1c0[0x360 - 0x1c0];
    char text[256];
};

struct FileDeviceTexts {
    char *text[3];
};

struct FileOperationTexts {
    char *text[3];
};

struct FileSelectionTexts {
    char *text[4];
};

struct FileListTexts {
    char *text[2];
};

struct FileConfirmationTexts {
    char *text[12];
};

struct FileSlotTexts {
    char *text[3];
};

struct FileSlotStateTexts {
    char *text[2][3];
};

struct FileCardTexts {
    char *text[7];
};

struct FileRequestTexts {
    char *text[9];
};

struct FileFormatTexts {
    char *text[3];
};

struct FileFinishTexts {
    char *text[2];
};

struct FileProgressTexts {
    char *text[4];
};

extern const struct FileDeviceTexts fileDeviceTexts;

extern const struct FileOperationTexts fileOperationTexts;

extern const struct FileSelectionTexts fileSelectionTexts;

extern char *fileListTexts[];

extern char * const fileConfirmationTexts[12];

extern const struct FileSlotTexts fileSlotTexts;

extern char * const fileSlotStateTexts[6];

extern const struct FileCardTexts fileCardTexts;

extern const struct FileRequestTexts fileRequestTexts;

extern const struct FileFormatTexts fileFormatTexts;

extern char *fileFinishTexts[];

extern char * const fileProgressTexts[4];

extern char periodText[];
extern char periodEndText[];
extern const char insertCardPromptText[];

extern void eMessageCpy(char *destination, char *source);

extern void eMessageCat(char *source);

struct FileLoadGauge {
    short x;
    short y;
    unsigned int vertex_attribute;
    short width;
    short height;
    unsigned short current;
    unsigned short maximum;
    struct FileGaugeColor {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
        unsigned char alpha;
    } filled_color;
    struct FileGaugeColor unfilled_color;
};

static struct FileLoadGauge gauge;

void tyaDrawGauge(struct FileLoadGauge *progress);

typedef struct FileObjectImages {
    FileObjectEntryView slots[99];
    FileObjectEntryView templates[3];
    FileObjectEntryView current;
} FileObjectImages;

extern XglTaskScheduler *FileTask;

extern unsigned char *FileCardDataBuf;

extern unsigned char *FileExistCheckWork;

extern const unsigned char cardDataNewPath[];

extern const unsigned char newDataThumbnailPath[];

extern const unsigned char unusedDataThumbnailPath[];

extern const unsigned char brokenDataThumbnailPath[];

void endPrintDirectFrameCopy(XglPacket *packet, int source, int destination);

void endPrintInit(void);

void MenuCfTaikiPush(void);

void MenuCfTaikiPop(void);

XglTaskScheduler *MenuBgTaskInit(void *work, int mode);

void *xglTaskInitial(void *work, int entries, int mode);

void MenuBgTaskMain(void);

void MenuBgTaskBreak(void);

void MenuLoadInit(void);

void MenuFontLoad(int mode, int wait);

void WindowTexLoad(int mode, int resource);

void *MenuSaveMapTextLoad(void *work, int mode);

void MenuLoadFile(const unsigned char *name, void *destination);

void MenuWorkEndCheck(void *end);

int MenuLoadSync(void);

int GameSnapShotSaveThumbnail(int quality, void *destination);

void GameSnapShotCheck(void);

void GameCfPlayerLoadResource(int mode);

void MapChangeFadeSet(void);

void exit(int status);

void *MenuWorkEndGet(void);

void MenuFile(void);

static void VersionUpDate(SaveDataHeader *save_data)
{
    save_data->version += 1;
    FileCheckSumGet((FileChecksumData *)save_data, (long long *)&save_data->checksum);
}

static void VersionChange00To01(SaveDataVersions *save_data)
{
    SaveData01 new_data;

    new_data.added_40 = 0;
    new_data.head = save_data->version_00.head;
    new_data.head.added_3c = 0;
    new_data.added_44 = 0;
    new_data.block = save_data->version_00.block;
    save_data->version_01 = new_data;
    VersionUpDate(&save_data->header);
}

void FileVersionCheck(SaveDataHeader *save_data)
{
}

int FileObjectDataNoGet(int number, int offset)
{
    number += offset;
    if (number < 0)
        return number + 99;
    if (number < 99)
        return number;
    return number - 99;
}

void FileObjectDataClear(void)
{
    int i;

    for (i = 0; i <= 100; ++i) {
        FileObjectData[i * 0x1060 + 0x1004] = 0;
        FileObjectData[i * 0x1060 + 0x1005] = -1;
    }
    for (i = 0; i <= 4; ++i)
        FileJpegDec[i * 0xe100 + 1] = -1;
}

void FileThumbnailDecode(u32 source, u32 destination)
{
    FileThumbnailRequest request;

    memset(&request, 0, sizeof(request));
    request.source = source;
    request.destination = destination;
    request.destination_x = 0;
    request.destination_y = 0;
    request.width = 0;
    request.height = 0;
    xglJpegDecode(&request);
}

void FileCheckSumGet(FileChecksumData *source, long long *checksum)
{
    FileChecksumData local = *source;
    byte *data = local.data;
    unsigned int count = 0;
    int i = 0;

    local.fields.stored_checksum = 0;
    *checksum = 0;
    do {
        *checksum += (long long)*data * i + 0x7ca;
        ++data;
        ++i;
        ++count;
    } while (count <= 0x163af);
}

int FileCheckSumCheck(FileChecksumData *data)
{
    long long checksum;

    FileCheckSumGet(data, &checksum);
    return data->fields.stored_checksum == checksum;
}

void FileObjectDataInput(int number, int state)
{
    FileObjectEntryView *entry = (FileObjectEntryView *)FileObjectData + number;
    int i;
    int *party_ids;
    PartyStateImage *party;
    short map_number;
    u64 play_time;
    unsigned char *thumbnail;

    memset(entry, 0, sizeof(*entry));
    switch (state) {
    case 2:
        *entry = ((FileObjectEntryView *)FileObjectData)[99];
        break;
    case 3:
        *entry = ((FileObjectEntryView *)FileObjectData)[100];
        break;
    case 4:
        *entry = ((FileObjectEntryView *)FileObjectData)[101];
        break;
    case 1:
        if (FileSaveData->version != 3) {
            *entry = ((FileObjectEntryView *)FileObjectData)[101];
            FileObjectDataSystem->state[number] = 4;
        } else if (!FileCheckSumCheck((FileChecksumData *)FileSaveData)) {
            *entry = ((FileObjectEntryView *)FileObjectData)[101];
            FileObjectDataSystem->state[number] = 4;
        } else {
            party_ids = entry->party_ids;
            entry->chapter = FileSaveData->chapter;
            entry->leader = FileSaveData->leader;
            map_number = FileSaveData->map_number;
            play_time = FileSaveData->play_time;
            entry->elapsed = FileSaveData->elapsed;
            entry->map_number = map_number;
            entry->play_time = play_time;
            PartyTimeDispChange(&entry->elapsed);
            party = (PartyStateImage *)FileSaveData->party;
            memset(party_ids, 0, sizeof(entry->party_ids));
            PartyAllPartyGet2(party_ids, party);
            thumbnail = FileSaveData->thumbnail;
            for (i = 0; i < 0x1000; ++i)
                entry->thumbnail[i] = thumbnail[i];
        }
        break;
    }
    entry->number = number;
}

void FileJpegCheck(void)
{
    signed char valid_numbers[5];
    FileJpegSlotView *slot;
    int i;
    int j;

    i = -1;
    do {
        valid_numbers[i + 1] = FileObjectDataNoGet(((FileWorkCurrentSlotView *)FileWork)->current_slot, i);
        i = i + 1;
    } while (i < 4);

    for (i = 0; i < 5; ++i) {
        slot = (FileJpegSlotView *)FileJpegDec + i;
        for (j = 0; j < 5; ++j) {
            if (slot->number == valid_numbers[j])
                break;
        }
        if (j == 5)
            slot->number = -1;
    }
}

void FileJpegDecode(int number)
{
    FileObjectEntryView *entry = (FileObjectEntryView *)FileObjectData + number;
    FileJpegDecodeSlotView *jpeg = (FileJpegDecodeSlotView *)FileJpegDec;
    FileJpegDecodeSlotView *queued;
    FileJpegDecodeSlotView *slot;
    int i;

    queued = jpeg;
    for (i = 0; i < 5; ++i) {
        if (queued->number == number)
            return;
        ++queued;
    }

    slot = jpeg;
    for (i = 0; i < 5; ++i) {
        if (slot->number < 0) {
            FileThumbnailDecode((u32)entry->thumbnail, (u32)slot->image);
            slot->dma_qwc = 0;
            slot->dma_address = 0;
            slot->vif_nop = 0;
            slot->vif_directhl = 0x51000e01;
            slot->gif_tag_low = 0x8e00;
            slot->gif_tag_high = 0x08000000;
            slot->gif_regs_low = 0;
            slot->gif_regs_high = 0;
            entry->jpeg = slot;
            slot->number = number;
            return;
        }
        ++slot;
    }
}

void FileObjectJpegSet(void)
{
    int offset;
    int slot;

    FileJpegCheck();
    offset = -1;
    do {
        slot = FileObjectDataNoGet(((FileWorkCurrentSlotView *)FileWork)->current_slot, offset);
        offset += 1;
        FileJpegDecode(slot);
    } while (offset < 4);
}

void FileObjectJpegDecChange(int number)
{
    int i;

    for (i = 0; i < 5; ++i) {
        /* The original adds the slot offset to the buffer address as integers
         * (addu offset, base); pointer arithmetic swaps the addu operands. */
        FileJpegSlotView *slot = (FileJpegSlotView *)(i * sizeof(FileJpegSlotView) + (u32)FileJpegDec);

        if (slot->number == number) {
            slot->number = -1;
            FileJpegDecode(number);
            break;
        }
    }
}

int FileObjectAllLoad(void)
{
    struct FileHddLoadRequest {
        int slot;
        FileSaveRecord *data;
        int size;
    } hdd_request;
    McRequestArguments card_request;
    int xglHddMcLoadMount(void);
    int xglHddMcLoadCore(void *request);
    int xglHddMcUmount(void);
    int index;
    int loaded_count;
    int state;
    unsigned char *load_state = &FileObjectDataSystem->load_state;
    unsigned char *load_index = &FileObjectDataSystem->load_index;

    switch (*load_state) {
    case 0: {
        FileWorkBlock *initial_work = FileWork;

        gauge.x = 123;
        gauge.y = 296;
        gauge.vertex_attribute = 0xffffffffU;
        loaded_count = 0;
        gauge.width = 256;
        index = 0;
        gauge.height = 8;
        gauge.current = 0;
        gauge.maximum = 0;
        gauge.filled_color.red = 161;
        gauge.filled_color.green = 232;
        gauge.filled_color.blue = 205;
        gauge.filled_color.alpha = 128;
        gauge.unfilled_color.red = 64;
        gauge.unfilled_color.green = 64;
        gauge.unfilled_color.blue = 64;
        gauge.unfilled_color.alpha = 128;
        __builtin_memset(&initial_work->hdd_result, 0,
                         sizeof(initial_work->hdd_result));
        for (; index < 99; index++) {
            state = FileObjectDataSystem->state[index];

            if (state != 1) {
                int empty = state <= 0;

                if (empty) {
                    continue;
                }
                if (state < 5) {
                    FileObjectDataInput(index, state);
                    loaded_count++;
                }
            } else {
                gauge.maximum++;
            }
        }
        *load_state = 2;
        *load_index = 0;
        if (loaded_count == 99) {
            FileWorkBlock *work = FileWork;

            work->hdd_result = 0;
            if (!work->use_hdd) {
                unsigned short *card_status = work->card.status;

                if (card_status[work->card_number] != 1) {
                    work->hdd_result = -1;
                }
            }
            *load_state = 0;
            return 0;
        }
        if (FileWork->use_hdd) {
            FileWork->hdd_result = xglHddMcLoadMount();
        }
    }
    case 2:
        gauge.current++;
        while (*load_index < 99) {
            if (FileObjectDataSystem->state[*load_index] == 1) {
                *load_state = 4;
                break;
            }
            (*load_index)++;
        }
        if (*load_index == 99) {
            *load_state = 0;
            if (FileWork->use_hdd) {
                FileWork->hdd_result = xglHddMcUmount();
            }
            return 0;
        }
    case 4:
        if (FileWork->use_hdd) {
            hdd_request.slot = *load_index;
            hdd_request.data = FileSaveData;
            hdd_request.size = sizeof(*FileSaveData);
            memset(FileSaveData, 0, sizeof(*FileSaveData));
            FileWork->hdd_result = xglHddMcLoadCore(&hdd_request);
            *load_state = 6;
        } else {
            card_request.slot = *load_index;
            card_request.buffer = (int)FileSaveData;
            card_request.size = sizeof(*FileSaveData);
            memset(FileSaveData, 0, sizeof(*FileSaveData));
            xglMcRequest(FileWork->card_number, 4, &card_request,
                         &FileWork->request_result);
            *load_state = 6;
            tyaDrawGauge(&gauge);
            return 1;
        }
    case 6:
        if (FileWork->use_hdd) {
            if (FileWork->hdd_result != 0) {
                FileObjectDataSystem->state[*load_index] = 4;
                FileObjectDataInput(*load_index, 4);
                *load_state = 9;
            } else {
                FileObjectDataInput(*load_index, 1);
                *load_state = 8;
            }
        } else {
            FileWorkBlock *work = FileWork;
            unsigned short *card_status = work->card.status;
            int status = card_status[work->card_number];

            if (status != 1) {
                if (status != 255) {
                    FileObjectDataSystem->state[*load_index] = 4;
                    FileObjectDataInput(*load_index, 4);
                    FileWork->hdd_result = -1;
                    *load_state = 9;
                }
            } else {
                FileObjectDataInput(*load_index, 1);
                *load_state = 8;
            }
        }
        if (*load_state == 8) {
            *load_state = 2;
            (*load_index)++;
        }
        if (*load_state == 9) {
            *load_state = 0;
            if (FileWork->use_hdd) {
                FileWork->hdd_result = xglHddMcUmount();
            }
            return 0;
        }
        break;
    }
    tyaDrawGauge(&gauge);
    return 1;
}

void FileSinkiSaveDataPush(void)
{
}

void FileSaveDataPush(void)
{
    long long checksum;
    const unsigned char *name;
    int scenario;
    int rank;
    int i;
    short map_no;

    GamePushSaveDataUser();
    for (i = 0; i < 0x1000; ++i)
        ((FileSaveOutput *)SaveData)->thumbnail[i] = ((FileObjectEntryView *)FileObjectData)[102].thumbnail[i];
    map_no = GameLoopState.map_no;
    ((FileSaveOutput *)SaveData)->map_no = map_no;
    name = MenuSaveMapNameGet(map_no);
    xglMcSetMapName(name, name + 33);
    if (map_no == 10)
        ((FileSaveOutput *)SaveData)->unit_map_count++;
    scenario = MenuScenarioNoGet();
    if (scenario > 300)
        rank = 3;
    else if (scenario > 100)
        rank = 2;
    else
        rank = 1;
    ((FileSaveOutput *)SaveData)->version = 3;
    ((FileSaveOutput *)SaveData)->save_count++;
    ((FileSaveOutput *)SaveData)->scenario_rank = rank;
    PartyTimeUpDate();
    xglClockRead(&((FileSaveOutput *)SaveData)->clock);
    FileCheckSumGet((FileChecksumData *)SaveData, &checksum);
    ((FileSaveOutput *)SaveData)->checksum = checksum;
}

char *FileSlotNameGet(int slot)
{
    return name_0_0036D628[slot];
}

void TskObjectSet(TskObject *task, TskObjectWorker worker, void *data)
{
    task->data = data;
    task->worker = worker;
    task->state = 0;
}

void tskTskMain(TskObject *task)
{
    TskObjectWorker worker = task->worker;

    if (FileWork->state == 0xff) {
        xglTaskWaitRemove(&task->base);
        return;
    }

    if (task->state != 0) {
        if (task->state != 2)
            return;
    } else {
        worker(task, task->data);
        task->state = 2;
    }

    worker(task, task->data);
}

static void MenuFilePas(TskObject *task, void *data)
{
    FilePasWork *work = data;
    int i;
    int lengths[6];
    unsigned short targets[6];
    short slide_target;
    int card_target;

    for (i = 0; i < 6; ++i)
        lengths[i] = MenuPasLengthGet(msg00_1_0036D638[i]);

    switch (task->state) {
    case 0:
        work->x = -272;
        work->y = 8;
        work->color = 0xfffff0;
        WindowDXSet((WindowDX *)(void *)(&work->window));
        work->window.x = work->x;
        work->window.color = work->color;
        work->window.width = 352;
        work->window.y = work->y;
        work->window.height = 32;
        work->window.state = 1;
        WindowDXMain((WindowDX *)(void *)(&work->window));
        work->window.state = 3;
        for (i = 0; i < 6; ++i) {
            eMessageSet((struct EMessageSetParam *)(void *)(&work->messages[i]), msg00_1_0036D638[i]);
            work->messages[i].mode = 32;
            work->messages[i].x = 288;
            work->messages[i].color = work->color + 2;
            work->messages[i].y = work->y + 3;
        }
        break;
    case 2:
        slide_target = -16;
        for (i = 0; i < 6; ++i)
            targets[i] = 288;
        switch (FileWork->state) {
        case 36:
        case 128:
        case 130:
            targets[FileWork->text_index] = 16;
            break;
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 44:
        case 45:
        case 46:
        case 80:
        case 82:
        case 136:
        case 144:
        case 145:
        case 146:
        case 147:
        case 148:
        case 149:
        case 150:
        case 152:
        case 160:
        case 161:
        case 176:
        case 184:
            targets[FileWork->text_index] = 16;
            if (FileWork->use_hdd)
                targets[4] = targets[FileWork->text_index] + (unsigned short)lengths[FileWork->text_index];
            else
                targets[2 + FileWork->card_number] = targets[FileWork->text_index] + (unsigned short)lengths[FileWork->text_index];
            break;
        case 132:
        case 134:
        case 135:
            targets[FileWork->text_index] = -(unsigned short)lengths[FileWork->text_index];
            targets[5] = 16;
            card_target = (unsigned short)lengths[5] + 16;
            targets[2 + ((union { int i; char c; } *)&FileWork->card_number)->i] = card_target;
            break;
        default:
            slide_target = -352;
            break;
        }
        MoveSlide(&work->window.x, &slide_target, 3.0f);
        WindowDXMain((WindowDX *)(void *)(&work->window));
        work->rect.x = work->window.x + 3;
        work->rect.y = work->window.y + 3;
        work->rect.color = work->color;
        work->rect.width = work->window.width - 6;
        work->rect.height = work->window.height - 6;
        endPrintExtFunc(work->color, 101, &work->rect);
        for (i = 0; i < 6; ++i) {
            MoveSlide(&work->messages[i].x, (short *)&targets[i], 3.0f);
            if (work->messages[i].x < 256)
                eMessageMain((EMessageParam *)(void *)(&work->messages[i]));
        }
        endPrintExtFunc(work->color, 102, 0);
        break;
    }
}

static void MenuFileInfo(TskObject *task, void *data)
{
    char *device_text[3];
    char *operation_text[3];
    char *selection_text[4];
    char *list_text[2];
    char *confirmation_text[12];
    char *slot_text[3];
    char *slot_state_text[2][3];
    char *card_text[7];
    char *request_text[9];
    char *status_text;
    char *format_text[3];
    char *finish_text[2];
    char *progress_text[4];
    short target_y;
    short target_height;
    struct FileInfoContext *view;

    *(struct FileDeviceTexts *)device_text = fileDeviceTexts;
    *(struct FileOperationTexts *)operation_text = fileOperationTexts;
    *(struct FileSelectionTexts *)selection_text = fileSelectionTexts;
    view = data;
    *(struct FileListTexts *)list_text = *(struct FileListTexts *)fileListTexts;
    *(struct FileConfirmationTexts *)confirmation_text = *(struct FileConfirmationTexts *)fileConfirmationTexts;
    *(struct FileSlotTexts *)slot_text = fileSlotTexts;
    *(struct FileSlotStateTexts *)slot_state_text = *(struct FileSlotStateTexts *)fileSlotStateTexts;
    *(struct FileCardTexts *)card_text = fileCardTexts;
    *(struct FileRequestTexts *)request_text = fileRequestTexts;
    status_text = insertCardPromptText;
    *(struct FileFormatTexts *)format_text = fileFormatTexts;
    *(struct FileFinishTexts *)finish_text = *(struct FileFinishTexts *)fileFinishTexts;
    *(struct FileProgressTexts *)progress_text = *(struct FileProgressTexts *)fileProgressTexts;

    switch (task->state) {
    case 0: {
        struct FileInfoWindowData *contents;

        view->work = 0xfffff0;
        WindowDXSet((WindowDX *)(void *)(&view->window));
        view->window.x = -16;
        view->window.y = 480;
        view->window.width = 544;
        view->window.height = 54;
        view->window.state = 0;
        contents = &view->contents;
        view->window.draw = MenuInfoWindow;
        view->window.color = view->work;
        view->window.data = contents;
        contents->x = 0;
        view->window.state = 1;
        view->contents.y = 0;
        view->contents.text = 0;
        WindowDXMain((WindowDX *)(void *)(&view->window));
        view->window.state = 3;
        view->contents.mode = 1;
        memset(view->text, 0, sizeof(view->text));
        break;
    }
    case 2:
        target_y = 378;
        target_height = 54;
        switch (FileWork->state) {
        case 36:
            eMessageCpy(view->text, selection_text[FileWork->text_index]);
            switch (FileWork->device_selection) {
            case 0:
            case 1: {
                unsigned short *card_statuses = FileWork->card.status;

                switch (card_statuses[FileWork->device_selection]) {
                case 0:
                case 129:
                case 130:
                    eMessageCpy(view->text, selection_text[2]);
                    eMessageCat(FileSlotNameGet(FileWork->device_selection));
                    eMessageCat(periodText);
                    break;
                }
                break;
            }
            case 2:
                eMessageCpy(view->text, device_text[0]);
                break;
            }
            break;
        case 37:
        case 38:
        case 39: {
            unsigned char update_text = FileWork->input_flags & 1;

            if (update_text) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, operation_text[2]);
                    eMessageCat(FileSlotNameGet(2));
                    eMessageCat(operation_text[1]);
                } else {
                    eMessageCpy(view->text, operation_text[2]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(operation_text[0]);
                }
            }
            target_y = 352;
            target_height = 78;
            break;
        }
        case 40:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, list_text[FileWork->text_index]);
            target_y = 404;
            target_height = 30;
            break;
        case 41:
            switch (FileObjectDataSystem->state[FileWork->selected_slot]) {
            case 2:
                eMessageCpy(view->text, slot_state_text[FileWork->text_index][2]);
                break;
            case 3:
                eMessageCpy(view->text, slot_state_text[FileWork->text_index][0]);
                break;
            case 4:
                eMessageCpy(view->text, slot_state_text[FileWork->text_index][1]);
                break;
            }
            break;
        case 42:
            if (FileWork->input_flags & 1) {
                if (FileWork->text_index == 0) {
                    if (FileWork->use_hdd) {
                        eMessageCpy(view->text, FileSlotNameGet(2));
                        if (FileObjectDataSystem->state[FileWork->selected_slot] == 2)
                            eMessageCat(confirmation_text[10]);
                        else
                            eMessageCpy(view->text, confirmation_text[1]);
                    } else {
                        int card_slot = FileWork->selected_slot;

                        if (FileObjectDataSystem->state[card_slot] == 2) {
                            eMessageCpy(view->text, confirmation_text[0]);
                            eMessageCat(FileSlotNameGet(FileWork->card_number));
                            eMessageCat(periodText);
                        } else {
                            eMessageCpy(view->text, confirmation_text[1]);
                        }
                    }
                } else {
                    if (FileWork->use_hdd) {
                        eMessageCpy(view->text, slot_text[2]);
                        eMessageCat(FileSlotNameGet(2));
                        eMessageCat(periodText);
                    } else {
                        eMessageCpy(view->text, slot_text[0]);
                        eMessageCat(FileSlotNameGet(FileWork->card_number));
                        eMessageCat(periodText);
                    }
                }
            }
            break;
        case 44:
        case 45:
        case 80:
            if (FileWork->input_flags & 1) {
                if (FileWork->use_hdd) {
                    if (FileWork->text_index == 0)
                        eMessageCpy(view->text, confirmation_text[4]);
                    else
                        eMessageCpy(view->text, confirmation_text[5]);
                    eMessageCat(FileSlotNameGet(2));
                    eMessageCat(confirmation_text[6]);
                } else {
                    if (FileWork->text_index == 0)
                        eMessageCpy(view->text, confirmation_text[7]);
                    else
                        eMessageCpy(view->text, confirmation_text[8]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(confirmation_text[9]);
                }
            }
            target_y = 336;
            target_height = 78;
            break;
        case 132:
            if ((FileWork->input_flags & 1) && !FileWork->use_hdd) {
                eMessageCpy(view->text, confirmation_text[2]);
                eMessageCat(FileSlotNameGet(FileWork->card_number));
                eMessageCat(confirmation_text[11]);
            }
            target_y = 336;
            target_height = 78;
            break;
        case 46:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, confirmation_text[3]);
            break;
        case 82:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, slot_text[1]);
            break;
        case 128:
            if (FileWork->input_flags & 1) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, card_text[4]);
                    eMessageCat(FileSlotNameGet(2));
                    eMessageCat(card_text[6]);
                } else {
                    eMessageCpy(view->text, card_text[0]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(card_text[5]);
                }
            }
            break;
        case 130:
            if (FileWork->input_flags & 1) {
                eMessageCpy(view->text, card_text[1]);
                eMessageCat(FileSlotNameGet(FileWork->card_number));
                eMessageCat(periodText);
            }
            break;
        case 134:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, card_text[2]);
            break;
        case 135:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, card_text[3]);
            break;
        case 144:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, request_text[FileWork->text_index]);
            break;
        case 145:
            if (FileWork->input_flags & 1) {
                eMessageCpy(view->text, request_text[4]);
                eMessageCat(FileSlotNameGet(FileWork->card_number));
                eMessageCat(periodEndText);
            }
            break;
        case 146:
            if (FileWork->input_flags & 1) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, request_text[6]);
                    eMessageCat(FileSlotNameGet(2));
                    eMessageCat(request_text[8]);
                } else {
                    eMessageCpy(view->text, request_text[5]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(request_text[7]);
                }
            }
            break;
        case 148:
        case 149:
        case 150:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, progress_text[FileWork->state - 148]);
            target_y = 336;
            target_height = 78;
            break;
        case 151:
            if (FileWork->input_flags & 1) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, FileSlotNameGet(2));
                    eMessageCat(request_text[6]);
                } else {
                    char *card_message = view->text;

                    eMessageCpy(card_message, FileSlotNameGet(FileWork->card_number));
                    eMessageCat(request_text[5]);
                }
            }
            break;
        case 160:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, request_text[FileWork->text_index + 2]);
            break;
        case 161:
            if (FileWork->input_flags & 1)
                eMessageCpy(view->text, status_text);
            target_y = 336;
            target_height = 78;
            break;
        case 176:
            if (FileWork->input_flags & 1) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, FileSlotNameGet(2));
                    eMessageCat(format_text[1]);
                } else {
                    eMessageCpy(view->text, format_text[0]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(format_text[2]);
                }
            }
            if (FileWork->use_hdd) {
                target_y = 352;
                target_height = 78;
            } else {
                target_y = 328;
                target_height = 102;
            }
            break;
        case 184:
            if (FileWork->input_flags & 1) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, finish_text[1]);
                    eMessageCat(FileSlotNameGet(2));
                    eMessageCat(periodEndText);
                } else {
                    eMessageCpy(view->text, finish_text[0]);
                }
            }
            break;
        default:
            target_y = 480;
            break;
        }
        MoveSlide(&view->window.y, &target_y, 3.0f);
        MoveSlide(&view->window.height, &target_height, 3.0f);
        view->contents.text = view->text;
        WindowDXMain((WindowDX *)(void *)(&view->window));
        break;
    }
}

struct FileSelectItem {
    char *text;
    unsigned char unmodeled_04[4];
    signed char color_state;
    unsigned char unmodeled_09[3];
};

struct FileSelectList {
    unsigned char unmodeled_00[2];
    unsigned short layout_mode;
    int selected;
    char *title;
    void *confirmation_data;
    void *items;
    unsigned char unmodeled_14[0x5f8 - 0x14];
};

struct FileSelectContext {
    unsigned char unmodeled_00[2];
    unsigned char initial_flags;
    unsigned char initial_state;
    int work;
    unsigned char animation_flags[4];
    short targets[4];
    struct FilePassWindow windows[4];
    struct FileSelectList lists[4];
    struct FileSelectItem entries[6];
};

struct FileSelectSource {
    char *message;
    char *title;
};

extern const char memoryCardSlot1Text[];
extern const char memoryCardSlot2Text[];
extern char cancelText[];
extern char fileSelectText[];
static struct FileSelectSource msg01_2_0036D650[] ;

static void tskFileSelect(TskObject *task, void *data)
{
    struct FileSelectContext *const view = data;
    int i;

    void ListColorChage(void)
    {
        for (i = 0; i < 2; ++i) {
            unsigned short *card_statuses = FileWork->card.status;
            int card_status = card_statuses[i];
            switch (card_status) {
            case 0:
            case 129:
            case 130:
                view->entries[i].color_state = 1;
                break;
            default:
                view->entries[i].color_state = 0;
                break;
            }
        }
    }

    switch (task->state) {
    case 0: {
        struct FileSelectSource *confirmation;

        view->work = 0xfffff0;
        view->entries[0].text = memoryCardSlot1Text;
        view->entries[1].text = memoryCardSlot2Text;
        for (i = 0; i < 2; ++i)
            view->entries[i].color_state = 0;
        view->entries[i].text = cancelText;
        view->entries[i].color_state = 0;
        view->entries[i + 1].text = 0;
        ListColorChage();
        for (i = 0; i < 2; ++i) {
            WindowDXSet((WindowDX *)(void *)(&view->windows[i]));
            view->windows[i].color = view->work;
            view->windows[i].data = &view->lists[i];
            view->windows[i].draw = MenuSelectWindow;
            view->windows[i].width = 245;
            view->windows[i].height = 78;
            view->lists[i].selected = 0;
            view->lists[i].items = view->entries;
            view->windows[i].parameters = fileSelectText;
            view->lists[i].layout_mode = 2;
        }
        view->windows[i].height = 102;
        confirmation = msg01_2_0036D650;
        for (i = 2; i < 4; ++i) {
            WindowDXSet((WindowDX *)(void *)(&view->windows[i]));
            view->windows[i].color = view->work;
            view->windows[i].data = &view->lists[i];
            view->windows[i].draw = MenuSelectWindow;
            view->windows[i].width = 207;
            view->windows[i].parameters = fileSelectText;
            view->lists[i].title = confirmation->title;
            view->windows[i].height = 102;
            view->lists[i].selected = 0;
            view->lists[i].confirmation_data = confirmation->message;
            view->lists[i].layout_mode = 1;
        }
        for (i = 0; i < 4; ++i) {
            view->windows[i].x = -258;
            view->windows[i].y = 0;
            view->windows[i].state = 1;
            WindowDXMain((WindowDX *)(void *)(&view->windows[i]));
            view->windows[i].state = 3;
            view->targets[i] = -258;
            view->animation_flags[i] = 0;
        }
        view->initial_flags = 0;
        view->initial_state = 0;
        return;
    }
    case 2: {
        short targets[4];

        for (i = 0; i < 4; ++i)
            targets[i] = -258;
        switch (FileWork->state) {
        case 36:
            if (FileWork->input_flags & 1) {
                view->windows[0].x = 544;
                view->windows[0].y = 176;
            }
            view->lists[0].selected = FileWork->device_selection;
            ListColorChage();
        case 37:
        case 38:
        case 39:
            if (!FileWork->hide_selection)
                targets[0] = 128;
            break;
        case 42:
            if (FileWork->input_flags & 1) {
                view->windows[2].x = 544;
                view->windows[2].y = 224;
            }
            view->lists[2].selected = FileWork->action_selection;
            targets[2] = 160;
            break;
        case 130:
            if (FileWork->input_flags & 1) {
                view->windows[2].x = 544;
                view->windows[2].y = 160;
            }
            view->lists[2].selected = FileWork->action_selection;
            targets[2] = 160;
            break;
        case 160:
            if (FileWork->input_flags & 1) {
                view->windows[3].x = 544;
                view->windows[3].y = 160;
            }
            view->lists[3].selected = FileWork->action_selection;
            targets[3] = 160;
            break;
        }
        for (i = 0; i < 4; ++i) {
            MoveSlide(&view->windows[i].x, &targets[i], 3.0f);
            WindowDXMain((WindowDX *)(void *)(&view->windows[i]));
        }
        break;
    }
    }
}

static void tskFileEx(TskObject *task, FileExplanation *text)
{
    switch (task->state) {
    case 0:
        text->color = 0xffffff;
        eMessageSet((struct EMessageSetParam *)(void *)(&text->message), 0);
        text->message.color = text->color;
        text->message.mode = 32;
        text->open = 0;
        break;
    case 2:
        switch (FileWork->state) {
        case 44:
        case 45:
        case 80:
        case 132:
            if (FileWork->input_flags & 1) {
                if (!text->open) {
                    text->open = 1;
                    text->message.x = 544;
                    if (FileWork->state == 132) {
                        text->target_x = 192;
                        text->message.y = 192;
                        text->message.text = msg00_4_0036D658[2];
                    } else {
                        text->target_x = 224;
                        text->message.y = 256;
                        text->message.text = msg00_4_0036D658[FileWork->text_index];
                    }
                }
            }
            break;
        default:
            text->target_x = -140;
            if (text->message.x == -140)
                text->open = 0;
            break;
        }
        if (text->open) {
            MoveSlide(&text->message.x, &text->target_x, 3.0f);
            do {
                eMessageMain((EMessageParam *)(void *)(&text->message));
            } while (0);
        }
        break;
    }
}

/* Thumbnail display reads the packed time word and signed scenario bytes. */
typedef struct FileThumbnailSaveEntry {
    struct FileJpegDecodeSlotView *jpeg;
    unsigned char unmodeled_004[0x1007 - 0x004];
    signed char chapter;
    signed char leader;
    unsigned char unmodeled_1009[3];
    int map_number;
    int party_ids[16];
    unsigned int play_time;
    unsigned char unmodeled_1054[4];
    unsigned int packed_time;
    unsigned char unmodeled_105c[4];
} FileThumbnailSaveEntry;

extern const char shionLevelText[];
extern const char newFileText[];
extern const char corruptDataText[];
extern const char cannotSaveText[];
extern const char cannotLoadText[];
extern char slotNumberText[];
static char * msg00_5_0036D668[] ;

void FileThumbnailDisp(struct FileThumbnailContext *panel)
{
    int i;
    int party_index;
    int party_count;
    FileThumbnailSaveEntry *entry;
    const unsigned char *map_name;
    unsigned char *window_origin;
    unsigned char *layer_anchor;
    int *slot_layer;
    int *slot_number_layer;
    unsigned char *ribbon_fields;
    unsigned char *leader_fields;
    int ribbon_width;
    int ribbon_height;

    switch (panel->state) {
        do {
    case 0:
        window_origin = (unsigned char *)panel + ((unsigned char *)&panel->window - (unsigned char *)panel);
        WindowDXSet((WindowDX *)(void *)((struct FilePassWindow *)window_origin));
        panel->window.x = 576;
        panel->window.color = panel->color;
        panel->window.width = 336;
        panel->window.parameters = shionLevelText;
        layer_anchor = (unsigned char *)panel + ((unsigned char *)&panel->window.parameters - (unsigned char *)panel);
        panel->window.height = 96;
        panel->window.state = 1;
        panel->window.y = 0;
        WindowDXMain((WindowDX *)(void *)(&panel->window));
        panel->window.state = 3;
        {
            FileMessage *message = panel->messages;
            for (i = 0; i < 2; ++i) {
                unsigned char *fields;
                int *layer;
                int field_offset;
                eMessageSet((struct EMessageSetParam *)(void *)(&panel->messages[i]), 0);
                field_offset = i * sizeof(FileMessage) +
                    ((unsigned char *)&panel->messages - (unsigned char *)panel) - 16;
                layer = (int *)(layer_anchor + field_offset);
                fields = (unsigned char *)panel + field_offset;
                *layer = panel->color + 2;
                *(unsigned char *)(fields + 16 + ((unsigned char *)&panel->messages[0].mode - (unsigned char *)&panel->messages[0])) = 32;
            }
        }
        {
            struct FileTagFont *label = panel->labels;
            char **text = msg00_5_0036D668;
            unsigned char *layer = layer_anchor +
                ((unsigned char *)&panel->labels[0].color - (unsigned char *)panel) - ((unsigned char *)&panel->window.parameters - (unsigned char *)panel);
            for (i = 0; i < 2; ++i) {
                eTagFontSet((struct ETagFont *)label++, *text++);
                *(int *)layer = panel->color + 2;
                layer += sizeof(struct FileTagFont);
            }
        }
        {
            unsigned char *layer = (unsigned char *)panel +
                ((unsigned char *)&panel->numbers[0].color - (unsigned char *)panel);
            struct FileNumber *number = panel->numbers;
            for (i = 0; i < 5; ++i) {
                eNumberSet(number++, 0);
                *(int *)layer = panel->color + 2;
                layer += sizeof(struct FileNumber);
            }
        }
        slot_layer = &panel->slot_label.color;
        eTagFontSet((struct ETagFont *)&panel->slot_label, slotNumberText);
        *slot_layer = panel->color + 2;
        slot_number_layer = (int *)(window_origin + ((unsigned char *)&panel->slot_number.color - (unsigned char *)panel) - ((unsigned char *)&panel->window - (unsigned char *)panel));
        ribbon_width = 144;
        ribbon_height = 110;
        eNumberSet(&panel->slot_number, 0);
        *slot_number_layer = panel->color + 2;
        eRibbonSet((struct ERibbonSetData *)&panel->ribbon, 1);
        {
            int field_offset = ((unsigned char *)&panel->ribbon.width - (unsigned char *)panel) - 16;
            int *layer;
            ribbon_fields = (unsigned char *)panel + field_offset;
            layer = (int *)(window_origin + field_offset);
            *(short *)(ribbon_fields + 16) = ribbon_width;
            *layer = panel->color + 1;
            *(short *)(ribbon_fields + 18) = ribbon_height;
        }
        panel->state = 2;
        /* Fall through to the first display. */
    case 2:
        if (!panel->visible)
            break;
        entry = (FileThumbnailSaveEntry *)FileObjectData + panel->slot;
        if (!entry)
            break;
        panel->window.x = panel->x + 144;
        panel->window.y = panel->y;
        WindowDXMain((WindowDX *)(void *)(&panel->window));
        {
            struct FileTagFont *widget = panel->labels;
            int row_offset = 0;
            unsigned char *fields = (unsigned char *)panel +
                ((unsigned char *)&panel->labels - (unsigned char *)panel) - 16;
            for (i = 0; i < 2; ++i) {
                *(short *)(fields + 16 + ((unsigned char *)&panel->labels[0].x - (unsigned char *)&panel->labels[0])) = panel->window.x + 67;
                *(short *)(fields + 16 + ((unsigned char *)&panel->labels[0].y - (unsigned char *)&panel->labels[0])) = panel->window.y + row_offset + 54;
                eTagFontMain((struct ETagFontMainData *)widget++);
                fields += sizeof(struct FileTagFont);
                row_offset += 18;
            }
        }
        switch (FileObjectDataSystem->state[panel->slot]) {
        case 2:
            panel->messages[1].text = 0;
            panel->messages[0].text = newFileText;
            break;
        case 4:
            panel->messages[1].text = 0;
            panel->messages[0].text = corruptDataText;
            break;
        case 3:
            if (FileWork->text_index == 0)
                panel->messages[0].text = cannotSaveText;
            else
                panel->messages[0].text = cannotLoadText;
            panel->messages[1].text = 0;
            break;
        default:
            map_name = MenuSaveMapNameGet(entry->map_number);
            panel->messages[0].text = (char *)map_name;
            panel->messages[1].text = (char *)map_name + 33;
            break;
        }
        {
            FileMessage *widget = panel->messages;
            int row_offset = 0;
            unsigned char *fields = (unsigned char *)panel +
                ((unsigned char *)&panel->messages - (unsigned char *)panel) - 16;
            for (i = 0; i < 2; ++i) {
                *(short *)(fields + 16 + ((unsigned char *)&panel->messages[0].x - (unsigned char *)&panel->messages[0])) = panel->window.x + 3;
                *(short *)(fields + 16 + ((unsigned char *)&panel->messages[0].y - (unsigned char *)&panel->messages[0])) = panel->window.y + row_offset + 3;
                eMessageMain((EMessageParam *)(void *)(widget++));
                fields += sizeof(FileMessage);
                row_offset += 24;
            }
        }
        panel->numbers[0].value = entry->packed_time;
        if (FileObjectDataSystem->state[panel->slot] == 1)
            panel->numbers[0].format = 2;
        else
            panel->numbers[0].format = 11;
        panel->numbers[1].value = entry->play_time;
        panel->numbers[1].digits = 9;
        panel->numbers[1].format = 14;
        {
            struct FileNumber *widget = panel->numbers;
            unsigned char *fields = (unsigned char *)panel +
                ((unsigned char *)&panel->numbers - (unsigned char *)panel) - 24;
            int row_offset = 0;
            for (i = 0; i < 2; ++i) {
                *(short *)(fields + 24 + ((unsigned char *)&panel->numbers[0].x - (unsigned char *)&panel->numbers[0])) = panel->window.x + 72;
                *(short *)(fields + 24 + ((unsigned char *)&panel->numbers[0].y - (unsigned char *)&panel->numbers[0])) = panel->window.y + row_offset * 2 + 54;
                eNumberMain((struct ENumberMainData *)widget++);
                fields += sizeof(struct FileNumber);
                row_offset += 9;
            }
        }
        panel->numbers[2].digits = 1;
        panel->numbers[2].value = entry->chapter;
        panel->numbers[2].format = 1;
        panel->numbers[3].digits = 2;
        panel->numbers[3].value = entry->leader;
        panel->numbers[3].format = 1;
        leader_fields = (unsigned char *)panel + ((unsigned char *)&panel->numbers[3].x - (unsigned char *)panel) - 24;
        *(short *)(leader_fields + 24) = panel->window.x + 82;
        *(short *)(leader_fields + 26) = panel->window.y - 13;
        party_index = 0;
        eNumberMain((struct ENumberMainData *)&panel->numbers[3]);
        party_count = 0;
        for (; party_index < 16 && entry->party_ids[party_index] != 0; ++party_index) {
            if (++party_count == 6)
                break;
        }
if (party_count > 0) {
            struct FileFaceSprite *face = panel->faces;
            int column_offset = 0;
            int *member = entry->party_ids;
            unsigned char *layer = (unsigned char *)panel +
                ((unsigned char *)&panel->faces[0].color - (unsigned char *)panel);
            party_index = party_count;
            do {
                eSpriteSet((struct ESprite *)face, (short)MenuFaceEpidGet(*member++, 1));
                face->x = panel->window.x + column_offset * 4 + 196;
                *(int *)layer = panel->window.color + 2;
                face->y = panel->window.y + 58;
                eSpriteMain((struct ESpriteMainData *)face++);
                layer += sizeof(struct FileFaceSprite);
                column_offset += 5;
                --party_index;
            } while (party_index != 0);
        }
        panel->ribbon.x = panel->x;
        panel->ribbon.y = panel->y - 14;
        eRibbonMain((struct ERibbonMainData *)&panel->ribbon);
        panel->slot_label.x = panel->x + 8;
        panel->slot_label.y = panel->y - 14;
        eTagFontMain((struct ETagFontMainData *)&panel->slot_label);
        panel->slot_number.digits = 2;
        panel->slot_number.value = panel->slot + 1;
        panel->slot_number.format = 1;
        panel->slot_number.x = panel->slot_label.x + 30;
        panel->slot_number.y = panel->slot_label.y + 2;
        eNumberMain((struct ENumberMainData *)&panel->slot_number);
        panel->thumbnail.packet = &entry->jpeg->dma_qwc;
        panel->thumbnail.x = panel->x + 16;
        panel->thumbnail.y = panel->y + 1;
        panel->thumbnail.color = panel->color + 2;
        panel->thumbnail.channels[0] = panel->thumbnail.channels[1] =
            panel->thumbnail.channels[2] = panel->thumbnail.channels[3] = -128;
        endPrintExtFunc(panel->thumbnail.color, 13, &panel->thumbnail);
        break;
        } while (0);
    }
}

void FileThumbnailDisp(struct FileThumbnailContext *panel);

static void tskFileData(TskObject *task, void *data)
{
    int i;
    int complete;
    short entrance_x;
    short entrance_y;
    short exit_x;
    short exit_y;
    short expanded_y;
    short closing_x;
    struct FileDataContext *view = data;

    switch (task->state) {
    case 0: {
        int offscreen_x = -512;

        view->work = 0xffff00;
        for (i = 0; i < 4; ++i) {
            view->panels[i].color = view->work;
            view->panels[i].y = 64 + i * 112;
            view->panels[i].x = offscreen_x;
            view->panels[i].slot = 0;
            view->panels[i].state = 0;
            view->panels[i].visible = 0;
        }
        view->panels[3].x = 16;
        eCursolSet((ECursol *)&view->cursor, 0);
        view->cursor.work = view->work + 2;
        view->cursor.state = 32;
        view->visible = 0;
        view->initial_value = 0;
        view->initial_flags = 0;
        view->state = 0;
        view->extra_panel = 0;
        return;
    }
    case 2:
        switch (view->state) {
        case 0:
            view->visible = 0;
            view->state = 1;
        case 1:
            if (FileWork->state == 40) {
                view->visible = 1;
                view->state = 10;
            }
            break;
        case 10:
            for (i = 0; i < 3; ++i) {
                view->panels[i].y = 64 + i * 112;
                view->panels[i].x = 528;
            }
            view->cursor.state = 32;
            view->state = 12;
        case 12: {
            int panel_index;

            complete = 0;
            entrance_x = 16;
            for (panel_index = 0; panel_index < 3; ++panel_index) {
                MoveSlide(&view->panels[panel_index].x, &entrance_x, 3.0f);
                if (view->panels[panel_index].x == entrance_x) {
                    entrance_y = 64 + panel_index * 112;
                    MoveSlide(&view->panels[panel_index].y, &entrance_y, 3.0f);
                    if (view->panels[panel_index].y == entrance_y)
                        ++complete;
                }
            }
            if (complete == 3)
                view->state = 50;
            break;
        }
        case 14: {
            int exit_complete;
            int panel_index;

            exit_complete = 0;
            exit_x = -512;
            exit_y = 64;
            for (panel_index = 0; panel_index < 3; ++panel_index) {
                if (FileWork->selected_panel != panel_index) {
                    MoveSlide(&view->panels[panel_index].x, &exit_x, 3.0f);
                    if (view->panels[panel_index].x == exit_x)
                        ++exit_complete;
                } else {
                    MoveSlide(&view->panels[panel_index].y, &exit_y, 3.0f);
                    if (view->panels[panel_index].y == exit_y)
                        ++exit_complete;
                }
            }
            if (exit_complete == 3)
                view->state = 52;
            break;
        }
        case 16: {

            expanded_y = 112;
            MoveSlide(&view->panels[FileWork->selected_panel].y, &expanded_y, 3.0f);
            if (view->panels[FileWork->selected_panel].y == expanded_y)
                view->state = 52;
            break;
        }
        case 18: {
            int exit_complete;
            int panel_index;

            exit_complete = 0;
            closing_x = -512;
            for (panel_index = 0; panel_index < 3; ++panel_index) {
                MoveSlide(&view->panels[panel_index].x, &closing_x, 3.0f);
                if (view->panels[panel_index].x == closing_x)
                    ++exit_complete;
            }
            if (exit_complete == 3)
                view->state = 0;
            break;
        }
        case 50:
            switch (FileWork->state) {
            case 36:
            case 144:
            case 145:
            case 146:
            case 147:
            case 240:
                view->state = 18;
                break;
            case 41:
            case 42:
                view->state = 14;
                break;
            case 44:
            case 45:
            case 46:
            case 80:
                view->state = 16;
                break;
            }
            if (FileWork->scroll_direction) {
                int scroll_speed;
                int scroll_index;

                for (scroll_index = 0; scroll_index < 3; ++scroll_index) {
                    int direction = FileWork->scroll_direction;

                    view->panels[scroll_index].y += direction * 112;
                }
                {
                    FileWorkBlock *scroll_work = FileWork;

                    view->panels[3].x = 16;
                    if (scroll_work->scroll_direction < 0)
                        view->panels[3].y = 288;
                    else
                        view->panels[3].y = 64;
                    scroll_speed = 14;
                    if (scroll_work->fast_scroll)
                        scroll_speed = 28;
                    for (scroll_index = 0; scroll_index < 4; ++scroll_index)
                        view->panels[scroll_index].y += scroll_speed * -FileWork->scroll_direction;
                }
                view->extra_panel = 1;
                view->state = 60;
            }
            break;
        case 52:
            switch (FileWork->state) {
            case 144:
            case 145:
            case 146:
            case 147:
            case 148:
            case 149:
            case 150:
            case 151:
            case 152:
                view->state = 18;
                break;
            case 240:
                view->state = 18;
                break;
            case 44:
            case 45:
            case 46:
                view->state = 16;
                break;
            case 40:
                view->state = 12;
                break;
            }
            break;
        case 60: {
            int scroll_speed = 14;

            if (FileWork->fast_scroll)
                scroll_speed = 28;

            for (i = 0; i < 4; ++i)
                view->panels[i].y += scroll_speed * -FileWork->scroll_direction;
            if (view->panels[0].y == 64) {
                view->extra_panel = 0;
                view->state = 50;
                FileWork->scroll_direction = 0;
            }
            break;
        }
        }
        if (view->visible) {
            FileWorkBlock *work;

            for (i = 0; i < 3; ++i)
                view->panels[i].slot = FileObjectDataNoGet(FileWork->current_slot, i);
            if (view->extra_panel) {
                if (FileWork->scroll_direction < 0)
                    view->panels[3].slot = FileObjectDataNoGet(FileWork->current_slot, 3);
                else
                    view->panels[3].slot = FileObjectDataNoGet(FileWork->current_slot, -1);
            }
            view->clip.x = 0;
            view->clip.y = 48;
            view->clip.color = view->work;
            view->clip.width = 512;
            view->clip.height = 336;
            endPrintExtFunc(view->work, 5, &view->clip);
            endPrintExtFunc(view->work, 101, &view->clip);
            for (i = 0; i < view->extra_panel + 3; ++i) {
                view->panels[i].visible = 1;
                FileThumbnailDisp(&view->panels[i]);
            }
            work = FileWork;
            view->cursor.x = view->panels[work->selected_panel].x;
            view->cursor.y = view->panels[work->selected_panel].y + 32;
            eCursolMain((ECursolMainData *)&view->cursor);
            endPrintExtFunc(view->work, 6, 0);
            endPrintExtFunc(view->work, 102, 0);
        }
        break;
    }
}

/* Defining-owner interface proposals from xgl_hdd. */
struct HddCheckState {
    int status;
    unsigned char *transfer;
    int dataSize;
};

struct HddExistRequest {
    int card;
    unsigned char *found;
};

struct HddLoadRequest {
    int card;
    void *data;
    int size;
};

struct HddSaveRequest {
    int card;
    const void *data;
    int size;
};

extern int xglMcMain(void);
extern int xglHddCheck(void);
extern int xglHddMcGetFree(void);
extern int xglHddMcExist(struct HddExistRequest *request);
extern int xglHddMcCreate(struct HddCheckState *request);
extern int xglHddMcLoad(struct HddLoadRequest *request);
extern int xglHddMcSave(const struct HddSaveRequest *request);
extern int MenuSelectMove(int selection, int count, int wrap);
extern void xglFontDebugHex(int x, int y, int value, int digits);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
/*
 * PadData as MenuFile reads it: a TU-local view of the 0xd0-byte pad object
 * (src/main/input.c lists the other views). held/pressed keep the PadPrefix
 * names at +0x28/+0x2a; the panel cursor tests 0x1000/0x4000 in the +0x30
 * halfword, and mappedRepeat is the +0x34 halfword of PadPrefix.
 */
typedef struct FileMenuPadData {
    unsigned char unmodeled_00[0x28];
    u16 half_28;
    u16 half_2a;
    u16 half_2c;
    u16 half_2e;
    u16 half_30;
    u16 mappedPressed;
    u16 mappedRepeat;
} FileMenuPadData;

extern FileMenuPadData PadData;
/* The card number as MenuFile reads it before the create request: through a
   one-member union, so the load is not ordered after the gauge stores. */
typedef union FileCardNumber {
    int number;
} FileCardNumber;
extern const char shortNumberFormat[];
extern const char memoryCardMarkerText[];
extern const char wideNumberFormat[];

void MenuFile(void) {
    unsigned char delay;
    unsigned char state;
    int card_status;
    union FileCardStatus *card;
    McRequestArguments card_request;
    union {
        struct HddExistRequest exist;
        struct HddCheckState create;
        struct HddLoadRequest load;
        struct HddSaveRequest save;
    } hdd_request;

    if (FileWork->state != FileWork->command) {
        FileWork->state = FileWork->command;
        FileWork->input_flags |= 1;
    } else {
        FileWork->input_flags &= 0xfe;
    }
    FileWork->card.word = xglMcMain();
    delay = FileWork->dispatch_delay;
    if (delay != 0) {
        FileWork->dispatch_delay = delay - 1;
        return;
    }
    state = FileWork->state;
    switch (state) {
    case 0x24: {
        signed char selection;
        unsigned char input_flags = FileWork->input_flags;
        if (input_flags & 1) {
            if (FileWork->use_hdd) {
                FileWork->device_selection = 2;
            } else {
                FileWork->device_selection = FileWork->card_number;
            }
            FileWork->hdd_status = (unsigned short) xglHddCheck();
            FileWork->dispatch_delay = 12;
            FileWork->hide_selection = 0;
            break;
        }
        else if (input_flags & 2) {
            break;
        } else {
            FileWork->device_selection = MenuSelectMove(FileWork->device_selection, 3, 0);
            if (PadData.half_2a & 0x20) {
                selection = FileWork->device_selection;
                switch (selection) {
                    case 0:
                    case 1:
                        FileWork->use_hdd = 0;
                        FileWork->card_number = FileWork->device_selection;
                        card = &FileWork->card;
                        card_status = card->status[FileWork->card_number];
                        switch (card_status) {
                        case 0:
                        case 0x81:
                        case 0x82:
                            xglSoundEffectNormalID(5, 0);
                            break;
                        case 2:
                            if (FileWork->text_index == 0) {
                                FileWork->command = 0x80;
                            } else {
                                FileWork->command = 0xb8;
                            }
                            xglSoundEffectNormalID(5, 0);
                            break;
                        case 0xff:
                            break;
                        default:
                            FileWork->command = 0x25;
                            xglSoundEffectNormalID(1, 0);
                            break;
                        }
                        break;
                    case 2:
                        xglSoundEffectNormalID(2, 0);
                        FileWork->command = 0xf0;
                        if (FileWork->text_index == 1) {
                            FileWork->result = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
            if (PadData.half_2a & 0x40) {
                FileWork->command = 0xf0;
                xglSoundEffectNormalID(2, 0);
                if (FileWork->text_index == 1) {
                    FileWork->result = 1;
                }
            }
        }
        break;
    }
    case 0x25:
    {
        int hdd_slot;
        int card_slot;
        if (FileWork->input_flags & 1) {
            if (FileWork->use_hdd) {
                FileWork->dispatch_delay = 12;
                FileWork->available_blocks = xglHddMcGetFree();
            } else {
                xglMcRequest(FileWork->card_number, 1, 0,
                             &FileWork->available_blocks);
                card_request.slot = -1;
                card_request.buffer = (int)FileExistCheckWork;
                xglMcRequest(FileWork->card_number, 2, &card_request,
                             &FileWork->directory_result);
                FileWork->menu_substate = 0;
            }
        } else if (FileWork->use_hdd) {
            if (FileWork->available_blocks < 0) {
                if (FileWork->text_index == 0) {
                    switch (FileWork->available_blocks) {
                    case -1:
                        FileWork->command = 0x92;
                        break;
                    case -2:
                        FileWork->command = 0x95;
                        break;
                    case -3:
                        FileWork->command = 0x94;
                        break;
                    case -4:
                        FileWork->command = 0x96;
                        break;
                    }
                } else {
                    switch (FileWork->available_blocks) {
                    case -1:
                        FileWork->command = 0x92;
                        break;
                    case -2:
                    case -3:
                    case -4:
                        FileWork->command = 0xb8;
                        break;
                    }
                }
                xglSoundEffectNormalID(5, 0);
                FileWork->dispatch_delay = 16;
                break;
            }
            hdd_request.exist.card = -1;
            hdd_request.exist.found = FileExistCheckWork;
            FileWork->directory_result = xglHddMcExist(&hdd_request.exist);
            if (FileWork->directory_result == 0) {
                if (FileWork->text_index == 0) {
                if (FileWork->available_blocks < 225) {
                    FileWork->command = 0xb0;
                    xglSoundEffectNormalID(5, 0);
                } else {
                    for (hdd_slot = 0; hdd_slot < 99; hdd_slot++) {
                        if (FileWork->available_blocks >= 225) {
                            FileObjectDataSystem->state[hdd_slot] = 2;
                            FileWork->available_blocks -= 225;
                        } else {
                            FileObjectDataSystem->state[hdd_slot] = 3;
                        }
                    }
                    FileExistCheckWork[100] = 0;
                    FileWork->command = 0x27;
                }
                } else {
                    FileWork->command = 0xb8;
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (FileWork->directory_result < 0) {
                xglSoundEffectNormalID(5, 0);
                FileWork->command = 0x92;
            } else {
                FileWork->selected_slot = 0;
                FileWork->slot_count = 99;
                if (FileWork->directory_result == 0 &&
                    FileWork->available_blocks < 225) {
                    if (FileWork->text_index == 0) {
                        FileWork->command = 0xb0;
                    } else {
                        FileWork->command = 0xb8;
                    }
                    xglSoundEffectNormalID(5, 0);
                    break;
                }
                for (hdd_slot = 0; hdd_slot < 99; hdd_slot++) {
                    if (FileExistCheckWork[hdd_slot] != 0) {
                        FileObjectDataSystem->state[hdd_slot] = 1;
                    } else if (FileWork->available_blocks >= 225) {
                        FileObjectDataSystem->state[hdd_slot] = 2;
                        FileWork->available_blocks -= 225;
                    } else {
                        FileObjectDataSystem->state[hdd_slot] = 3;
                    }
                }
                if (FileWork->text_index == 0) {
                    FileWork->command = 0x27;
                } else {
                    switch (FileWork->directory_result) {
                    default:
                        FileWork->command = 0x27;
                        break;
                    case 0:
                        FileWork->command = 0xb8;
                        xglSoundEffectNormalID(5, 0);
                        break;
                    }
                }
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 1:
                if (FileWork->menu_substate != 0) {
                    FileWork->selected_slot = 0;
                    FileWork->slot_count = 99;
                    if (FileWork->directory_result == 0 &&
                        FileWork->available_blocks < 165) {
                        if (FileWork->text_index == 0) {
                            FileWork->command = 0xb0;
                        } else {
                            FileWork->command = 0xb8;
                        }
                        xglSoundEffectNormalID(5, 0);
                        break;
                    } else {
                        for (card_slot = 0; card_slot < 99; card_slot++) {
                            if (FileExistCheckWork[card_slot] != 0) {
                                FileObjectDataSystem->state[card_slot] = 1;
                            } else if (FileWork->available_blocks >= 165) {
                                FileObjectDataSystem->state[card_slot] = 2;
                                FileWork->available_blocks -= 165;
                            } else {
                                FileObjectDataSystem->state[card_slot] = 3;
                            }
                        }
                        if (FileWork->directory_result < 0) {
                            FileWork->command = 0x91;
                            xglSoundEffectNormalID(5, 0);
                        } else {
                            if (FileWork->text_index == 0) {
                                FileWork->command = 0x27;
                            } else {
                                switch (FileWork->directory_result) {
                                default:
                                    FileWork->command = 0x27;
                                    break;
                                case 0:
                                    FileWork->command = 0xb8;
                                    xglSoundEffectNormalID(5, 0);
                                    break;
                                }
                            }
                        }
                    }
                }
                FileWork->menu_substate = 1;
                break;
            case 0:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            case 0xff:
                break;
            default:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            }
        }
        break;
    }
    case 0xff:
        break;
    case 0x27:
        if (FileWork->input_flags & 1) {
            FileWork->selected_slot = FileExistCheckWork[100];
            FileWork->panel_count = 3;
            FileWork->selected_panel = 0;
            FileWork->scroll_direction = 0;
            FileWork->current_slot = FileWork->selected_slot;
            FileObjectDataClear();
            FileObjectDataSystem->unmodeled_00[1] = FileWork->current_slot;
        } else if (FileWork->use_hdd) {
            int load_result = FileObjectAllLoad();
            if (load_result == 0) {
                if (FileWork->hdd_result >= 0) {
                    FileWork->slot_count = 99;
                    FileObjectJpegSet();
                    FileWork->command = 0x28;
                    FileWork->dispatch_delay = 12;
                }
            }
            if (FileWork->hdd_result != 0) {
                FileWork->command = 0x92;
                xglSoundEffectNormalID(5, 0);
            }
        } else {
            unsigned short loaded_card_status;
            int load_result = FileObjectAllLoad();
            if (load_result == 0) {
                if (FileWork->hdd_result >= 0) {
                    FileWork->slot_count = 99;
                    FileObjectJpegSet();
                    FileWork->command = 0x28;
                    FileWork->dispatch_delay = 12;
                }
            }
            if (FileWork->hdd_result >= 0) {
                break;
            }
            card = &FileWork->card;
            loaded_card_status = card->status[FileWork->card_number];
            switch (loaded_card_status) {
            default:
                FileWork->command = 0x92;
                FileWork->slot_count = 0;
                xglSoundEffectNormalID(5, 0);
                break;
            case 0xff:
                break;
            case 0:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            }
        }
        break;
    case 0x28:
    {
        if (FileWork->input_flags & 1) {
            FileWork->scroll_direction = 0;
        }
        if ((FileWork->input_flags & 2) == 0) {
            if (FileWork->scroll_direction == 0) {
                unsigned short action = 0;

                if (FileWork->fast_scroll != 0) {
                    if (FileWork->scroll_repeat_count == 10) {
                        FileWork->fast_scroll = 0;
                    } else {
                        FileWork->scroll_repeat_count++;
                        action = FileWork->scroll_repeat_direction < 0
                                     ? 0x1000
                                     : 0x4000;
                    }
                    FileWork->menu_substate = 0;
                } else {
                    if (FileWork->menu_substate == 0) {
                        u16 menu_input = PadData.half_30;

                        if (menu_input & 0x1000) {
                            if (FileWork->selected_panel == 0 ||
                                (PadData.mappedRepeat & 0x1000)) {
                                action = 0x1000;
                            }
                        } else if (menu_input & 0x4000) {
                            if (FileWork->selected_panel == 2 ||
                                (PadData.mappedRepeat & 0x4000)) {
                                action = 0x4000;
                            }
                        } else {
                            action = 0x40;
                            if ((menu_input & 0x40) == 0) {
                                action = menu_input & 0x20;
                            }
                        }
                        if (action != 0) {
                            FileWork->menu_substate = 4;
                        }
                    } else {
                        FileWork->menu_substate--;
                    }
                }
                if (action == 0x1000) {
                    FileWork->selected_slot = FileObjectDataNoGet(
                        FileWork->selected_slot, -1);
                    if (FileWork->selected_panel == 0) {
                        FileWork->scroll_direction = -1;
                        FileWork->current_slot = FileObjectDataNoGet(
                            FileWork->current_slot, -1);
                        FileObjectJpegSet();
                    } else {
                        FileWork->selected_panel--;
                    }
                    xglSoundEffectNormalID(3, 0);
                }
                if (action == 0x4000) {
                    FileWork->selected_slot = FileObjectDataNoGet(
                        FileWork->selected_slot, 1);
                    if (FileWork->selected_panel ==
                        (signed char) FileWork->panel_count - 1) {
                        FileWork->scroll_direction = 1;
                        FileWork->current_slot = FileObjectDataNoGet(
                            FileWork->current_slot, 1);
                        FileObjectJpegSet();
                    } else {
                        FileWork->selected_panel++;
                    }
                    xglSoundEffectNormalID(3, 0);
                }
                if (action == 0x40) {
                    FileWork->command = 0x24;
                    xglSoundEffectNormalID(2, 0);
                }
                if (action == 0x20) {
                    unsigned char object_status =
                        FileObjectDataSystem->state[FileWork->selected_slot];
                    switch (object_status) {
                    case 2:
                        if (FileWork->text_index == 0) {
                            FileWork->command = 0x2a;
                            xglSoundEffectNormalID(1, 0);
                        } else {
                            FileWork->command = 0x29;
                            xglSoundEffectNormalID(5, 0);
                        }
                        break;
                    case 3:
                    case 4:
                        FileWork->command = 0x29;
                        xglSoundEffectNormalID(5, 0);
                        break;
                    default:
                        if (FileWork->text_index == 0) {
                            FileWork->command = 0x2a;
                            xglSoundEffectNormalID(1, 0);
                        } else {
                            signed char selected_slot = FileWork->selected_slot;
                            FileObjectEntryView *selected_entry =
                                &((FileObjectEntryView *)FileObjectData)[selected_slot];
                            if (selected_entry->map_number == 10) {
                                xglSoundEffectNormalID(5, 0);
                            } else {
                                FileWork->command = 0x2a;
                                xglSoundEffectNormalID(1, 0);
                            }
                        }
                        break;
                    }
                }
            } else {
                FileWork->menu_substate = 0;
            }
        }
        if (FileWork->use_hdd == 0) {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 1:
            case 0xff:
                break;
            case 0:
            default:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            }
        }
        break;
    }
    case 0x29: {
        if (FileWork->input_flags & 1) {
            FileWork->dispatch_delay = 12;
        } else if (FileWork->use_hdd) {
            if (PadData.half_2a & 0x20) {
                FileWork->command = 0x28;
                FileWork->dispatch_delay = 12;
                xglSoundEffectNormalID(1, 0);
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 1:
                if (PadData.half_2a & 0x20) {
                    FileWork->command = 0x28;
                    FileWork->dispatch_delay = 12;
                    xglSoundEffectNormalID(1, 0);
                }
                break;
            case 0xff:
                break;
            case 0:
            default:
                xglSoundEffectNormalID(5, 0);
                FileWork->command = 0x91;
                break;
            }
        }
        break;
    }
    case 0x2a: {
        if (FileWork->input_flags & 1) {
            FileWork->dispatch_delay = 10;
            if (FileWork->text_index == 0) {
                FileWork->action_selection = 1;
            } else {
                FileWork->action_selection = 0;
            }
            if (!FileWork->use_hdd) {
                xglMcRequest(FileWork->card_number, 1, 0,
                             &FileWork->available_blocks);
            }
        } else if (FileWork->use_hdd) {
            if (!(FileWork->input_flags & 2)) {
                FileWork->action_selection =
                    MenuSelectMove(FileWork->action_selection, 2, 0);
                if (PadData.half_2a & 0x20) {
                    if (FileWork->action_selection == 0) {
                        if (FileWork->text_index != 0) {
                            FileWork->command = 0x50;
                        } else {
                            switch (FileObjectDataSystem->state[FileWork->selected_slot]) {
                            case 2:
                                FileWork->command = 0x2d;
                                break;
                            default:
                                FileWork->command = 0x2d;
                                break;
                            }
                        }
                        xglSoundEffectNormalID(1, 0);
                    } else {
                        FileWork->command = 0x28;
                        FileWork->dispatch_delay = 12;
                        xglSoundEffectNormalID(2, 0);
                    }
                }
                if (PadData.half_2a & 0x40) {
                    FileWork->command = 0x28;
                    FileWork->dispatch_delay = 12;
                    xglSoundEffectNormalID(2, 0);
                }
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 0:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            case 1:
            if (!(FileWork->input_flags & 2)) {
                FileWork->action_selection =
                    MenuSelectMove(FileWork->action_selection, 2, 0);
                if (PadData.half_2a & 0x20) {
                        if (FileWork->action_selection == 0) {
                            if (FileWork->text_index != 0) {
                                FileWork->command = 0x50;
                            } else if (FileObjectDataSystem->state[FileWork->selected_slot] != 2 ||
                                       FileWork->available_blocks >= 165) {
                                FileWork->command = 0x2d;
                            } else {
                                FileWork->command = 0xb0;
                            }
                        xglSoundEffectNormalID(1, 0);
                    } else {
                        FileWork->command = 0x28;
                        FileWork->dispatch_delay = 12;
                        xglSoundEffectNormalID(2, 0);
                    }
                }
                if (PadData.half_2a & 0x40) {
                    FileWork->dispatch_delay = 12;
                    FileWork->command = 0x28;
                    xglSoundEffectNormalID(2, 0);
                }
            }
                break;
            case 0xff:
                break;
            default:
                FileWork->command = 0x90;
                break;
            }
        }
        break;
    }
    case 0x2c:
    {
        int hdd_existing_slot;
        int card_existing_slot;
        if (FileWork->input_flags & 1) {
            if (FileWork->use_hdd) {
                FileWork->dispatch_delay = 16;
            } else {
                card_request.slot = FileWork->selected_slot;
                card_request.buffer = (int)SaveData;
                card_request.size = 0x163b0;
                xglMcRequest(FileWork->card_number, 6, &card_request,
                             &FileWork->hdd_result);
                xglMcRequest(FileWork->card_number, 1, 0,
                             &FileWork->available_blocks);
                FileWork->menu_substate = 0;
                tyaDrawGauge(&gauge);
            }
        } else if (FileWork->use_hdd) {
            hdd_request.save.card = FileWork->selected_slot;
            hdd_request.save.data = SaveData;
            hdd_request.save.size = 0x163b0;
            FileWork->directory_result = xglHddMcSave(&hdd_request.save);
            if (FileWork->directory_result != 0) {
                FileWork->command = 0x90;
                xglSoundEffectNormalID(5, 0);
            } else {
                xglSoundEffectNormalID(0x19, 0);
                FileWork->available_blocks = xglHddMcGetFree();
                *FileSaveData = *(FileSaveRecord *) SaveData;
                hdd_existing_slot = 0;
                FileObjectDataInput(FileWork->selected_slot, 1);
                FileObjectDataSystem->state[FileWork->selected_slot] = 1;
                FileObjectJpegDecChange(FileWork->selected_slot);
                    for (; hdd_existing_slot < 99; hdd_existing_slot++) {
                        if (FileObjectDataSystem->state[hdd_existing_slot] == 2) {
                        FileWork->available_blocks -= 225;
                    }
                }
                for (hdd_existing_slot = 0; hdd_existing_slot < 99; hdd_existing_slot++) {
                    if (FileObjectDataSystem->state[hdd_existing_slot] == 3 &&
                        FileWork->available_blocks >= 225) {
                        FileObjectDataInput(hdd_existing_slot, 2);
                        FileObjectDataSystem->state[hdd_existing_slot] = 2;
                        FileObjectJpegDecChange(hdd_existing_slot);
                        FileWork->available_blocks -= 225;
                    }
                }
                FileWork->command = 0x2e;
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            if (card_status != 1) {
                if (card_status != 0xff) {
                    FileWork->command = 0x90;
                    xglSoundEffectNormalID(5, 0);
                } else {
                    tyaDrawGauge(&gauge);
                }
            } else if (FileWork->menu_substate == 0) {
                xglSoundEffectNormalID(0x19, 0);
                *FileSaveData = *(FileSaveRecord *) SaveData;
                FileObjectDataInput(FileWork->selected_slot, 1);
                FileObjectDataSystem->state[FileWork->selected_slot] = 1;
                FileObjectJpegDecChange(FileWork->selected_slot);
                FileWork->menu_substate = 1;
                tyaDrawGauge(&gauge);
            } else {
                for (card_existing_slot = 0; card_existing_slot < 99; card_existing_slot++) {
                    if (FileObjectDataSystem->state[card_existing_slot] == 2) {
                        FileWork->available_blocks -= 165;
                    }
                }
                for (card_existing_slot = 0; card_existing_slot < 99; card_existing_slot++) {
                    if (FileObjectDataSystem->state[card_existing_slot] == 3 &&
                        FileWork->available_blocks >= 165) {
                        FileObjectDataInput(card_existing_slot, 2);
                        FileObjectDataSystem->state[card_existing_slot] = 2;
                        FileObjectJpegDecChange(card_existing_slot);
                        FileWork->available_blocks -= 165;
                    }
                }
                FileWork->command = 0x2e;
            }
        }
        break;
    }
    case 0x2d: {
        int create_result;

        if (FileWork->input_flags & 1) {
            if (FileWork->use_hdd) {
                FileWork->menu_substate = 16;
            } else {
                card_request.slot = FileWork->selected_slot;
                card_request.buffer = (int)FileCardDataBuf;
                FileSaveDataPush();
                gauge.x = 128;
                gauge.y = 296;
                gauge.vertex_attribute = 0xffffffffU;
                gauge.width = 256;
                gauge.height = 8;
                gauge.current = 0;
                gauge.maximum = 7;
                gauge.filled_color.red = 161;
                gauge.filled_color.green = 232;
                gauge.filled_color.blue = 205;
                gauge.filled_color.alpha = 128;
                gauge.unfilled_color.red = 64;
                gauge.unfilled_color.green = 64;
                gauge.unfilled_color.blue = 64;
                gauge.unfilled_color.alpha = 128;
                xglMcRequest(((FileCardNumber *)&FileWork->card_number)->number, 8,
                             &card_request,
                             &FileWork->directory_result);
                tyaDrawGauge(&gauge);
                break;
            }
        }
        if (FileWork->use_hdd) {
            if (FileWork->menu_substate == 0) {
                hdd_request.create.status = FileWork->selected_slot;
                hdd_request.create.transfer = FileCardDataBuf;
                hdd_request.create.dataSize = 0x163b0;
                FileSaveDataPush();
                create_result = xglHddMcCreate(&hdd_request.create);
                FileWork->directory_result = create_result;
                switch (create_result) {
                case 0:
                    FileWork->command = 0x2c;
                    break;
                case -13:
                    xglSoundEffectNormalID(5, 0);
                    FileWork->command = 0x94;
                    FileWork->dispatch_delay = 16;
                    break;
                case -9:
                    xglSoundEffectNormalID(5, 0);
                    FileWork->command = 0x95;
                    FileWork->dispatch_delay = 16;
                    break;
                case -15:
                    xglSoundEffectNormalID(5, 0);
                    FileWork->command = 0x96;
                    FileWork->dispatch_delay = 16;
                    break;
                default:
                    xglSoundEffectNormalID(5, 0);
                    FileWork->command = 0x90;
                    FileWork->dispatch_delay = 16;
                    break;
                }
            } else {
                FileWork->menu_substate--;
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            default:
                FileWork->command = 0x90;
                xglSoundEffectNormalID(5, 0);
                break;
            case 0xff:
                gauge.current = FileWork->directory_result;
                tyaDrawGauge(&gauge);
                break;
            case 1:
                if (FileWork->directory_result < 0) {
                    FileWork->command = 0x90;
                    xglSoundEffectNormalID(5, 0);
                } else {
                    FileWork->command = 0x2c;
                }
                gauge.current = 7;
                tyaDrawGauge(&gauge);
                break;
            }
        }
        break;
    }
    case 0x2e: {
        if (FileWork->use_hdd) {
            if (PadData.half_2a & 0x20) {
                FileWork->command = 0x28;
                xglSoundEffectNormalID(1, 0);
                FileWork->menu_substate = 16;
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 1:
                if (PadData.half_2a & 0x20) {
                    FileWork->command = 0x28;
                    xglSoundEffectNormalID(1, 0);
                    FileWork->menu_substate = 16;
                }
                break;
            case 0xff:
                break;
            case 0:
            default:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            }
        }
        break;
    }
    case 0x50:
    {
        if (FileWork->input_flags & 1) {
            if (FileWork->use_hdd) {
                FileWork->dispatch_delay = 16;
            } else {
                card_request.slot = FileWork->selected_slot;
                card_request.buffer = (int)FileSaveData;
                card_request.size = 0x163b0;
                memset(FileSaveData, 0, 0x163b0);
                xglMcRequest(FileWork->card_number, 4, &card_request,
                             &FileWork->directory_result);
            }
        } else {
            if (FileWork->use_hdd) {
                hdd_request.load.card = FileWork->selected_slot;
                hdd_request.load.data = FileSaveData;
                hdd_request.load.size = 0x163b0;
                memset(FileSaveData, 0, 0x163b0);
                FileWork->directory_result = xglHddMcLoad(&hdd_request.load);
                if (FileWork->directory_result != 0) {
                    FileWork->command = 0x90;
                    xglSoundEffectNormalID(5, 0);
                    break;
                }
                *(FileSaveRecord *) SaveData = *FileSaveData;
                FileVersionCheck((SaveDataHeader *) SaveData);
                xglSoundEffectNormalID(0x1a, 0);
                FileWork->command = 0x52;
            } else {
                card = &FileWork->card;
                card_status = card->status[FileWork->card_number];
                if (card_status != 1) {
                    if (card_status != 0xff) {
                        FileWork->command = 0x90;
                        xglSoundEffectNormalID(5, 0);
                    }
                    break;
                }
                *(FileSaveRecord *) SaveData = *FileSaveData;
                FileVersionCheck((SaveDataHeader *) SaveData);
                xglSoundEffectNormalID(0x1a, 0);
                FileWork->command = 0x52;
            }
        }
        break;
    }
    case 0x52: {
        if (FileWork->use_hdd) {
            if (PadData.half_2a & 0x20) {
                FileWork->command = 0xf0;
                xglSoundEffectNormalID(1, 0);
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 1:
                if (PadData.half_2a & 0x20) {
                    FileWork->command = 0xf0;
                    xglSoundEffectNormalID(1, 0);
                }
                break;
            case 0xff:
                break;
            case 0:
            default:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            }
        }
        break;
    }
    case 0x80: {
        if (FileWork->use_hdd) {
            if (PadData.half_2a & 0x20) {
                FileWork->command = 0xa0;
                xglSoundEffectNormalID(1, 0);
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 2:
                if (PadData.half_2a & 0x20) {
                    xglSoundEffectNormalID(1, 0);
                    FileWork->command = 0x82;
                }
                break;
            case 0:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            case 0xff:
                break;
            default:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            }
        }
        break;
    }
    case 0x82: {
        if (FileWork->input_flags & 1) {
            FileWork->action_selection = 1;
        }
        card = &FileWork->card;
        card_status = card->status[FileWork->card_number];
        switch (card_status) {
        case 2:
            FileWork->action_selection =
                MenuSelectMove(FileWork->action_selection, 2, 0);
            if (PadData.half_2a & 0x20) {
                if (FileWork->action_selection == 0) {
                    xglMcRequest(FileWork->card_number, 9, 0,
                                 &FileWork->available_blocks);
                    FileWork->command = 0x84;
                } else {
                    FileWork->command = 0xa0;
                }
                xglSoundEffectNormalID(1, 0);
            }
            if (PadData.half_2a & 0x40) {
                FileWork->command = 0x24;
                xglSoundEffectNormalID(2, 0);
            }
            break;
        case 0:
            FileWork->command = 0x91;
            xglSoundEffectNormalID(5, 0);
            break;
        case 0xff:
            break;
        default:
            FileWork->command = 0x91;
            xglSoundEffectNormalID(5, 0);
            break;
        }
        break;
    }
    case 0x84: {
        card = &FileWork->card;
        card_status = card->status[FileWork->card_number];
        switch (card_status) {
        case 1:
            FileWork->command = 0x86;
            xglSoundEffectNormalID(1, 0);
            break;
        case 2:
        case 0xff:
            break;
        default:
            FileWork->command = 0x87;
            xglSoundEffectNormalID(5, 0);
            break;
        }
        break;
    }
    case 0x86: {
        if (FileWork->dispatch_delay == 0) {
            if (PadData.half_2a & 0x20) {
                FileWork->command = 0x25;
                FileWork->hide_selection = 1;
            }
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case 0:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            case 1:
            case 0xff:
                break;
            default:
                FileWork->command = 0x91;
                xglSoundEffectNormalID(5, 0);
                break;
            }
        }
        break;
    }
    case 0x87:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & 0x20) {
            FileWork->command = 0xa0;
            xglSoundEffectNormalID(1, 0);
        }
        break;
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & 0x20) {
            FileWork->command = 0xa0;
            xglSoundEffectNormalID(1, 0);
        }
        break;
    case 0xa0:
        if (FileWork->input_flags & 1) {
            FileWork->action_selection = 1;
            FileWork->dispatch_delay = 8;
            xglHddCheck();
        }
        if (FileWork->dispatch_delay == 0) {
            FileWork->action_selection =
                MenuSelectMove(FileWork->action_selection, 2, 0);
            if (PadData.half_2a & 0x20) {
                if (FileWork->action_selection == 0) {
                    FileWork->result = 1;
                    FileWork->command = 0xf0;
                    xglSoundEffectNormalID(2, 0);
                } else {
                    FileWork->command = 0x24;
                    xglSoundEffectNormalID(1, 0);
                }
            }
        }
        break;
    case 0xa1:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & 0x20) {
            FileWork->command = 0x24;
            xglSoundEffectNormalID(1, 0);
        }
        break;
    case 0xb0:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & 0x20) {
            FileWork->command = 0xa0;
            xglSoundEffectNormalID(1, 0);
        }
        break;
    case 0xb8:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & 0x20) {
            FileWork->command = 0xa0;
            xglSoundEffectNormalID(1, 0);
        }
        break;
    case 0xf0:
        if (FileWork->input_flags & 1) {
            FileWork->menu_substate = 16;
        }
        if (FileWork->menu_substate == 0) {
            FileWork->command = 0xff;
        } else {
            FileWork->menu_substate--;
        }
        break;
    default:
        break;
    }
    FileWork->input_flags &= 0xfd;
    xglFontDebugHex(0, 0x48, FileWork->state, 2);
    xglFontDebugHex(0, 0x50, FileWork->command, 2);
    xglFontDebugHex(0, 0x58, FileWork->input_flags, 2);
    xglFontDebugHex(0, 0x60, FileWork->menu_substate, 2);
    xglFontDebugPrintf(0, 0x68, shortNumberFormat,
                       FileWork->debug_value_11);
    xglFontDebugPrintf(0x10, 0x68, shortNumberFormat,
                       FileWork->debug_value_12);
    xglFontDebugPrintf(0, 0x70, shortNumberFormat,
                       FileWork->device_selection);
    xglFontDebugPrintf(0x10, 0x70, shortNumberFormat,
                       FileWork->debug_value_14);
    xglFontDebugPrintf(0, 0x78, shortNumberFormat,
                       FileWork->debug_value_16);
    xglFontDebugPrintf(0x10, 0x78, shortNumberFormat,
                       FileWork->debug_value_17);
    xglFontDebugPrintf(0, 0x80, shortNumberFormat,
                       FileWork->selected_panel);
    xglFontDebugPrintf(0x10, 0x80, shortNumberFormat,
                       (signed char) FileWork->panel_count);
    xglFontDebugPrintf(0, 0x88, shortNumberFormat,
                       FileWork->scroll_direction);
    xglFontDebugPrintf(0x10, 0x88, shortNumberFormat,
                       FileWork->debug_value_1b);
    xglFontDebugPrintf(0, 0x90, shortNumberFormat,
                       FileWork->action_selection);
    xglFontDebugPrintf(0, 0x98, shortNumberFormat,
                       FileWork->selected_slot);
    xglFontDebugPrintf(0x10, 0x98, shortNumberFormat,
                       FileWork->slot_count);
    xglFontDebugPrintf(0x20, 0x30, memoryCardMarkerText);
    xglFontDebugHex(0x20, 0x38, FileWork->card.status[0], 2);
    xglFontDebugHex(0x20, 0x40, FileWork->card.status[1], 2);
    xglFontDebugPrintf(0x20, 0x50, wideNumberFormat,
                       FileWork->available_blocks);
    xglFontDebugPrintf(0x20, 0x58, wideNumberFormat,
                       FileWork->directory_result);
    xglFontDebugPrintf(0x20, 0x60, wideNumberFormat,
                       FileWork->hdd_result);
    xglFontDebugPrintf(0x20, 0x68, wideNumberFormat,
                       FileWork->request_result);
}

void endBackTexDraw(unsigned char *color)
{
    signed char rgba[4];
    XglPacket *packet;
    int i;
    FileBackTexturePacket *draw = (FileBackTexturePacket *)0x70000000;

    packet = xglPacketGetCurrent();
    if (color == (unsigned char *)-1) {
        rgba[0] = rgba[1] = rgba[2] = rgba[3] = -128;
    } else {
        for (i = 0; i < 4; ++i)
            rgba[i] = color[i];
    }
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    draw->gif_tag = 0xa08b400000008001ULL;
    draw->registers = 0xe53531eeeeULL;
    draw->texture_flush = 0;
    draw->texture_flush_register = 63;
    draw->depth_buffer = 0x131000000ULL;
    draw->depth_buffer_register = 78;
    draw->texture = 0x2000000640000000ULL | (0x24020000 | (sRender.draw_back_value << 5));
    draw->texture_register = 6;
    draw->pixel_test = 0x31001;
    draw->pixel_test_register = 71;
    draw->red = rgba[0];
    draw->green = rgba[1];
    draw->blue = rgba[2];
    draw->alpha_color = rgba[3];
    draw->u0 = 0;
    draw->v0 = 0;
    draw->uv0_zero = 0;
    draw->x0 = 28664;
    draw->y0 = 29176;
    draw->z0_and_adc = 0;
    draw->u1 = 8192;
    draw->v1 = 7168;
    draw->uv1_zero = 0;
    draw->x1 = 0x8ff8;
    draw->y1 = 0x8df8;
    draw->z1_and_adc = 0;
    draw->restored_depth_buffer = 0x31000000;
    draw->restored_depth_buffer_register = 78;
    sceVif1PkAddDirectDataN(packet, draw, 11);
    sceVif1PkCloseDirectHLCode(packet);
}

int MenuFileMain(int menu_mode)
{
    extern void exit(int status) __attribute__((noreturn));
    unsigned char *task_work;
    int buffer_end;
    XglTaskPrefix *entry;
    FileObjectEntryView *images;

    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.copy_width << 5,
                           sRender.copy_height << 5);
    GameLoopState.runtime_flags |= 0x04000000;
    xglSleep();
    endPrintInit();
    GameLoopState.runtime_flags |= 0x04000000;
    MenuCfTaikiPush();
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.copy_height << 5,
                           sRender.draw_back_value << 5);
    endBackTexDraw((unsigned char *)-1);
    xglSleep();

    FileWork = (FileWorkBlock *)(((unsigned int)MenuWorkEndGet() + 0x7f) & ~0x7f);
    task_work = (unsigned char *)(FileWork + 1);
    memset(FileWork, 0, sizeof(*FileWork));
    task_work = (unsigned char *)MenuBgTaskInit(task_work, 1);
    FileTask = (XglTaskScheduler *)task_work;
    task_work = xglTaskInitial(task_work, 0x10, 0);

    entry = xglTaskEntryNext(FileTask, (int (*)(XglTaskPrefix *))tskTskMain,
                             FileTask ? FileTask->active_tail : 0);
    TskObjectSet((TskObject *)entry, MenuFilePas, task_work);
    task_work += 0x3cc;
    entry = xglTaskEntryNext(FileTask, (int (*)(XglTaskPrefix *))tskTskMain,
                             FileTask ? FileTask->active_tail : 0);
    TskObjectSet((TskObject *)entry, MenuFileInfo, task_work);
    task_work += 0x460;
    entry = xglTaskEntryNext(FileTask, (int (*)(XglTaskPrefix *))tskTskMain,
                             FileTask ? FileTask->active_tail : 0);
    TskObjectSet((TskObject *)entry, tskFileSelect, task_work);
    task_work += 0x1e8c;
    entry = xglTaskEntryNext(FileTask, (int (*)(XglTaskPrefix *))tskTskMain,
                             FileTask ? FileTask->active_tail : 0);
    TskObjectSet((TskObject *)entry, (TskObjectWorker)tskFileEx, task_work);
    task_work += 0x50;
    entry = xglTaskEntryNext(FileTask, (int (*)(XglTaskPrefix *))tskTskMain,
                             FileTask ? FileTask->active_tail : 0);
    buffer_end = 0x01000000;
    TskObjectSet((TskObject *)entry, tskFileData, task_work);

    FileCardDataBuf = (unsigned char *)buffer_end;
    buffer_end += 0x12000;
    FileObjectDataSystem = (FileObjectSystem *)buffer_end;
    memset(FileObjectDataSystem, 0, 0x258);
    task_work += 0x249c;
    {
        int card;

        for (card = 0; card < 99; ++card)
            FileObjectDataSystem->card_status[card] = -1;
    }

    FileJpegDec = (unsigned char *)(buffer_end + 0x800);
    memset(FileJpegDec, 0, 0x46500);
    FileObjectData = (unsigned char *)((buffer_end + 0x800 + 0x46500 + 0x7ff) & ~0x7ff);
    memset(FileObjectData, 0, 0x696a0);
    buffer_end = ((buffer_end + 0x800 + 0x46500 + 0x7ff) & ~0x7ff) + 0x696a0;
    images = (FileObjectEntryView *)FileObjectData;
    {
        int i;

        for (i = 0; i <= 2; ++i) {
            FileObjectEntryView *leader_images;
            FileObjectEntryView *next_images;

            images[i + 99].chapter = -1;
            leader_images = (FileObjectEntryView *)FileObjectData;
            leader_images[i + 99].leader = -1;
            next_images = (FileObjectEntryView *)FileObjectData;
            next_images[i + 99].play_time = -1;
            images = next_images;
        }
    }
    FileSaveData = (FileSaveRecord *)((buffer_end + 0x7ff) & ~0x7ff);
    FileExistCheckWork = (void *)(((unsigned int)FileSaveData + 0x16baf) & ~0x7ff);
    memset(FileExistCheckWork, 0, 0x2000);
    if (menu_mode == 0) {
        FileObjectImages *objects = (FileObjectImages *)FileObjectData;
        unsigned char *thumbnail = objects->current.thumbnail;

        memset(thumbnail, 0, sizeof(objects->current.thumbnail));
        if (GameSnapShotSaveThumbnail(30, ((FileObjectImages *)FileObjectData)->current.thumbnail) > 0x1000)
            exit(1);
    }
    MenuLoadInit();
    MenuFontLoad(1, 1);
    WindowTexLoad(0, 0x10001);
    WindowTexLoad(0, 0x10002);
    task_work = MenuSaveMapTextLoad(task_work, 1);
    if (menu_mode == 0)
        MenuLoadFile(cardDataNewPath, FileCardDataBuf);
    MenuLoadFile(newDataThumbnailPath, ((FileObjectImages *)FileObjectData)->templates[0].thumbnail);
    MenuLoadFile(unusedDataThumbnailPath, ((FileObjectImages *)FileObjectData)->templates[1].thumbnail);
    MenuLoadFile(brokenDataThumbnailPath, ((FileObjectImages *)FileObjectData)->templates[2].thumbnail);
    MenuWorkEndCheck(task_work);
    if (menu_mode == 0)
        xglSoundEffectNormalID(1, 0);
    while (MenuLoadSync() != 0) {
        MenuBgTaskMain();
        xglSleep();
    }
    FileWork->text_index = menu_mode;
    FileWork->command = 0x24;
    FileWork->input_flags = 2;
    FileWork->result = 0;
    for (;; endPrintExtFunc(0, 100, 0), MenuBgTaskMain(),
            xglTaskExecute(FileTask), xglSleep()) {
        GameSnapShotCheck();
        if (FileWork->state != 0xff) {
            MenuFile();
        } else {
            break;
        }
    }
    MenuBgTaskBreak();
    xglTaskExecute(FileTask);
    MenuCfTaikiPop();
    MenuFontLoad(0, 0);
    WindowTexLoad(0, 0);
    if (menu_mode == 0)
        GameCfPlayerLoadResource(0);
    MapChangeFadeSet();
    GameLoopState.runtime_flags &= 0xfbffffff;
    return FileWork->result == 0;
}

extern char MenuFileMemoryCardSlot2Path[24];
extern char MenuFileLoadText[8];
extern char MenuFileFormatText[8];
extern char MenuFileSaveText[8];
extern char MenuFileHardDiskDrivePath[24];
extern char MenuFileMemoryCardSlot1Path[32];
extern const char savingText[16];
extern const char formattingText[16];
extern const char loadingText[16];
extern char timeText[8];
extern char fileMoneyText[8];
extern const char cardRemoveWarningText[72];
extern const char savingToCardText[48];
extern const char newDataSaveText[64];
extern const char loadingFromCardText[48];
extern const char overwriteSaveDataText[32];
extern const char savingNewDataText[24];
extern const char powerOffWarningText[32];
extern const char saveCompleteText[24];
extern const char formattingCardText[48];
extern char saveStateBlankText[8];
extern const char saveFailedText[16];
extern const char loadFailedNotFoundText[48];
extern const char loadFailedCorruptedText[48];
extern const char saveDataCorruptedText[64];
extern const char loadFailedText[16];
extern const char maxSaveFilesHddText[64];
extern const char maxSaveFilesExceededText[104];
extern const char maxFoldersExceededText[104];
extern const char notEnoughSpaceText[56];

static char * msg00_1_0036D638[6] = { MenuFileSaveText, MenuFileLoadText, MenuFileMemoryCardSlot1Path, MenuFileMemoryCardSlot2Path, MenuFileHardDiskDrivePath, MenuFileFormatText };

static char * msg00_4_0036D658[3] = { savingText, loadingText, formattingText };

static char * msg00_5_0036D668[] = { timeText, fileMoneyText };

const char cardRemoveWarningText[72] = ".\nDo not remove Memory Card (PS2), reset, or switch off the console.";

const char powerOffWarningText[32] = ".\nDo not turn power off.";

const char checkingCardText[48] = "Checking Memory Card (PS2) in MEMORY CARD ";

const char selectSlotToSaveText[40] = "Select MEMORY CARD slot to save.";

const char selectSlotToLoadText[40] = "Select MEMORY CARD slot to load.";

const char noCardInSlotText[40] = "No Memory Card (PS2) in MEMORY CARD ";

const char hddNotConnectedText[40] = "Hard Disk Drive is not connected.";

const char newDataSaveText[64] = "New data will be saved to the Memory Card (PS2) in\nMEMORY CARD ";

const char overwriteSaveDataText[32] = "Overwrite selected save data\?";

const char formattingCardText[48] = "Formatting Memory Card (PS2) in MEMORY CARD ";

/* Text: Save complete! \x1F */
const char saveCompleteText[24] = "Save complete! \037";

const char savingToCardText[48] = "Saving to Memory Card (PS2) in MEMORY CARD ";

const char loadingFromCardText[48] = "Loading from Memory Card (PS2) in MEMORY CARD ";

const char savingNewDataText[24] = "Saving new game data.";

char * const fileConfirmationTexts[12] = { newDataSaveText, overwriteSaveDataText, formattingCardText, saveCompleteText, savingToCardText, loadingFromCardText, powerOffWarningText, savingToCardText, loadingFromCardText, cardRemoveWarningText, savingNewDataText, cardRemoveWarningText };

const char loadSlotPromptText[64] = "Load selected data from Memory Card (PS2) in \nMEMORY CARD ";

/* Text: Load complete! \x1F */
const char loadCompleteText[24] = "Load complete! \037";

const char loadSlotPromptBreakText[64] = "Load selected data from Memory Card (PS2) in\nMEMORY CARD ";

/* Text: Save failed. \x1F */
const char saveFailedText[16] = "Save failed. \037";

/* Text: Selected save data is corrupted and cannot be overwritten. \x1F */
const char saveDataCorruptedText[64] = "Selected save data is corrupted and cannot be overwritten. \037";

/* Text: Load failed. \x1F */
const char loadFailedText[16] = "Load failed. \037";

/* Text: Load failed. Selected save data is corrupted. \x1F */
const char loadFailedCorruptedText[48] = "Load failed. Selected save data is corrupted. \037";

/* Text: Load failed. Save data does not exist. \x1F */
const char loadFailedNotFoundText[48] = "Load failed. Save data does not exist. \037";

char * const fileSlotStateTexts[6] = { saveFailedText, saveDataCorruptedText, saveStateBlankText, loadFailedText, loadFailedCorruptedText, loadFailedNotFoundText };

const char cardInSlotText[40] = "Memory Card (PS2) in MEMORY CARD ";

const char formatCardText[48] = "Format Memory Card (PS2) in MEMORY CARD ";

/* Text: Formatting complete! \x1F */
const char formatCompleteText[24] = "Formatting complete! \037";

/* Text: Formatting failed. \x1F */
const char formatFailedText[24] = "Formatting failed. \037";

const char unformattedCardPrefixText[40] = "The Memory Card (PS2) in MEMORY CARD\n";

/* Text:  is unformatted. \x1F */
const char unformattedSuffixText[24] = " is unformatted. \037";

const char cancelSaveText[16] = "Cancel Save.";

const char cancelLoadText[16] = "Cancel load.";

/* Text: \ncannot be accessed. \x1F */
const char cannotBeAccessedText[24] = "\ncannot be accessed. \037";

/* Text: Insert a Memory Card (PS2)\nin MEMORY CARD slot 1 or slot 2. Press \x1E\x01 to continue. \x1F */
const char insertCardPromptText[88] = "Insert a Memory Card (PS2)\nin MEMORY CARD slot 1 or slot 2. Press \036\001 to continue. \037";

const char insufficientCardSpaceText[64] = "Insufficient space on the Memory Card (PS2) in \nMEMORY CARD ";

/* Text: Insufficient free space to\ncreate save data. \x1F */
const char insufficientFreeSpaceText[48] = "Insufficient free space to\ncreate save data. \037";

/* Text: . Xenosaga requires 165KB of free \nspace to save data. \x1F */
const char saveSpaceRequirementText[64] = ". Xenosaga requires 165KB of free \nspace to save data. \037";

/* Text: Exceeds the maximum number of save files\non Hard Disk Drive.Additional save files cannot be\ncreated. \x1F */
const char maxSaveFilesExceededText[104] = "Exceeds the maximum number of save files\non Hard Disk Drive.Additional save files cannot be\ncreated. \037";

/* Text: Exceeds the maximum number of folders that\nmay be created.\nAdditional folders may not be created. \x1F */
const char maxFoldersExceededText[104] = "Exceeds the maximum number of folders that\nmay be created.\nAdditional folders may not be created. \037";

/* Text: Not enough free space exists to\ncreate save files. \x1F */
const char notEnoughSpaceText[56] = "Not enough free space exists to\ncreate save files. \037";

const char maxSaveFilesHddText[64] = "Exceeds the maximum number of save\nfiles on Hard Disk Drive.";

char * const fileProgressTexts[4] = { maxSaveFilesExceededText, maxFoldersExceededText, notEnoughSpaceText, maxSaveFilesHddText };

const char isThisOkayText[16] = "Is this okay\?";

const char memoryCardSlot1Text[24] = "MEMORY CARD slot 1";

const char memoryCardSlot2Text[24] = "MEMORY CARD slot 2";

const char formattingText[16] = "Formatting...";

const char loadingText[16] = "Loading...";

const char savingText[16] = "Saving...";

const char shionLevelText[16] = "Shion:Lv";

const char newFileText[16] = "New file";

const char corruptDataText[16] = "Corrupt data";

const char cannotSaveText[16] = "Cannot save";

const char cannotLoadText[24] = "Cannot load";

const unsigned char cardDataNewPath[24] = "data\\carddata.new";

const unsigned char newDataThumbnailPath[24] = "data\\endou\\new_d.jpg";

const unsigned char unusedDataThumbnailPath[24] = "data\\endou\\n_use.jpg";

const unsigned char brokenDataThumbnailPath[24] = "data\\endou\\brk_d.jpg";

const char segmentFileText[16] = "Segment File";

const char agwsAmmoText[16] = "A.G.W.S. Ammo";

const char agwsAccessoriesText[24] = "A.G.W.S. Accessories";

const char agwsWeaponsText[24] = "A.G.W.S. Weapons";

const char characterAmmoText[16] = "Character Ammo";

const char characterAccessoriesText[24] = "Character Accessories";

const char characterWeaponsText[24] = "Character Weapons";

/* Text: Special Items | Categorical Order | Numerical Order | Alphabetical Order | Sort by */
const char MenuItemInfoLabelsText[96] = "Special Items\000\000\000Categorical Order\000\000\000\000\000\000\000Numerical Order\000Alphabetical Order\000\000\000\000\000\000Sort by ";

char saveStateBlankText[8] = "";

char periodText[8] = ".";

const char yesNoText[8] = "Yes\nNo";

char cancelText[8] = "Cancel";

char fileSelectText[8] = "Select";

/* Text: \x01Money */
char fileMoneyText[8] = "\001Money";

/* Text: \x01Time */
char timeText[8] = "\001Time";

char slotNumberText[8] = "No.";

const char shortNumberFormat[8] = "%3d";

const char memoryCardMarkerText[8] = "-MC-";

const char wideNumberFormat[4] = "%6d";

XglTaskScheduler * FileTask = 0;

unsigned char * FileCardDataBuf = 0;

FileSaveRecord * FileSaveData = 0;

unsigned char * FileExistCheckWork = 0;

FileObjectSystem * FileObjectDataSystem = 0;

const char itemsText[8] = "Items";

const char MenuItemCancelText[8] = "Cancel.";

extern char MenuFileCancelText[8];

static struct FileSelectSource msg01_2_0036D650[] = { { .message = isThisOkayText, .title = yesNoText } };

const struct FileDeviceTexts fileDeviceTexts = { { MenuFileCancelText, MenuFileSaveText, MenuFileLoadText } };

const struct FileOperationTexts fileOperationTexts = { { cardRemoveWarningText, powerOffWarningText, checkingCardText } };

const struct FileSelectionTexts fileSelectionTexts = { { selectSlotToSaveText, selectSlotToLoadText, noCardInSlotText, hddNotConnectedText } };

const char selectSaveLocationText[32] = "Select location for save data.";

const char selectLoadDataText[40] = "Select the data you wish to load.";

const struct FileSlotTexts fileSlotTexts = { { loadSlotPromptText, loadCompleteText, loadSlotPromptBreakText } };

const struct FileCardTexts fileCardTexts = { { cardInSlotText, formatCardText, formatCompleteText, formatFailedText, unformattedCardPrefixText, unformattedSuffixText, unformattedSuffixText } };

const struct FileRequestTexts fileRequestTexts = { { saveFailedText, loadFailedText, cancelSaveText, cancelLoadText, noCardInSlotText, cardInSlotText, cardInSlotText, cannotBeAccessedText, cannotBeAccessedText } };

const struct FileFormatTexts fileFormatTexts = { { insufficientCardSpaceText, insufficientFreeSpaceText, saveSpaceRequirementText } };

/* Text: Xenosaga save data does not exist. \x1F */
const char saveDataNotExistText[40] = "Xenosaga save data does not exist. \037";

const char saveDataNotExistCardText[72] = "Xenosaga save data does not exist\non Memory Card (PS2) in MEMORY CARD ";

char *fileListTexts[2] = { selectSaveLocationText, selectLoadDataText };

char *fileFinishTexts[2] = { saveDataNotExistText, saveDataNotExistCardText };

/* Text: . \x1F */
char periodEndText[8] = ". \037";

int FileWorkEndTop = 0;

