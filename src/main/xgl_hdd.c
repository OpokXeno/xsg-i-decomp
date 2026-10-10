#include "common.h"

#include "shared.h"

#include "main/xgl_hdd.h"

#include "xgl_hdd.h"

/* xglHddMount removes the high bit from this stored volume identifier. */

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

static u8 hddcheck[] = "pfs0:/xenosaga.hdd";

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

/* EUC-JP warning text: HDD application data cannot be read, so loading
 * continues from DVD; the final control-marked label says “Button: Continue”.
 * Preserve the embedded font controls and line breaks from the original. */

const unsigned char D_004D2628[] =
    "\x0b\x0e\x01\x01\x00\x00\x00\x0d\x03\xa5\xcf\xa1\xbc\xa5\xc9\xa5\xc7\xa5\xa3\xa5"
    "\xb9\xa5\xaf\xa5\xc9\xa5\xe9\xa5\xa4\xa5\xd6\xa4\xcb\xa4\xa2\xa4\xeb\xa5\xbc\xa5"
    "\xce\xa5\xb5\xa1\xbc\xa5\xac\xa4\xce\x0a\x0a\xa5\xa2\xa5\xd7\xa5\xea\xa5\xb1\xa1"
    "\xbc\xa5\xb7\xa5\xe7\xa5\xf3\xa5\xc7\xa1\xbc\xa5\xbf\xa4\xac\xc6\xc9\xa4\xdf\xb9"
    "\xfe\xa4\xe1\xa4\xde\xa4\xbb\xa4\xf3\xa1\xa3\x0a\x0a\xa4\xb3\xa4\xec\xb0\xca\xb9"
    "\xdf\xa4\xcf\xa3\xc4\xa3\xd6\xa3\xc4\xa4\xab\xa4\xe9\xc6\xc9\xa4\xdf\xb9\xfe\xa4"
    "\xdf\xa4\xf2\xb9\xd4\xa4\xa4\xa4\xde\xa4\xb9\xa1\xa3\x0a\x0a\x0a\x0a\x0c\x80\x20"
    "\x20\xa1\xfb\x0c\x80\x80\x80\xa5\xdc\xa5\xbf\xa5\xf3\xa1\xa7\xc2\xb3\xb9\xd4";

const char cd_filename[] = "xenosaga.00";

char xgl_hdd_device[] = "hdd0:";

char hdd_mc_path[] = "pfs1:";

u8 mount_device[] = "pfs0:";

HddInstallCBParam *HddInstallCBparam = 0;

static u8 HddActive;

extern char D_004DC378[];

extern char D_004DC380[];

extern char D_004DC388[];

extern int strcmp(const char *left, const char *right);

struct HddFolderStat {
    unsigned int mode;
    unsigned char unmodeled_04_27[0x24];
    unsigned int privateData[6];
};

struct HddFolderDirectoryEntry {
    struct HddFolderStat stat;
    char name[256];
    unsigned char unmodeled_140[16];
};

struct HddSavedataStat {
    unsigned int mode;
    unsigned int flags;
    unsigned char unmodeled_08_27[0x20];
    unsigned int privateData[2];
    unsigned char unmodeled_30_3f[0x10];
};

struct HddSavedataDirectoryEntry {
    struct HddSavedataStat stat;
    char name[256];
    unsigned char unmodeled_140[16];
};

static int xglHddDummyCB(int event, int value)
{
    return 0;
}

static int xglHddCheckCore(void)
{
    long status;
    int expectedStatus;
    int result;

    if (HddActive >= 2)
        return -5;

    status = sceDevctl(xgl_hdd_device, 0x4807, 0, 0, 0, 0);
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
        if (sceDevctl(xgl_hdd_device, 0x4804, 0, 0, 0, 0) >= 0)
            return 0;
        if (HddActive == 1)
            return -6;
        xglHddActivate(0x102);
        return -5;
    }
    return result;
}

int xglHddCheck2(void)
{
    int result = xglHddCheckCore();

    if (result == -6) {
        xglHddActivate(0x103);
        return -5;
    }
    return result;
}

int xglHddCheck(void)
{
    int result;

    if (HddActive == 3) {
        xglHddErrorScreen();
        xglHddActivate(0);
        xglHddActivate(0x102);
    }

    result = xglHddCheckCore();
    if (result != -6)
        return result;

    xglHddErrorScreen();
    xglHddActivate(0);
    xglHddActivate(0x102);
    return -5;
}

