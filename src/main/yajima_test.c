#include "common.h"
#include "shared.h"
#include "yajima_test.h"
#include "main/xgl_menu.h"

#define hdd_mkdir_status_format fileLineFormat
#define PAD_U64_AT(byte_offset) PadData.doublewords[(byte_offset) / sizeof(u64)]
#define PAD_U16_AT(byte_offset) PadData.halfwords[(byte_offset) / sizeof(u16)]
#define SCE_CST_ATTR 0x0002
#define SCE_STAT_ATTRIBUTE_WORD 1

/* Leaf rows encode the action ID in the menu next-node slot. */
#define XGL_MENU_ACTION(id) ((XglMenuNode *)(id))

#define SAVE_FLAGS (((SaveDataDebug *)SaveData)->flags)

/* JP: エンセフェロン
 * EN: Encephalon */
#define MC_MAP_NAME_ENCEPHALON "\xA5\xA8\xA5\xF3\xA5\xBB\xA5\xD5\xA5\xA7\xA5\xED\xA5\xF3"

/* JP: シミュレーター内
 * EN: Inside the simulator */
#define MC_MAP_NAME_SIMULATOR "\xA5\xB7\xA5\xDF\xA5\xE5\xA5\xEC\xA1\xBC\xA5\xBF\xA1\xBC\xC6\xE2"

/* JP: メモカテスト
 * EN: Memory card test */
#define MC_TEXT_TITLE "\x0B\xA5\xE1\xA5\xE2\xA5\xAB\xA5\xC6\xA5\xB9\xA5\xC8"

/* JP: ポート選択
 * EN: Select port */
#define MC_TEXT_SELECT_PORT "\xA5\xDD\xA1\xBC\xA5\xC8\xC1\xAA\xC2\xF2"

/* JP: フォーマット
 * EN: Format */
#define MC_TEXT_FORMAT "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8"

/* JP: アンフォーマット
 * EN: Unformat */
#define MC_TEXT_UNFORMAT "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8"

/* JP: ロード　ファイル確認中
 * EN: Load: Checking file */
#define MC_TEXT_LOAD_CHECKING \
    "\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "\xB3\xCE\xC7\xA7\xC3\xE6"

/* JP: ロード　ファイルがありません\n\nＯ：戻る
 * EN: Load: No file found\n\nO: Return */
#define MC_TEXT_LOAD_NO_FILE \
    "\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "\xA4\xAC\xA4\xA2\xA4\xEA\xA4\xDE\xA4\xBB\xA4\xF3\n\n\xA3\xCF\xA1" \
    "\xA7\xCC\xE1\xA4\xEB"

/* JP: ファイル番号 %2d/99
 * EN: File number %2d/99 */
#define MC_TEXT_FILE_OF_99 "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xC8\xD6\xB9\xE6 %2d/99"

/* JP: ファイルなし
 * EN: No file */
#define MC_TEXT_FILE_ABSENT "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xA4\xCA\xA4\xB7"

/* JP: ファイルあり
 * EN: File exists */
#define MC_TEXT_FILE_PRESENT "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xA4\xA2\xA4\xEA"

/* JP: ロード　ファイル%d読み込み中
 * EN: Load: Reading file %d */
#define MC_TEXT_LOAD_READING \
    "\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "%d\xC6\xC9\xA4\xDF\xB9\xFE\xA4\xDF\xC3\xE6"

/* JP: セーブ　空き容量：確認中
 * EN: Save: Checking free space */
#define MC_TEXT_SAVE_CHECKING_FREE \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xB6\xF5\xA4\xAD\xCD\xC6\xCE\xCC" \
    "\xA1\xA7\xB3\xCE\xC7\xA7\xC3\xE6"

/* JP: セーブ　空き容量：%5dKbyte
 * EN: Save: Free space: %5dKbyte */
#define MC_TEXT_SAVE_FREE \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xB6\xF5\xA4\xAD\xCD\xC6\xCE\xCC" \
    "\xA1\xA7%5dKbyte"

/* JP: ファイル番号 %2d
 * EN: File number %2d */
#define MC_TEXT_FILE_NUMBER "\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xC8\xD6\xB9\xE6 %2d"

/* JP: 新規作成します\n\nＯ：はい　　Ｘ：いいえ
 * EN: Create a new file\n\nO: Yes    X: No */
#define MC_TEXT_CREATE_CONFIRM \
    "\xBF\xB7\xB5\xAC\xBA\xEE\xC0\xAE\xA4\xB7\xA4\xDE\xA4\xB9\n\n\xA3" \
    "\xCF\xA1\xA7\xA4\xCF\xA4\xA4\xA1\xA1\xA1\xA1\xA3\xD8\xA1\xA7\xA4" \
    "\xA4\xA4\xA4\xA4\xA8"

/* JP: セーブ　新規作成中
 * EN: Save: Creating a new file */
#define MC_TEXT_SAVE_CREATING \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xBF\xB7\xB5\xAC\xBA\xEE\xC0\xAE" \
    "\xC3\xE6"

/* JP: セーブ　ファイル%d確認中
 * EN: Save: Checking file %d */
#define MC_TEXT_SAVE_CHECKING_FILE \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "%d\xB3\xCE\xC7\xA7\xC3\xE6"

/* JP: セーブ　ファイル%d
 * EN: Save: File %d */
#define MC_TEXT_SAVE_FILE "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB%d"

/* JP: 上書きしてよろしいですか？\n\nＯ：はい　　Ｘ：いいえ
 * EN: Overwrite the file?\n\nO: Yes    X: No */
#define MC_TEXT_OVERWRITE_CONFIRM \
    "\xBE\xE5\xBD\xF1\xA4\xAD\xA4\xB7\xA4\xC6\xA4\xE8\xA4\xED\xA4\xB7" \
    "\xA4\xA4\xA4\xC7\xA4\xB9\xA4\xAB\xA1\xA9\n\n\xA3\xCF\xA1\xA7\xA4" \
    "\xCF\xA4\xA4\xA1\xA1\xA1\xA1\xA3\xD8\xA1\xA7\xA4\xA4\xA4\xA4\xA4" \
    "\xA8"

/* JP: セーブ　ファイル%d書き込み中
 * EN: Save: Writing file %d */
#define MC_TEXT_SAVE_WRITING \
    "\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB" \
    "%d\xBD\xF1\xA4\xAD\xB9\xFE\xA4\xDF\xC3\xE6"

/* JP: フォーマットしてよろしいですか？\n\nＯ：はい　　×：いいえ
 * EN: Format?\n\nO: Yes    Cross: No */
#define MC_TEXT_FORMAT_CONFIRM \
    "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8\xA4\xB7\xA4\xC6" \
    "\xA4\xE8\xA4\xED\xA4\xB7\xA4\xA4\xA4\xC7\xA4\xB9\xA4\xAB\xA1\xA9" \
    "\n\n\xA3\xCF\xA1\xA7\xA4\xCF\xA4\xA4\xA1\xA1\xA1\xA1\xA1\xDF\xA1" \
    "\xA7\xA4\xA4\xA4\xA4\xA4\xA8"

/* JP: フォーマット中・・・
 * EN: Formatting... */
#define MC_TEXT_FORMATTING \
    "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8\xC3\xE6\xA1\xA6" \
    "\xA1\xA6\xA1\xA6"

/* JP: フォーマットが終了しました\n\nＯ：戻る
 * EN: Formatting complete\n\nO: Return */
#define MC_TEXT_FORMAT_DONE \
    "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8\xA4\xAC\xBD\xAA" \
    "\xCE\xBB\xA4\xB7\xA4\xDE\xA4\xB7\xA4\xBF\n\n\xA3\xCF\xA1\xA7\xCC" \
    "\xE1\xA4\xEB"

/* JP: アンフォーマットしてよろしいですか？\n\nＯ：はい　　×：いいえ
 * EN: Unformat?\n\nO: Yes    Cross: No */
#define MC_TEXT_UNFORMAT_CONFIRM \
    "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8" \
    "\xA4\xB7\xA4\xC6\xA4\xE8\xA4\xED\xA4\xB7\xA4\xA4\xA4\xC7\xA4\xB9" \
    "\xA4\xAB\xA1\xA9\n\n\xA3\xCF\xA1\xA7\xA4\xCF\xA4\xA4\xA1\xA1\xA1" \
    "\xA1\xA1\xDF\xA1\xA7\xA4\xA4\xA4\xA4\xA4\xA8"

