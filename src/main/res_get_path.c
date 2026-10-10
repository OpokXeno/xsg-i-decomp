#include "common.h"

#include "shared.h"

static char *command_enenpcse(int command, char *cursor, int mode);

extern int GameDiskChange(int disk_number);

extern void Intermission(int save_number);

extern int xglCdGetFileSize(const char *name);

extern void SCRIPT_fade(int time);

static char *command_mpeg2_core(int command, char *cursor, int mode);

static int next_arc_size(void *address);

extern void Enemy_LoadPreset(void *address, const char *name);

extern void *AdrsEnemyPreset;

#include "main/xgl_thread.h"

typedef struct {
    u16 id;
    u8 type;
    u8 foot;
    char name[16];
} EnemySeEntry;

static u32 *arcfileaddr;

static int arcfilepreload;

static EnemySeEntry EnemySeBank[8];

static char foot_name[16];

static char env_name[0x10];

static char scene_txt_buffer[0x800];

static u32 archeader[0x40];

static u8 GameResourceWorkReloadTable[0x80];

u8 ScenePath = 0;

extern const char D_004C0098[];

extern const char D_004C0080[];

extern const char D_004C0068[];

extern const char D_004C0058[];

extern const char D_004C0040[];

extern const char D_004C0028[];

extern const char D_004C0010[];

extern const char D_004BFFF8[];

extern const char D_004BFFE0[];

extern const char D_004BFFC8[];

extern const char D_004BFFB0[];

extern const char D_004BFF98[];

extern const char D_004BFF80[];

extern const char D_004BFF68[];

extern const char D_004BFF50[];

extern const char D_004BFF38[];

extern const char D_004BFF20[];

extern const char D_004BFF08[];

extern const char D_004BFEF0[];

extern const char D_004BFED8[];

/* search_key walks this object in 12-byte records: it compares the first
 * string with the requested key and walks the string-pointer list at +4.
 * The third pointer is present in every record but is not read by the
 * recovered caller, so its role remains intentionally unnamed. */

typedef struct ResourceKeyEntry {
    const char *key;
    const void *aliases;
    const void *unmodeled_08;
} ResourceKeyEntry;

extern const char D_004D9DD0[];

extern const char D_004D9DC8[];

extern const char D_004D9DC0[];

extern const char D_004D9CA0[];

extern const char D_004D9C98[];

extern const char D_004D9DB8[];

extern const char D_004D9DB0[];

extern const char D_004D9DA8[];

extern const char D_004D9D98[];

extern const char D_004D9D90[];

extern const char D_004D9DA0[];

extern const char D_004D95D0[];

extern const char *AID0[];
extern const char *CID0[];
extern const char *CID1[];
extern const char *CID2[];
extern const char *CID3[];
extern const char *CID4[];
extern const char *CID5[];
extern const char *CID6[];
extern const char D_004BFC48[];
extern const char D_004BFC58[];
extern const char D_004BFC68[];
extern const char D_004BFC78[];
extern const char D_004BFC88[];
extern const char D_004BFC98[];
extern const char D_004BFCA8[];
extern const char D_004BFCB8[];
extern const char D_004BFCC8[];
extern const char D_004BFCD8[];
extern const char D_004BFCE8[];
extern const char D_004BFCF8[];
extern const char D_004BFD08[];
extern const char D_004BFD18[];
extern const char D_004BFD28[];
extern const char D_004BFD38[];
extern const char D_004BFD48[];
extern const char D_004BFD58[];
extern const char D_004D9C40[];
extern const char D_004D9C48[];
extern const char D_004D9C50[];
extern const char D_004D9C58[];
extern const char D_004D9C60[];
extern const char D_004D9C68[];
extern const char D_004D9C70[];
extern const char D_004D9C78[];
extern const char D_004D9C80[];
extern const char D_004D9C88[];
extern const char D_004D9C90[];
extern const char D_004D9CA8[];
extern const char D_004D9CB0[];
extern const char D_004D9CB8[];
extern const char D_004D9CC0[];
extern const char D_004D9CC8[];
extern const char D_004D9CD0[];
extern const char D_004D9CD8[];
extern const char D_004D9CE0[];
extern const char D_004D9CE8[];
extern const char D_004D9CF0[];
extern const char D_004D9CF8[];
extern const char D_004D9D00[];
extern const char D_004D9D08[];
extern const char D_004D9D10[];
extern const char D_004D9D18[];
extern const char D_004D9D20[];
extern const char D_004D9D28[];
extern const char D_004D9D30[];
extern const char D_004D9D38[];
extern const char D_004D9D40[];
extern const char D_004D9D48[];
extern const char D_004D9D50[];
extern const char D_004D9D58[];
extern const char D_004D9D60[];
extern const char D_004D9D68[];
extern const char D_004D9D70[];
extern const char D_004D9D78[];
extern const char D_004D9D80[];
extern const char D_004D9D88[];
extern const char *EID0[];
extern const char *EID1[];
extern const char *EID2[];
extern const char *EID3[];
extern const char *EID4[];
extern const char *EID5[];
extern const char *EID6[];
extern const char *EID7[];
extern const char *EID8[];
extern const char *EIDf[];
extern const char *MID0[];
extern const char *MU[];
extern const char *MUF[];
extern const char *MUS[];
extern const char *MUW[];
extern const char *MUX[];
extern const char *RID0[];
extern const char *RID1[];
extern const char *RID2[];
extern const char *TRE[];
extern const char *WID0[];
extern const char *WID1[];
extern const char *WID2[];
extern const char *WID3[];
extern const char *WID4[];
extern const char *WID5[];
extern const char *WID6[];
extern const char *WID7[];
extern const char *WID8[];

static ResourceKeyEntry CID[8] = {
    { D_004D95D0, CID0, D_004D9C58 },
    { D_004D95D0, CID1, D_004D9C50 },
    { D_004D95D0, CID2, D_004D9C48 },
    { D_004D95D0, CID3, D_004D9C48 },
    { D_004D95D0, CID4, D_004D9C48 },
    { D_004D95D0, CID5, D_004D9C48 },
    { D_004D95D0, CID6, D_004D9C40 },
    { 0, 0, 0 }
};

static ResourceKeyEntry RID[4] = {
    { D_004D9C80, RID0, D_004D9C78 },
    { D_004D9C70, RID1, D_004D9C68 },
    { D_004D95D0, RID2, D_004D9C60 },
    { 0, 0, 0 }
};

static ResourceKeyEntry WID[10] = {
    { D_004D9D00, WID0, D_004D9CF8 },
    { D_004D9CF0, WID1, D_004D9CE8 },
    { D_004D9CE0, WID2, D_004D9CD8 },
    { D_004D9CD0, WID3, D_004D9CC8 },
    { D_004D9CC0, WID4, D_004D9CB8 },
    { D_004D9CB0, WID5, D_004D9CA8 },
    { D_004D95D0, WID6, D_004D9CA0 },
    { D_004D95D0, WID7, D_004D9C98 },
    { D_004D9C90, WID8, D_004D9C88 },
    { 0, 0, 0 }
};

static ResourceKeyEntry EID[17] = {
    { D_004D9D58, EID0, D_004BFCD8 },
    { D_004D9D50, EID1, D_004BFCC8 },
    { D_004D9D48, EID2, D_004BFCB8 },
    { D_004D9D40, EID3, D_004BFCA8 },
    { D_004D9D38, EID4, D_004BFC98 },
    { D_004D9D30, EID5, D_004BFC88 },
    { D_004D9D28, EID6, D_004BFC78 },
    { D_004D9D20, EID7, D_004BFC68 },
    { D_004D9D18, EID8, D_004BFC58 },
    { D_004D9D10, EID8, D_004BFCD8 },
    { D_004D9D10, EID8, D_004BFCD8 },
    { D_004D9D10, EID8, D_004BFCD8 },
    { D_004D9D10, EID8, D_004BFCD8 },
    { D_004D9D10, EID8, D_004BFCD8 },
    { D_004D9D10, EID8, D_004BFCD8 },
    { D_004D9D08, EIDf, D_004BFC48 },
    { 0, 0, 0 }
};

