#include "common.h"
#include "shared.h"
#include "file.h"

static void VersionUpDate(SaveDataHeader *save_data)
{
    save_data->version += 1;
    FileCheckSumGet((FileChecksumData *)save_data, (long long *)&save_data->checksum);
}

INCLUDE_ASM("asm/main/nonmatchings/file", VersionChange00To01);

void FileVersionCheck(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/file", FileObjectDataNoGet);

void FileObjectDataClear(void)
{
    int i;

    for (i = 0; i <= 100; ++i) {
        FileObjectData[i * 0x1060 + 0x1004] = 0;
        FileObjectData[i * 0x1060 + 0x1005] = -1;
    }
    for (i = 0; i <= 4; ++i)
        FileJpegDec[i * 0xe100 + 1] = -1;
}

INCLUDE_ASM("asm/main/nonmatchings/file", FileThumbnailDecode);

void FileCheckSumGet(FileChecksumData *source, long long *checksum)
{
    FileChecksumData local = *source;
    byte *data = local.data;
    unsigned int count = 0;
    int i = 0;

    local.fields.stored_checksum = 0;
    *checksum = 0;
    do {
        *checksum += (long long)*data * i + 0x7ca;
        ++data;
        ++i;
        ++count;
    } while (count <= 0x163af);
}

int FileCheckSumCheck(FileChecksumData *data)
{
    long long checksum;

    FileCheckSumGet(data, &checksum);
    return data->fields.stored_checksum == checksum;
}

INCLUDE_ASM("asm/main/nonmatchings/file", FileObjectDataInput);

INCLUDE_ASM("asm/main/nonmatchings/file", FileJpegCheck);

INCLUDE_ASM("asm/main/nonmatchings/file", FileJpegDecode);

INCLUDE_ASM("asm/main/nonmatchings/file", FileObjectJpegSet);

INCLUDE_ASM("asm/main/nonmatchings/file", FileObjectJpegDecChange);

INCLUDE_ASM("asm/main/nonmatchings/file", FileObjectAllLoad);

void FileSinkiSaveDataPush(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/file", FileSaveDataPush);

INCLUDE_ASM("asm/main/nonmatchings/file", FileSlotNameGet);

void TskObjectSet(TskObject *task, TskObjectWorker worker, void *data)
{
    task->data = data;
    task->worker = worker;
    task->state = 0;
}

void tskTskMain(TskObject *task)
{
    TskObjectWorker worker = task->worker;

    if (FileWork[3] == 0xff) {
        xglTaskWaitRemove(&task->base);
        return;
    }

    if (task->state != 0) {
        if (task->state != 2)
            return;
    } else {
        worker(task, task->data);
        task->state = 2;
    }

    worker(task, task->data);
}

INCLUDE_ASM("asm/main/nonmatchings/file", MenuFilePas);

INCLUDE_ASM("asm/main/nonmatchings/file", MenuFileInfo);

INCLUDE_ASM("asm/main/nonmatchings/file", ListColorChage_3);

INCLUDE_ASM("asm/main/nonmatchings/file", tskFileSelect);

INCLUDE_ASM("asm/main/nonmatchings/file", tskFileEx);

INCLUDE_ASM("asm/main/nonmatchings/file", FileThumbnailDisp);

INCLUDE_ASM("asm/main/nonmatchings/file", tskFileData);

INCLUDE_ASM("asm/main/nonmatchings/file", MenuFile);

INCLUDE_ASM("asm/main/nonmatchings/file", endBackTexDraw);

INCLUDE_ASM("asm/main/nonmatchings/file", MenuFileMain);
