#include "common.h"
#include "shared.h"
#include "yajima_test.h"

extern int printf(const char *format, ...);
extern int sceFormat(char *device, char *block_device, void *argument, int argument_size);
extern int sceDevctl(const char *device, int command, const void *input,
                     unsigned int input_size, void *output, unsigned int output_size);
extern int sceOpen(const char *path, int flags, ...);
extern int sceClose(int descriptor);
extern int sceIoctl2(int handle, int command, const void *input, unsigned int input_size,
                     void *output, unsigned int output_size);
extern int sceMount(char *mount_point, char *device, int flags,
                    void *payload, unsigned int payload_length);
extern int sceUmount(const char *path);
extern int sceMkdir(const char *path, int mode);
extern int sceChstat(const char *path, void *status, unsigned int properties);

extern int xglHddMcGetFree(void);
extern int xglHddActivate(int state);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void xglSleep(void);

/* tya* are defined in main/tu133 (tyaCaptureStart, tyaCaptureEnd),
 * main/tu134 (tyaMenuBgEntry), main/tu135 (tyaElevatorTask), main/tu136
 * (tyaDisplaySetting) and main/tu137 (tyaDrawGauge), none recovered yet;
 * every call in this TU passes literal 0 arguments, so only the argument
 * count is evidenced. */
extern void tyaCaptureStart(int, int);
extern void tyaCaptureEnd(void);
extern void tyaMenuBgEntry(int, int);
extern void tyaElevatorTask(int);
extern void tyaDisplaySetting(int);
extern void tyaDrawGauge(int);

/* "hdd0:" (0x004DA2D0) and "hdd:" (0x004DA2E0): HddTestFormat's record spelled
 * the first one hdd_device_name, which HddTestShutdown's record uses for the
 * second. The merged TU keeps hdd_device for 0x004DA2D0 (canon, HddTest.c).
 */
extern char hdd_device[];
extern char hdd_device_name[];
extern char pfs_device_name[];
extern char hdd_common_device[];
extern char hdd_format_start_message[];
extern char hdd_format_result_format[];
extern char hdd_pfs_format_result_format[];
extern char hdd_shutdown_message[];
extern char hdd_full_path[];
extern char hdd_create_partition_format[];
extern char hdd_partition_size[];
extern char hdd_subcommand_format[];
extern char hdd_close_result_format[];
/* "pfs1:" (0x004DA2F0): HddTestMakeYourSaves' record calls it pfs_mount_point,
 * the mount/unmount records hdd_mount_point; one object, one name here. */
extern char hdd_mount_point[];
extern char hdd_mount_result_format[];
extern char hdd_unmount_result_format[];
extern char hdd_your_saves_directory[];
/* "make YourSaves:%d\n" (0x004C0B70). */
extern char hdd_mkdir_result_format[];
extern char hdd_zone_size_format[];
extern char hdd_zone_free_format[];
extern char hdd_1024_directory_name[];
extern char hdd_chstat_failure_format[];
extern char hdd_dummy_folder_name[];
extern HddTestMenuEntry hdd_test_menu[];
extern char hdd_status_format[];
extern char hdd_status_error_format[];
extern char hdd_free_format[];
extern char hdd_test_title[];
extern char hdd_use_title[];
extern char hdd_no_use_title[];
extern char menu_cursor[];
extern char *name_47[];

/*
 * "%s:%d\n" (0x004DA2F8), the per-directory mkdir report of HddTest1024Save
 * and HddTestDummyFolder. Their records also call it hdd_mkdir_result_format,
 * which in this TU is the 0x004C0B70 string, so it is named apart here. While
 * .sdata stays scaffold-owned the only name the link has for the object is
 * the splat label of its data piece.
 */
extern char D_004DA2F8[];
#define hdd_mkdir_status_format D_004DA2F8

/*
 * PadData (0xd0-byte object at 0x490d90) through HddTest's raw view: it reads
 * the pad state as one doubleword at +40 and halfwords at +42/+44, which no
 * field view of the shared header covers (config/header-canon.json PadData,
 * byte_required src/io/main-00256190/HddTest.c).
 */
extern PadDataRawView PadData;

#define PAD_U64_AT(byte_offset) PadData.doublewords[(byte_offset) / sizeof(u64)]
#define PAD_U16_AT(byte_offset) PadData.halfwords[(byte_offset) / sizeof(u16)]

#define SCE_CST_ATTR 0x0002
#define SCE_STAT_ATTRIBUTE_WORD 1

static void HddTestMountCommon(void);
static void HddTestMakeYourSaves(void);
static void HddTestUnmountCommon(void);