static ResourceKeyEntry MID[2] = {
    { D_004D95D0, MID0, D_004BFCE8 },
    { 0, 0, 0 }
};

static ResourceKeyEntry AID[2] = {
    { D_004D95D0, AID0, D_004BFCF8 },
    { 0, 0, 0 }
};

static ResourceKeyEntry MOBJ[10] = {
    { D_004D9D88, MU, D_004BFD58 },
    { D_004D9D80, MUW, D_004BFD48 },
    { D_004D9D78, TRE, D_004BFD38 },
    { D_004D9D78, TRE, D_004BFD38 },
    { D_004D9D70, MUX, D_004BFD28 },
    { D_004D9D70, MUX, D_004BFD28 },
    { D_004D9D68, MUF, D_004BFD18 },
    { D_004D9D68, MUF, D_004BFD18 },
    { D_004D9D60, MUS, D_004BFD08 },
    { 0, 0, 0 }
};

static ResourceKeyEntry model[9] = {
    {D_004D9DD0, CID, D_004D9DC8},
    {D_004D9DD0, CID, D_004D9DC8},
    {D_004D9DC0, RID, D_004D9CA0},
    {D_004D9DB8, WID, D_004D9DB0},
    {D_004D9DA8, EID, D_004D9C98},
    {D_004D9DA0, MID, D_004D9D98},
    {D_004D9D90, AID, D_004D9D98},
    {D_004D95D0, MOBJ, D_004D9D98},
    {0, 0, 0},
};

static const char *tbl_0_00366420[20] = {
    D_004C0098, D_004C0080, D_004C0068, D_004C0058, D_004C0040,
    D_004C0028, D_004C0010, D_004BFFF8, D_004BFFE0, D_004BFFC8,
    D_004BFFB0, D_004BFF98, D_004BFF80, D_004BFF68, D_004BFF50,
    D_004BFF38, D_004BFF20, D_004BFF08, D_004BFEF0, D_004BFED8
};

/*
 * The runtime resource-block table: up to 128 blocks carved out of the
 * arc-file load heap GameResourceInit sizes. address/size describe the
 * block's extent; handle is a random token GameResourceAlloc
 * (main:0x0024a9d8) assigns with xglSRand() when it hands a block out, and
 * both it and GameResourceReset (main:0x0024d8e8) clear it back to 0; state
 * is -1 for a free block (this TU's readers) and 9 once GameResourceAlloc
 * marks one in use (original bytes; neither function is claimed here).
 */

typedef struct {
    u32 address;   /* +0x0 */
    int size;       /* +0x4 */
    int handle;     /* +0x8 */
    int state;      /* +0xC, -1 = free */
} GameResourceEntry;

GameResourceEntry GameResource[128] = {0};

/*
 * The game loop's own record, as far as this TU reads it. Two spans are
 * evidenced: the runtime flags word command_player_lock ORs a bit into
 * (lw/ori/sw +0x10, main:0x0024c188) and the scene id GameResourceWorkReload
 * reloads by (lw +0x18, main:0x0024a998). Nothing else is modelled; the
 * object is still owned by the generated .sdata scaffolding.
 */

typedef struct {
    u8 unmodeled_00[0x10];
    u32 flags;      /* +0x10 */
    u8 unmodeled_14[4];
    int scene_id;   /* +0x18 */
} GameLoopStatePrefix;

extern GameLoopStatePrefix GameLoopState;

extern void ACT_init(void);

extern void MapChangeFadeSet(void);

extern void GameResourceLoad(int scene_id);

extern int RES_loadFile(int command, int callback, int resource_id, int flags);

static char *search_key(char *cursor, u8 *table, int *id);

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

struct Actor {
    unsigned char unmodeled_000[ACTOR_RESOURCE_OFFSET];
    ActorResource *resource;
};

extern int sefLoadEffectCfName(int cf_id, char *effect_name);

extern int sefLoadMemoryEffectCfName(void *data, char *effect_name, int size);

/* RES_loadFile callback: (resource type, file name, resource address). */

extern void GameCfPlayerLoadResource(int resource_set);

extern void SCRIPT_frameLock2Battle(void);

extern const char D_004D8B88[];

extern const char D_004BF888[];

extern const char D_004D8AC8[];

extern const char D_004D8AA8[];

extern const char D_004BF7A0[];

extern const char D_004BF8C0[];

extern const char D_004D8B30[];

extern const char D_004D8AE8[];

extern const char D_004BF838[];

extern const char D_004D8B08[];

extern const char D_004D8B58[];

extern const char D_004D8B40[];

extern const char D_004D8AC0[];

extern const char D_004D8B20[];

extern const char D_004D8B70[];

extern const char D_004D8BB0[];

extern const char D_004D8B28[];

extern const char D_004D8B18[];

extern const char D_004BF8D0[];

extern const char D_004D8AF0[];

extern const char D_004BF7F0[];

extern const char D_004BF7E0[];

extern const char D_004D8AF8[];

extern const char D_004BF858[];

extern const char D_004D8B10[];

extern const char D_004D8B80[];

extern const char D_004BF848[];

extern const char D_004BF818[];

extern const char D_004D8AB8[];

extern const char D_004D8AD8[];

extern const char D_004D8B00[];

extern const char D_004BF7C8[];

extern const char D_004BF8B0[];

extern const char D_004D8B60[];

extern const char D_004D8BA8[];

extern const char D_004D8B48[];

extern const char D_004D8AB0[];

extern const char D_004BF800[];

extern const char D_004BF8E0[];

extern const char D_004BF828[];

extern const char D_004D8B50[];

extern const char D_004D8B98[];

extern const char D_004BF898[];

extern const char D_004BF7B0[];

extern const char D_004D8B38[];

extern const char D_004D8AE0[];

extern const char D_004BF868[];

extern const char D_004BF878[];

extern const char D_004D8AD0[];

extern const char D_004D8B90[];

extern const char D_004D8B68[];

extern const char D_004D8BA0[];

extern const char D_004D8B78[];

extern const char D_004D8C78[];

extern const char D_004D8D08[];

extern const char D_004D8CF8[];

extern const char D_004D8C70[];

extern const char D_004BFA68[];

extern const char D_004BFB08[];

extern const char D_004D8D20[];

extern const char D_004D8CB0[];

extern const char D_004D8D30[];

extern const char D_004BFAF8[];

extern const char D_004D8D48[];

extern const char D_004D8D38[];

extern const char D_004D8CD8[];

extern const char D_004D8BD0[];

extern const char D_004BFA40[];

extern const char D_004BF900[];

extern const char D_004BFA30[];

extern const char D_004BFA20[];

extern const char D_004BFAA8[];

extern const char D_004BF928[];

extern const char D_004BFAB8[];

extern const char D_004D8BC8[];

extern const char D_004BF938[];

extern const char D_004D8CF0[];

extern const char D_004D8C90[];

extern const char D_004D8C10[];

extern const char D_004D8CC8[];

extern const char D_004BF9B0[];

extern const char D_004BF980[];

extern const char D_004D8C28[];

extern const char D_004D8C30[];

extern const char D_004BFA10[];

extern const char D_004D8C98[];

extern const char D_004BF9E0[];

extern const char D_004BFA88[];

extern const char D_004D8C80[];

extern const char D_004D8D80[];

extern const char D_004D8C00[];

extern const char D_004BFA00[];

extern const char D_004D8D50[];

extern const char D_004D8CE8[];

extern const char D_004D8C58[];

extern const char D_004D8BD8[];

extern const char D_004D8C20[];

