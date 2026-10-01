#include "common.h"
#include "shared.h"
#include "file.h"
#include "main/xgl_jpeg.h"

typedef struct FileThumbnailRequest {
    u32 source;
    u32 destination;
    u8 unmodeled_08[8];
    u16 destination_x;
    u16 destination_y;
    u16 width;
    u16 height;
    u8 unmodeled_18[8];
} FileThumbnailRequest;

/* FileWork+0x09 is the signed current-slot byte used by the JPEG routines. */
typedef struct FileWorkCurrentSlotView {
    unsigned char unmodeled_00[0x09];
    signed char current_slot;
} FileWorkCurrentSlotView;

/* Five decoder buffers are 0xe100 bytes apart; their signed object number is +1. */
typedef struct FileJpegSlotView {
    unsigned char unmodeled_00;
    signed char number;
    unsigned char unmodeled_02[0xe100 - 0x02];
} FileJpegSlotView;

typedef struct FileJpegDecodeSlotView {
    unsigned char unmodeled_00;
    signed char number;
    unsigned char unmodeled_02[0x80 - 0x02];
    unsigned int dma_qwc;
    unsigned int dma_address;
    unsigned int vif_nop;
    unsigned int vif_directhl;
    unsigned int gif_tag_low;
    unsigned int gif_tag_high;
    unsigned int gif_regs_low;
    unsigned int gif_regs_high;
    unsigned char image[0xe100 - 0xa0];
} FileJpegDecodeSlotView;

typedef struct FileObjectEntryView {
    FileJpegDecodeSlotView *jpeg;
    unsigned char thumbnail[0x1000];
    unsigned char state;
    signed char number;
    unsigned char unmodeled_1006[0x1060 - 0x1006];
} FileObjectEntryView;

extern void FileJpegDecode(int number);
extern char *name_0_0036D628[];

static void VersionUpDate(SaveDataHeader *save_data)
{
    save_data->version += 1;
    FileCheckSumGet((FileChecksumData *)save_data, (long long *)&save_data->checksum);
}

INCLUDE_ASM("asm/main/nonmatchings/file", VersionChange00To01);

void FileVersionCheck(void)
{
}

int FileObjectDataNoGet(int number, int offset)
{
    number += offset;
    if (number < 0)
        return number + 99;
    if (number < 99)
        return number;
    return number - 99;
}

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

void FileThumbnailDecode(u32 source, u32 destination)
{
    FileThumbnailRequest request;

    memset(&request, 0, sizeof(request));
    request.source = source;
    request.destination = destination;
    request.destination_x = 0;
    request.destination_y = 0;
    request.width = 0;
    request.height = 0;
    xglJpegDecode(&request);
}

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

/*
 * Frees every decoder slot whose object data number is no longer one of the
 * five numbers around the slot the file menu is showing.
 */
void FileJpegCheck(void)
{
    signed char valid_numbers[5];
    FileJpegSlotView *slot;
    int i;
    int j;

    i = -1;
    do {
        valid_numbers[i + 1] = FileObjectDataNoGet(((FileWorkCurrentSlotView *)FileWork)->current_slot, i);
        i = i + 1;
    } while (i < 4);

    for (i = 0; i < 5; ++i) {
        slot = (FileJpegSlotView *)FileJpegDec + i;
        for (j = 0; j < 5; ++j) {
            if (slot->number == valid_numbers[j])
                break;
        }
        if (j == 5)
            slot->number = -1;
    }
}

void FileJpegDecode(int number)
{
    FileObjectEntryView *entry = (FileObjectEntryView *)FileObjectData + number;
    FileJpegDecodeSlotView *jpeg = (FileJpegDecodeSlotView *)FileJpegDec;
    FileJpegDecodeSlotView *queued;
    FileJpegDecodeSlotView *slot;
    int i;

    queued = jpeg;
    for (i = 0; i < 5; ++i) {
        if (queued->number == number)
            return;
        ++queued;
    }

    slot = jpeg;
    for (i = 0; i < 5; ++i) {
        if (slot->number < 0) {
            FileThumbnailDecode((u32)entry->thumbnail, (u32)slot->image);
            slot->dma_qwc = 0;
            slot->dma_address = 0;
            slot->vif_nop = 0;
            slot->vif_directhl = 0x51000e01;
            slot->gif_tag_low = 0x8e00;
            slot->gif_tag_high = 0x08000000;
            slot->gif_regs_low = 0;
            slot->gif_regs_high = 0;
            entry->jpeg = slot;
            slot->number = number;
            return;
        }
        ++slot;
    }
}

void FileObjectJpegSet(void)
{
    int offset;
    int slot;

    FileJpegCheck();
    offset = -1;
    do {
        slot = FileObjectDataNoGet(((FileWorkCurrentSlotView *)FileWork)->current_slot, offset);
        offset += 1;
        FileJpegDecode(slot);
    } while (offset < 4);
}

INCLUDE_ASM("asm/main/nonmatchings/file", FileObjectJpegDecChange);

INCLUDE_ASM("asm/main/nonmatchings/file", FileObjectAllLoad);

void FileSinkiSaveDataPush(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/file", FileSaveDataPush);

char *FileSlotNameGet(int slot)
{
    return name_0_0036D628[slot];
}

void TskObjectSet(TskObject *task, TskObjectWorker worker, void *data)
{
    task->data = data;
    task->worker = worker;
    task->state = 0;
}

void tskTskMain(TskObject *task)
{
    TskObjectWorker worker = task->worker;

    if (FileWork->state == 0xff) {
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