/* JP: アンフォーマット中・・・
 * EN: Unformatting... */
#define MC_TEXT_UNFORMATTING \
    "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8" \
    "\xC3\xE6\xA1\xA6\xA1\xA6\xA1\xA6"

/* JP: アンフォーマットが終了しました\n\nＯ：戻る
 * EN: Unformatting complete\n\nO: Return */
#define MC_TEXT_UNFORMAT_DONE \
    "\xA5\xA2\xA5\xF3\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xDE\xA5\xC3\xA5\xC8" \
    "\xA4\xAC\xBD\xAA\xCE\xBB\xA4\xB7\xA4\xDE\xA4\xB7\xA4\xBF\n\n\xA3" \
    "\xCF\xA1\xA7\xCC\xE1\xA4\xEB"

/* JP: ＨＤＤロード　ファイルがありません\n\nＯ：戻る
 * EN: HDD load: No file found\n\nO: Return */
#define MC_TEXT_HDD_LOAD_NO_FILE \
    "\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xED\xA1\xBC\xA5\xC9\xA1\xA1\xA5\xD5" \
    "\xA5\xA1\xA5\xA4\xA5\xEB\xA4\xAC\xA4\xA2\xA4\xEA\xA4\xDE\xA4\xBB" \
    "\xA4\xF3\n\n\xA3\xCF\xA1\xA7\xCC\xE1\xA4\xEB"

/* JP: ＨＤＤロード
 * EN: HDD load */
#define MC_TEXT_HDD_LOAD "\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xED\xA1\xBC\xA5\xC9"

/* JP: ＨＤＤセーブ　空き容量：%5dKbyte
 * EN: HDD save: Free space: %5dKbyte */
#define MC_TEXT_HDD_SAVE_FREE \
    "\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xBB\xA1\xBC\xA5\xD6\xA1\xA1\xB6\xF5" \
    "\xA4\xAD\xCD\xC6\xCE\xCC\xA1\xA7%5dKbyte"

/* JP: ロード
 * EN: Load */
#define MC_TEXT_LOAD "\xA5\xED\xA1\xBC\xA5\xC9"

/* JP: セーブ
 * EN: Save */
#define MC_TEXT_SAVE "\xA5\xBB\xA1\xBC\xA5\xD6"

typedef struct XtxHeader {
    int magic[2]; /* +0x0 */
    int image_count;
    int table_offset;
} XtxHeader;

typedef struct XtxImageEntry {
    unsigned int dimensions;
    short height;
    unsigned char unmodeled_06[2];
    unsigned int vram_address;
    unsigned char unmodeled_0c[4];
    unsigned int data_offset;
} XtxImageEntry;

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

typedef struct SaveDataDebug {
    unsigned char unmodeled_00[0x74];
    unsigned char flags[128];
    unsigned char unmodeled_f4[0x102e2 - 0xf4];
    unsigned short umn_flags;
    unsigned char unmodeled_102e4[0x163b0 - 0x102e4];
} SaveDataDebug;

typedef struct UmlHeaderRecord {
    int size;
    int id;
    unsigned char unmodeled_08[0x60];
    char text[1];
} UmlHeaderRecord;

typedef struct UmlDispRect {
    unsigned char unmodeled_00[8];
    short x;
    short y;
    short width;
    short height;
    long depth;
} UmlDispRect;

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

/* "hdd0:" (0x004DA2D0) and "hdd:" (0x004DA2E0): HddTestFormat's record spelled
 * the first one hdd_device_name, which HddTestShutdown's record uses for the
 * second. The merged TU keeps hdd_device for 0x004DA2D0 (canon, HddTest.c).
 */
static char hdd_device[];

static char hdd_device_name[];
static char pfs_device_name[];
static const char hdd_common_device[];
static const char hdd_format_start_message[];
static const char hdd_format_result_format[];
static const char hdd_pfs_format_result_format[];
static const char hdd_shutdown_message[];
static const char hdd_full_path[];
static const char hdd_create_partition_format[];
static char hdd_partition_size[];
static const char hdd_subcommand_format[];
static const char hdd_close_result_format[];

/* "pfs1:" (0x004DA2F0): HddTestMakeYourSaves' record calls it pfs_mount_point,
 * the mount/unmount records hdd_mount_point; one object, one name here. */
static char hdd_mount_point[];

static const char hdd_mount_result_format[];
static const char hdd_unmount_result_format[];
static const char hdd_your_saves_directory[];

/* "make YourSaves:%d\n" (0x004C0B70). */
static const char hdd_mkdir_result_format[];

static const char hdd_zone_size_format[];
static const char hdd_zone_free_format[];
static char hdd_1024_directory_name[];
static const char hdd_chstat_failure_format[];
static char hdd_dummy_folder_name[];
extern char hddFormatLabel[8];
extern const char shutdownLabel[16];
extern const char hddFullLabel[16];
extern const char save1024Label[16];
extern const char dummyFolderLabel[16];
extern const char commonFullLabel[16];
extern const char makeYourSavesLabel[16];
extern const char port1Label[16];
extern const char port2Label[16];
extern char hddText[8];
static HddTestMenuEntry hdd_test_menu[];
static const char hdd_status_format[];
static const char hdd_status_error_format[];
static const char hdd_free_format[];
static const char hdd_test_title[];
static char hdd_use_title[];
static const char hdd_no_use_title[];
static char menu_cursor[];
static char *name_47[];

/*
 * "%s:%d\n" (0x004DA2F8), the per-directory mkdir report of HddTest1024Save
 * and HddTestDummyFolder. Their records also call it hdd_mkdir_result_format,
 * which in this TU is the 0x004C0B70 string, so it is named apart here. While
 * .sdata stays scaffold-owned the only name the link has for the object is
 * the splat label of its data piece.
 */
static char fileLineFormat[];

/*
 * PadData (0xd0-byte object at 0x490d90) through HddTest's raw view: it reads
 * the pad state as one doubleword at +40 and halfwords at +42/+44, which no
 * field view of the shared header covers (config/header-canon.json PadData,
 * byte_required src/io/main-00256190/HddTest.c).
 */
extern PadDataRawView PadData;