/* xtxdec_sleep (below) is still INCLUDE_ASM; declare it so xtxdec_error does
 * not see an implicit declaration. */
static int xtxdec_sleep(void);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern unsigned char TestEnv_0_0036A030[];

/* Only the packet handle at +0x00 is evidenced. */
static void testfunc(XglPacket **packet) {
    sceVif1PkAddDirectDataN(*packet, TestEnv_0_0036A030, 9);
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", FontTestSub);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", FontTestP0);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", FontTestP1);

extern void nmlModelDirectSend(int mode, u8 *data, int count);

/*
 * TestEnv_34 is a 6-entry, 0x10-byte-stride vertex template (matches its
 * config/symbols/main.txt size 0x60); FontTestLine only touches the x/y of
 * the last two entries (the line's two endpoints), so the rest of each entry
 * and every earlier entry (set elsewhere, still unresolved in this TU) stays
 * unmodeled.
 */
typedef struct TestLineVertex {
    int x;
    int y;
    unsigned char unmodeled_08[8];
} TestLineVertex;

extern TestLineVertex TestEnv_34[6];

static void FontTestLine(int xIndex, int yIndex, int width)
{
  int x0;
  int y;
  int x1;
  x0 = (xIndex * 0x10) + 0x6FF8;
  y = (yIndex * 0x10) + 0x71F7;
  x1 = x0 + (width * 0x10);
  TestEnv_34[5].x = x1;
  TestEnv_34[4].y = y;
  TestEnv_34[5].y = y;
  TestEnv_34[4].x = x0;
  nmlModelDirectSend(1, (u8 *) TestEnv_34, 6);
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", FontTestP2);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", FontTest);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", UmlDispTestSub);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", UmlDispTest);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", DatabaseTest);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", XenoMovieCheck);

static void HddTestFormat(void)
{
    int format_size;
    int status;

    printf(hdd_format_start_message);
    status = sceFormat(hdd_device, 0, 0, 0);
    printf(hdd_format_result_format, status);
    format_size = 0x2000;
    status = sceFormat(pfs_device_name, hdd_common_device, &format_size, 4);
    printf(hdd_pfs_format_result_format, status);
}

static void HddTestShutdown(void)
{
    printf(hdd_shutdown_message);
    sceDevctl(pfs_device_name, 0x5003, 0, 0, 0, 0);
    sceDevctl(hdd_device_name, 0x4806, 0, 0, 0, 0);
}

static void HddTestHddFull(void)
{
    int handle;
    int status;
    int subcommand;

    handle = sceOpen(hdd_full_path, 0x203);
    printf(hdd_create_partition_format, handle);
    for (subcommand = 0;; subcommand++) {
        status = sceIoctl2(handle, 0x6801, hdd_partition_size, 3, 0, 0);
        printf(hdd_subcommand_format, subcommand, status);
        if (status < 0) {
            break;
        }
    }
    status = sceClose(handle);
    printf(hdd_close_result_format, status);
}

static void HddTestMountCommon(void)
{
    int status;

    status = sceMount(hdd_mount_point, hdd_common_device, 4, 0, 0);
    printf(hdd_mount_result_format, status);
}

static void HddTestUnmountCommon(void)
{
    int status;

    status = sceUmount(hdd_mount_point);
    printf(hdd_unmount_result_format, status);
}

static void HddTestMakeYourSaves(void)
{
    int zone_size;
    int zone_free;

    zone_size = sceMkdir(hdd_your_saves_directory, 0x1ff);
    printf(hdd_mkdir_result_format, zone_size);
    zone_size = sceDevctl(hdd_mount_point, 0x5001, 0, 0, 0, 0);
    zone_free = sceDevctl(hdd_mount_point, 0x5002, 0, 0, 0, 0);
    printf(hdd_zone_size_format, zone_size);
    printf(hdd_zone_free_format, zone_free);
}