extern const char D_004D8BC0[];

extern const char D_004BF918[];

extern const char D_004D8C38[];

extern const char D_004BF9C8[];

extern const char D_004D8C40[];

extern const char D_004BF968[];

extern const char D_004D8BE0[];

extern const char D_004BF8F0[];

extern const char D_004BFA98[];

extern const char D_004D8CA8[];

extern const char D_004D8C48[];

extern const char D_004D8D10[];

extern const char D_004D8D70[];

extern const char D_004D8CB8[];

extern const char D_004BF9F0[];

extern const char D_004D8C68[];

extern const char D_004BFAD8[];

extern const char D_004BF998[];

extern const char D_004BF950[];

extern const char D_004D8CD0[];

extern const char D_004D8D28[];

extern const char D_004D8C50[];

extern const char D_004BFA78[];

extern const char D_004D8BF0[];

extern const char D_004D8BE8[];

extern const char D_004D8D78[];

extern const char D_004D8C60[];

extern const char D_004D8C18[];

extern const char D_004D8D18[];

extern const char D_004D8D40[];

extern const char D_004D8C08[];

extern const char D_004BFA58[];

extern const char D_004D8D00[];

extern const char D_004D8CA0[];

extern const char D_004BFAC8[];

extern const char D_004BFAE8[];

extern const char D_004D8CE0[];

extern const char D_004D8D58[];

extern const char D_004D8BB8[];

extern const char D_004D8C88[];

extern const char D_004D8D68[];

extern const char D_004D8BF8[];

extern const char D_004D8CC0[];

extern const char D_004D8D60[];

extern const char D_004D8DE0[];

extern const char D_004BFB18[];

extern const char D_004D8E48[];

extern const char D_004D8DF0[];

extern const char D_004D8DA8[];

extern const char D_004D8E38[];

extern const char D_004BFB28[];

extern const char D_004D8E08[];

extern const char D_004D8D98[];

extern const char D_004BFB58[];

extern const char D_004D8DD0[];

extern const char D_004D8DB0[];

extern const char D_004D8DD8[];

extern const char D_004D8D90[];

extern const char D_004D8E00[];

extern const char D_004D8D88[];

extern const char D_004D8DF8[];

extern const char D_004D8DC0[];

extern const char D_004D8DC8[];

extern const char D_004D8E50[];

extern const char D_004D8E20[];

extern const char D_004D8E18[];

extern const char D_004BFB38[];

extern const char D_004D8E28[];

extern const char D_004D8DA0[];

extern const char D_004D8E10[];

extern const char D_004D8DB8[];

extern const char D_004D8E30[];

extern const char D_004BFB48[];

extern const char D_004D8DE8[];

extern const char D_004D8E40[];

extern const char D_004D8EE0[];

extern const char D_004D8ED8[];

extern const char D_004D8EA0[];

extern const char D_004D8E78[];

extern const char D_004D8E80[];

extern const char D_004D8EC0[];

extern const char D_004D8E70[];

extern const char D_004D8E58[];

extern const char D_004D8EC8[];

extern const char D_004D8EB0[];

extern const char D_004D8EB8[];

extern const char D_004D8E60[];

extern const char D_004D8EA8[];

extern const char D_004D8ED0[];

extern const char D_004D8E88[];

extern const char D_004D8E98[];

extern const char D_004D8E68[];

extern const char D_004D8E90[];

extern const char D_004D8F18[];

extern const char D_004D8F38[];

extern const char D_004D8F50[];

extern const char D_004D8EE8[];

extern const char D_004D8EF0[];

extern const char D_004D8F30[];

extern const char D_004D8F28[];

extern const char D_004D8F48[];

extern const char D_004D8F00[];

extern const char D_004D8EF8[];

extern const char D_004D8F20[];

extern const char D_004D8F10[];

extern const char D_004D8F40[];

extern const char D_004D8F08[];

extern const char D_004D8F70[];

extern const char D_004D8F80[];

extern const char D_004D8F78[];

extern const char D_004D8F68[];

extern const char D_004D8F98[];

extern const char D_004D8F58[];

extern const char D_004D8F88[];

extern const char D_004D8F60[];

extern const char D_004D8F90[];

extern const char D_004D9048[];

extern const char D_004D8FC8[];

extern const char D_004D8FE0[];

extern const char D_004D9068[];

extern const char D_004D8FE8[];

extern const char D_004D91A8[];

extern const char D_004D8FA0[];

extern const char D_004D9190[];

extern const char D_004D9040[];

extern const char D_004BFB88[];

extern const char D_004BFBC8[];

extern const char D_004D9148[];

extern const char D_004D90E8[];

extern const char D_004D8FA8[];

extern const char D_004D9050[];

extern const char D_004BFBD8[];

extern const char D_004D9098[];

extern const char D_004D90D8[];

extern const char D_004D8FD0[];

extern const char D_004D9118[];

extern const char D_004D8FB0[];

extern const char D_004D9198[];

extern const char D_004D90B0[];

extern const char D_004D9080[];

extern const char D_004D9020[];

extern const char D_004D9000[];

extern const char D_004D9128[];

extern const char D_004D9058[];

extern const char D_004D8FC0[];

extern const char D_004D90C0[];

extern const char D_004D8FB8[];

extern const char D_004D9070[];

extern const char D_004D90F0[];

extern const char D_004D9018[];

extern const char D_004D9120[];

extern const char D_004D9010[];

extern const char D_004D8FF8[];

extern const char D_004D9188[];

extern const char D_004D9030[];

extern const char D_004D9158[];

extern const char D_004BFBF8[];

extern const char D_004D91A0[];

extern const char D_004D9060[];

extern const char D_004D8FF0[];

extern const char D_004D9100[];

extern const char D_004D9078[];

extern const char D_004D9130[];

extern const char D_004D90B8[];

extern const char D_004BFB98[];

extern const char D_004D91B0[];

extern const char D_004D9090[];

extern const char D_004D90A0[];

extern const char D_004D90F8[];

extern const char D_004BFB78[];

extern const char D_004D90C8[];

extern const char D_004D90E0[];

extern const char D_004BFBB8[];

extern const char D_004D9008[];

extern const char D_004D9138[];

extern const char D_004D9028[];

extern const char D_004D91B8[];

extern const char D_004D9180[];

extern const char D_004D90D0[];

extern const char D_004BFB68[];

extern const char D_004BFBE8[];

extern const char D_004D9178[];

extern const char D_004BFBA8[];

extern const char D_004D9108[];

extern const char D_004BFC18[];

extern const char D_004D8FD8[];

extern const char D_004D9038[];

extern const char D_004D9088[];

extern const char D_004D9168[];

extern const char D_004D9140[];

extern const char D_004D9150[];

extern const char D_004BFC08[];

extern const char D_004D9110[];

extern const char D_004D90A8[];

extern const char D_004D9160[];

extern const char D_004D9170[];

extern const char D_004D91C8[];

extern const char D_004D91E8[];

extern const char D_004D91D0[];

extern const char D_004D91C0[];

extern const char D_004D91D8[];

extern const char D_004D91E0[];

extern const char D_004D9208[];

extern const char D_004D9230[];

extern const char D_004D9228[];

extern const char D_004D9238[];

extern const char D_004D9220[];

extern const char D_004D9248[];

extern const char D_004D9218[];

extern const char D_004D9250[];

extern const char D_004D91F8[];

extern const char D_004D9240[];

extern const char D_004D9200[];

extern const char D_004D91F0[];

extern const char D_004D9210[];

extern const char D_004D9290[];

extern const char D_004D9278[];

extern const char D_004D9280[];

extern const char D_004D9298[];

extern const char D_004D9270[];

extern const char D_004D9268[];

extern const char D_004D9260[];

extern const char D_004BFC28[];

extern const char D_004D9258[];

extern const char D_004D9288[];

extern const char D_004D92C8[];

