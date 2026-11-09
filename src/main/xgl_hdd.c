#include "common.h"
#include "shared.h"
#include "main/xgl_hdd.h"
#include "main/xgl_mc.h"
#include "xgl_hdd.h"

/* sceOpen flags and permission modes (0666, 0555, 0777). */
#define HDD_OPEN_READ 1
#define HDD_OPEN_READ_WRITE 3
#define HDD_OPEN_READ_WRITE_CREATE 0x203
#define HDD_OPEN_WRITE_CREATE_TRUNC 0x602
#define HDD_MODE_RW 438
#define HDD_MODE_RW_FILE 0x1b6
#define HDD_MODE_READ_ONLY 0x16d
#define HDD_MODE_DIRECTORY 0x1ff

/* sceMount flags, and the errors the layer tests for. */
#define HDD_MOUNT_READ_WRITE 4
#define HDD_MOUNT_READ_ONLY 5
#define HDD_ERR_NOENT (-2)
#define HDD_ERR_BUSY (-16)
#define HDD_ERR_NODEV (-19)

/* Results of xglHddCheckCore() beyond 0 (ready). */
#define HDD_NOT_READY (-5)
#define HDD_RESET_REQUIRED (-6)

#define HDD_PATH_MAX 256
#define HDD_ERROR_WAIT_FRAMES 30
#define HDD_ERROR_SE_DECIDE 1
#define HDD_DIR_SCAN_LIMIT 1024
#define HDD_MAX_FOLDERS 256
#define HDD_FOLDER_ENTRY_LIMIT 0x3FE
#define HDD_SLACK_ZONES 7
/* Total free sectors above which the save area can be grown, and the free
 * sectors (total, and the largest free block) the game installation needs. */
#define HDD_LARGE_DISK_SECTORS 0x1fffff
#define HDD_INSTALL_MIN_SECTORS 0x37ffff
#define HDD_INSTALL_MIN_FREE_SECTORS 0x7ffff
#define HDD_LARGE_DISK_FREE_KB 0x100000

#define HDD_SYSTEM_CNF_SIZE 80
#define HDD_SECTOR_SIZE 512
#define HDD_INSTALL_IMAGE_ADDRESS 0x1000000
#define HDD_INSTALL_ARCHIVE_SIZE 0x1000000
#define HDD_INSTALL_PARTITION_STEP 0x80000

/* Failure codes of xglHddMcCreate(), one per step. The HDD_CREATE_* causes in
 * main/xgl_hdd.h are the reasons a save cannot be created. */
#define HDD_CREATE_ERR_MKDIR_ROOT (-10)
#define HDD_CREATE_ERR_CLOSE_ROOT (-0x0c)
#define HDD_CREATE_ERR_OPEN_DEVICE (-0x10)
#define HDD_CREATE_ERR_ADD_PARTITION (-0x11)
#define HDD_CREATE_ERR_CLOSE_DEVICE (-0x12)
#define HDD_CREATE_ERR_OPEN_SAVE_DIR (-0x13)
#define HDD_CREATE_ERR_MKDIR_SAVE (-0x14)
#define HDD_CREATE_ERR_SAVE_DIR_CLOSE (-0x15)
#define HDD_CREATE_ERR_WRITE_ICON (-0x16)
#define HDD_CREATE_ERR_WRITE_PAYLOAD (-0x17)
#define HDD_CREATE_ERR_CHSTAT_SAVE (-0x18)
#define HDD_CREATE_ERR_UNMOUNT (-0x19)
#define HDD_SAVE_ERR_WRITE (-24)
/* xglHddMcLoadCore() */
#define HDD_LOAD_ERR_OPEN (-6)
#define HDD_LOAD_ERR_READ (-7)
#define HDD_LOAD_ERR_CLOSE (-8)

/* xglHddInstall() reports its progress to the callback as these stages (the
 * second argument counts within a stage) and each failure as a negative code. */
#define HDD_INSTALL_STAGE_BEGIN 0
#define HDD_INSTALL_STAGE_RESOURCE_READ 1
#define HDD_INSTALL_STAGE_PARTITION_OPEN 2
#define HDD_INSTALL_STAGE_EXTEND 3
#define HDD_INSTALL_STAGE_FORMAT 4
#define HDD_INSTALL_STAGE_MOUNT 5
#define HDD_INSTALL_STAGE_COPY 6
#define HDD_INSTALL_STAGE_FINISH 7
#define HDD_INSTALL_STAGE_DONE 8
#define HDD_INSTALL_FAIL_RESOURCE (-1)
#define HDD_INSTALL_FAIL_PARTITION_OPEN (-2)
#define HDD_INSTALL_FAIL_WRITE_HEADER (-3)
#define HDD_INSTALL_FAIL_EXTEND (-4)
#define HDD_INSTALL_FAIL_PARTITION_CLOSE (-5)
#define HDD_INSTALL_FAIL_FORMAT (-6)
#define HDD_INSTALL_FAIL_MOUNT (-7)
#define HDD_INSTALL_FAIL_DATA_OPEN (-8)
#define HDD_INSTALL_FAIL_DATA_WRITE (-9)
#define HDD_INSTALL_FAIL_DATA_CLOSE (-10)
#define HDD_INSTALL_FAIL_CHECK_OPEN (-11)
#define HDD_INSTALL_FAIL_CHECK_WRITE (-12)
#define HDD_INSTALL_FAIL_CHECK_CLOSE (-13)
#define HDD_INSTALL_FAIL_UNMOUNT (-14)
#define HDD_INSTALL_FAIL_REMOUNT (-15)

/* The saved image begins with the transfer header. icon.sys occupies +0x40
 * through +0x403; the game payload starts at +0x440. The gaps remain unknown. */
