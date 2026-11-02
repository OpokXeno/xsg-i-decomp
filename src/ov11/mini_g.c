/*
 * OV11 original TU 1: 0x00a00100..0x00a091f8 (135 functions)
 */
#include "common.h"
#include "shared.h"
static struct CasinoResourcePrefix *ResData;
#include "res.h"
#include "mini_g.h"

/* xglCdReadFile is main:0x0021e310 (src/main/xgl_cd.c); every call below
 * passes it an asset path string and a fixed EE main RAM staging address. */
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern void xglPacketTextureTrans(void *buffer);

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

/* Partial newlib struct _reent: only the _stderr file pointer this debug
 * printf forwards to is touched; the preceding _errno/_stdin/_stdout
 * fields (newlib 1.9.0 libc/include/sys/reent.h field order) stay
 * unmodeled. _impure_ptr is the reentrant global newlib state pointer. */
struct Reent {
    unsigned char unmodeled_00[0xC];
    void *stderr_file;
};
extern struct Reent *_impure_ptr;
extern int vfprintf(void *stream, const char *format, va_list args);

void dprintf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(_impure_ptr->stderr_file, fmt, args);
    va_end(args);
}

/* OV11 SetTest and SetRegAD packet tags are original scaffold-owned
 * .rodata at 0x00A0BB80 and 0x00A0BB90. Refer to those symbols rather
 * than introducing new C-emitted constants into the tightly carved TU.
 * The stack-local copy is needed by the original 128-bit argument ABI. */
typedef unsigned int Ov11Quadword __attribute__((mode(TI)));
typedef union Ov11GifTag {
    struct { u64 lo, hi; } part;
    Ov11Quadword quad;
} Ov11GifTag;

extern const Ov11GifTag D_00A0BB80;
extern const Ov11GifTag D_00A0BB90;
extern void sceVif1PkCnt(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenGifTag(XglPacket *packet, Ov11Quadword tag);
extern void sceVif1PkAddGsAD(XglPacket *packet, int reg, u64 value);
extern void sceVif1PkCloseGifTag(XglPacket *packet);
extern void sceVif1PkCloseDirectCode(XglPacket *packet);

/* Preserve original compiler-owned NOBITS ordering: the BoxRGBA storage
 * must precede Pkt in the source declaration order for this pinned TU. */
static short BoxRGBA[4];
static XglPacket *Pkt;

extern const Ov11GifTag D_00A0BB30;
extern const Ov11GifTag D_00A0BB40;
extern void sceVif1PkAddGsData(XglPacket *packet, u64 value);

/* ov11:0x00a00168. Emits one textured sprite into Pkt. The sprite
 * descriptor gives the texture position (u, v), its size, two words for
 * the texture setup and a mode; the mode picks one of four texture-setup
 * words (modes 1 and 3 also double the v coordinate). Then the four
 * corners of rect (x, y, width, height) go out as UV and XYZ pairs with
 * the 0x7000/0x7200 screen offset and z = 0x80. Not sure of the GS
 * register meaning of the four setup words. */
static void MakeSprite(const struct SpriteTex *tex, const struct SpriteRect *pos)
{
    Ov11GifTag tex_tag = D_00A0BB30;
    Ov11GifTag prim_tag = D_00A0BB40;
    int u0 = tex->u + 1;
    int v0 = tex->v + 1;
    int du = tex->w - 1;
    int dv = tex->h - 1;
    u64 z;
    u64 ul;
    u64 ur;
    u64 vt;
    u64 vb;

    sceVif1PkCnt(Pkt, 0);
    sceVif1PkOpenDirectCode(Pkt, 0);
    sceVif1PkOpenGifTag(Pkt, tex_tag.quad);
    switch (tex->format) {
    case 1:
        sceVif1PkAddGsData(Pkt, (tex->texture_base + 0x3800) | 0x25440000 | ((u64)(tex->clut_base + 0x3800) << 37) | 0x2000000E40000000ULL);
        v0 = v0 * 2 - 1;
        break;
    case 2:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->clut_base + 0x3800) << 37) | 0x4000000E21343800ULL);
        break;
    case 3:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->clut_base + 0x3800) << 37) | 0x4000000EA9443800ULL);
        v0 = v0 * 2 - 1;
        break;
    default:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->clut_base + 0x3800) << 37) | 0x4000000E65323800ULL);
        break;
    }
    ul = u0 << 4;
    vt = (u64)(v0 << 4) << 16;
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkOpenGifTag(Pkt, prim_tag.quad);
    z = 0x8000000000ULL;
    ur = (u0 + du) << 4;
    sceVif1PkAddGsData(Pkt, 0x154);
    sceVif1PkAddGsData(Pkt, ul | vt);
    sceVif1PkAddGsData(Pkt, ((pos->x << 4) + 0x7000) | ((u64)((pos->y << 4) + 0x7200) << 16) | z);
    sceVif1PkAddGsData(Pkt, ur | vt);
    vb = (u64)((v0 + dv) << 4) << 16;
    sceVif1PkAddGsData(Pkt, (((pos->x + pos->w) << 4) + 0x7000) | ((u64)((pos->y << 4) + 0x7200) << 16) | z);
    sceVif1PkAddGsData(Pkt, ul | vb);
    sceVif1PkAddGsData(Pkt, ((pos->x << 4) + 0x7000) | ((u64)(((pos->y + pos->h) << 4) + 0x7200) << 16) | z);
    sceVif1PkAddGsData(Pkt, ur | vb);
    sceVif1PkAddGsData(Pkt, (((pos->x + pos->w) << 4) + 0x7000) | ((u64)(((pos->y + pos->h) << 4) + 0x7200) << 16) | z);
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkCloseDirectCode(Pkt);
}

extern const Ov11GifTag D_00A0BB50;

/* ov11:0x00a004d0. Builds the GS draw-state packet: it writes registers
 * 0x47, 0x14 and 0x08 with fixed values, then register 0x42 with a value
 * chosen by mode (0..6; modes 5 and 6 put the second argument in the upper
 * word, anything else writes 0). If I have the GS register map right, 0x47
 * is TEST_1 and 0x42 is ALPHA_1, i.e. this selects the blend mode; not
 * sure. */
static void SetDrawStatus(int mode, int arg)
{
    Ov11GifTag tag = D_00A0BB50;

    sceVif1PkCnt(Pkt, 0);
    sceVif1PkOpenDirectCode(Pkt, 0);
    sceVif1PkOpenGifTag(Pkt, tag.quad);
    sceVif1PkAddGsAD(Pkt, 0x47, 0x5001D);
    sceVif1PkAddGsAD(Pkt, 0x14, 0x20);
    sceVif1PkAddGsAD(Pkt, 0x8, 0);
    switch (mode) {
    case 0:
        sceVif1PkAddGsAD(Pkt, 0x42, 0x48);
        break;
    case 2:
        sceVif1PkAddGsAD(Pkt, 0x42, 0x6000000062ULL);
        break;
    case 3:
        sceVif1PkAddGsAD(Pkt, 0x42, 0);
        break;
    case 4:
        sceVif1PkAddGsAD(Pkt, 0x42, 0x88);
        break;
    case 5:
        sceVif1PkAddGsAD(Pkt, 0x42, ((u64)arg << 32) | 0x29);
        break;
    case 6:
        sceVif1PkAddGsAD(Pkt, 0x42, ((u64)arg << 32) | 0x68);
        break;
    case 1:
        sceVif1PkAddGsAD(Pkt, 0x42, 0x44);
        break;
    default:
        sceVif1PkAddGsAD(Pkt, 0x42, 0);
        break;
    }
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkCloseDirectCode(Pkt);
}

/* Experimental C candidates: matching status must come from the full
 * repository gate and compiler-emission verifier, never from appearance. */
static void SetTest(int value)
{
    Ov11GifTag tag = D_00A0BB80;
    sceVif1PkCnt(Pkt, 0);
    sceVif1PkOpenDirectCode(Pkt, 0);
    sceVif1PkOpenGifTag(Pkt, tag.quad);
    sceVif1PkAddGsAD(Pkt, 0x47, value);
    sceVif1PkCloseGifTag(Pkt);
    sceVif1PkCloseDirectCode(Pkt);
}

static void SetRegAD(XglPacket *packet, int reg, int value)
{
    Ov11GifTag tag = D_00A0BB90;
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
    sceVif1PkAddGsAD(packet, reg, value);
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectCode(packet);
}

static void ResetRGBA(void)
{
    int index;
    short reset_color;

    reset_color = 0x80;
    for (index = 3; index >= 0; index--) {
        BoxRGBA[index] = reset_color;
    }
}

/* ov11:0x00a007e0. Stores the four caller-supplied components in the
 * shared untextured box color state (original LOCAL data symbol
 * BoxRGBA, ov11:0x00a0df78). */
static short BoxRGBA[4];

static void SetRGBA(short red, short green, short blue, short alpha)
{
    BoxRGBA[0] = red;
    BoxRGBA[1] = green;
    BoxRGBA[2] = blue;
    BoxRGBA[3] = alpha;
}

/* Candidate-backed OV11/tu001 initialized DATA objects; function bodies are retained. */
static short real11array[21];
static short real12array[21];
static short real13array[21];
static short real21array[21];
static short real22array[21];
static short real23array[21];
static short real31array[21];
static short real32array[21];
static short real33array[21];
static short real41array[21];
static short real42array[21];
static short real43array[21];
static short real51array[21];
static short real52array[21];
static short real53array[21];

static char *SaveWork  = (char *)SaveData + 0x15190;
static short real01array[21];
static short real02array[21];
static short real03array[21];
static short *realpt[18]  = {
    real01array, real02array, real03array, real11array, real12array, real13array,
    real21array, real22array, real23array, real31array, real32array, real33array,
    real41array, real42array, real43array, real51array, real52array, real53array
};
static struct SpriteTex pay_window[2]  = {
    {224, 0, 32, 32, 0, 572, 0},
    {224, 32, 32, 32, 0, 572, 0}
};
static struct SpriteRect pay_window_rect_4[5]  = {
    {36, 33, 32, 32}, {36, 82, 32, 32}, {36, 131, 32, 32},
    {36, 180, 32, 32}, {36, 229, 32, 32}
};
static int status_keep_5  = 0;
static struct SpriteTex GuidPtTex[10]  = {
    {256, 248, 8, 8, 0, 564, 0}, {264, 248, 8, 8, 0, 564, 0},
    {272, 248, 8, 8, 0, 564, 0}, {280, 248, 8, 8, 0, 564, 0},
    {288, 248, 8, 8, 0, 564, 0}, {296, 248, 8, 8, 0, 564, 0},
    {304, 248, 8, 8, 0, 564, 0}, {312, 248, 8, 8, 0, 564, 0},
    {320, 248, 8, 8, 0, 564, 0}, {328, 248, 8, 8, 0, 564, 0}
};
static struct SpriteRect point_rect_20[9]  = {
    {59, 62, 8, 8}, {59, 111, 8, 8}, {59, 160, 8, 8}, {59, 209, 8, 8},
    {59, 258, 8, 8}, {192, 271, 8, 8}, {241, 271, 8, 8}, {290, 271, 8, 8},
    {439, 144, 8, 8}
};
static int seq_21  = 0;
static struct SpriteTex BoxSpr  = {0, 0, 511, 447, 0, 1020, 0};
static struct SpriteRect BoxRect  = {0, 0, 511, 447};
static struct SpriteTex NineGameBG_Tex_43  = {0, 0, 511, 447, 0, 1020, 0};
static struct SpriteRect NineGameBG_Rect_44  = {0, 0, 512, 448};
static int seq_45  = 0;
struct GuidPoint {
    int x;
    int y;
};
static struct GuidPoint GuidPt_46[11]  = {
    {192, 136}, {241, 136}, {290, 136}, {204, 240}, {253, 240}, {302, 240},
    {216, 344}, {265, 344}, {314, 344}, {458, 27}, {183, 362}
};
static struct SpriteTex tex_68  = {0, 0, 512, 448, 0, 1020, 0};
static struct SpriteRect rect_69  = {0, 0, 512, 448};
static struct SpriteTex tex_80  = {0, 0, 352, 176, 0, 1020, 0};
static struct SpriteRect rect_81  = {16, 256, 352, 176};
static struct SpriteRect result_rect_97[9]  = {
    {204, 362, 160, 32}, {204, 330, 160, 32}, {204, 298, 160, 32},
    {204, 266, 160, 32}, {0, 394, 244, 32}, {0, 362, 244, 32},
    {0, 330, 244, 32}, {0, 298, 244, 32}, {0, 266, 244, 32}
};
static struct SpriteTex tex_98  = {96, 224, 160, 32, 0, 756, 0};
static struct SpriteTex SubWindow_Tex  = {0, 0, 176, 120, 0, 564, 2};
static struct SpriteTex Level_Tex[4]  = {
    {176, 0, 72, 16, 0, 564, 2}, {176, 16, 72, 16, 0, 564, 2},
    {176, 32, 72, 16, 0, 564, 2}, {176, 48, 72, 16, 0, 564, 2}
};
static struct SpriteTex ExChange_Tex[3]  = {
    {176, 64, 64, 16, 0, 564, 2}, {176, 80, 64, 16, 0, 564, 2},
    {176, 96, 64, 16, 0, 564, 2}
};
static struct SpriteTex BGTEST_Tex  = {0, 0, 511, 447, 0, 1136, 0};
static struct SpriteRect BGTEST_Rect  = {0, 0, 512, 448};
static int flg_104  = 0;

/* Four GS XYZ vertices for an untextured box. Coordinates use 12.4
 * fixed point around the overlay's (0x7000, 0x7200) drawing origin;
 * the upper word supplies Z = 1. BoxPOS remains scaffold-owned storage. */
extern unsigned long long BoxPOS[4];

static void MakeBoxPos(int x, int y, int width, int height)
{
    int left;
    int right;
    unsigned long long top;
    unsigned long long bottom;
    unsigned long long depth;

    left = x * 16 + 0x7000;
    top = (unsigned long long)(y * 16 + 0x7200);
    right = (x + width) * 16 + 0x7000;
    bottom = (unsigned long long)((y + height) * 16 + 0x7200);
    depth = 1ULL << 32;

    BoxPOS[0] = left | (top << 16) | depth;
    BoxPOS[1] = right | (top << 16) | depth;
    BoxPOS[2] = left | (bottom << 16) | depth;
    BoxPOS[3] = right | (bottom << 16) | depth;
}

extern const Ov11GifTag D_00A0BBA0;

/* ov11:0x00a00878. Draws a filled box as a GS packet: MakeBoxPos builds
 * the corner positions, then four colour+position pairs are written using
 * the current box colour. */
static void DrawNonTextureBox(XglPacket *packet, int x, int y, int width, int height)
{
    Ov11GifTag tag = D_00A0BBA0;
    u64 *pos;
    short *color;
    int count;
    u64 value;
    u64 low;
    u64 high;

    MakeBoxPos(x, y, width, height);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
    sceVif1PkAddGsData(packet, 0x144);
    color = BoxRGBA;
    pos = BoxPOS;
    count = 3;
    do {
        high = ((u64)color[2] << 16) | ((u64)color[1] << 8);
        low = (u64)color[0] | 0x100000000ULL;
        value = low | high;
        value |= (u64)color[3] << 24;
        sceVif1PkAddGsData(packet, value);
        sceVif1PkAddGsData(packet, *pos);
        pos++;
    } while (--count >= 0);
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectCode(packet);
}

/* Casino settings, prize text/costs and coin-purchase offers from CASINO.res.
 * Original RES_* symbols: slus-20469-412d448de315 / ov11:0x00a00998..0x00a00a58.
 * See reports/readability-semantics.md for asset/consumer evidence.
 *
 * All eight are LOCAL symbols in the original overlay, so they have internal
 * linkage and are defined `static` here. The compiled object binds them the
 * same way, which is what the linked comparison checks alongside the bytes.
 */

static int RES_IsDebugMode(void)
{
    /* The debug word is proven; the intervening sound region's extent is not. */
    int *debug_mode = (int *)((unsigned char *)ResData + CASINO_DEBUG_SETTING_OFFSET);
    return *debug_mode;
}

static int RES_GetBonusTime(void)
{
    return ResData->bonus_duration;
}

static int RES_GetRealSpeed(void)
{
    return ResData->reel_step;
}

static char *RES_ShopDataInfo(int prize_index)
{
    return ResData->prizes[prize_index].description;
}

static int RES_ShopDataCoin(int prize_index)
{
    CasinoPrize *prize = ResData->prizes;
    prize += prize_index;
    return prize->coin_cost;
}

static char *RES_ShopDataName(int prize_index)
{
    return ResData->prizes[prize_index].name;
}

static int RES_Coin_Rate(int offer_index)
{
    return ResData->coin_offers[offer_index].money_cost;
}

static char *RES_Coin_Info(int offer_index)
{
    return ResData->coin_offers[offer_index].description;
}

/* The sound-effect ID table sits between the known CasinoResourcePrefix
 * prefix and CASINO_DEBUG_SETTING_OFFSET (0x1d3c); its own extent past this
 * indexed access is not proven, so it is reached by byte offset rather than
 * a named CasinoResourcePrefix member (matching RES_IsDebugMode above). */
#define CASINO_SOUND_ID_TABLE_OFFSET 0x193C

extern void xglSoundEffectNormalID(int soundEffectId, int variant);
static const char D_00A0BBB0[] = "Sound Req = SE_%.3d:Sound num = %x\n";

static void RES_SoundEffect(int soundId)
{
    int soundEffectId = *(int *)((unsigned char *)ResData + soundId * 4 + CASINO_SOUND_ID_TABLE_OFFSET);

    dprintf(D_00A0BBB0, soundId, soundEffectId);
    xglSoundEffectNormalID(soundEffectId, 0);
}

extern void xglSoundEffectStopDirect(int soundEffectId);
static const char D_00A0BBD8[] = "Sound Req = SE_%.3d:Sound Stop num = %x\n";

static void RES_SoundEffectStop(int soundId)
{
    int soundEffectId = *(int *)((unsigned char *)ResData + soundId * 4 + CASINO_SOUND_ID_TABLE_OFFSET);

    dprintf(D_00A0BBD8, soundId, soundEffectId);
    xglSoundEffectStopDirect(soundEffectId);
}

/* ov11:0x00a00af0. Loads the casino resource table (CASINO.res) into the
 * fixed asset staging address and records it as ResData, the same
 * (void *)FIXED_ADDRESS pattern PokerInit and the other loaders below use. */
static const char D_00A0BC08[] = "data\\tanaka\\CASINO.res";

#define CASINO_RES_BUFFER 0x01000000

/* Fixed EE RAM buffers the slot and poker loaders (init_slot, PokerInit)
 * read their textures into; the drawing code uploads from the same
 * addresses. */
#define SLOT_1_TEX_BUFFER 0x01008000
#define SLOT_2_TEX_BUFFER 0x01048800
#define SLOT_BASE_TEX_BUFFER 0x01089000
#define SLOT_W1_TEX_BUFFER 0x010C9800
#define SLOT_HELP_TEX_BUFFER 0x0110A000
#define POKER_SPADE_TEX_BUFFER  0x0128D000
#define POKER_CLOVER_TEX_BUFFER 0x012CD800
#define POKER_HEART_TEX_BUFFER  0x0130E000
#define POKER_DIA_TEX_BUFFER    0x0134E800
#define POKER_TABLE1_TEX_BUFFER 0x0138F000
#define POKER_TABLE2_TEX_BUFFER 0x013CF800

static void RES_Load(void)
{
    xglCdReadFile(D_00A0BC08, (void *)CASINO_RES_BUFFER, 0, 0);
    ResData = (CasinoResourcePrefix *)CASINO_RES_BUFFER;
}

/* The coin balance is the word at offset 0x50 in the persistent save block. */
struct CasinoSaveWork {
    unsigned char unmodeled_00[0x50];
    int coin_balance;
};
extern int dataMoneyBoxInc(int amount);

static int AddCoin(int amount)
{
    struct CasinoSaveWork *save;
    int total;
    int excess;

    save = (struct CasinoSaveWork *)SaveWork;
    total = save->coin_balance + amount;
    if (total > 9999999) {
        excess = total - 9999999;
        dataMoneyBoxInc(excess * 8);
        amount -= excess;
    }
    save = (struct CasinoSaveWork *)SaveWork;
    save->coin_balance += amount;
    return 1;
}

extern int D_00A0DD34[];

static short SlGetPic(int reel, int positionOffset, int row)
{
    int position;
    int pictureIndex;
    short *pictures;

    position = D_00A0DD34[reel] / 64;
    pictures = realpt[reel + row * 3];
    position -= positionOffset;
    pictureIndex = position + 2;
    if (pictureIndex < 0) {
        pictureIndex = position + 23;
    }
    if (pictureIndex >= 21) {
        pictureIndex -= 21;
    }
    return pictures[pictureIndex];
}

extern struct SpriteTex Real_2[9];
extern int xpos_3[3];
extern struct SpriteTex slot_window_0;
extern struct SpriteRect slot_window_rect_1[3];

/* ov11:0x00a00c10. Draws the three slot reels: wraps each reel position
 * (Gwork.slot.reel_pos) back by 0x540 when it passes 0x540, then for each reel
 * draws six picture cells (SlGetPic for the current row) scrolled by the
 * position, then the three reel windows and a dark box over the lower
 * area. Not sure what the box is for. */
static void SlRealDraw(void)
{
    struct SpriteRect rect;
    int i;
    int j;
    int n;

    SetDrawStatus(3, 0);
    for (n = 0; n < 3; n++) {
        if (Gwork.slot.reel_pos[n] > 0x540) {
            Gwork.slot.reel_pos[n] -= 0x540;
        }
    }
    rect.w = 0x70;
    rect.h = 0x40;
    for (i = 0; i < 3; i++) {
        rect.x = xpos_3[i];
        for (n = 4; n >= -1; n--) {
            j = 2 - n;
            rect.y = Gwork.slot.reel_pos[i] % 64 + j * 64 + 0x32;
            MakeSprite(&Real_2[SlGetPic(i, j, Gwork.slot.row)], &rect);
        }
    }
    SetDrawStatus(2, 0);
    for (n = 0; n < 3; n++) {
        MakeSprite(&slot_window_0, &slot_window_rect_1[n]);
    }
    SetDrawStatus(3, 0);
    SetTest(0x3000D);
    ResetRGBA();
    SetRGBA(0, 0, 0, 0x80);
    DrawNonTextureBox(Pkt, 0x50, 0x12C, 0x160, 0x40);
}