extern const char D_004D92B8[];

extern const char D_004D92B0[];

extern const char D_004D92A8[];

extern const char D_004D92D0[];

extern const char D_004D92E0[];

extern const char D_004D92D8[];

extern const char D_004D92C0[];

extern const char D_004D92E8[];

extern const char D_004D92A0[];

extern const char D_004D9368[];

extern const char D_004D92F8[];

extern const char D_004D92F0[];

extern const char D_004D9348[];

extern const char D_004D9330[];

extern const char D_004D9308[];

extern const char D_004D9300[];

extern const char D_004D9370[];

extern const char D_004D9318[];

extern const char D_004D9340[];

extern const char D_004D9338[];

extern const char D_004D9350[];

extern const char D_004D9328[];

extern const char D_004D9310[];

extern const char D_004D9358[];

extern const char D_004D9320[];

extern const char D_004D9360[];

extern const char D_004D9378[];

extern const char D_004D9398[];

extern const char D_004D9388[];

extern const char D_004D9390[];

extern const char D_004D93B8[];

extern const char D_004D9380[];

extern const char D_004D93A8[];

extern const char D_004D93A0[];

extern const char D_004D93B0[];

extern const char D_004D93C0[];

extern const char D_004D93C8[];

extern const char D_004D93D8[];

extern const char D_004D93D0[];

extern const char D_004D93E0[];

extern const char D_004D93F0[];

extern const char D_004D9518[];

extern const char D_004D94D0[];

extern const char D_004D9530[];

extern const char D_004D9468[];

extern const char D_004D9400[];

extern const char D_004D94C8[];

extern const char D_004D9418[];

extern const char D_004D9500[];

extern const char D_004D9428[];

extern const char D_004D9450[];

extern const char D_004D9460[];

extern const char D_004D94E8[];

extern const char D_004D9540[];

extern const char D_004D9490[];

extern const char D_004D94D8[];

extern const char D_004D9510[];

extern const char D_004D9410[];

extern const char D_004D9478[];

extern const char D_004D94F8[];

extern const char D_004D9498[];

extern const char D_004D9458[];

extern const char D_004D9420[];

extern const char D_004D9520[];

extern const char D_004D9550[];

extern const char D_004D94A8[];

extern const char D_004D94B8[];

extern const char D_004D9440[];

extern const char D_004D9438[];

extern const char D_004D9560[];

extern const char D_004D94C0[];

extern const char D_004D9480[];

extern const char D_004D9568[];

extern const char D_004D93F8[];

extern const char D_004D9528[];

extern const char D_004D93E8[];

extern const char D_004D94F0[];

extern const char D_004D9430[];

extern const char D_004D9508[];

extern const char D_004D9408[];

extern const char D_004D9488[];

extern const char D_004D9538[];

extern const char D_004D9558[];

extern const char D_004D9448[];

extern const char D_004D94B0[];

extern const char D_004D9470[];

extern const char D_004D94A0[];

extern const char D_004D94E0[];

extern const char D_004D9548[];

extern const char D_004D95A0[];

extern const char D_004D9598[];

extern const char D_004D95B0[];

extern const char D_004D9588[];

extern const char D_004D95A8[];

extern const char D_004D95B8[];

extern const char D_004D9570[];

extern const char D_004BFC38[];

extern const char D_004D9590[];

extern const char D_004D9580[];

extern const char D_004D9578[];

extern const char D_004D95C0[];

extern const char D_004D9700[];

extern const char D_004D9730[];

extern const char D_004D95E0[];

extern const char D_004D95D8[];

extern const char D_004D9750[];

extern const char D_004D9738[];

extern const char D_004D95C8[];

extern const char D_004D9648[];

extern const char D_004D9650[];

extern const char D_004D96F0[];

extern const char D_004D96A0[];

extern const char D_004D9600[];

extern const char D_004D96D8[];

extern const char D_004D9610[];

extern const char D_004D9638[];

extern const char D_004D96E8[];

extern const char D_004D9748[];

extern const char D_004D96F8[];

extern const char D_004D9668[];

extern const char D_004D96C8[];

extern const char D_004D9640[];

extern const char D_004D9720[];

extern const char D_004D9758[];

extern const char D_004D9618[];

extern const char D_004D96B0[];

extern const char D_004D9658[];

extern const char D_004D95E8[];

extern const char D_004D96C0[];

extern const char D_004D96B8[];

extern const char D_004D9678[];

extern const char D_004D9760[];

extern const char D_004D96E0[];

extern const char D_004D9690[];

extern const char D_004D9660[];

extern const char D_004D9608[];

extern const char D_004D9688[];

extern const char D_004D9718[];

extern const char D_004D96D0[];

extern const char D_004D96A8[];

extern const char D_004D9680[];

extern const char D_004D9628[];

extern const char D_004D9728[];

extern const char D_004D9740[];

extern const char D_004D9630[];

extern const char D_004D9698[];

extern const char D_004D9620[];

extern const char D_004D9710[];

extern const char D_004D9708[];

extern const char D_004D95F8[];

extern const char D_004D9670[];

extern const char D_004D95F0[];

extern const char D_004D9790[];

extern const char D_004D9780[];

extern const char D_004D97A0[];

extern const char D_004D9798[];

extern const char D_004D9788[];

extern const char D_004D9770[];

extern const char D_004D9768[];

extern const char D_004D9778[];

extern const char D_004D99B0[];

extern const char D_004D9AD0[];

extern const char D_004D9A08[];

extern const char D_004D9958[];

extern const char D_004D9AB8[];

extern const char D_004D9B78[];

extern const char D_004D9800[];

extern const char D_004D9940[];

extern const char D_004D98F8[];

extern const char D_004D9B28[];

extern const char D_004D9818[];

extern const char D_004D9B90[];

extern const char D_004D9B18[];

extern const char D_004D9C18[];

extern const char D_004D9AA0[];

extern const char D_004D9A90[];

extern const char D_004D9A88[];

extern const char D_004D9A70[];

extern const char D_004D9A58[];

extern const char D_004D9BF0[];

extern const char D_004D9AF0[];

extern const char D_004D9938[];

extern const char D_004D9928[];

extern const char D_004D9918[];

extern const char D_004D98B8[];

extern const char D_004D99B8[];

extern const char D_004D9978[];

extern const char D_004D97C8[];

extern const char D_004D9AE8[];

extern const char D_004D9930[];

extern const char D_004D9B20[];

extern const char D_004D9898[];

extern const char D_004D97B8[];

extern const char D_004D97A8[];

extern const char D_004D9BF8[];

extern const char D_004D98E8[];

extern const char D_004D9BC8[];

extern const char D_004D97F8[];

extern const char D_004D9AE0[];

extern const char D_004D9A40[];

extern const char D_004D9970[];

extern const char D_004D9A38[];

extern const char D_004D99E0[];

extern const char D_004D9AB0[];

extern const char D_004D9888[];

extern const char D_004D99D0[];

extern const char D_004D9BD0[];

extern const char D_004D98C0[];

extern const char D_004D9C00[];

extern const char D_004D97C0[];

extern const char D_004D9AF8[];

extern const char D_004D9A60[];

extern const char D_004D9848[];

extern const char D_004D9808[];

extern const char D_004D9B88[];

extern const char D_004D9A30[];

extern const char D_004D99A8[];

extern const char D_004D9B40[];

extern const char D_004D9B60[];

extern const char D_004D9C38[];

extern const char D_004D9A78[];

extern const char D_004D98D8[];

extern const char D_004D99E8[];

extern const char D_004D9C08[];

extern const char D_004D9A68[];

extern const char D_004D9BC0[];

extern const char D_004D9878[];

extern const char D_004D9870[];

extern const char D_004D97D0[];

extern const char D_004D9920[];

extern const char D_004D99F8[];

extern const char D_004D9838[];

