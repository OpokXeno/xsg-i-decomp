#ifndef INCLUDE_MAIN_XGL_MENU_H
#define INCLUDE_MAIN_XGL_MENU_H

typedef struct XglMenuNode XglMenuNode;

/*
 * The row's link word is either the child menu node pointer or, for a row that
 * ends the menu, a plain result value; xgl_menu.c reads it through both views.
 */
typedef union XglMenuRowLink {
    XglMenuNode *node;
    int result;
} XglMenuRowLink;

typedef struct XglMenuRow {
    const char *text;
    XglMenuRowLink next;
} XglMenuRow;

struct XglMenuNode {
    unsigned char item_count;
    struct { signed char initial_selected_item; unsigned char flags; unsigned char row_spacing; } initial;
    signed char selected_item;
    unsigned char unmodeled_05[3];
    int enter_result;
    int cancel_result;
    unsigned char unmodeled_10[8];
    XglMenuRow *rows;
};

typedef struct XglMenuListData {
    short position_x;
    short position_y;
    short position_z;
    unsigned char flags;
    unsigned char unmodeled_07;
    unsigned int current_index;
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
    } payload;
    short sprite_row_marks[18];
} XglMenuEntry;

/* Functions and data used by other translation units. */
extern const char darkRedTextFormat[];
extern const char dimTextFormat[];
extern const char plainTextFormat[];
void xglMenuInitial(void);

#endif /* INCLUDE_MAIN_XGL_MENU_H */
