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

void FileVersionCheck(void);

void FileObjectDataClear(void);

extern byte *FileObjectData;

extern byte *FileJpegDec;
void FileCheckSumGet(FileChecksumData *source, long long *checksum);

int FileCheckSumCheck(FileChecksumData *data);

void FileSinkiSaveDataPush(void);

/*
 * TU-local declarations of game/167_file (main#167).
 */

int xglTaskWaitRemove(XglTaskPrefix *task);

/*
 * The file menu's work block: MenuFileMain takes 0x80 bytes, 0x80-aligned, from
 * the menu work area, clears them and stores the pointer in FileWork.
 */
typedef struct FileWorkBlock {
    unsigned char unmodeled_00[0x03];
    unsigned char state; /* 0xff ends the file menu */
    unsigned char unmodeled_04[0x80 - 0x04];
} FileWorkBlock;

extern FileWorkBlock *FileWork;

#endif /* MAIN_GAME_167_FILE_H */