extern const char D_004D97B0[];

extern const char D_004D9828[];

extern const char D_004D97E8[];

extern const char D_004D9C30[];

extern const char D_004D9820[];

extern const char D_004D9A80[];

extern const char D_004D9960[];

extern const char D_004D9868[];

extern const char D_004D9BB8[];

extern const char D_004D9B50[];

extern const char D_004D9AC0[];

extern const char D_004D9B48[];

extern const char D_004D9890[];

extern const char D_004D9810[];

extern const char D_004D9B38[];

extern const char D_004D9A20[];

extern const char D_004D9B58[];

extern const char D_004D98F0[];

extern const char D_004D9A50[];

extern const char D_004D9C10[];

extern const char D_004D9A10[];

extern const char D_004D9B68[];

extern const char D_004D9B80[];

extern const char D_004D99A0[];

extern const char D_004D98A0[];

extern const char D_004D97D8[];

extern const char D_004D9B00[];

extern const char D_004D9840[];

extern const char D_004D99D8[];

extern const char D_004D9910[];

extern const char D_004D9850[];

extern const char D_004D9BA0[];

extern const char D_004D99C8[];

extern const char D_004D9A98[];

extern const char D_004D9880[];

extern const char D_004D9BA8[];

extern const char D_004D9998[];

extern const char D_004D99F0[];

extern const char D_004D9C28[];

extern const char D_004D9908[];

extern const char D_004D98D0[];

extern const char D_004D97E0[];

extern const char D_004D9B70[];

extern const char D_004D9858[];

extern const char D_004D9860[];

extern const char D_004D9BE8[];

extern const char D_004D9988[];

extern const char D_004D9980[];

extern const char D_004D9BD8[];

extern const char D_004D9C20[];

extern const char D_004D9AC8[];

extern const char D_004D9968[];

extern const char D_004D9B98[];

extern const char D_004D9B30[];

extern const char D_004D98B0[];

extern const char D_004D9A18[];

extern const char D_004D9A28[];

extern const char D_004D9B08[];

extern const char D_004D9900[];

extern const char D_004D98C8[];

extern const char D_004D9830[];

extern const char D_004D9BB0[];

extern const char D_004D9AD8[];

extern const char D_004D9A00[];

extern const char D_004D9950[];

extern const char D_004D9BE0[];

extern const char D_004D99C0[];

extern const char D_004D9B10[];

extern const char D_004D9948[];

extern const char D_004D98E0[];

extern const char D_004D9AA8[];

extern const char D_004D9A48[];

extern const char D_004D9990[];

extern const char D_004D97F0[];

extern const char D_004D98A8[];

static const char * CID0[54] = { D_004BF8E0, D_004D8BB0, D_004D8BA8, D_004D8BA0, D_004D8B98, D_004D8B90, D_004D8B88, D_004BF8D0, D_004BF8C0, D_004D8B80, D_004D8B78, D_004D8B70, D_004BF8B0, D_004D8B68, D_004D8B60, D_004D8B58, D_004D8B50, D_004D8B48, D_004D8B40, D_004D8B38, D_004D8B30, D_004D8B28, D_004D8B20, D_004D8B18, D_004D8B10, D_004D8B08, D_004D8B00, D_004D8AF8, D_004D8AF0, D_004BF898, D_004BF888, D_004D8AE8, D_004D8AE0, D_004D8AD8, D_004D8AD0, D_004BF878, D_004BF868, D_004BF858, D_004BF848, D_004BF838, D_004BF828, D_004BF818, D_004BF800, D_004D8AC8, D_004BF7F0, D_004BF7E0, D_004D8AC0, D_004BF7C8, D_004BF7B0, D_004D8AB8, D_004D8AB0, D_004D8AA8, D_004BF7A0, 0 };

extern const char D_004D9C58[];

static const char * CID1[89] = { D_004D8D80, D_004BFB08, D_004D8D78, D_004BFAF8, D_004D8D70, D_004D8D68, D_004D8D60, D_004D8D58, D_004D8D50, D_004D8D48, D_004D8D40, D_004D8D38, D_004D8D30, D_004D8D28, D_004D8D20, D_004D8D18, D_004D8D10, D_004D8D08, D_004D8D00, D_004BFAE8, D_004D8CF8, D_004D8CF0, D_004D8CE8, D_004D8CE0, D_004D8CD8, D_004D8CD0, D_004D8CC8, D_004D8CC0, D_004D8CB8, D_004D8CB0, D_004D8CA8, D_004D8CA0, D_004D8C98, D_004D8C90, D_004D8C88, D_004D8C80, D_004D8C78, D_004D8C70, D_004D8C68, D_004D8C60, D_004D8C58, D_004D8C50, D_004BFAD8, D_004D8C48, D_004D8C40, D_004BFAC8, D_004D8C38, D_004D8C30, D_004D8C28, D_004BFAB8, D_004BFAA8, D_004D8C20, D_004D8C18, D_004D8C10, D_004BFA98, D_004D8C08, D_004BFA88, D_004BFA78, D_004BFA68, D_004D8C00, D_004D8BF8, D_004BFA58, D_004D8BF0, D_004BFA40, D_004D8BE8, D_004BFA30, D_004BFA20, D_004BFA10, D_004D8BE0, D_004D8BD8, D_004D8BD0, D_004D8BC8, D_004BFA00, D_004BF9F0, D_004BF9E0, D_004BF9C8, D_004BF9B0, D_004BF998, D_004BF980, D_004BF968, D_004BF950, D_004BF938, D_004BF928, D_004BF918, D_004BF900, D_004D8BC0, D_004BF8F0, D_004D8BB8, 0 };

extern const char D_004D9C50[];

static const char * CID2[32] = { D_004D8E50, D_004D8E48, D_004D8E40, D_004D8E38, D_004D8E30, D_004D8E28, D_004D8E20, D_004D8E18, D_004D8E10, D_004D8E08, D_004D8E00, D_004D8DF8, D_004D8DF0, D_004D8DE8, D_004D8DE0, D_004D8DD8, D_004D8DD0, D_004D8DC8, D_004D8DC0, D_004D8DB8, D_004D8DB0, D_004D8DA8, D_004D8DA0, D_004D8D98, D_004D8D90, D_004BFB58, D_004BFB48, D_004BFB38, D_004BFB28, D_004D8D88, D_004BFB18, 0 };

extern const char D_004D9C48[];

static const char * CID3[29] = { D_004D8EE0, D_004D8ED8, D_004D8ED0, D_004D8EC8, D_004D8EC0, D_004D8EB8, D_004D8EB0, D_004D8EA8, D_004D8EA0, D_004D8E98, D_004D8E90, D_004D8E88, D_004D8E80, D_004D8E78, D_004D8E70, D_004D8E68, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E60, D_004D8E58, 0 };

static const char * CID4[17] = { D_004D8F50, D_004D8F48, D_004D8F40, D_004D8F38, D_004D8F30, D_004D8F28, D_004D8F20, D_004D8F18, D_004D8F10, D_004D8F08, D_004D8F00, D_004D8F00, D_004D8F00, D_004D8EF8, D_004D8EF0, D_004D8EE8, 0 };

static const char * CID5[10] = { D_004D8F98, D_004D8F90, D_004D8F88, D_004D8F80, D_004D8F78, D_004D8F70, D_004D8F68, D_004D8F60, D_004D8F58, 0 };

