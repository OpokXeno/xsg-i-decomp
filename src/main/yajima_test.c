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
static char hdd_device[];
static char hdd_device_name[];
static char pfs_device_name[];
static char hdd_common_device[];
static char hdd_format_start_message[];
static char hdd_format_result_format[];
static char hdd_pfs_format_result_format[];
static char hdd_shutdown_message[];
static char hdd_full_path[];
static char hdd_create_partition_format[];
static char hdd_partition_size[];
static char hdd_subcommand_format[];
static char hdd_close_result_format[];
/* "pfs1:" (0x004DA2F0): HddTestMakeYourSaves' record calls it pfs_mount_point,
 * the mount/unmount records hdd_mount_point; one object, one name here. */
static char hdd_mount_point[];
static char hdd_mount_result_format[];
static char hdd_unmount_result_format[];
static char hdd_your_saves_directory[];
/* "make YourSaves:%d\n" (0x004C0B70). */
static char hdd_mkdir_result_format[];
static char hdd_zone_size_format[];
static char hdd_zone_free_format[];
static char hdd_1024_directory_name[];
static char hdd_chstat_failure_format[];
static char hdd_dummy_folder_name[];
static HddTestMenuEntry hdd_test_menu[];
static char hdd_status_format[];
static char hdd_status_error_format[];
static char hdd_free_format[];
static char hdd_test_title[];
static char hdd_use_title[];
static char hdd_no_use_title[];
static char menu_cursor[];
static char *name_47[];

/*
 * "%s:%d\n" (0x004DA2F8), the per-directory mkdir report of HddTest1024Save
 * and HddTestDummyFolder. Their records also call it hdd_mkdir_result_format,
 * which in this TU is the 0x004C0B70 string, so it is named apart here. While
 * .sdata stays scaffold-owned the only name the link has for the object is
 * the splat label of its data piece.
 */
static char D_004DA2F8[];
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

/* xtxdec_sub is still INCLUDE_ASM; declare it here so XtxDecode does not
 * see an implicit declaration. */
static int xtxdec_sub(char *filename);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
static unsigned char TestEnv_0_0036A030[];

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

static TestLineVertex TestEnv_34[];

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

extern int sceWrite(int descriptor, void *buffer, unsigned int size);
static char D_004C0BB8[];
static char D_004C0BC8[];
static char D_004C0BD8[];
static char D_004C0BE8[];
static char D_004C0BF8[];
static char D_004C0C08[];
static char D_004DA300[];

static void HddTestDummySave(void)
{
    int zone_size;
    int chunk_size;
    int remaining;
    int write_size;
    int counter;
    int handle;
    int wrote;
    int mkdir_status;
    int close_status;

    HddTestMountCommon();
    zone_size = sceDevctl(hdd_mount_point, 0x5001, 0, 0, 0, 0);
    remaining = sceDevctl(hdd_mount_point, 0x5002, 0, 0, 0, 0) - 3;
    chunk_size = 0x1000000 / zone_size;
    printf(D_004C0BB8, remaining, zone_size);
    printf(D_004C0BC8, chunk_size);
    mkdir_status = sceMkdir(D_004C0BD8, 0x1ff);
    printf(D_004C0BE8, mkdir_status);
    handle = sceOpen(D_004C0BF8, 0x602, 0x1ff);
    printf(D_004C0C08, handle);

    counter = 0;
    for (;;) {
        if (remaining <= 0) {
            break;
        }
        if (chunk_size < remaining) {
            write_size = chunk_size * zone_size;
            remaining -= chunk_size;
        } else {
            write_size = remaining * zone_size;
            remaining = 0;
        }
        wrote = sceWrite(handle, (void *) 0x1000000, write_size);
        printf(D_004DA300, counter, wrote);
        counter++;
        if (wrote < 0) {
            break;
        }
    }

    close_status = sceClose(handle);
    printf(hdd_close_result_format, close_status);
    HddTestUnmountCommon();
}

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

static char D_004C14B8[];

static int xtxdec_sleep(void)
{
    if ((PAD_U64_AT(40) & 0x08000100ULL) == 0x08000100ULL) {
        return 1;
    }
    xglFontDebugPrintf(0, 0, D_004C14B8);
    xglSleep();
    return 0;
}

static void xtxdec_error(int y, const char *message)
{
    while (xtxdec_sleep() == 0) {
        xglFontDebugPrintf(0, y, message);
        if (PAD_U16_AT(42) & 0x20) {
            break;
        }
    }
}

extern int main_param_argc;
extern int main_param_argv;
static char D_004C14C8[];
static char D_004C14F8[];