struct HddSaveImage {
    struct HddTransferInfo header;
    unsigned char unmodeled_0c[0x40 - 0x0c];
    char iconSystem[MC_ICON_SYS_SIZE];
    unsigned char unmodeled_404[0x440 - 0x404];
    unsigned char payloadStart; /* First byte; the transfer header gives size. */
};

/* The install image (data\hdd.res) staged at the fixed work address: the
 * "PS2ICON3D" header with four (offset, size) pairs from +0x10, and the
 * 80-byte SYSTEM.CNF copy at +0x200. The last pair, at +0x28, ends the file. */
struct HddInstallImage {
    unsigned char unmodeled_00[0x28];
    int lastEntryOffset;
    int lastEntrySize;
    unsigned char unmodeled_30[0x1d0];
    unsigned char systemCnf[HDD_SYSTEM_CNF_SIZE];
};

/* decrypt(), called by xglHddMount, clears the high bit of this stored volume identifier. */
static char partitionname[];

static char hddname[];
static char hddcheck[];
static char commonname[];
static char yoursaves[];
extern HddInstallCBParam *HddInstallCBparam;
extern char pfs1RootPath[];
extern char currentDirEntryName[];
extern char parentDirEntryName[];
static unsigned char tbl_1_004A8AE0[] ;

extern int strcmp(const char *left, const char *right);

/* EUC-JP warning text: the HDD application data cannot be read, so loading
 * continues from the DVD. The embedded font controls and line breaks are kept
 * as they are in the data. */
/* JP: ハードディスクドライブにあるゼノサーガの\n\nアプリケーションデータが読み込めません。\n\nこれ以降はＤＶＤから読み込みを行います。\n\n\n\n○ボタン：続行
 * EN: Cannot load the Xenosaga\n\napplication data on the hard disk drive.\n\nData will be loaded from the DVD from now on.\n\n\n\nCircle button: Continue */
const char hddLoadErrorText[] =
    "\x0b\x0e\x01\x01\x00\x00\x00\x0d\x03\xa5\xcf\xa1\xbc\xa5\xc9\xa5\xc7\xa5\xa3\xa5"
    "\xb9\xa5\xaf\xa5\xc9\xa5\xe9\xa5\xa4\xa5\xd6\xa4\xcb\xa4\xa2\xa4\xeb\xa5\xbc\xa5"
    "\xce\xa5\xb5\xa1\xbc\xa5\xac\xa4\xce\x0a\x0a\xa5\xa2\xa5\xd7\xa5\xea\xa5\xb1\xa1"
    "\xbc\xa5\xb7\xa5\xe7\xa5\xf3\xa5\xc7\xa1\xbc\xa5\xbf\xa4\xac\xc6\xc9\xa4\xdf\xb9"
    "\xfe\xa4\xe1\xa4\xde\xa4\xbb\xa4\xf3\xa1\xa3\x0a\x0a\xa4\xb3\xa4\xec\xb0\xca\xb9"
    "\xdf\xa4\xcf\xa3\xc4\xa3\xd6\xa3\xc4\xa4\xab\xa4\xe9\xc6\xc9\xa4\xdf\xb9\xfe\xa4"
    "\xdf\xa4\xf2\xb9\xd4\xa4\xa4\xa4\xde\xa4\xb9\xa1\xa3\x0a\x0a\x0a\x0a\x0c\x80\x20"
    "\x20\xa1\xfb\x0c\x80\x80\x80\xa5\xdc\xa5\xbf\xa5\xf3\xa1\xa7\xc2\xb3\xb9\xd4";

static HddErrorPacket TestEnv_0_004A8A80[1];

static char dvdname_2[12];

static char partitionname[] = {
    'h' ^ 0x80, 'd' ^ 0x80, 'd' ^ 0x80, '0' ^ 0x80, ':' ^ 0x80,
    'P' ^ 0x80, 'P' ^ 0x80, '.' ^ 0x80, 'S' ^ 0x80, 'L' ^ 0x80,
    'P' ^ 0x80, 'S' ^ 0x80, '-' ^ 0x80, '2' ^ 0x80, '9' ^ 0x80,
    '0' ^ 0x80, '0' ^ 0x80, '2' ^ 0x80, '.' ^ 0x80, 'X' ^ 0x80,
    'E' ^ 0x80, 'N' ^ 0x80, 'O' ^ 0x80, 'S' ^ 0x80, 'A' ^ 0x80,
    'G' ^ 0x80, 'A' ^ 0x80, '1' ^ 0x80, '.' ^ 0x80, 'D' ^ 0x80,
    'I' ^ 0x80, 'S' ^ 0x80, 'C' ^ 0x80, '1' ^ 0x80, ',' ^ 0x80,
    'x' ^ 0x80, 'e' ^ 0x80, 'n' ^ 0x80, 'o' ^ 0x80, '1' ^ 0x80,
    'd' ^ 0x80, '1' ^ 0x80, ',' ^ 0x80, 'x' ^ 0x80, 'e' ^ 0x80,
    'n' ^ 0x80, 'o' ^ 0x80, '1' ^ 0x80, 'd' ^ 0x80, '1' ^ 0x80,
    0
};

static char hddname[] = "pfs0:/xenosaga.00";

static char hddcheck[] = "pfs0:/xenosaga.hdd";

static char commonname[] = "hdd0:__common";

static char yoursaves[] = "pfs1:/Your Saves";

/* VIF DIRECT hands five quadwords to GIF. The GIF tag selects one A+D
 * register write, one RGBAQ value and two XYZ2 corner records. */
static HddErrorPacket TestEnv_0_004A8A80[1] = {{
    0, 0, 0, 0x50000005,
    {0x00008001, 0x40034000, 0x551e},
    {0x30000, 0x47},
    {0, 0x40, 0, 0x80},
    {
        {0x6ff8, 0x71f7, 0x00fffff0},
        {0x8ff8, 0x8df7, 0x00fffff0},
    },
}};

