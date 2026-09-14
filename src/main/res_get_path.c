#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", default_callback);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_getPath);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_getScenePath);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetEnemySeBank);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetEnemySeType);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetEnemySeFoot);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetEnemySeName);

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

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_npcse);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_disk);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_save);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourcePreLoad);

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
