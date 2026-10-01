/*
 * TU-local declarations of main/tu099 (src/main/xgl_hdd.c).
 */

#ifndef SRC_MAIN_XGL_HDD_H
#define SRC_MAIN_XGL_HDD_H

extern int xglCdGetFileData();

#include "shared.h"

struct HddRenderState {
    unsigned char unmodeled_00[0x58];
    unsigned char errorScreenActive;
};

extern struct HddRenderState sRender;
extern PadPrefix PadData;
extern unsigned char TestEnv_0_004A8A80[];
extern char D_004D2628[];
extern void xglDmaDirectNormal(int channel, const void *packet, int qwords);
extern void xglSoundEffectNormalDirect(int sound_id);

int sceRead(int descriptor, void *buffer, int bytes);

int xglHddCheck2(void);

static int xglHddCheckCore(void);

extern int xglHddMcUmount(void);

extern int sceUmount(const char *path);

extern char hdd_mc_path[];

int xglHddMcLoad(void *save);

extern int xglHddMcLoadMount(void);

extern int xglHddMcLoadCore(void *save);

int xglHddUninstall(void);

extern int sceRemove(const char *path);

extern char partitionname[];

extern u8 system_cnf[];

extern u8 HddActive;

extern u8 mount_device[];

/*
 * The xglHdd layer keeps its own copy of the "hdd0:" device name at
 * main:0x004DC368; the HddTest debug tool has an identical string of its own
 * at 0x004DA2D0, which accepted source calls hdd_device.  Neither address
 * carries an original symbol, so this one is named for the layer that owns
 * it.
 */
extern char xgl_hdd_device[];

extern u8 hddcheck[];

extern u8 cd_filename[];

extern int xglHddCheck(void);

extern void xglHddErrorScreen(void);

extern void xglCdArcCheck(void);

extern int sceMount(char *mount_point, char *device, int flags,
                    void *payload, unsigned int payload_length);

extern int sceDevctl(const char *device, int command, const void *input,
                     unsigned int input_size, void *output, unsigned int output_size);

/*
 * SCE fileio: the third argument is the creation mode, supplied only when
 * the flags request creation (xglHddMount passes 0x16D with flag 1).
 */
extern int sceOpen(const char *path, int flags, ...);

extern char commonname[];
extern char yoursaves[];

struct HddLoadRequest {
    int card;
    void *data;
    int size;
};

struct HddTransferInfo {
    int unmodeled_00;
    int begin;
    int end;
};

struct HddCheckState {
    int status;
    struct HddTransferInfo *transfer;
    int dataSize;
};

struct HddIoStat {
    unsigned int mode;
    unsigned int attributes;
    unsigned int size;
    unsigned char created[8];
    unsigned char accessed[8];
    unsigned char modified[8];
    unsigned int sizeHigh;
    unsigned int privateData[6];
};

struct HddSaveRequest {
    int card;
    const void *data;
    int size;
};

struct HddExistRequest {
    int card;
    unsigned char *found;
};

struct HddFileStat {
    unsigned char unmodeled_00[29];
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
    unsigned char unmodeled_24[28];
};

struct HddDirectoryEntry {
    struct HddFileStat stat;
    char name[256];
    unsigned char unmodeled_140[16];
};

extern int sceDopen(const char *path);
extern int sceDread(int descriptor, struct HddDirectoryEntry *entry);
extern int sceDclose(int descriptor);
extern int sceGetstat(const char *path, struct HddFileStat *stat);
extern int sceClose(int descriptor);
extern int strncmp(const char *left, const char *right, unsigned int length);
extern unsigned int xglClockDayTime2UInt(XglClock *clock_time);
extern int sceWrite(int descriptor, const void *buffer, int size);
extern int sceChstat(const char *path, const struct HddIoStat *stat, int flags);
static int Judge_MakeNewFolder(void);
extern int Judge_MakeNewSavedata(int descriptor, int card);
static int xglHddMcCheckYourSaves(int);
static int xglHddMcCheckCore(struct HddCheckState *state);
extern int xglHddMcCheck(struct HddCheckState *state);
static int create_file(int card, int slot, const void *data, int size);
extern unsigned char *xglMcSetFullPath(int card, int slot);

extern int xglHddMcGetFree(void);

/*
 * HddInstallCBparam (main:0x004DC3B0) points at the install context
 * xglHddInstallReadCB reports progress through: callback is invoked with
 * command 6 and a computed offset built from base and the current read
 * value, param is passed through unchanged, status keeps callback's last
 * result, and total is the read count the first (event == 1) call records.
 */
typedef struct HddInstallCBParam {
    int (*callback)(int command, int offset, int param);
    int base;
    int param;
    int status;
    int total;
} HddInstallCBParam;

extern HddInstallCBParam *HddInstallCBparam;

extern int xglCdReadCancel(void);

#endif /* SRC_MAIN_XGL_HDD_H */