static unsigned char tbl_1_004A8AE0[] = { 14, 9, 13, 15 };

static char dvdname_2[12] = "xenosaga.00";

char xgl_hdd_device[] = "hdd0:";

char hdd_mc_path[] = "pfs1:";

char pfs1RootPath[8] = "pfs1:/";

char currentDirEntryName[8] = ".";

char parentDirEntryName[8] = "..";

static u8 HddActive;

static int xglHddDummyCB(int event, int value)
{
    return 0;
}

/*
 * Polls the HDD device status and flushes it. Returns 0 when ready, HDD_RESET_REQUIRED
 * or HDD_NOT_READY, or another negative status.
 */
static int xglHddCheckCore(void)
{
    long status;
    int expectedStatus;
    int result;

    if (HddActive >= 2)
        return HDD_NOT_READY;

    status = sceDevctl(xgl_hdd_device, HDD_DEVCTL_STATUS, 0, 0, 0, 0);
    if (status != 0) {
        if (status == 2)
            return -2;
        if (status < 3) {
            expectedStatus = 1;
            result = -3;
        } else {
            expectedStatus = 3;
            result = -1;
        }
        if (status != expectedStatus)
            return -99;
    } else {
        if (sceDevctl(xgl_hdd_device, HDD_DEVCTL_FLUSH, 0, 0, 0, 0) >= 0)
            return 0;
        if (HddActive == 1)
            return HDD_RESET_REQUIRED;
        xglHddActivate(0x102);
        return HDD_NOT_READY;
    }
    return result;
}

/* Runs xglHddCheckCore. A required reset starts activation 0x103 and reports HDD_NOT_READY. */
int xglHddCheck2(void)
{
    int result = xglHddCheckCore();

    if (result == HDD_RESET_REQUIRED) {
        xglHddActivate(0x103);
        return HDD_NOT_READY;
    }
    return result;
}

/*
 * Checks the HDD. A state of 3 shows the error screen first, and a required reset
 * shows it and reactivates the HDD.
 */
int xglHddCheck(void)
{
    int result;

    if (HddActive == 3) {
        xglHddErrorScreen();
        xglHddActivate(0);
        xglHddActivate(0x102);
    }

    result = xglHddCheckCore();
    if (result != HDD_RESET_REQUIRED)
        return result;

    xglHddErrorScreen();
    xglHddActivate(0);
    xglHddActivate(0x102);
    return HDD_NOT_READY;
}

/* Shows the HDD load error text, and returns once Circle is pressed after the wait frames. */
void xglHddErrorScreen(void)
{
    int waitFrames = HDD_ERROR_WAIT_FRAMES;

    sRender.errorScreenActive = 1;
    for (;;) {
        xglDmaDirectNormal(2, (u32)TestEnv_0_004A8A80, 6);
        xglFontPrint(48, 64, 0xffffff, hddLoadErrorText);
        if (waitFrames > 0) {
            waitFrames--;
        } else if (PadData.half_2a & PAD_CIRCLE) {
            break;
        }
        xglSleep();
    }
    xglSoundEffectNormalDirect(HDD_ERROR_SE_DECIDE);
    sRender.errorScreenActive = 0;
}

/*
 * Checks the HDD and mounts the common area read-only. Returns 0 on success (a busy
 * mount counts), or -1 on failure.
 */
int xglHddMcLoadMount(void)
{
    int result;

    if (xglHddCheck2() != 0)
        return -1;

    result = sceMount(hdd_mc_path, commonname, HDD_MOUNT_READ_ONLY, 0, 0);
    if (result < 0 && result != HDD_ERR_BUSY)
        return -1;
    return 0;
}

int xglHddMcUmount(void)
{
    int result = sceUmount(hdd_mc_path);
    return result < 0 ? -2 : 0;
}

/* Writes the save-area path and the save path for card and slot to destination. Returns the end of the string. */
static char *make_fullpath(char *destination, int card, int slot)
{
    const char *source = yoursaves;

    while ((*destination = *source) != 0) {
        source++;
        destination++;
    }

    source = xglMcSetFullPath(card, slot);
    while ((*destination = *source) != 0) {
        source++;
        destination++;
    }
    return destination;
}

/*
 * Checks for saves. With card >= 0, returns 1 when that slot's file opens and 0 when not.
 * With card < 0, marks each matching save in found[], stores the newest slot in found[100]
 * and returns the number of matches.
 */
int xglHddMcExist(struct HddExistRequest *request)
{
    char filePath[HDD_PATH_MAX];
    char name[HDD_PATH_MAX];
    struct HddDirectoryEntry entry;
    char savePath[HDD_PATH_MAX];
    XglClock timestamp;
    int result;
    int descriptor;
    int save;
    int count;
    int newestSave;
    unsigned int newestTime;
    unsigned int time;
    unsigned char *found;
    char *source;
    char *destination;
    unsigned char *clearPosition;

    result = xglHddMcLoadMount();
    if (result < 0)
        return result;

    if (request->card >= 0) {
        make_fullpath(filePath, request->card, 0);
        descriptor = sceOpen(filePath, HDD_OPEN_READ, HDD_MODE_RW);
        if (descriptor >= 0) {
            result = sceClose(descriptor) < 0 ? -3 : 1;
        } else {
            result = 0;
        }
    } else {
        found = request->found;
        source = xglMcSetFullPath(-1, -1);
        source++;
        newestTime = 0;
        newestSave = 0;
        destination = name;
        *destination = *source;
        if (((unsigned int)*destination << 24) != 0) {
            do {
                source++;
                destination++;
                *destination = *source;
            } while (((unsigned int)*destination << 24) != 0);
        }
        /* The -1 card path ends in a wildcard; compare its directory stem. */
        destination[-1] = 0;

        /* Clear found[0] to found[100]. */
        count = 100;
        clearPosition = &found[100];
        do {
            count--;
            *clearPosition = 0;
            clearPosition--;
        } while (count >= 0);

        /* Scan the save area for entries whose name matches the stem. */
        count = 0;
        descriptor = sceDopen(yoursaves);
        while (descriptor >= 0 && sceDread(descriptor, &entry) > 0) {
            if (strncmp(name, entry.name, 16) != 0)
                continue;
            save = entry.name[16] * 10 + entry.name[17] - (10 * '0' + '0');
            make_fullpath(savePath, save, 0);
            if (sceGetstat(savePath, &entry.stat) < 0)
                continue;
            found[save] = 1;
            timestamp.status = 0;
            timestamp.year = entry.stat.year;
            timestamp.month = entry.stat.month;
            timestamp.day = entry.stat.day;
            timestamp.hour = entry.stat.hour;
            timestamp.minute = entry.stat.minute;
            timestamp.second = entry.stat.second;
            time = xglClockDayTime2UInt(&timestamp);
            if (newestTime < time) {
                newestTime = time;
                newestSave = save;
            }
            count++;
        }
        found[100] = newestSave;
        result = count;
        if (descriptor >= 0 && sceDclose(descriptor) < 0)
            result = -5;
    }
    xglHddMcUmount();
    return result;
}

