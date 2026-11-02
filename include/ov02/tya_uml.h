#ifndef INCLUDE_OV02_TYA_UML_H
#define INCLUDE_OV02_TYA_UML_H

#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/menu_top_command_check.h"
#include "main/open_close_main.h"

/*
 * One 0x10-byte entry of the eight-slot texture table tyaUmlDispInit2
 * clears and texture_trans transfers through sceVif1PkRef: only the origin,
 * the two dimension fields, the environment pointer and the transfer count are
 * evidenced, so the rest of the record stays unmodeled.
 */
typedef struct TyaUmlImage {
    s16 x;              /* 0x00: copied from the texture table entry at +4 */
    s16 y;              /* 0x02: copied from the texture table entry at +6 */
    u16 width;          /* 0x04: masked to an even value before use */
    s16 height;         /* 0x06 */
    void *environment;  /* 0x08: cleared by tyaUmlDispInit2 */
    int count;           /* 0x0C: sceVif1PkRef's transfer count */
} TyaUmlImage;

/* The display parameters of a text screen, set by tyaUmlDispParamReset. */
typedef struct TyaUmlDispParam {
    int resource_id;
    u8 unmodeled_04[4];
    s16 render_x;
    s16 render_y;
    s16 render_width;
    s16 render_height;
    u64 color;
    s16 clip_x;
    s16 clip_y;
    s16 clip_width;
    s16 clip_height;
    int loaded_resource_id;
    int bufferUsed;
    u8 render_pending;
    u8 render_scrolling;
} TyaUmlDispParam;

/*
 * The database view tyaUmlDatabaseMain drives: it starts with the display
 * parameters it hands to tyaUmlDispMain/tyaUmlDispType3/tyaUmlDispLoad, so
 * the record embeds a TyaUmlDispParam. Only the fields the database routines
 * read or write are modeled; the rest stays unmodeled.
 */
typedef struct TyaUmlDatabase {
    TyaUmlDispParam display;   /* 0x00 */
    u8 unmodeled_30[0x10];
    WindowDX *window;      /* 0x40 */
    MenuTopMenuPanel *menu;     /* 0x44 */
    ECursolMainData *cursorA;     /* 0x48 */
    ECursolMainData *cursorB;     /* 0x4C */
    s16 state;                 /* 0x50: selects the step of tyaUmlDatabaseMain */
    s16 lastEntry;             /* 0x52: text lines read by dbheader_load, minus one */
    s16 listCursor;            /* 0x54 */
    s16 indexCursor;           /* 0x56: highlighted entry of the index bar */
    s16 foundCount;            /* 0x58: text records that had text */
    s16 fileIndex;             /* 0x5A */
    s16 topRow;                /* 0x5C */
    s16 rowsPerPage;           /* 0x5E */
} TyaUmlDatabase;

/* Functions and data used by other translation units. */
extern TyaUmlImage image[8];
int tyaUmlDispMain(TyaUmlDispParam *parameter);
int tyaUmlDatabaseMain(TyaUmlDatabase *database);
int tyaUmlDispParamReset(TyaUmlDispParam *parameter, int mode);
void tyaUmlDispInit2(u8 *work_buffer);
void tyaUmlDispInit(void);

#endif /* INCLUDE_OV02_TYA_UML_H */
