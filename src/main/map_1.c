#include "common.h"

#include "shared.h"

/* Partial view of the resource record RES_loadFile hands back for a map:
 * MapReset (0x00246598, still INCLUDE_ASM) dereferences it at +0x04
 * (xglCullingMapSet argument), +0x0c (UnduInit argument) and +0x10 (second
 * argument to the still-unrecovered func_A1CFB8). Only those three offsets
 * are evidenced; the rest stays unmodeled. */

typedef struct {
    const Vector4 *place;
    int culling_map;
    const char *texture;
    unsigned char *undu_state;
    const void *think_data;
} MapResource;

typedef struct MapPlayerActorState MapPlayerActorState;

typedef struct {
    u8 unmodeled_00[4];
    MapPlayerActorState *player_actor;
    u8 unmodeled_08[4];
    u16 map_draw_mode;
    u8 unmodeled_0e[2];
    u32 render_flags;
    u8 unmodeled_14[0x3c];
    short map_no;
    short entrance_no;
    MapResource *map_resource;
    u8 unmodeled_58[0x28];
    float render_color[4];
    u8 unmodeled_90[0x20];
    int render_level;
    int clip_mode;
    int fade_countdown;
    u8 unmodeled_bc[5];
    u8 map_flags;
} MapGameState;

typedef struct {
    u8 unmodeled_00[0x10];
    u8 state;
    u8 unmodeled_11[3];
} MapTaskState;

typedef void (*MapTaskCallback)(void *task);