/*
 * Reads request->size bytes of the save file for request->card into request->data.
 * Returns 0, 1 when the file does not exist, or an HDD_LOAD_ERR_* code.
 */
int xglHddMcLoadCore(void *save)
{
    struct HddLoadRequest *request = save;
    char path[HDD_PATH_MAX];
    int descriptor;
    int result;

    make_fullpath(path, request->card, 0);
    descriptor = sceOpen(path, HDD_OPEN_READ, HDD_MODE_RW);
    if (descriptor < 0) {
        result = descriptor == HDD_ERR_NOENT ? 1 : HDD_LOAD_ERR_OPEN;
    } else {
        result = 0;
        if (sceRead(descriptor, request->data, request->size) < 0)
            result = HDD_LOAD_ERR_READ;
        if (sceClose(descriptor) < 0)
            result = HDD_LOAD_ERR_CLOSE;
    }
    return result;
}

/* Mounts the common area, loads the save with xglHddMcLoadCore and unmounts. Returns the first failure. */
int xglHddMcLoad(void *save)
{
    int result;

    result = xglHddMcLoadMount();
    if (result < 0)
        return result;
    result = xglHddMcLoadCore(save);
    if (result < 0)
        return result;
    return xglHddMcUmount();
}

/* Counts the save folders in the root. Returns 1 while the count is below HDD_MAX_FOLDERS, or a negative error. */
static int Judge_MakeNewFolder(void)
{
    struct HddDirectoryEntry entry;
    int directoryCount;
    int entriesRead;
    int result;
    int descriptor;
    int closeResult;
    char *entryName;

    directoryCount = 0;
    result = sceDopen(pfs1RootPath);
    if (result < 0)
        return result;
    descriptor = result;

    for (entriesRead = 0; entriesRead < HDD_DIR_SCAN_LIMIT; entriesRead++) {
        result = sceDread(descriptor, &entry);
        if (result <= 0)
            break;

        entryName = entry.name;
        if (strcmp(entryName, currentDirEntryName) != 0 &&
            strcmp(entryName, parentDirEntryName) != 0 &&
            (entry.stat.mode & 0xF000) == 0x1000 &&
            entry.stat.privateData[0] == 0xFFFF &&
            entry.stat.privateData[1] == 0xFFFF) {
            directoryCount++;
            if (directoryCount >= HDD_MAX_FOLDERS)
                break;
        }
    }

    closeResult = sceDclose(descriptor);
    if (result >= 0)
        result = closeResult;

    if (result < 0)
        return result;
    return directoryCount < HDD_MAX_FOLDERS;
}

/* Counts the save files in the directory, skipping folders and other entries. Returns 1 while the count is below HDD_FOLDER_ENTRY_LIMIT. */
int Judge_MakeNewSavedata(int descriptor, int card)
{
    extern unsigned int strlen(const char *string);
    extern char *strchr(const char *string, int character);
    struct HddDirectoryEntry entry;
    int directoryCount;
    int entriesRead;
    int result;
    int closeResult;
    char *path;
    char *entryName;
    unsigned int pathLength;

    directoryCount = 0;
    result = 0;
    path = xglMcSetFullPath(card, -1) + 1;
    if (card < 0) {
        char *end = path;

        if (*path != '\0') {
            do {
                end++;
            } while (*end != '\0');
        }
        end[-1] = '\0';
    }
    pathLength = strlen(path);

    for (entriesRead = 0; entriesRead < HDD_DIR_SCAN_LIMIT; entriesRead++) {
        result = sceDread(descriptor, &entry);
        if (result <= 0)
            break;

        entryName = entry.name;
        if (strcmp(entryName, currentDirEntryName) == 0 ||
            strcmp(entryName, parentDirEntryName) == 0 ||
            strncmp(entryName, path, pathLength) == 0)
            continue;

        if ((entry.stat.mode & 0xF000) == 0x1000 &&
            (entry.stat.flags & 0x4000) == 0)
            continue;

        if (strchr(entryName, ':') != 0 &&
            (entry.stat.mode & 0xF000) == 0x2000)
            continue;

        if ((entry.stat.mode & 0xF000) == 0x4000)
            continue;
        if (entry.stat.privateData[0] != 0xFFFF)
            continue;
        if (entry.stat.privateData[1] == entry.stat.privateData[0])
            directoryCount++;
    }

    closeResult = sceDclose(descriptor);
    if (result >= 0)
        result = closeResult;

    if (result < 0)
        return result;
    return directoryCount < HDD_FOLDER_ENTRY_LIMIT;
}

