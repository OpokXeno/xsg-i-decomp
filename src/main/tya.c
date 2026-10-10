#include "common.h"

#include "shared.h"

typedef struct ActorModelParam {
    float scale;
    u8 unmodeled_04[0x80 - 0x04];
    u32 flags;
    u8 unmodeled_84[0xA0 - 0x84];
    float transparency;
    u8 unmodeled_a4[0xB8 - 0xA4];
    int alpha;
    u8 unmodeled_bc[0x150 - 0xBC];
} ActorModelParam;

typedef struct {
    u8 unmodeled_00[0x38];
    u64 buffer_select;
} TyaDisplayEnvironment;

extern TyaDisplayEnvironment DispEnv;

typedef struct {
    u8 unmodeled_00[8];
    u16 depth_base_page;
    u8 unmodeled_0a[0x12 - 0x0A];
    u16 framebuffer_base_page[2];
} TyaFramebufferState;

extern TyaFramebufferState sRender;

/*
 * The engine's actor record (`actor`, 64-entry array at main 0x0043c1e0,
 * 0xa70-byte stride; fuller evidence in src/main/near_dir.h,
 * src/main/enemy_2.h, src/main/set_motion.h and src/main/db_light_write.h,
 * which names the same two fields for the same reason). tyaCaptureShadow
 * only reads +0x00 flags (bits 0x8 and 0x20) and +0x86, the in-use id
 * ACT_create writes and ACT_update skips a slot on when it is zero; the
 * offsets between them are not evidenced by this TU and stay unmodeled.
 */

#define ACTOR_COUNT 64

#define ACTOR_IN_USE_ID_OFFSET 0x86

typedef struct {
    u32 flags;
    u8 unmodeled_04[4];
    void (*draw)(void *self);
    u8 unmodeled_0c[0x80 - 0x0C];
    u8 captureLayer;
    u8 unmodeled_81[ACTOR_IN_USE_ID_OFFSET - 0x81];
    short inUseId;
    u8 unmodeled_88[0x8FC - (ACTOR_IN_USE_ID_OFFSET + 2)];
    void *parent;
    u8 unmodeled_900[0x920 - 0x900];
    ActorModelParam model;
} ActorHead;

extern ActorHead actor[ACTOR_COUNT];

/* The scene's active capture layer: compared against a per-array base
 * (0x200 for the actor array here, 0x300 for MapUnit below) plus the slot
 * index, so only the one actor or map unit the current capture pass wants
 * is drawn. */

static u16 layer;

/* Defined in src/main/act_3.c (main/tu265). */

void ACT_DrawShadowBegin(void);

void ACT_DrawShadow(ActorHead *unit);

void ACT_DrawShadowEnd(void);

/*
 * MapUnit[] entries are game-wide unit records (src/main/init_drill.c,
 * src/main/init_uwamono_sys.c and src/main/map_create_unit_peer.h keep
 * their own views of the same array); only the field this TU touches is
 * named. tyaCaptureUnit reads +0xA4 serial, the same field
 * src/main/init_uwamono_sys.c's UwamonoCommonUnit and
 * src/main/init_drill.c's DrillMapUnit name.
 */

#define MAP_UNIT_COUNT 64

#define MAP_UNIT_SERIAL_OFFSET 0xA4

typedef struct {
    u8 unmodeled_00[MAP_UNIT_SERIAL_OFFSET];
    short serial;
    u8 unmodeled_a6[0xFC - (MAP_UNIT_SERIAL_OFFSET + 2)];
    ActorHead *owner;
    u8 unmodeled_100[0x300 - 0x100];
} MapUnitHead;

extern MapUnitHead MapUnit[MAP_UNIT_COUNT];

/* Defined in src/main/map_2.c (main/tu269). */

void MAP_drawUnitAt(MapUnitHead *unit);

typedef int (*TyaCaptureCallback)(int);