extern const char xtxListPathFormat[];
extern const char xtxReadFormat[];
extern const char xtxFilePathFormat[];
extern const char xtxTbwFormat[];
extern const char xtxEntryFormat[];
static unsigned char TestEnv_0_0036A030[];
static TestLineVertex TestEnv_34[];
static const char zoneStatusFormat[];
static const char chunkStepFormat[];
static const char dummySaveFolderPath[];
static const char mkdirStatusFormat[];
static const char dummySaveFilePath[];
static const char openHandleFormat[];
static char counterWroteFormat[];
static const char xtxDecodeText[];
extern int main_param_argc;
extern int main_param_argv;
static const char specifyFileNameText[];
static const char xtxFinishedText[];
static char selectText[];
static const char clockDateTimeFormat[];
extern const char databaseLabel[16];
extern const char xtxDecodeLabel[16];
const char movieLabel[];
const char memcardLabel[];
const char umlDispLabel[];
const char fontLabel[];
const char hddLabel[];
static XglMenuRow i1_52[];
extern const char nisuiFontPath[];
extern const char scenarioTextPath[];
extern const char nisuiHostPath[];
extern char fontTestWidthFormat[];
extern char fontTestSpacingControl[];
extern const char fontTestEffectControl[];
extern const char characterSizeFormat[];
extern const char smallCharacterTestText[];
extern const char test000Text[];
extern const char test001Text[];
extern const char hesitationText[];
extern unsigned char SaveData[];
extern const char umlDispFormat[];
extern const char umnHeaderPath[];
extern const char umnRecordFormat[];
extern const char fileSelectFormat[];
extern const char scrollPositionFormat[];
extern char umlBitsFormat[];
extern char umnModeSoundName[];
extern char databaseIndexFormat[];
extern const char moviePlaybackHelpText[];
extern const char movieFilePathFormat[];
extern const char movie3050C1_9Name[16];
extern const char prologueMovieName[16];
extern const char movie3058DName[8];
extern const char movie3058BName[8];
extern const char movie3058AName[8];
extern const char movie3056CName[8];
extern const char movie3056BName[8];
extern const char movie3056AName[8];
extern const char movie3055_1Name[8];
extern const char movie3054B2Name[8];
extern const char movie3052Name[8];
extern const char movie3050C2Name[8];
extern const char movie3048Name[8];
extern const char movie3047DName[8];
extern const char movie3047CName[8];
extern const char movie3047BName[8];
extern const char movie3046BName[8];
extern const char movie3046AName[8];
extern const char movie3045Name[8];
extern const char movie3042Name[8];
extern const char movie3040Name[8];
extern const char movie3039CName[8];
extern const char movie3039B_1Name[8];
extern const char movie3039AName[8];
extern const char movie3037Name[8];
extern const char movie3036IName[8];
extern const char movie3035Name[8];
extern const char movie3028BName[8];
extern const char movie3027_1Name[8];
extern const char movie3026AName[8];
extern const char movie3022Name[8];
extern const char movie3021_1Name[8];
extern const char movie3020Name[8];
extern const char movie3017_1Name[8];
extern const char movie3016_1Name[8];
extern const char movie3015Name[8];
extern const char movie3014_1Name[8];
extern const char movie3011Name[8];
extern const char movie3010Name[8];
extern const char movie3009DName[8];
extern const char movie3008BName[8];
extern const char movie3008AName[8];
extern const char movie3007Name[8];
extern const char movie3003EName[8];
extern const char movie3003AName[8];
extern const char movie3000Name[8];
extern const char movie2064_2Name[8];
extern const char movie2064_1Name[8];
extern const char movie2058AName[8];
extern const char movie2057Name[8];
extern const char movie2056AName[8];
extern const char movie2055Name[8];
extern const char movie2053Name[8];
extern const char movie2051Name[8];
extern const char movie2048Name[8];
extern const char movie2046Name[8];
extern const char movie2044AName[8];
extern const char movie2034Name[8];
extern const char movie2033BName[8];
extern const char movie2033AName[8];
extern const char movie2032Name[8];
extern const char movie2031_2Name[8];
extern const char movie2031_1Name[8];
extern const char movie2018Name[8];
extern const char movie2017Name[8];
extern const char movie2015Name[8];
extern const char movie2012BName[8];
extern const char movie2010Name[8];
extern const char movie2007C_2Name[8];
extern const char movie2007C_1Name[8];
extern const char movie2007BName[8];
extern const char movie2007Name[8];
extern const char movie2006_1Name[8];
extern const char movie2004D_2Name[8];
extern const char movie2004C_1Name[8];
extern const char movie2002B_1Name[8];
extern const char movie2001A_1Name[8];
extern const char movie2000_1Name[8];
extern const char movie1044BName[8];
extern const char movie1043_1Name[8];
extern const char movie1041_1Name[8];
extern const char movie1040Name[8];
extern const char movie1039BName[8];
extern const char movie1039Name[8];
extern const char resource1038BName[8];
extern const char resource1035Name[8];
extern const char resource1033Name[8];
extern const char resource1032Name[8];
extern const char resource1028Name[8];
extern const char resource1022Name[8];
extern const char resource1020AltName[8];
extern const char resource1019AltName[8];
extern const char resource1017Name[8];
extern const char resource1011AltName[8];
extern const char resource1010AltName[8];
extern const char resource1008AltName[8];
extern const char resource1006Name[8];
extern const char resource1005Name[8];
extern const char resource1004AltName[8];
extern const char resource1003B2Name[8];
extern const char resource10B01AltName[8];
extern const char resource1001AltName[8];
extern const char resource10OpName[8];

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
static void HddTestFormat(void);
static void HddTestShutdown(void);
static void HddTestHddFull(void);
static void HddTest1024Save(void);
static void HddTestDummyFolder(void);
static void HddTestDummySave(void);
static void HddTestMakeYS(void);
static void HddTestMountCommon(void);
static void HddTestMakeYourSaves(void);
static void HddTestUnmountCommon(void);

/* xtxdec_sleep (below) is still INCLUDE_ASM; declare it so xtxdec_error does
 * not see an implicit declaration. */
static int xtxdec_sleep(void);

/* xtxdec_sub is still INCLUDE_ASM; declare it here so XtxDecode does not
 * see an implicit declaration. */
static int xtxdec_sub(char *filename);

extern int sceRead(int descriptor, void *buffer, int bytes);
extern void *memcpy(void *destination, const void *source, unsigned int size);
extern int sceGsSyncPath(int mode, unsigned int timeout);

extern void sceGsSetDefLoadImage(void *image, short address, short width, short psm, short x,
                                 short y, short w, short h);

extern int sceGsExecLoadImage(void *image, void *source);

extern int sceGsSetDefStoreImage(void *image, short address, short width, short psm, short x,
                                 short y, short w, short h);

extern int sceGsExecStoreImage(void *image, void *destination);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

/* Only the packet handle at +0x00 is evidenced. */
extern void nmlModelDirectSend(int mode, u8 *data, int count);

extern int sceWrite(int descriptor, void *buffer, unsigned int size);
extern void xglRenderClearFrame(void);
extern void xglRenderClearColor(unsigned int color);
extern void xglRenderClearDepth(void);
extern void xglMenuInitial(void);
extern void xglMenuOpen(int mode, YajimaMenuState *menu);
extern void xglMenuDraw(void);
extern void xglClockRead(XglClock *clock);
extern float xglFRand(void);
static void FontTest(void);
static void UmlDispTest(void);
static void MemcardTest(void);
static void DatabaseTest(void);
static void XenoMovieCheck(void);
static void HddTest(void);
static void XtxDecode(void);
static void FontTestSub(void);
extern int xglCdGetFileSize(const char *name);
extern int xglFontLoad(int font, int mode);
extern void xglFontPrint(int x, int y, int color, const char *text);
extern void xglFontPrintDirect(const char *text);
extern int xglFontGetStringWidth(const char *text);
extern int xglFontGetProportionalSize(int character);
extern int GameSnapShotNumber(int number);
extern void GameSnapShotCheck(void);
extern void xglFontDebugHex(int x, int y, int value, int digits);
extern void FlushCache(int mode);
static void FontTestP0(void);
static void FontTestP1(void);
static void FontTestP2(void);
extern int xglFontGetSPcodeSize(int code, const char *text);
extern void tyaUmlDispMain(UmlDispParam *param);
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
extern void GameResourceInit(int, int);
extern int GameMpeg2Play(char *path, int mode);
extern int xglMcMain(void);
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

const char nisuiFontPath[24] = "data\\yajima\\nisui.euc";

const char scenarioTextPath[32] = "data\\yajima\\scenario.txt";

const char nisuiHostPath[16] = "host0:nisui.txt";

/* JP: 縮小文字のテストっす拡大菩薩魔法嵐闘\n\n菩薩魔法嵐闘\n改行テスト
 * EN: Small character test; enlarge; bodhisattva, magic, storm, battle\n\nbodhisattva, magic, storm, battle\nLine break test */
const char smallCharacterTestText[80] = "\r\003\022\014\014\275\314\276\256\312\270\273\372\244\316\245\306\245\271\245\310\244\303\244\271\022  \263\310\302\347\022\014\014\312\356\273\247\313\342\313\241\315\362\306\256\022\n\n\312\356\273\247\313\342\313\241\315\362\306\256\n\262\376\271\324\245\306\245\271\245\310";

/* JP: テスト（０）
 * EN: Test (0) */
const char test000Text[24] = "\r\002\016\004\004\000\000\000\245\306\245\271\245\310\241\312\243\260\241\313";

/* JP: テスト（１）
 * EN: Test (1) */
const char test001Text[16] = "\r\001\245\306\245\271\245\310\241\312\243\261\241\313";

/* JP: ええと──　ああ　マークⅡ㌢
 * EN: Um... ah... mark II, centi */
const char hesitationText[32] = "\244\250\244\250\244\310\250\241\250\241\241\241\244\242\244\242\241\241\245\336\241\274\245\257\255\266\255\302";

/* Text: {reset}{effect 2}{line height 12}{char width 8} */
const char fontTestEffectControl[16] = "\013\r\002\025\002\001\014\025\002\002\010";

const char characterSizeFormat[16] = "%3d:%04x\n";

const char umlDispFormat[16] = "UMLDISP %4x";

const char umnHeaderPath[24] = "data\\umn\\header.lst";

const char umnRecordFormat[16] = "%04d[%s][%s]\n";

const char fileSelectFormat[16] = "FILE SELECT\n%4d";

const char scrollPositionFormat[16] = "%4d,%4d,%3d,%6x";

const char databaseLabel[16] = "DATABASE";

const char movie3050C1_9Name[16] = "3050C1_9";

const char prologueMovieName[16] = "prologue";