/*
 * Returns 0 when the save area can take a save, 1 when its folder must be created,
 * or an HDD_CHECK_TOO_MANY_* code.
 */
static int xglHddMcCheckYourSaves(int card)
{
    int descriptor = sceDopen(yoursaves);

    if (descriptor < 0)
        return Judge_MakeNewFolder() == 1 ? 1 : HDD_CHECK_TOO_MANY_FOLDERS;
    return Judge_MakeNewSavedata(descriptor, card) == 1 ? 0 : HDD_CHECK_TOO_MANY_SAVES;
}

/*
 * Checks the save area, then whether the free zones cover the save data, the icon and the
 * transfer. Returns 0 when they fit, HDD_CHECK_NO_SPACE on a small disk or 1 on a large one
 * when they do not, or a negative error.
 */
static int xglHddMcCheckCore(struct HddCheckState *state)
{
    int result = xglHddMcCheckYourSaves(state->status);

    if (result < 0)
        return result;
    {
        int clusterSize = sceDevctl(hdd_mc_path, HDD_DEVCTL_ZONE_SIZE, 0, 0, 0, 0);
        int freeClusters = sceDevctl(hdd_mc_path, HDD_DEVCTL_FREE_ZONES, 0, 0, 0, 0);
        int dataClusters = (state->dataSize + clusterSize - 1) / clusterSize;
        int reserveClusters = ((unsigned int)clusterSize + MC_ICON_SYS_SIZE - 1) / (unsigned int)clusterSize;
        int transferClusters =
            ((unsigned int)(state->transfer.info->end - state->transfer.info->begin) + clusterSize - 1)
            / (unsigned int)clusterSize;

        dataClusters += reserveClusters;
        if (freeClusters < dataClusters + transferClusters + HDD_SLACK_ZONES) {
            int capacity;

            if (sceDevctl(xgl_hdd_device, HDD_DEVCTL_FREE_SECTORS_TOTAL, 0, 0, &capacity, 4) < 0)
                return -1;
            return capacity <= HDD_LARGE_DISK_SECTORS ? HDD_CHECK_NO_SPACE : 1;
        }
    }
    return 0;
}

/*
 * Checks the save area on a read-write mount of the common area, with state->status set
 * to -1 during the check. Returns the check result, or -1 when the HDD or mount is not ready.
 */
int xglHddMcCheck(struct HddCheckState *state)
{
    int previous;
    int result;

    if (xglHddCheck2() != 0)
        return -1;

    xglHddMcUmount();
    if (sceMount(hdd_mc_path, commonname, HDD_MOUNT_READ_WRITE, 0, 0) < 0)
        return -1;

    previous = state->status;
    state->status = -1;
    result = xglHddMcCheckCore(state);
    state->status = previous;
    xglHddMcUmount();
    return result;
}

/*
 * Returns the free save space in KB, or the fixed HDD_LARGE_DISK_FREE_KB on a large disk.
 * Returns a negative code when the HDD or the save area is not ready.
 */
int xglHddMcGetFree(void)
{
    int result;
    int freeSpace;
    int clusterSize;
    int freeClusters;
    int capacity;

    result = xglHddMcLoadMount();
    if (result >= 0) {
        freeSpace = xglHddMcCheckYourSaves(-1);
        if (freeSpace >= 0) {
            if (sceDevctl(xgl_hdd_device, HDD_DEVCTL_FREE_SECTORS_TOTAL, 0, 0, &capacity, 4) == 0
                && capacity > HDD_LARGE_DISK_SECTORS) {
                freeSpace = HDD_LARGE_DISK_FREE_KB;
            } else {
                clusterSize = sceDevctl(hdd_mc_path, HDD_DEVCTL_ZONE_SIZE, 0, 0, 0, 0);
                freeClusters = sceDevctl(hdd_mc_path, HDD_DEVCTL_FREE_ZONES, 0, 0, 0, 0);
                freeSpace = (freeClusters < 0 ? 0 : freeClusters) * (clusterSize / 1024);
            }
        }
        xglHddMcUmount();
        result = freeSpace;
    }
    return result;
}

/* Writes size bytes of data to the save file for card and slot, then sets its attributes. Returns 0 or -1. */
static int create_file(int card, int slot, const void *data, int size)
{
    char path[HDD_PATH_MAX];
    struct HddIoStat stat;
    int descriptor;
    int result;

    make_fullpath(path, card, slot);
    descriptor = sceOpen(path, HDD_OPEN_WRITE_CREATE_TRUNC, HDD_MODE_RW);
    if (descriptor < 0)
        return -1;

    result = 0;
    if (sceWrite(descriptor, data, size) < 0)
        result = -1;
    if (sceClose(descriptor) < 0)
        result = -1;
    stat.attributes = 0x8497;
    if (sceChstat(path, &stat, 2) < 0)
        result = -1;
    return result;
}

/*
 * Creates the save folder for state->status and writes its icon and payload files.
 * Returns 0 or a negative failure code.
 */
