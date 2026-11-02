#ifndef INCLUDE_MAIN_XGL_HDD_H
#define INCLUDE_MAIN_XGL_HDD_H

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
