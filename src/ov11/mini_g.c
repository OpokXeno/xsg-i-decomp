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
static void MakeSprite(void *sprite, void *rect)
{
    Ov11GifTag tex_tag = D_00A0BB30;
    Ov11GifTag prim_tag = D_00A0BB40;
    const struct SpriteTex *tex = sprite;
    const struct SpriteRect *pos = rect;
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
    switch (tex->mode) {
    case 1:
        sceVif1PkAddGsData(Pkt, (tex->setup0 + 0x3800) | 0x25440000 | ((u64)(tex->setup1 + 0x3800) << 37) | 0x2000000E40000000ULL);
        v0 = v0 * 2 - 1;
        break;
    case 2:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->setup1 + 0x3800) << 37) | 0x4000000E21343800ULL);
        break;
    case 3:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->setup1 + 0x3800) << 37) | 0x4000000EA9443800ULL);
        v0 = v0 * 2 - 1;
        break;
    default:
        sceVif1PkAddGsData(Pkt, ((u64)(tex->setup1 + 0x3800) << 37) | 0x4000000E65323800ULL);
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
typedef unsigned int CasinoSpriteWords[7];
typedef unsigned int CasinoRectWords[4];
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
static CasinoSpriteWords pay_window[2]  = {
    {224, 0, 32, 32, 0, 572, 0},
    {224, 32, 32, 32, 0, 572, 0}
};
static CasinoRectWords pay_window_rect_4[5]  = {
    {36, 33, 32, 32}, {36, 82, 32, 32}, {36, 131, 32, 32},
    {36, 180, 32, 32}, {36, 229, 32, 32}
};
static int status_keep_5  = 0;
static CasinoSpriteWords BoxSpr  = {0, 0, 511, 447, 0, 1020, 0};
static CasinoRectWords BoxRect  = {0, 0, 511, 447};
static CasinoSpriteWords NineGameBG_Tex_43  = {0, 0, 511, 447, 0, 1020, 0};
static CasinoRectWords NineGameBG_Rect_44  = {0, 0, 512, 448};
static CasinoSpriteWords tex_68  = {0, 0, 512, 448, 0, 1020, 0};
static CasinoRectWords rect_69  = {0, 0, 512, 448};
static CasinoSpriteWords tex_80  = {0, 0, 352, 176, 0, 1020, 0};
static CasinoRectWords rect_81  = {16, 256, 352, 176};
static CasinoRectWords result_rect_97[9]  = {
    {204, 362, 160, 32}, {204, 330, 160, 32}, {204, 298, 160, 32},
    {204, 266, 160, 32}, {0, 394, 244, 32}, {0, 362, 244, 32},
    {0, 330, 244, 32}, {0, 298, 244, 32}, {0, 266, 244, 32}
};
static CasinoSpriteWords tex_98  = {96, 224, 160, 32, 0, 756, 0};
static CasinoSpriteWords SubWindow_Tex  = {0, 0, 176, 120, 0, 564, 2};
static CasinoSpriteWords Level_Tex[4]  = {
    {176, 0, 72, 16, 0, 564, 2}, {176, 16, 72, 16, 0, 564, 2},
    {176, 32, 72, 16, 0, 564, 2}, {176, 48, 72, 16, 0, 564, 2}
};
static CasinoSpriteWords ExChange_Tex[3]  = {
    {176, 64, 64, 16, 0, 564, 2}, {176, 80, 64, 16, 0, 564, 2},
    {176, 96, 64, 16, 0, 564, 2}
};
static CasinoSpriteWords BGTEST_Tex  = {0, 0, 511, 447, 0, 1136, 0};
static CasinoRectWords BGTEST_Rect  = {0, 0, 512, 448};
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

extern unsigned int Real_2[9][7];
extern int xpos_3[3];
extern unsigned int slot_window_0[7];
extern unsigned int slot_window_rect_1[3][4];

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
            MakeSprite(Real_2[SlGetPic(i, j, Gwork.slot.row)], &rect);
        }
    }
    SetDrawStatus(2, 0);
    for (n = 0; n < 3; n++) {
        MakeSprite(slot_window_0, slot_window_rect_1[n]);
    }
    SetDrawStatus(3, 0);
    SetTest(0x3000D);
    ResetRGBA();
    SetRGBA(0, 0, 0, 0x80);
    DrawNonTextureBox(Pkt, 0x50, 0x12C, 0x160, 0x40);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", decprint);

static void decprint(int value, int digits, int x, int y);
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
        MakeSprite(pay_window[windowIndex], pay_window_rect_4[rectIndex]);
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

extern unsigned int spinbt[3][7];
extern unsigned int spinbt_rect_6[4];
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
        MakeSprite(spinbt[index - 1], spinbt_rect_6);
    }
}

extern unsigned int stopbt[3][7];
extern unsigned int stopbt_rect_8[3][4];

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
        MakeSprite(stopbt[index], stopbt_rect_8[i]);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlDecPrintSmall);

