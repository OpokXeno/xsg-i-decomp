#include "common.h"
#include "shared.h"
#include "main/xgl_sound.h"
#include "main/xgl_studio.h"

typedef struct CfActor {
    u32 flags;
    u8 unmodeled_04[12];
    float x;
    float y;
    float z;
} CfActor;

typedef struct CfClearEnvironment {
    u8 unmodeled_00[32];
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
} CfClearEnvironment;

typedef struct CfGameLoopState {
    u32 unmodeled_00;
    CfActor *current_actor;
    u8 unmodeled_08[0x2A030 - 8];
} CfGameLoopState;

typedef struct TextureTransferHeader {
    u8 unmodeled_00[16];
    int width;
    int height;
    u8 unmodeled_18[40];
    u8 payload[1];
} TextureTransferHeader;

/* The original loads read PadData at +0x28 as a 64-bit snapshot and as
 * halfwords at +0x2a and +0x2c. */
typedef struct SePacketPad {
    u8 unmodeled_00[0x28];
    union {
        u64 snapshot;
        struct {
            u16 held;
            u16 pressed;
            u16 repeat;
            u16 unmodeled_2e;
        } buttons;
    } input;
} SePacketPad;

extern SePacketPad PadData;
extern struct SoundWork SoundWork;
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void xglSoundLoadEffect(const char *name, int address, int bank);
extern void xglMakeSePacket(int command, int handle, int volume, ...);
extern const char D_004C9BF0[];
extern const char D_004C9C08[];
extern const char D_004C9C20[];
extern const char D_004C9C38[];
extern const char D_004C9C50[];
/* EUC-JP circle, triangle and square font codes prefix these test labels. */
extern const char D_004C9C68[];
extern const char D_004C9C78[];
extern const char D_004C9C88[];
extern const char D_004C9CA0[];
extern const char D_004C9D08[];
extern const char D_004C9D18[];
extern const char D_004C9D28[];
/* EUC-JP sound-packet test label with the font's line-start control byte. */
extern const char D_004C9D38[];
extern const char D_004C9D50[];
extern const char D_004C9D60[];
/* Font control byte followed by the EUC-JP title 唐鎌. */
extern const char D_004DB6A8[];
extern const char D_004DB6B8[];
extern int printf(const char *format, ...);
extern CfClearEnvironment ClearEnv;
extern void xglRenderClearFrame(void);
extern void xglRenderClearDepth(void);
extern void CfTest(void);
extern void SePacketTest(void);
extern void TexTest(void);
typedef struct ScratchpadPrimitiveVertex {
    int x;
    int y;
    int z;
    int w;
} ScratchpadPrimitiveVertex;

typedef struct ScratchpadPrimitiveBuffer {
    void *next;
    void *end;
    signed char vertex_count;
    signed char stride;
    u8 unmodeled_0a[0x16];
    ScratchpadPrimitiveVertex *vertices;
    u8 unmodeled_24[4];
    void *colors;
} ScratchpadPrimitiveBuffer;

extern int FrameCount;
static u64 rgba_0 = 0x0000FF00000000FFULL;
extern float I2F(int value);
extern void xglPrimAddGouraudStripN(void *buffer, int count);
static CfActor *tActor;
signed char printflg = 0;
signed char collflg = 0;
extern CfGameLoopState GameLoopState;
extern void xglLightSetDefault(StudioLight *light);
extern void xglCdInitial(void);
extern void xglCdReset(void);
extern void xglCdSetCallback(int callback);
extern void GameResourceInit(int resource_bytes, int work_bytes);
extern void ACT_init(void);
extern void ACT_DrawShadowInit(void);
extern void MSG_init(void);
extern void TWSYS_init(void);
extern void MapInit(void);
extern void MAP_initUnit(void);
extern void Enemy_SystemInit(void);
extern void InitUwamonoSys(void);
extern void GameCfPlayerMoveInit(void);
extern CfActor *ACT_create(int type, int id);
extern void ACT_initMotion(CfActor *actor);
extern void ACT_loadMotion(CfActor *actor, int motion_id, int category);
extern void ACT_loadResource(CfActor *actor, int resource_id);
extern void ACT_setMotion(CfActor *actor, int motion_id);
typedef struct GifAdEntry {
    u64 value;
    u64 register_address;
} GifAdEntry;

typedef struct TexTestPacket {
    u64 gif_control;
    u64 register_descriptors;
    GifAdEntry registers[4];
} TexTestPacket;

