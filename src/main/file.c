#include "common.h"
#include "main/e_battle_win_open.h"
#include "main/open_close_main.h"
#include "main/end_print.h"
#include "main/e_number_main.h"
#include "main/menu_1.h"
#include "shared.h"
#include "file.h"
#include "main/xgl_hdd.h"
#include "main/xgl_jpeg.h"
#include "main/menu_top_command_check.h"
#include "file.private.h"
#include "main/look_at_player.h"
#include "main/map_1.h"
#include "main/menu_para_pt_rate_get.h"
#include "main/xgl_mc.h"

/* FileWork->text_index / MenuFileMain's mode: what the menu was opened for. */
#define FILE_MODE_SAVE 0
#define FILE_MODE_LOAD 1

/* The file menu's state machine (FileWork->state, set from FileWork->command). */
enum FileMenuState {
    FILE_STATE_SELECT_DEVICE = 0x24,      /* memory card 1 / 2 / HDD / Cancel */
    FILE_STATE_CHECK_DEVICE = 0x25,       /* free space and save directory */
    FILE_STATE_LOAD_SLOTS = 0x27,         /* read every slot's header */
    FILE_STATE_SLOT_LIST = 0x28,          /* browse the three-panel slot list */
    FILE_STATE_SLOT_REJECTED = 0x29,      /* the chosen slot cannot be used */
    FILE_STATE_CONFIRM_SLOT = 0x2a,       /* yes/no for the chosen slot */
    FILE_STATE_SAVE_WRITE = 0x2c,
    FILE_STATE_SAVE_CREATE = 0x2d,        /* create the save folder first */
    FILE_STATE_SAVE_DONE = 0x2e,
    FILE_STATE_LOAD_READ = 0x50,
    FILE_STATE_LOAD_DONE = 0x52,
    FILE_STATE_CARD_UNFORMATTED = 0x80,
    FILE_STATE_FORMAT_CONFIRM = 0x82,
    FILE_STATE_FORMAT = 0x84,
    FILE_STATE_FORMAT_DONE = 0x86,
    FILE_STATE_FORMAT_FAILED = 0x87,
    FILE_STATE_ERROR_REQUEST_FAILED = 0x90, /* "Save failed." / "Load failed." */
    FILE_STATE_ERROR_NO_CARD = 0x91,
    FILE_STATE_ERROR_CANNOT_ACCESS = 0x92,
    FILE_STATE_ERROR_HDD_TOO_MANY_SAVES = 0x94,
    FILE_STATE_ERROR_HDD_TOO_MANY_FOLDERS = 0x95,
    FILE_STATE_ERROR_HDD_NO_SPACE = 0x96,
    FILE_STATE_CANCEL_CONFIRM = 0xa0,     /* "Cancel Save." / "Cancel load." */
    FILE_STATE_INSERT_CARD = 0xa1,
    FILE_STATE_NO_CARD_SPACE = 0xb0,      /* not enough free blocks */
    FILE_STATE_NO_SAVE_DATA = 0xb8,
    FILE_STATE_CLOSING = 0xf0,
    FILE_STATE_DONE = 0xff                /* ends the file menu */
};

/* FileObjectDataSystem->state[slot]: what a save slot holds. */
enum FileSlotState {
    FILE_SLOT_EXISTING = 1,  /* a save exists */
    FILE_SLOT_NEW = 2,       /* empty, room for a new save */
    FILE_SLOT_FULL = 3,      /* empty, no room left */
    FILE_SLOT_CORRUPT = 4
};

/* Save slots and the placeholder entries after them in FileObjectData. */
#define FILE_SLOT_COUNT MC_SAVE_SLOT_COUNT
#define FILE_ENTRY_NEW_DATA 99     /* thumbnail shown for FILE_SLOT_NEW */
#define FILE_ENTRY_UNUSABLE 100    /* ... for FILE_SLOT_FULL */
#define FILE_ENTRY_CORRUPT 101     /* ... for FILE_SLOT_CORRUPT */
#define FILE_ENTRY_SNAPSHOT 102    /* the current game's snapshot */

/* The map whose saves are counted in unit_map_count; the load list refuses
 * such saves. Map names are two 33-byte lines. */
#define FILE_UNIT_MAP_NO 10
#define FILE_MAP_NAME_LINE_SIZE 33

/* MenuScenarioNoGet() above these values saves scenario rank 3 / 2. */
#define FILE_SCENARIO_RANK3_ABOVE 300
#define FILE_SCENARIO_RANK2_ABOVE 100

/* FileSaveRecord.version of the current save format. */
#define FILE_SAVE_VERSION 3

/* Save size per slot in kilobytes of free space. */
#define FILE_CARD_SAVE_KB 165
#define FILE_HDD_SAVE_KB 225

/* Thumbnails are 4 KB JPEGs; five decoder buffers hold the slots around
 * the cursor. */
#define FILE_THUMBNAIL_SIZE 0x1000
#define FILE_THUMBNAIL_QUALITY 30

/* MenuFileMain stages its buffers from this fixed main-memory address on: the
 * card data image, the system block, the JPEG decoder slots and the object
 * table, each region starting 0x800-aligned. */
#define FILE_STAGING_BASE 0x01000000
#define FILE_STAGING_ALIGN 0x800
#define FILE_CARD_DATA_BUF_SIZE 0x12000
#define FILE_SYSTEM_BLOCK_SIZE 0x258
#define FILE_JPEG_AREA_SIZE 0x46500
#define FILE_OBJECT_AREA_SIZE 0x696a0
#define FILE_EXIST_CHECK_SIZE 0x2000
#define FILE_JPEG_BUFFERS 5
#define FILE_JPEG_SLOT_SIZE 0xe100

/* Party members stored per save, and the face icons a panel shows. */
#define FILE_PARTY_IDS_MAX 16
#define FILE_PARTY_FACES_MAX 6

/* Slot panels: three visible plus one scrolling in. */
#define FILE_PANELS_VISIBLE 3
#define FILE_PANELS_MAX 4
#define FILE_PANEL_TOP 64
#define FILE_PANEL_PITCH 112
#define FILE_PANEL_X 16
#define FILE_PANEL_OFFSCREEN_X (-512)
#define FILE_PANEL_ENTER_X 528
#define FILE_SCROLL_SPEED 14
#define FILE_SCROLL_SPEED_FAST 28

/* tskFileData's own panel animation states (FileDataContext.state). */
enum FilePanelState {
    FILE_PANEL_HIDDEN = 0,
    FILE_PANEL_WAIT = 1,
    FILE_PANEL_OPEN = 10,
    FILE_PANEL_SLIDE_IN = 12,
    FILE_PANEL_SLIDE_OUT = 14,    /* all but the selected panel leave */
    FILE_PANEL_EXPAND = 16,       /* selected panel moves to the middle */
    FILE_PANEL_CLOSE = 18,
    FILE_PANEL_BROWSE = 50,
    FILE_PANEL_EXPANDED = 52,
    FILE_PANEL_SCROLL = 60
};

/* xglSoundEffectNormalID ids. */
#define FILE_SE_DECIDE 1
#define FILE_SE_CANCEL 2
#define FILE_SE_CURSOR 3
#define FILE_SE_BUZZER 5
#define FILE_SE_SAVE_DONE 0x19
#define FILE_SE_LOAD_DONE 0x1a

/* Slot list: the fast scroll repeats this many steps, and a pad press is
 * accepted again after this many frames (FileWork->menu_substate). */
#define FILE_FAST_SCROLL_STEPS 10

/* Frames FileWork->menu_substate counts down before a state proceeds. */
#define FILE_WAIT_FRAMES 16
#define FILE_INPUT_REPEAT_DELAY 4

/* Yes/No prompts (FileWork->action_selection). */
#define FILE_CHOICE_YES 0
#define FILE_CHOICE_NO 1
#define FILE_CONFIRM_ROWS 2

/* Device list: entries 0 and 1 are the memory card slots, entry 2 is the
 * Cancel row. FileSlotNameGet() names devices instead: 0 and 1 are the card
 * slots, 2 is the hard disk drive. */
#define FILE_DEVICE_CANCEL 2
#define FILE_DEVICE_HDD 2
#define FILE_DEVICE_ROWS 3

/* Frames MenuFile skips before running the state again
 * (FileWork->dispatch_delay): after a state change, and before an HDD
 * request or an error state. */
#define FILE_DELAY_SHORT 12
#define FILE_DELAY_LONG 16

/* FileWork->input_flags bits. */
#define FILE_INPUT_ENTER 1  /* the state was just entered */
#define FILE_INPUT_SKIP 2   /* no pad input on the first frame */

/* GameLoopState.runtime_flags: set while the file menu is up. */
#define FILE_RUNTIME_FLAG_MENU_OPEN 0x04000000

/* FileObjectSystem.load_state: FileObjectAllLoad's progress. */
enum FileLoadState {
    FILE_LOAD_START = 0,
    FILE_LOAD_NEXT_SLOT = 2,
    FILE_LOAD_REQUEST = 4,
    FILE_LOAD_RESULT = 6,
    FILE_LOAD_SLOT_DONE = 8,
    FILE_LOAD_FAILED = 9
};

/* The top banner (MenuFilePas) slides in the names of the current action
 * (Save / Load), the two memory card slots, the HDD and Format. The slot
 * names are FILE_PAS_CARD + card number. */
#define FILE_PAS_MESSAGES 6
#define FILE_PAS_CARD 2
#define FILE_PAS_HDD 4
#define FILE_PAS_FORMAT 5
#define FILE_PAS_X_SHOWN 16
#define FILE_PAS_X_HIDDEN 288

/* Windows and messages slide in from the right edge of the 512-wide screen
 * (x = 544); the device/confirmation lists wait off the left edge. */
#define FILE_SLIDE_IN_X 544
#define FILE_SELECT_HIDDEN_X (-258)

/* Load progress gauge. */
#define FILE_GAUGE_Y 296
#define FILE_GAUGE_WIDTH 256
#define FILE_GAUGE_HEIGHT 8
#define FILE_GAUGE_ALPHA 128
#define FILE_GAUGE_FILLED_RED 161
#define FILE_GAUGE_FILLED_GREEN 232
#define FILE_GAUGE_FILLED_BLUE 205
#define FILE_GAUGE_UNFILLED 64
/* Steps of the card's save-folder creation (create_sub2's subphases). */
#define FILE_CREATE_STEPS 7

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


/* Five decoder buffers are FILE_JPEG_SLOT_SIZE bytes apart; their signed object number is +1. */
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
    unsigned char image[FILE_JPEG_SLOT_SIZE - 0xa0];
} FileJpegDecodeSlotView;

typedef struct FileObjectEntryView {
    struct FileJpegDecodeSlotView *jpeg;
    unsigned char thumbnail[FILE_THUMBNAIL_SIZE];
    unsigned char state;
    signed char number;
    unsigned char unmodeled_1006;
    signed char chapter;
    signed char leader;
    unsigned char unmodeled_1009[3];
    int map_number;
    int party_ids[FILE_PARTY_IDS_MAX];
    u64 play_time;
    /* The packed time word is the first four bytes of the clock. */
    union {
        XglClock clock;
        unsigned int packed;
    } elapsed;
} FileObjectEntryView;



struct FilePassClip {
    short x;
    short y;
    int color;
    short width;
    short height;
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
    /* The window and the widgets drawn in it form one record at +0x0c: the
       variable-index widget addresses split at its 16-byte offset units. */
    struct FileThumbnailBody {
        WindowDX window;
        ETagFontMainData labels[2];
        unsigned char unmodeled_1d4[0x1f4 - 0x1d4];
        EMessageParam messages[2];
        ENumberMainData numbers[5];
        unsigned char unmodeled_54c[0x62c - 0x54c];
        struct FileFaceSprite faces[FILE_PARTY_FACES_MAX];
        unsigned char unmodeled_71c[0x76c - 0x71c];
    } body;
    /* The slot tab above the panel, one record at +0x778. */
    struct FileThumbnailTab {
        ETagFontMainData label;
        ENumberMainData number;
        ERibbonMainData ribbon;
    } tab;
    unsigned char unmodeled_898[0x908 - 0x898];
    struct FileThumbnailPacket thumbnail;
};

struct FileDataContext {
    unsigned char state;
    unsigned char visible;
    unsigned char extra_panel;
    unsigned char initial_flags;
    int initial_value;
    int work;
    struct FileThumbnailContext panels[FILE_PANELS_MAX];
    ECursolMainData cursor;
    struct FilePassClip clip;
};

typedef struct FilePasWork {
    unsigned char unmodeled_00[0x04];
    short x;
    short y;
    int color;
    WindowDX window;
    EMessageParam messages[FILE_PAS_MESSAGES];
    unsigned char unmodeled_338[0x3c0 - 0x338];
    struct FilePassClip rect;
} FilePasWork;

struct FileInfoWindowData {
    short x;
    short y;
    unsigned char unmodeled_04[4];
    int mode;
    const char *text;
};

struct FileInfoContext {
    unsigned char unmodeled_00[4];
    int work;
    WindowDX window;
    unsigned char unmodeled_19c[0x1b0 - 0x19c];
    struct FileInfoWindowData contents;
    unsigned char unmodeled_1c0[0x360 - 0x1c0];
    char text[256];
};

