#ifndef INCLUDE_MAIN_XGL_FONT_H
#define INCLUDE_MAIN_XGL_FONT_H

#include "shared.h"

/* The VIF packet the font effect command (code 19) builds in the work area. */
struct FontEffectScratchPacket {
    u64 vif_tag;
    u64 vif_count;
    u64 effect_state;
    u64 payload_count;
};

/* DrawCredit (src/main/game_pause_disp.c) writes this 128-byte direct-data
 * payload into the work area. */
typedef struct PauseCreditData {
    unsigned int initial_words[4];
    unsigned long long wide_words[3];
    unsigned char unmodeled_28[8];
    int body_words[20];
} PauseCreditData;

/* The two packets DrawUmnInterface2 (umn_interface2.c) builds in the work
 * area. The setup packet is sent before the geometry packet overwrites it. */
typedef struct UmnIf2AdCommand {
    u64 value;
    u64 register_address;
} UmnIf2AdCommand;

typedef struct UmnIf2SetupPacket {
    u64 tag;
    u64 registers;
    UmnIf2AdCommand texture_flush;
    UmnIf2AdCommand alpha;
    UmnIf2AdCommand texture;
    UmnIf2AdCommand test;
} UmnIf2SetupPacket;

/* GIF PACKED mode expands each register into four 32-bit lanes. The tag's
 * 0x531 list repeats RGBAQ, UV and XYZ2 for each of the six vertices. */
typedef struct UmnIf2PackedRegister {
    u32 lane[4];
} UmnIf2PackedRegister;

typedef struct UmnIf2GeometryPacket {
    u32 tag_low;
    u32 tag_high;
    u32 registers_low;
    u32 registers_high;
    UmnIf2PackedRegister payload[18];
} UmnIf2GeometryPacket;

/*
 * The rest of the 16 KiB EE scratchpad after the draw cursor header. The
 * font effect command and every xglFontPrintExtFunc callback build their
 * direct-data packets here: the qword views write command quadwords, the
 * word views the 32-bit fields inside them.
 */
typedef union FontDrawWork {
    struct FontEffectScratchPacket effect_packet;
    PauseCreditData credit_data;
    UmnIf2SetupPacket umn_setup;
    UmnIf2GeometryPacket umn_geometry;
    u64 qwords[(0x4000 - 0x30) / sizeof(u64)];
    int words[(0x4000 - 0x30) / sizeof(int)];
} FontDrawWork;

/*
 * The font renderer's draw cursor. xglFontFlush builds it at the scratchpad
 * base (0x70000000) and xglFontFlushCore passes it to the callbacks queued
 * by xglFontPrintExtFunc and to xglFontReloadTexture.
 */
typedef struct FontDrawCursor {
    XglPacket *packet;
    short origin_x;
    unsigned char unmodeled_06[2];
    unsigned short x;
    unsigned short y;
    int depth;
    unsigned short offset_x;
    unsigned short offset_y;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char unmodeled_17;
    unsigned char flags;
    unsigned char effect;
    unsigned char attributes;
    unsigned char draw_state_high;
    unsigned char unmodeled_1c[4];
    signed char effect_x;
    signed char effect_y;
    unsigned char unmodeled_22[2];
    unsigned char effect_red;
    unsigned char effect_green;
    unsigned char effect_blue;
    unsigned char unmodeled_27;
    unsigned char primitive_flags;
    unsigned char primitive_mode;
    unsigned char alpha;
    unsigned char unmodeled_2b;
    unsigned short scale_x;
    unsigned short scale_y;
    FontDrawWork work;
} FontDrawCursor;

/* A queued draw callback: called with the draw cursor and the argument
 * registered with it. */
typedef void (*FontExtFunc)(FontDrawCursor *cursor, void *argument);

void xglFontPrintExtFunc(int ot, FontExtFunc function, void *argument);
void xglFontReloadTexture(FontDrawCursor *cursor, int flags);

extern void xglFontPrint(int x, int y, int color, const char *text);
extern void xglFontPrintDirectOT(int ot, const char *text);
extern void xglFontPrintf(int x, int y, unsigned int color, void *arg);

/* The accepted io-domain prototype (include/xeno/io/functions.h, the same
 * spelling as src/io/main-00256190/HddTest.c). The io headers cannot be
 * included next to the core ones (conflicting XglClock/PadPrefix views), so
 * it is repeated here. It must be the void prototype: an implicit int
 * declaration makes the call set $v0, and local-alloc's false-dependency
 * avoidance then keeps the owner pointer out of $v0 (funcObserver:
 * attempt-4d6f277fff4a form 17, implicit, vs attempt-5684d45333e0 form 01). */
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

unsigned int xglFontAscii2Euc(signed char character, signed char **text);

/* Functions and data used by other translation units. */
int xglFontGetStringWidth(const char *text);
int xglFontGetSPcodeSize(int code, const unsigned char *text);
unsigned short xglFontGetFlags(void);
void xglFontPrintDirect(const char *text);
void xglFontSetFlags(int flags);
unsigned int xglFontGetProportionalSize(int code);
extern int xglFontGetStringWidth2(const char *text, int);

#endif /* INCLUDE_MAIN_XGL_FONT_H */