static TexTestPacket TestEnv_2_0036DE40 = {
    0x4000000000008001ULL,
    0x000000000000EEEEULL,
    {
        {0x0004380000000000ULL, 0x50},
        {0, 0x51},
        {0x0000010000000100ULL, 0x52},
        {0, 0x53}
    }
};
extern void FlushCache(int mode);
extern void xglDmaDirectNormal(u32 channel, u32 address, u32 count);

void KarakamaTest(void)
{
    SePacketPad *pad;
    u64 exit_buttons = 0x8000100ULL;

    xglRenderClearFrame();
    ClearEnv.alpha = 255;
    ClearEnv.red = 0;
    ClearEnv.green = 0;
    ClearEnv.blue = 0;
    printf(D_004C9BF0, 0x163B0);
    printf(D_004C9C08, 0xA70);
    printf(D_004C9C20, 0x300);
    printf(D_004C9C38, 0x38B0);
    printf(D_004C9C50, 0xAB0);
    pad = &PadData;

    for (;;) {
        xglSleep();
        xglFontDebugPrintf(0, 0, D_004DB6A8);
        xglFontDebugPrintf(0, 16, D_004C9C68);
        xglFontDebugPrintf(0, 32, D_004C9C78);
        xglFontDebugPrintf(0, 48, D_004C9C88);
        xglFontDebugPrintf(0, 64, D_004C9CA0);

        if ((pad->input.snapshot & exit_buttons) != exit_buttons) {
            if (pad->input.buttons.pressed & 0x20) {
                CfTest();
            }
            if (pad->input.buttons.pressed & 0x10) {
                TexTest();
            }
            if (pad->input.buttons.pressed & 0x80) {
                SePacketTest();
            }
            if ((pad->input.buttons.pressed & 0x40) == 0) {
                xglRenderClearFrame();
                ClearEnv.red = 0;
                ClearEnv.green = 0;
                ClearEnv.blue = 0;
                ClearEnv.alpha = 0;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    xglRenderClearDepth();
}

INCLUDE_ASM("asm/main/nonmatchings/karakama_test", CfTest);

void SePacketTest(void)
{
    int sound_id = 0;
    int pitch = 0;
    int volume = 127;
    int pan = 64;
    u16 pressed;
    u16 repeat;
    u16 held;
    int lower_volume;

    printf(D_004C9D08, 0x1000000);
    xglSoundLoadEffect(D_004C9D18, 0x1000000, 1);
    xglSoundLoadEffect(D_004C9D28, 0x1000000, 2);
    xglSoundLoadEffect(D_004DB6B8, 0x1000000, 3);
    if ((PadData.input.snapshot & 0x8000100ULL) == 0x8000100ULL) {
        return;
    }

    do {
        xglFontDebugPrintf(0, 0, D_004C9D38);
        xglFontDebugPrintf(0, 16, D_004C9D50, sound_id);
        xglFontDebugPrintf(0, 32, D_004C9D60, pitch, volume, pan);
        pressed = PadData.input.buttons.pressed;
        if (pressed & 0x20) {
            xglMakeSePacket(124,
                            (SoundWork.effect_banks[sound_id >> 16].handle << 16) +
                                (sound_id & 0xffff), volume, pan, pitch);
        }
        pressed = PadData.input.buttons.pressed;
        if (pressed & 0x40) {
            xglMakeSePacket(126,
                            (SoundWork.effect_banks[sound_id >> 16].handle << 16) +
                                (sound_id & 0xffff), 500, pitch);
        }
        pressed = PadData.input.buttons.pressed;
        repeat = PadData.input.buttons.repeat;
        if (repeat & 0x2000) {
            sound_id++;
        }
        if (repeat & 0x8000 && --sound_id < 0) {
            sound_id = 0;
        }
        if (repeat & 0x1000) {
            pitch++;
        }
        if (repeat & 0x4000 && --pitch < 0) {
            pitch = 0;
        }
        if (pressed & 0x10) {
            sound_id += 0x10000;
        }
        if (pressed & 0x80) {
            sound_id -= 0x10000;
        }
        held = PadData.input.buttons.held;
        lower_volume = held & 2;
        if (held & 8) {
            volume++;
        }
        if (held & 4) {
            pan++;
        }
        pan -= held & 1;
        if (lower_volume) {
            volume--;
        }
        xglSleep();
    } while ((PadData.input.snapshot & 0x8000100ULL) != 0x8000100ULL);
}

void PrimTest(void)
{
    ScratchpadPrimitiveVertex *vertices = (ScratchpadPrimitiveVertex *)0x70000040;
    ScratchpadPrimitiveBuffer *packet = (ScratchpadPrimitiveBuffer *)0x70000000;
    float frame_position;
    float second_frame_position;

    vertices[0].x = 0x7000;
    vertices[0].y = 0x7200;
    vertices[0].z = -1;
    vertices[0].w = 0;
    frame_position = I2F(FrameCount & 0xFFF) * 16.0f;
    vertices[1].z = -1;
    vertices[1].w = 0;
    vertices[1].x = (int)(frame_position + 28672.0f);
    vertices[1].y = (int)(frame_position + 29184.0f);
    second_frame_position = I2F(FrameCount & 0xFFF) * 16.0f;
    vertices[2].z = -1;
    packet->end = &vertices[2];
    vertices[2].w = 0;
    packet->vertices = vertices;
    vertices[2].x = (int)(second_frame_position - 28672.0f);
    vertices[2].y = (int)(second_frame_position - 29184.0f);
    packet->vertex_count = 2;
    packet->next = 0;
    packet->stride = 16;
    packet->colors = &rgba_0;
    xglPrimAddGouraudStripN(packet, 3);
}

INCLUDE_ASM("asm/main/nonmatchings/karakama_test", SetMapUnitTest);

void InitCf(void)
{
    StudioLight *light;

    xglStudioGetLight(&light);
    xglLightSetDefault(light);
    xglRenderClearFrame();
    ClearEnv.red = 0;
    ClearEnv.green = 0;
    ClearEnv.blue = 0;
    ClearEnv.alpha = 0;
    xglCdInitial();
    xglCdReset();
    xglCdSetCallback(0);
    GameResourceInit(0x07000000, 0x06000000);
    ACT_init();
    ACT_DrawShadowInit();
    MSG_init();
    TWSYS_init();
    MapInit();
    MAP_initUnit();
    Enemy_SystemInit();
    InitUwamonoSys();
    GameCfPlayerMoveInit();
    tActor = ACT_create(0, 1);
    ACT_initMotion(tActor);
    ACT_loadMotion(tActor, 1, 1);
    ACT_loadResource(tActor, 1);
    ACT_setMotion(tActor, 2);
    GameLoopState.current_actor = tActor;
    tActor->x = -14.0f;
    tActor->z = 18.0f;
    tActor->flags |= 0x10000;
}

INCLUDE_ASM("asm/main/nonmatchings/karakama_test", TexTest);

void TexTrans(TextureTransferHeader *texture)
{
    int width_in_blocks;
    int width;
    int height;

    width = texture->width;
    height = texture->height;
    width_in_blocks = width;
    if (width < 0) {
        width_in_blocks += 63;
    }
    TestEnv_2_0036DE40.registers[2].value = (u64)width | ((u64)height << 32);
    TestEnv_2_0036DE40.registers[0].value =
        ((u64)(width_in_blocks >> 6) << 48) | ((u64)0xE000 << 30);
    FlushCache(0);
    xglDmaDirectNormal(2, (u32)&TestEnv_2_0036DE40, 5);
    width *= height;
    xglDmaDirectNormal(2, (u32)texture->payload, width / 4 + 1);
}

const char D_004C9BF0[] = "sizeof(sSaveData):%d\n";
const char D_004C9C08[] = "sizeof(sActor):%d\n";
const char D_004C9C20[] = "sizeof(sMapUnit):%d\n";
const char D_004C9C38[] = "sizeof(sEnepc):%d\n";
const char D_004C9C50[] = "sizeof(sefScheduler)%d\n";
const char D_004C9C68[16] = "\xA1\xFB CF Test";
const char D_004C9C78[16] = "\xA2\xA4 Tex Test";
const char D_004C9C88[24] = "\xA2\xA2 SePacket Test";
const char D_004C9CA0[] = "\xA1\xDF return\0\0\0\0\0\0";
const char D_004C9D08[] = "buffer:%08x\n";
const char D_004C9D18[] = "REGISTCF";
const char D_004C9D28[] = "SO_VOK05";
const char D_004C9D38[24] = "\x0B\xA5\xB5\xA5\xA6\xA5\xF3\xA5\xC9\xA5\xD1\xA5\xB1\xA5\xC3\xA5\xC8\xA5\xC6\xA5\xB9\xA5\xC8";
const char D_004C9D50[] = "\x0BSE=%8x\n";
const char D_004C9D60[] = "\x0BID=%8x vol=%3d pan=%3d\n";
const char D_004DB6A8[] = "\x0B\xC5\xE2\xB3\xF9";
const char D_004DB6B8[] = "ENV_VOK";