/* JP: ムービープレーヤー\n\n\n%s\n\n上下で選択。\n\n○ボタンを押すと黒画面で待機します。\n\nその状態でさらに○ボタンで再生開始。\n\n再生中はゲーム中と同じ操作が可能。\n\n再生が終了するとまた黒画面で待機。\n\nそこで○ボタンを押すとこの画面に戻ります。
 * EN: Movie player\n\n\n%s\n\nUse up/down to select.\n\nPress the circle button to wait on a black screen.\n\nPress the circle button again to start playback.\n\nThe usual in-game controls are available during playback.\n\nAfter playback ends, wait on a black screen again.\n\nPress the circle button there to return to this screen. */
const char moviePlaybackHelpText[240] = "\013\025\002\001\010\245\340\241\274\245\323\241\274\245\327\245\354\241\274\245\344\241\274\n\n\n%s\n\n\276\345\262\274\244\307\301\252\302\362\241\243\n\n\241\373\245\334\245\277\245\363\244\362\262\241\244\271\244\310\271\365\262\350\314\314\244\307\302\324\265\241\244\267\244\336\244\271\241\243\n\n\244\275\244\316\276\365\302\326\244\307\244\265\244\351\244\313\241\373\245\334\245\277\245\363\244\307\272\306\300\270\263\253\273\317\241\243\n\n\272\306\300\270\303\346\244\317\245\262\241\274\245\340\303\346\244\310\306\261\244\270\301\340\272\356\244\254\262\304\307\275\241\243\n\n\272\306\300\270\244\254\275\252\316\273\244\271\244\353\244\310\244\336\244\277\271\365\262\350\314\314\244\307\302\324\265\241\241\243\n\n\244\275\244\263\244\307\241\373\245\334\245\277\245\363\244\362\262\241\244\271\244\310\244\263\244\316\262\350\314\314\244\313\314\341\244\352\244\336\244\271\241\243";

const char movieFilePathFormat[24] = "data\\movie\\mpeg2\\%s.pss";

static const char hdd_format_start_message[16] = "format HDD\n";

static const char hdd_format_result_format[16] = " result:%d\n";

static const char hdd_common_device[16] = "hdd0:__common";

static const char hdd_pfs_format_result_format[24] = " format __common:%d\n";

static const char hdd_shutdown_message[16] = "shutdown hdd\n";

static const char hdd_full_path[40] = "hdd0:PP.SLPS-99999.DUMMY.DUMMY,,,1G,PFS";

static const char hdd_create_partition_format[24] = "create partition:%d\n";

static const char hdd_subcommand_format[16] = "sub%d:%d\n";

static const char hdd_close_result_format[16] = "close:%d\n";

static const char hdd_mount_result_format[16] = "mount:%d\n";

static const char hdd_unmount_result_format[16] = "umount:%d\n";

static const char hdd_your_saves_directory[24] = "pfs1:/Your Saves";

static const char hdd_mkdir_result_format[24] = "make YourSaves:%d\n";

static const char hdd_zone_size_format[16] = "zonesz:%d\n";

static const char hdd_zone_free_format[16] = "zonefree:%d\n";

static const char hdd_chstat_failure_format[16] = "chstat:%d\n";

static const char zoneStatusFormat[16] = "zone:%d,%d\n";

static const char chunkStepFormat[16] = "step:%d\n";

static const char dummySaveFolderPath[16] = "pfs1:/999";

static const char mkdirStatusFormat[16] = "mkdir:%d\n";

static const char dummySaveFilePath[16] = "pfs1:/999/dummy";

static const char openHandleFormat[16] = "open:%d\n";

const char makeYourSavesLabel[16] = "MAKE YOURSAVES";

const char commonFullLabel[16] = "COMMON FULL";

const char dummyFolderLabel[16] = "DUMMY FOLDER";

const char save1024Label[16] = "1024 SAVE";

const char hddFullLabel[16] = "HDD FULL";

const char shutdownLabel[16] = "SHUTDOWN";

static const char hdd_status_format[24] = "HDIOC_STATUS:%d\n";

/* JP: HDDデバイスが存在しません:%d\n
 * EN: HDD device does not exist: %d\n */
static const char hdd_status_error_format[32] =
    "HDD\xA5\xC7\xA5\xD0\xA5\xA4\xA5\xB9\xA4\xAC\xC2\xB8\xBA\xDF\xA4\xB7\xA4\xDE\xA4\xBB\xA4\xF3:%d\n";

static const char hdd_free_format[24] = "xglHddMcGetFree:%d\n";

/* JP: ＨＤＤテスト
 * EN: HDD test */
static const char hdd_test_title[16] = "\x0B\xA3\xC8\xA3\xC4\xA3\xC4\xA5\xC6\xA5\xB9\xA5\xC8";

static const char hdd_no_use_title[16] = "HDD NoUse";

/* JP: ポート２
 * EN: Port 2 */
const char port2Label[16] = "\245\335\241\274\245\310\243\262";

/* JP: ポート１
 * EN: Port 1 */
const char port1Label[16] = "\245\335\241\274\245\310\243\261";

static YajimaMenuState m_54;

static void (*func_51[7])(void);

static XglMenuNode l1_53;

static char test00_26[51];

static char test01_27[51];

static char test02_28[51];

static char test03_29[51];

static char test04_30[51];

static char test05_31[51];

static char test06_32[51];

static char test07_33[56];

static int width_35;

static char test_36[6];

static char test0_37[40];

static char test1_38[19];

static char test2_39[20];

static char test3_40[36];

static char test4_41[35];

static int rate_2_004D9F60;

static int dir_3;

static int rate_8;

static int dir_9;

static unsigned char test_1[22];

static char test_4[16];

static char test_5[16];

static char test1_6[33];

static char test2_7[15];

static char test_10[13];

static unsigned char txt_11[15];

static unsigned char xy3_12[12];

static unsigned char xy5_13[28];

static unsigned char xy7_14[52];

static unsigned char xy9_15[84];

static char test_16[25];

static char test_17[43];

static char test_18[40];

static char test_19[31];

static char init_20[13];

static char test_21[33];

static char test_22[21];

static char test_23[33];

static char test_24[31];

static char test_25[26];

static UmlDispVertex TestEnv_42[6];

static char * name_43[103];

static unsigned char TestEnv_0_0036A030[0x90] = {
    0x01, 0x80, 0x00, 0x00, 0x00, 0x40, 0x8B, 0x80, 0xEE, 0x1E, 0x53, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xB8, 0x42, 0xA9, 0x06, 0x00, 0x07, 0x20, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00
};

/* JP: アルファ
 * EN: Alpha */
static unsigned char test_1[22] = "\017\000\000\000\r\002\016\004\004\000\000\000\245\242\245\353\245\325\245\241\013";

/* JP: 縁どり
 * EN: Outline */
static char test_4[16] = "\r\003\016\002\002\000\000\000\261\357\244\311\244\352\013";

/* JP: 縁どり
 * EN: Outline */
static char test_5[16] = "\r\003\016\001\001\000\000\000\261\357\244\311\244\352\013";

/* JP: ルビのテストヴＡＢＣ０１２
 * EN: Ruby annotation test: vu ABC012 */
static char test1_6[33] = "\r\003\030\001\245\353\245\323\244\316\245\306\245\271\245\310\245\364\243\301\243\302\243\303\243\260\243\261\243\262\030";

/* JP: 漢字との対比
 * EN: Comparison with kanji */
static char test2_7[15] = "\r\003\264\301\273\372\244\310\244\316\302\320\310\346";

/* Text: {effect 0}{effect params}{blend} */
static char test_10[13] = "\r\000\016\000\000\000\000\000\017D\000\200";

/* JP: 文字テスト
 * EN: Character test */
static unsigned char txt_11[15] = "\001\200\005\000\312\270\273\372\245\306\245\271\245\310";

static unsigned char xy3_12[12] = { 5, 0, 1, 0, 0, 1, 1, 1, 2, 1, 1, 2 };

static unsigned char xy5_13[28] = { 13, 0, 2, 0, 1, 1, 2, 1, 3, 1, 0, 2, 1, 2, 2, 2, 3, 2, 4, 2, 1, 3, 2, 3, 3, 3, 2, 4 };

static unsigned char xy7_14[52] = { 25, 0, 3, 0, 2, 1, 3, 1, 4, 1, 1, 2, 2, 2, 3, 2, 4, 2, 5, 2, 0, 3, 1, 3, 2, 3, 3, 3, 4, 3, 5, 3, 6, 3, 1, 4, 2, 4, 3, 4, 4, 4, 5, 4, 2, 5, 3, 5, 4, 5, 3, 6 };