extern const struct SpriteRect D_00A0BC38;
extern const struct SpriteRect D_00A0BC48;
extern const struct SpriteRect D_00A0BC58;
extern const struct SpriteRect D_00A0BC68;
extern const struct SpriteRect D_00A0BC78;
extern const struct SpriteRect D_00A0BC88;
extern int pos_10[7][2];
extern unsigned int PicTable[7][7];
extern unsigned int OptionText[5][7];
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
        MakeSprite(PicTable[i], &rect);
    }
    rect.y = pos_10[i][1];
    rect.x = pos_10[i][0];
    rect.w = 0x1C;
    MakeSprite(PicTable[i], &rect);
    rect = D_00A0BC38;
    MakeSprite(OptionText[1], &rect);
    rect = D_00A0BC48;
    MakeSprite(OptionText[1], &rect);
    rect = D_00A0BC58;
    MakeSprite(OptionText[1], &rect);
    rect = D_00A0BC68;
    MakeSprite(OptionText[2], &rect);
    rect = D_00A0BC78;
    MakeSprite(OptionText[3], &rect);
    rect = D_00A0BC88;
    MakeSprite(OptionText[4], &rect);
    for (i = 0; i < 6; i++) {
        if (Gwork.slot.double_pay != 0) {
            SlDecPrintSmall(0xAF, 0x125 + i * 0x14, 0xC, double_rate_12[i]);
        } else {
            SlDecPrintSmall(0xAF, 0x125 + i * 0x14, 0xC, rate_11[i]);
        }
    }
}

extern unsigned int Pic_13[9][7];
extern unsigned int Total_14[7];
extern unsigned int linetex_15[7];
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
                MakeSprite(Pic_13[line->pic[k]], &rect);
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
                    MakeSprite(OptionText[1], &rect);
                    break;
                case 2:
                    rect.w = 0x68;
                    MakeSprite(OptionText[2], &rect);
                    break;
                case 3:
                    rect.w = 0x50;
                    MakeSprite(OptionText[0], &rect);
                    break;
                }
            } else {
                SlDecPrintSmall(0x119, y, 0xC, line->prize * Gwork.slot.line_pay[i]);
            }
            y += 0x14;
        }
    }
    rect = D_00A0BC98;
    MakeSprite(Total_14, &rect);
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
            MakeSprite(linetex_15, &linerect_16[i]);
        }
    }
    SetDrawStatus(3, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlModePrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlGuidPrint);

extern PadPrefix PadData;

/* PadData as the casino reads it. The shared PadPrefix stops at +0x36; this
 * TU also needs the button word at +0x2C and the four analog stick bytes at
 * +0x64, so it uses a union with the canonical prefix, the same way
 * src/ov02/umn_guno_data_base_get.c widens it. Left/right are read from
 * +0x2C while up/down come from the pressed word at +0x2A; not sure how the
 * two words differ (key repeat would fit). */
typedef union CasinoPadView {
    PadPrefix canonical;
    /* The four halfwords from +0x28 read as one doubleword; coin_main tests
     * bits of +0x2A and +0x2C together this way. */
    struct {
        unsigned char unmodeled_00[0x28];
        u64 buttons;
    } dword;
    struct {
        unsigned char unmodeled_00[0x2C];
        u16 buttons;
        unsigned char unmodeled_2e[0x36];
        signed char stick[4];
    } wide;
} CasinoPadView;
#define CASINO_PAD ((CasinoPadView *)&PadData)
static void SlGuidPrint(int index);

/* ov11:0x00a01d20. Slot pay-line pointer. check_mode() 0 draws the guide
 * at Gwork.slot.guide; 1 draws it at Gwork.slot.pointer + 5 and pad bits
 * 0x2000 / 0x8000 step the pointer forward / back (wrapping within 0..2)
 * with a sound. Not sure what the pointer is for in game terms. */
