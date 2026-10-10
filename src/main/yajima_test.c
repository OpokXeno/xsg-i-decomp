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

static void HddTestFormat(void);
static void HddTestShutdown(void);
static void HddTestHddFull(void);
static void HddTest1024Save(void);
static void HddTestDummyFolder(void);
static void HddTestDummySave(void);
static void HddTestMakeYS(void);

extern char D_004DA308[8];
extern char D_004C0C68[16];
extern char D_004C0C58[16];
extern char D_004C0C48[16];
extern char D_004C0C38[16];
extern char D_004C0C28[16];
extern char D_004C0C18[16];
extern char D_004C0CF8[16];
extern char D_004C0CE8[16];
extern char D_004DA320[8];

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

extern int sceWrite(int descriptor, void *buffer, unsigned int size);

static char D_004C0BB8[];

static char D_004C0BC8[];

static char D_004C0BD8[];

static char D_004C0BE8[];

static char D_004C0BF8[];

static char D_004C0C08[];

static char D_004DA300[];

static char D_004C14B8[];

extern int main_param_argc;

extern int main_param_argv;

static char D_004C14C8[];

static char D_004C14F8[];

extern void xglRenderClearFrame(void);

extern void xglRenderClearColor(unsigned int color);

extern void xglRenderClearDepth(void);

extern void xglMenuInitial(void);

extern void xglMenuOpen(int mode, YajimaMenuState *menu);

extern void xglMenuDraw(void);

extern void xglClockRead(XglClock *clock);

extern float xglFRand(void);

static YajimaMenuState m_54;

static void FontTest(void);

static void UmlDispTest(void);

static void MemcardTest(void);

static void DatabaseTest(void);

static void XenoMovieCheck(void);

static void HddTest(void);
static void XtxDecode(void);

char D_004DA308[];

char D_004C0C68[];

char D_004C0C58[];

char D_004C0C48[];

char D_004C0C38[];

char D_004C0C28[];

char D_004C0C18[];

char D_004C0CF8[];

char D_004C0CE8[];

char D_004DA320[];

static void (*func_51[7])(void) = {
    FontTest, UmlDispTest, HddTest, MemcardTest,
    DatabaseTest, XenoMovieCheck, XtxDecode,
};

static char D_004DA3A0[];

static char D_004C1518[];

/* Exact storage objects from original initialized data; the partial types above reflect only caller-visible fields. */

const char D_004C0948[16] = "DATABASE";

const char D_004C1508[16] = "XTX DECODE";

const char D_004DA378[];

const char D_004DA380[];

const char D_004DA390[];

const char D_004DA398[];

#include "xgl_menu.h"

/* Leaf rows encode the action ID in the menu next-node slot. */

#define XGL_MENU_ACTION(id) ((XglMenuNode *)(id))

const char D_004DA388[];

static XglMenuRow i1_52[];

static XglMenuNode l1_53;

static void FontTestSub(void);

extern int xglCdGetFileSize(const char *name);

extern char D_004C07F0[];

extern char D_004C0808[];

extern char D_004C0828[];

extern int xglFontLoad(int font, int mode);

extern char test00_26[];

extern char test01_27[];

extern char test02_28[];

extern char test03_29[];

extern char test04_30[];

extern char test05_31[];

extern char test06_32[];

extern char test07_33[];

extern void xglFontPrint(int x, int y, int color, const char *text);

extern void xglFontPrintDirect(const char *text);

extern int xglFontGetStringWidth(const char *text);

extern int xglFontGetProportionalSize(int character);

extern int width_35;

extern char D_004D9F78[];

extern char test_36[6];

extern char D_004D9F88[];

extern char D_004C08D0[];

extern char D_004C08E0[];

extern char test0_37[];

extern char test1_38[];

extern char test2_39[];

extern char test3_40[];

extern char test4_41[];

extern int GameSnapShotNumber(int number);

extern void GameSnapShotCheck(void);

static void FontTestP0(void);

static void FontTestP1(void);

static void FontTestP2(void);

typedef struct UmlDispParam {
    int file_number;
    unsigned char unmodeled_04[4];
    short x;
    short y;
    short width;
    short height;
    long depth;
    short scroll_x;
    short scroll_y;
    unsigned char unmodeled_1c[8];
    int flags;
    unsigned char scroll_enable;
    unsigned char scrolling;
    unsigned char unmodeled_2a[2];
    short scroll_target;
    unsigned char unmodeled_2e[0x12];
} UmlDispParam;

typedef struct UmlDatabaseParam {
    UmlDispParam disp;
    unsigned char *work[4];
    short cursor;
    unsigned char unmodeled_52[6];
    short total;
    short index;
} UmlDatabaseParam;

typedef struct UmlDispVertex {
    int x;
    int y;
    int z;
    int w;
} UmlDispVertex;

extern UmlDispVertex TestEnv_42[6];

typedef struct SaveDataDebug {
    unsigned char unmodeled_00[0x74];
    unsigned char flags[128];
    unsigned char unmodeled_f4[0x102e2 - 0xf4];
    unsigned short umn_flags;
    unsigned char unmodeled_102e4[0x163b0 - 0x102e4];
} SaveDataDebug;

extern unsigned char SaveData[];

extern int xglFontGetSPcodeSize(int code, const char *text);

extern void tyaUmlDispMain(UmlDispParam *param);

extern char D_004C08F0[];

extern char D_004C0900[];

extern char D_004C0918[];

extern char D_004C0928[];

extern char D_004C0938[];

extern char D_004D9F90[];

typedef struct UmlHeaderRecord {
    int size;
    int id;
    unsigned char unmodeled_08[0x60];
    char text[1];
} UmlHeaderRecord;

extern void xglCdLoadOverlay(int overlay);

extern void tyaUmlDispInit2(unsigned char *area);

extern int WindowTexLoad(unsigned char *buffer, unsigned int request);

extern void xglFlagsInitial(void);

extern int xglFlagsSet(int flag, int a, int b);

extern unsigned short xglSRand(void);

extern void xglSoundLoadRequestSmd(const char *name, void *buffer);

extern void endPrintExtFunc(int x, int y, void *data);

extern int tyaUmlDispParamReset(UmlDispParam *param, int mode);