#define DOUBLE_TEN 0x4024000000000000ULL

extern unsigned long long litodp(int value);
extern int dptoli(unsigned long long value);
/* The soft-float helpers take raw 64-bit values, either a double or its
 * encoding, so these three stay unprototyped. */
extern unsigned long long pow();
extern unsigned long long dpdiv();
extern int dpcmp();
extern struct SpriteTex decfont[10];

/* ov11:0x00a00de0. Prints value in decimal with 16x24 digit sprites from
 * decfont, right-aligned at x and moving left. A negative digits count
 * means as many digits as value needs (at most 11, found by comparing
 * against powers of ten); zero digits still draws one zero. */
static void decprint(int value, int digits, int x, int y)
{
    struct SpriteRect rect;
    double number;
    unsigned long long power;
    int i;

    if (digits < 0) {
        digits = 0;
        while (1) {
            if (digits < 11) {
                number = (double)value;
                if (dpcmp(number, pow(DOUBLE_TEN, litodp(digits))) < 0) {
                    break;
                }
                digits++;
            } else {
                break;
            }
        }
    }
    rect.w = 16;
    rect.h = 24;
    if (digits > 0) {
        for (i = 0; i < digits; ) {
            rect.x = x - i * 16;
            rect.y = y;
            power = pow(DOUBLE_TEN, litodp(i++));
            /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
             * the pow result copy ahead of the next conversion argument. */
            do { } while (0);
            MakeSprite(&decfont[dptoli(dpdiv((double)value, power)) % 10], &rect);
        }
    } else {
        rect.x = x;
        rect.y = y;
        MakeSprite(decfont, &rect);
    }
}
extern int D_00A0DDC8;

static void SlCashPrint(void)
{
    decprint(((struct CasinoSaveWork *)SaveWork)->coin_balance, -1, 440, 309);
    decprint(D_00A0DDC8, -1, 448, 373);
}

extern const struct PayLayout { struct { short x; short y; } row[5]; } D_00A0BC20;
/* ov11:0x00a00fb0. Slot: prints the five per-line pay amounts with
 * decprint at five fixed screen positions (a 20-byte position table copied
 * to the stack). */
static void pay_print(XglPacket *packet)
{
    struct PayLayout layout = D_00A0BC20;
    int i;

    for (i = 0; i < 5; i++) {
        decprint(Gwork.slot.line_pay[i], 2, layout.row[i].x, layout.row[i].y);
    }
}

/* ov11:0x00a01060. Draws the three-digit slot total stake at its fixed
 * screen position through decprint (ov11:0x00a00de0, LOCAL asm sibling). */

extern short D_00A0DD74;

static void total_pay_print(XglPacket *packet)
{
    decprint(D_00A0DD74, 3, 0x1DC, 0x1B);
}

static int check_mode(void)
{
    int index;

    for (index = 0; index < 3; index++) {
        if (Gwork.slot.reel_state[index] == 1) {
            return 1;
        }
    }
    return 0;
}

static void SetDrawStatus(int mode, int enable);

static void SlPayWindowPrint(void)
{
    int rectIndex;
    int windowIndex;

    SetDrawStatus(0, 0);
    windowIndex = check_mode() != 1;
    for (rectIndex = 0; rectIndex < 5; rectIndex++) {
        MakeSprite(&pay_window[windowIndex], &pay_window_rect_4[rectIndex]);
    }
}


static int check_spin(void)
{
    int active;

    active = check_mode();
    if (active == 0) {
        status_keep_5 = 0;
        return active;
    }
    if (active != status_keep_5) {
        status_keep_5 = active;
        return 1;
    }
    status_keep_5 = active;
    return 2;
}

extern struct SpriteTex spinbt[3];
extern struct SpriteRect spinbt_rect_6;
extern int pushcnt_7;

/* ov11:0x00a011b0. Slot: draws the spin button. The sprite comes from
 * check_spin()'s result plus a push counter (pushcnt_7); not sure of the
 * exact meaning of the counter. */
static void SpinBtnPrint(void)
{
    int index;

    switch (check_spin()) {
    case 0:
        index = 1;
        break;
    case -1:
        index = 0;
        break;
    case 1:
        index = 2;
        pushcnt_7 = 0;
        break;
    default:
        if (pushcnt_7 >= 0) {
            index = 2;
            pushcnt_7 = pushcnt_7 + 1;
            if (pushcnt_7 >= 5) {
                pushcnt_7 = -1;
            }
        } else {
            index = 3;
        }
        break;
    }
    if (index != 0) {
        MakeSprite(&spinbt[index - 1], &spinbt_rect_6);
    }
}

extern struct SpriteTex stopbt[3];
extern struct SpriteRect stopbt_rect_8[3];

/* ov11:0x00a01278. Slot: draws the three stop-button sprites, choosing
 * each texture from that reel's state (1, 2 or anything else). */
static void SlStopBtnPrint(void)
{
    int i;
    int index;

    for (i = 0; i < 3; i++) {
        switch (Gwork.slot.reel_state[i]) {
        case 1:
            index = 0;
            break;
        case 2:
            index = 1;
            break;
        default:
            index = 2;
            break;
        }
        MakeSprite(&stopbt[index], &stopbt_rect_8[i]);
    }
}

extern struct SpriteTex DeciList_9[10];

/* ov11:0x00a01330. Prints value in decimal with 12x16 digit sprites from
 * DeciList_9 starting at x and moving left by w per digit, at height y.
 * The digit count is as many as value needs (at most 11); zero still
 * draws one zero. */
static void SlDecPrintSmall(int x, int y, int w, int value)
{
    struct SpriteRect rect;
    double number;
    unsigned long long power;
    int digits;
    int i;

    digits = 0;
    while (1) {
        if (digits < 11) {
            number = (double)value;
            if (dpcmp(number, pow(DOUBLE_TEN, litodp(digits))) < 0) {
                break;
            }
            digits++;
        } else {
            break;
        }
    }
    rect.w = 12;
    rect.h = 16;
    if (digits > 0) {
        for (i = 0; i < digits; ) {
            rect.x = x - i * w;
            rect.y = y;
            power = pow(DOUBLE_TEN, litodp(i++));
            /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
             * the pow result copy ahead of the next conversion argument. */
            do { } while (0);
            MakeSprite(&DeciList_9[dptoli(dpdiv((double)value, power)) % 10], &rect);
        }
    } else {
        rect.x = x;
        rect.y = y;
        MakeSprite(DeciList_9, &rect);
    }
}

extern const struct SpriteRect D_00A0BC38;
extern const struct SpriteRect D_00A0BC48;
extern const struct SpriteRect D_00A0BC58;
extern const struct SpriteRect D_00A0BC68;
extern const struct SpriteRect D_00A0BC78;
extern const struct SpriteRect D_00A0BC88;
extern int pos_10[7][2];
extern struct SpriteTex PicTable[7];
extern struct SpriteTex OptionText[5];
extern int rate_11[6];
extern int double_rate_12[6];
static void SlDecPrintSmall(int x, int y, int w, int value);

/* ov11:0x00a014b8. Draws the slot pay-rate table: six symbol pictures from
 * PicTable at the pos_10 positions, a seventh narrower one, six OptionText
 * labels from fixed rectangles, then the six rates (double_rate_12 when
 * Gwork.slot.double_pay is set, otherwise rate_11) with SlDecPrintSmall.
 * Not sure what the labels say or what SlDecPrintSmall's third argument
 * means. */
static void SlRateTableDraw(void)
{
    struct SpriteRect rect;
    int i;

    SetDrawStatus(3, 0);
    rect.w = 0x54;
    rect.h = 0x10;
    for (i = 0; i < 6; i++) {
        rect.x = pos_10[i][0];
        rect.y = pos_10[i][1];
        MakeSprite(&PicTable[i], &rect);
    }
    rect.y = pos_10[i][1];
    rect.x = pos_10[i][0];
    rect.w = 0x1C;
    MakeSprite(&PicTable[i], &rect);
    rect = D_00A0BC38;
    MakeSprite(&OptionText[1], &rect);
    rect = D_00A0BC48;
    MakeSprite(&OptionText[1], &rect);
    rect = D_00A0BC58;
    MakeSprite(&OptionText[1], &rect);
    rect = D_00A0BC68;
    MakeSprite(&OptionText[2], &rect);
    rect = D_00A0BC78;
    MakeSprite(&OptionText[3], &rect);
    rect = D_00A0BC88;
    MakeSprite(&OptionText[4], &rect);
    for (i = 0; i < 6; i++) {
        if (Gwork.slot.double_pay != 0) {
            SlDecPrintSmall(0xAF, 0x125 + i * 0x14, 0xC, double_rate_12[i]);
        } else {
            SlDecPrintSmall(0xAF, 0x125 + i * 0x14, 0xC, rate_11[i]);
        }
    }
}

extern struct SpriteTex Pic_13[9];
extern struct SpriteTex Total_14;
extern struct SpriteTex linetex_15;
extern struct SpriteRect linerect_16[9];
extern const struct SpriteRect D_00A0BC98;

/* ov11:0x00a01750. Draws the slot result screen. For every pay line with a
 * prize it draws the three reel pictures (Pic_13), the prize and the
 * line's stake with SlDecPrintSmall, adds prize times stake to the total,
 * and either draws a label for the line's kind (OptionText 1, 2 or 0 for
 * kinds 1, 2, 3) or prints the line payout; rows move down 0x14 per
 * winning line. It then draws the total, and lights the winning pay lines
 * on the 3x3 grid (linetex_15 at linerect_16 positions). Not sure which
 * game labels the kinds stand for. */
static void SlResultDraw(void)
{
    int lit[9];
    struct SpriteRect rect;
    int total;
    int y;
    int i;
    int k;
    int x;
    struct SlotLine *line;

    rect.h = 0x10;
    SetDrawStatus(3, 0);
    y = 0x125;
    total = 0;
    for (i = 0; i < 5; i++) {
        line = &Gwork.slot.line[i];
        if (line->prize != 0) {
            rect.w = 0x1C;
            x = 0x21;
            for (k = 0; k < 3; k++) {
                rect.x = x;
                rect.y = y;
                MakeSprite(&Pic_13[line->pic[k]], &rect);
                x += 0x1C;
            }
            SlDecPrintSmall(0xAF, y, 0xC, line->prize);
            SlDecPrintSmall(0x8B, y, 0xC, Gwork.slot.line_pay[i]);
            total += line->prize * Gwork.slot.line_pay[i];
            if (line->kind != 0) {
                rect.x = 0xC5;
                rect.y = y;
                rect.h = 0x10;
                switch (line->kind) {
                case 1:
                    rect.w = 0x60;
                    MakeSprite(&OptionText[1], &rect);
                    break;
                case 2:
                    rect.w = 0x68;
                    MakeSprite(&OptionText[2], &rect);
                    break;
                case 3:
                    rect.w = 0x50;
                    MakeSprite(&OptionText[0], &rect);
                    break;
                }
            } else {
                SlDecPrintSmall(0x119, y, 0xC, line->prize * Gwork.slot.line_pay[i]);
            }
            y += 0x14;
        }
    }
    rect = D_00A0BC98;
    MakeSprite(&Total_14, &rect);
    SlDecPrintSmall(0x119, 0x19D, 0xC, total);
    for (i = 8; i >= 0; i--) {
        lit[i] = 0;
    }
    if (Gwork.slot.line[0].prize != 0) {
        lit[0] = 1;
        lit[4] = 1;
        lit[8] = 1;
    }
    if (Gwork.slot.line[1].prize != 0) {
        lit[0] = 1;
        lit[3] = 1;
        lit[6] = 1;
    }
    if (Gwork.slot.line[2].prize != 0) {
        lit[1] = 1;
        lit[4] = 1;
        lit[7] = 1;
    }
    if (Gwork.slot.line[3].prize != 0) {
        lit[2] = 1;
        lit[5] = 1;
        lit[8] = 1;
    }
    if (Gwork.slot.line[4].prize != 0) {
        lit[2] = 1;
        lit[4] = 1;
        lit[6] = 1;
    }
    SetDrawStatus(0, 0);
    for (i = 0; i < 9; i++) {
        if (lit[i] != 0) {
            MakeSprite(&linetex_15, &linerect_16[i]);
        }
    }
    SetDrawStatus(3, 0);
}

extern struct SpriteTex Text_17[3];
extern struct SpriteTex Bar_18;
extern struct SpriteTex TimeBar_19;
extern const struct SpriteRect D_00A0BCA8;
extern const struct SpriteRect D_00A0BCB8;
extern const struct SpriteRect D_00A0BCC8;
extern const struct SpriteRect D_00A0BCD8;

/* ov11:0x00a01a90. Draws the bonus banner. Text_17[1] shows while the
 * prize doubling is on without the bonus, Text_17[2] while the bonus is
 * on, and Text_17[0] when neither is. With either one on, the time bar
 * (Bar_18, then TimeBar_19 scaled by the remaining bonus time over
 * RES_GetBonusTime) is drawn and the bonus time counts down on idle
 * frames; at zero it restarts when the slot state is nonzero, otherwise
 * it raises event 7. */
static void SlModePrint(void)
{
    struct SpriteRect rect;
    struct SpriteRect bar;

    if (Gwork.slot.double_pay != 0 && Gwork.slot.bonus == 0) {
        rect = D_00A0BCA8;
        MakeSprite(&Text_17[1], &rect);
    }
    if (Gwork.slot.bonus != 0) {
        rect = D_00A0BCA8;
        MakeSprite(&Text_17[2], &rect);
    }
    if (Gwork.slot.bonus == 0 && Gwork.slot.double_pay == 0) {
        rect = D_00A0BCB8;
        MakeSprite(&Text_17[0], &rect);
    } else {
        float elapsed;

        rect = D_00A0BCC8;
        MakeSprite(&Bar_18, &rect);
        bar = D_00A0BCD8;
        elapsed = Gwork.slot.bonus_time;
        elapsed = 180.0f * (elapsed / RES_GetBonusTime());
        bar.w = (int)elapsed;
        MakeSprite(&TimeBar_19, &bar);
        if (Gwork.slot.event == 0) {
            Gwork.slot.bonus_time--;
        }
        if (Gwork.slot.bonus_time == 0) {
            if (Gwork.slot.state != 0) {
                Gwork.slot.bonus_time = 1;
            } else {
                Gwork.slot.event = 7;
            }
        }
    }
}

extern struct SpriteTex GuidPtTex[10];
extern struct SpriteRect point_rect_20[9];
extern int seq_21;
extern int seq_45;


static void SlGuidPrint(int index)
{
    MakeSprite(&GuidPtTex[seq_21], &point_rect_20[index]);
    seq_21 = seq_21 + 1;
    seq_21 = seq_21 % 10;
}

extern PadPrefix PadData;

/* PadData's button words. xglPadRead (src/main/xgl_pad.c) stores four
 * halfwords from +0x28: the buttons held (+0x28), newly pressed (+0x2A),
 * newly pressed with auto-repeat (+0x2C: the newly pressed set, replaced by
 * the held buttons in xglPadSetRepeat's mask whenever the repeat timer
 * fires) and released (+0x2E). All four use these bits, the same mapping
 * src/main/game_debug_menu.h and src/main/menu_ether.c name. */
#define PAD_L2 0x0001
#define PAD_R2 0x0002
#define PAD_L1 0x0004
#define PAD_R1 0x0008
#define PAD_TRIANGLE 0x0010
#define PAD_CIRCLE 0x0020
#define PAD_CROSS 0x0040
#define PAD_SQUARE 0x0080
#define PAD_SELECT 0x0100
#define PAD_START 0x0800
#define PAD_UP 0x1000
#define PAD_RIGHT 0x2000
#define PAD_DOWN 0x4000
#define PAD_LEFT 0x8000

/* Bits of the four words read as one doubleword from +0x28. */
#define PAD_WORDS_PRESSED(bits) ((u64)(bits) << 16)
#define PAD_WORDS_REPEAT(bits) ((u64)(bits) << 32)

/* PadData as the casino reads it. The shared PadPrefix stops at +0x36; this
 * TU also needs the auto-repeat word at +0x2C and the four analog stick
 * bytes at +0x64, so it uses a union with the canonical prefix, the same way
 * src/ov02/umn_guno_data_base_get.c widens it. */
typedef union CasinoPadView {
    PadPrefix canonical;
    /* The four button words from +0x28 read as one doubleword; coin_main
     * tests a pressed bit and a repeat bit together this way. */
    struct {
        unsigned char unmodeled_00[0x28];
        u64 words;
    } dword;
    struct {
        unsigned char unmodeled_00[0x2C];
        u16 repeat;
        unsigned char unmodeled_2e[0x36];
        signed char stick[4];
    } wide;
} CasinoPadView;
#define CASINO_PAD ((CasinoPadView *)&PadData)
static void SlGuidPrint(int index);

/* ov11:0x00a01d20. Slot pay-line pointer. check_mode() 0 draws the guide
 * at Gwork.slot.guide; 1 draws it at Gwork.slot.pointer + 5 and right /
 * left (newly pressed) step the pointer forward / back (wrapping within
 * 0..2) with a sound. Not sure what the pointer is for in game terms. */
static void point_control(XglPacket *packet)
{
    switch (check_mode()) {
    case 0:
        SlGuidPrint(Gwork.slot.guide);
        break;
    case 1:
        SlGuidPrint(Gwork.slot.pointer + 5);
        if (PadData.half_2a & PAD_RIGHT) {
            Gwork.slot.pointer = Gwork.slot.pointer + 1;
            if (Gwork.slot.pointer >= 3) {
                Gwork.slot.pointer = 0;
            }
            RES_SoundEffect(0x24);
        }
        if (PadData.half_2a & PAD_LEFT) {
            Gwork.slot.pointer = Gwork.slot.pointer - 1;
            if (Gwork.slot.pointer < 0) {
                Gwork.slot.pointer = 2;
            }
            RES_SoundEffect(0x25);
        }
        break;
    }
}

/* ov11:0x00a01e28. Configures background draw state, then emits the fixed
 * slot background sprite built from BoxRect (ov11:0x00a0a0e0) into BoxSpr
 * (ov11:0x00a0a0c0). */


static void SetTest(int value);

static void SlBgDraw(XglPacket *packet)
{
    SetDrawStatus(3, 0);
    SetTest(0x5000D);

    /* MakeSprite is the last statement of this void function and gcc 2.96
     * compiles a trailing call like that as a sibling jump; the original
     * keeps a real jal plus the shared epilogue (ld $31/jr $31). Wrapping
     * only this call in do{}while(0) defeats the sibcall pass instead
     * (CP-0288, config/compiler-patterns.json). */
    do {
        MakeSprite(&BoxSpr, &BoxRect);
    } while (0);
}

/* The four stake steps (1, 5, 10, 30) SlCoinEntry copies from the original
 * .rodata to its stack. */
struct StakeSteps {
    short step[4];
};
extern const struct StakeSteps D_00A0BCE8;

/* Non-union view of the slot work: SlCoinEntry reads and writes it through
 * the struct, not the Gwork union. */
#define SLOT_WORK ((struct SlotWork *)&Gwork)

/* ov11:0x00a01e68. Stake entry on the slot idle screen. Up / down move the
 * pay line (Gwork.slot.guide, wrapping within 0..4); right / left (with
 * auto-repeat) raise or lower that line's stake step (0..3) while the
 * total stays below the coin balance; any other button toggles the help
 * flag. line_pay is each step times the stake chosen by the menu level. */
static void SlCoinEntry(void)
{
    struct StakeSteps steps = D_00A0BCE8;
    int i;

    if (PadData.half_2a & PAD_UP) {
        Gwork.slot.guide = Gwork.slot.guide - 1;
        Gwork.slot.help = 0;
        if (Gwork.slot.guide < 0) {
            Gwork.slot.guide = 4;
        }
        RES_SoundEffect(0x1D);
    }
    if (PadData.half_2a & PAD_DOWN) {
        Gwork.slot.guide = Gwork.slot.guide + 1;
        Gwork.slot.help = 0;
        if (Gwork.slot.guide >= 5) {
            Gwork.slot.guide = 0;
        }
        RES_SoundEffect(0x1E);
    }
    if (CASINO_PAD->wide.repeat & PAD_RIGHT) {
        Gwork.slot.help = 0;
        if (Gwork.slot.line_bet[Gwork.slot.guide] < 3) {
            if (Gwork.slot.total_bet + steps.step[Gwork.slot.level] <
                ((struct CasinoSaveWork *)SaveWork)->coin_balance) {
                Gwork.slot.line_bet[Gwork.slot.guide]++;
                RES_SoundEffect(0x1F);
            } else {
                RES_SoundEffect(0x21);
            }
        }
    }
    if (CASINO_PAD->wide.repeat & PAD_LEFT) {
        Gwork.slot.help = 0;
        if (Gwork.slot.line_bet[Gwork.slot.guide] > 0) {
            Gwork.slot.line_bet[Gwork.slot.guide]--;
            RES_SoundEffect(0x20);
        }
    }
    if (PadData.half_2a & 0x99f) {
        if (Gwork.slot.help == 0) {
            Gwork.slot.help = 1;
        } else {
            Gwork.slot.help = 0;
        }
    }
    SLOT_WORK->total_bet = 0;
    for (i = 0; i < 5; i++) {
        SLOT_WORK->line_pay[i] = SLOT_WORK->line_bet[i] * steps.step[SLOT_WORK->level];
        SLOT_WORK->total_bet += SLOT_WORK->line_pay[i];
    }
}

/* The slot picture that counts as any other on a pay line (SlPrizeCheck). */
#define SLOT_PIC_WILD 6

/* ov11:0x00a02068. Scores one slot pay line. A line of three equal
 * pictures pays in full; picture 6 acts as a wild card (two equal pictures
 * plus a 6, or two 6s) and pays half, rounded up. The picture number then
 * selects the prize (5, 10, 20, 20, 1, 30, 0 or 30) and a kind (1, 2 or 3
 * for some pictures, otherwise 0); Gwork.slot.double_pay doubles the
 * prize. Not sure of the in-game picture names or what the kind values
 * mean. */
