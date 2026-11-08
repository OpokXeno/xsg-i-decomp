#include "common.h"
#include "shared.h"
#include "main/exm_2.h"
#include "main/act_1.h"
#include "main/tya.h"
#include "main/act_2.h"
#include "main/control_entry.h"
#include "main/map_2.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"

/*
 * The engine's actor record (`actor`, 64-entry array at main 0x0043c1e0,
 * 0xa70-byte stride). tyaCaptureShadow only reads +0x00 flags (bits 0x8 and
 * 0x20) and +0x86, the in-use id ACT_create writes and ACT_update skips a
 * slot on when it is zero; the offsets between them are not evidenced by
 * this file and stay unmodeled.
 */
/* MapUnit[] entries are MapUnitRecord (include/main/exm_2.h); tyaCaptureUnit
 * reads its +0xA4 serial and tyaCaptureUnit2 its parent_peer owner. */

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

/* Scratchpad layout of one captured row: colour pixels at +0x70, depth pixels
 * at +0x870, the run-length encoded output from +0x1070; the depth word
 * converted for the Z file lives at +0x16B0. */
#define SPR_DEPTH_OFFSET 0x870
#define SPR_RLE_OFFSET 0x1070
#define CAPTURE_DEPTH_RESULT_OFFSET 0x16B0

/* sceOpen flags: write only, create, truncate. */
#define TYA_OPEN_WRITE_NEW 0x602

/* GS store-image parameters: framebuffer width in 64-pixel units (512 px)
 * and the pixel formats read back. */
#define TYA_BUFFER_WIDTH 8
#define TYA_PSM_CT32 0
#define TYA_PSM_CT24 1
#define TYA_PSM_Z32 0x30

/* Run-length coding of the SiPic rows: a literal block holds up to MAX_RUN
 * values behind a count byte; a run of up to MAX_RUN values is one byte
 * (length + MAX_RUN - 1), longer runs are LONG_RUN_TAG plus a 16-bit length. */
#define TYA_RLE_MAX_RUN 128
#define TYA_RLE_LONG_RUN_TAG 0x80

/* tyaCaptureMain commands (the capture-task protocol of GameLoopState). */
#define TYA_CAPTURE_START 0
#define TYA_CAPTURE_END 1
#define TYA_CAPTURE_FRAME 5

/* tyaCaptureStart mode: low bits select the layer, bit 15 asks for a depth
 * picture. Layers 0x200+n draw actor n, 0x300+n map unit n. */
#define TYA_CAPTURE_LAYER_MASK 0x7FFF
#define TYA_CAPTURE_ZPIC_FLAG 0x8000
#define TYA_LAYER_ACTOR_BASE 0x200
#define TYA_LAYER_MAP_UNIT_BASE 0x300

#define GS_CSR_FINISH 0x10

/* Partial view of GsDispEnv; xgl_render.c's own GsDispEnv definition keeps its GS register types local to that file. */
typedef struct {
    u8 unmodeled_00[0x38];
    u64 buffer_select;
} GsDispEnvBufferView;

typedef struct {
    u8 unmodeled_00[8];
    u16 depth_base_page;
    u8 unmodeled_0a[0x12 - 0x0A];
    u16 framebuffer_base_page[2];
} TyaFramebufferState;

typedef int (*TyaCaptureCallback)(int);

typedef struct TyaGameLoop {
    u8 unmodeled_00[0x10];
    u32 flags;
    u8 unmodeled_14[0x30 - 0x14];
    void (*hook[4])(void);
    int (*captureTask)(int);
} TyaGameLoop;

/* Capture transfers reuse 0x16C0 scratchpad bytes. Save and restore that
 * whole block in WorkEnd around the GS image/depth reads. */
enum { CAPTURE_SCRATCHPAD_SIZE = 0x16C0 };

typedef struct {
    u8 unmodeled_00[CAPTURE_DEPTH_RESULT_OFFSET];
    float depth_result;
} TyaScratchpadDepthResult;

