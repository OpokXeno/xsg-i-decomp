#include "common.h"

/* Points at the slot digit inside the snapshot file name (snapname + 0x1F). */
static char *snapnameno;

int GameSnapShotNumber(int number) {
    if (number >= 0) {
        *snapnameno = number + '0';
    }
    return *snapnameno - '0';
}

void GameSnapShotCheck(void) {
}

#include "main/xgl_cd.h"
enum {
    SNAPSHOT_OPEN_FLAGS = 0x602,
    SNAPSHOT_CACHE_LINE_SIZE = 64,
    SNAPSHOT_WIDTH = 512,
    SNAPSHOT_HEIGHT = 448,
    SNAPSHOT_METADATA_SIZE = 160,
    SNAPSHOT_CHANNEL_LUMA = 0,
    SNAPSHOT_CHANNEL_CB = 1,
    SNAPSHOT_CHANNEL_CR = 2,
};
typedef struct {
    unsigned char quality;
    unsigned char unmodeled_01[3];
} GameSnapShotQualityChannel;
typedef struct {
    GameSnapShotQualityChannel channel[3];
    unsigned char unmodeled_0c[4];
} GameSnapShotQuality;
typedef struct {
    unsigned char luma_quality;
    unsigned char cb_quality;
    unsigned char cr_quality;
    unsigned char unmodeled_03[5];
} GameSnapShotQualitySettings;
typedef struct {
    unsigned int unmodeled_00;
    unsigned int output_buffer;
    unsigned int output_size;
    short source_x;
    short source_y;
    short destination_x;
    short destination_y;
    short width;
    short height;
    GameSnapShotQualitySettings quality;
} GameSnapShotJpegParams;
typedef struct {
    unsigned char unmodeled_00[0x10];
    unsigned long long display_origin;
} GameSnapShotDrawEnv;
typedef struct {
    XglClock clock_reading;
    XglClock game_time;
    int game_state_value;
    unsigned char unmodeled_14[SNAPSHOT_METADATA_SIZE - 20];
} GameSnapShotMetadata;
typedef struct {
    unsigned char unmodeled_00[0x50];
    short game_state_value;
} GameSnapShotGameState;

static char snapname[40] = "host0:/home/xeno/work/snap/game0000.jpg";
static GameSnapShotQuality quality_table[5] = {
    { { { 100, { 0 } }, { 100, { 0 } }, { 0, { 0 } } }, { 0 } },
    { { { 90, { 0 } }, { 90, { 0 } }, { 0, { 0 } } }, { 0 } },
    { { { 75, { 0 } }, { 75, { 0 } }, { 0, { 0 } } }, { 0 } },
    { { { 50, { 0 } }, { 50, { 0 } }, { 0, { 0 } } }, { 0 } },
    { { { 30, { 0 } }, { 30, { 0 } }, { 0, { 0 } } }, { 0 } }
};
static char *snapnameno = snapname + 31;
static char *snapnamext = snapname + 36;
char GameMovieTransparent = 0;
char GameMovieAlpha = 0;
short GameMovieFrame = 0;
const char D_004C01C8[] = "*** SnapShot:%s,%08x ***\n";
const char D_004C01E8[] = "%d,%d,%d\n";

extern GameSnapShotDrawEnv DrawEnv;
extern GameSnapShotGameState GameLoopState;
extern int sceOpen(const char *path, int flags, ...);
extern int sceClose(int descriptor);
extern int sceWrite(int descriptor, const void *buffer, unsigned int size);
extern void xglClockUInt2DayTime(XglClock *clock, unsigned int seconds);
extern void xglJpegEncode(GameSnapShotJpegParams *params);
extern void *memset(void *destination, int value, unsigned int size);
extern int printf(const char *format, ...);

void GameSnapShotExecute(int quality, int flags) {
    GameSnapShotJpegParams params;
    unsigned int buffer_address;
    unsigned long long display_origin;
    unsigned int source_x;

    memset(&params, 0, sizeof(params));
    buffer_address = ((unsigned int) WorkEnd + SNAPSHOT_CACHE_LINE_SIZE - 1) &
                     ~(SNAPSHOT_CACHE_LINE_SIZE - 1);
    params.output_buffer = buffer_address;
    printf(D_004C01C8, snapname, buffer_address);

    display_origin = DrawEnv.display_origin;
    source_x = display_origin & 0x1ff;
    params.source_x = source_x << 5;
    params.source_y = (short) ((display_origin >> 16) & 0x3f);
    params.width = SNAPSHOT_WIDTH;
    params.height = SNAPSHOT_HEIGHT;
    params.destination_x = 0;
    params.destination_y = 0;

    if (quality < 0) {
        params.quality.luma_quality = quality_table[~quality].channel[SNAPSHOT_CHANNEL_LUMA].quality;
        params.quality.cb_quality = quality_table[~quality].channel[SNAPSHOT_CHANNEL_CB].quality;
        params.quality.cr_quality = quality_table[~quality].channel[SNAPSHOT_CHANNEL_CR].quality;
    } else {
        params.quality.luma_quality = quality;
        params.quality.cb_quality = quality;
        params.quality.cr_quality = 0;
    }

    printf(D_004C01E8, params.quality.luma_quality,
           params.quality.cb_quality, params.quality.cr_quality);
    xglJpegEncode(&params);

    {
        int image_descriptor = sceOpen(snapname, SNAPSHOT_OPEN_FLAGS);
        sceWrite(image_descriptor, (const void *) params.output_buffer, params.output_size);
        sceClose(image_descriptor);
    }

    if (flags & 1) {
        GameSnapShotMetadata metadata;
        int metadata_descriptor;

        snapnamext[0] = 'b';
        snapnamext[1] = 'i';
        snapnamext[2] = 'n';
        memset(&metadata, 0, SNAPSHOT_METADATA_SIZE);
        xglClockRead(&metadata.clock_reading);
        xglClockUInt2DayTime(&metadata.game_time, 0);
        metadata.game_state_value = GameLoopState.game_state_value;

        metadata_descriptor = sceOpen(snapname, SNAPSHOT_OPEN_FLAGS);
        sceWrite(metadata_descriptor, &metadata, SNAPSHOT_METADATA_SIZE);
        sceClose(metadata_descriptor);

        snapnamext[0] = 'j';
        snapnamext[1] = 'p';
        snapnamext[2] = 'g';
    }

    if (snapnameno[3] == '9') {
        if (snapnameno[2] == snapnameno[3]) {
            snapnameno[1]++;
            snapnameno[2] = '0';
        } else {
            snapnameno[2]++;
        }
        snapnameno[3] = '0';
    } else {
        snapnameno[3]++;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/game_snap_shot", GameSnapShotSaveThumbnail);

INCLUDE_ASM("asm/main/nonmatchings/game_snap_shot", GameSnapShotSave);

INCLUDE_ASM("asm/main/nonmatchings/game_snap_shot", GameSnapShotSaveFile);