static void SlPrizeCheck(int index)
{
    struct SlotLine *line;
    int pic;

    line = &Gwork.slot.line[index];
    pic = -1;
    line->half = 1;
    if (line->pic[0] == line->pic[1] && line->pic[0] == line->pic[2]) {
        pic = line->pic[0];
        line->half = 0;
    }
    if (line->pic[0] == SLOT_PIC_WILD && line->pic[1] == line->pic[2]) {
        pic = line->pic[1];
    }
    if (line->pic[1] == SLOT_PIC_WILD && line->pic[0] == line->pic[2]) {
        pic = line->pic[0];
    }
    if (line->pic[2] == SLOT_PIC_WILD && line->pic[0] == line->pic[1]) {
        pic = line->pic[0];
    }
    if (line->pic[0] == SLOT_PIC_WILD) {
        if (line->pic[1] == SLOT_PIC_WILD) {
            pic = line->pic[2];
        }
        if (line->pic[2] == SLOT_PIC_WILD) {
            pic = line->pic[1];
        }
    }
    if (line->pic[1] == SLOT_PIC_WILD && line->pic[2] == SLOT_PIC_WILD) {
        pic = line->pic[0];
    }
    line->kind = 0;
    if (pic == -1) {
        return;
    }
    switch (pic) {
    case 0:
        line->prize = 5;
        break;
    case 1:
        line->prize = 10;
        line->kind = 1;
        break;
    case 2:
        line->prize = 20;
        line->kind = 1;
        break;
    case 3:
        line->prize = 20;
        line->kind = 1;
        break;
    case 4:
        line->prize = 1;
        line->kind = 2;
        break;
    case 5:
        line->prize = 30;
        break;
    case 7:
        line->prize = 30;
        line->kind = 3;
        break;
    default:
        line->prize = 0;
        break;
    }
    if (Gwork.slot.double_pay != 0) {
        line->prize *= 2;
    }
    if (line->half == 1) {
        line->prize = (line->prize + 1) / 2;
    }
}

/* The three reel cells read for each of the five pay lines: the reel and
 * the cell offset passed to SlGetPic. */
extern const struct SlotPatternPoint { short reel; short offset; } pat_22[5][3];
/* dprintf formats: "line %d [%d,%d,%d]\n", "prize = %d [%d]\n",
 * "paid = %d\n". */
extern const char D_00A0BD10[];
extern const char D_00A0BD28[];
extern const char D_00A0BD40[];

/* ov11:0x00a02260. Reads the three pictures of each pay line (pat_22),
 * then scores every line that has a stake (SlPrizeCheck), adds its prize
 * times the stake to the payout and keeps the highest kind 1-3. With a
 * payout, that kind becomes the slot event, or sound 0x42 plays when there
 * is none. The dprintf calls are the original's debug output. The second
 * read of the kind after its dprintf (kind and prize_kind) is what gives
 * the original's load and store order. */
static void SlLineCheck(void)
{
    int i;
    int k;
    int best;
    int amount;
    int kind;
    int prize_kind;

    for (i = 0; i < 5; i++) {
        struct SlotLine *ln = &Gwork.slot.line[i];

        ln->prize = 0;
        ln->kind = 0;
        for (k = 0; k < 3; k++) {
            int reel = pat_22[i][k].reel;

            ln->pic[reel] = SlGetPic(reel, pat_22[i][k].offset, Gwork.slot.row);
        }
    }
    for (i = 0; i < 5; i++) {
        short *pic = Gwork.slot.line[i].pic;

        dprintf(D_00A0BD10, i, pic[0], pic[1], pic[2]);
    }
    best = 0;
    for (i = 0; i < 5; i++) {
        struct SlotLine *ln = &Gwork.slot.line[i];

        if (Gwork.slot.line_pay[i] != 0) {
            SlPrizeCheck(i);
            amount = ln->prize * Gwork.slot.line_pay[i];
            prize_kind = ln->kind;
            Gwork.slot.payout += amount;
            dprintf(D_00A0BD28, amount, prize_kind);
            kind = ln->kind;
            if (kind == 1) {
                if (best <= 0) {
                    best = kind;
                }
            }
            if (kind == 2) {
                if (best < 2) {
                    best = kind;
                }
            }
            if (kind == 3) {
                if (best < 3) {
                    best = kind;
                }
            }
        }
    }
    if (Gwork.slot.payout != 0) {
        if (best != 0) {
            Gwork.slot.event = best;
        } else {
            RES_SoundEffect(0x42);
        }
    }
    dprintf(D_00A0BD40, Gwork.slot.payout);
}

/* ov11:0x00a02460. Runs the complete slot line evaluation and reports
 * whether the resulting payout is nonzero. */


static int check_real(void)
{
    SlLineCheck();
    return D_00A0DDC8 != 0;
}

extern int inter_23;
static void GameModeChange(int mode);

/* ov11:0x00a02488. Payout handler. When Gwork.slot.start_nine_game is
 * set it switches to the nine-game and stops the payout sound;
 * otherwise pressing circle pays the pending coins at once, else one coin per
 * frame while the pending amount counts down. Not sure of the exact game
 * wording. */
static int paid_action(void)
{
    if (Gwork.slot.start_nine_game == 1) {
        GameModeChange(CASINO_NINE_GAME);
        Gwork.slot.start_nine_game = 0;
        RES_SoundEffectStop(0x27);
        inter_23 = 0;
        return 1;
    }
    if (PadData.half_2a & PAD_CIRCLE) {
        AddCoin(Gwork.slot.payout);
        Gwork.slot.payout = 0;
        RES_SoundEffectStop(0x27);
    }
    if (Gwork.slot.payout == 0) {
        if (PadData.half_2a & PAD_CIRCLE) {
            inter_23 = 0;
            return 1;
        }
    } else if (Gwork.slot.payout > 0) {
        Gwork.slot.payout = Gwork.slot.payout - 1;
        if (Gwork.slot.payout == 0) {
            RES_SoundEffectStop(0x27);
        }
        AddCoin(1);
        inter_23 = inter_23 + 1;
        if (inter_23 == 1) {
            RES_SoundEffect(0x27);
        }
    }
    return 0;
}

extern const struct W22LineIds {int index[5];} D_00A0BD50;
static void linepay_check(void)
{
    struct W22LineIds ids = D_00A0BD50;
    char *root = (char *)&Gwork;
    char *score_root = root + 6;
    int cap = ((struct CasinoSaveWork *)SaveWork)->coin_balance;
    int sum = 0;
    int *p = ids.index;
    int count = 4;
    do {
        int offset = (*p) * 2;
        int scoreoff = offset + 80;
        short *score = (short *)((unsigned long)scoreoff + (unsigned long)score_root);
        char *at = (char *)((unsigned long)offset + (unsigned long)root);
        sum += *score;
        if (cap < sum) { *score = 0; *(short *)(at + 96) = 0; }
        ++p; --count;
    } while (count >= 0);
}


static void SlCoinEntry(void);

/* ov11:0x00a02630. Slot input handler, by Gwork.slot.state. Idle (0):
 * cross leaves to the menu; otherwise it runs SlCoinEntry, and circle
 * with a stake starts a spin (help off, pointer and reels reset to
 * spinning, the stake taken from the coin balance, sound 0x22). Spinning
 * (1): circle stops the reel under the pointer, a stopped reel slides on
 * by 0x10 per frame until it can snap to the next 64-pixel cell, and once
 * all three are still the result is checked (check_real, linepay_check).
 * Result (2): runs paid_action. A stop moves the pointer to the next reel. */
static void slot_key(void)
{
    int i;
    int stopped;
    int pressed;
    int rest;

    stopped = 0;
    pressed = 0;
    switch (Gwork.slot.state) {
    case 0:
        if (PadData.half_2a & PAD_CROSS) {
            RES_SoundEffect(0x17);
            GameModeChange(CASINO_MENU);
            break;
        }
        SlCoinEntry();
        if ((PadData.half_2a & PAD_CIRCLE) && Gwork.slot.total_bet > 0) {
            Gwork.slot.help = 0;
            Gwork.slot.pointer = 0;
            for (i = 0; i < 3; i++) {
                Gwork.slot.reel_state[i] = 1;
            }
            Gwork.slot.state = 1;
            ((struct CasinoSaveWork *)SaveWork)->coin_balance -= Gwork.slot.total_bet;
            RES_SoundEffect(0x22);
        }
        break;
    case 1:
        for (i = 0; i < 3; i++) {
            switch (Gwork.slot.reel_state[i]) {
            case 1:
                if (Gwork.slot.pointer == i && (PadData.half_2a & PAD_CIRCLE)) {
                    Gwork.slot.reel_state[i] = 2;
                    Gwork.slot.stop_travel[i] = 0;
                    RES_SoundEffect(0x23);
                    pressed = 1;
                }
                Gwork.slot.reel_pos[i] += RES_GetRealSpeed();
                break;
            case 2:
                if (Gwork.slot.stop_travel[i] >= 0) {
                    rest = 0x40 - Gwork.slot.reel_pos[i] % 64;
                    if (rest < 0x11) {
                        Gwork.slot.reel_state[i] = 0;
                        Gwork.slot.reel_pos[i] += rest;
                        break;
                    }
                }
                Gwork.slot.stop_travel[i] += 0x10;
                Gwork.slot.reel_pos[i] += 0x10;
                break;
            case 0:
                if ((PadData.half_2a & PAD_CIRCLE) && Gwork.slot.pointer == i) {
                    RES_SoundEffect(0x26);
                }
                stopped++;
                break;
            }
        }
        if (stopped == 3) {
            if (check_real() == 0) {
                Gwork.slot.state = 0;
            } else {
                Gwork.slot.state = 2;
            }
            linepay_check();
        }
        break;
    case 2:
        if (paid_action() != 0) {
            Gwork.slot.state = 0;
        }
        break;
    }
    if (pressed == 1) {
        Gwork.slot.pointer++;
        if (Gwork.slot.pointer >= 3) {
            Gwork.slot.pointer = 0;
        }
    }
}

extern const struct SpriteTex D_00A0BD68;
extern const struct SpriteRect D_00A0BD88;
extern int mode_24;
extern int D_00A0DC28;
extern int D_00A0DC2C;
extern int D_00A0DC30;
extern struct SpriteTex D_00A0DC38;
extern struct SpriteRect D_00A0DC58;

/* ov11:0x00a02938. Slot win banner animation, one step per call. Mode 0
 * loads the banner sprite (its v offset chosen by kind 1-4), resets the
 * counters and plays sound 0x28. Mode 1 shrinks the width towards 0x140
 * and grows the height to 0x40, keeping the banner centred on (0x100,
 * 0x90). Mode 2 holds for 16 more calls. Mode 3 shrinks the width to 0
 * while stretching the height to 0x800, then returns 0 and goes back to
 * mode 0. Otherwise it draws the banner and returns 1. Not sure what the
 * four kinds are. */
static int SlPrizeEffect(int kind)
{
    switch (mode_24) {
    case 0:
        D_00A0DC38 = D_00A0BD68;
        switch (kind) {
        case 1:
            D_00A0DC38.v = 0x40;
            break;
        case 2:
            D_00A0DC38.v = 0xC0;
            break;
        case 3:
            D_00A0DC38.v = 0x80;
            break;
        case 4:
            D_00A0DC38.v = 0;
            break;
        }
        D_00A0DC58 = D_00A0BD88;
        D_00A0DC28 = 0;
        D_00A0DC2C = -0x20;
        D_00A0DC30 = 0;
        mode_24 = 1;
        RES_SoundEffect(0x28);
        break;
    case 1:
        D_00A0DC58.w += D_00A0DC2C;
        D_00A0DC58.h += D_00A0DC30;
        D_00A0DC58.x = 0x100 - D_00A0DC58.w / 2;
        D_00A0DC58.y = 0x90 - D_00A0DC58.h / 2;
        if (D_00A0DC58.w <= 0x140) {
            D_00A0DC2C = 0;
            D_00A0DC58.w = 0x140;
        } else {
            D_00A0DC2C -= 8;
        }
        if (D_00A0DC58.h >= 0x40) {
            D_00A0DC30 = 0;
            D_00A0DC58.h = 0x40;
        } else {
            D_00A0DC30++;
        }
        if (D_00A0DC2C == 0 && D_00A0DC30 == 0) {
            mode_24 = 2;
        }
        break;
    case 2:
        if (++D_00A0DC28 > 0x10) {
            mode_24 = 3;
            D_00A0DC2C = -4;
            D_00A0DC30 = 0x10;
        }
        break;
    case 3:
        D_00A0DC58.w += D_00A0DC2C;
        D_00A0DC58.h += D_00A0DC30;
        D_00A0DC58.x = 0x100 - D_00A0DC58.w / 2;
        D_00A0DC58.y = 0x90 - D_00A0DC58.h / 2;
        if (D_00A0DC58.w < 0) {
            D_00A0DC2C = 0;
            D_00A0DC58.w = 0;
        } else {
            D_00A0DC2C -= 4;
        }
        if (D_00A0DC58.h >= 0x800) {
            D_00A0DC30 = 0;
            D_00A0DC58.h = 0x800;
        } else {
            D_00A0DC30 += 8;
        }
        if (D_00A0DC2C == 0 && D_00A0DC30 == 0) {
            mode_24 = 0;
            return 0;
        }
        break;
    }
    SetDrawStatus(1, 0);
    xglPacketTextureTrans((void *)SLOT_W1_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    MakeSprite(&D_00A0DC38, &D_00A0DC58);
    SetDrawStatus(3, 0);
    xglPacketTextureTrans((void *)SLOT_BASE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    return 1;
}

extern struct SpriteTex tex_30;
extern int mode_33;
extern struct SlReelPanel D_00A0DC68[3];
extern struct SlReelPanel *D_00A0DC8C;
extern int D_00A0DC90;

/* ov11:0x00a02d28. Slot reel-change animation, one step per call
 * (mode_33). Mode 0 resets the three panels with staggered delays. Mode 1
 * grows each panel to 0xC4 once its delay runs out (sound 0x29 when one
 * fills) and draws it. Mode 2 holds the full panels for 31 calls, then
 * picks the new reel row: 0 without a bonus, otherwise a random row 1-5
 * different from the current one. Mode 3 shrinks the panels. Mode 4 clears
 * or sets the bonus state (setting the bonus time from RES_GetBonusTime),
 * returns 0 and starts over. Returns 1 while the animation runs. Not sure
 * of the in-game meaning of the bonus state. */
static int SlChangeRealEffect(int bonus)
{
    struct SpriteRect rect;
    struct SlReelPanel *panel;
    int done;
    int i;
    unsigned int number;

    switch (mode_33) {
    case 0:
        for (i = 0; i < 3; i++) {
            D_00A0DC8C = &D_00A0DC68[i];
            D_00A0DC8C->delay = i * 4;
            D_00A0DC8C->state = 0;
            D_00A0DC8C->size = 0;
        }
        mode_33 = 1;
        return 1;
    case 1:
        done = 0;
        for (i = 0; i < 3; i++) {
            D_00A0DC8C = &D_00A0DC68[i];
            if (D_00A0DC8C->state == 0) {
                if (D_00A0DC8C->delay == 0) {
                    D_00A0DC8C->state = 1;
                } else {
                    D_00A0DC8C->delay--;
                }
            } else {
                if (D_00A0DC8C->size < 0xC4) {
                    D_00A0DC8C->size += 0x20;
                }
                if (D_00A0DC8C->size > 0xC4) {
                    D_00A0DC8C->size = 0xC4;
                    RES_SoundEffect(0x29);
                }
                if (D_00A0DC8C->size == 0xC4) {
                    done++;
                }
                tex_30.h = D_00A0DC8C->size;
                tex_30.v = 0x1D0 - D_00A0DC8C->size;
                rect = (struct SpriteRect){ 0x50 + i * 0x78, 0x32, 0x70, D_00A0DC8C->size };
                MakeSprite(&tex_30, &rect);
            }
        }
        if (done == 3) {
            mode_33 = 2;
            D_00A0DC90 = 0;
            RES_SoundEffect(0x29);
        }
        return 1;
    case 2:
        for (i = 0; i < 3; i++) {
            tex_30.h = 0xC4;
            tex_30.v = 0x10C;
            rect.x = 0x50 + i * 0x78;
            rect.y = 0x32;
            rect.w = 0x70;
            rect.h = 0xC4;
            MakeSprite(&tex_30, &rect);
        }
        if (++D_00A0DC90 >= 0x1F) {
            if (bonus == 0) {
                Gwork.slot.row = 0;
            } else {
                for (;;) {
                    number = xglSRand() % 5 + 1;
                    if (number != Gwork.slot.row) {
                        Gwork.slot.row = number;
                        break;
                    }
                }
            }
            mode_33 = 3;
        }
        return 1;
    case 3:
        done = 0;
        for (i = 0; i < 3; i++) {
            D_00A0DC8C = &D_00A0DC68[i];
            D_00A0DC8C->size -= 0x10;
            if (D_00A0DC8C->size < 0) {
                D_00A0DC8C->size = 0;
                done++;
            }
            tex_30.h = D_00A0DC8C->size;
            tex_30.v = 0x1D0 - D_00A0DC8C->size;
            rect = (struct SpriteRect){ 0x50 + i * 0x78, 0x32, 0x70, D_00A0DC8C->size };
            MakeSprite(&tex_30, &rect);
        }
        if (done == 3) {
            mode_33 = 4;
        }
        return 1;
    case 4:
        if (bonus == 0) {
            Gwork.slot.double_pay = 0;
            Gwork.slot.bonus = 0;
            Gwork.slot.row = 0;
        } else {
            Gwork.slot.bonus_time = RES_GetBonusTime();
            Gwork.slot.bonus = 1;
        }
        mode_33 = 0;
        return 0;
    }
    return 1;
}

extern int busy_35;

/* ov11:0x00a03150. Runs the current slot event (Gwork.slot.event), one step
 * per call. Events 1-4 run the win banner (SlPrizeEffect with the event as
 * its kind) and then: 1 and 4 move on to events 6 and 5; 2 turns on double
 * pay with a bonus time and sound 0x2B; 3 sets Gwork.slot.start_nine_game and plays
 * sound 0x2C. Event 5 just ends. Events 6 and 7 run the reel-change
 * animation with and without a bonus, then end with sounds 0x2A and 0x2D.
 * Returns 0 when idle (no event and busy_35 clear), otherwise 1. Not sure
 * of the in-game names of the events. */
static int SlEvent(int event)
{
    if (busy_35 == 0) {
        if (event == 0) {
            return 0;
        }
    }
    switch (event) {
    case 1:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.event = 6;
        }
        break;
    case 2:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.double_pay = 1;
            Gwork.slot.event = 0;
            Gwork.slot.bonus_time = RES_GetBonusTime();
            RES_SoundEffect(0x2B);
        }
        break;
    case 3:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.start_nine_game = 1;
            Gwork.slot.event = 0;
            RES_SoundEffect(0x2C);
        }
        break;
    case 4:
        if (SlPrizeEffect(event) == 0) {
            Gwork.slot.event = 5;
        }
        break;
    case 5:
        Gwork.slot.event = 0;
        break;
    case 6:
        if (SlChangeRealEffect(1) == 0) {
            Gwork.slot.event = 0;
            RES_SoundEffect(0x2A);
        }
        break;
    case 7:
        if (SlChangeRealEffect(0) == 0) {
            Gwork.slot.event = 0;
            RES_SoundEffect(0x2D);
        }
        break;
    }
    return 1;
}

extern const struct SpriteTex D_00A0BDE0;
extern const struct SpriteRect D_00A0BE00;

/* ov11:0x00a032c0. Slot help screen: uploads the help texture (0x0110A000)
 * and draws it as one sprite from the D_00A0BDE0 descriptor and D_00A0BE00
 * rectangle. Not sure what the help image shows. */