typedef struct TyaGameLoop {
    u8 unmodeled_00[0x10];
    u32 flags;
    u8 unmodeled_14[0x30 - 0x14];
    void (*hook[4])(void);
    int (*captureTask)(int);
} TyaGameLoop;

extern TyaGameLoop GameLoopState;

extern u8 *WorkEnd;

/* The scene's active capture layer, compared against each array's base. */

int sceGsSyncPath(int mode, int timeout);

int sceOpen(const char *path, int flags, ...);

int sceClose(int fd);

int sceWrite(int fd, const void *buffer, int size);

int sceGsSetDefStoreImage(void *image, short sbp, short sbw, short spsm, short x, short y,
                          short w, short h);

int sceGsExecStoreImage(void *image, void *destination);

void *memcpy(void *destination, const void *source, unsigned int size);

int printf(const char *format, ...);

float I2F(int value);

int xglFontGetFlags(void);

void xglFontSetFlags(int flags);

void *GameResourceGetFreeAddr(void);

/* Capture transfers reuse 0x16C0 scratchpad bytes. Save and restore that
 * whole block in WorkEnd around the GS image/depth reads. */
enum { CAPTURE_SCRATCHPAD_SIZE = 0x16C0 };

typedef struct {
    u8 unmodeled_00[0x16B0];
    float depth_result;
} TyaScratchpadDepthResult;

#define SCRATCHPAD ((u8 *)0x70000000)

#define SPR_COLOR ((u32 *)0x70000070)

#define SPR_DEPTH ((u32 *)0x70000870)

#define SPR_RLE ((u8 *)0x70001070)

#define RGB_MASK 0xFFFFFF

#define ALPHA_MASK 0xFF000000

/* The GS CSR is a 64-bit MMIO status register. Capture waits for its
 * FINISH bit, so each iteration must read the hardware again. */
#define GS_CSR_ADDRESS 0x12001000

#define CAPTURE_WIDTH 512

#define CAPTURE_HEIGHT 448

extern char *pnum;

extern char *pext;

extern TyaCaptureCallback cascade;

extern void *TmpBuffer;

extern u16 zpicf;

extern int alpha;

typedef struct { int v; } TyaAlpha;

extern int transparency;

static char outpath[256];

