#include "common.h"
#include "shared.h"

extern char *command_enenpcse(int command, char *cursor, int mode);
extern void GameDiskChange(int disk_number);
extern void Intermission(int save_number);
extern int xglCdGetFileSize(const char *name);
extern int arcfilepreload;
extern char scene_txt_buffer[];
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

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetFootStepNo);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadtex);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadmovie);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadeffect_sub);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadeffect);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadene_sub);

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

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_mpeg2battle);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_mpeg2nofade);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_event2battle);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_player_lock);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_enenpcse);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_enese);

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
