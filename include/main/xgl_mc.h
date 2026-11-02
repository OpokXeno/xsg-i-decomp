#ifndef INCLUDE_MAIN_XGL_MC_H
#define INCLUDE_MAIN_XGL_MC_H

/* Size of the memory-card / HDD save image (SaveData). */
#define MC_SAVE_DATA_SIZE 0x163B0

/* Size of the icon.sys image that opens every save (the icon header). */
#define MC_ICON_SYS_SIZE 964

/* Save slots per device. The directory scan also reports the most recently
 * written slot in one extra byte after the 99 presence flags. */
#define MC_SAVE_SLOT_COUNT 99
#define MC_LATEST_SLOT_INDEX 100

/* Per-card state xglMcMain() reports, one halfword per card (sceMcGetInfo
 * card types: PS1, PS2 and PocketStation are 1, 2 and 3). */
#define MC_TYPE_PS1 1
#define MC_TYPE_PS2 2
#define MC_TYPE_POCKETSTATION 3

#define MC_STATUS_NONE 0
#define MC_STATUS_READY 1
#define MC_STATUS_UNFORMATTED 2
#define MC_STATUS_ERROR 0x80
#define MC_STATUS_PS1_CARD 0x81
#define MC_STATUS_POCKETSTATION 0x82
#define MC_STATUS_BUSY 0xff
/* xglMcMain() word while a request or card poll is in flight: both cards BUSY. */
#define MC_STATUS_BUSY_BOTH 0xFF00FF

/* xglMcRequest() operations. */
#define MC_REQUEST_FREE_SPACE 1
#define MC_REQUEST_DIRECTORY 2
#define MC_REQUEST_PATH_EXISTS 3
#define MC_REQUEST_LOAD 4
#define MC_REQUEST_LOAD_PATH 5
#define MC_REQUEST_SAVE 6
#define MC_REQUEST_SAVE_PATH 7
#define MC_REQUEST_CREATE 8
#define MC_REQUEST_FORMAT 9
#define MC_REQUEST_UNFORMAT 10

extern unsigned char SaveData[];

extern void xglMcSetMapName(const unsigned char *primary_euc_name,
                            const unsigned char *secondary_euc_name);

void xglMcWriteMapName(char *buffer, int slot);

/* Functions and data used by other translation units. */
void xglMcInitial(void);
int xglMcMain(void);
char *xglMcSetFullPath(int card, int selector);

#endif /* INCLUDE_MAIN_XGL_MC_H */