static void SlHelpMode(void)
{
    struct SpriteTex tex = D_00A0BDE0;
    struct SpriteRect rect = D_00A0BE00;

    SetDrawStatus(1, 0);
    xglPacketTextureTrans((void *)SLOT_HELP_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    MakeSprite(&tex, &rect);
}

static void SlModePrint(void);

/* ov11:0x00a03380. Slot main loop. Fetches the current packet, then draws
 * in order: the reels (SlRealDraw), cash, pay amounts and total stake, the
 * slot_1 background, the spin/stop/mode buttons and the pointer, the pay
 * window, and either the rate table or the result (Gwork.slot.state == 2
 * picks the result). Then runs SlEvent on Gwork.slot.event and, when it
 * returns 0, slot_key, and shows the help screen when Gwork.slot.help is
 * 1. Not sure of the exact game wording. */
static void slot_main(void)
{
    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)SLOT_BASE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SlRealDraw();
    SlCashPrint();
    pay_print(Pkt);
    total_pay_print(Pkt);
    xglPacketTextureTrans((void *)SLOT_1_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SlBgDraw(Pkt);
    xglPacketTextureTrans((void *)SLOT_BASE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SpinBtnPrint();
    SlStopBtnPrint();
    SlModePrint();
    point_control(Pkt);
    SlPayWindowPrint();
    if (Gwork.slot.state != 2) {
        SlRateTableDraw();
    } else {
        SlResultDraw();
    }
    if (SlEvent(Gwork.slot.event) == 0) {
        slot_key();
    }
    if (Gwork.slot.help == 1) {
        SlHelpMode();
    }
}

/* ov11:0x00a034d8. Loads the five slot-machine texture resources into their
 * fixed rendering buffers through the resource loader. */
static const char D_00A0BE10[] = "data\\tanaka\\slot_1.xtx";
static const char D_00A0BE28[] = "data\\tanaka\\slot_2.xtx";
static const char D_00A0BE40[] = "data\\tanaka\\base.xtx";
static const char D_00A0BE58[] = "data\\tanaka\\w1.xtx";
static const char D_00A0BE70[] = "data\\tanaka\\help_all.xtx";

static void init_slot(void)
{
    xglCdReadFile(D_00A0BE10, (void *)SLOT_1_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE28, (void *)SLOT_2_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE40, (void *)SLOT_BASE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE58, (void *)SLOT_W1_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE70, (void *)SLOT_HELP_TEX_BUFFER, 0, 0);
}

/* W09 candidate: link to extant OV11 symbol tex_36 at 0x00A0A160;
   no hard-coded address or new static data. */
extern const struct SpriteRect D_00A0BE90;
extern struct SpriteTex tex_36;
static void NgTimePrint(int time)
{
    struct SpriteRect rect = D_00A0BE90;
    if (time > 900) time = 900;
    rect.w = (time * 47 * 4) / 900;
    SetDrawStatus(3, 0);
    MakeSprite(&tex_36, &rect);
}


/* ov11:0x00a03610. Draws the pending nine-reel payout at its fixed screen
 * position, multiplied by the current power-of-two streak factor. pow
 * (main/tu284) computes the streak factor; litodp and dptoli (main/tu330,
 * main/tu331) are this compiler's own int<->double conversion routines.
 * They pass a double as a raw 64-bit value in an integer register (this
 * target has no hardware double support); a genuinely double-typed call
 * chain here schedules the calls' delay slots differently from the
 * original bytes, so the encoded form is required. DOUBLE_TWO is the
 * IEEE-754 double-precision encoding of the literal 2.0. */
#define DOUBLE_TWO 0x4000000000000000ULL

extern unsigned long long litodp(int value);
extern int dptoli(unsigned long long value);

static void NgPaidPrint(void)
{
    decprint(Gwork.nine.payout * dptoli(pow(2.0, litodp(Gwork.nine.counter))), 7, 0x1B2, 0x182);
}

extern struct SpriteTex tex_37[3];
extern struct SpriteRect rect_38;
extern int count_39;

/* ov11:0x00a03668. Nine-game: draws the "gets down" button. A non-negative
 * argument restarts the countdown count_39; the sprite is picked from
 * three textures by the sign of the countdown, which then counts down and
 * wraps to -1 at zero. */
static void NgGetsDownBtnPrint(int button)
{
    int index;

    if (button >= 0) {
        count_39 = button;
    }
    index = 0;
    if (count_39 < 0) {
        index = 2;
    }
    if (count_39 > 0) {
        index = 1;
    }
    SetDrawStatus(3, 0);
    MakeSprite(&tex_37[index], &rect_38);
    if (count_39 != 0) {
        count_39 = count_39 - 1;
        if (count_39 == 0) {
            count_39 = -1;
        }
    }
}

extern struct SpriteTex spinbt[3];
extern struct SpriteRect rect_40;
extern int count_41;

/* ov11:0x00a03710. Nine-game: same shape as NgGetsDownBtnPrint, for the
 * spin button (spinbt, count_41). */
static void NgSpinBtnPrint(int button)
{
    int index;

    if (button >= 0) {
        count_41 = button;
    }
    index = 0;
    if (count_41 < 0) {
        index = 2;
    }
    if (count_41 > 0) {
        index = 1;
    }
    SetDrawStatus(3, 0);
    MakeSprite(&spinbt[index], &rect_40);
    if (count_41 != 0) {
        count_41 = count_41 - 1;
        if (count_41 == 0) {
            count_41 = -1;
        }
    }
}

/* ov11:0x00a037b8. Nine-game lamp row: draws one RateLamp_Rect cell per
 * counted streak level with pay_window[1], then the cell after them with
 * pay_window[anm_42 % 2], and advances anm_42. Nothing is drawn while the
 * counter is negative or the guide is 2. */
extern struct SpriteRect RateLamp_Rect[9];
extern int anm_42;

static void NgLamp(void)
{
    int i;

    if (Gwork.nine.counter < 0 || Gwork.nine.guide == 2) {
        return;
    }
    SetDrawStatus(0, 0);
    for (i = 0; i < Gwork.nine.counter; i++) {
        MakeSprite(&pay_window[1], &RateLamp_Rect[i]);
    }
    MakeSprite(&pay_window[anm_42 % 2], &RateLamp_Rect[i]);
    anm_42++;
}

/* ov11:0x00a038c0. Selects the loaded nine-game slot background texture
 * (SLOT_2_TEX_BUFFER, the "slot_2.xtx" buffer init_slot fills), sets the
 * packet's GS register write and draw state, then emits the fixed
 * nine-game background sprite. SetRegAD and Pkt are this overlay's own
 * LOCAL data/sibling; xglPacketTextureTrans is main:0x0022c268
 * (src/main/xgl_packet.c, still INCLUDE_ASM there). */
extern void xglPacketTextureTrans(void *buffer);
static void SetRegAD(XglPacket *packet, int reg, int value);
/* Pkt defined above next to the packet helper candidates. */

static void NineGameBG_Draw(void)
{
    xglPacketTextureTrans((void *)SLOT_2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&NineGameBG_Tex_43, &NineGameBG_Rect_44);
}

static void NgStopGuidPt(void);

static void NgStopGuidPt(void)
{
    struct SpriteRect rect;

    rect.x = GuidPt_46[Gwork.nine.mode].x;
    rect.y = GuidPt_46[Gwork.nine.mode].y;
    rect.h = 8;
    rect.w = 8;
    MakeSprite(&GuidPtTex[seq_45 % 10], &rect);
    seq_45 = seq_45 + 1;
}

/* ov11:0x00a039b0. Nine-game cursor: moves Gwork.nine.mode on to the next
 * reel after it that is still running (state 0); if there is none, wraps
 * to the first running reel from 0 up to the old cursor; if there is none
 * at all, the cursor goes to the gets-down button (10). */
static void NgFwdGuidPt(void)
{
    int i;
    int next;

    struct NineGameReel *r;

    next = 10;
    for (i = Gwork.nine.mode + 1; i < 9; i++) {
        r = &Gwork.nine.reel[i];
        if (r->state == 0) {
            next = i;
            break;
        }
    }
    if (next == 10) {
        for (i = 0; i < Gwork.nine.mode; i++) {
            r = &Gwork.nine.reel[i];
            if (r->state == 0) {
                next = i;
                break;
            }
        }
    }
    Gwork.nine.mode = next;
}

extern int stopbtn_pos_47[9][2];

/* ov11:0x00a03a80. Nine-game: draws the nine reel stop buttons. Each
 * button sits at its stopbtn_pos_47 position, 16 by 24, and its texture
 * comes from the reel's state (0: stopbt[0], 1 or 3: stopbt[2], 2:
 * stopbt[1]); other states keep the previous texture. */
static void NgStopBtnPrint(void)
{
    struct SpriteRect rect;
    struct SpriteTex *tex = 0;
    struct NineGameReel *reel = Gwork.nine.reel;
    int (*pos)[2] = stopbtn_pos_47;
    int i;

    rect.w = 16;
    rect.h = 24;
    for (i = 0; i < 9; i++) {
        switch (reel->state) {
        case 1:
        case 3:
            tex = &stopbt[2];
            break;
        case 0:
            tex = &stopbt[0];
            break;
        case 2:
            tex = &stopbt[1];
            break;
        }
        /* the store to rect.x is overwritten below; it keeps i live */
        rect.x = i;
        rect.x = (*pos)[0];
        rect.y = (*pos)[1];
        MakeSprite(tex, &rect);
        reel++;
        pos++;
    }
}

extern short D_00A0DDE2[];

static void NgAllStop(void)
{
    int forced_stop;
    short *reel_status;
    int remaining;

    forced_stop = 3;
    reel_status = D_00A0DDE2;
    remaining = 8;
    do {
        if (reel_status[2] == 0) {
            reel_status[2] = forced_stop;
        }
        reel_status += 4;
    } while (--remaining >= 0);
    RES_SoundEffectStop(47);
}

extern short *ng_realtbl[];

/* ov11:0x00a03be0. Nine-game: looks up the reel's result in ng_realtbl for
 * its current position; a hit (== 1) bumps the counter, otherwise it stops
 * all reels and resets the counter, amount and mode (10). */
static void NgCheckReal(int index)
{
    struct NineGameReel *reel = &Gwork.nine.reel[index];
    short *table = ng_realtbl[reel->number];
    int position;

    if (reel->position >= 0x3F0) {
        reel->position = 0;
    }
    position = reel->position / 0x30;
    if (table[position] == 1) {
        Gwork.nine.counter = Gwork.nine.counter + 1;
    } else {
        NgAllStop();
        Gwork.nine.counter = -1;
        Gwork.nine.payout = 0;
        Gwork.nine.mode = 10;
    }
}

/* ov11:0x00a03ca0. Returns the 28-byte sprite descriptor of one reel's
 * shape: the reel's index table in ng_realtbl picks the entry of
 * NgReal_Tex_48, with the shape number wrapped into 0..20. */
extern const struct SpriteTex NgReal_Tex_48[];

static struct SpriteTex get_ngreal_shape(int reel, int shape)
{
    short *indices;
    int selected;
    if (shape >= 21) shape -= 21;
    if (shape < 0) shape += 21;
    indices = ng_realtbl[reel];
    selected = indices[shape];
    return NgReal_Tex_48[selected];
}

/* ov11:0x00a03d58. Nine-game reel panel: splits the reel position into
 * 48-pixel cells and draws the visible panel pieces from the shape table
 * NgReal_49 (x, y per reel index) and the sprites of get_ngreal_shape. */
struct NgPanelPos { short x; short y; };
extern struct NgPanelPos NgReal_49[9];

static void DrawNgPanel(int index, int y, int x)
{
    struct SpriteRect rect;
    struct SpriteTex tex;
    int quot;
    int rem;

    rect.w = 48;
    rect.h = 64;
    quot = y / 48;
    rem = y % 48;
    if (rem >= 25) {
        rem -= 48;
        quot += 1;
    }
    tex = get_ngreal_shape(x, quot);
    rect.y = NgReal_49[index].y;
    rect.x = NgReal_49[index].x + rem;
    MakeSprite(&tex, &rect);
    tex = get_ngreal_shape(x, quot - 1);
    rect.x = rect.x + 48;
    rect.w = 24 - rem;
    tex.w = rect.w;
    if (rect.w > 0) {
        MakeSprite(&tex, &rect);
    }
    tex = get_ngreal_shape(x, quot + 1);
    rect.x = rect.x - rem - 72;
    tex.u = tex.u - rem + 24;
    rect.w = rem + 24;
    tex.w = rem + 24;
    MakeSprite(&tex, &rect);
}


/* ov11:0x00a03eb0. Nine-game: draws the nine reel panels, then advances
 * each reel's state machine (+12 per frame, finishing when the remaining
 * distance runs out, wrapping at 0x3F0). Not sure of the exact game
 * wording. */
static void NgReal(void)
{
    int i;
    struct NineGameReel *reel;

    for (i = 0; i < 9; i++) {
        if (Gwork.nine.reel[i].state != 3) {
            DrawNgPanel(i, Gwork.nine.reel[i].position, Gwork.nine.reel[i].number);
        }
    }
    for (i = 0; i < 9; i++) {
        reel = &Gwork.nine.reel[i];
        if (reel->state == 0) {
            reel->position += 12;
        }
        if (reel->state == 2) {
            if (reel->remaining > 12) {
                reel->remaining -= 12;
                reel->position += 12;
            } else {
                reel->position += reel->remaining;
                reel->remaining = 0;
                reel->state = 1;
                NgCheckReal(i);
            }
        }
        if (reel->position >= 0x3F0) {
            reel->position = 0;
        }
    }
}

extern unsigned long long dpmul(unsigned long long a, unsigned long long b);
static void NgGetsDownBtnPrint(int button);

/* ov11:0x00a03fc0. Nine-game: scales the pending amount by 2^counter with
 * the double helpers, clears the counter, redraws the gets-down button and
 * stops all reels. */
static void NgGame_GetsDown(void)
{
    unsigned long long scale;

    Gwork.nine.guide = 2;
    scale = pow(2.0, litodp(Gwork.nine.counter));
    /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
     * the original instruction order here (as in act_2.c). */
    do { } while (0);
    Gwork.nine.payout = dptoli(dpmul(litodp(Gwork.nine.payout), scale));
    Gwork.nine.counter = 0;
    NgGetsDownBtnPrint(16);
    NgAllStop();
}

static void NgFwdGuidPt(void);

/* ov11:0x00a04040. Nine-game select handler: up/down toggles the cursor
 * between the spin (9) and gets-down (10) buttons, circle starts the spin
 * or the gets-down action, and the shoulder, face, select and start buttons
 * toggle the flag. Not sure what the flag shows. */
static void NgGame_Select(void)
{
    int i;

    if (PadData.half_2a & (PAD_DOWN | PAD_UP)) {
        Gwork.nine.flag = 0;
        RES_SoundEffect(0x2E);
        switch (Gwork.nine.mode) {
        case 9:
            Gwork.nine.mode = 10;
            break;
        case 10:
            Gwork.nine.mode = 9;
            break;
        }
    }
    if (PadData.half_2a & PAD_CIRCLE) {
        Gwork.nine.flag = 0;
        switch (Gwork.nine.mode) {
        case 9:
            for (i = 8; i >= 0; i--) {
                Gwork.nine.reel[i].state = 0;
            }
            Gwork.nine.guide = 1;
            NgFwdGuidPt();
            NgSpinBtnPrint(16);
            RES_SoundEffect(0x2F);
            break;
        case 10:
            NgGame_GetsDown();
            RES_SoundEffect(0x30);
            break;
        }
    }
    if (PadData.half_2a & (PAD_START | PAD_SELECT | PAD_SQUARE | PAD_TRIANGLE | PAD_R1 | PAD_L1 | PAD_R2 | PAD_L2)) {
        if (Gwork.nine.flag == 0) {
            Gwork.nine.flag = 1;
        } else {
            Gwork.nine.flag = 0;
        }
    }
}

/* Cursor moves of the nine-game: for each cursor position (0-8 the reels
 * in a 3x3 grid, 0-2 the top row; 9 the spin button, 10 the gets-down
 * button) the position reached with each direction button. In the table,
 * left and right wrap within a row and up/down step a row, so the columns
 * are, in order, left, up, right and down. */
enum NgCursorMove {
    NG_MOVE_LEFT,
    NG_MOVE_UP,
    NG_MOVE_RIGHT,
    NG_MOVE_DOWN
};
extern unsigned short ptmove_50[11][4];

/* ov11:0x00a04188. Nine-game input: the direction buttons (auto-repeat
 * word) move the cursor (Gwork.nine.mode) through ptmove_50, cross jumps to
 * the gets-down button (10), and circle acts on the cursor: on a reel that
 * is still running (state 0) it starts the stop (state 2, remaining = 96
 * minus position % 48) and moves the cursor on (NgFwdGuidPt); on 10 it
 * takes the winnings (NgGame_GetsDown). The case range is what gives the
 * original's compare order (below 0, below 9, equal to 10). */
static void NgGame_Key(void)
{
    struct NineGameReel *reel;

    if (CASINO_PAD->wide.repeat & PAD_LEFT) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_LEFT];
        RES_SoundEffect(0x2E);
    }
    if (CASINO_PAD->wide.repeat & PAD_UP) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_UP];
        RES_SoundEffect(0x2E);
    }
    if (CASINO_PAD->wide.repeat & PAD_RIGHT) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_RIGHT];
        RES_SoundEffect(0x2E);
    }
    if (CASINO_PAD->wide.repeat & PAD_DOWN) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][NG_MOVE_DOWN];
        RES_SoundEffect(0x2E);
    }
    if (PadData.half_2a & PAD_CROSS) {
        Gwork.nine.mode = 10;
        RES_SoundEffect(0x31);
    }
    if (PadData.half_2a & PAD_CIRCLE) {
        switch (Gwork.nine.mode) {
        case 0 ... 8:
            reel = &Gwork.nine.reel[Gwork.nine.mode];
            if (reel->state == 0) {
                reel->state = 2;
                reel->remaining = 96 - reel->position % 48;
                NgFwdGuidPt();
                RES_SoundEffect(0x32);
            } else {
                RES_SoundEffect(0x33);
            }
            break;
        case 10:
            NgGame_GetsDown();
            RES_SoundEffect(0x30);
            break;
        }
    }
}

extern int se_51;

/* ov11:0x00a04360. Nine-game payout step: when nothing is left to pay it
 * stops the payout sound, switches back to the slot game and clears
 * se_51; otherwise it starts the payout sound once and pays out coins, all
 * at once if circle is pressed and one per call otherwise. */
static void NgGame_Paid(void)
{
    int amount;

    if (Gwork.nine.payout == 0) {
        RES_SoundEffectStop(0x34);
        GameModeChange(CASINO_SLOT);
        se_51 = 0;
        return;
    }
    if (se_51 == 0) {
        RES_SoundEffect(0x34);
        se_51 = 1;
    }
    amount = Gwork.nine.payout;
    if (PadData.half_2a & PAD_CIRCLE) {
        AddCoin(amount);
        Gwork.nine.payout = 0;
    } else {
        Gwork.nine.payout = amount - 1;
        AddCoin(1);
    }
}

extern const struct SpriteTex D_00A0BEA0;
extern const struct SpriteTex D_00A0BEC0;
extern const struct SpriteRect D_00A0BEE0;
extern const struct SpriteRect D_00A0BEF0;

/* ov11:0x00a04410. Nine-game help screen: two textured sprites from fixed
 * tables. */
static void NgHelpMode(void)
{
    struct SpriteTex tex1 = D_00A0BEA0;
    struct SpriteTex tex2 = D_00A0BEC0;
    struct SpriteRect rect1 = D_00A0BEE0;
    struct SpriteRect rect2 = D_00A0BEF0;

    SetDrawStatus(1, 0);
    xglPacketTextureTrans((void *)SLOT_HELP_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    MakeSprite(&tex1, &rect1);
    MakeSprite(&tex2, &rect2);
}


/* ov11:0x00a04560. Nine-game main loop. Fetches the current packet and
 * draws the reels, paid amount, background, lamps, the two buttons, the
 * countdown (time_left), the stop buttons and the guide pointer. Then runs
 * the handler for the current guide state (0 select, 1 key, 2 paid); in
 * state 1 it counts time_left down and calls NgGame_GetsDown when it runs
 * out, and it shows the help screen when the flag is 1. Not sure of the
 * exact game wording. */
static void NineGameMain(void)
{
    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)SLOT_BASE_TEX_BUFFER);
    NgReal();
    NgPaidPrint();
    NineGameBG_Draw();
    xglPacketTextureTrans((void *)SLOT_BASE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    NgLamp();
    NgSpinBtnPrint(-1);
    NgGetsDownBtnPrint(-1);
    NgTimePrint(Gwork.nine.time_left);
    NgStopBtnPrint();
    NgStopGuidPt();
    switch (Gwork.nine.guide) {
    case 0:
        NgGame_Select();
        break;
    case 1:
        NgGame_Key();
        break;
    case 2:
        NgGame_Paid();
        break;
    }
    if (Gwork.nine.guide == 1) {
        Gwork.nine.time_left--;
        if (Gwork.nine.time_left <= 0) {
            NgGame_GetsDown();
        }
    }
    if (Gwork.nine.flag == 1) {
        NgHelpMode();
    }
}

/* ov11:0x00a046d0. Nine-game start-up: clears the counter and guide, sets
 * mode 9, resets all nine reels, then gives each reel a unique random
 * number in 0..19 (re-rolling on a duplicate) and redraws the two buttons.
 * Not sure of the exact game wording. */
static void init_ninegame(void)
{
    int i;
    int j;
    unsigned short number;

    Gwork.nine.counter = 0;
    Gwork.nine.guide = 0;
    Gwork.nine.mode = 9;
    Gwork.nine.time_left = 0x384;
    for (i = 0; i < 9; i++) {
        Gwork.nine.reel[i].number = -1;
        Gwork.nine.reel[i].position = 0;
        Gwork.nine.reel[i].state = 1;
    }
    for (i = 0; i < 9; i++) {
        number = xglSRand() % 20;
        for (j = 0; j < i; j++) {
            if (number == Gwork.nine.reel[j].number) {
                number = xglSRand() % 20;
                j = -1;
            }
        }
        Gwork.nine.reel[i].number = number;
    }
    NgGetsDownBtnPrint(0);
    NgSpinBtnPrint(0);
    Gwork.nine.flag = 1;
}

#define DOUBLE_TEN 0x4024000000000000ULL

extern const struct SpriteRect D_00A0BF00;
extern const struct SpriteRect D_00A0BF10;
extern struct SpriteTex fontlist_52[10];
extern struct SpriteTex waku_53[2];

/* ov11:0x00a04828. Poker: draws a coin counter, a frame sprite picked by
 * mode and then the decimal digits of value right to left, 16 pixels apart
 * from x = 472 (a single 0 for a value below 1). The digit count comes from
 * comparing value with the powers of ten. */
static void PoMoneyPrint(int mode, int value)
{
    int digits = 0;
    struct SpriteRect digit_rect = D_00A0BF00;
    struct SpriteRect frame_rect = D_00A0BF10;
    int i;
    unsigned long long scale;
    unsigned long long amount;

    while (digits < 11) {
        double amount_d = value;
        if (dpcmp(amount_d, pow(10.0, litodp(digits))) >= 0) {
            digits++;
        } else {
            break;
        }
    }
    SetDrawStatus(3, 0);
    MakeSprite(&waku_53[mode], &frame_rect);
    if (digits > 0) {
        for (i = 0; i < digits;) {
            digit_rect.x = 472 - i * 16;
            scale = pow(10.0, litodp(i++));
            do { } while (0);
            amount = litodp(value);
            do { } while (0);
            MakeSprite(&fontlist_52[dptoli(dpdiv(amount, scale)) % 10], &digit_rect);
        }
    } else {
        MakeSprite(&fontlist_52[0], &digit_rect);
    }
}

extern struct SpriteTex PoCurTex[8];
extern int cnt_54;
extern struct SpriteRect D_00A0DC98;

extern struct SpriteTex PoCurTex[8];
extern int cnt_54;
extern struct SpriteRect D_00A0DC98;

/* ov11:0x00a04a08. Poker: draws the animated cursor at pos (x, y), a 24 by
 * 24 rectangle, using the frame counter cnt_54 to pick one of the eight
 * PoCurTex frames, then advances the counter. */
static void PoCurDraw(short *pos)
{
    struct SpriteRect *rect;

    xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    rect = &D_00A0DC98;
    rect->h = 24;
    rect->w = 24;
    rect->x = pos[0];
    rect->y = pos[1];
    MakeSprite(PoCurTex + cnt_54 % 8, rect);
    cnt_54 = cnt_54 + 1;
}

extern const struct SpriteRect D_00A0BF20;
extern struct SpriteTex Ltex_56;
extern struct SpriteTex Rtex_57;
extern struct SpriteTex tex_58;
extern const char D_00A0BF30[];
extern const char D_00A0BF38[];
extern void xglFontPrint(int x, int y, int color, const char *text);

/* ov11:0x00a04ac8. Poker: draws a message window sized by the text length
 * (left cap, right cap, middle) and prints the message. */
static void PoWindowMessage(const char *message, int length)
{
    struct SpriteRect rect = D_00A0BF20;

    SetDrawStatus(3, 0);
    SetTest(0x5000D);
    rect.x = 16;
    rect.w = 8;
    MakeSprite(&Ltex_56, &rect);
    rect.x = length * 9 + 28;
    MakeSprite(&Rtex_57, &rect);
    rect.x = 24;
    rect.w = length * 9 + 4;
    MakeSprite(&tex_58, &rect);
    xglFontPrint(0, 0, 0, D_00A0BF30);
    xglFontPrint(0, 0, 0, D_00A0BF38);
    xglFontPrint(24, 218, 0xFF00, message);
}