typedef struct { int v; } TyaAlpha;

extern GsDispEnvBufferView DispEnv;
extern TyaFramebufferState sRender;
extern Actor actor[ACTOR_COUNT];
extern MapUnitRecord MapUnit[MAP_UNIT_COUNT];
extern TyaGameLoop GameLoopState;
/* Defined in src/main/act_3.c (main/tu265). */
void ACT_DrawShadowBegin(void);

void ACT_DrawShadow(Actor *unit);
void ACT_DrawShadowEnd(void);

int sceGsSyncPath(int mode, int timeout);

int sceOpen(const char *path, int flags, ...);
int sceClose(int fd);
int sceWrite(int fd, const void *buffer, int size);

int sceGsSetDefStoreImage(void *image, short sbp, short sbw, short spsm, short x, short y,
                          short w, short h);

int sceGsExecStoreImage(void *image, void *destination);
void *memcpy(void *destination, const void *source, unsigned int size);
int printf(const char *format, ...);
void *GameResourceGetFreeAddr(void);
void ACT_filterGuno(Actor *unit);
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

static char outpath[256];

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static char * pnum;

static char * pext;

static TyaCaptureCallback cascade;

static void * TmpBuffer;

/* The scene's active capture layer: compared against a per-array base
 * (0x200 for the actor array here, 0x300 for MapUnit below) plus the slot
 * index, so only the one actor or map unit the current capture pass wants
 * is drawn. */
static u16 layer;

static u16 zpicf;

static int alpha;

static int transparency;

/* Returns the framebuffer base page of the display buffer that DispEnv selects. */
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

/*
 * Writes the scratchpad frame buffer to a 24-bit BMP file at path, converting
 * each row through pixels. Returns 0 on success and 1 if the file cannot be opened.
 */