struct FileDeviceTexts {
    const char *text[3];
};

struct FileOperationTexts {
    const char *text[3];
};

struct FileSelectionTexts {
    const char *text[4];
};

struct FileListTexts {
    const char *text[2];
};

struct FileConfirmationTexts {
    const char *text[12];
};

struct FileSlotTexts {
    const char *text[3];
};

struct FileSlotStateTexts {
    const char *text[2][3];
};

struct FileCardTexts {
    const char *text[7];
};

struct FileRequestTexts {
    const char *text[9];
};

struct FileFormatTexts {
    const char *text[3];
};

struct FileFinishTexts {
    const char *text[2];
};

struct FileProgressTexts {
    const char *text[4];
};

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

struct FileSelectItem {
    const char *text;
    unsigned char unmodeled_04[4];
    signed char color_state;
    unsigned char unmodeled_09[3];
};

struct FileSelectList {
    unsigned char unmodeled_00[2];
    unsigned short layout_mode;
    int selected;
    const char *title;
    const char *confirmation_data;
    struct FileSelectItem *items;
    unsigned char unmodeled_14[0x5f8 - 0x14];
};

struct FileSelectContext {
    unsigned char unmodeled_00[2];
    unsigned char initial_flags;
    unsigned char initial_state;
    int work;
    unsigned char animation_flags[4];
    short targets[4];
    WindowDX windows[4];
    struct FileSelectList lists[4];
    struct FileSelectItem entries[6];
};

struct FileSelectSource {
    const char *message;
    const char *title;
};

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

/* The card number as MenuFilePas and MenuFile read it for the panel target
   and the create request: through a one-member union (alias set 0), so the
   load is not ordered before the short stores that precede it. The field
   itself stays an int: MenuFile's device-selection store must be an ordinary
   int store, which kills the gcse load of FileWork (an alias-set-0 int union
   store does not, see reports/cc1-fsv10-swu.md). */
typedef union FileCardNumber {
    int number;
} FileCardNumber;

extern char MenuFileSlot1Text[];
extern char MenuFileSlot2Text[];
extern char MenuFileHardDiskDriveText[];
extern const struct FileDeviceTexts fileDeviceTexts;
extern const struct FileOperationTexts fileOperationTexts;
extern const struct FileSelectionTexts fileSelectionTexts;
extern struct FileListTexts fileListTexts[];
extern const struct FileConfirmationTexts fileConfirmationTexts;
extern const struct FileSlotTexts fileSlotTexts;
extern const struct FileSlotStateTexts fileSlotStateTexts;
extern const struct FileCardTexts fileCardTexts;
extern const struct FileRequestTexts fileRequestTexts;
extern const struct FileFormatTexts fileFormatTexts;
extern struct FileFinishTexts fileFinishTexts[];
extern const struct FileProgressTexts fileProgressTexts;
extern char periodText[];
extern char periodEndText[];
extern const char insertCardPromptText[];
extern XglTaskScheduler *FileTask;
extern unsigned char *FileCardDataBuf;
extern unsigned char *FileExistCheckWork;
extern const unsigned char cardDataNewPath[];
extern const unsigned char newDataThumbnailPath[];
extern const unsigned char unusedDataThumbnailPath[];
extern const unsigned char brokenDataThumbnailPath[];
extern const char memoryCardSlot1Text[];
extern const char memoryCardSlot2Text[];
extern char cancelText[];
extern char fileSelectText[];
static struct FileSelectSource msg01_2_0036D650[] ;
extern const char shionLevelText[];
extern const char newFileText[];
extern const char corruptDataText[];
extern const char cannotSaveText[];
extern const char cannotLoadText[];
extern char slotNumberText[];
static char * msg00_5_0036D668[] ;
extern FileMenuPadData PadData;
extern const char shortNumberFormat[];
extern const char memoryCardMarkerText[];
extern const char wideNumberFormat[];
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
extern char MenuFileCancelText[8];

void WindowSPSet(WindowSPCursor *cursor);
void WindowSPMain(WindowSPCursor *cursor);
void WindowSPSelectJump(WindowSPCursor *cursor, int selection);
void FileJpegDecode(int number);
void tyaDrawGauge(struct FileLoadGauge *progress);
void MenuCfTaikiPush(void);
void MenuCfTaikiPop(void);
unsigned char *MenuBgTaskInit(void *work, int mode);
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
void exit(int status);
void *MenuWorkEndGet(void);
void MenuFile(void);
void FileThumbnailDisp(struct FileThumbnailContext *panel);
extern int xglHddMcLoad(struct HddLoadRequest *request);
extern int MenuSelectMove(int selection, int count, int wrap);
extern void xglFontDebugHex(int x, int y, int value, int digits);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

static char *name_0_0036D628[3] = {
    MenuFileSlot1Text,
    MenuFileSlot2Text,
    MenuFileHardDiskDriveText,
};

FileWorkBlock *FileWork = 0;

FileJpegDecodeSlotView *FileJpegDec = 0;

FileObjectEntryView *FileObjectData = 0;

static char * msg00_1_0036D638[6];

static char * msg00_1_0036D638[6] = { MenuFileSaveText, MenuFileLoadText, MenuFileMemoryCardSlot1Path, MenuFileMemoryCardSlot2Path, MenuFileHardDiskDrivePath, MenuFileFormatText };

static const char * msg00_4_0036D658[3] ={ savingText, loadingText, formattingText };

static char * msg00_5_0036D668[] = { timeText, fileMoneyText };

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

static struct FileLoadGauge gauge;

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
        return number + FILE_SLOT_COUNT;
    if (number < FILE_SLOT_COUNT)
        return number;
    return number - FILE_SLOT_COUNT;
}

void FileObjectDataClear(void)
{
    int i;

    for (i = 0; i <= FILE_ENTRY_UNUSABLE; ++i) {
        FileObjectData[i].state = 0;
        FileObjectData[i].number = -1;
    }
    for (i = 0; i <= FILE_JPEG_BUFFERS - 1; ++i)
        FileJpegDec[i].number = -1;
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
    FileObjectEntryView *entry = FileObjectData + number;
    int i;
    int *party_ids;
    PartyDataPrefix *party;
    short map_number;
    u64 play_time;
    unsigned char *thumbnail;

    memset(entry, 0, sizeof(*entry));
    switch (state) {
    case FILE_SLOT_NEW:
        *entry = FileObjectData[FILE_ENTRY_NEW_DATA];
        break;
    case FILE_SLOT_FULL:
        *entry = FileObjectData[FILE_ENTRY_UNUSABLE];
        break;
    case FILE_SLOT_CORRUPT:
        *entry = FileObjectData[FILE_ENTRY_CORRUPT];
        break;
    case FILE_SLOT_EXISTING:
        if (FileSaveData->version != FILE_SAVE_VERSION) {
            *entry = FileObjectData[FILE_ENTRY_CORRUPT];
            FileObjectDataSystem->state[number] = FILE_SLOT_CORRUPT;
        } else if (!FileCheckSumCheck((FileChecksumData *)FileSaveData)) {
            *entry = FileObjectData[FILE_ENTRY_CORRUPT];
            FileObjectDataSystem->state[number] = FILE_SLOT_CORRUPT;
        } else {
            party_ids = entry->party_ids;
            entry->chapter = FileSaveData->chapter;
            entry->leader = FileSaveData->leader;
            map_number = FileSaveData->map_number;
            play_time = FileSaveData->play_time;
            entry->elapsed.clock = FileSaveData->elapsed;
            entry->map_number = map_number;
            entry->play_time = play_time;
            PartyTimeDispChange(&entry->elapsed.clock);
            party = &FileSaveData->party;
            memset(party_ids, 0, sizeof(entry->party_ids));
            PartyAllPartyGet2(party_ids, party);
            thumbnail = FileSaveData->thumbnail;
            for (i = 0; i < FILE_THUMBNAIL_SIZE; ++i)
                entry->thumbnail[i] = thumbnail[i];
        }
        break;
    }
    entry->number = number;
}

void FileJpegCheck(void)
{
    signed char valid_numbers[FILE_JPEG_BUFFERS];
    FileJpegDecodeSlotView *slot;
    int i;
    int j;

    i = -1;
    do {
        valid_numbers[i + 1] = FileObjectDataNoGet(FileWork->current_slot, i);
        i = i + 1;
    } while (i < FILE_JPEG_BUFFERS - 1);

    for (i = 0; i < FILE_JPEG_BUFFERS; ++i) {
        slot = FileJpegDec + i;
        for (j = 0; j < FILE_JPEG_BUFFERS; ++j) {
            if (slot->number == valid_numbers[j])
                break;
        }
        if (j == FILE_JPEG_BUFFERS)
            slot->number = -1;
    }
}

void FileJpegDecode(int number)
{
    FileObjectEntryView *entry = FileObjectData + number;
    FileJpegDecodeSlotView *jpeg = FileJpegDec;
    FileJpegDecodeSlotView *queued;
    FileJpegDecodeSlotView *slot;
    int i;

    queued = jpeg;
    for (i = 0; i < FILE_JPEG_BUFFERS; ++i) {
        if (queued->number == number)
            return;
        ++queued;
    }

    slot = jpeg;
    for (i = 0; i < FILE_JPEG_BUFFERS; ++i) {
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
        slot = FileObjectDataNoGet(FileWork->current_slot, offset);
        offset += 1;
        FileJpegDecode(slot);
    } while (offset < FILE_JPEG_BUFFERS - 1);
}

