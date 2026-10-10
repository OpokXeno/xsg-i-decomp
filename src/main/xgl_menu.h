/*
 * TU-local declarations of main/tu091 (src/main/xgl_menu.c).
 */

#ifndef SRC_MAIN_XGL_MENU_H
#define SRC_MAIN_XGL_MENU_H

#include "shared.h"

typedef struct XglMenuNode XglMenuNode;

typedef struct XglMenuRow {
    const char *text;
    XglMenuNode *next;
} XglMenuRow;

struct XglMenuNode {
    unsigned char item_count;
    unsigned char unmodeled_01[3];
    signed char selected_item;
    unsigned char unmodeled_05[19];
    XglMenuRow *rows;
};

typedef struct XglMenuListData {
    short position_x;
    short position_y;
    short position_z;
    unsigned char flags;
    unsigned char unmodeled_07;
    int current_index;
    XglMenuNode *items[8];
    short row_offsets[8];
} XglMenuListData;

typedef struct XglMenuDrawContext {
    unsigned char unmodeled_00[4];
    int x;
    int y;
} XglMenuDrawContext;

typedef struct XglMenuSpriteData {
    unsigned char alpha;
    unsigned char flags;
    unsigned char unmodeled_02[14];
    float x;
    float y;
    float z;
    float unit_params[5];
    short offsets[4];
    unsigned char unmodeled_38[4];
} XglMenuSpriteData;

typedef struct XglMenuParams {
    unsigned char type;
    unsigned char select_flags;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    short z;
    unsigned char unmodeled_0a[2];
    int default_result;
    XglMenuNode *root;
    int *result_out;
} XglMenuParams;

#define EE_SCRATCHPAD_BASE ((XglMenuDrawContext *)0x70000000)

#define MENU_LIST_INDENT 8

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern const char D_004DC2B8[];
extern const char D_004DC2C0[];
extern const char D_004DC2C8[];

/*
 * Partial view of one entry of the menutbl array (main 0x0021ce18/0x0021ce7c,
 * a 0x80-byte stride): xglMenuDrawType0 only touches the active flag at
 * offset 7, which xglMenuDraw (main 0x0021ce30) reads first and skips the
 * entry when it is zero.
 */
typedef struct {
    XglMenuNode *root;                  /* +0x00 */
    unsigned char type;                 /* +0x04 */
    unsigned char select_flags;         /* +0x05 */
    unsigned char unmodeled_06;
    unsigned char active;               /* +0x07 */
    /*
     * xglMenuInitial (main 0x0021ce98) indexes menutbl by this stride to
     * clear the active flag of every record; the rest of the record is not
     * recovered.
     */
    int default_result;
    int result_value;
    int *result_out;
    unsigned char unmodeled_14[0x0c];
    union {
        XglMenuListData list;
        XglMenuSpriteData sprite;
    };
    short sprite_row_marks[18];
} XglMenuEntry;

#define MENU_TABLE_COUNT 16

extern XglMenuEntry menutbl[MENU_TABLE_COUNT];

#endif