int tyaBmpOutput(char *path, u8 *pixels)
{
    int fd;
    int y;
    int x;
    int fbp;
    u8 *dst;
    u8 *src;
    unsigned long image; /* 64-bit holder of the scratchpad GS image address */

    sceGsSyncPath(0, 0);
    fd = sceOpen(path, TYA_OPEN_WRITE_NEW);
    if (fd < 0) {
        return 1;
    }
    sceWrite(fd, BmpHeader, sizeof(BmpHeader));
    __builtin_memcpy(WorkEnd, SCRATCHPAD, CAPTURE_SCRATCHPAD_SIZE);
    fbp = getfbp();
    image = 0x70000070;
    dst = pixels;
    for (y = 0; y < CAPTURE_HEIGHT; y++) {
        sceGsSetDefStoreImage(SCRATCHPAD, fbp, TYA_BUFFER_WIDTH, TYA_PSM_CT24, 0, CAPTURE_HEIGHT - 1 - y,
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

/*
 * Writes the scratchpad frame buffer to a SiPic file at path. Depth values go
 * to the Z file at zpath when zpath is not empty. Returns 0 on success and 1
 * if path cannot be opened.
 */
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
    fd = sceOpen(path, TYA_OPEN_WRITE_NEW);
    if (fd < 0) {
        return 1;
    }
    zfd = (zpath != 0 && *zpath != '\0') ? sceOpen(zpath, TYA_OPEN_WRITE_NEW) : 0;
    sceWrite(fd, SiPicHeader, sizeof(SiPicHeader));
    __builtin_memcpy(WorkEnd, spr, CAPTURE_SCRATCHPAD_SIZE);
    fbp = getfbp();
    dst = pixels;
    image = 0x70000070;
    for (y = 0; y < CAPTURE_HEIGHT; y++) {
        sceGsSetDefStoreImage(spr, fbp, TYA_BUFFER_WIDTH, TYA_PSM_CT32, 0, y, CAPTURE_WIDTH, 1);
        sceGsExecStoreImage(spr, (void *)(u32)image);
        sceGsSetDefStoreImage(spr, sRender.depth_base_page << 5, TYA_BUFFER_WIDTH, TYA_PSM_Z32, 0, y,
                              CAPTURE_WIDTH, 1);
        end = (u32 *)(spr + SPR_DEPTH_OFFSET);
        sceGsExecStoreImage(spr, end);
        out = SPR_RLE;

        /* Run-length encode the colour row into SPR_RLE. */
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
                if (length > TYA_RLE_MAX_RUN) {
                    out[0] = TYA_RLE_LONG_RUN_TAG;
                    out[1] = length >> 8;
                    out[2] = length;
                    out += 3;
                } else {
                    *out = length + (TYA_RLE_MAX_RUN - 1);
                    out++;
                }
                colorBytes = (u8 *)&color;
                out[0] = colorBytes[0];
                out[1] = colorBytes[1];
                out[2] = colorBytes[2];
                out += 3;
            } else {
                if (count == TYA_RLE_MAX_RUN) {
                    *header = TYA_RLE_MAX_RUN - 1;
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

        /* Compute the output alpha byte of each pixel from its alpha, the depth row and the capture alpha. */
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

        /* Run-length encode the alpha bytes of the same row. */
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
                if (length > TYA_RLE_MAX_RUN) {
                    out[0] = TYA_RLE_LONG_RUN_TAG;
                    out[1] = length >> 8;
                    out[2] = length;
                    out += 3;
                } else {
                    *out = length + (TYA_RLE_MAX_RUN - 1);
                    out++;
                }
                *out = alphaRun >> 24;
                out++;
            } else {
                if (count == TYA_RLE_MAX_RUN) {
                    *header = TYA_RLE_MAX_RUN - 1;
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

        memcpy(dst, SPR_RLE, out - spr - SPR_RLE_OFFSET);
        dst = dst + (out - spr) - SPR_RLE_OFFSET;

        /* Convert the depth row for the Z file, tracking the smallest and largest depth that is neither 0 nor RGB_MASK. */
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
                zout[0] = spr[CAPTURE_DEPTH_RESULT_OFFSET + 3];
                zout[1] = spr[CAPTURE_DEPTH_RESULT_OFFSET + 2];
                zout[2] = spr[CAPTURE_DEPTH_RESULT_OFFSET + 1];
                zout[3] = spr[CAPTURE_DEPTH_RESULT_OFFSET];
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

/*
 * Capture task hook. START installs it, FRAME writes one picture (BMP for
 * layer 0, SiPic otherwise) and advances the frame number in the file name,
 * END removes it. Every command is passed on to the previous capture task.
 */
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
    case TYA_CAPTURE_START:
        if (GameLoopState.captureTask != tyaCaptureMain) {
            *savedCapture = GameLoopState.captureTask;
            GameLoopState.captureTask = tyaCaptureMain;
        }
        flag = 1;
        break;
    case TYA_CAPTURE_FRAME:
        if (flag == 0) {
            flag = 1;
            break;
        }
        shifted_mode = (long long)(DispEnv.buffer_select << 7);
        selector = shifted_mode >> 32;
        /* Wait for the GS to finish drawing before the frame buffer is read back. */
        if ((selector & 3) == 1) {
            while (!(*(volatile u64 *)GS_CSR_ADDRESS & GS_CSR_FINISH)) {
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
        /* Advance the four-digit frame number in the file name. */
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
        /* Flag 2 ends the capture after this frame, so control falls through to TYA_CAPTURE_END. */
        if (flag != 2) {
            break;
        }
    case TYA_CAPTURE_END:
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

/* Draws the actors selected by the current layer, taking the capture alpha from each actor's draw environment. */
static void tyaCaptureActor(void)
{
    int i;
    int *alpha_value;
    Actor *unit;

    alpha_value = &alpha;
    for (i = 0, unit = actor; i < ACTOR_COUNT; i++, unit++) {
        if (unit->inUseId == 0 || (unit->flags & 8)) {
            continue;
        }
        if (layer != i + TYA_LAYER_ACTOR_BASE) {
            if (unit->parent == 0 || layer != unit->parent->number + TYA_LAYER_ACTOR_BASE) {
                continue;
            }
        }
        if (unit->draw == ACT_filterGuno) {
            *alpha_value = 1;
            transparency = F2I(unit->drawEnv.fogDistance[0] * 128.0f);
        } else {
            ActorDrawEnv *env = &unit->drawEnv;

            if (env->flags & 1) {
                *alpha_value = env->alpha;
                transparency = F2I(env->transparency);
            } else {
                *alpha_value = 0;
            }
        }
        ACT_modelDraw(unit);
    }
}

/* Draws the shadows of the actor selected by the current layer. */
static void tyaCaptureShadow(void)
{
    int i;
    Actor *unit;

    unit = actor;
    i = 0;
    ACT_DrawShadowBegin();
    for (; i < ACTOR_COUNT; i++, unit++) {
        if (unit->inUseId != 0 && !(unit->flags & 8) &&
            layer == i + TYA_LAYER_ACTOR_BASE && (unit->flags & 0x20)) {
            ACT_DrawShadow(unit);
        }
    }
    ACT_DrawShadowEnd();
}

static void tyaCaptureUnit(void)
{
    int i;
    MapUnitRecord *unit;

    for (i = 0, unit = MapUnit; i < MAP_UNIT_COUNT; i++, unit++) {
        if (unit->serial >= 0 && layer == i + TYA_LAYER_MAP_UNIT_BASE) {
            MAP_drawUnitAt(unit);
        }
    }
}

/*
 * Draws the map units whose parent_peer is the first active actor that matches
 * the current layer. With no such actor, the comparison is against (Actor *)-1.
 */
static void tyaCaptureUnit2(void)
{
    int i;
    Actor *unit;
    Actor *selected;
    MapUnitRecord *mapUnit;

    selected = (Actor *)-1;
    for (i = 0, unit = actor; i < ACTOR_COUNT; i++, unit++) {
        if (unit->inUseId != 0 && !(unit->flags & 8) && layer == i + TYA_LAYER_ACTOR_BASE) {
            selected = unit;
            break;
        }
    }
    for (i = MAP_UNIT_COUNT - 1, mapUnit = MapUnit; i >= 0; i--, mapUnit++) {
        if (mapUnit->serial >= 0 && mapUnit->parent_peer == selected) {
            MAP_drawUnitAt(mapUnit);
        }
    }
}

/*
 * Starts a capture of the layer in the low bits of mode. The output name is
 * built under host0:/home/xeno/capture/ from name. Layer 0 writes BMP frames,
 * other layers write SiPic frames.
 */
void tyaCaptureStart(const char *name, int mode)
{
    static char head[] = "host0:/home/xeno/capture/";
    char *dst;
    const char *src;

    TmpBuffer = GameResourceGetFreeAddr();
    printf("CaptureBuffer:%08x\n", TmpBuffer);
    layer = mode & TYA_CAPTURE_LAYER_MASK;
    zpicf = mode & TYA_CAPTURE_ZPIC_FLAG;
    alpha = 0;
    /* Build outpath as capture directory, name directory, layer prefix, frame number and extension. */
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
    tyaCaptureMain(TYA_CAPTURE_START);
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
        xglFontSetFlags(xglFontGetFlags() | 1);
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
        xglFontSetFlags(xglFontGetFlags() | 1);
        break;
    }
}

/* Ends the capture task and restores the font and game-loop flags that tyaCaptureStart changed. */
void tyaCaptureEnd(void)
{
    tyaCaptureMain(TYA_CAPTURE_END);
    xglFontSetFlags(xglFontGetFlags() | 3);
    GameLoopState.flags &= 0xFAFFFFFF;
}
