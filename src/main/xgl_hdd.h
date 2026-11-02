/*
 * TU-local declarations of main/tu099 (src/main/xgl_hdd.c).
 */

#ifndef SRC_MAIN_XGL_HDD_H
#define SRC_MAIN_XGL_HDD_H

#include "shared.h"
#include "main/xgl_hdd.h"
#include "main/xgl_dma.h"
#include "main/xgl_mc.h"
#include "main/xgl_sound.h"

struct HddRenderState {
    unsigned char unmodeled_00[0x58];
    unsigned char errorScreenActive;
};

typedef struct HddGifTag {
    u32 low_word;
    u32 high_word;
    u64 registers;
} HddGifTag;

typedef struct HddGsAdRegister {
    u64 value;
    u64 address;
} HddGsAdRegister;

typedef struct HddGsRgbaq {
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
} HddGsRgbaq;

typedef struct HddGsXyz2 {
    u32 x;
    u32 y;
    u32 z;
    u32 : 32;
} HddGsXyz2;

/* The fixed six-qword VIF-to-GIF packet used by the HDD error screen. */
typedef struct HddErrorPacket {
    u32 vif_nop_00;
    u32 vif_nop_04;
    u32 vif_nop_08;
    u32 vif_direct;
    HddGifTag gif_tag;
    HddGsAdRegister ad_register;
    HddGsRgbaq color;
    HddGsXyz2 corners[2];
} HddErrorPacket;

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

struct HddFileStat {
    unsigned int mode;
    unsigned int flags;
    unsigned char unmodeled_08[21];
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
    unsigned char unmodeled_24[4];
    unsigned int privateData[6];
};

struct HddDirectoryEntry {
    struct HddFileStat stat;
    char name[256];
    unsigned char unmodeled_140[16];
};

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

extern struct HddRenderState sRender;
extern HddErrorPacket TestEnv_0_004A8A80[];
extern u8 system_cnf[];
extern HddInstallCBParam *HddInstallCBparam;

extern int xglCdGetFileData();
int sceRead(int descriptor, void *buffer, int bytes);
int xglHddCheck2(void);
static int xglHddCheckCore(void);
extern int sceUmount(const char *path);
int xglHddMcLoad(void *save);
int xglHddUninstall(void);
extern int sceRemove(const char *path);
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
extern int xglCdReadCancel(void);

#endif /* SRC_MAIN_XGL_HDD_H */