static const char * CID6[81] = { D_004D91B8, D_004D91B0, D_004D91A8, D_004D91A0, D_004D9198, D_004D9190, D_004D9188, D_004D9180, D_004D9178, D_004D9170, D_004D9168, D_004D9160, D_004D9158, D_004D9150, D_004D9148, D_004D9140, D_004D9138, D_004D9130, D_004D9128, D_004BFC18, D_004BFC08, D_004D9120, D_004BFBF8, D_004BFBE8, D_004D9118, D_004D9110, D_004D9108, D_004D9100, D_004D90F8, D_004D90F0, D_004D90E8, D_004D90E0, D_004D90D8, D_004D90D0, D_004D90C8, D_004D90C0, D_004D90B8, D_004D90B0, D_004D90A8, D_004D90A0, D_004D9098, D_004D9090, D_004D9088, D_004D9080, D_004D9078, D_004D9070, D_004D9068, D_004D9060, D_004D9058, D_004BFBD8, D_004BFBC8, D_004D9050, D_004BFBB8, D_004BFBA8, D_004D9048, D_004D9040, D_004D9038, D_004D9030, D_004D9028, D_004D9020, D_004D9018, D_004D9010, D_004D9008, D_004D9000, D_004D8FF8, D_004D8FF0, D_004D8FE8, D_004D8FE0, D_004D8FD8, D_004BFB98, D_004BFB88, D_004D8FD0, D_004D8FC8, D_004BFB78, D_004BFB68, D_004D8FC0, D_004D8FB8, D_004D8FB0, D_004D8FA8, D_004D8FA0, 0 };

extern const char D_004D9C40[];

extern const char D_004D9C80[];

static const char * RID0[7] = { D_004D91E8, D_004D91E0, D_004D91D8, D_004D91D0, D_004D91C8, D_004D91C0, 0 };

extern const char D_004D9C78[];

extern const char D_004D9C70[];

static const char * RID1[14] = { D_004D9250, D_004D9248, D_004D9240, D_004D9238, D_004D9230, D_004D9228, D_004D9220, D_004D9218, D_004D9210, D_004D9208, D_004D9200, D_004D91F8, D_004D91F0, 0 };

extern const char D_004D9C68[];

static const char * RID2[11] = { D_004D9298, D_004D9290, D_004D9288, D_004D9280, D_004D9278, D_004BFC28, D_004D9270, D_004D9268, D_004D9260, D_004D9258, 0 };

extern const char D_004D9C60[];

extern const char D_004D9D00[];

static const char * WID0[11] = { D_004D92E8, D_004D92E0, D_004D92D8, D_004D92D0, D_004D92C8, D_004D92C0, D_004D92B8, D_004D92B0, D_004D92A8, D_004D92A0, 0 };

extern const char D_004D9CF8[];

extern const char D_004D9CF0[];

static const char * WID1[18] = { D_004D9370, D_004D9368, D_004D9360, D_004D9358, D_004D9350, D_004D9348, D_004D9340, D_004D9338, D_004D9330, D_004D9328, D_004D9320, D_004D9318, D_004D9310, D_004D9308, D_004D9300, D_004D92F8, D_004D92F0, 0 };

extern const char D_004D9CE8[];

extern const char D_004D9CE0[];

static const char * WID2[6] = { D_004D9378, D_004D92B8, D_004D92B0, D_004D92A8, D_004D92A0, 0 };

extern const char D_004D9CD8[];

extern const char D_004D9CD0[];

static const char * WID3[9] = { D_004D93B8, D_004D93B0, D_004D93A8, D_004D93A0, D_004D9398, D_004D9390, D_004D9388, D_004D9380, 0 };

extern const char D_004D9CC8[];

extern const char D_004D9CC0[];

static const char * WID4[6] = { D_004D9378, D_004D93D8, D_004D93D0, D_004D93C8, D_004D93C0, 0 };

extern const char D_004D9CB8[];

extern const char D_004D9CB0[];

static const char * WID5[2] = { D_004D93E0, 0 };

extern const char D_004D9CA8[];

static const char * WID6[50] = { D_004D9568, D_004D9560, D_004D9558, D_004D9550, D_004D9548, D_004D9540, D_004D9538, D_004D9530, D_004D9528, D_004D9520, D_004D9518, D_004D9510, D_004D9508, D_004D9500, D_004D94F8, D_004D94F0, D_004D94E8, D_004D94E0, D_004D94D8, D_004D94D0, D_004D94C8, D_004D94C0, D_004D94B8, D_004D94B0, D_004D94A8, D_004D94A0, D_004D9498, D_004D9490, D_004D9488, D_004D9480, D_004D9478, D_004D9470, D_004D9468, D_004D9460, D_004D9458, D_004D9450, D_004D9448, D_004D9440, D_004D9438, D_004D9430, D_004D9428, D_004D9420, D_004D9418, D_004D9410, D_004D9408, D_004D9400, D_004D93F8, D_004D93F0, D_004D93E8, 0 };

static const char * WID7[12] = { D_004D95B8, D_004D95B0, D_004D95A8, D_004D95A0, D_004D9598, D_004D9590, D_004D9588, D_004D9580, D_004D9578, D_004D9570, D_004BFC38, 0 };

extern const char D_004D9C90[];

static const char * WID8[2] = { D_004D95C0, 0 };

extern const char D_004D9C88[];

extern const char D_004D9D58[];

static const char * EID0[56] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, D_004D9710, D_004D9708, D_004D9700, D_004D96F8, D_004D96F0, D_004D96E8, D_004D96E0, D_004D96D8, D_004D96D0, D_004D96C8, D_004D96C0, D_004D96B8, D_004D96B0, D_004D96A8, D_004D96A0, D_004D9698, D_004D9690, D_004D9688, D_004D9680, D_004D9678, D_004D9670, D_004D9668, D_004D9660, D_004D9658, D_004D9650, D_004D9648, D_004D9640, D_004D9638, D_004D9630, D_004D9628, D_004D9620, D_004D9618, D_004D9610, D_004D9608, D_004D9600, D_004D95F8, D_004D95F0, D_004D95E8, D_004D95E0, D_004D95D8, D_004D95D0, D_004D95D0, D_004D95D0, D_004D95D0, D_004D95C8, 0 };

extern const char D_004BFCD8[];

extern const char D_004D9D50[];

static const char * EID1[15] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, D_004D9710, D_004D9708, D_004D9700, D_004D96F8, 0 };

extern const char D_004BFCC8[];

extern const char D_004D9D48[];

static const char * EID2[7] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, 0 };

extern const char D_004BFCB8[];

extern const char D_004D9D40[];

static const char * EID3[8] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, 0 };

extern const char D_004BFCA8[];

extern const char D_004D9D38[];

static const char * EID4[8] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, 0 };

extern const char D_004BFC98[];

extern const char D_004D9D30[];

static const char * EID5[7] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, 0 };

extern const char D_004BFC88[];

extern const char D_004D9D28[];

static const char * EID6[2] = { D_004D9760, 0 };

extern const char D_004BFC78[];

extern const char D_004D9D20[];

static const char * EID7[5] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, 0 };

extern const char D_004BFC68[];

extern const char D_004D9D18[];

static const char * EID8[4] = { D_004D9760, D_004D9758, D_004D9750, 0 };

extern const char D_004BFC58[];

extern const char D_004D9D10[];

extern const char D_004D9D08[];

static const char * EIDf[34] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, D_004D9710, D_004D9708, D_004D9700, D_004D96F8, D_004D96F0, D_004D96E8, D_004D96E0, D_004D96D8, D_004D96D0, D_004D96C8, D_004D96C0, D_004D96B8, D_004D96B0, D_004D96A8, D_004D96A0, D_004D97A0, D_004D9798, D_004D9790, D_004D9788, D_004D9780, D_004D9778, D_004D9770, D_004D9768, 0 };

extern const char D_004BFC48[];