int xglHddMcCreate(struct HddCheckState *state)
{
    extern int sceIoctl2(int descriptor, int command, const void *input,
                         unsigned int input_size, void *output,
                         unsigned int output_size);
    extern int sceMkdir(const char *path, int mode);
    extern void xglMcWriteMapName(char *icon_system, int card);
    extern unsigned char mcPartitionSize1G[];
    extern unsigned char tbl_1_004A8AE0[];
    char *deviceName;
    char path[HDD_PATH_MAX];
    struct HddIoStat stat;
    int result;
    int descriptor;
    int remountFailure;
    struct HddSaveImage *transfer;
    int checkResult;

    result = -1;
    xglHddMcUmount();
    if (sceMount(hdd_mc_path, commonname, HDD_MOUNT_READ_WRITE, 0, 0) < 0)
        return -1;

    make_fullpath(path, state->status, -1);
    if (state->status >= 0 && (descriptor = sceDopen(path)) >= 0) {
        if (sceDclose(descriptor) < 0) {
            result = HDD_CREATE_ERR_SAVE_DIR_CLOSE;
            goto unmount_card;
        }
    } else {
        /* Check the card; a result of 1 adds the sub-partition and remounts before the folders are created. */
        checkResult = xglHddMcCheckCore(state);
        if (checkResult < 0) {
            result = -(int)tbl_1_004A8AE0[~checkResult];
            xglHddMcUmount();
            return result;
        }
        if (checkResult == 1) {
            if (xglHddMcUmount() < 0)
                return HDD_CREATE_ERR_UNMOUNT;
            remountFailure = -1;
            deviceName = commonname;
            descriptor = sceOpen(deviceName, HDD_OPEN_READ_WRITE);
            if (descriptor < 0)
                return HDD_CREATE_ERR_OPEN_DEVICE;

            result = 0;
            if (sceIoctl2(descriptor, HDD_IOCTL2_ADD_SUBPARTITION, mcPartitionSize1G, 3, 0, 0) < 0)
                result = HDD_CREATE_ERR_ADD_PARTITION;
            if (sceClose(descriptor) < 0)
                result = HDD_CREATE_ERR_CLOSE_DEVICE;
            if (result != 0)
                return result;
            if (sceMount(hdd_mc_path, commonname, HDD_MOUNT_READ_WRITE, 0, 0) < 0)
                return remountFailure;
            result = 0;
        }

        descriptor = sceDopen(yoursaves);
        if (descriptor < 0) {
            if (sceMkdir(yoursaves, HDD_MODE_DIRECTORY) < 0) {
                result = HDD_CREATE_ERR_MKDIR_ROOT;
                goto unmount_card;
            }
        } else if (sceDclose(descriptor) < 0) {
            result = HDD_CREATE_ERR_CLOSE_ROOT;
            goto unmount_card;
        }

        if (state->status < 0) {
            xglHddMcUmount();
            return result;
        }

        descriptor = sceDopen(path);
        if (descriptor < 0 && descriptor != HDD_ERR_NOENT) {
            result = HDD_CREATE_ERR_OPEN_SAVE_DIR;
            goto unmount_card;
        }

        if (descriptor == HDD_ERR_NOENT) {
            if (sceMkdir(path, HDD_MODE_DIRECTORY) < 0) {
                result = HDD_CREATE_ERR_MKDIR_SAVE;
                goto unmount_card;
            }
            stat.attributes = 0xc4a7;
            if (sceChstat(path, &stat, 2) < 0) {
                result = HDD_CREATE_ERR_CHSTAT_SAVE;
                goto unmount_card;
            }
        } else {
            result = HDD_CREATE_ERR_SAVE_DIR_CLOSE;
            if (sceDclose(descriptor) < 0)
                goto unmount_card;
        }
    }

    /* Write the icon file, then the payload when the icon write succeeds. */
    result = HDD_CREATE_ERR_WRITE_ICON;
    transfer = state->transfer.image;
    xglMcWriteMapName(transfer->iconSystem, state->status);
    if (create_file(state->status, 1, transfer->iconSystem, MC_ICON_SYS_SIZE) >= 0) {
        result = HDD_CREATE_ERR_WRITE_PAYLOAD;
        if (create_file(state->status, 2, &transfer->payloadStart,
                        transfer->header.end - transfer->header.begin) >= 0)
            result = 0;
    }

unmount_card:
    xglHddMcUmount();
    return result;
}

/* Writes save->data to slot 0 of save->card. Returns 0 or HDD_SAVE_ERR_WRITE. */
int xglHddMcSave(const struct HddSaveRequest *save)
{
    int result;

    if (xglHddCheck2() != 0)
        return -1;
    if (sceMount(hdd_mc_path, commonname, HDD_MOUNT_READ_WRITE, 0, 0) < 0)
        return -1;

    result = create_file(save->card, 0, save->data, save->size) < 0 ? HDD_SAVE_ERR_WRITE : 0;
    xglHddMcUmount();
    return result;
}

/* Deactivates the HDD, unmounts it and removes the partition named by partitionname. Returns 0 or a negative code. */
int xglHddUninstall(void)
{
    int result;

    xglHddActivate(0);
    result = sceUmount(mount_device);
    if (result < 0 && result != HDD_ERR_NODEV)
        return -1;
    result = sceRemove(partitionname);
    return result < 0 ? -5 : 0;
}

/*
 * Read callback for the install copy. Event 1 records the part count. Event 2 reports copy
 * progress to the install callback and cancels the read when that callback returns nonzero.
 */
static int xglHddInstallReadCB(int event, int value)
{
    HddInstallCBParam *cb;
    int result;
    int total;
    int scaledValue;
    int offset;

    cb = HddInstallCBparam;
    if (event == 1) {
        cb->total = value + 1;
        return 1;
    }

    result = 2;
    if (event == 2) {
        total = cb->total;
        scaledValue = value * 0x10;
        offset = cb->base * 0x10 + scaledValue / total;
        result = cb->callback(HDD_INSTALL_STAGE_COPY, offset, cb->param);
        cb->status = result;
        if (result != 0) {
            return xglCdReadCancel();
        }
    }
    return result;
}

/*
 * Installs the game data to the HDD and reports each stage and failure through callback.
 * Returns 0 on success and -1 on failure.
 */