static unsigned char xy9_15[84] = { 41, 0, 4, 0, 3, 1, 4, 1, 5, 1, 2, 2, 3, 2, 4, 2, 5, 2, 6, 2, 1, 3, 2, 3, 3, 3, 4, 3, 5, 3, 6, 3, 7, 3, 0, 4, 1, 4, 2, 4, 3, 4, 4, 4, 5, 4, 6, 4, 7, 4, 8, 4, 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 7, 5, 2, 6, 3, 6, 4, 6, 5, 6, 6, 6, 3, 7, 4, 7, 5, 7, 4, 8 };

/* JP: Ｎｏｒｍａｌ．０１２３
 * EN: Normal.0123 */
static char test_16[25] = "\031\000\243\316\243\357\243\362\243\355\243\341\243\354\241\245\243\260\243\261\243\262\243\263";

/* JP: Ｐｒｏｐｏｒｔｉｏｎａｌ．０１２３АВЕ
 * EN: Proportional.0123ABE */
static char test_17[43] = "\031\001\243\320\243\362\243\357\243\360\243\357\243\362\243\364\243\351\243\357\243\356\243\341\243\354\241\245\243\260\243\261\243\262\243\263\247\241\247\243\247\246";

/* JP: テストテスト漢字テスト漢字
 * EN: Test; test kanji; test kanji */
static char test_18[40] = "\r\000\245\306\245\271\245\310\r\004\245\306\245\271\245\310\264\301\273\372\r\003\016\001\001\200\200\200\245\306\245\271\245\310\264\301\273\372\013";

/* JP: ωАБВЯабвя─│┌╂\xAD\xA1\xAD\xA2
 * EN: Glyph test: Greek, Cyrillic and box-drawing characters */
static char test_19[31] = "\246\330\247\241\247\242\247\243\247\301\247\321\247\322\247\323\247\361\250\241\250\242\250\243\250\300\255\241\255\242";

/* Text: {effect 3}{effect params}{color #808080} */
static char init_20[13] = "\r\003\016\002\002\030\030\030\014\200\200\200";

/* Text: {proportional 0}{style 3}{char width 8}Half-width iiiiii test */
static char test_21[33] = "\025\002\004\000\031\003\025\002\002\010Half-width iiiiii test";

/* JP: 全角文字列と一緒に…
 * EN: Together with a full-width character string... */
static char test_22[21] = "\301\264\263\321\312\270\273\372\316\363\244\310\260\354\275\357\244\313\241\304";

/* Text: {proportional 1}{style 3}{char width 8}Half-width iiiiii test */
static char test_23[33] = "\025\002\004\001\031\003\025\002\002\010Half-width iiiiii test";

/* Text: {style 3}{char width 8}{shift 6}Itaric Text TEST{shift 0} */
static char test_24[31] = "\031\003\025\002\002\010\025\002\005\006Itaric Text TEST\025\002\005";

/* Text: {style 3}{char width 8}{shift 10}More Itaric{shift 0} */
static char test_25[26] = "\031\003\025\002\002\010\025\002\005\nMore Itaric\025\002\005";

/* Text: {style 2}#00#01#02#03#04#05#06#07#08#09#0a#0b#0c#0d#0e#0f */
static char test00_26[51] = "\031\002#00#01#02#03#04#05#06#07#08#09#0a#0b#0c#0d#0e#0f";

/* Text: {style 2}#10#11#12#13#14#15#16#17#18#19#1a#1b#1c#1d#1e#1f */
static char test01_27[51] = "\031\002#10#11#12#13#14#15#16#17#18#19#1a#1b#1c#1d#1e#1f";

/* Text: {style 2}#20#21#22#23#24#25#26#27#28#29#2a#2b#2c#2d#2e#2f */
static char test02_28[51] = "\031\002#20#21#22#23#24#25#26#27#28#29#2a#2b#2c#2d#2e#2f";

/* Text: {style 2}#30#31#32#33#34#35#36#37#38#39#3a#3b#3c#3d#3e#3f */
static char test03_29[51] = "\031\002#30#31#32#33#34#35#36#37#38#39#3a#3b#3c#3d#3e#3f";

/* Text: {style 2}#40#41#42#43#44#45#46#47#48#49#4a#4b#4c#4d#4e#4f */
static char test04_30[51] = "\031\002#40#41#42#43#44#45#46#47#48#49#4a#4b#4c#4d#4e#4f";

/* Text: {style 2}#50#51#52#53#54#55#56#57#58#59#5a#5b#5c#5d#5e#5f */
static char test05_31[51] = "\031\002#50#51#52#53#54#55#56#57#58#59#5a#5b#5c#5d#5e#5f";

/* Text: {style 2}#60#61#62#63#64#65#66#67#68#69#6a#6b#6c#6d#6e#6f */
static char test06_32[51] = "\031\002#60#61#62#63#64#65#66#67#68#69#6a#6b#6c#6d#6e#6f";

/* Text: {style 2}#70#71#72#73#74#75#76#77#78#79#7a#7b#7c#7d#7e#7f###00 */
static char test07_33[56] = "\031\002#70#71#72#73#74#75#76#77#78#79#7a#7b#7c#7d#7e#7f###00";