static void point_control(XglPacket *packet)
{
    switch (check_mode()) {
    case 0:
        SlGuidPrint(Gwork.slot.guide);
        break;
    case 1:
        SlGuidPrint(Gwork.slot.pointer + 5);
        if (PadData.half_2a & 0x2000) {
            Gwork.slot.pointer = Gwork.slot.pointer + 1;
            if (Gwork.slot.pointer >= 3) {
                Gwork.slot.pointer = 0;
            }
            RES_SoundEffect(0x24);
        }
        if (PadData.half_2a & 0x8000) {
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
        MakeSprite(BoxSpr, BoxRect);
    } while (0);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", SlCoinEntry);

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
 * otherwise pad bit 0x20 pays the pending coins at once, else one coin per
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
    if (PadData.half_2a & 0x20) {
        AddCoin(Gwork.slot.payout);
        Gwork.slot.payout = 0;
        RES_SoundEffectStop(0x27);
    }
    if (Gwork.slot.payout == 0) {
        if (PadData.half_2a & 0x20) {
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

/* ov11:0x00a02630. Slot input handler, by Gwork.slot.state. Idle (0): pad
 * 0x40 leaves to the menu; otherwise it runs SlCoinEntry, and pad 0x20
 * with a stake starts a spin (help off, pointer and reels reset to
 * spinning, the stake taken from the coin balance, sound 0x22). Spinning
 * (1): pad 0x20 stops the reel under the pointer, a stopped reel slides on
 * by 0x10 per frame until it can snap to the next 64-pixel cell, and once
 * all three are still the result is checked (check_real, linepay_check).
 * Result (2): runs paid_action. A stop moves the pointer to the next reel.
 * Not sure of the button names. */
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
        if (PadData.half_2a & 0x40) {
            RES_SoundEffect(0x17);
            GameModeChange(CASINO_MENU);
            break;
        }
        SlCoinEntry();
        if ((PadData.half_2a & 0x20) && Gwork.slot.total_bet > 0) {
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
                if (Gwork.slot.pointer == i && (PadData.half_2a & 0x20)) {
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
                if ((PadData.half_2a & 0x20) && Gwork.slot.pointer == i) {
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
extern const unsigned int D_00A0BE90[4];
extern unsigned int tex_36[7];
struct Wave09Rect4 { unsigned int words[4]; };
static void NgTimePrint(int time)
{
    struct Wave09Rect4 rect = *(const struct Wave09Rect4 *)D_00A0BE90;
    if (time > 900) time = 900;
    rect.words[2] = (time * 47 * 4) / 900;
    SetDrawStatus(3, 0);
    MakeSprite(tex_36, &rect);
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
extern unsigned long long pow(unsigned long long base, unsigned long long exponent);

static void NgPaidPrint(void)
{
    decprint(Gwork.nine.payout * dptoli(pow(DOUBLE_TWO, litodp(Gwork.nine.counter))), 7, 0x1B2, 0x182);
}

extern unsigned int tex_37[3][7];
extern unsigned int rect_38[4];
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
    MakeSprite(tex_37[index], rect_38);
    if (count_39 != 0) {
        count_39 = count_39 - 1;
        if (count_39 == 0) {
            count_39 = -1;
        }
    }
}

extern unsigned int spinbt[3][7];
extern unsigned int rect_40[4];
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
    MakeSprite(spinbt[index], rect_40);
    if (count_41 != 0) {
        count_41 = count_41 - 1;
        if (count_41 == 0) {
            count_41 = -1;
        }
    }
}

static void NgLamp(void);
INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgLamp);

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
    MakeSprite(NineGameBG_Tex_43, NineGameBG_Rect_44);
}

static void NgStopGuidPt(void);
INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgStopGuidPt);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgFwdGuidPt);

static void NgStopBtnPrint(void);
INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", NgStopBtnPrint);

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

/* Hypothesis, OV11 0x00A03CA0. Original reads a 28-byte descriptor with
 * three unaligned doubleword pairs and one final word; the struct copy
 * is intended to expose that original GCC bulk-copy lowering. */
struct Wave06Sprite28 { unsigned int word[7]; };
static void *get_ngreal_shape(unsigned int *out, int reel, int shape)
{
    short *const *reel_table = (short *const *)0x00A0A738;
    const struct Wave06Sprite28 *sprites = (const struct Wave06Sprite28 *)0x00A0A788;
    short *indices;
    int selected;
    if (shape >= 21) shape -= 21;
    if (shape < 0) shape += 21;
    indices = reel_table[reel];
    selected = indices[shape];
    *(struct Wave06Sprite28 *)out = sprites[selected];
    return out;
}


INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", DrawNgPanel);

static void DrawNgPanel(int index, int y, int x);

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
    scale = pow(DOUBLE_TWO, litodp(Gwork.nine.counter));
    /* Empty loop: GCC 2.96 does not schedule across loop notes, which keeps
     * the original instruction order here (as in act_2.c). */
    do { } while (0);
    Gwork.nine.payout = dptoli(dpmul(litodp(Gwork.nine.payout), scale));
    Gwork.nine.counter = 0;
    NgGetsDownBtnPrint(16);
    NgAllStop();
}

static void NgFwdGuidPt(void);

/* ov11:0x00a04040. Nine-game select handler: up/down toggles mode 9/10,
 * confirm (0x20) starts the spin or the gets-down action, and several
 * buttons toggle the flag. Not sure of the button names. */
static void NgGame_Select(void)
{
    int i;

    if (PadData.half_2a & 0x5000) {
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
    if (PadData.half_2a & 0x20) {
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
    if (PadData.half_2a & 0x99F) {
        if (Gwork.nine.flag == 0) {
            Gwork.nine.flag = 1;
        } else {
            Gwork.nine.flag = 0;
        }
    }
}

/* Cursor moves of the nine-game: for each cursor position (0-8 the reels,
 * 10 the gets-down button; not sure what 9 is) the position reached with
 * the four direction buttons. Not sure which column is which direction. */
extern unsigned short ptmove_50[11][4];

/* ov11:0x00a04188. Nine-game input: the four direction buttons move the
 * cursor (Gwork.nine.mode) through ptmove_50, button 0x40 jumps to
 * position 10, and button 0x20 acts on the cursor: on a reel that is still
 * running (state 0) it starts the stop (state 2, remaining = 96 minus
 * position % 48) and moves the cursor on (NgFwdGuidPt); on position 10
 * it takes the winnings (NgGame_GetsDown). The case range is what gives
 * the original's compare order (below 0, below 9, equal to 10). */
static void NgGame_Key(void)
{
    struct NineGameReel *reel;

    if (CASINO_PAD->wide.buttons & 0x8000) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][0];
        RES_SoundEffect(0x2E);
    }
    if (CASINO_PAD->wide.buttons & 0x1000) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][1];
        RES_SoundEffect(0x2E);
    }
    if (CASINO_PAD->wide.buttons & 0x2000) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][2];
        RES_SoundEffect(0x2E);
    }
    if (CASINO_PAD->wide.buttons & 0x4000) {
        Gwork.nine.mode = ptmove_50[Gwork.nine.mode][3];
        RES_SoundEffect(0x2E);
    }
    if (PadData.half_2a & 0x40) {
        Gwork.nine.mode = 10;
        RES_SoundEffect(0x31);
    }
    if (PadData.half_2a & 0x20) {
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
 * at once if pad button 0x20 is held and one per call otherwise. Not sure
 * of the button name. */
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
    if (PadData.half_2a & 0x20) {
        AddCoin(amount);
        Gwork.nine.payout = 0;
    } else {
        Gwork.nine.payout = amount - 1;
        AddCoin(1);
    }
}