static u8 BmpHeader[0x36] = {
    'B', 'M', 0x36, 0x80, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00,
    0x28, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x01, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static u8 SiPicHeader[0x70] = {
    0x53, 0x80, 0xF6, 0x34, 0x40, 0x27, 0xAE, 0x14,
    '(', 'c', ')', 'm', 'o', 'n', 'o', 'l', 'i', 't', 'h', 's', 'o', 'f', 't', 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    'P', 'I', 'C', 'T', 0x02, 0x00, 0x01, 0xC0, 0x3F, 0x80, 0x00, 0x00,
    0x00, 0x03, 0x00, 0x00, 0x01, 0x08, 0x02, 0xE0, 0x00, 0x08, 0x02, 0x10,
};

void ACT_filterGuno(void *unit);

void ACT_modelDraw(ActorHead *unit);

int F2I(float value);

static int getfbp(void)
{
    long long shifted_mode;
    int selector;

    shifted_mode = (long long)(DispEnv.buffer_select << 7);
    selector = shifted_mode >> 32;
    if ((selector & 3) == 1) {
        return sRender.framebuffer_base_page[1] << 5;
    }
    return sRender.framebuffer_base_page[0] << 5;
}

int tyaBmpOutput(char *path, u8 *pixels)
{
    int fd;
    int y;
    int x;
    int fbp;
    u8 *dst;
    u8 *src;
    unsigned long image;

    sceGsSyncPath(0, 0);
    fd = sceOpen(path, 0x602);
    if (fd < 0) {
        return 1;
    }
    sceWrite(fd, BmpHeader, sizeof(BmpHeader));
    __builtin_memcpy(WorkEnd, SCRATCHPAD, CAPTURE_SCRATCHPAD_SIZE);
    fbp = getfbp();
    image = 0x70000070;
    dst = pixels;
    for (y = 0; y < CAPTURE_HEIGHT; y++) {
        sceGsSetDefStoreImage(SCRATCHPAD, fbp, 8, 1, 0, CAPTURE_HEIGHT - 1 - y,
                              CAPTURE_WIDTH, 1);
        sceGsExecStoreImage(SCRATCHPAD, (void *)(u32)image);
        src = (u8 *)(u32)image;
        for (x = 0; x < CAPTURE_WIDTH; x++) {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            src += 3;
            dst += 3;
        }
    }
    dst[1] = 1;
    dst[0] = 0;
    dst += 2;
    sceWrite(fd, pixels, dst - pixels);
    sceClose(fd);
    __builtin_memcpy(SCRATCHPAD, WorkEnd, CAPTURE_SCRATCHPAD_SIZE);
    return 0;
}

int tyaSiPicOutput(char *path, char *zpath, u8 *pixels)
{
    u8 zbuffer[2048];
    u32 color;
    u32 alphaRun;
    int fd;
    int zfd;
    int y;
    int i;
    int count;
    int length;
    int fbp;
    u32 zmin;
    u32 zmax;
    u32 z;
    u32 *p;
    unsigned long image;
    u32 *end;
    u32 *rowLimit;
    u32 *zp;
    u32 *pixel;
    u32 *depth;
    int n;
    u8 *out;
    u8 *header;
    u8 *dst;
    u8 *zout;
    u8 *spr;
    u8 *colorBytes;
    TyaScratchpadDepthResult *depth_result;

    spr = SCRATCHPAD;
    depth_result = (TyaScratchpadDepthResult *)spr;
    zmin = 0xFFFFFF;
    zmax = 0;
    sceGsSyncPath(0, 0);
    fd = sceOpen(path, 0x602);
    if (fd < 0) {
        return 1;
    }
    zfd = (zpath != 0 && *zpath != '\0') ? sceOpen(zpath, 0x602) : 0;
    sceWrite(fd, SiPicHeader, sizeof(SiPicHeader));
    __builtin_memcpy(WorkEnd, spr, CAPTURE_SCRATCHPAD_SIZE);
    fbp = getfbp();
    dst = pixels;
    image = 0x70000070;
    for (y = 0; y < CAPTURE_HEIGHT; y++) {
        sceGsSetDefStoreImage(spr, fbp, 8, 0, 0, y, CAPTURE_WIDTH, 1);
        sceGsExecStoreImage(spr, (void *)(u32)image);
        sceGsSetDefStoreImage(spr, sRender.depth_base_page << 5, 8, 0x30, 0, y,
                              CAPTURE_WIDTH, 1);
        end = (u32 *)(spr + 0x870);
        sceGsExecStoreImage(spr, end);
        out = SPR_RLE;

        header = 0;
        count = 0;
        rowLimit = SPR_DEPTH;
        p = (u32 *)(u32)image;
        if (p < rowLimit) do {
            if (end - p >= 3 && (p[0] & RGB_MASK) == (p[1] & RGB_MASK)) {
                if (count != 0) {
                    *header = count - 1;
                    count = 0;
                }
                color = p[0] & RGB_MASK;
                length = 0;
                while (p < end && (*p & RGB_MASK) == color) {
                    p++;
                    length++;
                }
                if (length > 128) {
                    out[0] = 0x80;
                    out[1] = length >> 8;
                    out[2] = length;
                    out += 3;
                } else {
                    *out = length + 127;
                    out++;
                }
                colorBytes = (u8 *)&color;
                out[0] = colorBytes[0];
                out[1] = colorBytes[1];
                out[2] = colorBytes[2];
                out += 3;
            } else {
                if (count == 128) {
                    *header = 127;
                    count = 0;
                }
                if (count == 0) {
                    *out = 0;
                    header = out;
                    out++;
                }
                count++;
                out[0] = ((u8 *)p)[0];
                out[1] = ((u8 *)p)[1];
                out[2] = ((u8 *)p)[2];
                p++;
                out += 3;
            }
        } while (p < end);
        if (count != 0) {
            *header = count - 1;
        }

        for (pixel = SPR_COLOR, depth = SPR_DEPTH, n = 0; n < CAPTURE_WIDTH; n++, pixel++, depth++) {
            u32 value;

            if (alpha != 0) {
                if (((char *)pixel)[3] >= 0 &&
                    (!(GameLoopState.flags & 0x2000000) || (*pixel & RGB_MASK) == 0) &&
                    (*depth & RGB_MASK) == 0) {
                    value = (-1 - ((u8 *)pixel)[3] * 2) & 0xFF;
                } else {
                    value = (255 - transparency * 2) & 0xFF;
                }
            } else {
                if (!(((char *)pixel)[3] >= 0 &&
                    (!(GameLoopState.flags & 0x2000000) || (*pixel & RGB_MASK) == 0) &&
                    (*depth & RGB_MASK) == 0)) {
                    value = 0;
                } else {
                    value = (-1 - ((u8 *)pixel)[3] * 2) & 0xFF;
                }
            }
            ((u8 *)pixel)[3] = ~value;
        }

        p = SPR_COLOR;
        header = 0;
        count = 0;
        for (; p < end;) {
            if (end - p >= 3 && (p[0] & ALPHA_MASK) == (p[1] & ALPHA_MASK)) {
                if (count != 0) {
                    *header = count - 1;
                    count = 0;
                }
                alphaRun = p[0] & ALPHA_MASK;
                length = 0;
                while (p < end && (*p & ALPHA_MASK) == alphaRun) {
                    p++;
                    length++;
                }
                if (length > 128) {
                    out[0] = 0x80;
                    out[1] = length >> 8;
                    out[2] = length;
                    out += 3;
                } else {
                    *out = length + 127;
                    out++;
                }
                *out = alphaRun >> 24;
                out++;
            } else {
                if (count == 128) {
                    *header = 127;
                    count = 0;
                }
                if (count == 0) {
                    *out = 0;
                    header = out;
                    out++;
                }
                count++;
                *out = ((u8 *)p)[3];
                p++;
                out++;
            }
        }
        if (count != 0) {
            *header = count - 1;
        }

        memcpy(dst, SPR_RLE, out - spr - 0x1070);
        dst = dst + (out - spr) - 0x1070;

        if (zfd != 0) {
            zp = SPR_DEPTH;
            zout = zbuffer;
            for (i = 0; i < CAPTURE_WIDTH; i++) {
                z = RGB_MASK - (*zp++ & RGB_MASK);
                if (z != 0 && z != RGB_MASK) {
                    if (z < zmin) {
                        zmin = z;
                    }
                    if (zmax < z) {
                        zmax = z;
                    }
                }
                depth_result->depth_result = I2F(z);
                zout[0] = spr[0x16B3];
                zout[1] = spr[0x16B2];
                zout[2] = spr[0x16B1];
                zout[3] = spr[0x16B0];
                zout += 4;
            }
            sceWrite(zfd, zbuffer, sizeof(zbuffer));
        }
    }
    sceWrite(fd, pixels, dst - pixels);
    if (zfd != 0) {
        printf("zmm:%06x-%06x\n", zmin, zmax);
    }
    sceClose(fd);
    if (zfd != 0) {
        sceClose(zfd);
    }
    __builtin_memcpy(spr, WorkEnd, CAPTURE_SCRATCHPAD_SIZE);
    return 0;
}

static int tyaCaptureMain(int command)
{
    static unsigned char flag;
    TyaCaptureCallback *savedCapture = &cascade;
    char buffer[256];
    char *src;
    char *dst;
    int i;
    long long shifted_mode;
    int selector;

    switch (command) {
    case 0:
        if (GameLoopState.captureTask != tyaCaptureMain) {
            *savedCapture = GameLoopState.captureTask;
            GameLoopState.captureTask = tyaCaptureMain;
        }
        flag = 1;
        break;
    case 5:
        if (flag == 0) {
            flag = 1;
            break;
        }
        shifted_mode = (long long)(DispEnv.buffer_select << 7);
        selector = shifted_mode >> 32;
        if ((selector & 3) == 1) {
            while (!(*(volatile u64 *)GS_CSR_ADDRESS & 0x10)) {
                for (i = 0; i < 0x100000; i++) {
                }
            }
        }
        if (layer == 0) {
            printf(" bmp:%s\n", outpath);
            tyaBmpOutput(outpath, TmpBuffer);
        } else {
            printf(" pic:%s\n", outpath);
            if (zpicf == 0) {
                dst = buffer;
                for (src = outpath; src < pext; src++) {
                    *dst++ = *src;
                }
                dst[0] = 'z';
                dst[1] = 'p';
                dst[2] = 'i';
                dst[3] = 'c';
                dst[4] = '\0';
                printf("zpic:%s\n", buffer);
            } else {
                buffer[0] = '\0';
            }
            tyaSiPicOutput(outpath, buffer, TmpBuffer);
        }
        if (pnum[3] == '9') {
            pnum[3] = '0';
            if (pnum[2] == '9') {
                pnum[2] = '0';
                if (pnum[1] == '9') {
                    pnum[1] = '0';
                    pnum[0]++;
                } else {
                    pnum[1]++;
                }
            } else {
                pnum[2]++;
            }
        } else {
            pnum[3]++;
        }
        if (flag != 2) {
            break;
        }
    case 1:
        GameLoopState.hook[1] = 0;
        GameLoopState.hook[0] = 0;
        GameLoopState.hook[2] = 0;
        GameLoopState.captureTask = *savedCapture;
        break;
    case 2:
    case 3:
    case 4:
    default:
        break;
    }
    if (*savedCapture != 0) {
        (*savedCapture)(command);
    }
    return 0;
}

static void tyaCaptureNull(void)
{
}

static void tyaCaptureActor(void)
{
    int i;
    int *alpha_value;
    ActorHead *unit;

    alpha_value = &alpha;
    for (i = 0, unit = actor; i < ACTOR_COUNT; i++, unit++) {
        if (unit->inUseId == 0 || (unit->flags & 8)) {
            continue;
        }
        if (layer != i + 0x200) {
            if (unit->parent == 0 || layer != ((ActorHead *)unit->parent)->captureLayer + 0x200) {
                continue;
            }
        }
        if (unit->draw == ACT_filterGuno) {
            *alpha_value = 1;
            transparency = F2I(unit->model.scale * 128.0f);
        } else {
            ActorModelParam *model = &unit->model;

            if (model->flags & 1) {
                *alpha_value = model->alpha;
                transparency = F2I(model->transparency);
            } else {
                *alpha_value = 0;
            }
        }
        ACT_modelDraw(unit);
    }
}

static void tyaCaptureShadow(void)
{
    int i;
    ActorHead *unit;

    unit = actor;
    i = 0;
    ACT_DrawShadowBegin();
    for (; i < ACTOR_COUNT; i++, unit++) {
        if (unit->inUseId != 0 && !(unit->flags & 8) &&
            layer == i + 0x200 && (unit->flags & 0x20)) {
            ACT_DrawShadow(unit);
        }
    }
    ACT_DrawShadowEnd();
}

static void tyaCaptureUnit(void)
{
    int i;
    MapUnitHead *unit;

    for (i = 0, unit = MapUnit; i < MAP_UNIT_COUNT; i++, unit++) {
        if (unit->serial >= 0 && layer == i + 0x300) {
            MAP_drawUnitAt(unit);
        }
    }
}

static void tyaCaptureUnit2(void)
{
    int i;
    ActorHead *unit;
    ActorHead *selected;
    MapUnitHead *mapUnit;

    selected = (ActorHead *)-1;
    for (i = 0, unit = actor; i < ACTOR_COUNT; i++, unit++) {
        if (unit->inUseId != 0 && !(unit->flags & 8) && layer == i + 0x200) {
            selected = unit;
            break;
        }
    }
    for (i = MAP_UNIT_COUNT - 1, mapUnit = MapUnit; i >= 0; i--, mapUnit++) {
        if (mapUnit->serial >= 0 && mapUnit->owner == selected) {
            MAP_drawUnitAt(mapUnit);
        }
    }
}

void tyaCaptureStart(const char *name, int mode)
{
    static char head[] = "host0:/home/xeno/capture/";
    char *dst;
    const char *src;

    TmpBuffer = GameResourceGetFreeAddr();
    printf("CaptureBuffer:%08x\n", TmpBuffer);
    layer = mode & 0x7FFF;
    zpicf = mode & 0x8000;
    alpha = 0;
    dst = outpath;
    for (src = head; *src != '\0'; src++) {
        *dst++ = *src;
    }
    src = name;
    if (*src == '/') {
        src++;
    }
    for (; *src != '\0'; src++) {
        *dst++ = *src;
    }
    if (dst[-1] != '/') {
        *dst++ = '/';
    }
    if (layer == 0) {
        dst[0] = 'p';
        dst[1] = 'x';
    } else {
        dst[0] = (layer >> 8) + 'b';
        dst[1] = (layer & 0xF) + 'a';
    }
    dst += 2;
    pnum = dst;
    dst[0] = '0';
    dst[1] = '0';
    dst[2] = '0';
    dst[3] = '0';
    dst[4] = '.';
    dst += 5;
    pext = dst;
    if (layer == 0) {
        dst[0] = 'b';
        dst[1] = 'm';
        dst[2] = 'p';
    } else {
        dst[0] = 'p';
        dst[1] = 'i';
        dst[2] = 'c';
    }
    dst[3] = '\0';
    tyaCaptureMain(0);
    GameLoopState.hook[0] = tyaCaptureNull;
    GameLoopState.hook[1] = tyaCaptureNull;
    GameLoopState.hook[2] = tyaCaptureNull;
    GameLoopState.hook[3] = tyaCaptureNull;
    GameLoopState.flags |= 0x4000000;
    xglFontSetFlags(xglFontGetFlags() & 0xFFFC);
    if ((layer >> 8) != 0) {
        GameLoopState.flags |= 0x1000000;
    }
    switch (layer >> 8) {
    case 0:
        GameLoopState.hook[0] = 0;
        GameLoopState.hook[1] = 0;
        GameLoopState.hook[2] = 0;
        GameLoopState.hook[3] = 0;
        GameLoopState.flags &= ~0x4000000;
        xglFontSetFlags((u16)xglFontGetFlags() | 1);
        return;
    case 1:
        GameLoopState.hook[0] = 0;
        break;
    case 2:
        GameLoopState.hook[1] = tyaCaptureActor;
        GameLoopState.hook[2] = tyaCaptureShadow;
        GameLoopState.hook[3] = tyaCaptureUnit2;
        break;
    case 3:
        GameLoopState.hook[3] = tyaCaptureUnit;
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        break;
    case 8:
        GameLoopState.flags &= ~0x4000000;
        break;
    case 0x10:
        xglFontSetFlags((u16)xglFontGetFlags() | 1);
        break;
    }
}

void tyaCaptureEnd(void)
{
    tyaCaptureMain(1);
    xglFontSetFlags((unsigned short)(xglFontGetFlags() | 3));
    GameLoopState.flags &= 0xFAFFFFFF;
}