int xglHddInstall(int (*callback)(), int param)
{
    extern int sceFormat(char *mount_point, char *device, void *payload,
                         int payload_length);
    extern int sceIoctl2(int descriptor, int command, const void *input,
                         unsigned int input_size, void *output,
                         unsigned int output_size);
    extern char hddResourcePath[];
    extern char partitionSpecFormat[];
    extern char hddPartitionSize256M[];
    extern char pfsDevicePath[];
    extern char dvdname_2[];
    struct HddInstallImage *image = (struct HddInstallImage *)HDD_INSTALL_IMAGE_ADDRESS;
    HddInstallCBParam context;
    char path[HDD_PATH_MAX];
    int formatSize;
    int partition;
    int descriptor;
    int result;
    int size;
    int chunk;
    u8 *source;
    int i;
    int archives;
    int offset;
    int length;

    if (callback == 0)
        callback = xglHddDummyCB;
    HddInstallCBparam = &context;
    context.callback = callback;
    context.param = param;
    context.status = 0;
    callback(HDD_INSTALL_STAGE_BEGIN, 0, param);

    /* Read the install image, then copy SYSTEM.CNF into it. */
    if (xglCdReadFile(hddResourcePath, image, 0, 1) < 0) {
        callback(HDD_INSTALL_FAIL_RESOURCE, 0, param);
        return -1;
    }
    source = system_cnf;
    for (i = 0; i < HDD_SYSTEM_CNF_SIZE; i++)
        image->systemCnf[i] = source[i];
    size = (image->lastEntryOffset + image->lastEntrySize + HDD_SECTOR_SIZE - 1) & -HDD_SECTOR_SIZE;

    callback(HDD_INSTALL_STAGE_RESOURCE_READ, 0, param);
    /* Create the partition and extend it in 256M steps. */
    sprintf(path, partitionSpecFormat, partitionname);
    partition = sceOpen(path, HDD_OPEN_READ_WRITE_CREATE);
    if (partition < 0) {
        callback(HDD_INSTALL_FAIL_PARTITION_OPEN, partition, param);
        return -1;
    }
    callback(HDD_INSTALL_STAGE_PARTITION_OPEN, 0, param);
    if ((result = sceWrite(partition, image, size)) < 0) {
        callback(HDD_INSTALL_FAIL_WRITE_HEADER, result, param);
        xglHddUninstall();
        return -1;
    }

    for (size = HDD_INSTALL_PARTITION_STEP, chunk = 1; size <= HDD_INSTALL_MIN_SECTORS; size += HDD_INSTALL_PARTITION_STEP) {
        callback(HDD_INSTALL_STAGE_EXTEND, chunk++, param);
        result = sceIoctl2(partition, HDD_IOCTL2_ADD_SUBPARTITION, hddPartitionSize256M, 5, 0, 0);
        if (result < 0) {
            callback(HDD_INSTALL_FAIL_EXTEND, result, param);
            xglHddUninstall();
            return -1;
        }
    }
    if ((result = sceClose(partition)) < 0) {
        callback(HDD_INSTALL_FAIL_PARTITION_CLOSE, result, param);
        return -1;
    }

    callback(HDD_INSTALL_STAGE_FORMAT, 0, param);
    /* Format and mount the new partition. */
    formatSize = 0x2000;
    if ((result = sceFormat(pfsDevicePath, partitionname, &formatSize, 4)) < 0) {
        callback(HDD_INSTALL_FAIL_FORMAT, result, param);
        xglHddUninstall();
        return -1;
    }
    callback(HDD_INSTALL_STAGE_MOUNT, 0, param);
    if ((result = sceMount(mount_device, partitionname, HDD_MOUNT_READ_WRITE, 0, 0)) < 0) {
        callback(HDD_INSTALL_FAIL_MOUNT, result, param);
        xglHddUninstall();
        return -1;
    }
    callback(HDD_INSTALL_STAGE_COPY, 0, param);
    descriptor = sceOpen(hddname, HDD_OPEN_WRITE_CREATE_TRUNC, HDD_MODE_RW_FILE);
    if (descriptor < 0) {
        result = descriptor;
        callback(HDD_INSTALL_FAIL_DATA_OPEN, result, param);
        xglHddUninstall();
        return -1;
    }

    /* Count the archive parts on the disc, then copy each part to the hard disk. */
    archives = 0;
    dvdname_2[10] = '0';
    while ((size = xglCdGetFileSize(dvdname_2)) >= 0) {
        archives += (size + HDD_INSTALL_ARCHIVE_SIZE - 1) / HDD_INSTALL_ARCHIVE_SIZE;
        dvdname_2[10]++;
    }
    archives <<= 16;

    dvdname_2[10] = '0';
    while ((size = xglCdGetFileSize(dvdname_2)) >= 0) {
        for (offset = 0; offset < size;) {
            length = size - offset;
            if (length > HDD_INSTALL_ARCHIVE_SIZE)
                length = HDD_INSTALL_ARCHIVE_SIZE;
            context.base = archives++;
            /* The CD layer types completion callbacks as void; this one
             * returns a status (the original does too), so the pointer type
             * is converted. */
            xglCdReadFilePart(dvdname_2, image, 2,
                              (CdCompletionCallback *)xglHddInstallReadCB,
                              offset, length);
            offset += length;
            if (context.status != 0) {
                sceClose(descriptor);
                if (xglHddCheck() == 0) {
                    xglHddUninstall();
                    return 1;
                }
                return context.status == 1 ? 1 : -1;
            }
            result = sceWrite(descriptor, image, length);
            if (result != length) {
                callback(HDD_INSTALL_FAIL_DATA_WRITE, result, param);
                sceClose(descriptor);
                if (xglHddCheck() == 0)
                    xglHddUninstall();
                return -1;
            }
        }
        dvdname_2[10]++;
    }

    if ((result = sceClose(descriptor)) < 0) {
        callback(HDD_INSTALL_FAIL_DATA_CLOSE, result, param);
        xglHddUninstall();
        return -1;
    }
    /* Write the check file, then remount the partition read-only. */
    callback(HDD_INSTALL_STAGE_FINISH, 0, param);
    descriptor = sceOpen(hddcheck, HDD_OPEN_WRITE_CREATE_TRUNC, HDD_MODE_RW_FILE);
    if (descriptor < 0) {
        result = descriptor;
        callback(HDD_INSTALL_FAIL_CHECK_OPEN, result, param);
        xglHddUninstall();
        return -1;
    }
    callback(HDD_INSTALL_STAGE_FINISH, 1, param);
    xglCdGetFileData(cd_filename, image);
    result = sceWrite(descriptor, image, HDD_SECTOR_SIZE);
    if (result != HDD_SECTOR_SIZE) {
        callback(HDD_INSTALL_FAIL_CHECK_WRITE, result, param);
        sceClose(descriptor);
        xglHddUninstall();
        return -1;
    }
    callback(HDD_INSTALL_STAGE_FINISH, 2, param);
    if ((result = sceClose(descriptor)) < 0) {
        callback(HDD_INSTALL_FAIL_CHECK_CLOSE, result, param);
        xglHddUninstall();
        return -1;
    }
    callback(HDD_INSTALL_STAGE_FINISH, 3, param);
    if ((result = sceUmount(mount_device)) < 0) {
        callback(HDD_INSTALL_FAIL_UNMOUNT, result, param);
        xglHddUninstall();
        return -1;
    }
    callback(HDD_INSTALL_STAGE_FINISH, 4, param);
    if ((result = sceMount(mount_device, partitionname, HDD_MOUNT_READ_ONLY, 0, 0)) < 0) {
        callback(HDD_INSTALL_FAIL_REMOUNT, result, param);
        xglHddUninstall();
        return -1;
    }
    callback(HDD_INSTALL_STAGE_DONE, 0, param);
    return 0;
}