extern const struct NgHelpTex {unsigned int word[7];} D_00A0BEA0, D_00A0BEC0;
extern const struct NgHelpRect {unsigned int word[4];} D_00A0BEE0, D_00A0BEF0;

/* ov11:0x00a04410. Nine-game help screen: two textured sprites from fixed
 * tables. */
static void NgHelpMode(void)
{
    struct NgHelpTex tex1 = D_00A0BEA0;
    struct NgHelpTex tex2 = D_00A0BEC0;
    struct NgHelpRect rect1 = D_00A0BEE0;
    struct NgHelpRect rect2 = D_00A0BEF0;

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

static void PoMoneyPrint(int mode, int value);
INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoMoneyPrint);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoCurDraw);

extern const struct PoWinRect { int x, y, w, h; } D_00A0BF20;
extern unsigned int Ltex_56[7];
extern unsigned int Rtex_57[7];
extern unsigned int tex_58[7];
extern const char D_00A0BF30[];
extern const char D_00A0BF38[];
extern void xglFontPrint(int x, int y, int color, const char *text);

/* ov11:0x00a04ac8. Poker: draws a message window sized by the text length
 * (left cap, right cap, middle) and prints the message. */
static void PoWindowMessage(const char *message, int length)
{
    struct PoWinRect rect = D_00A0BF20;

    SetDrawStatus(3, 0);
    SetTest(0x5000D);
    rect.x = 16;
    rect.w = 8;
    MakeSprite(Ltex_56, &rect);
    rect.x = length * 9 + 28;
    MakeSprite(Rtex_57, &rect);
    rect.x = 24;
    rect.w = length * 9 + 4;
    MakeSprite(tex_58, &rect);
    xglFontPrint(0, 0, 0, D_00A0BF30);
    xglFontPrint(0, 0, 0, D_00A0BF38);
    xglFontPrint(24, 218, 0xFF00, message);
}

extern unsigned int tex_59[7];
extern unsigned int rect_60[2][4];
extern unsigned int type0_tex_61[7];
extern unsigned int type0_rect_62[4];
extern unsigned int type1_tex_63[7];
extern unsigned int type1_rect_64[4];
extern short type0_pos_65[2][2];
extern short type1_pos_66[3][2];
extern int dialog_cur_67;
static void PoCurDraw(short *pos);
static short PoGetBase(int cardValue);