void xglHddErrorScreen(void)
{
    int waitFrames = 30;

    sRender.errorScreenActive = 1;
    for (;;) {
        xglDmaDirectNormal(2, TestEnv_0_004A8A80, 6);
        xglFontPrint(48, 64, 0xffffff, D_004D2628);
        if (waitFrames > 0) {
            waitFrames--;
        } else if (PadData.half_2a & 0x20) {
            break;
        }
        xglSleep();
    }
    xglSoundEffectNormalDirect(1);
    sRender.errorScreenActive = 0;
}

int xglHddMcLoadMount(void)
{
    int result;

    if (xglHddCheck2() != 0)
        return -1;

    result = sceMount((char *)hdd_mc_path, (char *)commonname, 5, 0, 0);
    if (result < 0 && result != -16)
        return -1;
    return 0;
}

int xglHddMcUmount(void)
{
    int result = sceUmount(hdd_mc_path);
    return result < 0 ? -2 : 0;
}

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

int xglHddMcExist(struct HddExistRequest *request)
{
    char filePath[256];
    char name[256];
    struct HddDirectoryEntry entry;
    char savePath[256];
    XglClock timestamp;
    int result;
    int descriptor;
    int save;
    int count;
    int newestSave;
    unsigned int newestTime;
    unsigned int time;
    unsigned char *found;
    unsigned char *source;
    char *destination;
    unsigned char *clearPosition;

    result = xglHddMcLoadMount();
    if (result < 0)
        return result;

    if (request->card >= 0) {
        make_fullpath(filePath, request->card, 0);
        descriptor = sceOpen(filePath, 1, 438);
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

        count = 100;
        clearPosition = &found[100];
        do {
            count--;
            *clearPosition = 0;
            clearPosition--;
        } while (count >= 0);

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

int xglHddMcLoadCore(void *save)
{
    struct HddLoadRequest *request = save;
    char path[256];
    int descriptor;
    int result;

    make_fullpath(path, request->card, 0);
    descriptor = sceOpen(path, 1, 438);
    if (descriptor < 0) {
        result = descriptor == -2 ? 1 : -6;
    } else {
        result = 0;
        if (sceRead(descriptor, request->data, request->size) < 0)
            result = -7;
        if (sceClose(descriptor) < 0)
            result = -8;
    }
    return result;
}

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

static int Judge_MakeNewFolder(void)
{
    struct HddFolderDirectoryEntry entry;
    int directoryCount;
    int entriesRead;
    int result;
    int descriptor;
    int closeResult;
    char *entryName;

    directoryCount = 0;
    result = sceDopen(D_004DC378);
    if (result < 0)
        return result;
    descriptor = result;

    for (entriesRead = 0; entriesRead < 1024; entriesRead++) {
        result = sceDread(descriptor, (struct HddDirectoryEntry *)&entry);
        if (result <= 0)
            break;

        entryName = entry.name;
        if (strcmp(entryName, D_004DC380) != 0 &&
            strcmp(entryName, D_004DC388) != 0 &&
            (entry.stat.mode & 0xF000) == 0x1000 &&
            entry.stat.privateData[0] == 0xFFFF &&
            entry.stat.privateData[1] == 0xFFFF) {
            directoryCount++;
            if (directoryCount >= 256)
                break;
        }
    }

    closeResult = sceDclose(descriptor);
    if (result >= 0)
        result = closeResult;

    if (result < 0)
        return result;
    return directoryCount < 256;
}

int Judge_MakeNewSavedata(int descriptor, int card)
{
    extern unsigned int strlen(const char *string);
    extern char *strchr(const char *string, int character);
    struct HddSavedataDirectoryEntry entry;
    int directoryCount;
    int entriesRead;
    int result;
    int closeResult;
    char *path;
    char *entryName;
    unsigned int pathLength;

    directoryCount = 0;
    result = 0;
    path = (char *)xglMcSetFullPath(card, -1) + 1;
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

    for (entriesRead = 0; entriesRead < 1024; entriesRead++) {
        result = sceDread(descriptor, (struct HddDirectoryEntry *)&entry);
        if (result <= 0)
            break;

        entryName = entry.name;
        if (strcmp(entryName, D_004DC380) == 0 ||
            strcmp(entryName, D_004DC388) == 0 ||
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
    return directoryCount < 0x3FE;
}

static int xglHddMcCheckYourSaves(int card)
{
    int descriptor = sceDopen(yoursaves);

    if (descriptor < 0)
        return Judge_MakeNewFolder() == 1 ? 1 : -2;
    return Judge_MakeNewSavedata(descriptor, card) == 1 ? 0 : -3;
}

static int xglHddMcCheckCore(struct HddCheckState *state)
{
    int result = xglHddMcCheckYourSaves(state->status);

    if (result < 0)
        return result;
    {
        int clusterSize = sceDevctl(hdd_mc_path, 0x5001, 0, 0, 0, 0);
        int freeClusters = sceDevctl(hdd_mc_path, 0x5002, 0, 0, 0, 0);
        int dataClusters = (state->dataSize + clusterSize - 1) / clusterSize;
        int reserveClusters = ((unsigned int)clusterSize + 963) / (unsigned int)clusterSize;
        int transferClusters =
            ((unsigned int)(state->transfer->end - state->transfer->begin) + clusterSize - 1)
            / (unsigned int)clusterSize;

        dataClusters += reserveClusters;
        if (freeClusters < dataClusters + transferClusters + 7) {
            int capacity;

            if (sceDevctl((char *)xgl_hdd_device, 0x480a, 0, 0, &capacity, 4) < 0)
                return -1;
            return capacity <= 0x1fffff ? -4 : 1;
        }
    }
    return 0;
}

int xglHddMcCheck(struct HddCheckState *state)
{
    int previous;
    int result;

    if (xglHddCheck2() != 0)
        return -1;

    xglHddMcUmount();
    if (sceMount(hdd_mc_path, commonname, 4, 0, 0) < 0)
        return -1;

    previous = state->status;
    state->status = -1;
    result = xglHddMcCheckCore(state);
    state->status = previous;
    xglHddMcUmount();
    return result;
}

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
            if (sceDevctl(xgl_hdd_device, 0x480A, 0, 0, &capacity, 4) == 0
                && capacity > 0x1FFFFF) {
                freeSpace = 0x100000;
            } else {
                clusterSize = sceDevctl(hdd_mc_path, 0x5001, 0, 0, 0, 0);
                freeClusters = sceDevctl(hdd_mc_path, 0x5002, 0, 0, 0, 0);
                freeSpace = (freeClusters < 0 ? 0 : freeClusters) * (clusterSize / 1024);
            }
        }
        xglHddMcUmount();
        result = freeSpace;
    }
    return result;
}

static int create_file(int card, int slot, const void *data, int size)
{
    char path[256];
    struct HddIoStat stat;
    int descriptor;
    int result;

    make_fullpath(path, card, slot);
    descriptor = sceOpen(path, 0x602, 438);
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

/* The saved image begins with the transfer header. icon.sys occupies +0x40
 * through +0x403; the game payload starts at +0x440. The gaps remain unknown. */
struct HddSaveImage {
    struct HddTransferInfo header;
    unsigned char unmodeled_0c[0x40 - 0x0c];
    char iconSystem[0x3c4];
    unsigned char unmodeled_404[0x440 - 0x404];
    unsigned char payloadStart; /* First byte; the transfer header gives size. */
};

int xglHddMcCreate(struct HddCheckState *state)
{
    extern int sceIoctl2(int descriptor, int command, const void *input,
                         unsigned int input_size, void *output,
                         unsigned int output_size);
    extern int sceMkdir(const char *path, int mode);
    extern void xglMcWriteMapName(char *icon_system, int card);
    extern unsigned char D_004DC390[];
    extern unsigned char tbl_1_004A8AE0[];
    char *deviceName;
    char path[256];
    struct HddIoStat stat;
    int result;
    int descriptor;
    int remountFailure;
    struct HddSaveImage *transfer;
    int checkResult;

    result = -1;
    xglHddMcUmount();
    if (sceMount(hdd_mc_path, commonname, 4, 0, 0) < 0)
        return -1;

    make_fullpath(path, state->status, -1);
    if (state->status >= 0 && (descriptor = sceDopen(path)) >= 0) {
        if (sceDclose(descriptor) < 0) {
            result = -0x15;
            goto unmount_card;
        }
    } else {
        checkResult = xglHddMcCheckCore(state);
        if (checkResult < 0) {
            result = -(int)tbl_1_004A8AE0[~checkResult];
            xglHddMcUmount();
            return result;
        }
        if (checkResult == 1) {
            if (xglHddMcUmount() < 0)
                return -0x19;
            remountFailure = -1;
            deviceName = commonname;
            descriptor = sceOpen(deviceName, 3);
            if (descriptor < 0)
                return -0x10;

            result = 0;
            if (sceIoctl2(descriptor, 0x6801, D_004DC390, 3, 0, 0) < 0)
                result = -0x11;
            if (sceClose(descriptor) < 0)
                result = -0x12;
            if (result != 0)
                return result;
            if (sceMount(hdd_mc_path, commonname, 4, 0, 0) < 0)
                return remountFailure;
            result = 0;
        }

        descriptor = sceDopen(yoursaves);
        if (descriptor < 0) {
            if (sceMkdir(yoursaves, 0x1ff) < 0) {
                result = -10;
                goto unmount_card;
            }
        } else if (sceDclose(descriptor) < 0) {
            result = -0x0c;
            goto unmount_card;
        }

        if (state->status < 0) {
            xglHddMcUmount();
            return result;
        }

        descriptor = sceDopen(path);
        if (descriptor < 0 && descriptor != -2) {
            result = -0x13;
            goto unmount_card;
        }

        if (descriptor == -2) {
            if (sceMkdir(path, 0x1ff) < 0) {
                result = -0x14;
                goto unmount_card;
            }
            stat.attributes = 0xc4a7;
            if (sceChstat(path, &stat, 2) < 0) {
                result = -0x18;
                goto unmount_card;
            }
        } else {
            result = -0x15;
            if (sceDclose(descriptor) < 0)
                goto unmount_card;
        }
    }

    result = -0x16;
    transfer = (struct HddSaveImage *)state->transfer;
    xglMcWriteMapName(transfer->iconSystem, state->status);
    if (create_file(state->status, 1, transfer->iconSystem, 0x3c4) >= 0) {
        result = -0x17;
        if (create_file(state->status, 2, &transfer->payloadStart,
                        transfer->header.end - transfer->header.begin) >= 0)
            result = 0;
    }

unmount_card:
    xglHddMcUmount();
    return result;
}

int xglHddMcSave(const struct HddSaveRequest *save)
{
    int result;

    if (xglHddCheck2() != 0)
        return -1;
    if (sceMount(hdd_mc_path, commonname, 4, 0, 0) < 0)
        return -1;

    result = create_file(save->card, 0, save->data, save->size) < 0 ? -24 : 0;
    xglHddMcUmount();
    return result;
}

int xglHddUninstall(void)
{
    int result;

    xglHddActivate(0);
    result = sceUmount(mount_device);
    if (result < 0 && result != -19)
        return -1;
    result = sceRemove(partitionname);
    return result < 0 ? -5 : 0;
}

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
        result = cb->callback(6, offset, cb->param);
        cb->status = result;
        if (result != 0) {
            return xglCdReadCancel();
        }
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddInstall);

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
        return readiness == -5 ? -1 : readiness;

    mount_result = sceMount((char *)mount_device, partitionname, 5, 0, 0);
    if (mount_result == -5)
        return mount_result;
    if (mount_result < 0 && mount_result != -16) {
        if (mount_result == -2) {
            device_result = sceDevctl((char *)xgl_hdd_device, 0x480a, 0, 0,
                                      &capacity, 4);
            if (device_result < 0)
                return -2;
            if (capacity <= 0x37ffff)
                return -4;
            device_result = sceDevctl((char *)xgl_hdd_device, 0x4801, 0, 0, 0, 0);
            if (device_result <= 0x7ffff)
                return -4;
            return 0;
        }
        return -2;
    }

    descriptor = sceOpen((char *)hddcheck, 1, 0x16d);
    if (descriptor < 0)
        return -5;

    buffer = (u8 *)(((u32)WorkEnd + 63) & (u32)-64);
    reference = buffer + 512;
    buffer[0] = 1;
    reference[0] = 2;
    mismatch = 0;
    if (sceRead(descriptor, buffer, 512) < 0) {
        mismatch = 1;
    } else {
        xglCdGetFileData((char *)cd_filename, reference);
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