static void HddTest1024Save(void)
{
    int status;
    int folder_number;
    int tens;
    int hundreds;
    int thousands;
    SceStatRawWords directory_status;

    HddTestMountCommon();
    HddTestMakeYourSaves();
    for (folder_number = 0; folder_number < 1024;) {
        tens = folder_number / 10;
        hdd_1024_directory_name[20] = (folder_number % 10) + '0';
        hundreds = tens / 10;
        hdd_1024_directory_name[19] = (tens % 10) + '0';
        thousands = hundreds / 10;
        hdd_1024_directory_name[18] = (hundreds % 10) + '0';
        hdd_1024_directory_name[17] = (thousands % 10) + '0';
        status = sceMkdir(hdd_1024_directory_name, 0x1ff);
        if ((folder_number++ & 31) == 0) {
            printf(hdd_mkdir_status_format, hdd_1024_directory_name, status);
        }
        if (status < 0) {
            printf(hdd_mkdir_status_format, hdd_1024_directory_name, status);
            break;
        }
        /* Only the 32-bit attribute scalar at byte 4 is caller-evidenced;
           sceChstat copies the complete 64-byte raw record. */
        directory_status[SCE_STAT_ATTRIBUTE_WORD] = 0xc4a7;
        status = sceChstat(hdd_1024_directory_name, directory_status, SCE_CST_ATTR);
        if (status < 0) {
            printf(hdd_chstat_failure_format, status);
            break;
        }
    }
    HddTestUnmountCommon();
}

static void HddTestDummyFolder(void)
{
    int folder_number;
    int tens;
    int hundreds;
    int mkdir_status;

    HddTestMountCommon();
    for (folder_number = 0; folder_number < 256; ++folder_number) {
        tens = folder_number / 10;
        hdd_dummy_folder_name[8] = (folder_number % 10) + '0';
        hundreds = tens / 10;
        hdd_dummy_folder_name[7] = (tens % 10) + '0';
        hdd_dummy_folder_name[6] = (hundreds % 10) + '0';
        mkdir_status = sceMkdir(hdd_dummy_folder_name, 0x1ff);
        printf(hdd_mkdir_status_format, hdd_dummy_folder_name, mkdir_status);
        if (mkdir_status < 0) {
            break;
        }
    }
    HddTestUnmountCommon();
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", HddTestDummySave);

static void HddTestMakeYS(void)
{
    HddTestMountCommon();
    HddTestMakeYourSaves();
    HddTestUnmountCommon();
}

static void HddTest(void)
{
    int status;
    int menu_count;
    int selection;
    int row;
    const char *use_title;

    status = sceDevctl(hdd_device, 0x4807, 0, 0, 0, 0);
    printf(hdd_status_format, status);
    if ((unsigned int)status >= 2) {
        printf(hdd_status_error_format, status);
        return;
    }

    for (menu_count = 0; hdd_test_menu[menu_count].label != 0; menu_count++) {
    }
    selection = 0;
    printf(hdd_free_format, xglHddMcGetFree());

    while ((PAD_U64_AT(40) & 0x08000100ULL) != 0x08000100ULL) {
        int item;

        xglFontDebugPrintf(0, 0, hdd_test_title);
        if (PAD_U16_AT(44) & 0x1000) {
            selection--;
        }
        if (PAD_U16_AT(44) & 0x4000) {
            selection++;
        }
        if (selection < 0) {
            selection = menu_count - 1;
        }
        if (selection >= menu_count) {
            selection = 0;
        }
        if ((PAD_U16_AT(42) & 0x20) &&
            hdd_test_menu[selection].action != 0) {
            hdd_test_menu[selection].action();
        }

        if (xglHddActivate(-1)) {
            use_title = hdd_use_title;
        } else {
            use_title = hdd_no_use_title;
        }
        xglFontDebugPrintf(16, 16, use_title);

        row = 32;
        for (item = 0; item < menu_count; item++) {
            if (item == selection) {
                xglFontDebugPrintf(8, row, menu_cursor);
            }
            xglFontDebugPrintf(16, row, hdd_test_menu[item].label);
            row += 8;
        }
        xglSleep();
    }
}

static char *mt_GetPortName(int port)
{
    return name_47[port];
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", MemcardTest);

static char *xtxdec_nextline(char *text)
{
    while (*text != '\n') {
        if ((unsigned char)*text == 0)
            return 0;
        ++text;
    }
    ++text;
    return (unsigned char)*text ? text : 0;
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", xtxdec_getdec);

static void xtxdec_put4byte(LittleEndianWord *destination, int value)
{
    (*destination)[0] = value;
    (*destination)[1] = value >> 8;
    (*destination)[2] = value >> 16;
    (*destination)[3] = value >> 24;
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", xtxdec_sub);

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", xtxdec_sleep);

static void xtxdec_error(int y, const char *message)
{
    while (xtxdec_sleep() == 0) {
        xglFontDebugPrintf(0, y, message);
        if (PAD_U16_AT(42) & 0x20) {
            break;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", XtxDecode);

static void Dummy(void)
{
    tyaCaptureStart(0, 0);
    tyaCaptureEnd();
    tyaElevatorTask(0);
    tyaMenuBgEntry(0, 0);
    tyaDisplaySetting(0);
    tyaDrawGauge(0);
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", YajimaTest);