/* ov11:0x00a04be0. Poker yes/no style dialog. type 0 shows a two-choice
 * window and any other type a three-choice window. Pad bit 0x1000 moves
 * the cursor (dialog_cur_67) up with wraparound and 0x4000 moves it down,
 * both with a sound; 0x20 confirms (plays a sound, resets the cursor and
 * returns the chosen entry) and 0x40 cancels (returns -1); otherwise it
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
    if (PadData.half_2a & 0x1000) {
        dialog_cur_67--;
        RES_SoundEffect(0x3C);
        if (dialog_cur_67 == 0) {
            dialog_cur_67 = limit;
        }
    }
    if (PadData.half_2a & 0x4000) {
        RES_SoundEffect(0x3C);
        dialog_cur_67++;
        if (dialog_cur_67 == limit + 1) {
            dialog_cur_67 = 1;
        }
    }
    SetDrawStatus(3, 0);
    if (type == 0) {
        MakeSprite(tex_59, rect_60[0]);
        MakeSprite(type0_tex_61, type0_rect_62);
        PoCurDraw(type0_pos_65[dialog_cur_67 - 1]);
    } else {
        MakeSprite(tex_59, rect_60[1]);
        MakeSprite(type1_tex_63, type1_rect_64);
        PoCurDraw(type1_pos_66[dialog_cur_67 - 1]);
    }
    if (PadData.half_2a & 0x20) {
        RES_SoundEffect(0x3F);
        selected = dialog_cur_67;
        dialog_cur_67 = 1;
        return selected;
    }
    if (PadData.half_2a & 0x40) {
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
    MakeSprite(tex_68, rect_69);
}

extern void *CardBuf_70[4];
extern unsigned int CardTex[15][7];
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
                MakeSprite(CardTex[card - base], &CardPos[slot]);
            }
        }
        base += 13;
    }
}

extern struct SpriteRect D_00A0DCA8[5];
extern int D_00A0DCF8;
extern int status_73;
extern unsigned int tex_72[7];

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
            MakeSprite(tex_72, &D_00A0DCA8[i]);
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
            MakeSprite(tex_72, &D_00A0DCA8[i]);
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
extern unsigned int tex_76[7];

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
        MakeSprite(tex_76, &D_00A0DD00);
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
        MakeSprite(tex_76, &D_00A0DD00);
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
    MakeSprite(tex_80, rect_81);
}

extern int cnt_82;
extern int add_83;
extern int rate_84;
extern unsigned int tex_85[7];
extern unsigned int rect_86[4];
extern unsigned int MarkerTex_87[7];
extern unsigned int MarkerPos_88[5][4];
extern unsigned int PtrTex_89[7];
extern unsigned int PtrPos_90[4][4];

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
    MakeSprite(tex_85, rect_86);
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
        MakeSprite(MarkerTex_87, MarkerPos_88[i]);
    }
    for (i = 0; i < Gwork.poker_deal.bonus_round && i != 5; i++) {
        if (i == Gwork.poker_deal.bonus_round - 1) {
            SetDrawStatus(6, rate_84);
        } else {
            SetDrawStatus(6, 0x30);
        }
        MakeSprite(PtrTex_89, PtrPos_90[i]);
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

static int PoShuffleCard(void);
INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoShuffleCard);

extern unsigned int PoSelecterTex[2][7];
extern unsigned int PoSelecterPos[6][4];

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
        MakeSprite(PoSelecterTex[Gwork.poker_deal.replace_selected[i]], PoSelecterPos[i]);
    }
    MakeSprite(PoSelecterTex[1], PoSelecterPos[5]);
}

static int PoKey(void);
INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", PoKey);

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
    MakeSprite(tex_98, result_rect_97[category]);
    SetDrawStatus(3, 0);
}

/* ov11:0x00a06308. Stores the caller-supplied poker sequence state in the
 * global sequence word. */
extern int D_00A0DEA8;

static void PoSeqChange(int sequenceState)
{
    D_00A0DEA8 = sequenceState;
}

/* ov11:0x00a06318. Looks up the currently selected bet level's ready
 * message and message-box width from two fixed four-entry tables and
 * forwards both to the message window renderer. D_00A0DD2C is the level
 * index level_select stores (0..3); the four strings are the "Play N
 * coins" prompts for the four bet levels. The compiled form copies each
 * table onto the stack as a 16-byte aggregate before indexing it, which is
 * why each table is typed as a one-member array struct rather than a bare
 * array (a bare array cannot be copied by value in C). PoWindowMessage is
 * a LOCAL asm sibling. */
typedef struct {
    const char *text[4];
} PokerReadyMessageTable;

typedef struct {
    int width[4];
} PokerReadyWidthTable;

static void PoWindowMessage(const char *message, int width);
extern int D_00A0DD2C;
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

    PoWindowMessage(readyMessage.text[D_00A0DD2C], readyWidth.width[D_00A0DD2C]);
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
 * Gwork.poker_deal.seq (jump table jtbl_00A0C220):
 *   0: if the coin balance is below the level's stake (PokerCoinRate) go to
 *      1, else show the play dialog; play goes to 2, anything else back to
 *      the casino menu. 1: D_00A0C1D8 message, any button leaves.
 *   2: move the stake from the balance into the pot, 1, 5 or 10 coins per
 *      frame. 3-7: deal (PoCardDrawAnime), PoShuffleCard, choose cards
 *      (PoKey), PoCardFlip and PoShuffleCard again, then score the hand
 *      (PoCardCheck; a win multiplies the pot by PokerRate).
 *   8: D_00A0C1F8 dialog. 9/11: D_00A0C208 dialog; yes starts the bonus
 *      game (12), no pays out (18).
 *   12-17: bonus game. Each round deals a card and compares its value with
 *      the previous card against the guess from the dialog in 17 (a tie
 *      also wins). A win doubles the pot and five wins end the game; a
 *      loss clears the pot (16 waits for button 0x20).
 *   18: pay the pot out, a step per frame by level (1, 5, 10, 100) or all
 *      at once with button 0x20.
 * Not sure what sets state 8 or what the dialog texts say. coins is kept
 * apart from step: one shared variable allocates state 18 differently. */
