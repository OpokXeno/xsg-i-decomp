#ifndef INCLUDE_MAIN_XGL_HDD_H
#define INCLUDE_MAIN_XGL_HDD_H

/* Why the HDD save area cannot take another save: xglHddMcGetFree() returns
 * these in place of a free-space count, and xglHddMcCreate() reports the same
 * four causes as the codes below. */
#define HDD_CHECK_ACCESS (-1)
#define HDD_CHECK_TOO_MANY_FOLDERS (-2)
#define HDD_CHECK_TOO_MANY_SAVES (-3)
#define HDD_CHECK_NO_SPACE (-4)

#define HDD_CREATE_ACCESS (-14)
#define HDD_CREATE_TOO_MANY_FOLDERS (-9)
#define HDD_CREATE_TOO_MANY_SAVES (-13)
#define HDD_CREATE_NO_SPACE (-15)

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

/* sceDevctl commands on the hdd0: device (HDIOC_*) and on a mounted pfs
 * partition (PDIOC_*), and the sceIoctl2 command that adds a sub-partition. */
#define HDD_DEVCTL_MAX_SECTORS 0x4801
#define HDD_DEVCTL_FLUSH 0x4804
#define HDD_DEVCTL_DEV9_OFF 0x4806
#define HDD_DEVCTL_STATUS 0x4807
#define HDD_DEVCTL_FREE_SECTORS_TOTAL 0x480a
#define HDD_DEVCTL_ZONE_SIZE 0x5001
#define HDD_DEVCTL_FREE_ZONES 0x5002
#define HDD_DEVCTL_CLOSE_ALL 0x5003
#define HDD_IOCTL2_ADD_SUBPARTITION 0x6801

/* The header of the card data image: the begin and end of the transferred
 * payload (carddata.new). */
struct HddTransferInfo {
    int unmodeled_00;
    int begin;
    int end;
};

struct HddSaveImage;

/* The card save request: xglHddMcCheck/xglHddMcCreate read it through
 * `transfer`, which file.c fills with its card data buffer. */
struct HddCheckState {
    int status;
    union {
        unsigned char *bytes; /* file.c: the card data buffer it hands in */
        struct HddTransferInfo *info; /* xgl_hdd: the begin/end transfer header */
        struct HddSaveImage *image; /* xgl_hdd: the whole saved image, header first */
    } transfer;
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

int xglHddMount(void);
extern int xglHddActivate(int state);
extern int xglHddMcUmount(void);
extern int xglHddMcLoadMount(void);
extern int xglHddMcLoadCore(void *save);

/* Functions and data used by other translation units. */
extern const char hddLoadErrorText[];
extern const char cd_filename[16];
/*
 * The xglHdd layer keeps its own copy of the "hdd0:" device name at
 * main:0x004DC368; the HddTest debug tool has an identical string of its own
 * at 0x004DA2D0, which accepted source calls hdd_device.  Neither address
 * carries an original symbol, so this one is named for the layer that owns
 * it.
 */
extern char xgl_hdd_device[];
extern char hdd_mc_path[];
extern char mount_device[];
extern int xglHddCheck(void);
extern int xglHddMcGetFree(void);
int xglHddMcExist(struct HddExistRequest *request);
int xglHddMcCreate(struct HddCheckState *state);
int xglHddMcSave(const struct HddSaveRequest *save);
extern void xglHddErrorScreen(void);

#endif /* INCLUDE_MAIN_XGL_HDD_H */