typedef struct {
    u8 unmodeled_00[0x14];
    u16 packet_offset_units;
    u8 unmodeled_16[0x1a];
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

struct MapPlayerActorState {
    u8 unmodeled_00[0x10];
    float position_x;
    float position_y;
    float position_z;
    u8 unmodeled_1c[0x38];
    float entrance_rotation;
    u8 unmodeled_58[0x98c];
    float entrance_rotation_copy;
};

typedef struct {
    MapResource *resource;
    int size;
    int handle;
    int state;
} GameResourceEntry;

typedef struct {
    u8 unmodeled_00[0x20];
    u64 task_state_word;
    u8 unmodeled_28[0x28];
    u64 vif_command_word;
    u8 unmodeled_58[0x58];
} MapTestPrimData;

typedef struct {
    float components[4];
} MapEntranceHeader;

typedef struct {
    char **names;
    int count;
} MapNameGroup;

typedef struct {
    Vector4 color;
    Vector4 direction;
} MapDrawLightEntry;

typedef struct {
    Vector4 ambient_color;
    MapDrawLightEntry lights[3];
    float normal_matrix[4][4];
    float color_modifier_matrix[4][4];
} MapDrawLight;

extern GameResourceEntry GameResource[];

extern MapNameGroup maptbl[];

extern MapEntranceHeader *UnduDataGetHeader(int map_index, int unit_index);

extern void UnduInit(unsigned char *data);

extern int func_A1CFB8(int think_no, const void *data);

extern void nmlModelSetMapLastInit(void);

extern void xglCullingMapSet(int culling_map);

extern MapDrawLight *xglStudioGetLight2(void);

extern void xglLightCalcMatrix(MapDrawLight *light);

extern void nmlModelSetLight(void *color_modifier, void *normal);

extern void nmlModelSetTexture(const char *texture);

extern void nmlModelSetMapEntry(void);

extern void nmlModelSetMulColor(const float color[4]);

extern void nmlModelSetRenderLevel(int level);

extern void nmlModelSetPlace(const Vector4 *place);

extern void nmlModelSetClip(int clip);

extern void nmlModelEntry(int entry);

extern void sceVif1PkCnt(NmlPacket packet, int count);

extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

extern void nmlModelSendBackBufferSignal(void);

extern MapTestPrimData TestPrim_0_00362790;

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

static void taskMapChange(void *task)
{
    int command_low = 0x24120000 | ((u32)sRender.packet_offset_units << 5);

    TestPrim_0_00362790.task_state_word = ((u64)tsk.state << 32) | 100;
    TestPrim_0_00362790.vif_command_word = ((u64)0xc800 << 19) | command_low;
    sceVif1PkCnt(task, 0);
    sceVif1PkAddDataN(task, &TestPrim_0_00362790, 0x2c);
    nmlModelSendBackBufferSignal();
    tsk.state -= 4;
    if (tsk.state == 0) {
        sRender.callback = 0;
    }
}

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

void MapReset(void)
{
    UnduInit(GameLoopState.map_resource->undu_state);
    GameLoopState.map_flags |= 0x80;
    GameLoopState.render_flags |= 0x30;
    func_A1CFB8(GameLoopState.map_no, GameLoopState.map_resource->think_data);
    nmlModelSetMapLastInit();
    xglCullingMapSet(GameLoopState.map_resource->culling_map);
    GameLoopState.render_level = 0;
    GameLoopState.clip_mode = 0;
    GameLoopState.render_color[3] = 1.0f;
    GameLoopState.render_color[2] = 1.0f;
    GameLoopState.render_color[1] = 1.0f;
    GameLoopState.render_color[0] = 1.0f;
}

void MapChangeResource(int map_id, int entrance_id)
{
    if (GameResource[map_id].state == 1) {
        GameLoopState.map_no = GameResource[map_id].handle;
        GameLoopState.map_resource = GameResource[map_id].resource;
        if (map_id < 1000) {
            MapChangeFadeSet();
        }
        MapReset();
        GameLoopState.entrance_no = (short)entrance_id;
        {
            MapEntranceHeader *header = UnduDataGetHeader(0x201, entrance_id + 0x8000);

            if (header != 0) {
                float last_component;

                GameLoopState.player_actor->position_x = header->components[0];
                GameLoopState.player_actor->position_y = header->components[1];
                GameLoopState.player_actor->position_z = header->components[2];
                last_component = header->components[3];
                GameLoopState.player_actor->entrance_rotation = last_component;
                GameLoopState.player_actor->entrance_rotation_copy = last_component;
            }
        }
    }
}

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

char *MapGetName(int map_no)
{
    int table_index;
    int entry_count;
    char *map_name;

    if (map_no < 0) {
        map_no = GameLoopState.map_no;
    }
    map_name = 0;
    if (map_no < 0x71F) {
        table_index = map_no / 100;
        entry_count = maptbl[table_index].count;
        if ((map_no % 100) < entry_count) {
            map_name = maptbl[table_index].names[map_no % 100];
        }
    }
    return map_name;
}

void MapDraw(void)
{
    MapResource *resource = GameLoopState.map_resource;

    if (GameLoopState.map_draw_mode == 2 && (GameLoopState.render_flags & 0x20) != 0) {
        if ((GameLoopState.render_flags & 0x10) != 0) {
            GameLoopState.render_flags &= ~0x10;
        } else {
            GameLoopState.render_flags &= ~0x20;
        }
    }

    if (resource != 0 && resource->culling_map != 0) {
        MapDrawLight *light = xglStudioGetLight2();

        xglLightCalcMatrix(light);
        nmlModelSetLight(light->color_modifier_matrix, light->normal_matrix);
        if (resource->texture != 0) {
            nmlModelSetTexture(resource->texture);
        }
        nmlModelSetMapEntry();
        nmlModelSetMulColor(GameLoopState.render_color);
        nmlModelSetRenderLevel(GameLoopState.render_level);
        nmlModelSetPlace(resource->place);
        if (GameLoopState.clip_mode != 0) {
            nmlModelSetClip(1);
        } else {
            nmlModelSetClip(2);
        }
        nmlModelEntry(resource->culling_map);
    }
}

int MapGetNo(void)
{
    return GameLoopState.map_no;
}

void MapInit(void)
{
    UseTestPath = 0;
}