static void XtxDecode(void)
{
    int i;
    int index;
    int y;
    int status;

    if (main_param_argc < 2) {
        xtxdec_error(16, D_004C14C8);
        return;
    }

    i = 1;
    y = 16;
    for (;;) {
        if (xtxdec_sleep()) {
            break;
        }
        if (i >= main_param_argc) {
            y = 16;
            break;
        }
        y = 16;
        for (index = 1; index <= i; index++) {
            xglFontDebugPrintf(0, y, ((char **) main_param_argv)[index]);
            y += 8;
        }
        status = xtxdec_sub(((char **) main_param_argv)[i]);
        if (status < 0) {
            break;
        }
        i++;
    }
    xtxdec_error(y, D_004C14F8);
}

static void Dummy(void)
{
    tyaCaptureStart(0, 0);
    tyaCaptureEnd();
    tyaElevatorTask(0);
    tyaMenuBgEntry(0, 0);
    tyaDisplaySetting(0);
    tyaDrawGauge(0);
}

extern void xglRenderClearFrame(void);
extern void xglRenderClearColor(unsigned int color);
extern void xglRenderClearDepth(void);
extern void xglMenuInitial(void);
extern void xglMenuOpen(int mode, YajimaMenuState *menu);
extern void xglMenuDraw(void);
extern void xglClockRead(XglClock *clock);
extern int xglFRand(void);
static YajimaMenuState m_54;
extern void FontTest(void);
extern void UmlDispTest(void);
extern void MemcardTest(void);
extern void DatabaseTest(void);
extern void XenoMovieCheck(void);
extern char D_004DA308[];
extern char D_004C0C68[];
extern char D_004C0C58[];
extern char D_004C0C48[];
extern char D_004C0C38[];
extern char D_004C0C28[];
extern char D_004C0C18[];
extern char D_004C0CF8[];
extern char D_004C0CE8[];
extern char D_004DA320[];
static void (*func_51[])(void);
static char D_004DA3A0[];
static char D_004C1518[];

void YajimaTest(void)
{
    int selection;
    XglClock clock;

    PadData.bytes[0x4E] = 0x40;
    PadData.bytes[0x4F] = 0x40;
    xglRenderClearFrame();
    xglRenderClearColor(0x80004000);

    for (;;) {
        xglMenuInitial();
        m_54.selection = &selection;
        selection = 0;
        xglMenuOpen(-1, &m_54);
        while (selection == 0) {
            xglFontDebugPrintf(0, 0, D_004DA3A0);
            xglMenuDraw();
            xglClockRead(&clock);
            xglFontDebugPrintf(0x80, 0x10, D_004C1518, clock.year, clock.month,
                                clock.day, clock.hour, clock.minute, clock.second);
            xglFRand();
            xglSleep();
        }
        if (selection == -1) {
            break;
        }
        func_51[selection - 1]();
        xglSleep();
    }
    xglRenderClearDepth();
}

/* Exact storage objects from original initialized data; the partial types above reflect only caller-visible fields. */
static char hdd_format_start_message[16] = "format HDD\n";
static char hdd_format_result_format[16] = " result:%d\n";
static char hdd_common_device[16] = "hdd0:__common";
static char hdd_pfs_format_result_format[24] = " format __common:%d\n";
static char hdd_shutdown_message[16] = "shutdown hdd\n";
static char hdd_full_path[40] = "hdd0:PP.SLPS-99999.DUMMY.DUMMY,,,1G,PFS";
static char hdd_create_partition_format[24] = "create partition:%d\n";
static char hdd_subcommand_format[16] = "sub%d:%d\n";
static char hdd_close_result_format[16] = "close:%d\n";
static char hdd_mount_result_format[16] = "mount:%d\n";
static char hdd_unmount_result_format[16] = "umount:%d\n";
static char hdd_your_saves_directory[24] = "pfs1:/Your Saves";
static char hdd_mkdir_result_format[24] = "make YourSaves:%d\n";
static char hdd_zone_size_format[16] = "zonesz:%d\n";
static char hdd_zone_free_format[16] = "zonefree:%d\n";
static char hdd_chstat_failure_format[16] = "chstat:%d\n";
static char D_004C0BB8[16] = "zone:%d,%d\n";
static char D_004C0BC8[16] = "step:%d\n";
static char D_004C0BD8[16] = "pfs1:/999";
static char D_004C0BE8[16] = "mkdir:%d\n";
static char D_004C0BF8[16] = "pfs1:/999/dummy";
static char D_004C0C08[16] = "open:%d\n";
static char hdd_status_format[24] = "HDIOC_STATUS:%d\n";
static char hdd_status_error_format[32] =
    "HDD\xA5\xC7\xA5\xD0\xA5\xA4\xA5\xB9\xA4\xAC\xC2\xB8\xBA\xDF\xA4\xB7\xA4\xDE\xA4\xBB\xA4\xF3:%d\n";
