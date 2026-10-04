#include "common.h"
#include "shared.h"

/* Partial view of the resource record RES_loadFile hands back for a map:
 * MapReset (0x00246598, still INCLUDE_ASM) dereferences it at +0x04
 * (xglCullingMapSet argument), +0x0c (UnduInit argument) and +0x10 (second
 * argument to the still-unrecovered func_A1CFB8). Only those three offsets
 * are evidenced; the rest stays unmodeled. */
typedef struct {
    u8 unmodeled_00[4];
    void *culling_map;
    u8 unmodeled_08[4];
    void *undu_state;
    int map_parameter;
} MapResource;

typedef struct {
    u8 unmodeled_00[0x50];
    short map_no;
    u8 unmodeled_52[2];
    MapResource *map_resource;
    u8 unmodeled_58[0x60];
    int fade_countdown;
} MapGameState;

typedef struct {
    u8 unmodeled_00[0x10];
    u8 state;
    u8 unmodeled_11[3];
} MapTaskState;

typedef void (*MapTaskCallback)(void *task);

typedef struct {
    u8 unmodeled_00[0x30];
    MapTaskCallback callback;
} MapRenderState;

extern MapGameState GameLoopState;
extern u8 UseTestPath;
extern void MapChangeFadeSet(void);
extern void MapChange2(int map_id);
extern MapResource *RES_loadFile(int file_id, int resource_type, int map_id, int flags);
extern void MapReset(void);
extern XglPacket *xglPacketGetCurrent(void);
extern void xglRenderDrawFlipPk(XglPacket *packet);
extern void nmlModelSetFadeInCancel(int model_id);
extern void nmlModelSetFadeOutCancel(int model_id);
const char D_004BE308[16] = "data\\map\\";
const char D_004BE318[16] = "data\\map\\test\\";
const char D_004BE328[16] = "data\\map\\check\\";
static void taskMapChange(void *task);
MapTaskState tsk = { { 0 }, 0, { 0, 0, 0 } };
extern MapRenderState sRender;

const char *MAP_getPath(void)
{
    int test_path = UseTestPath;

    if (test_path != 1) {
        if (test_path < 2) {
            return D_004BE308;
        }
        if (test_path != 2) {
            return D_004BE308;
        }
        return D_004BE328;
    }
    return D_004BE318;
}

INCLUDE_ASM("asm/main/nonmatchings/map_1", taskMapChange);

void MapChangeFadeSet(void)
{
    if (GameLoopState.fade_countdown <= 0) {
        xglRenderDrawFlipPk(xglPacketGetCurrent());
        tsk.state = 0x80;
        sRender.callback = taskMapChange;
        nmlModelSetFadeInCancel(5);
        nmlModelSetFadeOutCancel(5);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/map_1", MapReset);

INCLUDE_ASM("asm/main/nonmatchings/map_1", MapChangeResource);

void MapChange(int map_id)
{
    MapChangeFadeSet();
    MapChange2(map_id);
}

void MapChange2(int map_id)
{
    if ((unsigned int)map_id < 0x71e) {
        GameLoopState.map_resource = RES_loadFile(-1, 1, map_id, 0);
        GameLoopState.map_no = map_id;
        MapReset();
    }
}

INCLUDE_ASM("asm/main/nonmatchings/map_1", MapGetName);

INCLUDE_ASM("asm/main/nonmatchings/map_1", MapDraw);

int MapGetNo(void)
{
    return GameLoopState.map_no;
}

void MapInit(void)
{
    UseTestPath = 0;
}