void FileObjectJpegDecChange(int number)
{
    int i;

    for (i = 0; i < FILE_JPEG_BUFFERS; ++i) {
        /* The original adds the slot offset to the buffer address as integers
         * (addu offset, base); pointer arithmetic swaps the addu operands. */
        FileJpegDecodeSlotView *slot = (FileJpegDecodeSlotView *)(i * sizeof(FileJpegDecodeSlotView) + (u32)FileJpegDec);

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
    case FILE_LOAD_START: {
        FileWorkBlock *initial_work = FileWork;

        gauge.x = 123;
        gauge.y = FILE_GAUGE_Y;
        gauge.vertex_attribute = 0xffffffffU;
        loaded_count = 0;
        gauge.width = FILE_GAUGE_WIDTH;
        index = 0;
        gauge.height = FILE_GAUGE_HEIGHT;
        gauge.current = 0;
        gauge.maximum = 0;
        gauge.filled_color.red = FILE_GAUGE_FILLED_RED;
        gauge.filled_color.green = FILE_GAUGE_FILLED_GREEN;
        gauge.filled_color.blue = FILE_GAUGE_FILLED_BLUE;
        gauge.filled_color.alpha = FILE_GAUGE_ALPHA;
        gauge.unfilled_color.red = FILE_GAUGE_UNFILLED;
        gauge.unfilled_color.green = FILE_GAUGE_UNFILLED;
        gauge.unfilled_color.blue = FILE_GAUGE_UNFILLED;
        gauge.unfilled_color.alpha = FILE_GAUGE_ALPHA;
        __builtin_memset(&initial_work->hdd_result, 0,
                         sizeof(initial_work->hdd_result));
        for (; index < FILE_SLOT_COUNT; index++) {
            state = FileObjectDataSystem->state[index];

            if (state != FILE_SLOT_EXISTING) {
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
        *load_state = FILE_LOAD_NEXT_SLOT;
        *load_index = 0;
        if (loaded_count == FILE_SLOT_COUNT) {
            FileWorkBlock *work = FileWork;

            work->hdd_result = 0;
            if (!work->use_hdd) {
                unsigned short *card_status = work->card.status;

                if (card_status[work->card_number] != MC_STATUS_READY) {
                    work->hdd_result = -1;
                }
            }
            *load_state = FILE_LOAD_START;
            return 0;
        }
        if (FileWork->use_hdd) {
            FileWork->hdd_result = xglHddMcLoadMount();
        }
    }
    case FILE_LOAD_NEXT_SLOT:
        gauge.current++;
        while (*load_index < FILE_SLOT_COUNT) {
            if (FileObjectDataSystem->state[*load_index] == FILE_SLOT_EXISTING) {
                *load_state = FILE_LOAD_REQUEST;
                break;
            }
            (*load_index)++;
        }
        if (*load_index == FILE_SLOT_COUNT) {
            *load_state = FILE_LOAD_START;
            if (FileWork->use_hdd) {
                FileWork->hdd_result = xglHddMcUmount();
            }
            return 0;
        }
    case FILE_LOAD_REQUEST:
        if (FileWork->use_hdd) {
            hdd_request.slot = *load_index;
            hdd_request.data = FileSaveData;
            hdd_request.size = sizeof(*FileSaveData);
            memset(FileSaveData, 0, sizeof(*FileSaveData));
            FileWork->hdd_result = xglHddMcLoadCore(&hdd_request);
            *load_state = FILE_LOAD_RESULT;
        } else {
            card_request.slot = *load_index;
            card_request.buffer = (int)FileSaveData;
            card_request.size = sizeof(*FileSaveData);
            memset(FileSaveData, 0, sizeof(*FileSaveData));
            xglMcRequest(FileWork->card_number, MC_REQUEST_LOAD, &card_request,
                         &FileWork->request_result);
            *load_state = FILE_LOAD_RESULT;
            tyaDrawGauge(&gauge);
            return 1;
        }
    case FILE_LOAD_RESULT:
        if (FileWork->use_hdd) {
            if (FileWork->hdd_result != 0) {
                FileObjectDataSystem->state[*load_index] = FILE_SLOT_CORRUPT;
                FileObjectDataInput(*load_index, FILE_SLOT_CORRUPT);
                *load_state = FILE_LOAD_FAILED;
            } else {
                FileObjectDataInput(*load_index, FILE_SLOT_EXISTING);
                *load_state = FILE_LOAD_SLOT_DONE;
            }
        } else {
            FileWorkBlock *work = FileWork;
            unsigned short *card_status = work->card.status;
            int status = card_status[work->card_number];

            if (status != MC_STATUS_READY) {
                if (status != MC_STATUS_BUSY) {
                    FileObjectDataSystem->state[*load_index] = FILE_SLOT_CORRUPT;
                    FileObjectDataInput(*load_index, FILE_SLOT_CORRUPT);
                    FileWork->hdd_result = -1;
                    *load_state = FILE_LOAD_FAILED;
                }
            } else {
                FileObjectDataInput(*load_index, FILE_SLOT_EXISTING);
                *load_state = FILE_LOAD_SLOT_DONE;
            }
        }
        if (*load_state == FILE_LOAD_SLOT_DONE) {
            *load_state = FILE_LOAD_NEXT_SLOT;
            (*load_index)++;
        }
        if (*load_state == FILE_LOAD_FAILED) {
            *load_state = FILE_LOAD_START;
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
    for (i = 0; i < FILE_THUMBNAIL_SIZE; ++i)
        ((FileSaveOutput *)SaveData)->thumbnail[i] = FileObjectData[FILE_ENTRY_SNAPSHOT].thumbnail[i];
    map_no = GameLoopState.map_no;
    ((FileSaveOutput *)SaveData)->map_no = map_no;
    name = MenuSaveMapNameGet(map_no);
    xglMcSetMapName(name, name + FILE_MAP_NAME_LINE_SIZE);
    if (map_no == FILE_UNIT_MAP_NO)
        ((FileSaveOutput *)SaveData)->unit_map_count++;
    scenario = MenuScenarioNoGet();
    if (scenario > FILE_SCENARIO_RANK3_ABOVE)
        rank = 3;
    else if (scenario > FILE_SCENARIO_RANK2_ABOVE)
        rank = 2;
    else
        rank = 1;
    ((FileSaveOutput *)SaveData)->version = FILE_SAVE_VERSION;
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

    if (FileWork->state == FILE_STATE_DONE) {
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
    int lengths[FILE_PAS_MESSAGES];
    unsigned short targets[FILE_PAS_MESSAGES];
    short slide_target;
    int card_target;

    for (i = 0; i < FILE_PAS_MESSAGES; ++i)
        lengths[i] = MenuPasLengthGet(msg00_1_0036D638[i]);

    switch (task->state) {
    case 0:
        work->x = -272;
        work->y = 8;
        work->color = 0xfffff0;
        WindowDXSet(&work->window);
        work->window.x = work->x;
        work->window.style = work->color;
        work->window.width = 352;
        work->window.y = work->y;
        work->window.height = 32;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        for (i = 0; i < FILE_PAS_MESSAGES; ++i) {
            eMessageSet(&work->messages[i], msg00_1_0036D638[i]);
            work->messages[i].mode = 32;
            work->messages[i].x = FILE_PAS_X_HIDDEN;
            work->messages[i].draw_parameter = work->color + 2;
            work->messages[i].y = work->y + 3;
        }
        break;
    case 2:
        slide_target = -16;
        for (i = 0; i < FILE_PAS_MESSAGES; ++i)
            targets[i] = FILE_PAS_X_HIDDEN;
        switch (FileWork->state) {
        case FILE_STATE_SELECT_DEVICE:
        case FILE_STATE_CARD_UNFORMATTED:
        case FILE_STATE_FORMAT_CONFIRM:
            targets[FileWork->text_index] = FILE_PAS_X_SHOWN;
            break;
        case FILE_STATE_CHECK_DEVICE:
        case 38:
        case FILE_STATE_LOAD_SLOTS:
        case FILE_STATE_SLOT_LIST:
        case FILE_STATE_SLOT_REJECTED:
        case FILE_STATE_CONFIRM_SLOT:
        case FILE_STATE_SAVE_WRITE:
        case FILE_STATE_SAVE_CREATE:
        case FILE_STATE_SAVE_DONE:
        case FILE_STATE_LOAD_READ:
        case FILE_STATE_LOAD_DONE:
        case 136:
        case FILE_STATE_ERROR_REQUEST_FAILED:
        case FILE_STATE_ERROR_NO_CARD:
        case FILE_STATE_ERROR_CANNOT_ACCESS:
        case 147:
        case FILE_STATE_ERROR_HDD_TOO_MANY_SAVES:
        case FILE_STATE_ERROR_HDD_TOO_MANY_FOLDERS:
        case FILE_STATE_ERROR_HDD_NO_SPACE:
        case 152:
        case FILE_STATE_CANCEL_CONFIRM:
        case FILE_STATE_INSERT_CARD:
        case FILE_STATE_NO_CARD_SPACE:
        case FILE_STATE_NO_SAVE_DATA:
            targets[FileWork->text_index] = FILE_PAS_X_SHOWN;
            if (FileWork->use_hdd)
                targets[FILE_PAS_HDD] = targets[FileWork->text_index] + (unsigned short)lengths[FileWork->text_index];
            else
                targets[FILE_PAS_CARD + FileWork->card_number] = targets[FileWork->text_index] + (unsigned short)lengths[FileWork->text_index];
            break;
        case FILE_STATE_FORMAT:
        case FILE_STATE_FORMAT_DONE:
        case FILE_STATE_FORMAT_FAILED:
            targets[FileWork->text_index] = -(unsigned short)lengths[FileWork->text_index];
            targets[FILE_PAS_FORMAT] = FILE_PAS_X_SHOWN;
            card_target = (unsigned short)lengths[FILE_PAS_FORMAT] + FILE_PAS_X_SHOWN;
            targets[FILE_PAS_CARD + ((FileCardNumber *)&FileWork->card_number)->number] = card_target;
            break;
        default:
            slide_target = -352;
            break;
        }
        MoveSlide(&work->window.x, &slide_target, 3.0f);
        WindowDXMain(&work->window);
        work->rect.x = work->window.x + 3;
        work->rect.y = work->window.y + 3;
        work->rect.color = work->color;
        work->rect.width = work->window.width - 6;
        work->rect.height = work->window.height - 6;
        endPrintExtFunc(work->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET, &work->rect);
        for (i = 0; i < FILE_PAS_MESSAGES; ++i) {
            MoveSlide(&work->messages[i].x, (short *)&targets[i], 3.0f);
            if (work->messages[i].x < 256)
                eMessageMain(&work->messages[i]);
        }
        endPrintExtFunc(work->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
        break;
    }
}

static void MenuFileInfo(TskObject *task, void *data)
{
    struct FileDeviceTexts device_text;
    struct FileOperationTexts operation_text;
    struct FileSelectionTexts selection_text;
    struct FileListTexts list_text;
    struct FileConfirmationTexts confirmation_text;
    struct FileSlotTexts slot_text;
    struct FileSlotStateTexts slot_state_text;
    struct FileCardTexts card_text;
    struct FileRequestTexts request_text;
    const char *status_text;
    struct FileFormatTexts format_text;
    struct FileFinishTexts finish_text;
    struct FileProgressTexts progress_text;
    short target_y;
    short target_height;
    struct FileInfoContext *view;

    device_text = fileDeviceTexts;
    operation_text = fileOperationTexts;
    selection_text = fileSelectionTexts;
    view = data;
    list_text = fileListTexts[0];
    confirmation_text = fileConfirmationTexts;
    slot_text = fileSlotTexts;
    slot_state_text = fileSlotStateTexts;
    card_text = fileCardTexts;
    request_text = fileRequestTexts;
    status_text = insertCardPromptText;
    format_text = fileFormatTexts;
    finish_text = fileFinishTexts[0];
    progress_text = fileProgressTexts;

    switch (task->state) {
    case 0: {
        struct FileInfoWindowData *contents;

        view->work = 0xfffff0;
        WindowDXSet(&view->window);
        view->window.x = -16;
        view->window.y = 480;
        view->window.width = 544;
        view->window.height = 54;
        view->window.state = 0;
        contents = &view->contents;
        view->window.close_callback = MenuInfoWindow;
        view->window.style = view->work;
        view->window.close_callback_arg = contents;
        contents->x = 0;
        view->window.state = 1;
        view->contents.y = 0;
        view->contents.text = 0;
        WindowDXMain(&view->window);
        view->window.state = 3;
        view->contents.mode = 1;
        memset(view->text, 0, sizeof(view->text));
        break;
    }
    case 2:
        target_y = 378;
        target_height = 54;
        switch (FileWork->state) {
        case FILE_STATE_SELECT_DEVICE:
            eMessageCpy(view->text, selection_text.text[FileWork->text_index]);
            switch (FileWork->device_selection) {
            case 0:
            case 1: {
                unsigned short *card_statuses = FileWork->card.status;

                switch (card_statuses[FileWork->device_selection]) {
                case MC_STATUS_NONE:
                case MC_STATUS_PS1_CARD:
                case MC_STATUS_POCKETSTATION:
                    eMessageCpy(view->text, selection_text.text[2]);
                    eMessageCat(FileSlotNameGet(FileWork->device_selection));
                    eMessageCat(periodText);
                    break;
                }
                break;
            }
            case FILE_DEVICE_CANCEL:
                eMessageCpy(view->text, device_text.text[0]);
                break;
            }
            break;
        case FILE_STATE_CHECK_DEVICE:
        case 38:
        case FILE_STATE_LOAD_SLOTS: {
            unsigned char update_text = FileWork->input_flags & FILE_INPUT_ENTER;

            if (update_text) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, operation_text.text[2]);
                    eMessageCat(FileSlotNameGet(FILE_DEVICE_HDD));
                    eMessageCat(operation_text.text[1]);
                } else {
                    eMessageCpy(view->text, operation_text.text[2]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(operation_text.text[0]);
                }
            }
            target_y = 352;
            target_height = 78;
            break;
        }
        case FILE_STATE_SLOT_LIST:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, list_text.text[FileWork->text_index]);
            target_y = 404;
            target_height = 30;
            break;
        case FILE_STATE_SLOT_REJECTED:
            switch (FileObjectDataSystem->state[FileWork->selected_slot]) {
            case FILE_SLOT_NEW:
                eMessageCpy(view->text, slot_state_text.text[FileWork->text_index][2]);
                break;
            case FILE_SLOT_FULL:
                eMessageCpy(view->text, slot_state_text.text[FileWork->text_index][0]);
                break;
            case FILE_SLOT_CORRUPT:
                eMessageCpy(view->text, slot_state_text.text[FileWork->text_index][1]);
                break;
            }
            break;
        case FILE_STATE_CONFIRM_SLOT:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (FileWork->text_index == FILE_MODE_SAVE) {
                    if (FileWork->use_hdd) {
                        eMessageCpy(view->text, FileSlotNameGet(FILE_DEVICE_HDD));
                        if (FileObjectDataSystem->state[FileWork->selected_slot] == FILE_SLOT_NEW)
                            eMessageCat(confirmation_text.text[10]);
                        else
                            eMessageCpy(view->text, confirmation_text.text[1]);
                    } else {
                        int card_slot = FileWork->selected_slot;

                        if (FileObjectDataSystem->state[card_slot] == FILE_SLOT_NEW) {
                            eMessageCpy(view->text, confirmation_text.text[0]);
                            eMessageCat(FileSlotNameGet(FileWork->card_number));
                            eMessageCat(periodText);
                        } else {
                            eMessageCpy(view->text, confirmation_text.text[1]);
                        }
                    }
                } else {
                    if (FileWork->use_hdd) {
                        eMessageCpy(view->text, slot_text.text[2]);
                        eMessageCat(FileSlotNameGet(FILE_DEVICE_HDD));
                        eMessageCat(periodText);
                    } else {
                        eMessageCpy(view->text, slot_text.text[0]);
                        eMessageCat(FileSlotNameGet(FileWork->card_number));
                        eMessageCat(periodText);
                    }
                }
            }
            break;
        case FILE_STATE_SAVE_WRITE:
        case FILE_STATE_SAVE_CREATE:
        case FILE_STATE_LOAD_READ:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (FileWork->use_hdd) {
                    if (FileWork->text_index == FILE_MODE_SAVE)
                        eMessageCpy(view->text, confirmation_text.text[4]);
                    else
                        eMessageCpy(view->text, confirmation_text.text[5]);
                    eMessageCat(FileSlotNameGet(FILE_DEVICE_HDD));
                    eMessageCat(confirmation_text.text[6]);
                } else {
                    if (FileWork->text_index == FILE_MODE_SAVE)
                        eMessageCpy(view->text, confirmation_text.text[7]);
                    else
                        eMessageCpy(view->text, confirmation_text.text[8]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(confirmation_text.text[9]);
                }
            }
            target_y = 336;
            target_height = 78;
            break;
        case FILE_STATE_FORMAT:
            if ((FileWork->input_flags & FILE_INPUT_ENTER) && !FileWork->use_hdd) {
                eMessageCpy(view->text, confirmation_text.text[2]);
                eMessageCat(FileSlotNameGet(FileWork->card_number));
                eMessageCat(confirmation_text.text[11]);
            }
            target_y = 336;
            target_height = 78;
            break;
        case FILE_STATE_SAVE_DONE:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, confirmation_text.text[3]);
            break;
        case FILE_STATE_LOAD_DONE:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, slot_text.text[1]);
            break;
        case FILE_STATE_CARD_UNFORMATTED:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, card_text.text[4]);
                    eMessageCat(FileSlotNameGet(FILE_DEVICE_HDD));
                    eMessageCat(card_text.text[6]);
                } else {
                    eMessageCpy(view->text, card_text.text[0]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(card_text.text[5]);
                }
            }
            break;
        case FILE_STATE_FORMAT_CONFIRM:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                eMessageCpy(view->text, card_text.text[1]);
                eMessageCat(FileSlotNameGet(FileWork->card_number));
                eMessageCat(periodText);
            }
            break;
        case FILE_STATE_FORMAT_DONE:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, card_text.text[2]);
            break;
        case FILE_STATE_FORMAT_FAILED:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, card_text.text[3]);
            break;
        case FILE_STATE_ERROR_REQUEST_FAILED:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, request_text.text[FileWork->text_index]);
            break;
        case FILE_STATE_ERROR_NO_CARD:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                eMessageCpy(view->text, request_text.text[4]);
                eMessageCat(FileSlotNameGet(FileWork->card_number));
                eMessageCat(periodEndText);
            }
            break;
        case FILE_STATE_ERROR_CANNOT_ACCESS:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, request_text.text[6]);
                    eMessageCat(FileSlotNameGet(FILE_DEVICE_HDD));
                    eMessageCat(request_text.text[8]);
                } else {
                    eMessageCpy(view->text, request_text.text[5]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(request_text.text[7]);
                }
            }
            break;
        case FILE_STATE_ERROR_HDD_TOO_MANY_SAVES:
        case FILE_STATE_ERROR_HDD_TOO_MANY_FOLDERS:
        case FILE_STATE_ERROR_HDD_NO_SPACE:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, progress_text.text[FileWork->state - FILE_STATE_ERROR_HDD_TOO_MANY_SAVES]);
            target_y = 336;
            target_height = 78;
            break;
        case 151:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, FileSlotNameGet(FILE_DEVICE_HDD));
                    eMessageCat(request_text.text[6]);
                } else {
                    char *card_message = view->text;

                    eMessageCpy(card_message, FileSlotNameGet(FileWork->card_number));
                    eMessageCat(request_text.text[5]);
                }
            }
            break;
        case FILE_STATE_CANCEL_CONFIRM:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, request_text.text[FileWork->text_index + 2]);
            break;
        case FILE_STATE_INSERT_CARD:
            if (FileWork->input_flags & FILE_INPUT_ENTER)
                eMessageCpy(view->text, status_text);
            target_y = 336;
            target_height = 78;
            break;
        case FILE_STATE_NO_CARD_SPACE:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, FileSlotNameGet(FILE_DEVICE_HDD));
                    eMessageCat(format_text.text[1]);
                } else {
                    eMessageCpy(view->text, format_text.text[0]);
                    eMessageCat(FileSlotNameGet(FileWork->card_number));
                    eMessageCat(format_text.text[2]);
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
        case FILE_STATE_NO_SAVE_DATA:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (FileWork->use_hdd) {
                    eMessageCpy(view->text, finish_text.text[1]);
                    eMessageCat(FileSlotNameGet(FILE_DEVICE_HDD));
                    eMessageCat(periodEndText);
                } else {
                    eMessageCpy(view->text, finish_text.text[0]);
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
        WindowDXMain(&view->window);
        break;
    }
}

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
            case MC_STATUS_NONE:
            case MC_STATUS_PS1_CARD:
            case MC_STATUS_POCKETSTATION:
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
            WindowDXSet(&view->windows[i]);
            view->windows[i].style = view->work;
            view->windows[i].close_callback_arg = &view->lists[i];
            view->windows[i].close_callback = MenuSelectWindow;
            view->windows[i].width = 245;
            view->windows[i].height = 78;
            view->lists[i].selected = 0;
            view->lists[i].items = view->entries;
            view->windows[i].tag_id = fileSelectText;
            view->lists[i].layout_mode = 2;
        }
        view->windows[i].height = 102;
        confirmation = msg01_2_0036D650;
        for (i = 2; i < 4; ++i) {
            WindowDXSet(&view->windows[i]);
            view->windows[i].style = view->work;
            view->windows[i].close_callback_arg = &view->lists[i];
            view->windows[i].close_callback = MenuSelectWindow;
            view->windows[i].width = 207;
            view->windows[i].tag_id = fileSelectText;
            view->lists[i].title = confirmation->title;
            view->windows[i].height = 102;
            view->lists[i].selected = 0;
            view->lists[i].confirmation_data = confirmation->message;
            view->lists[i].layout_mode = 1;
        }
        for (i = 0; i < 4; ++i) {
            view->windows[i].x = FILE_SELECT_HIDDEN_X;
            view->windows[i].y = 0;
            view->windows[i].state = 1;
            WindowDXMain(&view->windows[i]);
            view->windows[i].state = 3;
            view->targets[i] = FILE_SELECT_HIDDEN_X;
            view->animation_flags[i] = 0;
        }
        view->initial_flags = 0;
        view->initial_state = 0;
        return;
    }
    case 2: {
        short targets[4];

        for (i = 0; i < 4; ++i)
            targets[i] = FILE_SELECT_HIDDEN_X;
        switch (FileWork->state) {
        case FILE_STATE_SELECT_DEVICE:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                view->windows[0].x = FILE_SLIDE_IN_X;
                view->windows[0].y = 176;
            }
            view->lists[0].selected = FileWork->device_selection;
            ListColorChage();
        case FILE_STATE_CHECK_DEVICE:
        case 38:
        case FILE_STATE_LOAD_SLOTS:
            if (!FileWork->hide_selection)
                targets[0] = 128;
            break;
        case FILE_STATE_CONFIRM_SLOT:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                view->windows[2].x = FILE_SLIDE_IN_X;
                view->windows[2].y = 224;
            }
            view->lists[2].selected = FileWork->action_selection;
            targets[2] = 160;
            break;
        case FILE_STATE_FORMAT_CONFIRM:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                view->windows[2].x = FILE_SLIDE_IN_X;
                view->windows[2].y = 160;
            }
            view->lists[2].selected = FileWork->action_selection;
            targets[2] = 160;
            break;
        case FILE_STATE_CANCEL_CONFIRM:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                view->windows[3].x = FILE_SLIDE_IN_X;
                view->windows[3].y = 160;
            }
            view->lists[3].selected = FileWork->action_selection;
            targets[3] = 160;
            break;
        }
        for (i = 0; i < 4; ++i) {
            MoveSlide(&view->windows[i].x, &targets[i], 3.0f);
            WindowDXMain(&view->windows[i]);
        }
        break;
    }
    }
}