static char hdd_free_format[24] = "xglHddMcGetFree:%d\n";
static char hdd_test_title[16] = "\x0B\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xC6\xA5\xB9\xA5\xC8";
static char hdd_no_use_title[16] = "HDD NoUse";
static char D_004C14B8[16] = "\x0B\xA3\xD8\xA3\xD4\xA3\xD8\xA5\xC7\xA5\xB3\xA1\xBC\xA5\xC9";
static char D_004C14C8[48] = "\x0B\xB5\xAF\xC6\xB0\xBB\xFE\xA5\xAA\xA5\xD7\xA5\xB7\xA5\xE7\xA5\xF3\xA4\xC7\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xCC\xBE\xA4\xF2\xBB\xD8\xC4\xEA\xA4\xB7\xA4\xC6\xA4\xAF\xA4\xC0\xA4\xB5\xA4\xA4";
static char D_004C14F8[16] = "\xBD\xAA\xCE\xBB\xA4\xB7\xA4\xDE\xA4\xB7\xA4\xBF";
static char D_004C1518[24] = "%4d.%2d.%2d\n%2d.%2d.%2d";
static char hdd_device[8] = "hdd0:";
static char pfs_device_name[8] = "pfs:";
static char hdd_device_name[8] = "hdd:";
static char hdd_partition_size[8] = "1G";
static char hdd_mount_point[8] = "pfs1:";
static char D_004DA2F8[8] = "%s:%d\n";
static char D_004DA300[8] = "%d:%d\n";
static char hdd_use_title[8] = "HDD Use";
static char menu_cursor[8] = ">";
static char D_004DA3A0[8] = "SELECT";
static char hdd_1024_directory_name[22] = "pfs1:/Your Saves/0000";
static char hdd_dummy_folder_name[10] = "pfs1:/000";
static unsigned char TestEnv_0_0036A030[0x90] = {
    0x01, 0x80, 0x00, 0x00, 0x00, 0x40, 0x8B, 0x80, 0xEE, 0x1E, 0x53, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xB8, 0x42, 0xA9, 0x06, 0x00, 0x07, 0x20, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00
};
static TestLineVertex TestEnv_34[6] = {
    { 0, 0, { 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x51 } },
    { 32769, 1073823744, { 0x1E, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 458752, 0, { 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 192, 128, { 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00 } },
    { 0, 0, { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 } },
    { 0, 0, { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 } }
};
static HddTestMenuEntry hdd_test_menu[8] = {
    { D_004DA308, HddTestFormat },
    { D_004C0C68, HddTestShutdown },
    { D_004C0C58, HddTestHddFull },
    { D_004C0C48, HddTest1024Save },
    { D_004C0C38, HddTestDummyFolder },
    { D_004C0C28, HddTestDummySave },
    { D_004C0C18, HddTestMakeYS },
    { 0, 0 },
};
static char *name_47[3] = { D_004C0CF8, D_004C0CE8, D_004DA320 };
static void (*func_51[7])(void) = {
    FontTest, UmlDispTest, HddTest, MemcardTest,
    DatabaseTest, XenoMovieCheck, XtxDecode,
};
static YajimaMenuState m_54 = {
    { 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xA9, 0x36, 0x00 },
    0
};



const char D_004C0948[16] = "DATABASE";

char D_004C0C18[16] = "MAKE YOURSAVES";

char D_004C0C28[16] = "COMMON FULL";

char D_004C0C38[16] = "DUMMY FOLDER";

char D_004C0C48[16] = "1024 SAVE";

char D_004C0C58[16] = "HDD FULL";

char D_004C0C68[16] = "SHUTDOWN";

char D_004C0CE8[16] = "\245\335\241\274\245\310\243\262";

char D_004C0CF8[16] = "\245\335\241\274\245\310\243\261";

const char D_004C1508[16] = "XTX DECODE";

char D_004DA308[8] = "FORMAT";

char D_004DA320[8] = "\243\310\243\304\243\304";

const char D_004DA378[8] = "MOVIE";

const char D_004DA380[8] = "MEMCARD";

const char D_004DA390[8] = "UMLDISP";

const char D_004DA398[8] = "FONT";

#include "xgl_menu.h"
/* Leaf rows encode the action ID in the menu next-node slot. */
#define XGL_MENU_ACTION(id) ((XglMenuNode *)(id))
extern const char D_004DA398[];
extern const char D_004DA390[];
extern const char D_004DA388[];
extern const char D_004DA380[];
extern const char D_004C0948[];
extern const char D_004DA378[];
extern const char D_004C1508[];
static XglMenuRow i1_52[];

static XglMenuRow i1_52[7] = {
    { D_004DA398, XGL_MENU_ACTION(1) },
    { D_004DA390, XGL_MENU_ACTION(2) },
    { D_004DA388, XGL_MENU_ACTION(3) },
    { D_004DA380, XGL_MENU_ACTION(4) },
    { D_004C0948, XGL_MENU_ACTION(5) },
    { D_004DA378, XGL_MENU_ACTION(6) },
    { D_004C1508, XGL_MENU_ACTION(7) }
};

static XglMenuNode l1_53 = { 7, { 255, 1, 8 }, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, i1_52 };

const char D_004DA388[8] = "HDD";
