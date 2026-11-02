#ifndef INCLUDE_MAIN_END_PRINT_H
#define INCLUDE_MAIN_END_PRINT_H

#include "shared.h"
#include "main/xgl_packet.h"
#include "main/xgl_font.h"

/* Sprite coordinates, color and packed UV/CLUT indices consumed by the draw routines. */
typedef struct PrintSpriteInfo {
    short x, y;
    unsigned int depth;
    short width, height;
    unsigned char color[4];
    short uv; /* low byte: UV table entry; high byte: CLUT table entry */
    unsigned char column, row;
} PrintSpriteInfo;

/*
 * The print routines draw through the font draw cursor (FontDrawCursor,
 * main/xgl_font.h): PrintFlush runs as an xglFontPrintExtFunc callback and
 * passes the cursor to every queued print command, which builds its GIF
 * tags and VIF direct data in the cursor's work area.
 */

/* Standard print commands: endPrintExtFunc queues func_tbl_4[command]. */
enum EndPrintCommand {
    END_PRINT_CMD_LINE,
    END_PRINT_CMD_WINDOW,
    END_PRINT_CMD_SPRITE,
    END_PRINT_CMD_RIBBON,
    END_PRINT_CMD_POINT,
    END_PRINT_CMD_SCISSOR_SET,
    END_PRINT_CMD_SCISSOR_RESET,
    END_PRINT_CMD_TAG_FONT,
    END_PRINT_CMD_SPRITE_QUAD,
    END_PRINT_CMD_FRAME,
    END_PRINT_CMD_NUMBER,
    END_PRINT_CMD_LINE_RAW,
    END_PRINT_CMD_Z_SCISSOR,
    END_PRINT_CMD_THUMBNAIL,
    END_PRINT_CMD_FONT_TEX_CHANGE,
    END_PRINT_CMD_FONT_TEX_RELOAD,
    END_PRINT_CMD_ETHER_LINE,
    END_PRINT_CMD_CIRCLE,
    END_PRINT_CMD_STANDARD_COUNT,
    END_PRINT_CMD_DRAW_BACK_SET = END_PRINT_CMD_STANDARD_COUNT,
    END_PRINT_CMD_DRAW_BACK_RESET,
    END_PRINT_CMD_SCREEN_CLEAR,
    END_PRINT_CMD_BACK_SPRITE,
    END_PRINT_CMD_BACK_SPRITE_RECT
};

/* Commands at END_PRINT_CMD_EXT_BASE + n are run at once through xglFontPrintExtFunc. */
#define END_PRINT_CMD_EXT_BASE 100
enum EndPrintExtCommand {
    END_PRINT_EXT_FLUSH,
    END_PRINT_EXT_SCISSOR_SET,
    END_PRINT_EXT_SCISSOR_RESET,
    END_PRINT_EXT_FONT_TEX_CHANGE,
    END_PRINT_EXT_FONT_TEX_RELOAD,
    END_PRINT_EXT_Z_SCISSOR
};
#define END_PRINT_EXT_CMD_LIMIT 7

/* endSpriteSet flags: which PrintSpriteInfo parts to reset from the UV table. */
#define END_SPRITE_SET_SIZE  1  /* width/height from the UV entry */
#define END_SPRITE_SET_COLOR 2  /* all four color bytes to END_PRINT_COLOR_NEUTRAL */
#define END_SPRITE_SET_CELL  4  /* column/row back to 0 */
#define END_SPRITE_SET_ALL   255
#define END_PRINT_COLOR_NEUTRAL 128

/* High byte of PrintSpriteInfo.uv: index of the UV table (and its CLUT) in end_print.c. */
enum EndPrintUvTable {
    END_PRINT_UV_CUR,
    END_PRINT_UV_OTHERS,
    END_PRINT_UV_PAD,
    END_PRINT_UV_ICOM,
    END_PRINT_UV_FACE_PUTI,
    END_PRINT_UV_TAG,
    END_PRINT_UV_WIN,
    END_PRINT_UV_NUM,
    END_PRINT_UV_PAD2,
    END_PRINT_UV_FACE_AGWS,
    END_PRINT_UV_NAME,
    END_PRINT_UV_OTHERS2,
    END_PRINT_UV_FACE00,
    END_PRINT_UV_FACE01,
    END_PRINT_UV_FACE02,
    END_PRINT_UV_FACE03,
    END_PRINT_UV_FACE04,
    END_PRINT_UV_THUMNAIL,
    END_PRINT_UV_ETHER,
    END_PRINT_UV_SEG,
    END_PRINT_UV_UMN,
    END_PRINT_UV_BTL_MSG,
    END_PRINT_UV_BTL_MSG2,
    END_PRINT_UV_BTL_MSG3,
    END_PRINT_UV_UMN2,
    END_PRINT_UV_UMN3
};

void endPrintInit(void);
void endPrintExtFunc(int order, int command, void *param);
void endPrintDirectFrameCopy(XglPacket *packet, int tagLow, int tagHigh);

#endif /* INCLUDE_MAIN_END_PRINT_H */