extern struct SpriteTex tex_59;
extern struct SpriteRect rect_60[2];
extern struct SpriteTex type0_tex_61;
extern struct SpriteRect type0_rect_62;
extern struct SpriteTex type1_tex_63;
extern struct SpriteRect type1_rect_64;
extern short type0_pos_65[2][2];
extern short type1_pos_66[3][2];
extern int dialog_cur_67;
static void PoCurDraw(short *pos);
static short PoGetBase(int cardValue);

/* ov11:0x00a04be0. Poker yes/no style dialog. type 0 shows a two-choice
 * window and any other type a three-choice window. Up moves the cursor
 * (dialog_cur_67) up with wraparound and down moves it down, both with a
 * sound; circle confirms (plays a sound, resets the cursor and returns the
 * chosen entry) and cross cancels (returns -1); otherwise it
 * returns 0. Not sure of the exact game wording. */
static int PoDialog(int type)
{
    int limit;
    int selected;

    xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    limit = 2;
    if (type != 0) {
        limit = 3;
    }
    if (PadData.half_2a & PAD_UP) {
        dialog_cur_67--;
        RES_SoundEffect(0x3C);
        if (dialog_cur_67 == 0) {
            dialog_cur_67 = limit;
        }
    }
    if (PadData.half_2a & PAD_DOWN) {
        RES_SoundEffect(0x3C);
        dialog_cur_67++;
        if (dialog_cur_67 == limit + 1) {
            dialog_cur_67 = 1;
        }
    }
    SetDrawStatus(3, 0);
    if (type == 0) {
        MakeSprite(&tex_59, &rect_60[0]);
        MakeSprite(&type0_tex_61, &type0_rect_62);
        PoCurDraw(type0_pos_65[dialog_cur_67 - 1]);
    } else {
        MakeSprite(&tex_59, &rect_60[1]);
        MakeSprite(&type1_tex_63, &type1_rect_64);
        PoCurDraw(type1_pos_66[dialog_cur_67 - 1]);
    }
    if (PadData.half_2a & PAD_CIRCLE) {
        RES_SoundEffect(0x3F);
        selected = dialog_cur_67;
        dialog_cur_67 = 1;
        return selected;
    }
    if (PadData.half_2a & PAD_CROSS) {
        RES_SoundEffect(0x40);
        dialog_cur_67 = 1;
        return -1;
    }
    return 0;
}

/* ov11:0x00a04dc0. Selects the loaded poker table-1 background texture
 * (0x0138f000, the "poker_1.xtx" buffer PokerInit below fills as
 * POKER_TABLE1_TEX_BUFFER) and emits the fixed poker background sprite. */

static void PoBgDraw(void)
{
    SetDrawStatus(3, 0);
    xglPacketTextureTrans((void *)POKER_TABLE1_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&tex_68, &rect_69);
}

extern void *CardBuf_70[4];
extern struct SpriteTex CardTex[15];
extern struct SpriteRect CardPos[5];
extern struct PoCardRange { int lo; int hi; } range_71[4];

/* ov11:0x00a04e20. Poker: for each of the four suits selects the card
 * texture buffer and draws the held cards that fall in that suit's number
 * range. */
static void PoCardDraw(void)
{
    int suit;
    int slot;
    int base;

    base = 0;
    SetDrawStatus(3, 0);
    SetTest(0x5000D);
    for (suit = 0; suit < 4; suit++) {
        xglPacketTextureTrans(CardBuf_70[suit]);
        SetRegAD(Pkt, 0x3F, 0);
        for (slot = 0; slot < 5; slot++) {
            int card = Gwork.poker_deal.cards.hand[slot];
            if (card >= range_71[suit].lo && card <= range_71[suit].hi) {
                MakeSprite(&CardTex[card - base], &CardPos[slot]);
            }
        }
        base += 13;
    }
}

extern struct SpriteRect D_00A0DCA8[5];
extern int D_00A0DCF8;
extern int status_73;
extern struct SpriteTex tex_72;

/* ov11:0x00a04f60. Poker deal animation for all five cards at once, driven
 * by status_73. State 0 copies each card's slot rect and moves it up and
 * left by an amount that grows per card; state 1 draws the five card
 * sprites, moves each down 0x20 per call (a sound plays when a card's y
 * reaches 5) until all five arrive, then switches to state 2 with a 4-call
 * hold; state 2 keeps drawing and, when the hold ends, resets to state 0
 * and returns 1. Otherwise returns 0. Not sure of the exact game wording. */