extern int tyaUmlDatabaseMain(UmlDatabaseParam *param);

extern char D_004D9F98[];

extern char D_004D9FA0[];

typedef struct UmlDispRect {
    unsigned char unmodeled_00[8];
    short x;
    short y;
    short width;
    short height;
    long depth;
} UmlDispRect;

extern char D_004C0978[];

extern char D_004C0A68[];

extern char *name_43[103];

extern void GameResourceInit(int, int);

extern int GameMpeg2Play(char *path, int mode);

#define SAVE_FLAGS (((SaveDataDebug *)SaveData)->flags)

extern int xglMcMain(void);

/* Request records copied by the original memory-card and HDD queues. */
typedef struct McRequestParam {
    int file;
    char *name;
    void *data;
    int size;
} McRequestParam;

typedef struct HddMcRequestParam {
    int file;
    void *data;
    int size;
} HddMcRequestParam;

/* carddata.new's two payload offsets are read at +4 and +8. */
typedef struct CardDataHeader {
    unsigned char unmodeled_00[4];
    int begin;
    int end;
} CardDataHeader;

typedef struct MemcardTestWork {
    int result;
    int check;
    int file;
} MemcardTestWork;

extern int xglMcRequest(int port, int command, McRequestParam *param, int *result);

extern int xglMcEasySave(const char *path, const void *buffer, int size);

extern void xglMcSetMapName(const unsigned char *primary_euc_name,
                            const unsigned char *secondary_euc_name);

extern int sceMcSync(int mode, int *command, int *result);

extern int xglHddCheck(void);

extern int xglHddMcExist(HddMcRequestParam *request);

extern int xglHddMcLoad(HddMcRequestParam *request);

extern int xglHddMcCreate(HddMcRequestParam *request);

extern int xglHddMcSave(HddMcRequestParam *request);

#define MC_MAP_NAME_ENCEPHALON "\xA5\xA8\xA5\xF3\xA5\xBB\xA5\xD5\xA5\xA7\xA5\xED\xA5\xF3"

#define MC_MAP_NAME_SIMULATOR "\xA5\xB7\xA5\xDF\xA5\xE5\xA5\xEC\xA1\xBC\xA5\xBF\xA1\xBC\xC6\xE2"

#define MC_TEXT_TITLE "\x0B\xA5\xE1\xA5\xE2\xA5\xAB\xA5\xC6\xA5\xB9\xA5\xC8"

#define MC_TEXT_SELECT_PORT "\xA5\xDD\xA1\xBC\xA5\xC8\xC1\xAA\xC2\xF2"

#define MC_TEXT_FORMAT "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8"

#define MC_TEXT_UNFORMAT "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8"

#define MC_TEXT_LOAD_CHECKING \
    "\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "\xB3\xCE\xC7\xA7\xC3\xE6"

#define MC_TEXT_LOAD_NO_FILE \
    "\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "\xA4\xAC\xA4\xA2\xA4\xEA\xA4\xDE\xA4\xBB\xA4\xF3\n\n\xA3\xCF\xA1" \
    "\xA7\xCC\xE1\xA4\xEB"

#define MC_TEXT_FILE_OF_99 "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xC8\xD6\xB9\xE6 %2d/99"

#define MC_TEXT_FILE_ABSENT "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xA4\xCA\xA4\xB7"

#define MC_TEXT_FILE_PRESENT "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xA4\xA2\xA4\xEA"

#define MC_TEXT_LOAD_READING \
    "\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "%d\xC6\xC9\xA4\xDF\xB9\xFE\xA4\xDF\xC3\xE6"

#define MC_TEXT_SAVE_CHECKING_FREE \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xB6\xF5\xA4\xAD\xCD\xC6\xCE\xCC" \
    "\xA1\xA7\xB3\xCE\xC7\xA7\xC3\xE6"

#define MC_TEXT_SAVE_FREE \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xB6\xF5\xA4\xAD\xCD\xC6\xCE\xCC" \
    "\xA1\xA7%5dKbyte"

#define MC_TEXT_FILE_NUMBER "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xC8\xD6\xB9\xE6 %2d"

#define MC_TEXT_CREATE_CONFIRM \
    "\xBF\xB7\xB5\xAC\xBA\xEE\xC0\xAE\xA4\xB7\xA4\xDE\xA4\xB9\n\n\xA3" \
    "\xCF\xA1\xA7\xA4\xCF\xA4\xA4\xA1\xA1\xA1\xA1\xA3\xD8\xA1\xA7\xA4" \
    "\xA4\xA4\xA4\xA4\xA8"

#define MC_TEXT_SAVE_CREATING \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xBF\xB7\xB5\xAC\xBA\xEE\xC0\xAE" \
    "\xC3\xE6"

#define MC_TEXT_SAVE_CHECKING_FILE \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "%d\xB3\xCE\xC7\xA7\xC3\xE6"

#define MC_TEXT_SAVE_FILE "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB%d"

#define MC_TEXT_OVERWRITE_CONFIRM \
    "\xBE\xE5\xBD\xF1\xA4\xAD\xA4\xB7\xA4\xC6\xA4\xE8\xA4\xED\xA4\xB7" \
    "\xA4\xA4\xA4\xC7\xA4\xB9\xA4\xAB\xA1\xA9\n\n\xA3\xCF\xA1\xA7\xA4" \
    "\xCF\xA4\xA4\xA1\xA1\xA1\xA1\xA3\xD8\xA1\xA7\xA4\xA4\xA4\xA4\xA4" \
    "\xA8"

#define MC_TEXT_SAVE_WRITING \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "%d\xBD\xF1\xA4\xAD\xB9\xFE\xA4\xDF\xC3\xE6"

#define MC_TEXT_FORMAT_CONFIRM \
    "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8\xA4\xB7\xA4\xC6" \
    "\xA4\xE8\xA4\xED\xA4\xB7\xA4\xA4\xA4\xC7\xA4\xB9\xA4\xAB\xA1\xA9" \
    "\n\n\xA3\xCF\xA1\xA7\xA4\xCF\xA4\xA4\xA1\xA1\xA1\xA1\xA1\xDF\xA1" \
    "\xA7\xA4\xA4\xA4\xA4\xA4\xA8"