/*
 * Sets HddActive to state (0 to 3) unless it is already 2. A state of -1 only queries.
 * Returns the previous state, or 0 when it was 2 or more.
 */
int xglHddActivate(int state)
{
    int previous = HddActive;

    if (state != -1) {
        if (previous != 2 && (state & 0xff) < 4 && (state & 0xff) >= 0) {
            HddActive = (u8)state;
            if (state < 0x100)
                xglCdArcCheck();
        }
    }
    return previous < 2 ? previous : 0;
}

/*
 * Decodes partitionname in place (XOR 0x80) unless it is already decoded, then copies
 * SYSTEM.CNF byte 0x1a into byte 17.
 */
static void decrypt(void)
{
    u8 *source = partitionname;
    int value = *source;
    register int masked = value & 0xff;
    register u8 *destination = source;

    if (masked != 0x68) {
        if (masked != 0) {
            do {
                *destination = (unsigned int)value ^ 0x80;
                source++;
                value = *source;
                destination++;
            } while (value != 0);
        }
    }
    partitionname[17] = system_cnf[0x1a];
}

/*
 * Checks the HDD and mounts the game partition read-only, comparing its check file with
 * the disc. Returns 1 when ready, 0 when the partition is missing, or a negative code.
 */
int xglHddMount(void)
{
    int mount_result;
    register int readiness;
    int device_result;
    int close_result;
    int capacity;
    int descriptor;
    int mismatch;
    int index;
    u8 *buffer;
    u8 *reference;

    decrypt();
    if (HddActive >= 2)
        return -1;

    HddActive = 0;
    readiness = xglHddCheck();
    if (readiness != 0)
        return readiness == HDD_NOT_READY ? -1 : readiness;

    mount_result = sceMount(mount_device, partitionname, HDD_MOUNT_READ_ONLY, 0, 0);
    if (mount_result == -5)
        return mount_result;
    if (mount_result < 0 && mount_result != HDD_ERR_BUSY) {
        if (mount_result == HDD_ERR_NOENT) {
            device_result = sceDevctl(xgl_hdd_device, HDD_DEVCTL_FREE_SECTORS_TOTAL, 0, 0,
                                      &capacity, 4);
            if (device_result < 0)
                return -2;
            if (capacity <= HDD_INSTALL_MIN_SECTORS)
                return -4;
            device_result = sceDevctl(xgl_hdd_device, HDD_DEVCTL_MAX_SECTORS, 0, 0, 0, 0);
            if (device_result <= HDD_INSTALL_MIN_FREE_SECTORS)
                return -4;
            return 0;
        }
        return -2;
    }

    descriptor = sceOpen(hddcheck, HDD_OPEN_READ, HDD_MODE_READ_ONLY);
    if (descriptor < 0)
        return -5;

    buffer = (u8 *)(((u32)WorkEnd + 63) & (u32)-64);
    reference = buffer + HDD_SECTOR_SIZE;
    buffer[0] = 1;
    reference[0] = 2;
    mismatch = 0;
    if (sceRead(descriptor, buffer, HDD_SECTOR_SIZE) < 0) {
        mismatch = 1;
    } else {
        xglCdGetFileData(cd_filename, reference);
        for (index = 0; index < 36; index++) {
            if (buffer[index] != reference[index]) {
                mismatch = 1;
                break;
            }
        }
    }
    close_result = sceClose(descriptor);
    if (close_result < 0 || mismatch == 1)
        return -5;
    HddActive = 1;
    return 1;
}

/* Declared here, not at the top: moving it up changes the original code. */
extern const char hddResourcePath[16];

extern const char partitionSpecFormat[16];
extern unsigned char mcPartitionSize1G[8];
extern char hddPartitionSize256M[8];
extern char pfsDevicePath[8];

const char hddResourcePath[16] = "data\\hdd.res";

const char partitionSpecFormat[16] = "%s,256M,PFS";

const char cd_filename[16] = "xenosaga.00";

unsigned char mcPartitionSize1G[8] = "1G";

char mount_device[] = "pfs0:";

char hddPartitionSize256M[8] = "256M";

char pfsDevicePath[8] = "pfs:";

HddInstallCBParam *HddInstallCBparam = 0;
