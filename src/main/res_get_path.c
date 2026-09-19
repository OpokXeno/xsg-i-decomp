#include "common.h"
#include "shared.h"

extern char *command_enenpcse(int command, char *cursor, int mode);
extern void GameDiskChange(int disk_number);
extern void Intermission(int save_number);
extern int xglCdGetFileSize(const char *name);
extern int arcfilepreload;
extern char scene_txt_buffer[];
extern void SCRIPT_fade(int time);
extern char *command_mpeg2_core(int command, char *cursor, int mode);
extern int next_arc_size(void *address);
extern void Enemy_LoadPreset(void *address, const char *name);
extern u32 *arcfileaddr;
extern void *AdrsEnemyPreset;
#include "main/xgl_thread.h"

typedef struct {
    u16 id;
    u8 type;
    u8 foot;
    char name[16];
} EnemySeEntry;

extern EnemySeEntry EnemySeBank[8];
extern u8 ScenePath;
extern const char *tbl_0_00366420[20];

static void default_callback(void)
{
    xglSleep();
}

const char *RES_getPath(void)
{
    if (ScenePath >= 20)
        ScenePath = 0;
    return tbl_0_00366420[ScenePath];
}

static char *RES_getScenePath(char *path, int resource_id)
{
    const char *scene_path = RES_getPath();
    int scene_id;

    while ((*path = *scene_path) != 0) {
        path++;
        scene_path++;
    }

    if (resource_id <= 0x0fffffff) {
        path[0] = 'c';
        path[1] = 'f';
    } else {
        path[0] = 'e';
        path[1] = 'v';
    }

    scene_id = resource_id & 0x0fffffff;

    path[5] = (char)('0' + scene_id % 10);
    scene_id /= 10;
    path[4] = (char)('0' + scene_id % 10);
    scene_id /= 10;
    path[3] = (char)('0' + scene_id % 10);
    scene_id /= 10;
    path[2] = (char)('0' + scene_id % 10);
    path[6] = '.';
    path[7] = 't';
    path[8] = 'x';
    path[9] = 't';
    path[10] = 0;

    return path + 7;
}

int RES_GetEnemySeBank(int sound_id)
{
    int index = 0;
    int bank = 0x40000;

    for (; index < 8; index++) {
        if (EnemySeBank[index].id == sound_id)
            return bank;
        bank += 0x10000;
    }
    return -1;
}

int RES_GetEnemySeType(int sound_id)
{
    int index;

    for (index = 0; index < 8; index++) {
        if (EnemySeBank[index].id == sound_id)
            return EnemySeBank[index].type;
    }
    return -1;
}

int RES_GetEnemySeFoot(int sound_id)
{
    int index;

    for (index = 0; index < 8; index++) {
        if (EnemySeBank[index].id == sound_id)
            return EnemySeBank[index].foot;
    }
    return -1;
}

const char *RES_GetEnemySeName(int index)
{
    if (index < 8)
        return EnemySeBank[index].name;
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetMotBaseID);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", convert_face);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", resource_typeid_translate);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceSearch);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", resource_get_free);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceGetFreeAddr);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceWorkAlloc);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceWorkReload);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceAlloc);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceRealloc);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", next_arc_size);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", next_line);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", skip_space);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", search_key);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_reset);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadmap);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadact);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadface);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadmot);

/*
 * The engine's actor record: main/tu194 (src/main/enemy_2.h) defines and
 * owns it. This TU only reads the resource pointer past that definition's
 * extent, so it names the same tag without completing it (no local header
 * can include another TU's TU-local one) rather than modelling a
 * competing layout.
 */
struct Actor;

/*
 * The resource record an actor's `resource` pointer refers to.
 * RES_GetFootStepNo is the only reader in this TU, and it reads only the
 * footstep number byte at +6; the bytes before it are an unread span here.
 */
typedef struct ActorResource {
    unsigned char unmodeled_000[6];
    u8 foot_step_no; /* +6 */
} ActorResource;

/*
 * +0x8dc: the actor's resource pointer, past the extent src/main/enemy_2.h
 * completes; RES_GetFootStepNo is the only reader of it in this TU.
 */
#define ACTOR_RESOURCE_OFFSET 0x8dc
#define ACTOR_RESOURCE(actor) \
    (*(ActorResource **)((unsigned char *)(actor) + ACTOR_RESOURCE_OFFSET))

u8 RES_GetFootStepNo(struct Actor *actor)
{
    ActorResource *resource = ACTOR_RESOURCE(actor);
    u8 foot_step_no = 0;

    if (resource != 0) {
        foot_step_no = resource->foot_step_no;
    }
    return foot_step_no;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadtex);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadmovie);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadeffect_sub);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadeffect);

/* RES_loadFile callback: (resource type, file name, resource address). */
static int command_loadene_sub(int type, const char *name, void *address)
{
    if (arcfileaddr != 0)
        next_arc_size(AdrsEnemyPreset);
    else
        Enemy_LoadPreset(AdrsEnemyPreset, name);

    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadene);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_script);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_player);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_dummy);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetLeaderSeName);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetMapEnvSeName);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadse);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadsmd);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_mpeg2_core);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_mpeg2);

static char *command_mpeg2battle(int command, char *cursor, int mode)
{
    SCRIPT_fade(2);
    return command_mpeg2_core(command, cursor, 1);
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_mpeg2nofade);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_event2battle);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_player_lock);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_enenpcse);

static char *command_enese(int command, char *cursor, int mode)
{
    return command_enenpcse(command, cursor, 0);
}

static char *command_npcse(int command, char *cursor, int mode)
{
    return command_enenpcse(command, cursor, 1);
}

static char *command_disk(int command, char *cursor, int mode)
{
    GameDiskChange((unsigned char)cursor[0] - '0');
    return cursor;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_save);

void GameResourcePreLoad(int scene_number)
{
    char path[256];
    char *archive_path;
    int archive_size;

    archive_path = RES_getScenePath(path, scene_number);
    arcfilepreload = 0;
    if (xglCdGetFileSize(path) > 0) {
        xglCdReadFile(path, scene_txt_buffer, 1, 1);

        archive_path[0] = 'a';
        archive_path[1] = '\0';
        archive_size = xglCdGetFileSize(path);
        if (archive_size > 0) {
            arcfilepreload = 0x02000000 - ((archive_size + 2047) & -2048);
            xglCdReadFile(path, (void *)arcfilepreload, 1, 1);
        }
    }
}

static void readreq(const char *name, void *buffer)
{
    xglCdReadFile(name, buffer, 0, 1);
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceLoad);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_loadFileSubMapSub);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_loadFileSubMap);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetMotBaseName);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetMdlFileNameSub);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetMdlFileName);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_loadFileSub);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_loadFile);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceGetIndex);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceDump);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceReset);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceInit);