static const char * MID0[182] = { D_004D9C38, D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, D_004D9710, D_004D9708, D_004D9700, D_004D96F8, D_004D96F0, D_004D96E8, D_004D96E0, D_004D96D8, D_004D96D0, D_004D96C8, D_004D96C0, D_004D96B8, D_004D96B0, D_004D96A8, D_004D96A0, D_004D97A0, D_004D9798, D_004D9790, D_004D9788, D_004D9780, D_004D9778, D_004D9770, D_004D9768, D_004D9C30, D_004D9C28, D_004D9C20, D_004D9C18, D_004D9C10, D_004D9C08, D_004D9C00, D_004D9BF8, D_004D9BF0, D_004D9BE8, D_004D9BE0, D_004D9BD8, D_004D9BD0, D_004D9BC8, D_004D9BC0, D_004D9BB8, D_004D9BB0, D_004D9BA8, D_004D9BA0, D_004D9B98, D_004D9B90, D_004D9B88, D_004D9B80, D_004D9B78, D_004D9B70, D_004D9B68, D_004D9B60, D_004D9B58, D_004D9B50, D_004D9B48, D_004D9B40, D_004D9B38, D_004D9B30, D_004D9B28, D_004D9B20, D_004D9B18, D_004D9B10, D_004D9B08, D_004D9B00, D_004D9AF8, D_004D9AF0, D_004D9AE8, D_004D9AE0, D_004D9AD8, D_004D9AD0, D_004D9AC8, D_004D9AC0, D_004D9AB8, D_004D9AB0, D_004D9AA8, D_004D9AA0, D_004D9A98, D_004D9A90, D_004D9A88, D_004D9A80, D_004D9A78, D_004D9A70, D_004D9A68, D_004D9A60, D_004D9A58, D_004D9A50, D_004D9A48, D_004D9A40, D_004D9A38, D_004D9A30, D_004D9A28, D_004D9A20, D_004D9A18, D_004D9A10, D_004D9A08, D_004D9A00, D_004D99F8, D_004D99F0, D_004D99E8, D_004D99E0, D_004D99D8, D_004D99D0, D_004D99C8, D_004D99C0, D_004D99B8, D_004D95C8, D_004D99B0, D_004D99A8, D_004D99A0, D_004D9998, D_004D9990, D_004D9988, D_004D9980, D_004D9978, D_004D9970, D_004D9968, D_004D9960, D_004D9958, D_004D9950, D_004D9948, D_004D9940, D_004D9938, D_004D9930, D_004D9928, D_004D9920, D_004D9918, D_004D9910, D_004D9908, D_004D9900, D_004D98F8, D_004D98F0, D_004D98E8, D_004D98E0, D_004D98D8, D_004D98D0, D_004D98C8, D_004D98C0, D_004D98B8, D_004D98B0, D_004D98A8, D_004D98A0, D_004D9898, D_004D9890, D_004D9888, D_004D9880, D_004D9878, D_004D9870, D_004D9868, D_004D9860, D_004D9858, D_004D9850, D_004D9848, D_004D9840, D_004D9838, D_004D9830, D_004D9828, D_004D9820, D_004D9818, D_004D9810, D_004D9808, D_004D9800, D_004D97F8, D_004D97F0, D_004D97E8, D_004D97E0, D_004D97D8, D_004D97D0, D_004D97C8, D_004D97C0, D_004D97B8, D_004D97B0, D_004D97A8, 0 };

extern const char D_004BFCE8[];

static const char * AID0[128] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, D_004D9710, D_004D9708, D_004D9700, D_004D96F8, D_004D96F0, D_004D96E8, D_004D96E0, D_004D96D8, D_004D96D0, D_004D96C8, D_004D96C0, D_004D96B8, D_004D96B0, D_004D96A8, D_004D96A0, D_004D97A0, D_004D9798, D_004D9790, D_004D9788, D_004D9780, D_004D9778, D_004D9770, D_004D9768, D_004D9C30, D_004D9C28, D_004D9C20, D_004D9C18, D_004D9C10, D_004D9C08, D_004D9C00, D_004D9BF8, D_004D9BF0, D_004D9BE8, D_004D9BE0, D_004D9BD8, D_004D9BD0, D_004D9BC8, D_004D9BC0, D_004D9BB8, D_004D9BB0, D_004D9BA8, D_004D9BA0, D_004D9B98, D_004D9B90, D_004D9B88, D_004D9B80, D_004D9B78, D_004D9B70, D_004D9B68, D_004D9B60, D_004D9B58, D_004D9B50, D_004D9B48, D_004D9B40, D_004D9B38, D_004D9B30, D_004D9B28, D_004D9B20, D_004D9B18, D_004D9B10, D_004D9B08, D_004D9B00, D_004D9AF8, D_004D9AF0, D_004D9AE8, D_004D9AE0, D_004D9AD8, D_004D9AD0, D_004D9AC8, D_004D9AC0, D_004D9AB8, D_004D9AB0, D_004D9AA8, D_004D9AA0, D_004D9A98, D_004D9A90, D_004D9A88, D_004D9A80, D_004D9A78, D_004D9A70, D_004D9A68, D_004D9A60, D_004D9A58, D_004D9A50, D_004D9A48, D_004D9A40, D_004D9A38, D_004D9A30, D_004D9A28, D_004D9A20, D_004D9A18, D_004D9A10, D_004D9A08, D_004D9A00, D_004D99F8, D_004D99F0, D_004D99E8, D_004D99E0, D_004D99D8, D_004D99D0, D_004D99C8, D_004D99C0, D_004D99B8, D_004D95C8, D_004D99B0, D_004D99A8, D_004D99A0, D_004D9998, D_004D9990, D_004D9988, D_004D9980, D_004D9978, D_004D9970, D_004D9968, D_004D9960, D_004D9958, D_004D9950, 0 };

extern const char D_004BFCF8[];

extern const char D_004D9D88[];

static const char * MU[131] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, D_004D9710, D_004D9708, D_004D9700, D_004D96F8, D_004D96F0, D_004D96E8, D_004D96E0, D_004D96D8, D_004D96D0, D_004D96C8, D_004D96C0, D_004D96B8, D_004D96B0, D_004D96A8, D_004D96A0, D_004D97A0, D_004D9798, D_004D9790, D_004D9788, D_004D9780, D_004D9778, D_004D9770, D_004D9768, D_004D9C30, D_004D9C28, D_004D9C20, D_004D9C18, D_004D9C10, D_004D9C08, D_004D9C00, D_004D9BF8, D_004D9BF0, D_004D9BE8, D_004D9BE0, D_004D9BD8, D_004D9BD0, D_004D9BC8, D_004D9BC0, D_004D9BB8, D_004D9BB0, D_004D9BA8, D_004D9BA0, D_004D9B98, D_004D9B90, D_004D9B88, D_004D9B80, D_004D9B78, D_004D9B70, D_004D9B68, D_004D9B60, D_004D9B58, D_004D9B50, D_004D9B48, D_004D9B40, D_004D9B38, D_004D9B30, D_004D9B28, D_004D9B20, D_004D9B18, D_004D9B10, D_004D9B08, D_004D9B00, D_004D9AF8, D_004D9AF0, D_004D9AE8, D_004D9AE0, D_004D9AD8, D_004D9AD0, D_004D9AC8, D_004D9AC0, D_004D9AB8, D_004D9AB0, D_004D9AA8, D_004D9AA0, D_004D9A98, D_004D9A90, D_004D9A88, D_004D9A80, D_004D9A78, D_004D9A70, D_004D9A68, D_004D9A60, D_004D9A58, D_004D9A50, D_004D9A48, D_004D9A40, D_004D9A38, D_004D9A30, D_004D9A28, D_004D9A20, D_004D9A18, D_004D9A10, D_004D9A08, D_004D9A00, D_004D99F8, D_004D99F0, D_004D99E8, D_004D99E0, D_004D99D8, D_004D99D0, D_004D99C8, D_004D99C0, D_004D99B8, D_004D95C8, D_004D99B0, D_004D99A8, D_004D99A0, D_004D9998, D_004D9990, D_004D9988, D_004D9980, D_004D9978, D_004D9970, D_004D9968, D_004D9960, D_004D9958, D_004D9950, D_004D9948, D_004D9940, D_004D9938, 0 };

