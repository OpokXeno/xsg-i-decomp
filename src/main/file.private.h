#ifndef FILE_PRIVATE_H

#define FILE_PRIVATE_H

#include "main/xgl_mc.h"

#include "main/xgl_packet.h"

#include "main/party.h"

typedef struct SaveDataHead00 {
    u64 words[0x38 / 8];
    int unmodeled_38;
    int added_3c;
} SaveDataHead00;

typedef struct SaveDataBlock00 {
    u64 words[(0x163a0 - 0x40) / 8];
} SaveDataBlock00;

typedef struct SaveData00 {
    SaveDataHead00 head;
    SaveDataBlock00 block;
} SaveData00;

typedef struct SaveData01 {
    SaveDataHead00 head;
    int added_40;
    int added_44;
    byte unmodeled_48[8];
    SaveDataBlock00 block;
} SaveData01;

typedef union SaveDataVersions {
    SaveDataHeader header;
    SaveData00 version_00;
    SaveData01 version_01;
} SaveDataVersions;

typedef struct FileSaveRecord {
    u64 version;
    u64 checksum;
    unsigned char unmodeled_10[0x68 - 0x10];
    XglClock elapsed;
    unsigned char unmodeled_70[0x10078 - 0x70];
    unsigned char party[0x188];
    unsigned char unmodeled_10200[0x50];
    short map_number;
    unsigned char chapter;
    unsigned char unmodeled_10253[0x10718 - 0x10253];
    unsigned char leader;
    unsigned char unmodeled_10719[0x15088 - 0x10719];
    u64 play_time;
    unsigned char unmodeled_15090[0x300];
    unsigned char thumbnail[0x1000];
    unsigned char unmodeled_16390[0x20];
} FileSaveRecord;

typedef struct FileObjectSystem {
    unsigned char unmodeled_00[0x67];
    unsigned char state[99];
} FileObjectSystem;

extern FileSaveRecord *FileSaveData;

extern FileObjectSystem *FileObjectDataSystem;

typedef struct PartyStateImage PartyStateImage;

extern int PartyAllPartyGet2(int *out, PartyStateImage *state);

extern void PartyTimeDispChange(XglClock *elapsed);

typedef struct FileSaveData {
    u64 version;
    u64 checksum;
    int save_count;
    int unit_map_count;
    unsigned char unmodeled_18[0x60 - 0x18];
    XglClock clock;
    unsigned char unmodeled_68[0x10250 - 0x68];
    short map_no;
    unsigned char scenario_rank;
    unsigned char unmodeled_10253[0x15390 - 0x10253];
    unsigned char thumbnail[0x1000];
    unsigned char unmodeled_16390[0x163b0 - 0x16390];
} FileSaveOutput;

/* Defining-owner interface proposals: game.c, menu_1.c and party.c. */

typedef struct FileGameLoopState {
    unsigned char unmodeled_00[0x50];
    short map_no;
} FileGameLoopState;

extern FileGameLoopState GameLoopState;

extern void GamePushSaveDataUser(void);

extern unsigned char *MenuSaveMapNameGet(int map_no);

extern int MenuScenarioNoGet(void);

extern XglClock *PartyTimeUpDate(void);

extern void xglClockRead(XglClock *clock);

extern void xglMcSetMapName(const unsigned char *primary, const unsigned char *secondary);

/* e_message.c interface proposal and the FILE explanation context. */

typedef struct FileMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[0x02];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x18 - 0x0c];
    char *text;
} FileMessage;

typedef struct FileExplanation {
    unsigned char unmodeled_00[0x04];
    int color;
    unsigned char open;
    unsigned char unmodeled_09;
    short target_x;
    FileMessage message;
} FileExplanation;

extern void eMessageSet(FileMessage *message, int text);

extern void eMessageMain(FileMessage *message);

extern void MoveSlide(short *current, short *target, float rate);

extern char *msg00_4_0036D658[3];

/* xgl_render.c and SDK packet-builder interface proposals. */

typedef struct FileRenderState {
    unsigned char unmodeled_00[0x14];
    unsigned short draw_back_value;
} FileRenderState;

extern FileRenderState sRender;

extern void sceVif1PkCnt(XglPacket *packet, int count);

extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);

extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

typedef struct FileBackTexturePacket {
    u64 gif_tag;
    u64 registers;
    u64 texture_flush;
    u64 texture_flush_register;
    u64 depth_buffer;
    u64 depth_buffer_register;
    u64 texture;
    u64 texture_register;
    u64 pixel_test;
    u64 pixel_test_register;
    int red;
    int green;
    int blue;
    int alpha_color;
    unsigned int u0;
    unsigned int v0;
    u64 uv0_zero;
    unsigned int x0;
    unsigned int y0;
    u64 z0_and_adc;
    unsigned int u1;
    unsigned int v1;
    u64 uv1_zero;
    unsigned int x1;
    unsigned int y1;
    u64 z1_and_adc;
    u64 restored_depth_buffer;
    u64 restored_depth_buffer_register;
} FileBackTexturePacket;

#endif