static void PokerMain(void)
{
    int sel;
    int step;
    int coins;
    int i;
    int a;
    int b;

    Pkt = xglPacketGetCurrent();
    PoBgDraw();
    xglPacketTextureTrans((void *)POKER_TABLE2_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    switch (Gwork.poker_deal.seq) {
    case 0:
        Gwork.poker_deal.rates_hidden = 0;
        Gwork.poker_deal.in_round = 0;
        if (((struct CasinoSaveWork *)SaveWork)->coin_balance < PokerCoinRate[Gwork.poker_deal.level]) {
            PoSeqChange(1);
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
            PoSeqChange(2);
            Gwork.poker_deal.pot = 0;
            PoResetCard();
            break;
        }
        GameModeChange(CASINO_MENU);
        init_99 = 0;
        RES_SoundEffect(0x35);
        break;
    case 1:
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
    case 2:
        if (Gwork.poker_deal.pot == PokerCoinRate[Gwork.poker_deal.level]) {
            Gwork.poker_deal.in_round = 1;
            PoSeqChange(3);
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
    case 3:
        Gwork.poker_deal.rates_hidden = 0;
        if (PoCardDrawAnime() != 0) {
            PoSeqChange(4);
        }
        break;
    case 4:
        if (PoShuffleCard() == 1) {
            for (i = 0; i < 5; i++) {
                Gwork.poker_deal.replace_selected[i] = 1;
            }
            PoSeqChange(5);
        }
        PoCardDraw();
        break;
    case 5:
        PoCardDraw();
        PoSelectMarkDraw();
        if (PoKey() == 1) {
            PoSeqChange(6);
            PoCardFlip();
        }
        break;
    case 6:
        if (PoShuffleCard() == 1) {
            PoSeqChange(7);
        }
        PoCardDraw();
        break;
    case 7:
        Gwork.poker_deal.hand_rank = PoCardCheck();
        if (Gwork.poker_deal.hand_rank > 0) {
            Gwork.poker_deal.pot *= PokerRate[Gwork.poker_deal.hand_rank];
            PoSeqChange(9);
        } else {
            PoSeqChange(0);
        }
        PoCardDraw();
        break;
    case 8:
        Gwork.poker_deal.rates_hidden = 1;
        PoWindowMessage(D_00A0C1F8, 10);
        sel = PoDialog(0);
        if (sel != 0) {
            if (sel == 1) {
                PoSeqChange(3);
                PoResetCard();
                break;
            }
            GameModeChange(CASINO_MENU);
            init_99 = 0;
        }
        PoCardDraw();
        break;
    case 9:
        PoSeqChange(11);
    case 11:
        Gwork.poker_deal.bonus_round = 0;
        Gwork.poker_deal.rates_hidden = 1;
        PoRateTableDraw();
        PoResultLamp(Gwork.poker_deal.hand_rank - 1);
        PoWindowMessage(D_00A0C208, 0x11);
        sel = PoDialog(0);
        if (sel != 0) {
            if (sel == 1) {
                PoSeqChange(12);
                break;
            }
            RES_SoundEffect(0x3B);
            PoSeqChange(18);
        }
        PoCardDraw();
        break;
    case 12:
        PoResetCard();
        for (i = 0; i < 5; i++) {
            Gwork.poker_deal.cards.hand[i] = 0x36;
            Gwork.poker_deal.replace_selected[i] = 0;
        }
        PoSeqChange(13);
    case 13:
        if (PoBonusCardAnime() == 1) {
            PoSeqChange(14);
        }
        PoCardDraw();
        PoBonusBGDraw();
        break;
    case 14:
        if (PoShuffleCard() == 1) {
            PoSeqChange(15);
        }
        PoBonusBGDraw();
        PoCardDraw();
        break;
    case 15:
        if (Gwork.poker_deal.bonus_round == 0) {
            Gwork.poker_deal.bonus_round = 1;
            PoSeqChange(17);
        } else {
            a = Gwork.poker_deal.cards.hand[Gwork.poker_deal.bonus_round - 1];
            a -= PoGetBase(PoGetMark(a));
            b = Gwork.poker_deal.cards.hand[Gwork.poker_deal.bonus_round];
            b -= PoGetBase(PoGetMark(b));
            if ((b < a) == Gwork.poker_deal.guess || a == b) {
                RES_SoundEffect(0x38);
                Gwork.poker_deal.bonus_round++;
                Gwork.poker_deal.pot *= 2;
                if (Gwork.poker_deal.bonus_round != 5) {
                    PoSeqChange(17);
                } else {
                    PoSeqChange(18);
                }
            } else {
                Gwork.poker_deal.bonus_round = 0;
                Gwork.poker_deal.pot = 0;
                RES_SoundEffect(0x3A);
                PoSeqChange(16);
            }
        }
        PoBonusBGDraw();
        PoCardDraw();
        break;
    case 16:
        PoBonusBGDraw();
        PoCardDraw();
        if (PadData.half_2a & 0x20) {
            PoSeqChange(0);
        }
        break;
    case 17:
        PoBonusBGDraw();
        PoCardDraw();
        sel = PoDialog(1);
        if (sel != 0) {
            if (sel == 3 || sel == -1) {
                PoSeqChange(18);
                RES_SoundEffect(0x3B);
                break;
            }
            if (sel == 1) {
                Gwork.poker_deal.guess = 0;
            } else {
                Gwork.poker_deal.guess = 1;
            }
            PoSeqChange(13);
        }
        break;
    case 18:
        if (Gwork.poker_deal.bonus_round != 0) {
            PoBonusBGDraw();
        } else {
            PoRateTableDraw();
            PoResultLamp(Gwork.poker_deal.hand_rank - 1);
        }
        PoCardDraw();
        if (PadData.half_2a & 0x20) {
            AddCoin(Gwork.poker_deal.pot);
            PoSeqChange(0);
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
            PoSeqChange(0);
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

/* ov11:0x00a06c08. Resets five Gwork work-state words used by the poker
 * minigame: its only caller, GameModeChange, uses it to clear the main
 * sequence, display flags, bonus mode and bonus round index before a
 * poker session starts. Field identities beyond their byte offsets are
 * not separately evidenced. */
static void PoInitWork(void)
{
    Gwork.words[96] = 0;
    Gwork.words[97] = 0;
    Gwork.poker_deal.bonus_mode = 0;
    Gwork.poker_deal.bonus_round = 0;
    Gwork.words[98] = 0;
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

/* ov11:0x00a06df0. Builds a fixed 0xB8x0x70 rect at the caller-supplied
 * (y, x) origin on the stack and draws the submenu window sprite into it;
 * this is the same {y, x, width, height} rect layout MakeSprite's other
 * (data-supplied) rect arguments use elsewhere in this TU. windowId itself
 * is not read here; level_select and exchange_select forward it on to
 * submenu_select right after calling sub_window. */

static void sub_window(int windowId, int y, int x)
{
    SubWindowRect rect;

    SetDrawStatus(1, 0);
    rect.y = y;
    rect.x = x;
    rect.width = 0xB8;
    rect.height = 0x70;
    MakeSprite(SubWindow_Tex, &rect);
}

extern int anm_100;
extern unsigned int Cursor_Tex[8][7];

/* ov11:0x00a06e50. Draws a submenu and handles its input: the window
 * (sub_window), one icon per choice in a row starting at (y + 0x34, x +
 * 0xC), and an animated cursor (Cursor_Tex, 8 frames by anm_100) above the
 * selected icon. Up/down on the +0x2C pad word move *selected with
 * wraparound (sounds 5 and 6). Returns 1 when button 0x20 is pressed, else
 * 0. Not sure of the button names. */
static int submenu_select(int windowId, int y, int x, int iconCount,
                          void *icons, int *selected)
{
    SubWindowRect rect;
    SubWindowRect cursor;
    unsigned int (*icon)[7];
    int cursor_x;
    int i;

    sub_window(windowId, y, x);
    SetDrawStatus(1, 0);
    cursor_x = 0;
    rect.y = y + 0x34;
    rect.width = 0x48;
    rect.height = 0x10;
    icon = icons;
    for (i = 0; i < iconCount; i++) {
        rect.x = x + 0xC + i * 0x18;
        if (i == *selected) {
            cursor_x = rect.x;
        }
        MakeSprite(icon[i], &rect);
    }
    cursor.y = rect.y - 0x10;
    cursor.x = cursor_x;
    cursor.height = 0x10;
    cursor.width = 0x10;
    MakeSprite(Cursor_Tex[anm_100 % 8], &cursor);
    anm_100++;
    if (CASINO_PAD->wide.buttons & 0x1000) {
        (*selected)--;
        RES_SoundEffect(5);
    }
    if (CASINO_PAD->wide.buttons & 0x4000) {
        (*selected)++;
        RES_SoundEffect(6);
    }
    if (*selected > iconCount - 1) {
        *selected = 0;
    }
    if (*selected < 0) {
        *selected = iconCount - 1;
    }
    if (PadData.half_2a & 0x20) {
        return 1;
    }
    return 0;
}

/* ov11:0x00a07000. Draws a four-choice submenu bound to the shared game
 * level selection. sub_window's own body (asm/nonmatchings/ov11/mini_g/
 * sub_window.s) overwrites its first argument before using it, so windowId
 * only reaches submenu_select; y and x are the row/column submenu_select
 * stores for the first icon (offset there by 0x34 and 0xC before laying the
 * remaining icons out, per its and exchange_select's own disassembly).
 * level_select's own tail call (`j submenu_select`) is why its return value
 * is submenu_select's, and menu_mode branches on it (`beqz $2` right after
 * `jal level_select`). */
static void sub_window(int windowId, int y, int x);
static int submenu_select(int windowId, int y, int x, int iconCount,
                          void *icons, int *selected);
extern int D_00A0DD2C;

static int level_select(int windowId, int y, int x)
{
    sub_window(windowId, y, x);
    return submenu_select(windowId, y, x, 4, Level_Tex, &D_00A0DD2C);
}

/* ov11:0x00a07060. Draws a three-choice submenu bound to the shared exchange
 * mode selection, the same sub_window()+submenu_select() shape level_select
 * uses above; exchange_select's own tail call (`j submenu_select`) is why its
 * return value is submenu_select's. */
extern int D_00A0DD24;

static int exchange_select(int windowId, int y, int x)
{
    sub_window(windowId, y, x);
    return submenu_select(windowId, y, x, 3, ExChange_Tex, &D_00A0DD24);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", menu_mode);

/* ov11:0x00a07410. Loads the menu background and window textures, then
 * clears the bet level (Gwork.modes.level) and the short at 0x10 (not
 * sure what that one holds). */
static const char D_00A0C320[] = "data\\tanaka\\BG.xtx";
static const char D_00A0C338[] = "data\\tanaka\\window.xtx";

#define MENU_BG_TEX_BUFFER 0x0114A800
#define MENU_WINDOW_TEX_BUFFER 0x0118B000
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
    MakeSprite(BGTEST_Tex, BGTEST_Rect);
}

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", DecPrint2);

/* Wave 14: new C order hwxy; unverified, no inline assembly. */
extern unsigned int Tex_106[56];
extern int anm_107;
static void UtlCurPrint(int x, int y)
{
    struct W14Rect { int x, y, w, h; } rect;
    int n;
    xglPacketTextureTrans((void *)MENU_WINDOW_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    rect.h = 24;
    rect.w = 24;
    rect.x = x;
    rect.y = y;
    n = anm_107;
    MakeSprite(Tex_106 + 7 * (n % 8), &rect);
    anm_107 = anm_107 + 1;
}


#define COIN_EXCHANGE_TEX_BUFFER 0x011CB800

extern unsigned int BG_Tex[7];
extern unsigned int BG_Rect[4];
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
 * (cur_108, five rows) with up/down on the +0x2C pad word, and on confirm
 * (pad bits tested together as one doubleword) trades one money box for
 * 10, 100, 500 or 1000 coins on rows 0-3. If the balance is already
 * 9999999 or more, the box is put back. The last row, or button 0x40,
 * leaves to the menu. Then it draws the cursor and the money-box and coin
 * totals. Not sure of the button names. */
static void coin_main(void)
{
    int leave;

    Pkt = xglPacketGetCurrent();
    xglPacketTextureTrans((void *)COIN_EXCHANGE_TEX_BUFFER);
    SetRegAD(Pkt, 0x3F, 0);
    SetDrawStatus(3, 0);
    MakeSprite(BG_Tex, BG_Rect);
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
    if (CASINO_PAD->wide.buttons & 0x1000) {
        RES_SoundEffect(0xE);
        cur_108--;
    }
    if (CASINO_PAD->wide.buttons & 0x4000) {
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
    if (PadData.half_2a & 0x40) {
        leave = 1;
    }
    if (CASINO_PAD->dword.buttons & 0x800200000ULL) {
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
            if (PadData.half_2a & 0x20) {
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

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", init_shpmenu);

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", shop_main);

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
extern char fileinfo[30];
extern int old_113;
extern const char D_00A0C5C8[];
extern const char D_00A0C5D8[];
extern const char D_00A0C5E8[];
extern const char D_00A0C5F8[];
extern const char D_00A0C608[];
extern const char D_00A0C618[];
extern const char D_00A0C628[];

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

INCLUDE_ASM("asm/nonmatchings/ov11/mini_g", VW_ViewMode);

extern const char *Cajino_GetTitle(int sample);
extern const char *Cajino_GetInfo(int sample);
extern unsigned int back_tex_118[7];
extern unsigned int ViewSamTex[30][7];
extern unsigned int sam_rect_117[4];
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
 * scroll offset with up/down (sounds 0x18 and 0x19). If the list is not
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
    MakeSprite(back_tex_118, BG_Rect);
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
    if (CASINO_PAD->wide.buttons & 0x1000) {
        RES_SoundEffect(0x18);
        if (cur_119 != 0) {
            cur_119--;
        } else if (ofs_120 > 0) {
            ofs_120--;
        }
    }
    if (CASINO_PAD->wide.buttons & 0x4000) {
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
        MakeSprite(ViewSamTex[sample], sam_rect_117);
    }
    return sample;
}

extern int mode_123;
extern int D_00A0DD18;
static int VW_ViewMode(int arg);

/* ov11:0x00a08ce0. View mode main: switches between VW_ViewMode and
 * VW_SamMode on mode_123; pad bit 0x10 enters the sam list and 0x40 leaves
 * it (calls GameModeChange(CASINO_MENU)). Not sure of the button names. */
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
        if (PadData.half_2a & 0x10) {
            RES_SoundEffect(0x1A);
            mode_123 = 1;
        }
    }
    if (PadData.half_2a & 0x40) {
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
static int menu_mode(void);
static void shop_main(void);

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