extern const char D_004BFD58[];

extern const char D_004D9D80[];

static const char * MUW[21] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, D_004D9710, D_004D9708, D_004D9700, D_004D96F8, D_004D96F0, D_004D96E8, D_004D96E0, D_004D96D8, D_004D96D0, D_004D96C8, 0 };

extern const char D_004BFD48[];

extern const char D_004D9D78[];

static const char * TRE[11] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, 0 };

extern const char D_004BFD38[];

extern const char D_004D9D70[];

static const char * MUX[11] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, 0 };

extern const char D_004BFD28[];

extern const char D_004D9D68[];

static const char * MUF[11] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, 0 };

extern const char D_004BFD18[];

extern const char D_004D9D60[];

static const char * MUS[11] = { D_004D9760, D_004D9758, D_004D9750, D_004D9748, D_004D9740, D_004D9738, D_004D9730, D_004D9728, D_004D9720, D_004D9718, 0 };

extern const char D_004BFD08[];

extern void GameResourceReset(int resource_id);

extern void *GameResourceAlloc(int size);

extern void *GameResourceRealloc(void *address, int size);

static u8 GameResourceWorkReloadLeader;

void (*GameResourceReadFileCallback)(void) = 0;

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

static int resource_get_free(void)
{
    int index;

    for (index = 0; index < 0x80; index++) {
        if (GameResource[index].state == -1)
            return index;
    }
    return -1;
}

u32 GameResourceGetFreeAddr(void)
{
    u32 free_address = GameResource[resource_get_free()].address;

    if (free_address <= 0x01129FFF)
        return 0x0112A000;
    return free_address;
}

u32 GameResourceWorkAlloc(int size)
{
    int index;
    GameResourceEntry *entry;

    GameResourceWorkReloadLeader = 0;
    entry = GameResource;
    GameResourceWorkReloadTable[0] = 0;
    index = 0;
    do {
        if (entry->state == -1) {
            if (entry->size > size)
                return entry->address;
            break;
        }
        index++;
        entry++;
    } while (index < 0x80);
    GameResourceWorkReloadTable[0] = 1;
    return 0x0112A000;
}

void GameResourceWorkReload(void)
{
    if (GameResourceWorkReloadTable[0] != 0) {
        ACT_init();
        GameResourceLoad(GameLoopState.scene_id);
        return;
    }
    MapChangeFadeSet();
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceAlloc);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceRealloc);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", next_arc_size);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", next_line);

static u8 *skip_space(u8 *cursor)
{
    while (*cursor == ' ' || *cursor == '\t')
        cursor++;
    return cursor;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", search_key);

static char *command_reset(int command, char *cursor)
{
    int resource_id = 0;

    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit = (unsigned char)cursor[0];
            int parsed_id;

            cursor++;
            resource_id = resource_id * 10;
            parsed_id = resource_id + digit;
            resource_id = parsed_id - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    GameResourceReset(resource_id);
    return cursor;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadmap);

static char *command_loadact(int command, char *cursor, int mode)
{
    int resource_id = 0;
    char *next_cursor = search_key(cursor, (u8 *)model, &resource_id);

    RES_loadFile(command, 0, resource_id, 0);
    return next_cursor;
}

static char *command_loadface(int command, char *cursor, int mode)
{
    int resource_id = 0;
    char *next_cursor = search_key(cursor, (u8 *)model, &resource_id);

    RES_loadFile(command, 0, resource_id + 0x10000, 0);
    return next_cursor;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadmot);

u8 RES_GetFootStepNo(struct Actor *actor)
{
    ActorResource *resource = actor->resource;
    u8 foot_step_no = 0;

    if (resource != 0) {
        foot_step_no = resource->foot_step_no;
    }
    return foot_step_no;
}

static char *command_loadtex(int command, char *cursor, int mode)
{
    int resource_id = 0;

    (void)mode;
    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit;
            int parsed_id;

            digit = (unsigned char)cursor[0];
            cursor++;
            resource_id = resource_id * 10;
            parsed_id = resource_id + digit;
            resource_id = parsed_id - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    RES_loadFile(command, 0xB, resource_id, 0);
    return cursor;
}

static char *command_loadmovie(int command, char *cursor, int mode)
{
    int resource_id = 0;

    (void)mode;
    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit;
            int parsed_id;

            digit = (unsigned char)cursor[0];
            cursor++;
            resource_id = resource_id * 10;
            parsed_id = resource_id + digit;
            resource_id = parsed_id - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    RES_loadFile(command, 0xA, resource_id, 0);
    return cursor;
}

static void command_loadeffect_sub(int type, char *effect_name, void *address)
{
    if (arcfileaddr != 0) {
        sefLoadMemoryEffectCfName(address, effect_name, next_arc_size(address));
        return;
    }
    sefLoadEffectCfName((int)address, effect_name);
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadeffect);

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

static char *command_player(int command, char *cursor, int mode)
{
    GameCfPlayerLoadResource(0);
    return cursor;
}

static int command_dummy(int resource_id, int result)
{
    if (GameResource[resource_id + 1].address == 0)
        GameResourceAlloc(0);
    else
        GameResourceRealloc((void *)GameResource[resource_id].address, 0);
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", RES_GetLeaderSeName);

void RES_GetMapEnvSeName(char *name)
{
    char *env = env_name;

    while ((*name = *env) != 0) {
        env++;
        name++;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadse);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_loadsmd);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", command_mpeg2_core);

static char *command_mpeg2(int command, char *cursor)
{
    return command_mpeg2_core(command, cursor, 2);
}

static char *command_mpeg2battle(int command, char *cursor, int mode)
{
    SCRIPT_fade(2);
    return command_mpeg2_core(command, cursor, 1);
}

static char *command_mpeg2nofade(int command, char *cursor)
{
    return command_mpeg2_core(command, cursor, 5);
}

static char *command_event2battle(int command, char *cursor, int mode)
{
    SCRIPT_frameLock2Battle();
    return cursor;
}

static char *command_player_lock(int command, char *cursor, int mode)
{
    GameLoopState.flags |= 0x8000;
    return cursor;
}

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

static char *command_save(int command, char *cursor, int mode)
{
    int save_number = 0;

    (void)mode;
    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit = (unsigned char)cursor[0];
            int parsed_number;

            cursor++;
            save_number = save_number * 10;
            parsed_number = save_number + digit;
            save_number = parsed_number - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    Intermission(save_number);
    return cursor;
}

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

int GameResourceGetIndex(int address)
{
    int index = 0;
    GameResourceEntry *entry = GameResource;
    int resourceAddress;
    int result;

    do {
        if (index >= 0x80)
            break;
        resourceAddress = entry->address;
        entry++;
        result = index++;
        if (resourceAddress == address)
            return result;
    } while (resourceAddress != 0);
    return -1;
}

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceDump);

INCLUDE_ASM("asm/main/nonmatchings/res_get_path", GameResourceReset);

void GameResourceInit(u32 address, int size)
{
    u32 *resource_buffer;
    int remaining = 0x7F;
    int free_state = -1;
    GameResourceEntry *entry = GameResource;

    do {
        remaining--;
        entry->address = 0;
        entry->size = 0;
        entry->handle = 0;
        entry->state = free_state;
        entry++;
    } while (remaining >= 0);

    GameResource[0].address = address;
    GameResource[0].size = size;
    GameResourceReset(0);
    /* Reset the first word of each fixed-address resource buffer in EE RAM.
     * These addresses lie beyond the ELF's allocated data sections. */
    resource_buffer = (void *)0x01000000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x01070800;
    resource_buffer[0] = 0;
    GameResourceReadFileCallback = default_callback;
    resource_buffer = (void *)0x010B1000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x010B4000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x010E6000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x010E7000;
    resource_buffer[0] = 0;
}