static void tskFileEx(TskObject *task, void *data)
{
    FileExplanation *text = data;

    switch (task->state) {
    case 0:
        text->color = 0xffffff;
        eMessageSet(&text->message, 0);
        text->message.draw_parameter = text->color;
        text->message.mode = 32;
        text->open = 0;
        break;
    case 2:
        switch (FileWork->state) {
        case FILE_STATE_SAVE_WRITE:
        case FILE_STATE_SAVE_CREATE:
        case FILE_STATE_LOAD_READ:
        case FILE_STATE_FORMAT:
            if (FileWork->input_flags & FILE_INPUT_ENTER) {
                if (!text->open) {
                    text->open = 1;
                    text->message.x = FILE_SLIDE_IN_X;
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
                eMessageMain(&text->message);
            } while (0);
        }
        break;
    }
}

void FileThumbnailDisp(struct FileThumbnailContext *panel)
{
    int i;
    int party_index;
    int party_count;
    FileObjectEntryView *entry;
    const unsigned char *map_name;
    unsigned char *window_origin;
    int ribbon_width;
    int ribbon_height;

    switch (panel->state) {
        do {
    case 0:
        window_origin = (unsigned char *)&panel->body.window;
        WindowDXSet(&panel->body.window);
        panel->body.window.x = 576;
        panel->body.window.style = panel->color;
        panel->body.window.width = 336;
        panel->body.window.tag_id = shionLevelText;
        panel->body.window.height = 96;
        panel->body.window.state = 1;
        panel->body.window.y = 0;
        WindowDXMain(&panel->body.window);
        panel->body.window.state = 3;
        for (i = 0; i < 2; ++i) {
            eMessageSet(&panel->body.messages[i], 0);
            panel->body.messages[i].draw_parameter = panel->color + 2;
            panel->body.messages[i].mode = 32;
        }
        {
            ETagFontMainData *label = panel->body.labels;
            char **text = msg00_5_0036D668;
            for (i = 0; i < 2; ++i) {
                eTagFontSet(label++, *text++);
                panel->body.labels[i].work = panel->color + 2;
            }
        }
        {
            int *layer = &panel->body.numbers[0].work;
            ENumberMainData *number = panel->body.numbers;
            for (i = 0; i < 5; ++i) {
                eNumberSet(number++, 0);
                *layer = panel->color + 2;
                layer += sizeof(ENumberMainData) / sizeof(int);
            }
        }
        /* The tab's number and ribbon layers are reached from the window
           origin by byte offsets: the original keeps the window pointer and
           one offset register shared by the ribbon fields. */
        {
            int *label_layer = &panel->tab.label.work;
            int *number_layer;
            int ribbon_offset;
            unsigned char *ribbon_fields;
            int *ribbon_layer;
            eTagFontSet(&panel->tab.label, slotNumberText);
            *label_layer = panel->color + 2;
            number_layer = (int *)(window_origin + ((unsigned char *)&panel->tab.number.work -(unsigned char *)&panel->body.window));
            ribbon_width = 144;
            ribbon_height = 110;
            eNumberSet(&panel->tab.number, 0);
            *number_layer = panel->color + 2;
            eRibbonSet(&panel->tab.ribbon, 1);
            ribbon_offset = (unsigned char *)&panel->tab.ribbon.work - (unsigned char *)&panel->body.window;
            ribbon_fields = (unsigned char *)panel + ribbon_offset;
            ribbon_layer = (int *)(window_origin + ribbon_offset);
            *(short *)(ribbon_fields + 16) = ribbon_width;
            *ribbon_layer = panel->color + 1;
            *(short *)(ribbon_fields + 18) = ribbon_height;
        }
        panel->state = 2;
        /* Fall through to the first display. */
    case 2:
        if (!panel->visible)
            break;
        entry = FileObjectData + panel->slot;
        if (!entry)
            break;
        panel->body.window.x = panel->x + 144;
        panel->body.window.y = panel->y;
        WindowDXMain(&panel->body.window);
        {
            ETagFontMainData *label = panel->body.labels;
            int row_offset = 0;
            for (i = 0; i < 2; ++i) {
                panel->body.labels[i].x = panel->body.window.x + 67;
                panel->body.labels[i].y = panel->body.window.y + row_offset + 54;
                eTagFontMain(label++);
                row_offset += 18;
            }
        }
        switch (FileObjectDataSystem->state[panel->slot]) {
        case FILE_SLOT_NEW:
            panel->body.messages[1].text = 0;
            panel->body.messages[0].text = newFileText;
            break;
        case FILE_SLOT_CORRUPT:
            panel->body.messages[1].text = 0;
            panel->body.messages[0].text = corruptDataText;
            break;
        case FILE_SLOT_FULL:
            if (FileWork->text_index == FILE_MODE_SAVE)
                panel->body.messages[0].text = cannotSaveText;
            else
                panel->body.messages[0].text = cannotLoadText;
            panel->body.messages[1].text = 0;
            break;
        default:
            map_name = MenuSaveMapNameGet(entry->map_number);
            panel->body.messages[0].text = (const char *)map_name;
            panel->body.messages[1].text = (const char *)map_name + FILE_MAP_NAME_LINE_SIZE;
            break;
        }
        {
            EMessageParam *message = panel->body.messages;
            int row_offset = 0;
            for (i = 0; i < 2; ++i) {
                panel->body.messages[i].x = panel->body.window.x + 3;
                panel->body.messages[i].y = panel->body.window.y + row_offset + 3;
                eMessageMain(message++);
                row_offset += 24;
            }
        }
        panel->body.numbers[0].value = entry->elapsed.packed;
        if (FileObjectDataSystem->state[panel->slot] == FILE_SLOT_EXISTING)
            panel->body.numbers[0].format = 2;
        else
            panel->body.numbers[0].format = 11;
        panel->body.numbers[1].value = entry->play_time;
        panel->body.numbers[1].digits = 9;
        panel->body.numbers[1].format = 14;
        {
            ENumberMainData *number = panel->body.numbers;
            for (i = 0; i < 2; ++i) {
                int row_offset = i * 9;
                panel->body.numbers[i].x = panel->body.window.x + 72;
                panel->body.numbers[i].y = panel->body.window.y + row_offset * 2 + 54;
                eNumberMain(number++);
            }
        }
        panel->body.numbers[2].digits = 1;
        panel->body.numbers[2].value = entry->chapter;
        panel->body.numbers[2].format = 1;
        panel->body.numbers[3].digits = 2;
        panel->body.numbers[3].value = entry->leader;
        panel->body.numbers[3].format = 1;
        /* i is 2 after the loop: the leader number follows the chapter. */
        panel->body.numbers[i + 1].x = panel->body.window.x + 82;
        panel->body.numbers[i + 1].y = panel->body.window.y - 13;
        party_index = 0;
        eNumberMain(&panel->body.numbers[3]);
        party_count = 0;
        for (; party_index < FILE_PARTY_IDS_MAX && entry->party_ids[party_index] != 0; ++party_index) {
            if (++party_count == FILE_PARTY_FACES_MAX)
                break;
        }
if (party_count > 0) {
            struct FileFaceSprite *face = panel->body.faces;
            int column_offset = 0;
            int *member = entry->party_ids;
            int *layer = &panel->body.faces[0].color;
            party_index = party_count;
            do {
                eSpriteSet(face, (short)MenuFaceEpidGet(*member++, 1));
                face->x = panel->body.window.x + column_offset * 4 + 196;
                *layer = panel->body.window.style + 2;
                face->y = panel->body.window.y + 58;
                eSpriteMain(face++);
                layer += sizeof(struct FileFaceSprite) / sizeof(int);
                column_offset += 5;
                --party_index;
            } while (party_index != 0);
        }
        panel->tab.ribbon.x = panel->x;
        panel->tab.ribbon.y = panel->y - 14;
        eRibbonMain(&panel->tab.ribbon);
        panel->tab.label.x = panel->x + 8;
        panel->tab.label.y = panel->y - 14;
        eTagFontMain(&panel->tab.label);
        panel->tab.number.digits = 2;
        panel->tab.number.value = panel->slot + 1;
        panel->tab.number.format = 1;
        panel->tab.number.x = panel->tab.label.x + 30;
        panel->tab.number.y = panel->tab.label.y + 2;
        eNumberMain(&panel->tab.number);
        panel->thumbnail.packet = &entry->jpeg->dma_qwc;
        panel->thumbnail.x = panel->x + 16;
        panel->thumbnail.y = panel->y + 1;
        panel->thumbnail.color = panel->color + 2;
        panel->thumbnail.channels[0] = panel->thumbnail.channels[1] =
            panel->thumbnail.channels[2] = panel->thumbnail.channels[3] = -128;
        endPrintExtFunc(panel->thumbnail.color, END_PRINT_CMD_THUMBNAIL,&panel->thumbnail);
        break;
        } while (0);
    }
}

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
        int offscreen_x = FILE_PANEL_OFFSCREEN_X;

        view->work = 0xffff00;
        for (i = 0; i < FILE_PANELS_MAX; ++i) {
            view->panels[i].color = view->work;
            view->panels[i].y = FILE_PANEL_TOP + i * FILE_PANEL_PITCH;
            view->panels[i].x = offscreen_x;
            view->panels[i].slot = 0;
            view->panels[i].state = 0;
            view->panels[i].visible = 0;
        }
        view->panels[3].x = FILE_PANEL_X;
        eCursolSet(&view->cursor, 0);
        view->cursor.work = view->work + 2;
        view->cursor.state = 32;
        view->visible = 0;
        view->initial_value = 0;
        view->initial_flags = 0;
        view->state = FILE_PANEL_HIDDEN;
        view->extra_panel = 0;
        return;
    }
    case 2:
        switch (view->state) {
        case FILE_PANEL_HIDDEN:
            view->visible = 0;
            view->state = FILE_PANEL_WAIT;
        case FILE_PANEL_WAIT:
            if (FileWork->state == FILE_STATE_SLOT_LIST) {
                view->visible = 1;
                view->state = FILE_PANEL_OPEN;
            }
            break;
        case FILE_PANEL_OPEN:
            for (i = 0; i < FILE_PANELS_VISIBLE; ++i) {
                view->panels[i].y = FILE_PANEL_TOP + i * FILE_PANEL_PITCH;
                view->panels[i].x = FILE_PANEL_ENTER_X;
            }
            view->cursor.state = 32;
            view->state = FILE_PANEL_SLIDE_IN;
        case FILE_PANEL_SLIDE_IN: {
            int panel_index;

            complete = 0;
            entrance_x = FILE_PANEL_X;
            for (panel_index = 0; panel_index < FILE_PANELS_VISIBLE; ++panel_index) {
                MoveSlide(&view->panels[panel_index].x, &entrance_x, 3.0f);
                if (view->panels[panel_index].x == entrance_x) {
                    entrance_y = FILE_PANEL_TOP + panel_index * FILE_PANEL_PITCH;
                    MoveSlide(&view->panels[panel_index].y, &entrance_y, 3.0f);
                    if (view->panels[panel_index].y == entrance_y)
                        ++complete;
                }
            }
            if (complete == FILE_PANELS_VISIBLE)
                view->state = FILE_PANEL_BROWSE;
            break;
        }
        case FILE_PANEL_SLIDE_OUT: {
            int exit_complete;
            int panel_index;

            exit_complete = 0;
            exit_x = FILE_PANEL_OFFSCREEN_X;
            exit_y = FILE_PANEL_TOP;
            for (panel_index = 0; panel_index < FILE_PANELS_VISIBLE; ++panel_index) {
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
            if (exit_complete == FILE_PANELS_VISIBLE)
                view->state = FILE_PANEL_EXPANDED;
            break;
        }
        case FILE_PANEL_EXPAND: {

            expanded_y = 112;
            MoveSlide(&view->panels[FileWork->selected_panel].y, &expanded_y, 3.0f);
            if (view->panels[FileWork->selected_panel].y == expanded_y)
                view->state = FILE_PANEL_EXPANDED;
            break;
        }
        case FILE_PANEL_CLOSE: {
            int exit_complete;
            int panel_index;

            exit_complete = 0;
            closing_x = FILE_PANEL_OFFSCREEN_X;
            for (panel_index = 0; panel_index < FILE_PANELS_VISIBLE; ++panel_index) {
                MoveSlide(&view->panels[panel_index].x, &closing_x, 3.0f);
                if (view->panels[panel_index].x == closing_x)
                    ++exit_complete;
            }
            if (exit_complete == FILE_PANELS_VISIBLE)
                view->state = FILE_PANEL_HIDDEN;
            break;
        }
        case FILE_PANEL_BROWSE:
            switch (FileWork->state) {
            case FILE_STATE_SELECT_DEVICE:
            case FILE_STATE_ERROR_REQUEST_FAILED:
            case FILE_STATE_ERROR_NO_CARD:
            case FILE_STATE_ERROR_CANNOT_ACCESS:
            case 147:
            case FILE_STATE_CLOSING:
                view->state = FILE_PANEL_CLOSE;
                break;
            case FILE_STATE_SLOT_REJECTED:
            case FILE_STATE_CONFIRM_SLOT:
                view->state = FILE_PANEL_SLIDE_OUT;
                break;
            case FILE_STATE_SAVE_WRITE:
            case FILE_STATE_SAVE_CREATE:
            case FILE_STATE_SAVE_DONE:
            case FILE_STATE_LOAD_READ:
                view->state = FILE_PANEL_EXPAND;
                break;
            }
            if (FileWork->scroll_direction) {
                int scroll_speed;
                int scroll_index;

                for (scroll_index = 0; scroll_index < FILE_PANELS_VISIBLE; ++scroll_index) {
                    int direction = FileWork->scroll_direction;

                    view->panels[scroll_index].y += direction * FILE_PANEL_PITCH;
                }
                {
                    FileWorkBlock *scroll_work = FileWork;

                    view->panels[3].x = FILE_PANEL_X;
                    if (scroll_work->scroll_direction < 0)
                        view->panels[3].y = 288;
                    else
                        view->panels[3].y = FILE_PANEL_TOP;
                    scroll_speed = FILE_SCROLL_SPEED;
                    if (scroll_work->fast_scroll)
                        scroll_speed = FILE_SCROLL_SPEED_FAST;
                    for (scroll_index = 0; scroll_index < FILE_PANELS_MAX; ++scroll_index)
                        view->panels[scroll_index].y += scroll_speed * -FileWork->scroll_direction;
                }
                view->extra_panel = 1;
                view->state = FILE_PANEL_SCROLL;
            }
            break;
        case FILE_PANEL_EXPANDED:
            switch (FileWork->state) {
            case FILE_STATE_ERROR_REQUEST_FAILED:
            case FILE_STATE_ERROR_NO_CARD:
            case FILE_STATE_ERROR_CANNOT_ACCESS:
            case 147:
            case FILE_STATE_ERROR_HDD_TOO_MANY_SAVES:
            case FILE_STATE_ERROR_HDD_TOO_MANY_FOLDERS:
            case FILE_STATE_ERROR_HDD_NO_SPACE:
            case 151:
            case 152:
                view->state = FILE_PANEL_CLOSE;
                break;
            case FILE_STATE_CLOSING:
                view->state = FILE_PANEL_CLOSE;
                break;
            case FILE_STATE_SAVE_WRITE:
            case FILE_STATE_SAVE_CREATE:
            case FILE_STATE_SAVE_DONE:
                view->state = FILE_PANEL_EXPAND;
                break;
            case FILE_STATE_SLOT_LIST:
                view->state = FILE_PANEL_SLIDE_IN;
                break;
            }
            break;
        case FILE_PANEL_SCROLL: {
            int scroll_speed = FILE_SCROLL_SPEED;

            if (FileWork->fast_scroll)
                scroll_speed = FILE_SCROLL_SPEED_FAST;

            for (i = 0; i < FILE_PANELS_MAX; ++i)
                view->panels[i].y += scroll_speed * -FileWork->scroll_direction;
            if (view->panels[0].y == FILE_PANEL_TOP) {
                view->extra_panel = 0;
                view->state = FILE_PANEL_BROWSE;
                FileWork->scroll_direction = 0;
            }
            break;
        }
        }
        if (view->visible) {
            FileWorkBlock *work;

            for (i = 0; i < FILE_PANELS_VISIBLE; ++i)
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
            endPrintExtFunc(view->work, END_PRINT_CMD_SCISSOR_SET,&view->clip);
            endPrintExtFunc(view->work, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET, &view->clip);
            for (i = 0; i < view->extra_panel + FILE_PANELS_VISIBLE; ++i) {
                view->panels[i].visible = 1;
                FileThumbnailDisp(&view->panels[i]);
            }
            work = FileWork;
            view->cursor.x = view->panels[work->selected_panel].x;
            view->cursor.y = view->panels[work->selected_panel].y + 32;
            eCursolMain(&view->cursor);
            endPrintExtFunc(view->work, END_PRINT_CMD_SCISSOR_RESET, 0);
            endPrintExtFunc(view->work, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
        }
        break;
    }
}

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
        FileWork->input_flags |= FILE_INPUT_ENTER;
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
    case FILE_STATE_SELECT_DEVICE: {
        signed char selection;
        unsigned char input_flags = FileWork->input_flags;
        if (input_flags & FILE_INPUT_ENTER) {
            if (FileWork->use_hdd) {
                FileWork->device_selection = FILE_DEVICE_CANCEL;
            } else {
                FileWork->device_selection = FileWork->card_number;
            }
            FileWork->hdd_status = (unsigned short) xglHddCheck();
            FileWork->dispatch_delay = FILE_DELAY_SHORT;
            FileWork->hide_selection = 0;
            break;
        }
        else if (input_flags & FILE_INPUT_SKIP) {
            break;
        } else {
            FileWork->device_selection = MenuSelectMove(FileWork->device_selection, FILE_DEVICE_ROWS, 0);
            if (PadData.half_2a & PAD_CIRCLE) {
                selection = FileWork->device_selection;
                switch (selection) {
                    case 0:
                    case 1:
                        FileWork->use_hdd = 0;
                        FileWork->card_number = FileWork->device_selection;
                        card = &FileWork->card;
                        card_status = card->status[FileWork->card_number];
                        switch (card_status) {
                        case MC_STATUS_NONE:
                        case MC_STATUS_PS1_CARD:
                        case MC_STATUS_POCKETSTATION:
                            xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                            break;
                        case MC_STATUS_UNFORMATTED:
                            if (FileWork->text_index == FILE_MODE_SAVE) {
                                FileWork->command = FILE_STATE_CARD_UNFORMATTED;
                            } else {
                                FileWork->command = FILE_STATE_NO_SAVE_DATA;
                            }
                            xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                            break;
                        case MC_STATUS_BUSY:
                            break;
                        default:
                            FileWork->command = FILE_STATE_CHECK_DEVICE;
                            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                            break;
                        }
                        break;
                    case FILE_DEVICE_CANCEL:
                        xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                        FileWork->command = FILE_STATE_CLOSING;
                        if (FileWork->text_index == FILE_MODE_LOAD) {
                            FileWork->result = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
            if (PadData.half_2a & PAD_CROSS) {
                FileWork->command = FILE_STATE_CLOSING;
                xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                if (FileWork->text_index == FILE_MODE_LOAD) {
                    FileWork->result = 1;
                }
            }
        }
        break;
    }
    case FILE_STATE_CHECK_DEVICE:
    {
        int hdd_slot;
        int card_slot;
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            if (FileWork->use_hdd) {
                FileWork->dispatch_delay = FILE_DELAY_SHORT;
                FileWork->available_blocks = xglHddMcGetFree();
            } else {
                xglMcRequest(FileWork->card_number, MC_REQUEST_FREE_SPACE, 0,
                             &FileWork->available_blocks);
                card_request.slot = -1;
                card_request.buffer = (int)FileExistCheckWork;
                xglMcRequest(FileWork->card_number, MC_REQUEST_DIRECTORY, &card_request,
                             &FileWork->directory_result);
                FileWork->menu_substate = 0;
            }
        } else if (FileWork->use_hdd) {
            if (FileWork->available_blocks < 0) {
                if (FileWork->text_index == FILE_MODE_SAVE) {
                    switch (FileWork->available_blocks) {
                    case HDD_CHECK_ACCESS:
                        FileWork->command = FILE_STATE_ERROR_CANNOT_ACCESS;
                        break;
                    case HDD_CHECK_TOO_MANY_FOLDERS:
                        FileWork->command = FILE_STATE_ERROR_HDD_TOO_MANY_FOLDERS;
                        break;
                    case HDD_CHECK_TOO_MANY_SAVES:
                        FileWork->command = FILE_STATE_ERROR_HDD_TOO_MANY_SAVES;
                        break;
                    case HDD_CHECK_NO_SPACE:
                        FileWork->command = FILE_STATE_ERROR_HDD_NO_SPACE;
                        break;
                    }
                } else {
                    switch (FileWork->available_blocks) {
                    case HDD_CHECK_ACCESS:
                        FileWork->command = FILE_STATE_ERROR_CANNOT_ACCESS;
                        break;
                    case HDD_CHECK_TOO_MANY_FOLDERS:
                    case HDD_CHECK_TOO_MANY_SAVES:
                    case HDD_CHECK_NO_SPACE:
                        FileWork->command = FILE_STATE_NO_SAVE_DATA;
                        break;
                    }
                }
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                FileWork->dispatch_delay = FILE_DELAY_LONG;
                break;
            }
            hdd_request.exist.card = -1;
            hdd_request.exist.found = FileExistCheckWork;
            FileWork->directory_result = xglHddMcExist(&hdd_request.exist);
            if (FileWork->directory_result == 0) {
                if (FileWork->text_index == FILE_MODE_SAVE) {
                if (FileWork->available_blocks < FILE_HDD_SAVE_KB) {
                    FileWork->command = FILE_STATE_NO_CARD_SPACE;
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                } else {
                    for (hdd_slot = 0; hdd_slot < FILE_SLOT_COUNT; hdd_slot++) {
                        if (FileWork->available_blocks >= FILE_HDD_SAVE_KB) {
                            FileObjectDataSystem->state[hdd_slot] = FILE_SLOT_NEW;
                            FileWork->available_blocks -= FILE_HDD_SAVE_KB;
                        } else {
                            FileObjectDataSystem->state[hdd_slot] = FILE_SLOT_FULL;
                        }
                    }
                    FileExistCheckWork[MC_LATEST_SLOT_INDEX] = 0;
                    FileWork->command = FILE_STATE_LOAD_SLOTS;
                }
                } else {
                    FileWork->command = FILE_STATE_NO_SAVE_DATA;
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                }
            } else if (FileWork->directory_result < 0) {
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                FileWork->command = FILE_STATE_ERROR_CANNOT_ACCESS;
            } else {
                FileWork->selected_slot = 0;
                FileWork->slot_count = FILE_SLOT_COUNT;
                if (FileWork->directory_result == 0 &&
                    FileWork->available_blocks < FILE_HDD_SAVE_KB) {
                    if (FileWork->text_index == FILE_MODE_SAVE) {
                        FileWork->command = FILE_STATE_NO_CARD_SPACE;
                    } else {
                        FileWork->command = FILE_STATE_NO_SAVE_DATA;
                    }
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                    break;
                }
                for (hdd_slot = 0; hdd_slot < FILE_SLOT_COUNT; hdd_slot++) {
                    if (FileExistCheckWork[hdd_slot] != 0) {
                        FileObjectDataSystem->state[hdd_slot] = FILE_SLOT_EXISTING;
                    } else if (FileWork->available_blocks >= FILE_HDD_SAVE_KB) {
                        FileObjectDataSystem->state[hdd_slot] = FILE_SLOT_NEW;
                        FileWork->available_blocks -= FILE_HDD_SAVE_KB;
                    } else {
                        FileObjectDataSystem->state[hdd_slot] = FILE_SLOT_FULL;
                    }
                }
                if (FileWork->text_index == FILE_MODE_SAVE) {
                    FileWork->command = FILE_STATE_LOAD_SLOTS;
                } else {
                    switch (FileWork->directory_result) {
                    default:
                        FileWork->command = FILE_STATE_LOAD_SLOTS;
                        break;
                    case 0:
                        FileWork->command = FILE_STATE_NO_SAVE_DATA;
                        xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                        break;
                    }
                }
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case MC_STATUS_READY:
                if (FileWork->menu_substate != 0) {
                    FileWork->selected_slot = 0;
                    FileWork->slot_count = FILE_SLOT_COUNT;
                    if (FileWork->directory_result == 0 &&
                        FileWork->available_blocks < FILE_CARD_SAVE_KB) {
                        if (FileWork->text_index == FILE_MODE_SAVE) {
                            FileWork->command = FILE_STATE_NO_CARD_SPACE;
                        } else {
                            FileWork->command = FILE_STATE_NO_SAVE_DATA;
                        }
                        xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                        break;
                    } else {
                        for (card_slot = 0; card_slot < FILE_SLOT_COUNT; card_slot++) {
                            if (FileExistCheckWork[card_slot] != 0) {
                                FileObjectDataSystem->state[card_slot] = FILE_SLOT_EXISTING;
                            } else if (FileWork->available_blocks >= FILE_CARD_SAVE_KB) {
                                FileObjectDataSystem->state[card_slot] = FILE_SLOT_NEW;
                                FileWork->available_blocks -= FILE_CARD_SAVE_KB;
                            } else {
                                FileObjectDataSystem->state[card_slot] = FILE_SLOT_FULL;
                            }
                        }
                        if (FileWork->directory_result < 0) {
                            FileWork->command = FILE_STATE_ERROR_NO_CARD;
                            xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                        } else {
                            if (FileWork->text_index == FILE_MODE_SAVE) {
                                FileWork->command = FILE_STATE_LOAD_SLOTS;
                            } else {
                                switch (FileWork->directory_result) {
                                default:
                                    FileWork->command = FILE_STATE_LOAD_SLOTS;
                                    break;
                                case 0:
                                    FileWork->command = FILE_STATE_NO_SAVE_DATA;
                                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                                    break;
                                }
                            }
                        }
                    }
                }
                FileWork->menu_substate = 1;
                break;
            case MC_STATUS_NONE:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            case MC_STATUS_BUSY:
                break;
            default:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            }
        }
        break;
    }
    case FILE_STATE_DONE:
        break;
    case FILE_STATE_LOAD_SLOTS:
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            FileWork->selected_slot = FileExistCheckWork[MC_LATEST_SLOT_INDEX];
            FileWork->panel_count = FILE_PANELS_VISIBLE;
            FileWork->selected_panel = 0;
            FileWork->scroll_direction = 0;
            FileWork->current_slot = FileWork->selected_slot;
            FileObjectDataClear();
            FileObjectDataSystem->unmodeled_00[1] = FileWork->current_slot;
        } else if (FileWork->use_hdd) {
            int load_result = FileObjectAllLoad();
            if (load_result == 0) {
                if (FileWork->hdd_result >= 0) {
                    FileWork->slot_count = FILE_SLOT_COUNT;
                    FileObjectJpegSet();
                    FileWork->command = FILE_STATE_SLOT_LIST;
                    FileWork->dispatch_delay = FILE_DELAY_SHORT;
                }
            }
            if (FileWork->hdd_result != 0) {
                FileWork->command = FILE_STATE_ERROR_CANNOT_ACCESS;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
            }
        } else {
            unsigned short loaded_card_status;
            int load_result = FileObjectAllLoad();
            if (load_result == 0) {
                if (FileWork->hdd_result >= 0) {
                    FileWork->slot_count = FILE_SLOT_COUNT;
                    FileObjectJpegSet();
                    FileWork->command = FILE_STATE_SLOT_LIST;
                    FileWork->dispatch_delay = FILE_DELAY_SHORT;
                }
            }
            if (FileWork->hdd_result >= 0) {
                break;
            }
            card = &FileWork->card;
            loaded_card_status = card->status[FileWork->card_number];
            switch (loaded_card_status) {
            default:
                FileWork->command = FILE_STATE_ERROR_CANNOT_ACCESS;
                FileWork->slot_count = 0;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            case MC_STATUS_BUSY:
                break;
            case MC_STATUS_NONE:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            }
        }
        break;
    case FILE_STATE_SLOT_LIST:
    {
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            FileWork->scroll_direction = 0;
        }
        if ((FileWork->input_flags & FILE_INPUT_SKIP) == 0) {
            if (FileWork->scroll_direction == 0) {
                unsigned short action = 0;

                if (FileWork->fast_scroll != 0) {
                    if (FileWork->scroll_repeat_count == FILE_FAST_SCROLL_STEPS) {
                        FileWork->fast_scroll = 0;
                    } else {
                        FileWork->scroll_repeat_count++;
                        action = FileWork->scroll_repeat_direction < 0
                                     ? PAD_UP
                                     : PAD_DOWN;
                    }
                    FileWork->menu_substate = 0;
                } else {
                    if (FileWork->menu_substate == 0) {
                        u16 menu_input = PadData.half_30;

                        if (menu_input & PAD_UP) {
                            if (FileWork->selected_panel == 0 ||
                                (PadData.mappedRepeat & PAD_UP)) {
                                action = PAD_UP;
                            }
                        } else if (menu_input & PAD_DOWN) {
                            if (FileWork->selected_panel == FILE_PANELS_VISIBLE - 1 ||
                                (PadData.mappedRepeat & PAD_DOWN)) {
                                action = PAD_DOWN;
                            }
                        } else {
                            action = PAD_CROSS;
                            if ((menu_input & PAD_CROSS) == 0) {
                                action = menu_input & PAD_CIRCLE;
                            }
                        }
                        if (action != 0) {
                            FileWork->menu_substate = FILE_INPUT_REPEAT_DELAY;
                        }
                    } else {
                        FileWork->menu_substate--;
                    }
                }
                if (action == PAD_UP) {
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
                    xglSoundEffectNormalID(FILE_SE_CURSOR, 0);
                }
                if (action == PAD_DOWN) {
                    FileWork->selected_slot = FileObjectDataNoGet(
                        FileWork->selected_slot, 1);
                    if (FileWork->selected_panel ==
                        FileWork->panel_count - 1) {
                        FileWork->scroll_direction = 1;
                        FileWork->current_slot = FileObjectDataNoGet(
                            FileWork->current_slot, 1);
                        FileObjectJpegSet();
                    } else {
                        FileWork->selected_panel++;
                    }
                    xglSoundEffectNormalID(FILE_SE_CURSOR, 0);
                }
                if (action == PAD_CROSS) {
                    FileWork->command = FILE_STATE_SELECT_DEVICE;
                    xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                }
                if (action == PAD_CIRCLE) {
                    unsigned char object_status =
                        FileObjectDataSystem->state[FileWork->selected_slot];
                    switch (object_status) {
                    case FILE_SLOT_NEW:
                        if (FileWork->text_index == FILE_MODE_SAVE) {
                            FileWork->command = FILE_STATE_CONFIRM_SLOT;
                            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                        } else {
                            FileWork->command = FILE_STATE_SLOT_REJECTED;
                            xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                        }
                        break;
                    case FILE_SLOT_FULL:
                    case FILE_SLOT_CORRUPT:
                        FileWork->command = FILE_STATE_SLOT_REJECTED;
                        xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                        break;
                    default:
                        if (FileWork->text_index == FILE_MODE_SAVE) {
                            FileWork->command = FILE_STATE_CONFIRM_SLOT;
                            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                        } else {
                            signed char selected_slot = FileWork->selected_slot;
                            FileObjectEntryView *selected_entry =
                                &FileObjectData[selected_slot];
                            if (selected_entry->map_number == FILE_UNIT_MAP_NO) {
                                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                            } else {
                                FileWork->command = FILE_STATE_CONFIRM_SLOT;
                                xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
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
            case MC_STATUS_READY:
            case MC_STATUS_BUSY:
                break;
            case MC_STATUS_NONE:
            default:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            }
        }
        break;
    }
    case FILE_STATE_SLOT_REJECTED: {
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            FileWork->dispatch_delay = FILE_DELAY_SHORT;
        } else if (FileWork->use_hdd) {
            if (PadData.half_2a & PAD_CIRCLE) {
                FileWork->command = FILE_STATE_SLOT_LIST;
                FileWork->dispatch_delay = FILE_DELAY_SHORT;
                xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case MC_STATUS_READY:
                if (PadData.half_2a & PAD_CIRCLE) {
                    FileWork->command = FILE_STATE_SLOT_LIST;
                    FileWork->dispatch_delay = FILE_DELAY_SHORT;
                    xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                }
                break;
            case MC_STATUS_BUSY:
                break;
            case MC_STATUS_NONE:
            default:
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                break;
            }
        }
        break;
    }
    case FILE_STATE_CONFIRM_SLOT: {
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            FileWork->dispatch_delay = 10;
            if (FileWork->text_index == FILE_MODE_SAVE) {
                FileWork->action_selection = FILE_CHOICE_NO;
            } else {
                FileWork->action_selection = FILE_CHOICE_YES;
            }
            if (!FileWork->use_hdd) {
                xglMcRequest(FileWork->card_number, MC_REQUEST_FREE_SPACE, 0,
                             &FileWork->available_blocks);
            }
        } else if (FileWork->use_hdd) {
            if (!(FileWork->input_flags & FILE_INPUT_SKIP)) {
                FileWork->action_selection =
                    MenuSelectMove(FileWork->action_selection, FILE_CONFIRM_ROWS, 0);
                if (PadData.half_2a & PAD_CIRCLE) {
                    if (FileWork->action_selection == FILE_CHOICE_YES) {
                        if (FileWork->text_index != FILE_MODE_SAVE) {
                            FileWork->command = FILE_STATE_LOAD_READ;
                        } else {
                            switch (FileObjectDataSystem->state[FileWork->selected_slot]) {
                            case FILE_SLOT_NEW:
                                FileWork->command = FILE_STATE_SAVE_CREATE;
                                break;
                            default:
                                FileWork->command = FILE_STATE_SAVE_CREATE;
                                break;
                            }
                        }
                        xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                    } else {
                        FileWork->command = FILE_STATE_SLOT_LIST;
                        FileWork->dispatch_delay = FILE_DELAY_SHORT;
                        xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                    }
                }
                if (PadData.half_2a & PAD_CROSS) {
                    FileWork->command = FILE_STATE_SLOT_LIST;
                    FileWork->dispatch_delay = FILE_DELAY_SHORT;
                    xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                }
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case MC_STATUS_NONE:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            case MC_STATUS_READY:
            if (!(FileWork->input_flags & FILE_INPUT_SKIP)) {
                FileWork->action_selection =
                    MenuSelectMove(FileWork->action_selection, FILE_CONFIRM_ROWS, 0);
                if (PadData.half_2a & PAD_CIRCLE) {
                        if (FileWork->action_selection == FILE_CHOICE_YES) {
                            if (FileWork->text_index != FILE_MODE_SAVE) {
                                FileWork->command = FILE_STATE_LOAD_READ;
                            } else if (FileObjectDataSystem->state[FileWork->selected_slot] != FILE_SLOT_NEW ||
                                       FileWork->available_blocks >= FILE_CARD_SAVE_KB) {
                                FileWork->command = FILE_STATE_SAVE_CREATE;
                            } else {
                                FileWork->command = FILE_STATE_NO_CARD_SPACE;
                            }
                        xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                    } else {
                        FileWork->command = FILE_STATE_SLOT_LIST;
                        FileWork->dispatch_delay = FILE_DELAY_SHORT;
                        xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                    }
                }
                if (PadData.half_2a & PAD_CROSS) {
                    FileWork->dispatch_delay = FILE_DELAY_SHORT;
                    FileWork->command = FILE_STATE_SLOT_LIST;
                    xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                }
            }
                break;
            case MC_STATUS_BUSY:
                break;
            default:
                FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                break;
            }
        }
        break;
    }
    case FILE_STATE_SAVE_WRITE:
    {
        int hdd_existing_slot;
        int card_existing_slot;
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            if (FileWork->use_hdd) {
                FileWork->dispatch_delay = FILE_DELAY_LONG;
            } else {
                card_request.slot = FileWork->selected_slot;
                card_request.buffer = (int)SaveData;
                card_request.size = MC_SAVE_DATA_SIZE;
                xglMcRequest(FileWork->card_number, MC_REQUEST_SAVE, &card_request,
                             &FileWork->hdd_result);
                xglMcRequest(FileWork->card_number, MC_REQUEST_FREE_SPACE, 0,
                             &FileWork->available_blocks);
                FileWork->menu_substate = 0;
                tyaDrawGauge(&gauge);
            }
        } else if (FileWork->use_hdd) {
            hdd_request.save.card = FileWork->selected_slot;
            hdd_request.save.data = SaveData;
            hdd_request.save.size = MC_SAVE_DATA_SIZE;
            FileWork->directory_result = xglHddMcSave(&hdd_request.save);
            if (FileWork->directory_result != 0) {
                FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
            } else {
                xglSoundEffectNormalID(FILE_SE_SAVE_DONE, 0);
                FileWork->available_blocks = xglHddMcGetFree();
                *FileSaveData = *(FileSaveRecord *) SaveData;
                hdd_existing_slot = 0;
                FileObjectDataInput(FileWork->selected_slot, FILE_SLOT_EXISTING);
                FileObjectDataSystem->state[FileWork->selected_slot] = FILE_SLOT_EXISTING;
                FileObjectJpegDecChange(FileWork->selected_slot);
                    for (; hdd_existing_slot < FILE_SLOT_COUNT; hdd_existing_slot++) {
                        if (FileObjectDataSystem->state[hdd_existing_slot] == FILE_SLOT_NEW) {
                        FileWork->available_blocks -= FILE_HDD_SAVE_KB;
                    }
                }
                for (hdd_existing_slot = 0; hdd_existing_slot < FILE_SLOT_COUNT; hdd_existing_slot++) {
                    if (FileObjectDataSystem->state[hdd_existing_slot] == FILE_SLOT_FULL &&
                        FileWork->available_blocks >= FILE_HDD_SAVE_KB) {
                        FileObjectDataInput(hdd_existing_slot, FILE_SLOT_NEW);
                        FileObjectDataSystem->state[hdd_existing_slot] = FILE_SLOT_NEW;
                        FileObjectJpegDecChange(hdd_existing_slot);
                        FileWork->available_blocks -= FILE_HDD_SAVE_KB;
                    }
                }
                FileWork->command = FILE_STATE_SAVE_DONE;
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            if (card_status != MC_STATUS_READY) {
                if (card_status != MC_STATUS_BUSY) {
                    FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                } else {
                    tyaDrawGauge(&gauge);
                }
            } else if (FileWork->menu_substate == 0) {
                xglSoundEffectNormalID(FILE_SE_SAVE_DONE, 0);
                *FileSaveData = *(FileSaveRecord *) SaveData;
                FileObjectDataInput(FileWork->selected_slot, FILE_SLOT_EXISTING);
                FileObjectDataSystem->state[FileWork->selected_slot] = FILE_SLOT_EXISTING;
                FileObjectJpegDecChange(FileWork->selected_slot);
                FileWork->menu_substate = 1;
                tyaDrawGauge(&gauge);
            } else {
                for (card_existing_slot = 0; card_existing_slot < FILE_SLOT_COUNT; card_existing_slot++) {
                    if (FileObjectDataSystem->state[card_existing_slot] == FILE_SLOT_NEW) {
                        FileWork->available_blocks -= FILE_CARD_SAVE_KB;
                    }
                }
                for (card_existing_slot = 0; card_existing_slot < FILE_SLOT_COUNT; card_existing_slot++) {
                    if (FileObjectDataSystem->state[card_existing_slot] == FILE_SLOT_FULL &&
                        FileWork->available_blocks >= FILE_CARD_SAVE_KB) {
                        FileObjectDataInput(card_existing_slot, FILE_SLOT_NEW);
                        FileObjectDataSystem->state[card_existing_slot] = FILE_SLOT_NEW;
                        FileObjectJpegDecChange(card_existing_slot);
                        FileWork->available_blocks -= FILE_CARD_SAVE_KB;
                    }
                }
                FileWork->command = FILE_STATE_SAVE_DONE;
            }
        }
        break;
    }
    case FILE_STATE_SAVE_CREATE: {
        int create_result;

        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            if (FileWork->use_hdd) {
                FileWork->menu_substate = FILE_WAIT_FRAMES;
            } else {
                card_request.slot = FileWork->selected_slot;
                card_request.buffer = (int)FileCardDataBuf;
                FileSaveDataPush();
                gauge.x = 128;
                gauge.y = FILE_GAUGE_Y;
                gauge.vertex_attribute = 0xffffffffU;
                gauge.width = FILE_GAUGE_WIDTH;
                gauge.height = FILE_GAUGE_HEIGHT;
                gauge.current = 0;
                gauge.maximum = FILE_CREATE_STEPS;
                gauge.filled_color.red = FILE_GAUGE_FILLED_RED;
                gauge.filled_color.green = FILE_GAUGE_FILLED_GREEN;
                gauge.filled_color.blue = FILE_GAUGE_FILLED_BLUE;
                gauge.filled_color.alpha = FILE_GAUGE_ALPHA;
                gauge.unfilled_color.red = FILE_GAUGE_UNFILLED;
                gauge.unfilled_color.green = FILE_GAUGE_UNFILLED;
                gauge.unfilled_color.blue = FILE_GAUGE_UNFILLED;
                gauge.unfilled_color.alpha = FILE_GAUGE_ALPHA;
                xglMcRequest(((FileCardNumber *)&FileWork->card_number)->number, MC_REQUEST_CREATE,
                             &card_request,
                             &FileWork->directory_result);
                tyaDrawGauge(&gauge);
                break;
            }
        }
        if (FileWork->use_hdd) {
            if (FileWork->menu_substate == 0) {
                hdd_request.create.status = FileWork->selected_slot;
                hdd_request.create.transfer.bytes = FileCardDataBuf;
                hdd_request.create.dataSize = MC_SAVE_DATA_SIZE;
                FileSaveDataPush();
                create_result = xglHddMcCreate(&hdd_request.create);
                FileWork->directory_result = create_result;
                switch (create_result) {
                case 0:
                    FileWork->command = FILE_STATE_SAVE_WRITE;
                    break;
                case HDD_CREATE_TOO_MANY_SAVES:
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                    FileWork->command = FILE_STATE_ERROR_HDD_TOO_MANY_SAVES;
                    FileWork->dispatch_delay = FILE_DELAY_LONG;
                    break;
                case HDD_CREATE_TOO_MANY_FOLDERS:
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                    FileWork->command = FILE_STATE_ERROR_HDD_TOO_MANY_FOLDERS;
                    FileWork->dispatch_delay = FILE_DELAY_LONG;
                    break;
                case HDD_CREATE_NO_SPACE:
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                    FileWork->command = FILE_STATE_ERROR_HDD_NO_SPACE;
                    FileWork->dispatch_delay = FILE_DELAY_LONG;
                    break;
                default:
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                    FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                    FileWork->dispatch_delay = FILE_DELAY_LONG;
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
                FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            case MC_STATUS_BUSY:
                gauge.current = FileWork->directory_result;
                tyaDrawGauge(&gauge);
                break;
            case MC_STATUS_READY:
                if (FileWork->directory_result < 0) {
                    FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                } else {
                    FileWork->command = FILE_STATE_SAVE_WRITE;
                }
                gauge.current = FILE_CREATE_STEPS;
                tyaDrawGauge(&gauge);
                break;
            }
        }
        break;
    }
    case FILE_STATE_SAVE_DONE: {
        if (FileWork->use_hdd) {
            if (PadData.half_2a & PAD_CIRCLE) {
                FileWork->command = FILE_STATE_SLOT_LIST;
                xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                FileWork->menu_substate = FILE_WAIT_FRAMES;
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case MC_STATUS_READY:
                if (PadData.half_2a & PAD_CIRCLE) {
                    FileWork->command = FILE_STATE_SLOT_LIST;
                    xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                    FileWork->menu_substate = FILE_WAIT_FRAMES;
                }
                break;
            case MC_STATUS_BUSY:
                break;
            case MC_STATUS_NONE:
            default:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            }
        }
        break;
    }
    case FILE_STATE_LOAD_READ:
    {
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            if (FileWork->use_hdd) {
                FileWork->dispatch_delay = FILE_DELAY_LONG;
            } else {
                card_request.slot = FileWork->selected_slot;
                card_request.buffer = (int)FileSaveData;
                card_request.size = MC_SAVE_DATA_SIZE;
                memset(FileSaveData, 0, MC_SAVE_DATA_SIZE);
                xglMcRequest(FileWork->card_number, MC_REQUEST_LOAD, &card_request,
                             &FileWork->directory_result);
            }
        } else {
            if (FileWork->use_hdd) {
                hdd_request.load.card = FileWork->selected_slot;
                hdd_request.load.data = FileSaveData;
                hdd_request.load.size = MC_SAVE_DATA_SIZE;
                memset(FileSaveData, 0, MC_SAVE_DATA_SIZE);
                FileWork->directory_result = xglHddMcLoad(&hdd_request.load);
                if (FileWork->directory_result != 0) {
                    FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                    xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                    break;
                }
                *(FileSaveRecord *) SaveData = *FileSaveData;
                FileVersionCheck((SaveDataHeader *) SaveData);
                xglSoundEffectNormalID(FILE_SE_LOAD_DONE, 0);
                FileWork->command = FILE_STATE_LOAD_DONE;
            } else {
                card = &FileWork->card;
                card_status = card->status[FileWork->card_number];
                if (card_status != MC_STATUS_READY) {
                    if (card_status != MC_STATUS_BUSY) {
                        FileWork->command = FILE_STATE_ERROR_REQUEST_FAILED;
                        xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                    }
                    break;
                }
                *(FileSaveRecord *) SaveData = *FileSaveData;
                FileVersionCheck((SaveDataHeader *) SaveData);
                xglSoundEffectNormalID(FILE_SE_LOAD_DONE, 0);
                FileWork->command = FILE_STATE_LOAD_DONE;
            }
        }
        break;
    }
    case FILE_STATE_LOAD_DONE: {
        if (FileWork->use_hdd) {
            if (PadData.half_2a & PAD_CIRCLE) {
                FileWork->command = FILE_STATE_CLOSING;
                xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case MC_STATUS_READY:
                if (PadData.half_2a & PAD_CIRCLE) {
                    FileWork->command = FILE_STATE_CLOSING;
                    xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                }
                break;
            case MC_STATUS_BUSY:
                break;
            case MC_STATUS_NONE:
            default:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            }
        }
        break;
    }
    case FILE_STATE_CARD_UNFORMATTED: {
        if (FileWork->use_hdd) {
            if (PadData.half_2a & PAD_CIRCLE) {
                FileWork->command = FILE_STATE_CANCEL_CONFIRM;
                xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
            }
        } else {
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case MC_STATUS_UNFORMATTED:
                if (PadData.half_2a & PAD_CIRCLE) {
                    xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                    FileWork->command = FILE_STATE_FORMAT_CONFIRM;
                }
                break;
            case MC_STATUS_NONE:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            case MC_STATUS_BUSY:
                break;
            default:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            }
        }
        break;
    }
    case FILE_STATE_FORMAT_CONFIRM: {
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            FileWork->action_selection = FILE_CHOICE_NO;
        }
        card = &FileWork->card;
        card_status = card->status[FileWork->card_number];
        switch (card_status) {
        case MC_STATUS_UNFORMATTED:
            FileWork->action_selection =
                MenuSelectMove(FileWork->action_selection, FILE_CONFIRM_ROWS, 0);
            if (PadData.half_2a & PAD_CIRCLE) {
                if (FileWork->action_selection == FILE_CHOICE_YES) {
                    xglMcRequest(FileWork->card_number, MC_REQUEST_FORMAT, 0,
                                 &FileWork->available_blocks);
                    FileWork->command = FILE_STATE_FORMAT;
                } else {
                    FileWork->command = FILE_STATE_CANCEL_CONFIRM;
                }
                xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
            }
            if (PadData.half_2a & PAD_CROSS) {
                FileWork->command = FILE_STATE_SELECT_DEVICE;
                xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
            }
            break;
        case MC_STATUS_NONE:
            FileWork->command = FILE_STATE_ERROR_NO_CARD;
            xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
            break;
        case MC_STATUS_BUSY:
            break;
        default:
            FileWork->command = FILE_STATE_ERROR_NO_CARD;
            xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
            break;
        }
        break;
    }
    case FILE_STATE_FORMAT: {
        card = &FileWork->card;
        card_status = card->status[FileWork->card_number];
        switch (card_status) {
        case MC_STATUS_READY:
            FileWork->command = FILE_STATE_FORMAT_DONE;
            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
            break;
        case MC_STATUS_UNFORMATTED:
        case MC_STATUS_BUSY:
            break;
        default:
            FileWork->command = FILE_STATE_FORMAT_FAILED;
            xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
            break;
        }
        break;
    }
    case FILE_STATE_FORMAT_DONE: {
        if (FileWork->dispatch_delay == 0) {
            if (PadData.half_2a & PAD_CIRCLE) {
                FileWork->command = FILE_STATE_CHECK_DEVICE;
                FileWork->hide_selection = 1;
            }
            card = &FileWork->card;
            card_status = card->status[FileWork->card_number];
            switch (card_status) {
            case MC_STATUS_NONE:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            case MC_STATUS_READY:
            case MC_STATUS_BUSY:
                break;
            default:
                FileWork->command = FILE_STATE_ERROR_NO_CARD;
                xglSoundEffectNormalID(FILE_SE_BUZZER, 0);
                break;
            }
        }
        break;
    }
    case FILE_STATE_FORMAT_FAILED:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            FileWork->command = FILE_STATE_CANCEL_CONFIRM;
            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
        }
        break;
    case FILE_STATE_ERROR_REQUEST_FAILED:
    case FILE_STATE_ERROR_NO_CARD:
    case FILE_STATE_ERROR_CANNOT_ACCESS:
    case 0x93:
    case FILE_STATE_ERROR_HDD_TOO_MANY_SAVES:
    case FILE_STATE_ERROR_HDD_TOO_MANY_FOLDERS:
    case FILE_STATE_ERROR_HDD_NO_SPACE:
    case 0x97:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            FileWork->command = FILE_STATE_CANCEL_CONFIRM;
            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
        }
        break;
    case FILE_STATE_CANCEL_CONFIRM:
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            FileWork->action_selection = FILE_CHOICE_NO;
            FileWork->dispatch_delay = 8;
            xglHddCheck();
        }
        if (FileWork->dispatch_delay == 0) {
            FileWork->action_selection =
                MenuSelectMove(FileWork->action_selection, FILE_CONFIRM_ROWS, 0);
            if (PadData.half_2a & PAD_CIRCLE) {
                if (FileWork->action_selection == FILE_CHOICE_YES) {
                    FileWork->result = 1;
                    FileWork->command = FILE_STATE_CLOSING;
                    xglSoundEffectNormalID(FILE_SE_CANCEL, 0);
                } else {
                    FileWork->command = FILE_STATE_SELECT_DEVICE;
                    xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
                }
            }
        }
        break;
    case FILE_STATE_INSERT_CARD:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            FileWork->command = FILE_STATE_SELECT_DEVICE;
            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
        }
        break;
    case FILE_STATE_NO_CARD_SPACE:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            FileWork->command = FILE_STATE_CANCEL_CONFIRM;
            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
        }
        break;
    case FILE_STATE_NO_SAVE_DATA:
        if (FileWork->dispatch_delay != 0) {
            break;
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            FileWork->command = FILE_STATE_CANCEL_CONFIRM;
            xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
        }
        break;
    case FILE_STATE_CLOSING:
        if (FileWork->input_flags & FILE_INPUT_ENTER) {
            FileWork->menu_substate = FILE_WAIT_FRAMES;
        }
        if (FileWork->menu_substate == 0) {
            FileWork->command = FILE_STATE_DONE;
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
                       FileWork->panel_count);
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
    FileBackTexturePacket *draw = (FileBackTexturePacket *)SCRATCHPAD_BASE;

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
    void *task_work;
    int buffer_end;
    XglTaskPrefix *entry;
    FileObjectEntryView *images;

    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.copy_width << 5,
                           sRender.copy_height << 5);
    GameLoopState.runtime_flags |= FILE_RUNTIME_FLAG_MENU_OPEN;
    xglSleep();
    endPrintInit();
    GameLoopState.runtime_flags |= FILE_RUNTIME_FLAG_MENU_OPEN;
    MenuCfTaikiPush();
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.copy_height << 5,
                           sRender.draw_back_value << 5);
    endBackTexDraw((unsigned char *)-1);
    xglSleep();

    FileWork = (FileWorkBlock *)(((unsigned int)MenuWorkEndGet() + 0x7f) & ~0x7f);
    task_work = FileWork + 1;
    memset(FileWork, 0, sizeof(*FileWork));
    task_work = MenuBgTaskInit(task_work, 1);
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
    TskObjectSet((TskObject *)entry, tskFileEx, task_work);
    task_work += 0x50;
    entry = xglTaskEntryNext(FileTask, (int (*)(XglTaskPrefix *))tskTskMain,
                             FileTask ? FileTask->active_tail : 0);
    buffer_end = FILE_STAGING_BASE;
    TskObjectSet((TskObject *)entry, tskFileData, task_work);

    FileCardDataBuf = (unsigned char *)buffer_end;
    buffer_end += FILE_CARD_DATA_BUF_SIZE;
    FileObjectDataSystem = (FileObjectSystem *)buffer_end;
    memset(FileObjectDataSystem, 0, FILE_SYSTEM_BLOCK_SIZE);
    task_work += 0x249c;
    {
        int card;

        for (card = 0; card < 99; ++card)
            FileObjectDataSystem->card_status[card] = -1;
    }

    FileJpegDec = (FileJpegDecodeSlotView *)(buffer_end + FILE_STAGING_ALIGN);
    memset(FileJpegDec, 0, FILE_JPEG_AREA_SIZE);
    FileObjectData = (struct FileObjectEntryView *)((buffer_end + FILE_STAGING_ALIGN + FILE_JPEG_AREA_SIZE + FILE_STAGING_ALIGN - 1) & ~(FILE_STAGING_ALIGN - 1));
    memset(FileObjectData, 0, FILE_OBJECT_AREA_SIZE);
    buffer_end = ((buffer_end + FILE_STAGING_ALIGN + FILE_JPEG_AREA_SIZE + FILE_STAGING_ALIGN - 1) & ~(FILE_STAGING_ALIGN - 1)) + FILE_OBJECT_AREA_SIZE;
    images = FileObjectData;
    {
        int i;

        for (i = 0; i <= 2; ++i) {
            FileObjectEntryView *leader_images;
            FileObjectEntryView *next_images;

            images[i + 99].chapter = -1;
            leader_images = FileObjectData;
            leader_images[i + 99].leader = -1;
            next_images = FileObjectData;
            next_images[i + 99].play_time = -1;
            images = next_images;
        }
    }
    FileSaveData = (FileSaveRecord *)((buffer_end + FILE_STAGING_ALIGN - 1) & ~(FILE_STAGING_ALIGN - 1));
    FileExistCheckWork = (void *)(((unsigned int)FileSaveData + 0x16baf) & ~0x7ff);
    memset(FileExistCheckWork, 0, FILE_EXIST_CHECK_SIZE);
    if (menu_mode == 0) {
        unsigned char *thumbnail = FileObjectData[102].thumbnail;

        memset(thumbnail, 0, sizeof(FileObjectData[102].thumbnail));
        if (GameSnapShotSaveThumbnail(FILE_THUMBNAIL_QUALITY, FileObjectData[102].thumbnail) > FILE_THUMBNAIL_SIZE)
            exit(1);
    }
    MenuLoadInit();
    MenuFontLoad(1, 1);
    WindowTexLoad(0, 0x10001);
    WindowTexLoad(0, 0x10002);
    task_work = MenuSaveMapTextLoad(task_work, 1);
    if (menu_mode == 0)
        MenuLoadFile(cardDataNewPath, FileCardDataBuf);
    MenuLoadFile(newDataThumbnailPath, FileObjectData[99].thumbnail);
    MenuLoadFile(unusedDataThumbnailPath, FileObjectData[100].thumbnail);
    MenuLoadFile(brokenDataThumbnailPath, FileObjectData[101].thumbnail);
    MenuWorkEndCheck(task_work);
    if (menu_mode == 0)
        xglSoundEffectNormalID(FILE_SE_DECIDE, 0);
    while (MenuLoadSync() != 0) {
        MenuBgTaskMain();
        xglSleep();
    }
    FileWork->text_index = menu_mode;
    FileWork->command = FILE_STATE_SELECT_DEVICE;
    FileWork->input_flags = FILE_INPUT_SKIP;
    FileWork->result = 0;
    for (;; endPrintExtFunc(0, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0), MenuBgTaskMain(),
            xglTaskExecute(FileTask), xglSleep()) {
        GameSnapShotCheck();
        if (FileWork->state != FILE_STATE_DONE) {
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
    GameLoopState.runtime_flags &= ~FILE_RUNTIME_FLAG_MENU_OPEN;
    return FileWork->result == 0;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
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

const struct FileConfirmationTexts fileConfirmationTexts = { { newDataSaveText, overwriteSaveDataText, formattingCardText, saveCompleteText, savingToCardText, loadingFromCardText, powerOffWarningText, savingToCardText, loadingFromCardText, cardRemoveWarningText, savingNewDataText, cardRemoveWarningText } };

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

const struct FileSlotStateTexts fileSlotStateTexts = { { { saveFailedText, saveDataCorruptedText, saveStateBlankText }, { loadFailedText, loadFailedCorruptedText, loadFailedNotFoundText } } };

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

const struct FileProgressTexts fileProgressTexts = { { maxSaveFilesExceededText, maxFoldersExceededText, notEnoughSpaceText, maxSaveFilesHddText } };

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

struct FileListTexts fileListTexts[1] = { { { selectSaveLocationText, selectLoadDataText } } };

struct FileFinishTexts fileFinishTexts[1] = { { { saveDataNotExistText, saveDataNotExistCardText } } };

/* Text: . \x1F */
char periodEndText[8] = ". \037";

int FileWorkEndTop = 0;