#define MC_TEXT_FORMATTING \
    "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8\xC3\xE6\xA1\xA6" \
    "\xA1\xA6\xA1\xA6"

#define MC_TEXT_FORMAT_DONE \
    "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8\xA4\xAC\xBD\xAA" \
    "\xCE\xBB\xA4\xB7\xA4\xDE\xA4\xB7\xA4\xBF\n\n\xA3\xCF\xA1\xA7\xCC" \
    "\xE1\xA4\xEB"

#define MC_TEXT_UNFORMAT_CONFIRM \
    "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8" \
    "\xA4\xB7\xA4\xC6\xA4\xE8\xA4\xED\xA4\xB7\xA4\xA4\xA4\xC7\xA4\xB9" \
    "\xA4\xAB\xA1\xA9\n\n\xA3\xCF\xA1\xA7\xA4\xCF\xA4\xA4\xA1\xA1\xA1" \
    "\xA1\xA1\xDF\xA1\xA7\xA4\xA4\xA4\xA4\xA4\xA8"

#define MC_TEXT_UNFORMATTING \
    "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8" \
    "\xC3\xE6\xA1\xA6\xA1\xA6\xA1\xA6"

#define MC_TEXT_UNFORMAT_DONE \
    "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8" \
    "\xA4\xAC\xBD\xAA\xCE\xBB\xA4\xB7\xA4\xDE\xA4\xB7\xA4\xBF\n\n\xA3" \
    "\xCF\xA1\xA7\xCC\xE1\xA4\xEB"

#define MC_TEXT_HDD_LOAD_NO_FILE \
    "\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5" \
    "\xA5\xA1\xA5\xA4\xA5\xEB\xA4\xAC\xA4\xA2\xA4\xEA\xA4\xDE\xA4\xBB" \
    "\xA4\xF3\n\n\xA3\xCF\xA1\xA7\xCC\xE1\xA4\xEB"

#define MC_TEXT_HDD_LOAD "\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xED\xA1\xBC\xA5\xC9"

#define MC_TEXT_HDD_SAVE_FREE \
    "\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xB6\xF5" \
    "\xA4\xAD\xCD\xC6\xCE\xCC\xA1\xA7%5dKbyte"

#define MC_TEXT_LOAD "\xA5\xED\xA1\xBC\xA5\xC9"

#define MC_TEXT_SAVE "\xA5\xBB\xA1\xBC\xA5\xD6"

extern unsigned char tbl_48[4];

extern unsigned char tbl_49[4];

static void testfunc(XglPacket **packet) {
    sceVif1PkAddDirectDataN(*packet, TestEnv_0_0036A030, 9);
}

