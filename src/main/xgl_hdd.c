#include "common.h"
#include "shared.h"
#include "main/xgl_hdd.h"
#include "xgl_hdd.h"

static int xglHddDummyCB(int event, int value)
{
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddCheckCore);

int xglHddCheck2(void)
{
    int result = xglHddCheckCore();

    if (result == -6) {
        xglHddActivate(0x103);
        return -5;
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddCheck);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddErrorScreen);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcLoadMount);

int xglHddMcUmount(void)
{
    int result = sceUmount(hdd_mc_path);
    return result < 0 ? -2 : 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", make_fullpath);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcExist);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcLoadCore);

int xglHddMcLoad(void *save)
{
    int result;

    result = xglHddMcLoadMount();
    if (result < 0)
        return result;
    result = xglHddMcLoadCore(save);
    if (result < 0)
        return result;
    return xglHddMcUmount();
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", Judge_MakeNewFolder);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", Judge_MakeNewSavedata);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcCheckYourSaves);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcCheckCore);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcCheck);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcGetFree);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", create_file);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcCreate);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddMcSave);

int xglHddUninstall(void)
{
    int result;

    xglHddActivate(0);
    result = sceUmount(mount_device);
    if (result < 0 && result != -19)
        return -1;
    result = sceRemove(partitionname);
    return result < 0 ? -5 : 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddInstallReadCB);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddInstall);

INCLUDE_ASM("asm/main/nonmatchings/xgl_hdd", xglHddActivate);

static void decrypt(void)
{
    u8 *source = partitionname;
    int value = *source;
    register int masked = value & 0xff;
    register u8 *destination = source;

    if (masked != 0x68) {
        if (masked != 0) {
            do {
                *destination = (unsigned int)value ^ 0x80;
                source++;
                value = *source;
                destination++;
            } while (value != 0);
        }
    }
    partitionname[17] = system_cnf[0x1a];
}

int xglHddMount(void)
{
    int mount_result;
    register int readiness;
    int device_result;
    int close_result;
    int capacity;
    int descriptor;
    int mismatch;
    int index;
    u8 *buffer;
    u8 *reference;

    decrypt();
    if (HddActive >= 2)
        return -1;

    HddActive = 0;
    readiness = xglHddCheck();
    if (readiness != 0)
        return readiness == -5 ? -1 : readiness;

    mount_result = sceMount((char *)mount_device, partitionname, 5, 0, 0);
    if (mount_result == -5)
        return mount_result;
    if (mount_result < 0 && mount_result != -16) {
        if (mount_result == -2) {
            device_result = sceDevctl((char *)xgl_hdd_device, 0x480a, 0, 0,
                                      &capacity, 4);
            if (device_result < 0)
                return -2;
            if (capacity <= 0x37ffff)
                return -4;
            device_result = sceDevctl((char *)xgl_hdd_device, 0x4801, 0, 0, 0, 0);
            if (device_result <= 0x7ffff)
                return -4;
            return 0;
        }
        return -2;
    }

    descriptor = sceOpen((char *)hddcheck, 1, 0x16d);
    if (descriptor < 0)
        return -5;

    buffer = (u8 *)(((u32)WorkEnd + 63) & (u32)-64);
    reference = buffer + 512;
    buffer[0] = 1;
    reference[0] = 2;
    mismatch = 0;
    if (sceRead(descriptor, buffer, 512) < 0) {
        mismatch = 1;
    } else {
        xglCdGetFileData((char *)cd_filename, reference);
        for (index = 0; index < 36; index++) {
            if (buffer[index] != reference[index]) {
                mismatch = 1;
                break;
            }
        }
    }
    close_result = sceClose(descriptor);
    if (close_result < 0 || mismatch == 1)
        return -5;
    HddActive = 1;
    return 1;
}
