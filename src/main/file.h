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

extern unsigned char *FileWork;

#endif /* MAIN_GAME_167_FILE_H */
