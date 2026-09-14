/*
 * TU-local declarations of main/tu099 (src/main/xgl_hdd.c).
 */

#ifndef SRC_MAIN_XGL_HDD_H
#define SRC_MAIN_XGL_HDD_H

extern int xglCdGetFileData();

#include "shared.h"

int sceRead(int descriptor, void *buffer, int bytes);

int xglHddCheck2(void);

extern int xglHddCheckCore(void);

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

extern int sceMount(char *mount_point, char *device, int flags,
                    void *payload, unsigned int payload_length);

extern int sceDevctl(const char *device, int command, const void *input,
                     unsigned int input_size, void *output, unsigned int output_size);

/*
 * SCE fileio: the third argument is the creation mode, supplied only when
 * the flags request creation (xglHddMount passes 0x16D with flag 1).
 */
extern int sceOpen(const char *path, int flags, ...);

extern int xglHddMcGetFree(void);

#endif /* SRC_MAIN_XGL_HDD_H */