static TestLineVertex TestEnv_34[6] = {
    { 0, 0, { 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x51 } },
    { 32769, 1073823744, { 0x1E, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 458752, 0, { 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 192, 128, { 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00 } },
    { 0, 0, { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 } },
    { 0, 0, { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 } }
};

/* Text: {style 3}Proportional font auto linefeed test\n */
static char test0_37[40] = "\031\003Proportional font auto linefeed test\n";

/* Text: {style 3}!"#$%&'()*+,-./\n */
static char test1_38[19] = "\031\003!\"#$%&'()*+,-./\n";

/* Text: {style 3}0123456789:;<=>?\n */
static char test2_39[20] = "\031\0030123456789:;<=>\?\n";

/* Text: {style 3}@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_\n */
static char test3_40[36] = "\031\003@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n";

/* Text: {style 3}`abcdefghijklmnopqrstuvwxyz{|}~\n */
static char test4_41[35] = "\031\003`abcdefghijklmnopqrstuvwxyz{|}~\n";

static UmlDispVertex TestEnv_42[6] __attribute__((aligned(16))) = { { .x = 32769, .y = 1342390272, .z = 348654, .w = 0 }, { .x = 458752, .y = 0, .z = 71, .w = 0 }, { .x = 68, .y = 0, .z = 66, .w = 0 }, { .x = 0, .y = 0, .z = 0, .w = 0 }, { .x = 0, .y = 0, .z = 0, .w = 0 }, { .x = 0, .y = 0, .z = 0, .w = 0 } };

static char * name_43[103] = { (char *)prologueMovieName, (char *)resource10OpName, (char *)resource1001AltName, (char *)resource10B01AltName, (char *)resource1003B2Name, (char *)resource1004AltName, (char *)resource1005Name, (char *)resource1006Name, (char *)resource1008AltName, (char *)resource1010AltName, (char *)resource1011AltName, (char *)resource1017Name, (char *)resource1019AltName, (char *)resource1020AltName, (char *)resource1022Name, (char *)resource1028Name, (char *)resource1032Name, (char *)resource1033Name, (char *)resource1035Name, (char *)resource1038BName, (char *)movie1039Name, (char *)movie1039BName, (char *)movie1040Name, (char *)movie1041_1Name, (char *)movie1043_1Name, (char *)movie1044BName, (char *)movie2000_1Name, (char *)movie2001A_1Name, (char *)movie2002B_1Name, (char *)movie2004C_1Name, (char *)movie2004D_2Name, (char *)movie2006_1Name, (char *)movie2007Name, (char *)movie2007BName, (char *)movie2007C_1Name, (char *)movie2007C_2Name, (char *)movie2010Name, (char *)movie2012BName, (char *)movie2015Name, (char *)movie2017Name, (char *)movie2018Name, (char *)movie2031_1Name, (char *)movie2031_2Name, (char *)movie2032Name, (char *)movie2033AName, (char *)movie2033BName, (char *)movie2034Name, (char *)movie2044AName, (char *)movie2046Name, (char *)movie2048Name, (char *)movie2051Name, (char *)movie2053Name, (char *)movie2055Name, (char *)movie2056AName, (char *)movie2057Name, (char *)movie2058AName, (char *)movie2064_1Name, (char *)movie2064_2Name, (char *)movie3000Name, (char *)movie3003AName, (char *)movie3003EName, (char *)movie3007Name, (char *)movie3008AName, (char *)movie3008BName, (char *)movie3009DName, (char *)movie3010Name, (char *)movie3011Name, (char *)movie3014_1Name, (char *)movie3015Name, (char *)movie3016_1Name, (char *)movie3017_1Name, (char *)movie3020Name, (char *)movie3021_1Name, (char *)movie3022Name, (char *)movie3026AName, (char *)movie3027_1Name, (char *)movie3028BName, (char *)movie3035Name, (char *)movie3036IName, (char *)movie3037Name, (char *)movie3039AName, (char *)movie3039B_1Name, (char *)movie3039CName, (char *)movie3040Name, (char *)movie3042Name, (char *)movie3045Name, (char *)movie3046AName, (char *)movie3046BName, (char *)movie3047BName, (char *)movie3047CName, (char *)movie3047DName, (char *)movie3048Name, (char *)movie3050C1_9Name, (char *)movie3050C2Name, (char *)movie3052Name, (char *)movie3054B2Name, (char *)movie3055_1Name, (char *)movie3056AName, (char *)movie3056BName, (char *)movie3056CName, (char *)movie3058AName, (char *)movie3058BName, (char *)movie3058DName };

static char hdd_1024_directory_name[22] = "pfs1:/Your Saves/0000";

static char hdd_dummy_folder_name[10] = "pfs1:/000";

static HddTestMenuEntry hdd_test_menu[8] = {
    { hddFormatLabel, HddTestFormat },
    { shutdownLabel, HddTestShutdown },
    { hddFullLabel, HddTestHddFull },
    { save1024Label, HddTest1024Save },
    { dummyFolderLabel, HddTestDummyFolder },
    { commonFullLabel, HddTestDummySave },
    { makeYourSavesLabel, HddTestMakeYS },
    { 0, 0 },
};

static char *name_47[3] = { port1Label, port2Label, hddText };

static void (*func_51[7])(void) = {
    FontTest, UmlDispTest, HddTest, MemcardTest,
    DatabaseTest, XenoMovieCheck, XtxDecode,
};

static XglMenuRow i1_52[7] = {
    { fontLabel, XGL_MENU_ACTION(1) },
    { umlDispLabel, XGL_MENU_ACTION(2) },
    { hddLabel, XGL_MENU_ACTION(3) },
    { memcardLabel, XGL_MENU_ACTION(4) },
    { databaseLabel, XGL_MENU_ACTION(5) },
    { movieLabel, XGL_MENU_ACTION(6) },
    { xtxDecodeLabel, XGL_MENU_ACTION(7) }
};

static XglMenuNode l1_53 = { 7, { 255, 1, 8 }, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, i1_52 };

static YajimaMenuState m_54 = {
    { 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF },
    &l1_53,
    0
};

static int rate_2_004D9F60 = 128;

static int dir_3 = -1;

static int rate_8 = 128;

static int dir_9 = -1;

static int width_35 = 256;

char fontTestWidthFormat[8] = "%3d";

/* Text: {spacing 1,0} */
static char test_36[6] = "\025\003\003\001";

/* Text: {spacing 0,0} */
char fontTestSpacingControl[8] = "\025\003\003";

char umlBitsFormat[8] = "%1x %1x";

char umnModeSoundName[8] = "Umnmode";

char databaseIndexFormat[8] = "%3d/%3d";

const char movie3058DName[8] = "3058D";

const char movie3058BName[8] = "3058B";

const char movie3058AName[8] = "3058A";

const char movie3056CName[8] = "3056C";

const char movie3056BName[8] = "3056B";

const char movie3056AName[8] = "3056A";

const char movie3055_1Name[8] = "3055_1";

const char movie3054B2Name[8] = "3054B2";

const char movie3052Name[8] = "3052";

const char movie3050C2Name[8] = "3050C2";

const char movie3048Name[8] = "3048";

const char movie3047DName[8] = "3047D";

const char movie3047CName[8] = "3047C";

const char movie3047BName[8] = "3047B";

const char movie3046BName[8] = "3046B";

const char movie3046AName[8] = "3046A";

const char movie3045Name[8] = "3045";

const char movie3042Name[8] = "3042";

const char movie3040Name[8] = "3040";

const char movie3039CName[8] = "3039C";

const char movie3039B_1Name[8] = "3039B_1";

const char movie3039AName[8] = "3039A";

const char movie3037Name[8] = "3037";

const char movie3036IName[8] = "3036I";

const char movie3035Name[8] = "3035";

const char movie3028BName[8] = "3028B";

const char movie3027_1Name[8] = "3027_1";

const char movie3026AName[8] = "3026A";

const char movie3022Name[8] = "3022";

const char movie3021_1Name[8] = "3021_1";

const char movie3020Name[8] = "3020";

const char movie3017_1Name[8] = "3017_1";

const char movie3016_1Name[8] = "3016_1";

const char movie3015Name[8] = "3015";

const char movie3014_1Name[8] = "3014_1";

const char movie3011Name[8] = "3011";

const char movie3010Name[8] = "3010";

const char movie3009DName[8] = "3009D";

const char movie3008BName[8] = "3008B";

const char movie3008AName[8] = "3008A";

const char movie3007Name[8] = "3007";

const char movie3003EName[8] = "3003E";

const char movie3003AName[8] = "3003A";

const char movie3000Name[8] = "3000";

const char movie2064_2Name[8] = "2064_2";

const char movie2064_1Name[8] = "2064_1";

const char movie2058AName[8] = "2058A";

const char movie2057Name[8] = "2057";

const char movie2056AName[8] = "2056A";

const char movie2055Name[8] = "2055";

const char movie2053Name[8] = "2053";

const char movie2051Name[8] = "2051";

const char movie2048Name[8] = "2048";

const char movie2046Name[8] = "2046";

const char movie2044AName[8] = "2044A";

const char movie2034Name[8] = "2034";

const char movie2033BName[8] = "2033B";

const char movie2033AName[8] = "2033A";

const char movie2032Name[8] = "2032";

const char movie2031_2Name[8] = "2031_2";

const char movie2031_1Name[8] = "2031_1";

const char movie2018Name[8] = "2018";

const char movie2017Name[8] = "2017";

const char movie2015Name[8] = "2015";

const char movie2012BName[8] = "2012B";

const char movie2010Name[8] = "2010";

const char movie2007C_2Name[8] = "2007C_2";

const char movie2007C_1Name[8] = "2007C_1";

const char movie2007BName[8] = "2007B";

const char movie2007Name[8] = "2007";

const char movie2006_1Name[8] = "2006_1";

const char movie2004D_2Name[8] = "2004D_2";

const char movie2004C_1Name[8] = "2004C_1";

const char movie2002B_1Name[8] = "2002B_1";

const char movie2001A_1Name[8] = "2001A_1";

const char movie2000_1Name[8] = "2000_1";

const char movie1044BName[8] = "1044B";

const char movie1043_1Name[8] = "1043_1";

const char movie1041_1Name[8] = "1041_1";

const char movie1040Name[8] = "1040";

const char movie1039BName[8] = "1039B";

const char movie1039Name[8] = "1039";

const char resource1038BName[8] = "1038B";

const char resource1035Name[8] = "1035";

const char resource1033Name[8] = "1033";

const char resource1032Name[8] = "1032";

const char resource1028Name[8] = "1028";

const char resource1022Name[8] = "1022";

const char resource1020AltName[8] = "1020_1";

const char resource1019AltName[8] = "1019_1";

const char resource1017Name[8] = "1017";

const char resource1011AltName[8] = "1011_2";

const char resource1010AltName[8] = "1010_1";

const char resource1008AltName[8] = "1008_2";

const char resource1006Name[8] = "1006";

const char resource1005Name[8] = "1005";

const char resource1004AltName[8] = "1004_1";

const char resource1003B2Name[8] = "1003B2";

const char resource10B01AltName[8] = "10B01_1";

const char resource1001AltName[8] = "1001_1";

const char resource10OpName[8] = "10OP";

static char hdd_device[8] = "hdd0:";

static char pfs_device_name[8] = "pfs:";

static char hdd_device_name[8] = "hdd:";

static char hdd_partition_size[8] = "1G";

static char hdd_mount_point[8] = "pfs1:";

static char fileLineFormat[8] = "%s:%d\n";

static char counterWroteFormat[8] = "%d:%d\n";

char hddFormatLabel[8] = "FORMAT";

static char hdd_use_title[8] = "HDD Use";

static char menu_cursor[8] = ">";

/* JP: ＨＤＤ
 * EN: HDD */
char hddText[8] = "\243\310\243\304\243\304";

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static char h_50[7];

static unsigned char tbl_49[4];

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

    if (xglCdReadFile(nisuiFontPath, table, 0, 0) > 0) {
        text = (unsigned char *)0x01000100;
        size = xglCdGetFileSize(scenarioTextPath);
        xglCdReadFile(scenarioTextPath, text, 0, 0);
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
        handle = sceOpen(nisuiHostPath, 0x602);
        sceWrite(handle, table, entry - table);
        sceClose(handle);
    }
}

static void FontTestP0(void)
{
    int i;
    int offset;
    unsigned char *table;

    if (PAD_U16_AT(42) & 0x20) {
        xglFontLoad(0, 0);
    }
    if (PAD_U16_AT(42) & 0x40) {
        xglFontLoad(1, 0);
    }
    if (PAD_U16_AT(42) & 0x10) {
        FontTestSub();
    }
    xglFontDebugPrintf(16, 16, smallCharacterTestText);
    xglFontDebugPrintf(16, 32, test000Text);
    xglFontDebugPrintf(16, 48, test001Text);
    rate_2_004D9F60 += dir_3;
    if (rate_2_004D9F60 <= 0) {
        dir_3 = 1;
    }
    if (rate_2_004D9F60 >= 128) {
        dir_3 = -1;
    }
    test_1[3] = rate_2_004D9F60;
    test_1[1] = 68;
    test_1[2] = 0;
    FlushCache(0);
    xglFontDebugPrintf(16, 64, (char *)test_1);
    xglFontDebugHex(0, 64, rate_2_004D9F60, 2);
    xglFontDebugPrintf(48, 80, test_4);
    xglFontDebugPrintf(16, 80, test_5);
    xglFontDebugPrintf(20, 96, test1_6);
    xglFontDebugPrintf(16, 99, test2_7);
    xglFontPrint(0, 0, 0xFFFFFF, test_10);
    txt_11[1] = 32;
    txt_11[3] = 0;
    xglFontPrint(0, 0, 0xFFFFFF, (char *)txt_11);
    table = xy3_12;
    for (i = 0, offset = 2; i < table[0]; i++, offset += 2) {
        txt_11[1] = table[offset] + 32;
        txt_11[3] = table[offset + 1] + 32;
        FlushCache(0);
        xglFontPrint(0, 0, 0xFFFFFF, (char *)txt_11);
    }
    table = xy5_13;
    for (i = 0, offset = 2; i < table[0]; i++, offset += 2) {
        txt_11[1] = table[offset] + 32;
        txt_11[3] = table[offset + 1] + 64;
        FlushCache(0);
        xglFontPrint(0, 0, 0xFFFFFF, (char *)txt_11);
    }
    table = xy7_14;
    for (i = 0, offset = 2; i < table[0]; i++, offset += 2) {
        txt_11[1] = table[offset] + 32;
        txt_11[3] = table[offset + 1] + 96;
        FlushCache(0);
        xglFontPrint(0, 0, 0xFFFFFF, (char *)txt_11);
    }
    table = xy9_15;
    for (i = 0, offset = 2; i < table[0]; i++, offset += 2) {
        txt_11[1] = table[offset] + 32;
        txt_11[3] = table[offset + 1] + 128;
        FlushCache(0);
        xglFontPrint(0, 0, 0xFFFFFF, (char *)txt_11);
    }
    rate_8 += dir_9;
    if (rate_8 < 2) {
        dir_9 = 1;
    }
    if (rate_8 >= 127) {
        dir_9 = -1;
    }
    xglFontDebugHex(0, 128, rate_8, 2);
    xglFontDebugPrintf(96, 32, test_16);
    xglFontDebugPrintf(96, 48, test_17);
    xglFontDebugPrintf(96, 64, test_18);
    xglFontDebugPrintf(96, 80, test_19);
    xglFontPrint(0, 0, 0x10000, init_20);
    xglFontPrint(192, 192, 0x10000, hesitationText);
    xglFontDebugPrintf(96, 112, test_21);
    xglFontDebugPrintf(96, 128, test_22);
    xglFontDebugPrintf(96, 144, test_23);
    xglFontDebugPrintf(96, 160, test_24);
    xglFontDebugPrintf(96, 176, test_25);
}

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

    xglFontDebugPrintf(8, 8, fontTestWidthFormat, width_35);
    if (PAD_U16_AT(148) & 0x2000) {
        width_35++;
    }
    if (PAD_U16_AT(148) & 0x8000) {
        width_35--;
    }
    xglFontPrint(64, 96, 0xFFFFFF, fontTestEffectControl);
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
    xglFontPrint(64, 96, 0xFFFFFF, fontTestSpacingControl);
    if (PAD_U16_AT(42) & 0x20) {
        for (character = 0; character < 64; character++) {
            printf(characterSizeFormat, character, xglFontGetProportionalSize(character));
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
        xglFontDebugPrintf(0, 0, umlDispFormat, number);
        if (detail == 0) {
            if (PAD_U16_AT(42) & 0x10) {
                record = (UmlHeaderRecord *)0x1000000;
                xglCdReadFile(umnHeaderPath, record, 0, 0);
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
                        printf(umnRecordFormat, id, record->text, text + 1);
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
            xglFontDebugPrintf(16, 40, umlBitsFormat, (save->umn_flags >> 4) & 1,
                               (save->umn_flags >> 5) & 1);
            xglFontDebugPrintf(16, 16, fileSelectFormat, number);
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
            xglFontDebugPrintf(0, 8, scrollPositionFormat, param.scroll_x, param.scroll_y,
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
    xglSoundLoadRequestSmd(umnModeSoundName, (void *)0x1000000);
    xglSleep();
    while ((PAD_U64_AT(40) & 0x08000100ULL) != 0x08000100ULL) {
        xglFontDebugPrintf(0, 0, databaseLabel);
        if (tyaUmlDatabaseMain(&param) != 0) {
            break;
        }
        xglFontDebugPrintf(80, 0, databaseIndexFormat, param.index, param.total);
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
        xglFontDebugPrintf(0, 0, moviePlaybackHelpText, name_43[index]);
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
            sprintf(path, movieFilePathFormat, name_43[index]);
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
    printf(zoneStatusFormat, remaining, zone_size);
    printf(chunkStepFormat, chunk_size);
    mkdir_status = sceMkdir(dummySaveFolderPath, 0x1ff);
    printf(mkdirStatusFormat, mkdir_status);
    handle = sceOpen(dummySaveFilePath, 0x602, 0x1ff);
    printf(openHandleFormat, handle);

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
        printf(counterWroteFormat, counter, wrote);
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
                /* Text: {color #202020}%s */
                xglFontDebugPrintf(16, 48, "\x0C   %s", name);
            }
            name = mt_GetPortName(1);
            if (port_status[1] == 1) {
                xglFontDebugPrintf(16, 64, "%s", name);
            } else {
                /* Text: {color #202020}%s */
                xglFontDebugPrintf(16, 64, "\x0C   %s", name);
            }
            name = mt_GetPortName(2);
            if (hdd == 0) {
                xglFontDebugPrintf(16, 80, "%s", name);
            } else {
                /* Text: {color #202020}%s */
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
                static unsigned char tbl_48[4] = { 32, 48, 64, 80 };

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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char xtxListPathFormat[16] = "host0:%s.lst";

const char xtxReadFormat[16] = "%d[%s]%d\n";

const char xtxFilePathFormat[16] = "host0:%s.xtx";

const char xtxTbwFormat[16] = "tbw:%d:%08x\n";

const char xtxEntryFormat[40] = "[%s],%d,(%d,%d)-(%d,%d),(%d,%d)\n";

static unsigned char tbl_49[4] = { 112, 128, 0, 0 };

static char h_50[7] = "host0:";

const char movieLabel[8] = "MOVIE";

const char memcardLabel[8] = "MEMCARD";

const char hddLabel[8] = "HDD";

const char umlDispLabel[8] = "UMLDISP";

const char fontLabel[8] = "FONT";

static char selectText[8] = "SELECT";

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

static int xtxdec_sub(char *filename)
{
    char path[256];
    unsigned char load_image[96];
    unsigned char store_image[112];
    int bits_per_pixel;
    int left;
    int top;
    int width;
    int height;
    int origin_x;
    int origin_y;
    int palette_size;
    int count;
    int tbw;
    int i;
    int j;
    int fd;
    int read_size;
    unsigned int dimensions;
    int pixel_width;
    char *line;
    char *src_name;
    char *dst;
    unsigned char *out;
    unsigned char *vram_dst;
    XtxImageEntry *entry;
    XtxHeader *header;
    unsigned char *bmp;
    unsigned char *palette;
    int status;

    tbw = 64;
    bmp = (unsigned char *)0x01410000;
    sprintf(path, xtxListPathFormat, filename);
    memset((void *)0x01000000, 0, 0x10000);
    fd = sceOpen(path, 1);
    read_size = sceRead(fd, (void *)0x01000000, 0x10000);
    printf(xtxReadFormat, fd, path, read_size);
    sceClose(fd);
    if (((XtxHeader *)0x01000000)->magic[0] != 0x58455423 ||
        ((XtxHeader *)0x01000000)->magic[1] != 0x314C5455)
        return -1;
    line = xtxdec_nextline((char *)0x01000000);
    if (line == 0)
        return -1;
    line = xtxdec_nextline(line);
    if (line == 0)
        return -1;
    sprintf(path, xtxFilePathFormat, filename);
    memset((void *)0x01010000, 0, 256);
    fd = sceOpen(path, 1);
    count = 0;
    read_size = sceRead(fd, (void *)0x01010000, 0x400000);
    printf(xtxReadFormat, fd, path, read_size);
    sceClose(fd);
    xglSleep();
    sceGsSyncPath(0, 0);
    header = (XtxHeader *)0x01010000;
    entry = (XtxImageEntry *)((char *)header + header->table_offset);
    for (; count < header->image_count; count++) {
        dimensions = entry->dimensions;
        pixel_width = dimensions & 0xFFFF;
        if (dimensions == pixel_width)
            tbw = (pixel_width + 63) >> 6;
        else
            tbw = dimensions >> 16;
        printf(xtxTbwFormat, tbw, entry->vram_address);
        sceGsSetDefLoadImage(load_image, (entry->vram_address + 0xE0000) >> 6, tbw, 0, 0, 0,
                             pixel_width, entry->height);
        FlushCache(0);
        sceGsExecLoadImage(load_image, (void *)(0x01010020 + entry->data_offset));
        entry++;
    }
    while (line != 0) {
        dst = path;
        for (src_name = h_50; *src_name != 0; src_name++)
            *dst++ = (unsigned char)*src_name;
        if (*line != '"')
            return -1;
        line++;
        dst = path + 6;
        while (*line != '"') {
            *dst++ = *line;
            if (*line == '\\')
                dst = path + 6;
            line++;
        }
        *dst = 0;
        out = (unsigned char *)0x01410036;
        status = 0;
        line = xtxdec_getdec(line + 2, 0);
        line = xtxdec_getdec(line, &bits_per_pixel);
        line = xtxdec_getdec(line, 0);
        line = xtxdec_getdec(line, &left);
        line = xtxdec_getdec(line, &top);
        line = xtxdec_getdec(line, &width);
        line = xtxdec_getdec(line, &height);
        line = xtxdec_getdec(line, 0);
        line = xtxdec_getdec(line, &origin_x);
        line = xtxdec_getdec(line, &origin_y);
        printf(xtxEntryFormat, path, bits_per_pixel, left, top, width, height, origin_x, origin_y);
        switch (bits_per_pixel) {
        default:
            palette_size = 0;
            break;
        case 4:
            sceGsSetDefStoreImage(store_image, 0x3800, tbw, 0, origin_x, origin_y, 8, 2);
            FlushCache(0);
            sceGsExecStoreImage(store_image, (void *)0x01810000);
            palette_size = 64;
            break;
        case 8:
            vram_dst = (unsigned char *)0x01810000;
            for (i = 0; i < 16; i += 2) {
                for (j = 0; j < 16; j += 8) {
                    sceGsSetDefStoreImage(store_image, 0x3800, tbw, 0, origin_x + j,
                                          origin_y + i, 8, 2);
                    FlushCache(0);
                    sceGsExecStoreImage(store_image, vram_dst);
                    vram_dst += 64;
                }
            }
            palette_size = 1024;
            break;
        }
        palette = (unsigned char *)0x01810000;
        for (i = 0; i < palette_size; i += 4) {
            out[0] = palette[i + 2];
            out[1] = palette[i + 1];
            out[2] = palette[i];
            out[3] = 0;
            out += 4;
        }
        for (i = height - 1; i >= 0; i--) {
            j = 0;
            switch (bits_per_pixel) {
            case 4: break;
            case 8:
                sceGsSetDefStoreImage(store_image, 0x3800, tbw * 2, 19, left, top + i, width, 1);
                j = width;
                break;
            case 16: case 24: case 32: break;
            }
            if (out != 0) {
                FlushCache(0);
                sceGsExecStoreImage(store_image, (void *)0x01810000);
                memcpy(out, (void *)0x01810000, j);
                out += j;
            }
        }
        i = out - bmp;
        out = (unsigned char *)0x0141000E;
        vram_dst = (unsigned char *)0x01410000;
        *vram_dst++ = 'B';
        *vram_dst++ = 'M';
        xtxdec_put4byte((LittleEndianWord *)0x01410002, i);
        xtxdec_put4byte((LittleEndianWord *)0x01410006, 0);
        xtxdec_put4byte((LittleEndianWord *)0x0141000A, palette_size + 54);
        xtxdec_put4byte((LittleEndianWord *)out, 40);
        xtxdec_put4byte((LittleEndianWord *)0x01410012, width);
        xtxdec_put4byte((LittleEndianWord *)0x01410016, height);
        out[12] = 1;
        out[13] = 0;
        out[14] = bits_per_pixel;
        out[15] = 0;
        xtxdec_put4byte((LittleEndianWord *)0x0141001E, 0);
        xtxdec_put4byte((LittleEndianWord *)0x01410022, 0);
        xtxdec_put4byte((LittleEndianWord *)0x01410026, 0);
        xtxdec_put4byte((LittleEndianWord *)0x0141002A, 0);
        xtxdec_put4byte((LittleEndianWord *)0x0141002E, 0);
        xtxdec_put4byte((LittleEndianWord *)0x01410032, 0);
        fd = sceOpen(path, 0x602);
        sceWrite(fd, (void *)0x01410000, i);
        sceClose(fd);
        line = xtxdec_nextline(line);
    }
    return status;
}

static int xtxdec_sleep(void)
{
    if ((PAD_U64_AT(40) & 0x08000100ULL) == 0x08000100ULL) {
        return 1;
    }
    xglFontDebugPrintf(0, 0, xtxDecodeText);
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
        xtxdec_error(16, specifyFileNameText);
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
    xtxdec_error(y, xtxFinishedText);
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
            xglFontDebugPrintf(0, 0, selectText);
            xglMenuDraw();
            xglClockRead(&clock);
            xglFontDebugPrintf(0x80, 0x10, clockDateTimeFormat, clock.year, clock.month,
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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: ＸＴＸデコード
 * EN: XTX decode */
static const char xtxDecodeText[16] = "\x0B\xA3\xD8\xA3\xD4\xA3\xD8\xA5\xC7\xA5\xB3\xA1\xBC\xA5\xC9";

/* JP: 起動時オプションでファイル名を指定してください
 * EN: Please specify a filename in the startup options. */
static const char specifyFileNameText[48] = "\x0B\xB5\xAF\xC6\xB0\xBB\xFE\xA5\xAA\xA5\xD7\xA5\xB7\xA5\xE7\xA5\xF3\xA4\xC7\xA5\xD5\xA5\xA1\xA5\xA4\xA5\xEB\xCC\xBE\xA4\xF2\xBB\xD8\xC4\xEA\xA4\xB7\xA4\xC6\xA4\xAF\xA4\xC0\xA4\xB5\xA4\xA4";

/* JP: 終了しました
 * EN: Finished */
static const char xtxFinishedText[16] = "\xBD\xAA\xCE\xBB\xA4\xB7\xA4\xDE\xA4\xB7\xA4\xBF";

const char xtxDecodeLabel[16] = "XTX DECODE";

static const char clockDateTimeFormat[24] = "%4d.%2d.%2d\n%2d.%2d.%2d";