static int PoCardDrawAnime(void)
{
    int i;
    int arrived;

    xglPacketTextureTrans((void *)POKER_SPADE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    switch (status_73) {
    case 0:
        for (i = 0; i < 5; i++) {
            D_00A0DCA8[i] = CardPos[i];
            D_00A0DCA8[i].y = D_00A0DCA8[i].y - i * 128 - 0xA0;
        }
        status_73 = 1;
        /* fall through */
    case 1:
        arrived = 0;
        for (i = 0; i < 5; i++) {
            MakeSprite(&tex_72, &D_00A0DCA8[i]);
            if (D_00A0DCA8[i].y == 5) {
                RES_SoundEffect(0x36);
            }
            if (D_00A0DCA8[i].y < CardPos[i].y) {
                D_00A0DCA8[i].y += 0x20;
            } else {
                arrived++;
            }
        }
        if (arrived == 5) {
            status_73 = 2;
            D_00A0DCF8 = 4;
        }
        return 0;
    case 2:
        for (i = 0; i < 5; i++) {
            MakeSprite(&tex_72, &D_00A0DCA8[i]);
        }
        if (--D_00A0DCF8 == 0) {
            status_73 = 0;
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

extern struct SpriteRect D_00A0DD00;
extern int D_00A0DD10;
extern int status_77;
extern struct SpriteTex tex_76;

/* ov11:0x00a05180. Poker bonus card animation, one step per call, driven
 * by status_77 and the card index in Gwork.poker_deal.bonus_round. State 0 starts a card
 * sprite at the card's slot position moved up by 0xA0; state 1 draws it,
 * moves it down 0x20 per call (a sound plays at y == 5) until it reaches
 * the slot, then switches to state 2 with a 4-call hold; in state 2 it
 * keeps drawing and, when the hold runs out, sets that hand slot to 0x35
 * (face-down, not sure) and marks it selected, returning 1. Otherwise
 * returns 0. Not sure of the exact game wording. */
static int PoBonusCardAnime(void)
{
    int card;
    int blank;

    xglPacketTextureTrans((void *)POKER_SPADE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    card = Gwork.poker_deal.bonus_round;
    switch (status_77) {
    case 0:
        status_77 = 1;
        D_00A0DD00 = CardPos[card];
        D_00A0DD00.y -= 0xA0;
        /* fall through */
    case 1:
        MakeSprite(&tex_76, &D_00A0DD00);
        D_00A0DD00.y += 0x20;
        if (D_00A0DD00.y == 5) {
            RES_SoundEffect(0x3D);
        }
        if (D_00A0DD00.y >= CardPos[card].y) {
            status_77 = 2;
            D_00A0DD10 = 4;
        }
        return 0;
    case 2:
        MakeSprite(&tex_76, &D_00A0DD00);
        blank = 0x35;
        if (--D_00A0DD10 == 0) {
            status_77 = 0;
            Gwork.poker_deal.cards.hand[card] = blank;
            Gwork.poker_deal.replace_selected[card] = 1;
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

/* ov11:0x00a05358. Emits the single fixed poker payout table sprite. */

static void PoRateTableDraw(void)
{
    MakeSprite(&tex_80, &rect_81);
}

extern int cnt_82;
extern int add_83;
extern int rate_84;
extern struct SpriteTex tex_85;
extern struct SpriteRect rect_86;
extern struct SpriteTex MarkerTex_87;
extern struct SpriteRect MarkerPos_88[5];
extern struct SpriteTex PtrTex_89;
extern struct SpriteRect PtrPos_90[4];

/* ov11:0x00a05380. Poker bonus-round background: draws the background
 * sprite, then pulses rate_84 up and down by 8 over a 16-frame cycle
 * (cnt_82, add_83). It draws one marker per bonus card so far
 * (Gwork.poker_deal.bonus_round) and then one pointer per card, the last with the pulsing blend
 * value and the others with 0x30. Not sure what the markers and pointers
 * mean in the game, or why the pointer loop also stops at 5. */
static void PoBonusBGDraw(void)
{
    int i;

    xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    MakeSprite(&tex_85, &rect_86);
    if (cnt_82 < 8) {
        add_83 = 8;
    } else {
        add_83 = -8;
    }
    cnt_82 = cnt_82 + 1;
    if (cnt_82 == 16) {
        cnt_82 = 0;
    }
    rate_84 = rate_84 + add_83;
    SetDrawStatus(6, 0x40);
    for (i = 0; i < Gwork.poker_deal.bonus_round; i++) {
        MakeSprite(&MarkerTex_87, &MarkerPos_88[i]);
    }
    for (i = 0; i < Gwork.poker_deal.bonus_round && i != 5; i++) {
        if (i == Gwork.poker_deal.bonus_round - 1) {
            SetDrawStatus(6, rate_84);
        } else {
            SetDrawStatus(6, 0x30);
        }
        MakeSprite(&PtrTex_89, &PtrPos_90[i]);
    }
}

/* Poker's deck-use flags, dealt cards and replacement choices occupy
 * these fields of Gwork. PoShuffleCard uses replacement value 1 to deal
 * a slot; card value 53 denotes an empty hand slot. */
static void PoResetCard(void)
{
    int index;
    for (index = 52; index >= 0; index--) {
        Gwork.poker_deal.cards.used[index] = 0;
    }
    for (index = 0; index < 5; index++) {
        Gwork.poker_deal.cards.hand[index] = 53;
        Gwork.poker_deal.replace_selected[index] = 1;
    }
}

extern int wait_91;

/* Returns 1 once no replacement slot remains selected. */
static int PoShuffleCard(void)
{
    int i;
    unsigned short card = 0;
    int count;

    count = 0;
    for (i = 0; i < 5; i++) {
        if (Gwork.poker_deal.replace_selected[i] == 1) {
            count++;
        }
    }
    if (count == 0) {
        return 1;
    }
    if (--wait_91 != 0) {
        return 0;
    }
    i = 0;
    while (i < 5) {
        if (Gwork.poker_deal.replace_selected[i] == 1) {
            break;
        }
        i++;
    }
    for (;;) {
        if (Gwork.poker_deal.bonus_mode == 0) {
            card = xglSRand() % 52;
        } else {
            switch (i) {
            case 0:
                card = 0;
                break;
            case 1:
                card = 9;
                break;
            case 2:
                card = 10;
                break;
            case 3:
                card = 11;
                break;
            case 4:
                card = 12;
                break;
            }
        }
        if (Gwork.poker_deal.cards.used[card] == 1) {
            continue;
        }
        Gwork.poker_deal.cards.hand[i] = card;
        Gwork.poker_deal.cards.used[card] = 1;
        break;
    }
    RES_SoundEffect(65);
    Gwork.poker_deal.replace_selected[i] = 0;
    wait_91 = 8;
    return 0;
}

extern struct SpriteTex PoSelecterTex[2];
extern struct SpriteRect PoSelecterPos[6];

/* ov11:0x00a05758. Poker: draws the five selection marks, texture picked
 * from replace_selected[], then one fixed sprite. 0x013CF800 is the buffer
 * POKER_TABLE2_TEX_BUFFER names below. */
static void PoSelectMarkDraw(void)
{
    int i;

    xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(1, 0);
    for (i = 0; i < 5; i++) {
        MakeSprite(&PoSelecterTex[Gwork.poker_deal.replace_selected[i]], &PoSelecterPos[i]);
    }
    MakeSprite(&PoSelecterTex[1], &PoSelecterPos[5]);
}

extern int cur_92;
extern int mode_93;
extern short selpos_94[5][2];
extern short chgpos_95[2];

/* ov11:0x00a05818. Poker: card selection. Up and down switch mode_93
 * between the card cursor (0) and the confirm button (1). In card mode
 * right and left (auto-repeat) move the cursor cur_92 over the five cards
 * with wraparound and circle toggles the card's replace flag. In button
 * mode circle plays a sound, resets both and returns 1; otherwise 0. */
static int PoKey(void)
{
    if (PadData.half_2a & PAD_UP) {
        RES_SoundEffect(0x3E);
        mode_93 = 0;
    }
    if (PadData.half_2a & PAD_DOWN) {
        RES_SoundEffect(0x3E);
        mode_93 = 1;
    }
    if (mode_93 == 1) {
        PoCurDraw(chgpos_95);
        if (PadData.half_2a & PAD_CIRCLE) {
            RES_SoundEffect(0x37);
            cur_92 = 0;
            mode_93 = 0;
            return 1;
        }
    } else {
        if (CASINO_PAD->wide.repeat & PAD_RIGHT) {
            RES_SoundEffect(0x3E);
            cur_92++;
            if (cur_92 >= 5) {
                cur_92 = 0;
            }
        }
        if (CASINO_PAD->wide.repeat & PAD_LEFT) {
            RES_SoundEffect(0x3E);
            cur_92--;
            if (cur_92 < 0) {
                cur_92 = 4;
            }
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            RES_SoundEffect(0x37);
            if (Gwork.poker_deal.replace_selected[cur_92] == 0) {
                Gwork.poker_deal.replace_selected[cur_92] = 1;
            } else {
                Gwork.poker_deal.replace_selected[cur_92] = 0;
            }
        }
        PoCurDraw(selpos_94[cur_92]);
    }
    return 0;
}

extern int status_96;

/* ov11:0x00a059b8. Poker: sets cards the player chose to replace to 53
 * (face-down, not sure); when all five are chosen for 32 frames it sets
 * the flag at Gwork+0x190 and clears the used-card table. */
static void PoCardFlip(void)
{
    int i;
    int count;

    count = 0;
    for (i = 0; i < 5; i++) {
        if (Gwork.poker_deal.replace_selected[i] == 1) {
            Gwork.poker_deal.cards.hand[i] = 53;
            count++;
        }
    }
    if (count == 5) {
        status_96 = status_96 + 1;
    } else {
        status_96 = 0;
    }
    if (status_96 >= 32) {
        Gwork.poker_deal.bonus_mode = 1;
        for (i = 0; i < 53; i++) {
            Gwork.poker_deal.cards.used[i] = 0;
        }
    } else {
        Gwork.poker_deal.bonus_mode = 0;
    }
}

/* ov11:0x00a05a78. Maps a 0..0x33 standard 52-card value (13 ranks per
 * suit) to its suit index. Callers (PoChk4 and PokerMain) read cardValue
 * as a signed `lh` from the sorted hand in Gwork; any value outside
 * 0..0x33 reports 0, matching the pre-switch init below. */
static int PoGetMark(int cardValue)
{
    int mark;

    mark = 0;
    switch (cardValue) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xC:
        mark = 0;
        break;
    case 0xD:
    case 0xE:
    case 0xF:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
        mark = 1;
        break;
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
        mark = 2;
        break;
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
        mark = 3;
        break;
    }
    return mark;
}


static short PoGetBase(int cardValue)
{
    short base;

    base = 0;
    if (cardValue != 1) {
        if (cardValue >= 2) {
            switch (cardValue) {
            case 2:
                base = 26;
                break;
            case 3:
                base = 39;
                break;
            }
        }
    } else {
        base = 13;
    }
    return base;
}

extern const char D_00A0C030[];
extern const char D_00A0C038[];

/* ov11:0x00a05b10. Poker: copies the five hand cards into sorted_hand,
 * bubble-sorts them and prints each with dprintf (debug output). */
static void PoSortCard(void)
{
    int i;
    int n;

    for (i = 0; i < 5; i++) {
        Gwork.poker_deal.sorted_hand[i] = Gwork.poker_deal.cards.hand[i];
    }
    for (n = 4; n != 0; n--) {
        for (i = 0; i < n; i++) {
            short prev = Gwork.poker_deal.sorted_hand[i];
            if (Gwork.poker_deal.sorted_hand[i + 1] < prev) {
                Gwork.poker_deal.sorted_hand[i] = Gwork.poker_deal.sorted_hand[i + 1];
                Gwork.poker_deal.sorted_hand[i + 1] = prev;
            }
        }
    }
    for (i = 0; i < 5; i++) {
        dprintf(D_00A0C030, Gwork.poker_deal.sorted_hand[i]);
    }
    dprintf(D_00A0C038);
}

extern const char D_00A0C040[];
extern const char D_00A0C048[];
extern const char D_00A0C050[];
extern const char D_00A0C058[];

/* ov11:0x00a05bf0. Poker: counts, over all ordered pairs of the five
 * sorted hand cards, how many differ by exactly 13, 26 or 39 (same rank in
 * another suit), printing a debug line for each hit. The match count
 * (1..4, 6) maps to a category 1..5 (pair, two pair, three of a kind, full
 * house and four of a kind, as far as I can tell); anything else returns
 * 0. Not sure of the exact category names. */
static int PoMultiChk(void)
{
    int i;
    int j;
    int matches;
    int card;
    int other;

    matches = 0;
    for (i = 0; i < 5; i++) {
        card = Gwork.poker_deal.sorted_hand[i];
        for (j = 0; j < 5; j++) {
            if (i == j) {
                continue;
            }
            other = Gwork.poker_deal.sorted_hand[j];
            if (other == card + 13) {
                matches++;
                dprintf(D_00A0C040);
            }
            if (other == card + 26) {
                matches++;
                dprintf(D_00A0C048);
            }
            if (other == card + 39) {
                matches++;
                dprintf(D_00A0C050);
            }
        }
        dprintf(D_00A0C058);
    }
    dprintf(D_00A0C038);
    switch (matches) {
    case 6:
        return 5;
    case 4:
        return 4;
    case 3:
        return 3;
    case 2:
        return 2;
    case 1:
        return 1;
    default:
        return 0;
    }
}

struct PokerHandWork {
    unsigned char unmodeled_00[0x1A2];
    short cards[5];
};

static int PoFlushChk(void)
{
    struct PokerHandWork *hand;
    short *card;
    int suit;
    int index;

    hand = (struct PokerHandWork *)Gwork.words;
    card = &hand->cards[1];
    suit = PoGetMark(hand->cards[0]);
    for (index = 1; index < 5; index++) {
        if (suit != PoGetMark(*card++)) {
            return 0;
        }
    }
    return 1;
}

/* PoMultiChk classifies the sorted hand's rank multiplicities and
 * PoFlushChk classifies same-suit runs. dprintf is the overlay's debug
 * printf. */
static int PoMultiChk(void);
static int PoFlushChk(void);
extern void dprintf(const char *fmt, ...);

/* ov11:0x00a05de0. Reports category 1, printing D_00A0C078 through
 * dprintf, when PoMultiChk's multiplicity equals 1. */
/* JP: ワンペア\n
 * EN: One pair\n */
static const char D_00A0C078[16] = "\xA5\xEF\xA5\xF3\xA5\xDA\xA5\xA2\n";

static int PoChk1(void)
{
    if (PoMultiChk() == 1) {
        dprintf(D_00A0C078);
        return 1;
    }
    return 0;
}

/* ov11:0x00a05e20. Reports category 2, printing D_00A0C088 through
 * dprintf, when PoMultiChk's multiplicity equals 2. */
/* JP: ツーペア\n
 * EN: Two pair\n */
static const char D_00A0C088[16] = "\xA5\xC4\xA1\xBC\xA5\xDA\xA5\xA2\n";

static int PoChk2(void)
{
    if (PoMultiChk() == 2) {
        dprintf(D_00A0C088);
        return 2;
    }
    return 0;
}

/* ov11:0x00a05e60. Reports category 3, printing D_00A0C098 through
 * dprintf, when PoMultiChk's multiplicity equals 3. */
/* JP: スリーカード\n
 * EN: Three of a kind\n */
static const char D_00A0C098[16] = "\xA5\xB9\xA5\xEA\xA1\xBC\xA5\xAB\xA1\xBC\xA5\xC9\n";

static int PoChk3(void)
{
    if (PoMultiChk() == 3) {
        dprintf(D_00A0C098);
        return 3;
    }
    return 0;
}

extern const char D_00A0C0A8[];

/* ov11:0x00a05ea0. Poker: category 4. Sorts the hands' rank offsets and,
 * when they are consecutive, prints D_00A0C0A8 with dprintf and returns 4
 * (looks like a straight; not sure of the label). */
static int PoChk4(void)
{
    int diff[5];
    int count;
    int i;
    int n;

    for (count = 0; count < 5; count++) {
        int value = Gwork.poker_deal.sorted_hand[count];
        diff[count] = value - PoGetBase(PoGetMark(value));
    }
    for (n = 4; n != 0; n--) {
        for (i = 0; i < n; i++) {
            int prev = diff[i];
            if (diff[i + 1] < prev) {
                diff[i] = diff[i + 1];
                diff[i + 1] = prev;
            }
        }
    }
    if (diff[1] == diff[0] + 1 && diff[2] == diff[0] + 2 &&
        diff[3] == diff[0] + 3 && diff[4] == diff[0] + 4) {
        dprintf(D_00A0C0A8);
        return 4;
    }
    return 0;
}

/* ov11:0x00a05fb0. Substitutes category 5 and prints D_00A0C0B8 through
 * dprintf whenever PoFlushChk returns nonzero; PoFlushChk's own nonzero
 * value is discarded rather than reused as the category. */
/* JP: フラッシュ\n
 * EN: Flush\n */
static const char D_00A0C0B8[16] = "\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk5(void)
{
    int category = PoFlushChk();

    if (category != 0) {
        dprintf(D_00A0C0B8);
        category = 5;
    }
    return category;
}

/* ov11:0x00a05fe8. Reports category 6, printing D_00A0C0C8 through
 * dprintf, when PoMultiChk's multiplicity equals 4. */
/* JP: フルハウス\n
 * EN: Full house\n */
static const char D_00A0C0C8[16] = "\xA5\xD5\xA5\xEB\xA5\xCF\xA5\xA6\xA5\xB9\n";

static int PoChk6(void)
{
    if (PoMultiChk() == 4) {
        dprintf(D_00A0C0C8);
        return 6;
    }
    return 0;
}

/* ov11:0x00a06028. Reports category 7, printing D_00A0C0D8 through
 * dprintf, when PoMultiChk's multiplicity equals 5. */
/* JP: フォーカード\n
 * EN: Four of a kind\n */
static const char D_00A0C0D8[16] = "\xA5\xD5\xA5\xA9\xA1\xBC\xA5\xAB\xA1\xBC\xA5\xC9\n";

static int PoChk7(void)
{
    if (PoMultiChk() == 5) {
        dprintf(D_00A0C0D8);
        return 7;
    }
    return 0;
}

/* JP: ストレートフラッシュ\n
 * EN: Straight flush\n */
static const char D_00A0C0E8[24] = "\xA5\xB9\xA5\xC8\xA5\xEC\xA1\xBC\xA5\xC8\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk8(void)
{
    struct PokerHandWork *hand;
    int first;

    if (PoFlushChk() == 0) {
        return 0;
    }
    hand = (struct PokerHandWork *)Gwork.words;
    first = hand->cards[0];
    if (hand->cards[1] != first + 1 ||
        hand->cards[2] != first + 2 ||
        hand->cards[3] != first + 3) {
        return 0;
    }
    if (hand->cards[4] == first + 4) {
        dprintf(D_00A0C0E8);
        return 8;
    }
    return 0;
}

/* ov11:0x00a060e8. Reports category 9 when PoFlushChk finds a flush and
 * the sorted hand's ranks, read from the Gwork work block, form a
 * consecutive run starting at the lowest card: base, base+9, base+0xA,
 * base+0xB, base+0xC. PoGetBase is a LOCAL asm sibling; the double
 * PoGetBase(PoGetBase(base)) round trip is the original's own check, not
 * a simplification. Any other outcome, including a flush that is not
 * this run, reports 0. */
#define POKER_HAND_RANK_INDEX 0xD1 /* short index; 0x1A2 / sizeof(short) */

/* JP: ロイヤルストレートフラッシュ\n
 * EN: Royal flush\n */
static const char D_00A0C100[32] = "\xA5\xED\xA5\xA4\xA5\xE4\xA5\xEB\xA5\xB9\xA5\xC8\xA5\xEC\xA1\xBC\xA5\xC8\xA5\xD5\xA5\xE9\xA5\xC3\xA5\xB7\xA5\xE5\n";

static int PoChk9(void)
{
    short *hand;
    short base;

    if (PoFlushChk() == 0) {
        return 0;
    }

    hand = (short *)Gwork.words;
    base = hand[POKER_HAND_RANK_INDEX];
    if (base == PoGetBase(PoGetBase(base))) {
        if (hand[POKER_HAND_RANK_INDEX + 1] == base + 9 &&
            hand[POKER_HAND_RANK_INDEX + 2] == base + 0xA &&
            hand[POKER_HAND_RANK_INDEX + 3] == base + 0xB &&
            hand[POKER_HAND_RANK_INDEX + 4] == base + 0xC) {
            dprintf(D_00A0C100);
            return 9;
        }
    }
    return 0;
}

/* ov11:0x00a06190. Sorts the hand, then checks hand-rank categories from
 * nine downward until one reports nonzero, plays one of three sound tiers
 * for the result, and reports it through the same debug printf as the
 * PoChk* family (D_00A0C120, "result = %d\n").
 *
 * The nested PoChk9..PoChk1 chain and the shared-tail-block `goto
 * defaultSound` are the shape recorded as CP-0254 in
 * config/compiler-patterns.json: a flat "if (category == 0) category =
 * PoChkN();" per-line chain lets cc1 2.96's range analysis fold the
 * repeated zero tests together with the sound-tier comparisons instead of
 * keeping each PoChkN call, and a fully structured if/else-if/else sound
 * dispatch duplicates the shared RES_SoundEffect(0x3A) call into two
 * physical blocks instead of the original's single call reached from both
 * the category<=0 and category>=0xA paths. */
static void PoSortCard(void);
static int PoChk4(void);
static int PoChk8(void);
static int PoChk9(void);
static void RES_SoundEffect(int soundId);
static const char D_00A0C120[] = "result = %d\n";

static int PoCardCheck(void)
{
    int category;

    PoSortCard();
    category = PoChk9();
    if (category == 0) {
        category = PoChk8();
        if (category == 0) {
            category = PoChk7();
            if (category == 0) {
                category = PoChk6();
                if (category == 0) {
                    category = PoChk5();
                    if (category == 0) {
                        category = PoChk4();
                        if (category == 0) {
                            category = PoChk3();
                            if (category == 0) {
                                category = PoChk2();
                                if (category == 0) {
                                    category = PoChk1();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (category > 0) {
        if (category >= 6) {
            if (category < 0xA) {
                RES_SoundEffect(0x39);
            } else {
                goto defaultSound;
            }
        } else {
            RES_SoundEffect(0x38);
        }
    } else {
defaultSound:
        RES_SoundEffect(0x3A);
    }

    dprintf(D_00A0C120, category);
    return category;
}

/* ov11:0x00a062b8. Draws the poker result lamp sprite for hand category
 * index category, selecting its fixed rect from a shared row of 0x10-byte
 * rects (the same tex/rect MakeSprite pair shape used throughout this TU;
 * the row's entry count beyond this stride is not separately evidenced). */

static void PoResultLamp(int category)
{
    SetDrawStatus(0, 0);
    MakeSprite(&tex_98, &result_rect_97[category]);
    SetDrawStatus(3, 0);
}

/* ov11:0x00a06308. Sets the poker sequence state (an enum PokerSeq
 * value). */
static void PoSeqChange(int sequenceState)
{
    Gwork.poker_deal.seq = sequenceState;
}

/* ov11:0x00a06318. Looks up the currently selected bet level's ready
 * message and message-box width from two fixed four-entry tables and
 * forwards both to the message window renderer. Gwork.modes.level is the
 * level index level_select stores (0..3); the four strings are the "Play N
 * coins" prompts for the four bet levels. The compiled form copies each
 * table onto the stack as a 16-byte aggregate before indexing it, which is
 * why each table is typed as a one-member array struct rather than a bare
 * array (a bare array cannot be copied by value in C). */
typedef struct {
    const char *text[4];
} PokerReadyMessageTable;

typedef struct {
    int width[4];
} PokerReadyWidthTable;

extern const char D_00A0C130[];
extern const char D_00A0C150[];
extern const char D_00A0C170[];
extern const char D_00A0C190[];
extern const PokerReadyMessageTable D_00A0C1B8; /* "Play 5/10/30/100 coins" */
extern const PokerReadyWidthTable D_00A0C1C8;

static void PokerReadyMes(void)
{
    PokerReadyMessageTable readyMessage = D_00A0C1B8;
    PokerReadyWidthTable readyWidth = D_00A0C1C8;

    PoWindowMessage(readyMessage.text[Gwork.modes.level], readyWidth.width[Gwork.modes.level]);
}

const PokerReadyMessageTable D_00A0C1B8 = {{
    D_00A0C130, D_00A0C150, D_00A0C170, D_00A0C190,
}};
const PokerReadyWidthTable D_00A0C1C8 = {{29, 30, 30, 31}};

extern int init_99;
extern int PokerCoinRate[4];
extern int PokerRate[10];
extern const char D_00A0C1D8[];
extern const char D_00A0C1F8[];
extern const char D_00A0C208[];

/* ov11:0x00a063a0. Poker main loop, one call per frame, switching on
 * Gwork.poker_deal.seq (enum PokerSeq; jump table jtbl_00A0C220):
 *   READY: if the coin balance is below the level's stake (PokerCoinRate)
 *      go to NO_COINS, else show the "Play N coins" dialog; yes goes to
 *      BET, anything else back to the casino menu. NO_COINS: "There are no
 *      more coins.", any button leaves.
 *   BET: move the stake from the balance into the pot, 1, 5 or 10 coins
 *      per frame. DEAL to SCORE: deal (PoCardDrawAnime), fill the hand
 *      (PoShuffleCard), choose cards (PoKey), PoCardFlip and PoShuffleCard
 *      again, then score the hand (PoCardCheck; a win multiplies the pot by
 *      PokerRate).
 *   CONTINUE: "Continue game?". WIN/DOUBLE_UP: "Double or nothing?"; yes
 *      starts the bonus game, no pays out.
 *   BONUS_*: the bonus game deals five cards one at a time. The first card
 *      is only revealed; each later one is compared with the card before it
 *      against the guess from BONUS_GUESS (a tie also wins). Each of the
 *      four comparisons doubles the pot when the guess is right, and after
 *      the fifth card the pot is paid out; a wrong guess clears the pot
 *      (BONUS_LOST waits for circle).
 *   PAYOUT: pay the pot out, a step per frame by level (1, 5, 10, 100) or
 *      all at once with circle.
 * Not sure what enters CONTINUE (nothing in OV11 does). coins is kept apart
 * from step: one shared variable allocates PAYOUT differently. */
static void PokerMain(void)
{
    int sel;
    int step;
    int coins;
    int i;
    int previous_rank;
    int current_rank;

    Pkt = xglPacketGetCurrent();
    PoBgDraw();
    xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    switch (Gwork.poker_deal.seq) {
    case POKER_SEQ_READY:
        Gwork.poker_deal.rates_hidden = 0;
        Gwork.poker_deal.in_round = 0;
        if (((struct CasinoSaveWork *)SaveWork)->coin_balance < PokerCoinRate[Gwork.poker_deal.level]) {
            PoSeqChange(POKER_SEQ_NO_COINS);
            break;
        }
        PokerReadyMes();
        sel = PoDialog(0);
        if (init_99 != 0) {
            PoCardDraw();
        }
        if (sel == 0) {
            break;
        }
        if (sel == 1) {
            init_99 = sel;
            PoSeqChange(POKER_SEQ_BET);
            Gwork.poker_deal.pot = 0;
            PoResetCard();
            break;
        }
        GameModeChange(CASINO_MENU);
        init_99 = 0;
        RES_SoundEffect(0x35);
        break;
    case POKER_SEQ_NO_COINS:
        if (init_99 != 0) {
            PoCardDraw();
            xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
            SetRegAD(Pkt, 0x3F, 0);
        }
        PoWindowMessage(D_00A0C1D8, 0x18);
        if (PadData.half_2a != 0) {
            GameModeChange(CASINO_MENU);
            init_99 = 0;
            RES_SoundEffect(0x35);
        }
        break;
    case POKER_SEQ_BET:
        if (Gwork.poker_deal.pot == PokerCoinRate[Gwork.poker_deal.level]) {
            Gwork.poker_deal.in_round = 1;
            PoSeqChange(POKER_SEQ_DEAL);
            break;
        }
        switch (Gwork.poker_deal.level) {
        case 2:
            step = 5;
            break;
        case 3:
            step = 10;
            break;
        default:
            step = 1;
            break;
        }
        ((struct CasinoSaveWork *)SaveWork)->coin_balance -= step;
        Gwork.poker_deal.pot += step;
        break;
    case POKER_SEQ_DEAL:
        Gwork.poker_deal.rates_hidden = 0;
        if (PoCardDrawAnime() != 0) {
            PoSeqChange(POKER_SEQ_DRAW_HAND);
        }
        break;
    case POKER_SEQ_DRAW_HAND:
        if (PoShuffleCard() == 1) {
            for (i = 0; i < 5; i++) {
                Gwork.poker_deal.replace_selected[i] = 1;
            }
            PoSeqChange(POKER_SEQ_SELECT);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_SELECT:
        PoCardDraw();
        PoSelectMarkDraw();
        if (PoKey() == 1) {
            PoSeqChange(POKER_SEQ_REPLACE);
            PoCardFlip();
        }
        break;
    case POKER_SEQ_REPLACE:
        if (PoShuffleCard() == 1) {
            PoSeqChange(POKER_SEQ_SCORE);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_SCORE:
        Gwork.poker_deal.hand_rank = PoCardCheck();
        if (Gwork.poker_deal.hand_rank > 0) {
            Gwork.poker_deal.pot *= PokerRate[Gwork.poker_deal.hand_rank];
            PoSeqChange(POKER_SEQ_WIN);
        } else {
            PoSeqChange(POKER_SEQ_READY);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_CONTINUE:
        Gwork.poker_deal.rates_hidden = 1;
        PoWindowMessage(D_00A0C1F8, 10);
        sel = PoDialog(0);
        if (sel != 0) {
            if (sel == 1) {
                PoSeqChange(POKER_SEQ_DEAL);
                PoResetCard();
                break;
            }
            GameModeChange(CASINO_MENU);
            init_99 = 0;
        }
        PoCardDraw();
        break;
    case POKER_SEQ_WIN:
        PoSeqChange(POKER_SEQ_DOUBLE_UP);
        /* fall through */
    case POKER_SEQ_DOUBLE_UP:
        Gwork.poker_deal.bonus_round = 0;
        Gwork.poker_deal.rates_hidden = 1;
        PoRateTableDraw();
        PoResultLamp(Gwork.poker_deal.hand_rank - 1);
        PoWindowMessage(D_00A0C208, 0x11);
        sel = PoDialog(0);
        if (sel != 0) {
            if (sel == 1) {
                PoSeqChange(POKER_SEQ_BONUS_START);
                break;
            }
            RES_SoundEffect(0x3B);
            PoSeqChange(POKER_SEQ_PAYOUT);
        }
        PoCardDraw();
        break;
    case POKER_SEQ_BONUS_START:
        PoResetCard();
        for (i = 0; i < 5; i++) {
            Gwork.poker_deal.cards.hand[i] = 0x36;
            Gwork.poker_deal.replace_selected[i] = 0;
        }
        PoSeqChange(POKER_SEQ_BONUS_DEAL);
        /* fall through */
    case POKER_SEQ_BONUS_DEAL:
        if (PoBonusCardAnime() == 1) {
            PoSeqChange(POKER_SEQ_BONUS_DRAW);
        }
        PoCardDraw();
        PoBonusBGDraw();
        break;
    case POKER_SEQ_BONUS_DRAW:
        if (PoShuffleCard() == 1) {
            PoSeqChange(POKER_SEQ_BONUS_JUDGE);
        }
        PoBonusBGDraw();
        PoCardDraw();
        break;
    case POKER_SEQ_BONUS_JUDGE:
        if (Gwork.poker_deal.bonus_round == 0) {
            Gwork.poker_deal.bonus_round = 1;
            PoSeqChange(POKER_SEQ_BONUS_GUESS);
        } else {
            previous_rank = Gwork.poker_deal.cards.hand[Gwork.poker_deal.bonus_round - 1];
            previous_rank -= PoGetBase(PoGetMark(previous_rank));
            current_rank = Gwork.poker_deal.cards.hand[Gwork.poker_deal.bonus_round];
            current_rank -= PoGetBase(PoGetMark(current_rank));
            if ((current_rank < previous_rank) == Gwork.poker_deal.guess
                || previous_rank == current_rank) {
                RES_SoundEffect(0x38);
                Gwork.poker_deal.bonus_round++;
                Gwork.poker_deal.pot *= 2;
                if (Gwork.poker_deal.bonus_round != 5) {
                    PoSeqChange(POKER_SEQ_BONUS_GUESS);
                } else {
                    PoSeqChange(POKER_SEQ_PAYOUT);
                }
            } else {
                Gwork.poker_deal.bonus_round = 0;
                Gwork.poker_deal.pot = 0;
                RES_SoundEffect(0x3A);
                PoSeqChange(POKER_SEQ_BONUS_LOST);
            }
        }
        PoBonusBGDraw();
        PoCardDraw();
        break;
    case POKER_SEQ_BONUS_LOST:
        PoBonusBGDraw();
        PoCardDraw();
        if (PadData.half_2a & PAD_CIRCLE) {
            PoSeqChange(POKER_SEQ_READY);
        }
        break;
    case POKER_SEQ_BONUS_GUESS:
        PoBonusBGDraw();
        PoCardDraw();
        sel = PoDialog(1);
        if (sel != 0) {
            if (sel == 3 || sel == -1) {
                PoSeqChange(POKER_SEQ_PAYOUT);
                RES_SoundEffect(0x3B);
                break;
            }
            if (sel == 1) {
                Gwork.poker_deal.guess = 0;
            } else {
                Gwork.poker_deal.guess = 1;
            }
            PoSeqChange(POKER_SEQ_BONUS_DEAL);
        }
        break;
    case POKER_SEQ_PAYOUT:
        if (Gwork.poker_deal.bonus_round != 0) {
            PoBonusBGDraw();
        } else {
            PoRateTableDraw();
            PoResultLamp(Gwork.poker_deal.hand_rank - 1);
        }
        PoCardDraw();
        if (PadData.half_2a & PAD_CIRCLE) {
            AddCoin(Gwork.poker_deal.pot);
            PoSeqChange(POKER_SEQ_READY);
            Gwork.poker_deal.in_round = 0;
            RES_SoundEffectStop(0x3B);
            break;
        }
        coins = 1;
        switch (Gwork.poker_deal.level) {
        case 1:
            coins = 1;
            break;
        case 2:
            coins = 5;
            break;
        case 3:
            coins = 10;
            break;
        case 4:
            coins = 100;
            break;
        }
        if (Gwork.poker_deal.pot >= coins) {
            Gwork.poker_deal.pot -= coins;
            AddCoin(coins);
        } else {
            AddCoin(Gwork.poker_deal.pot);
            Gwork.poker_deal.pot = 0;
        }
        if (Gwork.poker_deal.pot == 0) {
            Gwork.poker_deal.in_round = 0;
            RES_SoundEffectStop(0x3B);
            PoSeqChange(POKER_SEQ_READY);
        }
        break;
    }
    SetDrawStatus(3, 0);
    xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    if (Gwork.poker_deal.rates_hidden == 0) {
        PoRateTableDraw();
    }
    if (Gwork.poker_deal.in_round == 0) {
        PoMoneyPrint(0, ((struct CasinoSaveWork *)SaveWork)->coin_balance);
    } else {
        PoMoneyPrint(1, Gwork.poker_deal.pot);
    }
}

/* ov11:0x00a06c08. Resets five poker words before a session (its only
 * caller is GameModeChange): the sequence state, rates_hidden, bonus_mode,
 * bonus_round, and the word at 0x180, which nothing else in the TU reads
 * or writes. */
static void PoInitWork(void)
{
    Gwork.words[0x180 / 4] = 0; /* poker word 0x180, role not sure */
    Gwork.poker_deal.rates_hidden = 0;
    Gwork.poker_deal.bonus_mode = 0;
    Gwork.poker_deal.bonus_round = 0;
    Gwork.poker_deal.seq = POKER_SEQ_READY;
}

/* ov11:0x00a06c28. Loads the six poker minigame textures (four suits plus
 * two poker-table backgrounds; the "data\\tanaka\\*.xtx" paths are these
 * D_00A0Cxxx strings themselves) into fixed EE main RAM staging addresses,
 * following the same (void *)FIXED_ADDRESS pattern as src/main/game_over.c's
 * IMAGE_LOAD_BUFFER. */
static const char D_00A0C270[] = "data\\tanaka\\spade.xtx";
static const char D_00A0C288[] = "data\\tanaka\\clover.xtx";
static const char D_00A0C2A0[] = "data\\tanaka\\heart.xtx";
static const char D_00A0C2B8[] = "data\\tanaka\\dia.xtx";
static const char D_00A0C2D0[] = "data\\tanaka\\poker_1.xtx";
static const char D_00A0C2E8[] = "data\\tanaka\\poker_2.xtx";

static void PokerInit(void)
{
    xglCdReadFile(D_00A0C270, (void *)POKER_SPADE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C288, (void *)POKER_CLOVER_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2A0, (void *)POKER_HEART_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2B8, (void *)POKER_DIA_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2D0, (void *)POKER_TABLE1_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C2E8, (void *)POKER_TABLE2_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a06ce0. Switches the casino to another game and resets that
 * game's state. The menu clears the bet level; the nine-game runs
 * init_ninegame; poker runs PoInitWork. For the slot, unless it is coming
 * back from the nine-game it clears the per-line stakes and turns the help
 * screen on; it always clears the reel positions and states, the pointer,
 * state, total stake, the word at 0x40, double pay, the bonus flag and the
 * row. Finally it stores the new game in Gwork.modes.game. */
static void GameModeChange(int mode)
{
    int i;

    switch (mode) {
    case CASINO_MENU:
        Gwork.modes.level = 0;
        break;
    case CASINO_NINE_GAME:
        init_ninegame();
        break;
    case CASINO_SLOT:
        if (Gwork.modes.game != CASINO_NINE_GAME) {
            for (i = 0; i < 5; i++) {
                Gwork.slot.line_bet[i] = 0;
                Gwork.slot.line_pay[i] = 0;
            }
            Gwork.slot.help = 1;
        }
        for (i = 0; i < 3; i++) {
            Gwork.slot.reel_pos[i] = 0;
            Gwork.slot.reel_state[i] = 0;
        }
        Gwork.slot.pointer = 0;
        Gwork.slot.state = 0;
        Gwork.slot.total_bet = 0;
        Gwork.words[0x40 / 4] = 0; /* slot word 0x40, role not sure */
        Gwork.slot.double_pay = 0;
        Gwork.slot.bonus = 0;
        Gwork.slot.row = 0;
        break;
    case CASINO_POKER:
        PoInitWork();
        break;
    case CASINO_COIN_EXCHANGE:
    case CASINO_SHOP:
    case CASINO_VIEWER:
        break;
    }
    Gwork.modes.game = mode;
}

/* ov11:0x00a06df0. Draws the submenu window sprite as a fixed 0xB8 x 0x70
 * rectangle with its top-left corner at (x, y). windowId itself is not read
 * here; level_select and exchange_select forward it on to submenu_select
 * right after calling sub_window. */

static void sub_window(int windowId, int x, int y)
{
    struct SpriteRect rect;

    SetDrawStatus(1, 0);
    rect.x = x;
    rect.y = y;
    rect.w = 0xB8;
    rect.h = 0x70;
    MakeSprite(&SubWindow_Tex, &rect);
}

extern int anm_100;
extern struct SpriteTex Cursor_Tex[8];

/* ov11:0x00a06e50. Draws a submenu and handles its input: the window
 * (sub_window), one 0x48 x 0x10 icon per choice in a column at x + 0x34,
 * starting at y + 0xC and 0x18 apart, and an animated 0x10 x 0x10 cursor
 * (Cursor_Tex, 8 frames by anm_100) just left of the selected icon. Up and
 * down on the auto-repeat pad word move *selected with wraparound (sounds 5
 * and 6). Returns 1 when circle is pressed, else 0. */
static int submenu_select(int windowId, int x, int y, int iconCount,
                          const struct SpriteTex *icons, int *selected)
{
    struct SpriteRect rect;
    struct SpriteRect cursor;
    const struct SpriteTex *icon;
    int cursor_y;
    int i;

    sub_window(windowId, x, y);
    SetDrawStatus(1, 0);
    cursor_y = 0;
    rect.x = x + 0x34;
    rect.w = 0x48;
    rect.h = 0x10;
    icon = icons;
    for (i = 0; i < iconCount; i++) {
        rect.y = y + 0xC + i * 0x18;
        if (i == *selected) {
            cursor_y = rect.y;
        }
        MakeSprite(&icon[i], &rect);
    }
    cursor.x = rect.x - 0x10;
    cursor.y = cursor_y;
    cursor.h = 0x10;
    cursor.w = 0x10;
    MakeSprite(&Cursor_Tex[anm_100 % 8], &cursor);
    anm_100++;
    if (CASINO_PAD->wide.repeat & PAD_UP) {
        (*selected)--;
        RES_SoundEffect(5);
    }
    if (CASINO_PAD->wide.repeat & PAD_DOWN) {
        (*selected)++;
        RES_SoundEffect(6);
    }
    if (*selected > iconCount - 1) {
        *selected = 0;
    }
    if (*selected < 0) {
        *selected = iconCount - 1;
    }
    if (PadData.half_2a & PAD_CIRCLE) {
        return 1;
    }
    return 0;
}

/* ov11:0x00a07000. Draws a four-choice submenu bound to the shared game
 * level selection, with the window's top-left corner at (x, y). sub_window
 * does not read windowId, so it only reaches submenu_select. level_select's
 * own tail call (`j submenu_select`) is why its return value is
 * submenu_select's, and menu_mode branches on it (`beqz $2` right after
 * `jal level_select`). */
static int level_select(int windowId, int x, int y)
{
    sub_window(windowId, x, y);
    return submenu_select(windowId, x, y, 4, Level_Tex, &Gwork.modes.level);
}

/* ov11:0x00a07060. Draws a three-choice submenu bound to the shared exchange
 * mode selection, the same sub_window()+submenu_select() shape level_select
 * uses above; exchange_select's own tail call (`j submenu_select`) is why its
 * return value is submenu_select's. */
static int exchange_select(int windowId, int x, int y)
{
    sub_window(windowId, x, y);
    return submenu_select(windowId, x, y, 3, ExChange_Tex, &Gwork.modes.exchange_choice);
}

#define MENU_BG_TEX_BUFFER 0x0114A800
#define MENU_WINDOW_TEX_BUFFER 0x0118B000

extern struct SpriteTex EntryBG_Tex;
extern struct SpriteRect EntryBG_Rect;
extern struct SpriteRect Cursor_Rect[];
extern int curpos_101;
extern int cur_int_102;
extern int menu_mode_103;

/* ov11:0x00a070c0. Casino menu: draws the background, then in state 0 moves
 * the cursor over the four entries with the d-pad and picks one with
 * circle; state 2 shows the exchange submenu (entry 0), a bet level submenu
 * (entries 1 and 2) or leaves the menu (entry 3). Returns 1 on leaving. */
static int menu_mode(void)
{
    XglPacket **slot = &Pkt;

    *slot = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)MENU_BG_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    SetTest(0x5000D);
    MakeSprite(&EntryBG_Tex, &EntryBG_Rect);
    xglPacketTextureTrans((void *)MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    switch (menu_mode_103) {
    case 0:
        if (PadData.half_2a & PAD_UP) {
            curpos_101 = 0;
            RES_SoundEffect(0);
        }
        if (PadData.half_2a & PAD_LEFT) {
            curpos_101 = 1;
            RES_SoundEffect(1);
        }
        if (PadData.half_2a & PAD_RIGHT) {
            curpos_101 = 2;
            RES_SoundEffect(2);
        }
        if (PadData.half_2a & PAD_DOWN) {
            curpos_101 = 3;
            RES_SoundEffect(3);
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            menu_mode_103 = 2;
            RES_SoundEffect(4);
        }
        MakeSprite(&Cursor_Tex[cur_int_102 % 8], &Cursor_Rect[curpos_101]);
        break;
    case 1:
        break;
    case 2:
        switch (curpos_101) {
        case 0:
            if (exchange_select(Pkt, 151, 117)) {
                switch (Gwork.modes.exchange_choice) {
                case 0:
                    GameModeChange(CASINO_COIN_EXCHANGE);
                    RES_SoundEffect(9);
                    break;
                case 1:
                    GameModeChange(CASINO_SHOP);
                    RES_SoundEffect(10);
                    break;
                case 2:
                    GameModeChange(CASINO_VIEWER);
                    RES_SoundEffect(11);
                    break;
                }
            }
            break;
        case 1:
            if (level_select(Pkt, 33, 243)) {
                GameModeChange(CASINO_SLOT);
                RES_SoundEffect(12);
                menu_mode_103 = 0;
            }
            break;
        case 2:
            if (level_select(Pkt, 325, 243)) {
                GameModeChange(CASINO_POKER);
                RES_SoundEffect(13);
                menu_mode_103 = 0;
            }
            break;
        case 3:
            return 1;
        }
        if (PadData.half_2a & (PAD_LEFT | PAD_RIGHT | PAD_CROSS)) {
            RES_SoundEffect(8);
            menu_mode_103 = 0;
        }
        break;
    }
    cur_int_102++;
    return 0;
}

/* ov11:0x00a07410. Loads the menu background and window textures, then
 * clears the bet level (Gwork.modes.level) and the short at 0x10 (not
 * sure what that one holds). */
static const char D_00A0C320[] = "data\\tanaka\\BG.xtx";
static const char D_00A0C338[] = "data\\tanaka\\window.xtx";

#define MENU_LEVEL_INDEX 8 /* short index; 0x10 / sizeof(short) */

static void init_menu_mode(void)
{
    xglCdReadFile(D_00A0C320, (void *)MENU_BG_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C338, (void *)MENU_WINDOW_TEX_BUFFER, 0, 0);

    Gwork.modes.level = 0;
    ((short *)Gwork.words)[MENU_LEVEL_INDEX] = 0;
}

/* ov11:0x00a07470. Loads the casino test-mode background texture. */
extern const char D_00A0BE40[]; /* "data\\tanaka\\base.xtx" */

#define BASE_TEX_BUFFER            0x01089000
#define TEST_MODE_BASE_TEX_BUFFER  0x01593000

static void init_test_mode(void)
{
    xglCdReadFile(D_00A0BE40, (void *)TEST_MODE_BASE_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a074a0. Lazily loads the test-mode background texture on first
 * entry (flg_104 latches this), then selects it and draws the fixed
 * test-mode background sprite. xglPacketGetCurrent is main:0x0022c450
 * (src/main/xgl_packet.c). */
extern XglPacket *xglPacketGetCurrent(void);

static void test_mode(void)
{
    if (flg_104 == 0) {
        init_test_mode();
        flg_104 = 1;
    }
    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)TEST_MODE_BASE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&BGTEST_Tex, &BGTEST_Rect);
}

extern struct SpriteTex tex_105[11];

/* ov11:0x00a07528. Prints value like decprint with 14x24 digit sprites
 * from tex_105, stepping left by 14 per digit; a nonzero flag adds the
 * eleventh sprite of the table, the one past the digits, 16 pixels right
 * of x. */
static void DecPrint2(int value, int digits, int x, int y, int flag)
{
    struct SpriteRect rect;
    double number;
    unsigned long long power;
    int i;

    if (digits < 0) {
        digits = 0;
        while (1) {
            if (digits < 11) {
                number = (double)value;
                if (dpcmp(number, pow(DOUBLE_TEN, litodp(digits))) < 0) {
                    break;
                }
                digits++;
            } else {
                break;
            }
        }
    }
    rect.w = 14;
    rect.h = 24;
    if (digits > 0) {
        for (i = 0; i < digits; ) {
            rect.x = x - i * 14;
            rect.y = y;
            power = pow(DOUBLE_TEN, litodp(i++));
            /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
             * the pow result copy ahead of the next conversion argument. */
            do { } while (0);
            MakeSprite(&tex_105[dptoli(dpdiv((double)value, power)) % 10], &rect);
        }
    } else {
        rect.x = x;
        rect.y = y;
        MakeSprite(tex_105, &rect);
    }
    if (flag != 0) {
        rect.x = x + 16;
        rect.y = y;
        MakeSprite(&tex_105[10], &rect);
    }
}

/* Wave 14: new C order hwxy; unverified, no inline assembly. */
extern struct SpriteTex Tex_106[8];
extern int anm_107;
static void UtlCurPrint(int x, int y)
{
    struct SpriteRect rect;
    int n;
    xglPacketTextureTrans((void *)MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    rect.h = 24;
    rect.w = 24;
    rect.x = x;
    rect.y = y;
    n = anm_107;
    MakeSprite(&Tex_106[n % 8], &rect);
    anm_107 = anm_107 + 1;
}


#define COIN_EXCHANGE_TEX_BUFFER 0x011CB800

extern struct SpriteTex BG_Tex;
extern struct SpriteRect BG_Rect;
extern int cur_108;
extern const struct CoinCursorPos { short x; short y; } curpt_109[5];
extern int dataMoneyBoxChk(void);
extern int dataMoneyBoxDec(int amount);
static void DecPrint2(int value, int digits, int x, int y, int flag);
extern const char D_00A0C350[];
extern const char D_00A0C358[];
extern const char D_00A0C368[];
extern const char D_00A0C370[];
extern const char D_00A0C378[];
extern const char D_00A0C380[];
extern const char D_00A0C388[];

/* ov11:0x00a077a0. Coin-exchange screen, one frame per call. Draws the
 * background, the price list (RES_Coin_Info) and labels, moves the cursor
 * (cur_108, five rows) with up/down on the auto-repeat word, and when
 * circle is pressed or R1 repeats (one doubleword test) it trades one money
 * box for 10, 100, 500 or 1000 coins on rows 0-3. If the balance is already
 * 9999999 or more, the box is put back. Circle on the last row, or cross,
 * leaves to the menu. Then it draws the cursor and the money-box and coin
 * totals. */
static void coin_main(void)
{
    int leave;

    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)COIN_EXCHANGE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&BG_Tex, &BG_Rect);
    xglFontPrint(0, 0, 0xFF00, D_00A0C350);
    xglFontPrint(0xD2, 0x20, 0xFF00, D_00A0C358);
    xglFontPrint(0, 0, 0xFF00, D_00A0C368);
    xglFontPrint(0, 0, 0xFF00, D_00A0C370);
    xglFontPrint(0x104, 0x40, 0xFF00, RES_Coin_Info(0));
    xglFontPrint(0x104, 0x58, 0xFF00, RES_Coin_Info(1));
    xglFontPrint(0x104, 0x70, 0xFF00, RES_Coin_Info(2));
    xglFontPrint(0x104, 0x88, 0xFF00, RES_Coin_Info(3));
    xglFontPrint(0, 0, 0, D_00A0C378);
    xglFontPrint(0x19A, 0xD0, 0xFF00, D_00A0C380);
    xglFontPrint(0, 0, 0xFF00, D_00A0C388);
    if (CASINO_PAD->wide.repeat & PAD_UP) {
        RES_SoundEffect(0xE);
        cur_108--;
    }
    if (CASINO_PAD->wide.repeat & PAD_DOWN) {
        RES_SoundEffect(0xF);
        cur_108++;
    }
    if (cur_108 < 0) {
        cur_108 = 4;
    }
    if (cur_108 >= 5) {
        cur_108 = 0;
    }
    leave = 0;
    if (PadData.half_2a & PAD_CROSS) {
        leave = 1;
    }
    if (CASINO_PAD->dword.words & (PAD_WORDS_PRESSED(PAD_CIRCLE) | PAD_WORDS_REPEAT(PAD_R1))) {
        switch (cur_108) {
        case 0:
            if (dataMoneyBoxDec(RES_Coin_Rate(0)) == 1) {
                if (((struct CasinoSaveWork *)SaveWork)->coin_balance < 9999999) {
                    AddCoin(10);
                    RES_SoundEffect(0x10);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(0));
            }
            RES_SoundEffect(0x11);
            break;
        case 1:
            if (dataMoneyBoxDec(RES_Coin_Rate(1)) == 1) {
                if (((struct CasinoSaveWork *)SaveWork)->coin_balance < 9999999) {
                    AddCoin(100);
                    RES_SoundEffect(0x10);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(1));
            }
            RES_SoundEffect(0x11);
            break;
        case 2:
            if (dataMoneyBoxDec(RES_Coin_Rate(2)) == 1) {
                if (((struct CasinoSaveWork *)SaveWork)->coin_balance < 9999999) {
                    AddCoin(500);
                    RES_SoundEffect(0x10);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(2));
            }
            RES_SoundEffect(0x11);
            break;
        case 3:
            if (dataMoneyBoxDec(RES_Coin_Rate(3)) == 1) {
                if (((struct CasinoSaveWork *)SaveWork)->coin_balance < 9999999) {
                    AddCoin(1000);
                    RES_SoundEffect(0x10);
                    break;
                }
                dataMoneyBoxInc(RES_Coin_Rate(3));
            }
            RES_SoundEffect(0x11);
            break;
        default:
            if (PadData.half_2a & PAD_CIRCLE) {
                leave = 1;
            }
            break;
        }
    }
    UtlCurPrint(curpt_109[cur_108].x, curpt_109[cur_108].y);
    xglPacketTextureTrans((void *)MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(6, 0x60);
    DecPrint2(dataMoneyBoxChk(), -1, 0xE6, 0x104, 0);
    DecPrint2(((struct CasinoSaveWork *)SaveWork)->coin_balance, -1, 0x1D4, 0x104, 0);
    if (leave == 1) {
        cur_108 = 0;
        GameModeChange(CASINO_MENU);
        RES_SoundEffect(0x12);
    }
}

/* ov11:0x00a07c38. Loads the casino coin-exchange minigame's background and
 * exchange textures (D_00A0BE40 is the same "base.xtx" path init_test_mode
 * uses, into a different fixed buffer address here). */
static const char D_00A0C390[] = "data\\tanaka\\exchange2.xtx";


static void init_coin(void)
{
    xglCdReadFile(D_00A0BE40, (void *)BASE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C390, (void *)COIN_EXCHANGE_TEX_BUFFER, 0, 0);
}

/* ov11:0x00a07c80. Loads the casino shop's exchange and background textures
 * (D_00A0BE40 "base.xtx" again, into the same fixed buffer init_coin uses). */
static const char D_00A0C3B0[] = "data\\tanaka\\exchange1.xtx";

#define SHOP_EXCHANGE_TEX_BUFFER 0x0120C000

static void init_shop(void)
{
    xglCdReadFile(D_00A0C3B0, (void *)SHOP_EXCHANGE_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0BE40, (void *)BASE_TEX_BUFFER, 0, 0);
}

extern int shpmenu_work[51];
extern int MenuNum;
extern int xglFlagsGet1(int flag);

static void init_shpmenu(void)
{
    int *entry;
    int i;
    int enabled;

    entry = shpmenu_work;
    MenuNum = 0;
    for (i = 0; i < 51; i++) {
        enabled = 0;
        switch (i) {
        case 0:
        case 1:
            enabled = 1;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
            if (SaveWork[i] == 0) {
                enabled = 1;
            }
            break;
        case 36:
        case 37:
            if (xglFlagsGet1(115) == 1) {
                enabled = 1;
            }
            break;
        case 38:
            if (xglFlagsGet1(301) == 1) {
                enabled = 1;
            }
            break;
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
            if (xglFlagsGet1(389) == 1 && SaveWork[i] == 0) {
                enabled = 1;
            }
            break;
        case 50:
            if (xglFlagsGet1(391) == 1 && SaveWork[i] == 0) {
                enabled = 1;
            }
            break;
        }
        if (enabled == 1) {
            *entry = i;
            MenuNum++;
            entry = shpmenu_work + MenuNum;
        }
    }
}

extern int init_110;
extern int cur_111;
extern int top_112;
extern int MenuNum;
extern int shpmenu_work[51];
extern const char D_00A0C4A0[];
extern const char D_00A0C4A8[];
extern const char D_00A0C4B0[];
extern int dataItmBoxChk(int id);
extern int dataItmBoxInc(int id);
extern int dataAccBoxInc(int id);
extern int dataEvtBoxInc(int id);
static void init_shpmenu(void);

#define SHOP_MENU_TEX_BUFFER 0x0118B000
#define SHOP_PRIZE_COUNT 8

static void shop_main(void)
{
    int prize;
    int box_id;
    int item;
    int shown;
    int i;
    int *init;

    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)SHOP_EXCHANGE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    {
        float current = *(float *)&Pkt;
        *(float *)&Pkt = current;
    }
    init = &init_110;
    SetDrawStatus(3, 0);
    {
        int done = *init;
        *init = done;
    }
    MakeSprite(&BG_Tex, &BG_Rect);
    xglPacketTextureTrans((void *)SHOP_MENU_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    DecPrint2((*(int *)(SaveWork + 0x50)), -1, 468, 260, 0);
    if (*init == 0) {
        init_shpmenu();
        *init = 1;
    }
    shown = 0;
    i = 0;
    if (i < MenuNum) {
        do {
            if (i >= top_112 && shown != SHOP_PRIZE_COUNT) {
                item = shpmenu_work[i];
                xglFontPrint(0, 0, 0, D_00A0C4A0);
                xglFontPrint(0, 0, 0, D_00A0C4A8);
                xglFontPrint(240, shown * 24 + 32, 0xFF00, RES_ShopDataName(item));
                shown++;
            }
            i++;
        } while (i < MenuNum);
    }
    SetDrawStatus(3, 0);
    UtlCurPrint(216, cur_111 * 24 + 34);
    if (CASINO_PAD->wide.repeat & PAD_UP) {
        RES_SoundEffect(19);
        if (cur_111 == 0) {
            top_112--;
        } else {
            cur_111--;
        }
    }
    if (CASINO_PAD->wide.repeat & PAD_DOWN) {
        RES_SoundEffect(20);
        if (cur_111 + top_112 < MenuNum - 1 && cur_111 == SHOP_PRIZE_COUNT - 1) {
            top_112++;
        } else {
            cur_111++;
        }
    }
    if (top_112 + cur_111 >= MenuNum) {
        cur_111 = MenuNum - top_112 - 1;
    }
    if (top_112 < 0) {
        top_112 = 0;
    }
    if (MenuNum < cur_111) {
        cur_111 = MenuNum - 1;
    }
    prize = shpmenu_work[top_112 + cur_111];
    SetDrawStatus(0, 0);
    DecPrint2(RES_ShopDataCoin(prize), -1, 128, 260, 1);
    xglFontPrint(0, 0, 0xFF00, D_00A0C4A0);
    xglFontPrint(0, 0, 0, D_00A0C4B0);
    xglFontPrint(32, 310, 0xFF00, RES_ShopDataInfo(prize));
    if (CASINO_PAD->dword.words & PAD_WORDS_REPEAT(PAD_CIRCLE | PAD_R1)) {
        if (RES_ShopDataCoin(prize) <= (*(int *)(SaveWork + 0x50))) {
        (*(int *)(SaveWork + 0x50)) -= RES_ShopDataCoin(prize);
        if (prize >= 2 && prize <= 50) {
            SaveWork[prize] = 1;
            init_110 = 0;
            if (shown == 1) {
                top_112--;
            }
        }
        switch (prize) {
        case 0:
            if (dataItmBoxChk(1) == 99 && dataItmBoxChk(5) == 99 && dataItmBoxChk(10) == 99 && dataItmBoxChk(12) == 99) {
                RES_SoundEffect(22);
                (*(int *)(SaveWork + 0x50)) += RES_ShopDataCoin(prize);
            } else {
                RES_SoundEffect(21);
                dataItmBoxInc(1);
                dataItmBoxInc(5);
                dataItmBoxInc(10);
                dataItmBoxInc(12);
            }
            break;
        case 1:
            if (dataItmBoxChk(16) == 99 && dataItmBoxChk(36) == 99) {
                RES_SoundEffect(22);
                (*(int *)(SaveWork + 0x50)) += RES_ShopDataCoin(prize);
            } else {
                dataItmBoxInc(16);
                dataItmBoxInc(36);
                RES_SoundEffect(21);
            }
            break;
        case 2:
            if (dataAccBoxInc(100)) {
                RES_SoundEffect(21);
            } else {
                RES_SoundEffect(22);
                (*(int *)(SaveWork + 0x50)) += RES_ShopDataCoin(prize);
            }
            break;
        case 3:
            if (dataAccBoxInc(109)) {
                RES_SoundEffect(21);
            } else {
                RES_SoundEffect(22);
                (*(int *)(SaveWork + 0x50)) += RES_ShopDataCoin(prize);
            }
            break;
        case 4:
            if (dataItmBoxInc(11)) {
                RES_SoundEffect(21);
            } else {
                RES_SoundEffect(22);
                (*(int *)(SaveWork + 0x50)) += RES_ShopDataCoin(prize);
            }
            break;
        case 5:
            if (dataItmBoxInc(18)) {
                RES_SoundEffect(21);
            } else {
                RES_SoundEffect(22);
                (*(int *)(SaveWork + 0x50)) += RES_ShopDataCoin(prize);
            }
            break;
        case 36:
        case 37:
        case 38:
            box_id = prize + 17;
            goto event;
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
            box_id = prize + 2;
        event:
            if (dataEvtBoxInc(box_id)) {
                RES_SoundEffect(21);
            } else {
                RES_SoundEffect(22);
                (*(int *)(SaveWork + 0x50)) += RES_ShopDataCoin(prize);
            }
            break;
        default:
            RES_SoundEffect(21);
            break;
        }
        } else {
            RES_SoundEffect(22);
        }
    }
    if (PadData.half_2a & PAD_CROSS) {
        top_112 = 0;
        cur_111 = 0;
        init_110 = 0;
        GameModeChange(CASINO_MENU);
        RES_SoundEffect(23);
    }
}

/* ov11:0x00a083f8. Loads the data-viewer minigame's dataviewer and sampler
 * textures. */
static const char D_00A0C590[] = "data\\tanaka\\dataviewer.xtx";
static const char D_00A0C5B0[] = "data\\tanaka\\sam.xtx";

#define VW_DATAVIEWER_TEX_BUFFER 0x0124C800
#define VW_SAM_TEX_BUFFER        0x01410000

static void VW_Init(void)
{
    xglCdReadFile(D_00A0C590, (void *)VW_DATAVIEWER_TEX_BUFFER, 0, 0);
    xglCdReadFile(D_00A0C5B0, (void *)VW_SAM_TEX_BUFFER, 0, 0);
}

/* Build the compact list of the 30 sampler slots enabled in SaveWork. */
static __inline void sampler_offset(int **base, int *count)
{
    *base += 114;
    *count = 0;
}

static int VW_SamlistInit(void)
{
    int count;
    int index;
    char *enabled;
    int *sampler;

    sampler = Gwork.words;
    sampler_offset(&sampler, &count);
    enabled = SaveWork;
    enabled += 6;
    index = 0;

    for (index = 0; index < 30; index++) {
        if (*enabled == 1) {
            *sampler = index;
            sampler++;
            count++;
        }
        enabled++;
    }
    return count;
}

extern void xglSoundSendSmd(void *smd);
extern int xglSoundSendSwd(void *swd, int bank);
extern void xglSoundSequenceNormal(int volume);
extern int SsdSpuDmaCompleted(int wait);
extern int sprintf(char *buffer, const char *format, ...);
extern const char D_00A0C5C8[];
extern const char D_00A0C5D8[];
extern const char D_00A0C5E8[];
extern const char D_00A0C5F8[];
extern const char D_00A0C608[];
extern const char D_00A0C618[];
extern const char D_00A0C628[];
extern char fileinfo[30];
extern int old_113;

/* Viewer image pages: VW_ViewModeInit loads NN_Y1/Y2.xtx into the first
 * two and NN_W1..W4.xtx into all four. */
#define VW_IMAGE_1_BUFFER 0x01491000
#define VW_IMAGE_2_BUFFER 0x014D1800
#define VW_IMAGE_3_BUFFER 0x01512000
#define VW_IMAGE_4_BUFFER 0x01552800

/* ov11:0x00a08498. Viewer: when the selected entry changes, builds its
 * file names (data\\tanaka\\NN_Y1/Y2.xtx for type 0, NN_W1..W4.xtx
 * otherwise, with the type taken from fileinfo) and loads them into fixed
 * buffers, then resets the scroll offsets (Gwork.viewer). Returns the entry's type.
 * Not sure what the Y and W image sets are. */
static int VW_ViewModeInit(int index)
{
    char path[256];
    int info;

    info = fileinfo[index];
    if (index != old_113) {
        if (info == 0) {
            sprintf(path, D_00A0C5C8, D_00A0C5D8, index + 1);
            xglCdReadFile(path, (void *)VW_IMAGE_1_BUFFER, 0, 0);
            sprintf(path, D_00A0C5E8, D_00A0C5D8, index + 1);
            xglCdReadFile(path, (void *)VW_IMAGE_2_BUFFER, 0, 0);
        } else {
            sprintf(path, D_00A0C5F8, D_00A0C5D8, index + 1);
            xglCdReadFile(path, (void *)VW_IMAGE_1_BUFFER, 0, 0);
            sprintf(path, D_00A0C608, D_00A0C5D8, index + 1);
            xglCdReadFile(path, (void *)VW_IMAGE_2_BUFFER, 0, 0);
            sprintf(path, D_00A0C618, D_00A0C5D8, index + 1);
            xglCdReadFile(path, (void *)VW_IMAGE_3_BUFFER, 0, 0);
            sprintf(path, D_00A0C628, D_00A0C5D8, index + 1);
            xglCdReadFile(path, (void *)VW_IMAGE_4_BUFFER, 0, 0);
        }
        Gwork.viewer.scroll_y = 0;
        Gwork.viewer.scroll_x = 0;
    }
    old_113 = index;
    return info;
}

extern struct SpriteTex tex_114;
extern struct SpriteTex keytex_115;
extern struct SpriteRect keyrect_116;

/* ov11:0x00a08650. Viewer image display: loads the entry's images with
 * VW_ViewModeInit, draws them as a 2x2 grid of 578-wide pages at the scroll
 * offsets (the second row is 448 high, one page when the type is not 1 is
 * shown only as the first two), scrolls with the larger of the two stick
 * axes (the x offset only when the entry type is 1), clamps the offsets,
 * then draws the key sprite. Returns 1 when cross is newly pressed (sound
 * 0x1C), else 0. Not sure which stick bytes are which axis. */
static int VW_ViewMode(int arg)
{
    struct SpriteRect rect[4];
    int type;
    int dx;
    signed char dy;
    int ax;
    int ay;
    int bx;
    int by;
    signed char abs_ax;
    signed char abs_ay;
    signed char abs_bx;
    signed char abs_by;
    signed char by_char;

    type = VW_ViewModeInit(arg);
    rect[3].w = 578;
    rect[2].w = 578;
    rect[1].w = 578;
    rect[0].w = 578;
    rect[2].h = 512;
    rect[3].h = 448;
    rect[0].h = 512;
    rect[1].h = 448;
    rect[0].x = Gwork.viewer.scroll_x;
    rect[1].x = Gwork.viewer.scroll_x;
    rect[2].y = Gwork.viewer.scroll_y;
    rect[0].y = Gwork.viewer.scroll_y;
    rect[3].y = Gwork.viewer.scroll_y + 512;
    rect[1].y = Gwork.viewer.scroll_y + 512;
    rect[2].x = Gwork.viewer.scroll_x + 578;
    rect[3].x = Gwork.viewer.scroll_x + 578;
    SetDrawStatus(3, 0);
    xglPacketTextureTrans((void *)VW_IMAGE_1_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    MakeSprite(&tex_114, &rect[0]);
    xglPacketTextureTrans((void *)VW_IMAGE_2_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    MakeSprite(&tex_114, &rect[1]);
    if (type == 1) {
        xglPacketTextureTrans((void *)VW_IMAGE_3_BUFFER);
        SetRegAD(Pkt, 0x3F, 0);
        MakeSprite(&tex_114, &rect[2]);
        xglPacketTextureTrans((void *)VW_IMAGE_4_BUFFER);
        SetRegAD(Pkt, 0x3F, 0);
        MakeSprite(&tex_114, &rect[3]);
    }
    ax = CASINO_PAD->wide.stick[0] / 2;
    abs_ax = ax;
    if (ax < 0) {
        abs_ax = -ax;
    }
    ay = CASINO_PAD->wide.stick[1] / 2;
    abs_ay = ay;
    if (abs_ay < 0) {
        abs_ay = -abs_ay;
    }
    bx = CASINO_PAD->wide.stick[2] / 2;
    abs_bx = bx;
    if (bx < 0) {
        abs_bx = -bx;
    }
    by = CASINO_PAD->wide.stick[3] / 2;
    by_char = by;
    if (by_char < 0) {
        abs_by = -by_char;
    } else {
        abs_by = by_char;
    }
    dy = by;
    if (abs_ay >= abs_by) {
        dy = ay;
    }
    Gwork.viewer.scroll_y -= dy;
    dx = ax;
    if (abs_ax < abs_bx) {
        dx = bx;
    }
    if (type == 1) {
        Gwork.viewer.scroll_x -= dx;
    }
    if (Gwork.viewer.scroll_x < -578) {
        Gwork.viewer.scroll_x = -578;
    }
    if (Gwork.viewer.scroll_x > 0) {
        Gwork.viewer.scroll_x = 0;
    }
    if (Gwork.viewer.scroll_y < -592) {
        Gwork.viewer.scroll_y = -592;
    }
    if (Gwork.viewer.scroll_y > 0) {
        Gwork.viewer.scroll_y = 0;
    }
    xglPacketTextureTrans((void *)VW_DATAVIEWER_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    MakeSprite(&keytex_115, &keyrect_116);
    if (PadData.half_2a & PAD_CROSS) {
        RES_SoundEffect(0x1C);
        return 1;
    }
    return 0;
}

extern const char *Cajino_GetTitle(int sample);
extern const char *Cajino_GetInfo(int sample);
extern struct SpriteTex back_tex_118;
extern struct SpriteTex ViewSamTex[30];
extern struct SpriteRect sam_rect_117;
extern int cur_119;
extern int ofs_120;
extern int init_121;
extern int D_00A0DD14;
extern const char D_00A0C4A0[];
extern const char D_00A0C4B0[];
extern const char D_00A0C638[];
extern const char D_00A0C640[];
extern const char D_00A0C650[];

/* ov11:0x00a08960. Viewer sample list. With arg 0 it resets the cursor and
 * scroll offset and returns 0. Otherwise it builds the list of enabled
 * samples once (VW_SamlistInit), draws the background and up to eight
 * titles from the scroll offset, the cursor, and moves the cursor or
 * scroll offset with up/down on the auto-repeat word (sounds 0x18 and
 * 0x19). If the list is not
 * empty it prints the selected sample's title and info and draws its
 * picture, returning the sample id; otherwise it returns -1. Not sure what
 * the font control strings select. */
static int VW_SamMode(int arg)
{
    int i;
    int y;
    int sample;

    if (arg == 0) {
        cur_119 = ofs_120 = init_121 = 0;
        return 0;
    }
    if (init_121 == 0) {
        init_121 = 1;
        D_00A0DD14 = VW_SamlistInit();
    }
    xglPacketTextureTrans((void *)VW_DATAVIEWER_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(&back_tex_118, &BG_Rect);
    y = 0x14;
    for (i = 0; i < D_00A0DD14; i++) {
        if (i >= ofs_120 && i <= ofs_120 + 7) {
            xglFontPrint(0, 0, 0, D_00A0C4A0);
            xglFontPrint(0, 0, 0, D_00A0C4B0);
            xglFontPrint(0x154, y * 2, 0xFF00, Cajino_GetTitle(Gwork.viewer.sample[i]));
            y += 0xC;
        }
    }
    if (D_00A0DD14 > 0) {
        xglFontPrint(0x140, cur_119 * 24 + 0x2E, 0xFF00, D_00A0C638);
    }
    if (CASINO_PAD->wide.repeat & PAD_UP) {
        RES_SoundEffect(0x18);
        if (cur_119 != 0) {
            cur_119--;
        } else if (ofs_120 > 0) {
            ofs_120--;
        }
    }
    if (CASINO_PAD->wide.repeat & PAD_DOWN) {
        RES_SoundEffect(0x19);
        if (cur_119 == 7) {
            if (ofs_120 + cur_119 < D_00A0DD14 - 1) {
                ofs_120++;
            }
        } else if (cur_119 < D_00A0DD14 - 1) {
            cur_119++;
        }
    }
    sample = -1;
    if (D_00A0DD14 > 0) {
        sample = Gwork.viewer.sample[ofs_120 + cur_119];
        xglFontPrint(0, 0, 0, D_00A0C4A0);
        xglFontPrint(0, 0, 0, D_00A0C640);
        xglFontPrint(0x1C, 0x100, 0xFF00, Cajino_GetTitle(sample));
        xglFontPrint(0, 0, 0, D_00A0C650);
        xglFontPrint(0x46, 0x118, 0xFF00, Cajino_GetInfo(sample));
        xglPacketTextureTrans((void *)VW_SAM_TEX_BUFFER);
        SetRegAD(Pkt, 0x3F, 0);
        MakeSprite(&ViewSamTex[sample], &sam_rect_117);
    }
    return sample;
}

extern int mode_123;
extern int D_00A0DD18;

/* ov11:0x00a08ce0. View mode main: switches between VW_ViewMode and
 * VW_SamMode on mode_123; triangle enters the sample list and cross leaves
 * it (calls GameModeChange(CASINO_MENU)). */
static void VW_Main(void)
{
    int result;
    XglPacket **slot = &Pkt;

    *slot = xglPacketGetCurrent();
    if (mode_123 == 1) {
        if (VW_ViewMode(D_00A0DD18)) {
            mode_123 = 0;
        }
        return;
    }
    result = VW_SamMode(1);
    D_00A0DD18 = result;
    if (result != -1) {
        if (PadData.half_2a & PAD_TRIANGLE) {
            RES_SoundEffect(0x1A);
            mode_123 = 1;
        }
    }
    if (PadData.half_2a & PAD_CROSS) {
        VW_SamMode(0);
        GameModeChange(CASINO_MENU);
        RES_SoundEffect(0x1B);
    }
}

extern int MUSICLIST_125[];
extern int oldreq_126;
extern const char D_00A0C660[];
extern const char D_00A0C680[];
extern const char D_00A0C6A0[];
extern const char D_00A0C6B0[];
extern const char D_00A0C6C0[];

/* ov11:0x00a08dc0. Starts a background music request. -1 and a repeat of
 * the previous request (oldreq_126) do nothing. Otherwise it clears the
 * two sound banks, loads the shared sound data, waits for the SPU DMA,
 * then loads the sequence (.smd) and wave (.swd) files for the requested
 * track from MUSICLIST_125 and starts the normal sequence at volume 0x7F.
 * Note the function never updates oldreq_126 itself. Not sure of the exact
 * sound API semantics. */
static void MUSIC_CALL(int musicId)
{
    char path[256];

    if (musicId == -1) {
        return;
    }
    if (oldreq_126 == musicId) {
        return;
    }
    xglSoundSendEffect(0, 0, 1);
    xglSoundSendEffect(0, 0, 2);
    xglCdReadFile(D_00A0C660, (void *)0x017A7000, 0, 0);
    xglCdReadFile(D_00A0C680, (void *)0x017B7000, 0, 0);
    xglSoundSendEffect((void *)0x017B7000, (void *)0x017A7000, 1);
    while (SsdSpuDmaCompleted(0) != 0) {
    }
    xglSoundSendEffect((void *)0x017B7000, (void *)0x017A7000, 2);
    sprintf(path, D_00A0C6A0, D_00A0C6B0, MUSICLIST_125[musicId]);
    xglCdReadFile(path, (void *)0x015A3000, 0, 0);
    xglSoundSendSmd((void *)0x015A3000);
    sprintf(path, D_00A0C6C0, D_00A0C6B0, MUSICLIST_125[musicId]);
    xglCdReadFile(path, (void *)0x017A3000, 0, 0);
    xglSoundSendSwd((void *)0x017A3000, -1);
    while (SsdSpuDmaCompleted(0) != 0) {
    }
    xglSoundSequenceNormal(0x7F);
}

/* xglFlagsSet1 is main:0x00222938 (src/main/xgl_flags.c, external linkage
 * there). dataMoneyBoxInc is main:0x002BCE40, still INCLUDE_ASM there
 * (src/main/data.c); src/main/runtime.c already forwards to it with this
 * same signature. AddCoin (ov11:0x00a00b28) and MUSIC_CALL (ov11:0x00a08dc0)
 * are LOCAL asm siblings of this TU. */
extern int xglFlagsSet1(int bit_offset, int value);
extern int dataMoneyBoxInc(int amount);
static int AddCoin(int amount);
static void MUSIC_CALL(int musicId);

/* ov11:0x00a08f40. Resets the four leading Gwork words and the six Gwork
 * shorts at 0x240-0x24A, loads the casino resource table and starts the
 * background sound queue, applies four debug-mode config-flag unlocks and
 * grants a near-maximum bank balance plus a matching coin total when
 * RES_IsDebugMode() is set, then seeds six more Gwork shorts starting at
 * 0x54 (a leading sentinel of 5 followed by a descending fill of the next
 * five with 4..0). Field identities beyond their byte offsets are not
 * separately evidenced. Its only caller, MiniG_Init, runs this once. */
/* The original materializes Gwork's address once (%lo(Gwork)=0, so
 * `addiu s0,v0,0`) and reaches every field below off that same register
 * with an immediate byte offset; indexing Gwork as a word array and a
 * short array through separate expressions instead recomputes the base at
 * Gwork+0x240, which does not reproduce this. */
#define WORK_RESET_WORD_0 0x0
#define WORK_RESET_WORD_1 0x4
#define WORK_RESET_WORD_2 0x8
#define WORK_RESET_WORD_3 0xC
#define WORK_MODE_SHORT_0 0x240
#define WORK_MODE_SHORT_1 0x242
#define WORK_MODE_SHORT_2 0x244
#define WORK_MODE_SHORT_3 0x246
#define WORK_MODE_SHORT_4 0x248
#define WORK_MODE_SHORT_5 0x24A
#define WORK_STAKE_SENTINEL 0x54
#define WORK_STAKE_TABLE_END 0x5E

static void InitWork(void)
{
    unsigned char *work = (unsigned char *)Gwork.words;
    short *slot;
    int value;

    *(short *)(work + WORK_MODE_SHORT_0) = 0;
    *(short *)(work + WORK_MODE_SHORT_1) = 0;
    *(short *)(work + WORK_MODE_SHORT_2) = 0;
    *(short *)(work + WORK_MODE_SHORT_3) = 0;
    *(short *)(work + WORK_MODE_SHORT_4) = 0;
    *(short *)(work + WORK_MODE_SHORT_5) = 0;
    *(int *)(work + WORK_RESET_WORD_0) = 0;
    *(int *)(work + WORK_RESET_WORD_1) = 0;
    *(int *)(work + WORK_RESET_WORD_2) = 0;
    *(int *)(work + WORK_RESET_WORD_3) = 0;

    RES_Load();
    MUSIC_CALL(0);

    if (RES_IsDebugMode() != 0) {
        xglFlagsSet1(0x73, 1);
        xglFlagsSet1(0x12D, 1);
        xglFlagsSet1(0x185, 1);
        xglFlagsSet1(0x187, 1);
        dataMoneyBoxInc(0x3B9AC9FF);
        AddCoin(0x989298);
    }

    *(short *)(work + WORK_STAKE_SENTINEL) = 5;
    slot = (short *)(work + WORK_STAKE_TABLE_END);
    value = 4;
    do {
        *slot = value;
        value--;
        slot--;
    } while (value >= 0);
}

/* ov11:0x00a09020. Logs the configured heap address, then delegates all
 * casino state initialization to InitWork (LOCAL asm sibling). */
static void InitWork(void);
static const char D_00A0C6D0[] = "work size = %x\n";

void MiniG_Init(void)
{
    dprintf(D_00A0C6D0, 0x7CF000);
    InitWork();
}

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern const char D_00A0C6E0[];
extern const char D_00A0C6F0[];

/* ov11:0x00a09048. Casino main dispatcher. Gwork.modes.game selects the game:
 * 0 menu (returns 1 when the menu handler asks to leave), 1 slot, 2 poker,
 * 3 coin exchange, 4 shop, 5 viewer, 6 nine-game, 7 test mode. Each game's
 * init routine runs once, guarded by a per-game flag. After a game runs it
 * prints a debug line and returns 0; an unknown mode returns 0 without
 * printing. Not sure of the exact game wording. */
int MiniG_Main(void)
{
    switch (Gwork.modes.game) {
    case CASINO_MENU:
        if (Gwork.modes.initialized[0] == 0) {
            init_menu_mode();
            Gwork.modes.initialized[0] = 1;
        }
        if (menu_mode()) {
            return 1;
        }
        break;
    case CASINO_SLOT:
        if (Gwork.modes.initialized[1] == 0) {
            init_slot();
            Gwork.modes.initialized[1] = 1;
        }
        slot_main();
        break;
    case CASINO_POKER:
        if (Gwork.modes.initialized[2] == 0) {
            PokerInit();
            Gwork.modes.initialized[2] = 1;
        }
        PokerMain();
        break;
    case CASINO_COIN_EXCHANGE:
        if (Gwork.modes.initialized[3] == 0) {
            init_coin();
            Gwork.modes.initialized[3] = 1;
        }
        coin_main();
        break;
    case CASINO_SHOP:
        if (Gwork.modes.initialized[4] == 0) {
            init_shop();
            Gwork.modes.initialized[4] = 1;
        }
        shop_main();
        break;
    case CASINO_VIEWER:
        if (Gwork.modes.initialized[5] == 0) {
            VW_Init();
            Gwork.modes.initialized[5] = 1;
        }
        VW_Main();
        break;
    case CASINO_NINE_GAME:
        NineGameMain();
        break;
    case CASINO_TEST_MODE:
        test_mode();
        break;
    default:
        return 0;
    }
    xglFontDebugPrintf(4, 0, D_00A0C6E0, D_00A0C6F0);
    return 0;
}



static short real01array[21] = { 1, 3, 1, 0, 4, 4, 0, 1, 3, 1, 0, 2, 1, 0, 1, 2, 1, 0, 5, 5, 0 };

static short real02array[21] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real03array[21] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real11array[21] = { 1, 3, 1, 0, 4, 4, 0, 1, 3, 1, 0, 2, 1, 0, 1, 2, 1, 0, 7, 7, 0 };

static short real12array[21] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 7, 1 };

static short real13array[21] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 7, 3, 1, 0, 7, 7, 3 };

static short real21array[21] = { 1, 3, 1, 6, 4, 6, 0, 1, 3, 1, 6, 2, 1, 0, 1, 2, 1, 0, 5, 5, 0 };

static short real22array[21] = { 3, 0, 1, 3, 6, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real23array[21] = { 1, 0, 0, 6, 2, 1, 6, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real31array[21] = { 6, 3, 1, 6, 4, 6, 6, 1, 3, 1, 0, 2, 1, 0, 6, 2, 1, 6, 5, 5, 6 };

static short real32array[21] = { 3, 0, 1, 3, 0, 0, 0, 1, 2, 0, 4, 0, 1, 2, 0, 1, 0, 0, 3, 5, 1 };

static short real33array[21] = { 1, 0, 0, 0, 2, 1, 0, 2, 1, 4, 2, 0, 0, 1, 5, 3, 1, 0, 5, 5, 3 };

static short real41array[21] = { 8, 7, 8, 7, 8, 7, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

static short real42array[21] = { 8, 8, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 8, 8, 7, 8, 8, 8, 8 };

static short real43array[21] = { 8, 8, 8, 8, 7, 8, 8, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

static short real51array[21] = { 1, 3, 8, 0, 4, 4, 0, 8, 3, 1, 0, 2, 8, 0, 8, 2, 1, 0, 5, 5, 0 };

static short real52array[21] = { 3, 0, 1, 8, 0, 8, 8, 1, 2, 8, 4, 0, 1, 2, 0, 8, 0, 8, 3, 5, 1 };

static short real53array[21] = { 1, 0, 8, 8, 2, 1, 0, 2, 1, 8, 2, 8, 0, 8, 5, 3, 1, 0, 5, 5, 3 };

const char D_00A0C130[32] = "Play 5 coins per round poker\241\251";

const char D_00A0C150[32] = "Play 10 coins per round poker\241\251";

const char D_00A0C170[32] = "Play 30 coins per round poker\241\251";

const char D_00A0C190[40] = "Play 100 coins per round poker\241\251";
