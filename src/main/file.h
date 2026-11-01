/*
 * TU-local declarations of game/167_file (main#167).
 */

#ifndef MAIN_GAME_167_FILE_H
#define MAIN_GAME_167_FILE_H

#include "shared.h"

typedef struct SaveDataHeader {
    u64 version;
    u64 checksum;
} SaveDataHeader;

typedef unsigned char byte;

typedef union {
    byte data[0x163b0];
    struct {
        byte header[8];
        long long stored_checksum;
    } fields;
} FileChecksumData;

/*
 * The file menu's work block: MenuFileMain takes 0x80 bytes, 0x80-aligned, from
 * the menu work area, clears them and stores the pointer in FileWork.
 */
typedef struct FileWorkBlock {
    unsigned char input_flags; /* bit 0 enables the explanatory text */
    unsigned char command;
    unsigned char text_index;
    unsigned char state; /* 0xff ends the file menu */
    unsigned char unmodeled_04;
    unsigned char result;
    unsigned char menu_substate;
    unsigned char dispatch_delay;
    unsigned char unmodeled_08;
    signed char current_slot;
    unsigned char unmodeled_0a[7];
    signed char debug_value_11;
    signed char debug_value_12;
    signed char device_selection;
    signed char debug_value_14;
    signed char action_selection;
    signed char debug_value_16;
    signed char debug_value_17;
    signed char selected_panel;
    signed char panel_count;
    signed char scroll_direction;
    signed char debug_value_1b;
    signed char slot_count;
    signed char selected_slot;
    unsigned char unmodeled_1e[2];
    union FileCardStatus {
        unsigned short status[2];
        unsigned int word;
    } card;
    int card_number;
    unsigned char unmodeled_28[8];
    int available_blocks;
    int directory_result;
    int hdd_result;
    int request_result;
    unsigned short hdd_status;
    unsigned char unmodeled_42[2];
    signed char use_hdd;
    signed char scroll_repeat_count;
    signed char fast_scroll;
    signed char scroll_repeat_direction;
    signed char hide_selection;
    unsigned char unmodeled_49[0x80 - 0x49];
} FileWorkBlock;

extern byte *FileObjectData;
extern byte *FileJpegDec;
extern FileWorkBlock *FileWork;

void FileVersionCheck(SaveDataHeader *save_data);
void FileObjectDataClear(void);
void FileCheckSumGet(FileChecksumData *source, long long *checksum);
int FileCheckSumCheck(FileChecksumData *data);
void FileSinkiSaveDataPush(void);

int xglTaskWaitRemove(XglTaskPrefix *task);

#endif /* MAIN_GAME_167_FILE_H */
