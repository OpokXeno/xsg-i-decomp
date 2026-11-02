#ifndef INCLUDE_MAIN_TWSYS_INIT_H
#define INCLUDE_MAIN_TWSYS_INIT_H

#include "main/ew_sprt_set_cursor_uv.h"
#include "main/msg_init.h"
#include "main/xgl_font.h"
#include "shared.h"

/*
 * One slot of the fixed-size `tcomponent` table TWSYS_createComponent hands
 * out (still assembler in this TU). Only the header fields every component
 * kind shares, evidenced from this allocation, are modeled:
 *  - +0x10 flags: TWIN_dispose/TMENU_dispose clear bits 0x11 on disposal;
 *    TWSYS_update tests bit 0x10 before dispatching the component's update.
 *  - +0x16 kindAndGroup: low nibble selects the update/draw handler
 *    (TMENU/TSLIDER/TWIN), bits 4-5 select the `groupStatus` slot.
 *  - +0x32 closeState: cleared with the flags when a slot is (re)claimed.
 *  - +0x54 ewHandle: TWIN_dispose passes it to EW_dispose (a window-kind
 *    slot's own layout; other kinds keep their own ewHandle elsewhere).
 *  - +0x9C/+0xA0 msgBuffer: TWIN_dispose passes each non-zero entry to
 *    MBUF_dispose.
 * The 0x5B0 stride between slots is TWSYS_init/TWSYS_update/TWSYS_draw's own
 * loop increment.
 */
/*
 * The one record every slot kind shares. Its window-kind views are merged here:
 * the text window ("TWindow" in the comments, the former view of
 * src/main/twsys_init.c: the layout and line fields
 * from +0x9C on), the slider (TwinWindow2 and TwinWindow, src/main/tslider_create.h:
 * value/min/max at +0x58..+0x60, param/kind at +0x186/+0x187) and the scene
 * window header (scene_1.c's TComponent use: only class_ref at +0x00).
 * Bytes that two kinds read as different types or meanings are unions. The
 * menu kind (TMenu, src/main/twsys_init.c) keeps its own record: from +0x50
 * on its fields overlap the window kind's with other types (+0xD8 query
 * queue inside the window's widgets[], +0x100 widgets[16]).
 *
 * Window prefix shared by the close-state store and TWIN_dispose (script.c
 * reads class_ref and state, which the table comment above does not name). */
typedef struct TComponent {
    union {
        DispatchTable *class_ref;      /* +0x00 */
        int class_word;                /* +0x00: the Window natives store the header word as an int */
    };
    unsigned char unmodeled_04[0x8];
    unsigned short pixelWidth;        /* +0xc: TWindow width, TwinWindow2 width */
    unsigned short pixelHeight;        /* +0xE: TWindow height, TwinWindow2 height */
    unsigned int flags;                /* +0x10 */
    unsigned short state;              /* +0x14 */
    unsigned short kindAndGroup;       /* +0x16: TwinWindow2 mode */
    unsigned char unmodeled_18[0x20 - 0x18];
    float x;                           /* +0x20: screen location of every kind (TW_setPos, TMENU_create) */
    float y;                           /* +0x24 */
    float brightness;                  /* +0x28: TwinWindow2 */
    unsigned char unmodeled_2c[0x30 - 0x2c];
    short waitFrames;                  /* +0x30: TWindow wait count, TwinWindow2 timer */
    unsigned short closeState;         /* +0x32 */
    unsigned char unmodeled_34[0x40 - 0x34];
    int valueOffset;                   /* +0x40: TwinWindow2 */
    Vector4 *ownerPosition;            /* +0x44: TW_setPos's owner anchor */
    unsigned char unmodeled_48[0x50 - 0x48];
    union {
        unsigned short textVerticalOffset; /* +0x50: TWindow */
        unsigned int options;              /* +0x50: TwinWindow2 */
    };
    union {
        int ewHandle;                  /* +0x54: TWIN_dispose passes it to EW_dispose */
        EwContainerWidget *rootWidget; /* +0x54: TWindow */
        unsigned char max_digits;      /* +0x54: TwinWindow2 */
    };
    int value;                         /* +0x58: TwinWindow2 */
    int min_value;                     /* +0x5C: TwinWindow2 */
    int max_value;                     /* +0x60: TwinWindow2 */
    unsigned char unmodeled_64[0x9C - 0x64];
    union {
        MsgBuffer *messageBuffer[2];   /* +0x9C: TWindow */
        int *msgBuffer[2];             /* +0x9C: TWIN_dispose passes each entry to MBUF_dispose */
    };
    short currentPage;                 /* +0xA4 */
    short line_pitch;                  /* +0xA6 */
    char *textOutput;                  /* +0xA8 */
    MsgQueue queue;                    /* +0xAC */
    unsigned int frameCounter;         /* +0xCC */
    EwWidget *widgets[8];              /* +0xD0 */
    unsigned int initializationWord0;  /* +0xF0 */
    unsigned char unmodeled_f4[0x108 - 0xF4];
    unsigned int initializationWord1;  /* +0x108 */
    unsigned char unmodeled_10c[0x120 - 0x10c];
    int pendingMessage;                /* +0x120 */
    unsigned char unmodeled_124[0x14c - 0x124];
    unsigned short lineWidths[24];     /* +0x14C */
    char **lineBufferPointers;         /* +0x17C */
    char *textScratchBuffer;           /* +0x180 */
    unsigned char lineCharacterOffset; /* +0x184 */
    unsigned char currentLineIndex;    /* +0x185 */
    union {
        unsigned char layout_offset;   /* +0x186: TWindow */
        signed char param;             /* +0x186: TwinWindow, TwinWindow2 */
    };
    union {
        unsigned char line_count;      /* +0x187: TWindow */
        signed char kind;              /* +0x187: TwinWindow, TwinWindow2 */
    };
    char lineData[1];                  /* +0x188; inline pointer and character buffers follow */
    unsigned char unmodeled_189[0x18C - 0x189];
    unsigned char unmodeled_18c[0x5B0 - 0x18C];
} TComponent;

void TWSYS_draw(void);

/* Functions and data used by other translation units. */
unsigned char *STRING_int(unsigned char *destination, int value);
int WIN_checkActiveWindow(void);
void TWSYS_init(void);
int PARSE_int(const char *text, int length, int radix);
TComponent *TWIN_create2(int index);
TComponent *TWSYS_createComponent(int slotIndex, int kindAndGroup);

#endif /* INCLUDE_MAIN_TWSYS_INIT_H */