static void FontTestSub(void)
{
    unsigned char *table = (unsigned char *)0x01000000;
    unsigned char *text;
    unsigned char *entry;
    unsigned char lead;
    unsigned char trail;
    int size;
    int handle;

    if (xglCdReadFile(D_004C07F0, table, 0, 0) > 0) {
        text = (unsigned char *)0x01000100;
        size = xglCdGetFileSize(D_004C0808);
        xglCdReadFile(D_004C0808, text, 0, 0);
        text[size] = 0;
        text[size + 1] = 0;
        while (*text != 0) {
            lead = *text++;
            if (lead == '\n')
                continue;
            trail = *text++;
            if (lead <= 0xD0)
                continue;
            for (entry = table; *entry != 0; entry += 2) {
                if (entry[0] == lead && entry[1] == trail)
                    break;
            }
            if (*entry == 0) {
                entry[0] = lead;
                entry[1] = trail;
                entry[2] = 0;
                entry[3] = 0;
            }
        }
        entry = table;
        while (*entry != 0)
            ++entry;
        handle = sceOpen(D_004C0828, 0x602);
        sceWrite(handle, table, entry - table);
        sceClose(handle);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", FontTestP0);

static void FontTestP1(void)
{
    if (PAD_U16_AT(42) & 0x20) {
        xglFontLoad(0, 0);
    }
    if (PAD_U16_AT(42) & 0x40) {
        xglFontLoad(1, 0);
    }
    if (PAD_U16_AT(42) & 0x10) {
        FontTestSub();
    }
    xglFontDebugPrintf(16, 16, test00_26);
    xglFontDebugPrintf(16, 28, test01_27);
    xglFontDebugPrintf(16, 40, test02_28);
    xglFontDebugPrintf(16, 52, test03_29);
    xglFontDebugPrintf(16, 64, test04_30);
    xglFontDebugPrintf(16, 76, test05_31);
    xglFontDebugPrintf(16, 88, test06_32);
    xglFontDebugPrintf(16, 100, test07_33);
}

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

static void FontTestP2(void)
{
    int character;

    xglFontDebugPrintf(8, 8, D_004D9F78, width_35);
    if (PAD_U16_AT(148) & 0x2000) {
        width_35++;
    }
    if (PAD_U16_AT(148) & 0x8000) {
        width_35--;
    }
    xglFontPrint(64, 96, 0xFFFFFF, D_004C08D0);
    test_36[3] = width_35 >> 8;
    test_36[4] = width_35;
    xglFontPrintDirect(test_36);
    FontTestLine(64, 94, width_35);
    xglFontPrint(64, 96, 0xFFFFFF, test0_37);
    FontTestLine(64, 96, xglFontGetStringWidth(test0_37));
    xglFontPrint(64, 256, 0xFFFFFF, test1_38);
    FontTestLine(64, 256, xglFontGetStringWidth(test1_38));
    xglFontPrint(64, 288, 0xFFFFFF, test2_39);
    FontTestLine(64, 288, xglFontGetStringWidth(test2_39));
    xglFontPrint(64, 320, 0xFFFFFF, test3_40);
    FontTestLine(64, 320, xglFontGetStringWidth(test3_40));
    xglFontPrint(64, 352, 0xFFFFFF, test4_41);
    FontTestLine(64, 352, xglFontGetStringWidth(test4_41));
    xglFontPrint(64, 96, 0xFFFFFF, D_004D9F88);
    if (PAD_U16_AT(42) & 0x20) {
        for (character = 0; character < 64; character++) {
            printf(D_004C08E0, character, xglFontGetProportionalSize(character));
        }
    }
}

static void FontTest(void)
{
    int page = 0;

    GameSnapShotNumber(1);
    while ((PAD_U64_AT(40) & 0x08000100ULL) != 0x08000100ULL) {
        if (PAD_U16_AT(42) & 0x2000) {
            ++page;
        }
        if (PAD_U16_AT(42) & 0x8000) {
            --page;
        }
        switch (page) {
        default:
            page = 0;
        case 0:
            FontTestP0();
            break;
        case 1:
            FontTestP1();
            break;
        case -1:
            page = 2;
        case 2:
            FontTestP2();
            break;
        }
        GameSnapShotCheck();
        xglSleep();
    }
}

static void UmlDispTestSub(XglPacket **packet, UmlDispRect *rect)
{
    int x0 = rect->x * 0x10;
    int y0 = rect->y * 0x10;
    int x1 = (rect->x + rect->width) * 0x10;
    int y1 = (rect->y + rect->height) * 0x10;
    int z = rect->depth - 1;

    TestEnv_42[3].x = 0;
    TestEnv_42[3].y = 0;
    TestEnv_42[3].z = 0x20;
    TestEnv_42[3].w = 0x80;
    TestEnv_42[4].x = x0;
    TestEnv_42[4].y = y0;
    TestEnv_42[4].z = z;
    TestEnv_42[5].x = x1;
    TestEnv_42[5].y = y1;
    TestEnv_42[5].z = z;
    sceVif1PkAddDirectDataN(*packet, TestEnv_42, 6);
}

static void UmlDispTest(void)
{
    UmlDispParam param;
    SaveDataDebug *save;
    SaveDataDebug *first;
    int detail = 0;
    int number = 0;
    UmlHeaderRecord *record;
    char *text;
    int id;
    int size;
    unsigned char character;
    int delta;
    short step;

    xglCdLoadOverlay(2);
    tyaUmlDispInit2((unsigned char *)0x1000000);
    xglFontLoad(1, 0);
    first = (SaveDataDebug *)SaveData;
    first->umn_flags = 0;
    tyaUmlDispParamReset(&param, 0);
    param.file_number = 0;
    param.x = 1808;
    param.y = 1856;
    param.width = 480;
    param.height = 400;
    xglSleep();
    while ((PAD_U64_AT(40) & 0x08000100ULL) != 0x08000100ULL) {
        save = (SaveDataDebug *)SaveData;
        xglFontDebugPrintf(0, 0, D_004C08F0, number);
        if (detail == 0) {
            if (PAD_U16_AT(42) & 0x10) {
                record = (UmlHeaderRecord *)0x1000000;
                xglCdReadFile(D_004C0900, record, 0, 0);
                size = record->size;
                if (size != 0) {
                    do {
                        id = record->id;
                        text = record->text;
                        while ((character = *text) != 0) {
                            if (character < 32) {
                                text += xglFontGetSPcodeSize(character, text);
                            }
                            text++;
                        }
                        printf(D_004C0918, id, record->text, text + 1);
                        record = (UmlHeaderRecord *)((char *)record + size + 4);
                        size = record->size;
                    } while (size != 0);
                }
            }
            if (PAD_U16_AT(42) & 0x8) {
                save->umn_flags ^= 0x10;
            }
            if (PAD_U16_AT(42) & 0x2) {
                save->umn_flags ^= 0x20;
            }
            xglFontDebugPrintf(16, 40, D_004D9F90, (save->umn_flags >> 4) & 1,
                               (save->umn_flags >> 5) & 1);
            xglFontDebugPrintf(16, 16, D_004C0928, number);
            delta = 0;
            if (PAD_U16_AT(44) & 0x1000) {
                delta = -1;
            }
            if (PAD_U16_AT(44) & 0x8000) {
                delta = -1;
            }
            if (PAD_U16_AT(44) & 0x4000) {
                delta = 1;
            }
            if (PAD_U16_AT(44) & 0x2000) {
                delta = 1;
            }
            if (PAD_U16_AT(40) & 0x80) {
                delta *= 100;
            }
            number = (number + delta + 10000) % 10000;
            if (PAD_U16_AT(42) & 0x20) {
                param.file_number = number;
                detail = 1;
                param.scroll_x = 0;
                param.scroll_y = 0;
            }
        } else {
            if (PAD_U16_AT(44) & 0x1000) {
                param.scroll_y -= 8;
            }
            if (PAD_U16_AT(44) & 0x4000) {
                param.scroll_y += 8;
            }
            if (PAD_U16_AT(44) & 0x8000) {
                param.scroll_x -= 8;
            }
            if (PAD_U16_AT(44) & 0x2000) {
                param.scroll_x += 8;
            }
            if (PAD_U16_AT(42) & 0x40) {
                detail = 0;
            }
            if (param.scroll_enable != 0) {
                if (PAD_U16_AT(42) & 0x80) {
                    param.scrolling = 1;
                }
                if (param.scrolling != 0) {
                    step = param.scroll_target - param.scroll_y;
                    if (step == 0) {
                        param.scrolling = 0;
                    } else {
                        if ((unsigned short)step < 4) {
                            step = 4;
                        }
                        param.scroll_y += step >> 2;
                    }
                }
            }
            xglFontDebugPrintf(0, 8, D_004C0938, param.scroll_x, param.scroll_y,
                               param.file_number, param.flags);
            tyaUmlDispMain(&param);
        }
        xglSleep();
    }
}

static void DatabaseTest(void)
{
    unsigned char work0[416];
    unsigned char work1[1536];
    unsigned char work2[48];
    unsigned char work3[48];
    UmlDatabaseParam param;
    int flag;

    xglCdLoadOverlay(2);
    tyaUmlDispInit2((unsigned char *)0x1000000);
    WindowTexLoad((unsigned char *)0x1100000, 1);
    WindowTexLoad((unsigned char *)0x1200000, 2);
    xglFontLoad(1, 0);
    xglFlagsInitial();
    tyaUmlDispParamReset(&param.disp, 0);
    param.work[1] = work1;
    param.work[2] = work2;
    param.work[3] = work3;
    param.disp.x = 1800;
    param.disp.y = 1856;
    param.disp.width = 496;
    param.disp.height = 360;
    param.work[0] = work0;
    param.cursor = 0;
    for (flag = 0; flag < 500; flag++) {
        if (PAD_U16_AT(40) & 0x8) {
            if (xglSRand() & 1) {
                xglFlagsSet(flag, 1, 1);
            }
        } else {
            xglFlagsSet(flag, 1, 1);
        }
    }
    xglSoundLoadRequestSmd(D_004D9F98, (void *)0x1000000);
    xglSleep();
    while ((PAD_U64_AT(40) & 0x08000100ULL) != 0x08000100ULL) {
        xglFontDebugPrintf(0, 0, D_004C0948);
        if (tyaUmlDatabaseMain(&param) != 0) {
            break;
        }
        xglFontDebugPrintf(80, 0, D_004D9FA0, param.index, param.total);
        endPrintExtFunc(0, 100, 0);
        xglSleep();
    }
    xglSoundLoadRequestSmd(0, 0);
}

static void XenoMovieCheck(void)
{
    char path[256];
    int index = 0;

    GameResourceInit(0x1000000, 0x1000000);
    xglFontLoad(0, 0);
    while ((PAD_U64_AT(40) & 0x08000100ULL) != 0x08000100ULL) {
        xglFontDebugPrintf(0, 0, D_004C0978, name_43[index]);
        if (PAD_U16_AT(44) & 0x1000) {
            --index;
        }
        if (PAD_U16_AT(44) & 0x4000) {
            ++index;
        }
        if (index < 0) {
            index = 102;
        }
        if (index >= 103) {
            index = 0;
        }
        if (PAD_U16_AT(42) & 0x20) {
            xglRenderClearColor(0x80000000);
            do {
                xglSleep();
            } while (!(PAD_U16_AT(42) & 0x20));
            sprintf(path, D_004C0A68, name_43[index]);
            GameMpeg2Play(path, 0);
            xglRenderClearFrame();
            do {
                xglSleep();
            } while (!(PAD_U16_AT(42) & 0x20));
            xglRenderClearColor(0x80004000);
        }
        xglSleep();
    }
    xglFontLoad(1, 0);
}

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

static void MemcardTest(void)
{
    unsigned short port_status[2];
    MemcardTestWork work;
    McRequestParam list_request;
    McRequestParam load_request;
    McRequestParam check_request;
    McRequestParam create_request;
    char progress[32];
    McRequestParam save_request;
    HddMcRequestParam hdd_list_request;
    HddMcRequestParam hdd_load_request;
    HddMcRequestParam hdd_check_request;
    HddMcRequestParam hdd_create_request;
    HddMcRequestParam hdd_save_request;
    int sync_command;
    int sync_result;
    CardDataHeader *carddata = (CardDataHeader *)0x01002000;
    unsigned char *found = (unsigned char *)0x01000000;
    char *name;
    char *bar;
    unsigned char *flags;
    unsigned char *shown_flags;
    int port;
    int hdd;
    int state;
    int dir;
    int menu_step;
    int save_file_step;
    int hdd_menu_step;
    int hdd_file_step;
    int done;
    int i;

    port = -1;
    hdd = -99;
    state = 0;
    printf("carddata:%08x\n", carddata);
    xglCdReadFile("data\\carddata.new", carddata, 0, 0);
    xglMcSetMapName(MC_MAP_NAME_ENCEPHALON, MC_MAP_NAME_SIMULATOR);
    printf("sizeof(sceMcIconSys):%d\n", 964);
    printf("sizeof(sSaveData):%d\n", 0x163B0);
    printf("sizeof(carddata):%d\n", carddata->end - carddata->begin);

    while ((PAD_U64_AT(40) & 0x08000100ULL) != 0x08000100ULL) {
        xglFontDebugPrintf(0, 0, MC_TEXT_TITLE);
        /* xglMcMain returns both ports' status halfwords as one word. */
        *(unsigned int *)port_status = xglMcMain();
        xglFontDebugPrintf(64, 0, "%4x %4x", port_status[0], port_status[1]);
        if (port >= 0 && port < 2) {
            if (port_status[port] != 1 && port_status[port] != 0xFF) {
                state = 0;
            }
        }
        xglFontDebugPrintf(0, 16, "%2x", state);

        switch (state) {
        case 0x00:
            state = 0x01;
            hdd = xglHddCheck();
            /* fall through */
        case 0x01:
            xglFontDebugPrintf(0, 32, MC_TEXT_SELECT_PORT);
            name = mt_GetPortName(0);
            if (port_status[0] == 1) {
                xglFontDebugPrintf(16, 48, "%s", name);
            } else {
                xglFontDebugPrintf(16, 48, "\x0C   %s", name);
            }
            name = mt_GetPortName(1);
            if (port_status[1] == 1) {
                xglFontDebugPrintf(16, 64, "%s", name);
            } else {
                xglFontDebugPrintf(16, 64, "\x0C   %s", name);
            }
            name = mt_GetPortName(2);
            if (hdd == 0) {
                xglFontDebugPrintf(16, 80, "%s", name);
            } else {
                xglFontDebugPrintf(16, 80, "\x0C   %s", name);
            }
            if (port_status[0] == 1 || port_status[1] == 1 || hdd == 0) {
                dir = 0;
                if (PAD_U16_AT(42) & 0x1000) {
                    dir = -1;
                }
                if (PAD_U16_AT(42) & 0x4000) {
                    dir = 1;
                }
                if (dir != 0) {
                    work.result += dir;
                } else {
                    dir = 1;
                }
                for (;;) {
                    work.result = (work.result + 3) % 3;
                    if ((port_status[0] != 1 && work.result == 0) ||
                        (port_status[1] != 1 && work.result == 1) ||
                        (hdd < 0 && work.result == 2)) {
                        work.result += dir;
                    } else {
                        break;
                    }
                }
            } else {
                work.result = -1;
            }
            if (work.result != -1 && (PAD_U16_AT(42) & 0x20)) {
                if (work.result == 2) {
                    port = 2;
                    state = 0x60;
                } else {
                    port = work.result;
                    state = 0x10;
                }
            }
            xglFontDebugPrintf(8, work.result * 16 + 48, menu_cursor);
            break;

        case 0x10:
            work.result = 0;
            state = 0x11;
            /* fall through */
        case 0x11:
            xglFontDebugPrintf(0, 32, "%s", mt_GetPortName(port));
            xglFontDebugPrintf(16, 48, MC_TEXT_LOAD);
            xglFontDebugPrintf(16, 64, MC_TEXT_SAVE);
            xglFontDebugPrintf(16, 80, MC_TEXT_FORMAT);
            xglFontDebugPrintf(16, 96, MC_TEXT_UNFORMAT);
            menu_step = (PAD_U16_AT(42) & 0x1000) ? -1 : 0;
            if (PAD_U16_AT(42) & 0x4000) {
                menu_step = 1;
            }
            if (menu_step != 0) {
                work.result = (work.result + menu_step + 4) % 4;
            }
            xglFontDebugPrintf(8, work.result * 16 + 48, menu_cursor);
            if (PAD_U16_AT(42) & 0x20) {
                if (PAD_U16_AT(40) & 0x8) {
                    state = 0;
                    sceMcSync(0, &sync_command, &sync_result);
                    printf("%d %d\n", sync_command, sync_result);
                    xglMcEasySave("/BISLPS-99999Dummy", (void *)0x01000000, 0x77D800);
                } else {
                    state = tbl_48[work.result];
                }
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0;
            }
            break;

        case 0x20:
            list_request.file = -1;
            list_request.data = found;
            xglMcRequest(port, 2, &list_request, &work.result);
            state = 0x21;
            break;

        case 0x21:
            xglFontDebugPrintf(0, 32, MC_TEXT_LOAD_CHECKING);
            if (port_status[port] != 0xFF) {
                state = 0x22;
                if (work.result >= 0) {
                    work.file = 0;
                    state = 0x23;
                }
            }
            break;

        case 0x22:
            xglFontDebugPrintf(0, 32, MC_TEXT_LOAD_NO_FILE, work.file);
            if (PAD_U16_AT(42) & 0x20) {
                state = 0x10;
            }
            break;

        case 0x23:
            xglFontDebugPrintf(0, 32, MC_TEXT_LOAD);
            dir = 0;
            if (PAD_U16_AT(44) & 0x8000) {
                dir = -1;
            }
            if (PAD_U16_AT(44) & 0x2000) {
                dir = 1;
            }
            if (dir != 0) {
                work.file = (work.file + dir + 100) % 100;
            }
            xglFontDebugPrintf(16, 48, MC_TEXT_FILE_OF_99, work.file);
            if (found[work.file] == 0) {
                xglFontDebugPrintf(16, 64, MC_TEXT_FILE_ABSENT);
            } else {
                xglFontDebugPrintf(16, 64, MC_TEXT_FILE_PRESENT);
            }
            if ((PAD_U16_AT(42) & 0x20) && found[work.file] != 0) {
                load_request.file = work.file;
                load_request.data = SaveData;
                load_request.size = 0x163B0;
                state = 0x24;
                xglMcRequest(port, 4, &load_request, &work.result);
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x10;
            }
            break;

        case 0x24:
            xglFontDebugPrintf(0, 32, MC_TEXT_LOAD_READING, work.file);
            if (port_status[port] != 0xFF) {
                state = 0x10;
            }
            break;

        case 0x30:
            xglMcRequest(port, 1, 0, &work.result);
            state = 0x31;
            work.check = 0;
            break;

        case 0x31:
            xglFontDebugPrintf(0, 32, MC_TEXT_SAVE_CHECKING_FREE);
            if (port_status[port] != 0xFF) {
                work.file = 0;
                state = 0x32;
            }
            break;

        case 0x32:
            xglFontDebugPrintf(0, 32, MC_TEXT_SAVE_FREE, work.result);
            save_file_step = (PAD_U16_AT(44) & 0x8000) ? -1 : 0;
            if (PAD_U16_AT(44) & 0x2000) {
                save_file_step = 1;
            }
            if (save_file_step != 0) {
                work.file = (work.file + save_file_step + 100) % 100;
            }
            xglFontDebugPrintf(16, 48, MC_TEXT_FILE_NUMBER, work.file);
            if (PAD_U16_AT(42) & 0x20) {
                check_request.file = work.file;
                xglMcRequest(port, 2, &check_request, &work.check);
                state = 0x35;
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x10;
            }
            break;

        case 0x33:
            xglFontDebugPrintf(0, 32, MC_TEXT_SAVE_FREE, work.result);
            xglFontDebugPrintf(16, 48, MC_TEXT_CREATE_CONFIRM);
            if (PAD_U16_AT(42) & 0x20) {
                create_request.file = work.file;
                create_request.data = carddata;
                xglMcRequest(port, 8, &create_request, &work.result);
                state = 0x34;
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x10;
            }
            break;

        case 0x34:
            xglFontDebugPrintf(0, 32, MC_TEXT_SAVE_CREATING);
            bar = progress;
            for (done = 0; done < work.result; done++) {
                *bar++ = '*';
            }
            for (; done < 14; done++) {
                *bar++ = '.';
            }
            *bar = '\0';
            xglFontDebugPrintf(16, 48, progress);
            if (port_status[port] != 0xFF) {
                state = 0x37;
            }
            break;

        case 0x35:
            xglFontDebugPrintf(0, 32, MC_TEXT_SAVE_CHECKING_FILE, work.file);
            if (port_status[port] != 0xFF) {
                state = 0x33;
            }
            break;

        case 0x36:
            xglFontDebugPrintf(0, 32, MC_TEXT_SAVE_FILE, work.file);
            xglFontDebugPrintf(16, 48, MC_TEXT_OVERWRITE_CONFIRM);
            if (PAD_U16_AT(42) & 0x20) {
                state = 0x37;
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x10;
            }
            break;

        case 0x37:
            save_request.file = work.file;
            save_request.data = SaveData;
            save_request.size = 0x163B0;
            state = 0x38;
            xglMcRequest(port, 6, &save_request, &work.result);
            break;

        case 0x38:
            xglFontDebugPrintf(0, 32, MC_TEXT_SAVE_WRITING, work.file);
            if (port_status[port] != 0xFF) {
                state = 0x10;
            }
            break;

        case 0x40:
            state = 0x41;
            /* fall through */
        case 0x41:
            xglFontDebugPrintf(0, 32, MC_TEXT_FORMAT_CONFIRM);
            if (PAD_U16_AT(42) & 0x20) {
                xglMcRequest(port, 9, 0, &work.result);
                state = 0x42;
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x10;
            }
            break;

        case 0x42:
            xglFontDebugPrintf(0, 32, MC_TEXT_FORMATTING);
            if (port_status[port] != 0xFF) {
                state = 0x43;
            }
            break;

        case 0x43:
            xglFontDebugPrintf(0, 32, MC_TEXT_FORMAT_DONE);
            if (PAD_U16_AT(42) & 0x20) {
                state = 0x10;
            }
            break;

        case 0x50:
            state = 0x51;
            /* fall through */
        case 0x51:
            xglFontDebugPrintf(0, 32, MC_TEXT_UNFORMAT_CONFIRM);
            if (PAD_U16_AT(42) & 0x20) {
                xglMcRequest(port, 10, 0, &work.result);
                state = 0x52;
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x10;
            }
            break;

        case 0x52:
            xglFontDebugPrintf(0, 32, MC_TEXT_UNFORMATTING);
            if (port_status[port] != 0xFF) {
                state = 0x53;
            }
            break;

        case 0x53:
            xglFontDebugPrintf(0, 32, MC_TEXT_UNFORMAT_DONE);
            if (PAD_U16_AT(42) & 0x20) {
                state = 0x10;
            }
            break;

        case 0x60:
            work.result = 0;
            state = 0x61;
            /* fall through */
        case 0x61:
            xglFontDebugPrintf(0, 32, "%s", mt_GetPortName(port));
            xglFontDebugPrintf(16, 48, MC_TEXT_LOAD);
            xglFontDebugPrintf(16, 64, MC_TEXT_SAVE);
            hdd_menu_step = (PAD_U16_AT(42) & 0x1000) ? -1 : 0;
            if (PAD_U16_AT(42) & 0x4000) {
                hdd_menu_step = 1;
            }
            if (hdd_menu_step != 0) {
                work.result = (work.result + hdd_menu_step + 2) % 2;
            }
            xglFontDebugPrintf(8, work.result * 16 + 48, menu_cursor);
            if (PAD_U16_AT(42) & 0x20) {
                state = tbl_49[work.result];
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0;
            }
            break;

        case 0x70:
            hdd_list_request.file = -1;
            hdd_list_request.data = found;
            if (xglHddMcExist(&hdd_list_request) < 0) {
                state = 0x71;
            } else {
                state = 0x72;
            }
            break;

        case 0x71:
            xglFontDebugPrintf(0, 32, MC_TEXT_HDD_LOAD_NO_FILE, work.file);
            if (PAD_U16_AT(42) & 0x20) {
                state = 0x60;
            }
            break;

        case 0x72:
            xglFontDebugPrintf(0, 32, MC_TEXT_HDD_LOAD);
            dir = 0;
            if (PAD_U16_AT(44) & 0x8000) {
                dir = -1;
            }
            if (PAD_U16_AT(44) & 0x2000) {
                dir = 1;
            }
            if (dir != 0) {
                work.file = (work.file + dir + 100) % 100;
            }
            xglFontDebugPrintf(16, 48, MC_TEXT_FILE_OF_99, work.file);
            if (found[work.file] == 0) {
                xglFontDebugPrintf(16, 64, MC_TEXT_FILE_ABSENT);
            } else {
                xglFontDebugPrintf(16, 64, MC_TEXT_FILE_PRESENT);
            }
            if (PAD_U16_AT(42) & 0x20) {
                if (found[work.file] != 0) {
                    hdd_load_request.file = work.file;
                    hdd_load_request.data = SaveData;
                    hdd_load_request.size = 0x163B0;
                    printf("xglHddMcLoad():%d\n", xglHddMcLoad(&hdd_load_request));
                    state = 0x60;
                }
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x60;
            }
            break;

        case 0x80:
            work.result = 0;
            state = 0x81;
            /* fall through */
        case 0x81:
            xglFontDebugPrintf(0, 32, MC_TEXT_HDD_SAVE_FREE, work.result);
            hdd_file_step = (PAD_U16_AT(42) & 0x8000) ? -1 : 0;
            if (PAD_U16_AT(42) & 0x2000) {
                hdd_file_step = 1;
            }
            if (hdd_file_step != 0) {
                work.file = (work.file + hdd_file_step + 100) % 100;
            }
            xglFontDebugPrintf(16, 48, MC_TEXT_FILE_NUMBER, work.file);
            if (PAD_U16_AT(42) & 0x20) {
                int exist;

                hdd_check_request.file = work.file;
                state = 0x82;
                exist = xglHddMcExist(&hdd_check_request);
                printf("xglHddMcExist():%d\n", exist);
                if (exist < 0) {
                    state = 0x60;
                }
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x60;
            }
            break;

        case 0x82:
            xglFontDebugPrintf(0, 32, MC_TEXT_HDD_SAVE_FREE, work.result);
            xglFontDebugPrintf(16, 48, MC_TEXT_CREATE_CONFIRM);
            if (PAD_U16_AT(42) & 0x20) {
                hdd_create_request.file = work.file;
                hdd_create_request.data = carddata;
                hdd_create_request.size = 0x163B0;
                printf("xglHddMcCreate():%d\n", xglHddMcCreate(&hdd_create_request));
                state = 0x84;
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x60;
            }
            break;

        case 0x83:
            xglFontDebugPrintf(0, 32, MC_TEXT_HDD_SAVE_FREE, work.result);
            xglFontDebugPrintf(16, 48, MC_TEXT_OVERWRITE_CONFIRM);
            if (PAD_U16_AT(42) & 0x20) {
                state = 0x84;
            }
            if (PAD_U16_AT(42) & 0x40) {
                state = 0x60;
            }
            break;

        case 0x84:
            hdd_save_request.file = work.file;
            hdd_save_request.data = SaveData;
            hdd_save_request.size = 0x163B0;
            printf("xglHddMcSave():%d\n", xglHddMcSave(&hdd_save_request));
            state = 0x60;
            break;
        }

        if (PAD_U16_AT(42) & 0x10) {
            flags = SAVE_FLAGS;
            for (i = 0; i < 128; i++) {
                *flags++ = xglSRand();
            }
        }
        shown_flags = SAVE_FLAGS;
        xglFontDebugPrintf(16, 128, "%2x %2x %2x %2x %2x %2x %2x %2x", shown_flags[0], shown_flags[1],
                           shown_flags[2], shown_flags[3], shown_flags[4], shown_flags[5], shown_flags[6],
                           shown_flags[7]);
        xglSleep();
    }
}

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

static char *xtxdec_getdec(char *text, int *value)
{
    int number = 0;

    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    while (*text >= '0' && *text <= '9') {
        number = number * 10 + *text - '0';
        ++text;
    }
    if (value != 0) {
        *value = number;
    }
    while (*text != ',') {
        ++text;
    }
    return text + 1;
}

static void xtxdec_put4byte(LittleEndianWord *destination, int value)
{
    (*destination)[0] = value;
    (*destination)[1] = value >> 8;
    (*destination)[2] = value >> 16;
    (*destination)[3] = value >> 24;
}

INCLUDE_ASM("asm/main/nonmatchings/yajima_test", xtxdec_sub);

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

static char hdd_device[8] = "hdd0:";
static char hdd_device_name[8] = "hdd:";
static char pfs_device_name[8] = "pfs:";
static char hdd_common_device[16] = "hdd0:__common";
static char hdd_format_start_message[16] = "format HDD\n";
static char hdd_format_result_format[16] = " result:%d\n";
static char hdd_pfs_format_result_format[24] = " format __common:%d\n";
static char hdd_shutdown_message[16] = "shutdown hdd\n";
static char hdd_full_path[40] = "hdd0:PP.SLPS-99999.DUMMY.DUMMY,,,1G,PFS";
static char hdd_create_partition_format[24] = "create partition:%d\n";
static char hdd_partition_size[8] = "1G";
static char hdd_subcommand_format[16] = "sub%d:%d\n";
static char hdd_close_result_format[16] = "close:%d\n";
static char hdd_mount_point[8] = "pfs1:";
static char hdd_mount_result_format[16] = "mount:%d\n";
static char hdd_unmount_result_format[16] = "umount:%d\n";
static char hdd_your_saves_directory[24] = "pfs1:/Your Saves";
static char hdd_mkdir_result_format[24] = "make YourSaves:%d\n";
static char hdd_zone_size_format[16] = "zonesz:%d\n";
static char hdd_zone_free_format[16] = "zonefree:%d\n";
static char hdd_1024_directory_name[22] = "pfs1:/Your Saves/0000";
static char hdd_chstat_failure_format[16] = "chstat:%d\n";
static char hdd_dummy_folder_name[10] = "pfs1:/000";
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
static char hdd_status_format[24] = "HDIOC_STATUS:%d\n";
static char hdd_status_error_format[32] =
    "HDD\xA5\xC7\xA5\xD0\xA5\xA4\xA5\xB9\xA4\xAC\xC2\xB8\xBA\xDF\xA4\xB7\xA4\xDE\xA4\xBB\xA4\xF3:%d\n";
static char hdd_free_format[24] = "xglHddMcGetFree:%d\n";
static char hdd_test_title[16] = "\x0B\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xC6\xA5\xB9\xA5\xC8";
static char hdd_use_title[8] = "HDD Use";
static char hdd_no_use_title[16] = "HDD NoUse";
static char menu_cursor[8] = ">";
static char *name_47[3] = { D_004C0CF8, D_004C0CE8, D_004DA320 };
static char D_004DA2F8[8] = "%s:%d\n";
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
static char D_004C0BB8[16] = "zone:%d,%d\n";
static char D_004C0BC8[16] = "step:%d\n";
static char D_004C0BD8[16] = "pfs1:/999";
static char D_004C0BE8[16] = "mkdir:%d\n";
static char D_004C0BF8[16] = "pfs1:/999/dummy";
static char D_004C0C08[16] = "open:%d\n";
static char D_004DA300[8] = "%d:%d\n";
static char D_004C14B8[16] = "\x0B\xA3\xD8\xA3\xD4\xA3\xD8\xA5\xC7\xA5\xB3\xA1\xBC\xA5\xC9";
static char D_004C14C8[48] = "\x0B\xB5\xAF\xC6\xB0\xBB\xFE\xA5\xAA\xA5\xD7\xA5\xB7\xA5\xE7\xA5\xF3\xA4\xC7\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xCC\xBE\xA4\xF2\xBB\xD8\xC4\xEA\xA4\xB7\xA4\xC6\xA4\xAF\xA4\xC0\xA4\xB5\xA4\xA4";
static char D_004C14F8[16] = "\xBD\xAA\xCE\xBB\xA4\xB7\xA4\xDE\xA4\xB7\xA4\xBF";
static YajimaMenuState m_54 = {
    { 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xA9, 0x36, 0x00 },
    0
};
char D_004DA308[8] = "FORMAT";
char D_004C0C68[16] = "SHUTDOWN";
char D_004C0C58[16] = "HDD FULL";
char D_004C0C48[16] = "1024 SAVE";
char D_004C0C38[16] = "DUMMY FOLDER";
char D_004C0C28[16] = "COMMON FULL";
char D_004C0C18[16] = "MAKE YOURSAVES";
char D_004C0CF8[16] = "\245\335\241\274\245\310\243\261";
char D_004C0CE8[16] = "\245\335\241\274\245\310\243\262";
char D_004DA320[8] = "\243\310\243\304\243\304";
static char D_004DA3A0[8] = "SELECT";
static char D_004C1518[24] = "%4d.%2d.%2d\n%2d.%2d.%2d";
const char D_004DA378[8] = "MOVIE";
const char D_004DA380[8] = "MEMCARD";
const char D_004DA390[8] = "UMLDISP";
const char D_004DA398[8] = "FONT";
const char D_004DA388[8] = "HDD";
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


